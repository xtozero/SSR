#pragma once

class IListener
{
public:
	//Mouse
	virtual void OnLButtonDown( const int x, const int y ) {}
	virtual void OnLButtonUp( const int x, const int y ) {}
	virtual void OnRButtonDown( const int x, const int y ) {}
	virtual void OnRButtonUp( const int x, const int y ) {}
	virtual void OnMButtonDown( const int x, const int y ) {}
	virtual void OnMButtonUp( const int x, const int y ) {}
	virtual void OnMouseMove( const int x, const int y ) {}
	virtual void OnWheelMove( const int zDelta ) {}

	//KeyBoard
protected:
	IListener( ) {}
};