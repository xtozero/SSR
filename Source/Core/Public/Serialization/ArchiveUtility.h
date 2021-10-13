#include "Math/CXMFloat.h"

class Archive;

Archive& operator<<( Archive& ar, CXMFLOAT4& v )
{
	ar << v[0] << v[1] << v[2] << v[3];
	return ar;
}
