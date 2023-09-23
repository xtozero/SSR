#pragma once
#include "Component.h"
#include "Delegate.h"
#include "UserInput/UserInput.h"

#include <unordered_map>

namespace logic
{
	using InputDelegate = Delegate<void, const engine::UserInput&>;

	class InputComponent : public Component
	{
		GENERATE_CLASS_TYPE_INFO( InputComponent )

	public:
		using Component::Component;

		template <typename T>
		void BindInput( engine::UserInputCode code, T* object, void ( T::* memberFunc )( const engine::UserInput& ) )
		{
			InputDelegate delegate;
			delegate.BindMemberFunction( object, memberFunc );

			m_inputBinding.emplace( code, std::move( delegate ) );
		}

		void ProcessInput( const engine::UserInput& input );

	private:
		std::unordered_map<engine::UserInputCode, InputDelegate> m_inputBinding;
	};
}