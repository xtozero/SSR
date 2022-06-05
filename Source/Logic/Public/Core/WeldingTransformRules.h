#pragma once

enum class WeldingRule
{
	KeepRelative,
	KeepWorld
};

struct AttachmentTrasformRules
{
	static AttachmentTrasformRules KeepRelativeTransform;
	static AttachmentTrasformRules KeepWorldTransform;

	AttachmentTrasformRules( WeldingRule rule )
		: m_translationRule( rule )
		, m_rotationRule( rule )
		, m_scaleRule( rule )
	{}

	AttachmentTrasformRules( WeldingRule translationRule, WeldingRule rotationRule, WeldingRule scaleRule )
		: m_translationRule( translationRule )
		, m_rotationRule( rotationRule )
		, m_scaleRule( scaleRule )
	{}

	WeldingRule m_translationRule;
	WeldingRule m_rotationRule;
	WeldingRule m_scaleRule;
};

struct DetachmentTrasformRules
{
	static DetachmentTrasformRules KeepRelativeTransform;
	static DetachmentTrasformRules KeepWorldTransform;

	DetachmentTrasformRules( WeldingRule rule )
		: m_translationRule( rule )
		, m_rotationRule( rule )
		, m_scaleRule( rule )
	{}

	DetachmentTrasformRules( WeldingRule translationRule, WeldingRule rotationRule, WeldingRule scaleRule )
		: m_translationRule( translationRule )
		, m_rotationRule( rotationRule )
		, m_scaleRule( scaleRule )
	{}

	WeldingRule m_translationRule;
	WeldingRule m_rotationRule;
	WeldingRule m_scaleRule;
};
