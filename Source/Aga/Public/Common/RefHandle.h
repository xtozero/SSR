#pragma once

template <typename ReferencedType>
class RefHandle
{
public:
	ReferencedType* Get( ) const
	{
		return m_reference;
	}

	RefHandle( ) = default;
	RefHandle( ReferencedType* reference ) : m_reference( reference )
	{
		if ( m_reference )
		{
			m_reference->AddRef( );
		}
	}

	~RefHandle( )
	{
		if ( m_reference )
		{
			m_reference->ReleaseRef( );
		}
	}

	RefHandle( const RefHandle& ref )
	{
		*this = ref;
	}

	RefHandle& operator=( const RefHandle& other )
	{
		if ( this != &other )
		{
			m_reference = other.m_reference;
			if ( m_reference )
			{
				m_reference->AddRef( );
			}
		}

		return *this;
	}

	RefHandle( RefHandle&& other )
	{
		*this = std::move( other );
	}

	RefHandle& operator=( RefHandle&& other )
	{
		if ( this != &other )
		{
			if ( m_reference )
			{
				m_reference->ReleaseRef( );
			}

			m_reference = other.m_reference;
			other.m_reference = nullptr;
		}

		return *this;
	}

	ReferencedType* operator->( ) const
	{
		return m_reference;
	}

	friend bool operator==( const RefHandle& lhs, ReferencedType* rhs )
	{
		return lhs.m_reference == rhs;
	}

	friend bool operator==( const RefHandle& lhs, const RefHandle& rhs )
	{
		return lhs.m_reference == rhs.m_reference;
	}

private:
	ReferencedType* m_reference = nullptr;
};