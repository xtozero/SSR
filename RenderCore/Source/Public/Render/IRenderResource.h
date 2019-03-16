#pragma once

#include "common.h"
#include "Resource.h"

#include <array>

class IRenderResource
{
public:
	void SetAppSizeDependency( bool isDepentant ) { m_isAppSizeDependent = isDepentant; }
	bool IsAppSizeDependency() const { return m_isAppSizeDependent; }

	const String& GetName( ) const { return m_name; }
	void SetName( const String& name ) { m_name = name; }

	virtual ~IRenderResource( ) = default;

protected:
	IRenderResource( ) = default;

private:
	bool m_isAppSizeDependent = false;
	String m_name;
};

enum class TEXTURE_TYPE
{
	TEXTURE_NONE = -1,
	TEXTURE_1D,
	TEXTURE_2D,
	TEXTURE_3D
};

class ITexture : public IRenderResource
{
public:
	virtual TEXTURE_TYPE GetType( ) const = 0;
	const TEXTURE_TRAIT& GetTrait( ) const { return m_trait; }

	virtual ~ITexture( ) = default;

protected:
	TEXTURE_TRAIT m_trait;
};
