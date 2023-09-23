#include "Config/DefaultAppConfig.h"

namespace engine
{
	bool DefaultApp::IsEditor()
	{
		return GetInstance().m_editor;
	}

	std::pair<int32, int32> DefaultApp::GetClientSize()
	{
		return { GetInstance().m_clientWidth, GetInstance().m_clientHeight };
	}
}
