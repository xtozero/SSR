#pragma once

struct UserInput;

class IListener
{
public:
	virtual void ProcessInput( const UserInput& ) = 0;

	virtual ~IListener( ) = default;

protected:
	IListener( ) = default;
};