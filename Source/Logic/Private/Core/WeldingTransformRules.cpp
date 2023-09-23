#include "Core/WeldingTransformRules.h"

namespace logic
{
	AttachmentTrasformRules AttachmentTrasformRules::KeepRelativeTransform( WeldingRule::KeepRelative );
	AttachmentTrasformRules AttachmentTrasformRules::KeepWorldTransform( WeldingRule::KeepWorld );

	DetachmentTrasformRules DetachmentTrasformRules::KeepRelativeTransform( WeldingRule::KeepRelative );
	DetachmentTrasformRules DetachmentTrasformRules::KeepWorldTransform( WeldingRule::KeepWorld );
}
