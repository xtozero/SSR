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

	void DefaultApp::SetClientSize( const std::pair<int32, int32>& clientSize )
	{
		GetInstance().m_clientWidth = clientSize.first;
		GetInstance().m_clientHeight = clientSize.second;
	}
}
