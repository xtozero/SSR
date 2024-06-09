#pragma once

#include "ColorTypes.h"
#include "RefCounterType.h"
#include "SizedTypes.h"
#include "Reflection.h"

namespace rendercore
{
	class HitProxyId
	{
	public:
		uint32 GetId() const;
		Color GetColor() const;

		HitProxyId() = default;
		explicit HitProxyId( uint32 id );
		RENDERCORE_DLL explicit HitProxyId( Color color );

		std::strong_ordering operator<=>( const HitProxyId& other ) const = default;

	private:
		uint32 m_id;
	};

	class HitProxy : public RefCounter
	{
		GENERATE_CLASS_TYPE_INFO( HitProxy )

	public:
		HitProxyId GetId() const;

		HitProxy();
		~HitProxy();

	private:
		void InitHitProxy();

		HitProxyId m_id;
	};

	RENDERCORE_DLL HitProxy* GetHitProxyById( HitProxyId id );
}
