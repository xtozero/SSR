#pragma once

#include "NameTypes.h"
#include "SizedTypes.h"
#include "RefCounterType.h"
#include "RefHandle.h"

#include <type_traits>
#include <vector>

namespace rendercore
{
	enum class ShaderArgumentsMemberType: uint8
	{
		ShaderResource = 0,
		ShaderValue,
	};

	struct ShaderArgumentsMemberMetaData
	{
		ShaderArgumentsMemberType m_type;
		Name m_name;
		size_t m_offset = 0;
		int32 m_size = 0;
		ShaderArgumentsMemberMetaData* m_next = nullptr;
	};

	class ShaderArgumentsMetaData
	{
	public:
		struct ShaderValueLayout
		{
			ShaderValueLayout( const ShaderArgumentsMemberMetaData* memberMetaData, int32 offsetFromBufferStart )
				: m_memberMetaData( memberMetaData )
				, m_offsetFromBufferStart( offsetFromBufferStart )
			{}
			ShaderValueLayout() = default;

			const ShaderArgumentsMemberMetaData* m_memberMetaData = nullptr;
			int32 m_offsetFromBufferStart = 0;
		};

		struct ShaderResourceLayout
		{
			ShaderResourceLayout( const ShaderArgumentsMemberMetaData* memberMetaData )
				: m_memberMetaData( memberMetaData )
			{}
			ShaderResourceLayout() = default;

			const ShaderArgumentsMemberMetaData* m_memberMetaData = nullptr;
		};

		Name GetName() const;

		uint32 GetConstantBufferSize() const;
		const std::vector<ShaderValueLayout>& GetShaderValueLayouts() const;

		const std::vector<ShaderResourceLayout>& GetShaderResourceLayouts() const;

		ShaderArgumentsMetaData( Name name, const ShaderArgumentsMemberMetaData* firstMemberMetaData );
		ShaderArgumentsMetaData() = default;

	private:
		Name m_name;

		uint32 m_constantBufferSize = 0;
		std::vector<ShaderValueLayout> m_shaderValueLayouts;

		std::vector<ShaderResourceLayout> m_shaderResourceLayouts;
	};

	class ShaderArguments : public RefCounter
	{
	public:
		void Bind( const agl::ShaderParameterMap& parameterMap, agl::SingleShaderBindings& singleBinding ) const;

		agl::Buffer* Resource();
		const agl::Buffer* Resource() const;

		template <typename T>
		void Update( T& shaderArgumentsStruct )
		{
			assert( &m_metaData == &T::GetMetaData() );
			Update( &shaderArgumentsStruct );
		}

		template <typename T>
		explicit ShaderArguments( T& shaderArgumentsStruct )
			: m_metaData( T::GetMetaData() )
		{
			InitResource( &shaderArgumentsStruct );
		}

		explicit ShaderArguments( const ShaderArgumentsMetaData& metaData );

	private:
		virtual void Finalizer() override;

		void InitResource( void* contents );
		void Update( void* contents );

		void UpdateShaderValue( uint8* dest, void* contents );
		void UpdateShaderResouce( void* contents );

		const ShaderArgumentsMetaData& m_metaData;

		RefHandle<agl::Buffer> m_buffer;
		std::vector<RefHandle<agl::GraphicsApiResource>> m_resources;
	};

#define BEGIN_SHADER_ARGUMENTS_STRUCT( type ) \
class type \
{ \
public: \
\
	static const ShaderArgumentsMetaData& GetMetaData() \
	{ \
		static ShaderArgumentsMetaData metaData( \
			Name(#type), \
			type::GetMemberMetaData() ); \
		return metaData; \
	} \
\
	static RefHandle<ShaderArguments> CreateShaderArguments() \
	{ \
		return new ShaderArguments( type::GetMetaData() ); \
	} \
private: \
	struct FirstMemberId {}; \
	static void* GetMetaData( FirstMemberId, [[maybe_unused]] ShaderArgumentsMemberMetaData*& next ) \
	{ \
		return nullptr; \
	} \
	using ThisType = type; \
	using MemberFunc = void* (*)(FirstMemberId, ShaderArgumentsMemberMetaData*&); \
\
	typedef FirstMemberId

#define DECALRE_GET_MEATADATA( name, memberType ) \
	struct MemberId##name{}; \
	static void* GetMetaData( MemberId##name, ShaderArgumentsMemberMetaData*& next ) \
	{ \
		static ShaderArgumentsMemberMetaData metaData##name = { \
			.m_type = ShaderArgumentsMemberType::memberType, \
			.m_name = Name( #name ), \
			.m_offset = offsetof( ThisType, name ), \
			.m_size = sizeof( name ), \
			.m_next = next \
		}; \
\
		next = &metaData##name; \
		void* ( *prevFunc )( PrevMemberId##name, ShaderArgumentsMemberMetaData*& ); \
		prevFunc = &GetMetaData; \
		return (void*)prevFunc; \
	} \
\
	typedef MemberId##name \

#define DECLARE_RESOURCE_GET_METADATA( name ) DECALRE_GET_MEATADATA( name, ShaderResource )
#define DECLARE_VALUE_GET_METADATA( name ) DECALRE_GET_MEATADATA( name, ShaderValue )

#define DECLARE_RESOURCE( type, name ) \
	PrevMemberId##name; \
public: \
	static_assert( std::is_base_of_v<agl::Buffer, type> == false && std::is_base_of_v<agl::Texture, type> == false ); \
	RefHandle<type> name; \
\
private: \
	DECLARE_RESOURCE_GET_METADATA( name )

#define DECLARE_VALUE( type, name ) \
	PrevMemberId##name; \
public: \
	static_assert( ( sizeof(type) % 4 == 0 ) ); \
	type name = {}; \
\
private:\
	DECLARE_VALUE_GET_METADATA( name )

#define DECLARE_ARRAY_VALUE( type, name, count ) \
	PrevMemberId##name; \
public: \
	static_assert( ( sizeof(type) % 4 == 0 ) && ( count > 1 ) ); \
	type name[count] = {}; \
\
private:\
	DECLARE_VALUE_GET_METADATA( name )

#define END_SHADER_ARGUMENTS_STRUCT() \
	LastMemberId; \
\
	static ShaderArgumentsMemberMetaData* GetMemberMetaData()\
	{ \
		void* (*lastFunc)(LastMemberId, ShaderArgumentsMemberMetaData*&); \
		lastFunc = &GetMetaData; \
		ShaderArgumentsMemberMetaData* next = nullptr; \
		void* ptr = (void*)lastFunc; \
		do \
		{ \
			ptr = reinterpret_cast<MemberFunc>( ptr )( FirstMemberId(), next ); \
		} while( ptr ); \
		return next; \
	} \
};
}
