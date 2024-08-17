#pragma once

#include "GameObject.h"
#include "RenderView.h"
#include "UserInput/UserInput.h"

namespace logic
{
	class CameraComponent;

	class CPlayer : public CGameObject
	{
		GENERATE_CLASS_TYPE_INFO( CPlayer )

	public:
		virtual void Initialize( CGameLogic& gameLogic, World& world ) override;
		virtual void Think( float elapsedTime );

		virtual rendercore::RenderView* CalcSceneView( rendercore::RenderViewGroup& renderViewGroup );

		CPlayer();

	protected:
		virtual void SetupInputComponent() override;

	private:
		void OnMouseLButton( const engine::UserInput& input );
		void OnMouseRButton( const engine::UserInput& input );
		void OnMouseMove( const engine::UserInput& input );
		void OnWheelMove( const engine::UserInput& input );
		void OnMoveKey( const engine::UserInput& input );

		std::vector<rendercore::RenderViewStateReference, InlineAllocator<rendercore::RenderViewStateReference, 1>> m_viewStates;

		CameraComponent* m_cameraComponent = nullptr;

		bool m_cameraRotationEnabled = false;
		float m_mouseSensitivity = 0.01f;

		bool m_inputDirection[4] = { false, false, false, false };
	};
}
