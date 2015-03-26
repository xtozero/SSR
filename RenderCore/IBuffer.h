#pragma once

class IBufferTrait
{
protected:
	IBufferTrait () {}

public:
	virtual ~IBufferTrait () {}
};

class IBuffer
{
public:
	virtual bool CreateBuffer ( const IBufferTrait& trait ) = 0;

protected:
	IBuffer () {}

public:
	virtual ~IBuffer () {}
};