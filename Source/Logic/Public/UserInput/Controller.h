#pragma once

#include "GameObject/GameObject.h"

#include <vector>

namespace engine
{
	struct UserInput;
}

namespace logic
{
	class InputComponent;

	class InputController : public GameObject
	{
		GENERATE_CLASS_TYPE_INFO( InputController )

	public:
		virtual void ProcessInput( const engine::UserInput& input ) = 0;

		virtual void Control( GameObject* gameObject ) = 0;
		virtual void Abandon() = 0;

		void SetGameObject( GameObject* gameObject );

		InputController();

	protected:
		GameObject* m_gameObject = nullptr;
	};

	class PlayerController : public InputController
	{
		GENERATE_CLASS_TYPE_INFO( PlayerController )

	public:
		virtual void ProcessInput( const engine::UserInput& input ) override;

		virtual void Control( GameObject* gameObject ) override;
		virtual void Abandon() override;

	private:
		InputComponent* m_inputComponent = nullptr;
	};
}
