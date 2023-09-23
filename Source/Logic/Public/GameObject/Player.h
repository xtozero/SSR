#pragma once

#include "Core/IListener.h"
#include "GameObject.h"
#include "Scene/INotifyGraphicsDevice.h"

namespace logic
{
	class CameraComponent;

	class CPlayer : public CGameObject, public IListener
	{
		GENERATE_CLASS_TYPE_INFO( CPlayer )

	public:
		virtual void Initialize( CGameLogic& gameLogic, World& world ) override;
		virtual void Think( float elapsedTime );
		virtual void ProcessInput( const engine::UserInput& input, CGameLogic& gameLogic ) override;

		const CameraComponent* GetCameraComponent() const { return m_cameraComponent; }

		CPlayer();

	protected:
		virtual void SetupInputComponent() override;

	private:
		void OnMouseLButton( const engine::UserInput& input, CGameLogic& gameLogic );
		void RotatePrimaryLightDir( CGameLogic& gameLogic, float deltaTheta, float deltaPhi );

		void OnMouseLButton( const engine::UserInput& input );
		void OnMouseRButton( const engine::UserInput& input );
		void OnMouseMove( const engine::UserInput& input );
		void OnWheelMove( const engine::UserInput& input );
		void OnMoveKey( const engine::UserInput& input );

		CameraComponent* m_cameraComponent = nullptr;

		bool m_cameraRotationEnabled = false;
		bool m_cameraTranslationEnabled = false;
		float m_mouseSensitivity = 0.01f;

		bool m_inputDirection[4] = { false, false, false, false };
	};
}
