#pragma once

namespace agl
{
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
			if ( m_reference )
			{
				m_reference->AddRef();
			}
		}

		~RefHandle()
		{
			if ( m_reference )
			{
				m_reference->ReleaseRef();
			}

			m_reference = nullptr;
		}

		RefHandle( const RefHandle& other )
		{
			*this = other;
		}

		RefHandle& operator=( const RefHandle& other )
		{
			if ( this != &other )
			{
				if ( m_reference )
				{
					m_reference->ReleaseRef();
				}

				m_reference = other.m_reference;
				if ( m_reference )
				{
					m_reference->AddRef();
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
					m_reference->ReleaseRef();
				}

				m_reference = other.m_reference;
				other.m_reference = nullptr;
			}

			return *this;
		}

		operator ReferencedType* ( ) const
		{
			return m_reference;
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
		ReferencedType* m_reference = nullptr;
	};
}
