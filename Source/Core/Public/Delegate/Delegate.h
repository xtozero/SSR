#pragma once

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <type_traits>
#include <vector>

struct IDelegateInstance
{
	virtual ~IDelegateInstance( ) {}
};

class DelegateStorage
{
public:
	void* GetRaw( ) const
	{
		return m_storage;
	}

	void* Allocate( std::size_t size )
	{
		if ( IDelegateInstance* pInstance = static_cast<IDelegateInstance*>( m_storage ) )
		{
			pInstance->~IDelegateInstance( );
		}

		if ( m_size < size )
		{
			std::free( m_storage );
			m_storage = std::malloc( size );
			m_size = size;
		}

		return m_storage;
	}

	void Unbind( )
	{
		if ( IDelegateInstance* pInstance = static_cast<IDelegateInstance*>( m_storage ) )
		{
			pInstance->~IDelegateInstance( );
			std::free( m_storage );
			m_storage = nullptr;
			m_size = 0;
		}
	}

	DelegateStorage( ) = default;
	~DelegateStorage( ) = default;
	DelegateStorage( const DelegateStorage& ) = delete;
	DelegateStorage& operator=( const DelegateStorage& ) = delete;
	DelegateStorage( DelegateStorage&& other ) noexcept
	{
		*this = std::move( other );
	}

	DelegateStorage& operator=( DelegateStorage&& other ) noexcept
	{
		if ( &other != this )
		{
			m_storage = other.m_storage;
			m_size = other.m_size;

			other.m_storage = nullptr;
			other.m_size = 0;
		}

		return *this;
	}

private:
	void* m_storage = nullptr;
	std::size_t m_size = 0;
};

inline void* operator new( std::size_t count, DelegateStorage& storage )
{
	return storage.Allocate( count );
}

inline void operator delete( void* pMem, [[maybe_unused]] DelegateStorage& storage )
{
	std::free( pMem );
}

class DelegateHandle
{
public:
	struct GenerateNewHandle {};

	DelegateHandle( ) : m_id( 0 ) { }
	DelegateHandle( GenerateNewHandle ) : m_id( GenerateNewID() ) { }

	friend bool operator==( const DelegateHandle& lhs, const DelegateHandle& rhs )
	{
		return lhs.m_id == rhs.m_id;
	}

	friend bool operator!=( const DelegateHandle& lhs, const DelegateHandle& rhs )
	{
		return !( lhs == rhs );
	}

private:
	static int GenerateNewID( )
	{
		int newID = ++m_nextID;

		if ( newID == 0 )
		{
			newID = ++m_nextID;
		}

		return newID;
	}

	int m_id = 0;
	inline static std::atomic<int> m_nextID = 1;
};

template <typename RetType, typename... ArgTypes>
struct IDelegateInterface : public IDelegateInstance
{
	virtual void Clone( DelegateStorage& storage ) const = 0;
	virtual RetType Execute( ArgTypes... ) const = 0;
	virtual DelegateHandle GetHandle( ) const = 0;
};

template <typename RetType, typename... ArgTypes>
class FunctionDelegate : public IDelegateInterface<RetType, ArgTypes...>
{
	using FunctionPointer = RetType (*)( ArgTypes... );

public:
	virtual void Clone( DelegateStorage& storage ) const override final
	{
		new ( storage )FunctionDelegate( *this );
	}

	virtual RetType Execute( ArgTypes... args ) const override final
	{
		return m_func( std::forward<ArgTypes>( args )... );
	}

	virtual DelegateHandle GetHandle( ) const override final
	{
		return m_handle;
	}

	static void Create( DelegateStorage& storage, const FunctionPointer func )
	{
		new ( storage )FunctionDelegate( func );
	}

	FunctionDelegate( const FunctionPointer func ) : m_func( func ), m_handle( DelegateHandle::GenerateNewHandle{} ) {}

private:
	FunctionPointer m_func;
	DelegateHandle m_handle;
};

