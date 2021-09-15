#pragma once

#include <SizedTypes.h>
#include <utility>

class IPlatform
{
public:
	virtual std::pair<uint32, uint32> GetSize( ) const noexcept = 0;
	virtual void UpdateSize( uint32 width, uint32 height ) = 0;
	
	template <typename RT>
	RT GetRawHandle( )
	{
		return static_cast<RT>( GetRawHandleImple() );
	}

	virtual ~IPlatform( ) = default;

private:
	virtual void* GetRawHandleImple( ) const noexcept = 0;
};