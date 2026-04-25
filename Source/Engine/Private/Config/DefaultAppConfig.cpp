#include "Config/DefaultAppConfig.h"

namespace engine
{
	bool DefaultApp::IsEditor()
	{
#ifdef WITH_EDITOR
		return GetInstance().m_editor;
#else
		return false;
#endif
	}

	std::pair<int32, int32> DefaultApp::GetClientSize()
	{
		return { GetInstance().m_clientWidth, GetInstance().m_clientHeight };
	}

	void DefaultApp::SetClientSize( uint32 width, uint32 height )
	{
		GetInstance().m_clientWidth = width;
		GetInstance().m_clientHeight = height;
	}
}
