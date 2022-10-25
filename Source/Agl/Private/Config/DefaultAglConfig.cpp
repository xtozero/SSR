#include "Config/DefaultAglConfig.h"

namespace agl
{
	AglType DefaultAgl::GetType() const
	{
		return static_cast<AglType>( m_type );
	}
}
