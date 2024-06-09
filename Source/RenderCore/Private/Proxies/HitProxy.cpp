#include "HitProxy.h"

#include "SparseArray.h"

namespace rendercore
{
	class HitProxyArray
	{
	public:
		static HitProxyArray& GetInstance()
		{
			static HitProxyArray hitProxyArray;
			return hitProxyArray;
		}

		uint32 Add( HitProxy* hitProxy )
		{
			return static_cast<uint32>( m_hitProxies.Add( hitProxy ) );
		}

		void Remove( uint32 index )
		{
			m_hitProxies.RemoveAt( index );
		}

		HitProxy* GetHitProxy( uint32 index )
		{
			if ( index < m_hitProxies.Size() && m_hitProxies.IsAllocated( index ) )
			{
				return m_hitProxies[index];
			}
			else
			{
				return nullptr;
			}
		}

	private:
		SparseArray<HitProxy*> m_hitProxies;
	};

	uint32 HitProxyId::GetId() const
	{
		return m_id;
	}

	Color HitProxyId::GetColor() const
	{
		uint8 r = m_id & 0xFF;
		uint8 g = ( m_id >> 8 ) & 0xFF;
		uint8 b = ( m_id >> 16 ) & 0xFF;

		return Color( r, g, b, 0 );
	}

	HitProxyId::HitProxyId( uint32 id )
		: m_id( id )
	{
	}

	HitProxyId::HitProxyId( Color color )
		: m_id( color.R() | color.G() << 8 | color.B() << 16 )
	{
	}

	HitProxyId HitProxy::GetId() const
	{
		return m_id;
	}

	HitProxy::HitProxy()
	{
		InitHitProxy();
	}

	HitProxy::~HitProxy()
	{
		HitProxyArray::GetInstance().Remove( m_id.GetId() );
	}

	void HitProxy::InitHitProxy()
	{
		m_id = HitProxyId( HitProxyArray::GetInstance().Add( this ) );
	}

	HitProxy* GetHitProxyById( HitProxyId id )
	{
		return HitProxyArray::GetInstance().GetHitProxy( id.GetId() );
	}
}
