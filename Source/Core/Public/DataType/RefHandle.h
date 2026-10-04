#pragma once

template <typename ReferencedType>
class RefHandle final
{
public:
	ReferencedType* Get() const
	{
		return m_reference;
	}

	RefHandle() = default;
	RefHandle( ReferencedType* reference ) : m_reference( reference )
	{
		AddRef();
	}

	~RefHandle()
	{
		ReleaseRef();
		m_reference = nullptr;
	}

	RefHandle( const RefHandle& other ) noexcept
	{
		*this = other;
	}

	RefHandle& operator=( const RefHandle& other ) noexcept
	{
		if ( this != &other )
		{
			ReleaseRef();
			m_reference = other.m_reference;
			AddRef();
		}

		return *this;
	}

	RefHandle( RefHandle&& other ) noexcept
	{
		*this = std::move( other );
	}

	RefHandle& operator=( RefHandle&& other ) noexcept
	{
		if ( this != &other )
		{
			ReleaseRef();
			m_reference = other.m_reference;
			other.m_reference = nullptr;
		}

		return *this;
	}

	ReferencedType* operator->() const
	{
		return m_reference;
	}

	friend bool operator==( const RefHandle& lhs, const ReferencedType* rhs )
	{
		return lhs.m_reference == rhs;
	}

	friend bool operator==( const ReferencedType* lhs, const RefHandle& rhs )
	{
		return lhs == rhs.m_reference;
	}

	friend bool operator==( const RefHandle& lhs, const RefHandle& rhs )
	{
		return lhs.m_reference == rhs.m_reference;
	}

	friend bool operator<( const RefHandle& lhs, const RefHandle& rhs )
	{
		return lhs.m_reference < rhs.m_reference;
	}

private:
	void AddRef()
	{
		if ( m_reference )
		{
			const_cast<std::remove_const_t<ReferencedType>*>(m_reference)->AddRef();
		}
	}

	void ReleaseRef()
	{
		if ( m_reference )
		{
			const_cast<std::remove_const_t<ReferencedType>*>(m_reference)->ReleaseRef();
		}
	}

	ReferencedType* m_reference = nullptr;
};

template <typename To, typename From>
RefHandle<To> RefStaticCast( const RefHandle<From>& from )
{
	return RefHandle<To>( static_cast<To*>( from.Get() ) );
}