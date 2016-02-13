#pragma once

class IListener
{
public:
	//Mouse
	virtual void OnLButtonDown( const int, const int ) {}
	virtual void OnLButtonUp( const int, const int ) {}
	virtual void OnRButtonDown( const int, const int ) {}
	virtual void OnRButtonUp( const int, const int ) {}
	virtual void OnMButtonDown( const int, const int ) {}
	virtual void OnMButtonUp( const int, const int ) {}
	virtual void OnMouseMove( const int, const int ) {}
	virtual void OnWheelMove( const int ) {}

	//KeyBoard
protected:
	IListener( ) {}
};