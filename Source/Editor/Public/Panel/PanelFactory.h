#pragma once

#include <memory>
#include <vector>

namespace editor
{
	class IPanel;

	using PanelCreateFunc = IPanel*( * )( );

	template <typename T>
	IPanel* NewPanel()
	{
		return new T();
	}

	class PanelFactory final
	{
	public:
		static PanelFactory& GetInstance()
		{
			static PanelFactory panelFactory;
			return panelFactory;
		}

		template <typename T>
		bool RegisterPanel()
		{
			m_createfunctions.emplace_back( &NewPanel<T> );
			return true;
		}

		std::vector<std::unique_ptr<IPanel>> CreateAllPanel()
		{
			std::vector<std::unique_ptr<IPanel>> allPanel;
			allPanel.reserve( m_createfunctions.size() );

			for ( PanelCreateFunc func : m_createfunctions )
			{
				allPanel.emplace_back( func() );
			}

			return allPanel;
		}

	private:
		std::vector<PanelCreateFunc> m_createfunctions;
	};
}

#define REGISTER_PANEL( type ) \
	static bool type##_register = PanelFactory::GetInstance().RegisterPanel<type>();
