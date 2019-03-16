#pragma once

#include <utility>

class IPlatform
{
public:
	virtual std::pair<UINT, UINT> GetSize( ) const noexcept = 0;
	virtual void UpdateSize( UINT width, UINT height ) = 0;
	
	template <typename RT>
	RT GetRawHandle( )
	{
		return static_cast<RT>( GetRawHandleImple() );
	}

	virtual ~IPlatform( ) = default;

private:
	virtual void* GetRawHandleImple( ) const noexcept = 0;
};