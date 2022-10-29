#include "Config/DefaultAglConfig.h"

namespace agl
{
	AglType DefaultAgl::GetType()
	{
		return static_cast<AglType>( GetInstance().m_type );
	}
}