template <bool isConst, typename ClassType, typename RetType, typename... ArgTypes>
class MemberFunctionDelegate : public IDelegateInterface<RetType, ArgTypes...>
{
	using MemberFunctionPointer = typename std::conditional<isConst
		, RetType ( ClassType::* )( ArgTypes... ) const
		, RetType ( ClassType::* )( ArgTypes... )>::type;

public:
	virtual void Clone( DelegateStorage& storage ) const override final
	{
		new ( storage )MemberFunctionDelegate( *this );
	}

	virtual RetType Execute( ArgTypes... args ) const override final
	{
		return ( m_instance->*m_func )( std::forward<ArgTypes>( args )... );
	}

	virtual DelegateHandle GetHandle( ) const override final
	{
		return m_handle;
	}

	static void Create( DelegateStorage& storage, ClassType* instance, const MemberFunctionPointer func )
	{
		new ( storage )MemberFunctionDelegate( instance, func );
	}

	MemberFunctionDelegate( ClassType* instance, const MemberFunctionPointer func ) : m_instance( instance ), m_func( func ), m_handle( DelegateHandle::GenerateNewHandle{} ) {}

private:
	ClassType* m_instance;
	MemberFunctionPointer m_func;
	DelegateHandle m_handle;
};

template <typename ClassType, typename RetType, typename... ArgTypes>
MemberFunctionDelegate( ClassType* instance, RetType ( ClassType::* )( ArgTypes... ) const )->MemberFunctionDelegate<true, ClassType, RetType, ArgTypes...>;

template <typename ClassType, typename RetType, typename... ArgTypes>
MemberFunctionDelegate( ClassType* instance, RetType ( ClassType::* )( ArgTypes... ) )->MemberFunctionDelegate<false, ClassType, RetType, ArgTypes...>;

template <typename FunctorType, typename... ArgTypes>
class FunctorDelegate : public IDelegateInterface<typename std::invoke_result<FunctorType, ArgTypes...>::type, ArgTypes...>
{
	using RetType = typename std::invoke_result<FunctorType, ArgTypes...>::type;

public:
	virtual void Clone( DelegateStorage& storage ) const override final
	{
		new ( storage )FunctorDelegate( *this );
	}

	virtual RetType Execute( ArgTypes... args ) const override final
	{
		return m_func( std::forward<ArgTypes>( args )... );
	}

	virtual DelegateHandle GetHandle( ) const override final
	{
		return m_handle;
	}

	static void Create( DelegateStorage& storage, const FunctorType& func )
	{
		new ( storage )FunctorDelegate( func );
	}

	static void Create( DelegateStorage& storage, FunctorType&& func )
	{
		new ( storage )FunctorDelegate( std::move( func ) );
	}

	FunctorDelegate( const FunctorType& func ) : m_func( func ), m_handle( DelegateHandle::GenerateNewHandle{} ) {}
	FunctorDelegate( FunctorType&& func ) : m_func( std::move( func ) ), m_handle( DelegateHandle::GenerateNewHandle{} ) {}

private:
	FunctorType m_func;
	DelegateHandle m_handle;
};

template <typename RetType, typename... ArgTypes>
class Delegate
{
	using DelegateInterface = IDelegateInterface<RetType, ArgTypes...>;

public:
	void Unbind( )
	{
		m_storage.Unbind( );
	}

	bool IsBound( ) const
	{
		return m_storage.GetRaw( ) != nullptr;
	}
	
	DelegateHandle GetHandle( ) const
	{
		DelegateHandle handle;

		if ( DelegateInterface* pInstance = static_cast<DelegateInterface*>( m_storage.GetRaw( ) ) )
		{
			handle = pInstance->GetHandle( );
		}

		return handle;
	}

	using FunctionPointer = RetType (*)( ArgTypes... );
	DelegateHandle BindFunction( FunctionPointer func )
	{
		FunctionDelegate<RetType, ArgTypes...>::Create( m_storage, func );
		return GetHandle( );
	}

	template <typename ClassType>
	DelegateHandle BindMemberFunction( ClassType* instance, RetType ( ClassType::*func )( ArgTypes... ) )
	{
		MemberFunctionDelegate<false, ClassType, RetType, ArgTypes...>::Create( m_storage, instance, func );
		return GetHandle( );
	}

