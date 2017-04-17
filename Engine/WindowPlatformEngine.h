#pragma once

#include <memory>

#include "IEngine.h"

class ILogic;

class WindowPlatformEngine : public IEngine
{
public:
	virtual bool BootUp( IPlatform& ) override;
	virtual void ShutDown( ) override;

	virtual void ProcessInput( ) override;

	virtual void Run( ) override;

private:
	std::unique_ptr<ILogic> m_logic = nullptr;
};

