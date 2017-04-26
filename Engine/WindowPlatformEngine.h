#pragma once

#include "IEngine.h"
#include "WindowPlatformInputConvertor.h"

#include <memory>

class ILogic;

class WindowPlatformEngine : public IEngine
{
public:
	virtual bool BootUp( IPlatform& ) override;
	virtual void ShutDown( ) override;

	virtual void Run( ) override;

protected:
	virtual void ProcessInput( ) override;
	virtual bool IsAvailable( ) override { return m_isAvailable; }

private:
	WindowPlatformInputConvertor m_inputConvertor;
	std::unique_ptr<ILogic> m_logic = nullptr;

	bool m_isAvailable = false;
};

