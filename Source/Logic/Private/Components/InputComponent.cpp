#include "stdafx.h"
#include "Components/InputComponent.h"

namespace logic
{
	void InputComponent::ProcessInput( const engine::UserInput& input )
	{
		auto found = m_inputBinding.find( input.m_code );

		if ( found != std::end( m_inputBinding ) )
		{
			const InputDelegate& delegate = found->second;

			if ( delegate.IsBound() )
			{
				delegate( input );
			}
		}
	}
}
