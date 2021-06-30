#pragma once
#include "Component.h"
#include "Delegate.h"
#include "UserInput/UserInput.h"

#include <unordered_map>

using InputDelegate = Delegate<void, const UserInput&>;

class InputComponent : public Component
{
public:
	using Component::Component;

	template <typename T>
	void BindInput( USER_INPUT_CODE code, T* object, void (T::*memberFunc)( const UserInput& ) )
	{
		InputDelegate delegate;
		delegate.BindMemberFunction( object, memberFunc );

		m_inputBinding.emplace( code, std::move( delegate ) );
	}

	void ProcessInput( const UserInput& input );

private:
	std::unordered_map<USER_INPUT_CODE, InputDelegate> m_inputBinding;
};