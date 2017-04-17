#pragma once

#include <utility>

class IPlatform
{
public:
	virtual std::pair<UINT, UINT> GetSize( ) const noexcept = 0;
	
	template <typename RT>
	RT GetRawHandle( )
	{
		return static_cast<RT>( GetRawHandleImple() );
	}

private:
	virtual void* GetRawHandleImple( ) const noexcept = 0;
};