	template <typename ClassType>
	DelegateHandle BindMemberFunction( ClassType* instance, RetType ( ClassType::*func )( ArgTypes... ) const )
	{
		MemberFunctionDelegate<true, ClassType, RetType, ArgTypes...>::Create( m_storage, instance, func );
		return GetHandle( );
	}

	template <typename FunctorType>
	DelegateHandle BindFunctor( FunctorType&& func )
	{
		using UnwarpFunctorType = std::remove_cv_t<std::remove_reference_t<FunctorType>>;
		FunctorDelegate<UnwarpFunctorType, ArgTypes...>::Create( m_storage, std::forward<FunctorType>( func ) );
		return GetHandle( );
	}

	RetType Execute( ArgTypes... args ) const
	{
		DelegateInterface* pInstance = static_cast<DelegateInterface*>( m_storage.GetRaw( ) );
		
		assert( pInstance != nullptr );

		return pInstance->Execute( std::forward<ArgTypes>( args )... );
	}

	RetType operator()( ArgTypes... args ) const
	{
		return Execute( std::forward<ArgTypes>( args )... );
	}

	Delegate( ) = default;

	~Delegate( )
	{
		Unbind( );
	}

	Delegate( const Delegate& other )
	{
		( *this ) = other;
	}

	Delegate& operator=( const Delegate& other )
	{
		if ( &other != this )
		{
			DelegateInterface* pInstance = static_cast<DelegateInterface*>( other.m_storage.GetRaw( ) );

			if ( pInstance )
			{
				pInstance->Clone( m_storage );
			}
			else
			{
				Unbind( );
			}
		}

		return *this;
	}

	Delegate( Delegate&& other )
	{
		( *this ) = std::move( other );
	}

	Delegate& operator=( Delegate&& other )
	{
		if ( &other != this )
		{
			m_storage = std::move( other.m_storage );
		}

		return *this;
	}

private:
	DelegateStorage m_storage;
};

template <typename... ArgTypes>
class MulticastDelegate
{
	using DelegateType = Delegate<void, ArgTypes...>;

public:
	void RemoveAll( )
	{
		m_invocationList.clear( );
	}

	using FunctionPointer = void (*)( ArgTypes... );
	DelegateHandle AddFunction( FunctionPointer func )
	{
		return m_invocationList.emplace_back( ).BindFunction( func );
	}

	template <typename ClassType>
	DelegateHandle AddMemberFunction( ClassType* instance, void ( ClassType::*func )( ArgTypes... ) )
	{
		return m_invocationList.emplace_back( ).BindMemberFunction( instance, func );
	}

	template <typename ClassType>
	DelegateHandle AddMemberFunction( ClassType* instance, void ( ClassType::*func )( ArgTypes... ) const )
	{
		return m_invocationList.emplace_back( ).BindMemberFunction( instance, func );
	}

	template <typename FunctorType>
	DelegateHandle AddFunctor( FunctorType&& func )
	{
		return m_invocationList.emplace_back( ).BindFunctor( std::forward<FunctorType>( func ) );
	}

	bool Remove( DelegateHandle handle )
	{
		std::size_t oldSize = m_invocationList.size( );

		m_invocationList.erase( std::remove_if( m_invocationList.begin( ),
								m_invocationList.end( ), 
								[handle]( const DelegateType& delegate ) 
								{ 
									return handle == delegate.GetHandle( ); 
								} ), 
								m_invocationList.end() );

		return oldSize != m_invocationList.size( );;
	}

	void Boardcast( ArgTypes... args )
	{
		for ( auto& invocation : m_invocationList )
		{
			if ( invocation.IsBound( ) )
			{
				invocation.Execute( std::forward<ArgTypes>( args )... );
			}
		}
	}

	void operator()( ArgTypes... args )
	{
		Boardcast( std::forward<ArgTypes>( args )... );
	}

private:
	std::vector<DelegateType> m_invocationList;
};