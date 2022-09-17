#include "stdafx.h"
#include "Components/SceneComponent.h"

#include "GameObject/GameObject.h"
#include "Json/Json.hpp"
#include "Math/TransformationMatrix.h"

using ::DirectX::XMConvertToRadians;

void SceneComponent::DestroyComponent()
{
	if ( CGameObject* owner = GetOwner() )
	{
		DetachFromComponent( DetachmentTrasformRules::KeepWorldTransform );
	}

	SparseArray<SceneComponent*> attachedChildrenCopy( m_attachedChildren );
	for ( SceneComponent* child : attachedChildrenCopy )
	{
		child->DetachFromComponent( DetachmentTrasformRules::KeepWorldTransform );
	}

	Super::DestroyComponent();
}

void SceneComponent::LoadProperty( const JSON::Value& json )
{
	Super::LoadProperty( json );

	if ( const JSON::Value* pPos = json.Find( "Position" ) )
	{
		const JSON::Value& pos = *pPos;

		if ( pos.Size() == 3 )
		{
			float x = static_cast<float>( pos[0].AsReal() );
			float y = static_cast<float>( pos[1].AsReal() );
			float z = static_cast<float>( pos[2].AsReal() );

			SetRelativePosition( Vector( x, y, z ) );
		}
	}

	if ( const JSON::Value* pScale = json.Find( "Scale" ) )
	{
		const JSON::Value& scale = *pScale;

		if ( scale.Size() == 3 )
		{
			float x = static_cast<float>( scale[0].AsReal() );
			float y = static_cast<float>( scale[1].AsReal() );
			float z = static_cast<float>( scale[2].AsReal() );

			SetRelativeScale3D( Vector( x, y, z ) );
		}
	}

	if ( const JSON::Value* pRotation = json.Find( "Rotation" ) )
	{
		const JSON::Value& rotation = *pRotation;

		if ( rotation.Size() == 3 )
		{
			float pitch = static_cast<float>( rotation[0].AsReal() );
			pitch = XMConvertToRadians( pitch );

			float yaw = static_cast<float>( rotation[1].AsReal() );
			yaw = XMConvertToRadians( yaw );

			float roll = static_cast<float>( rotation[2].AsReal() );
			roll = XMConvertToRadians( roll );

			SetRelativeRotation( Quaternion( pitch, yaw, roll ) );
		}
	}
}

void SceneComponent::SetPosition( const Vector& translation )
{
	if ( m_transform.GetTranslation() == translation )
	{
		return;
	}

	m_transform.SetTranslation( translation );

	UpdateRelativeTransform();
	MarkRenderTransformDirty();
}

void SceneComponent::SetScale3D( const Vector& scale3D )
{
	if ( m_transform.GetScale3D() == scale3D )
	{
		return;
	}

	m_transform.SetScale3D( scale3D );

	UpdateRelativeTransform();
	MarkRenderTransformDirty();
}

void SceneComponent::SetRotation( const Quaternion& rotation )
{
	if ( m_transform.GetRotation() == rotation )
	{
		return;
	}

	m_transform.SetRotation( rotation );

	UpdateRelativeTransform();
	MarkRenderTransformDirty();
}

void SceneComponent::SetRelativePosition( const Vector& translation )
{
	if ( m_relativeTransform.GetTranslation() == translation )
	{
		return;
	}

	m_relativeTransform.SetTranslation( translation );

	UpdateTransform();
	MarkRenderTransformDirty();
}

void SceneComponent::SetRelativeScale3D( const Vector& scale3D )
{
	if ( m_relativeTransform.GetScale3D() == scale3D )
	{
		return;
	}

	m_relativeTransform.SetScale3D( scale3D );

	UpdateTransform();
	MarkRenderTransformDirty();
}

void SceneComponent::SetRelativeRotation( const Quaternion& rotation )
{
	if ( m_relativeTransform.GetRotation() == rotation )
	{
		return;
	}

	m_relativeTransform.SetRotation( rotation );

	UpdateTransform();
	MarkRenderTransformDirty();
}

const Vector& SceneComponent::GetPosition() const
{
	return m_transform.GetTranslation();
}

const Vector& SceneComponent::GetScale3D() const
{
	return m_transform.GetScale3D();
}

const Quaternion& SceneComponent::GetRotation() const
{
	return m_transform.GetRotation();
}

const Vector& SceneComponent::GetRelativePosition() const
{
	return m_relativeTransform.GetTranslation();
}

const Vector& SceneComponent::GetRelativeScale3D() const
{
	return m_relativeTransform.GetScale3D();
}

const Quaternion& SceneComponent::GetRelativeRotation() const
{
	return m_relativeTransform.GetRotation();
}

Vector SceneComponent::GetForwardVector() const
{
	return m_transform.TransformVectorNoScale( Vector::ForwardVector ).GetNormalized();
}

Vector SceneComponent::GetRightVector() const
{
	return m_transform.TransformVectorNoScale( Vector::RightVector ).GetNormalized();
}

Vector SceneComponent::GetUpVector() const
{
	return m_transform.TransformVectorNoScale( Vector::UpVector ).GetNormalized();
}

const Transform& SceneComponent::GetTransform() const
{
	return m_transform;
}

const Matrix& SceneComponent::GetTransformMatrix()
{
	RebuildTransformMatrix();
	return m_matTransform;
}

const Matrix& SceneComponent::GetInvTransformMatrix()
{
	RebuildTransformMatrix();
	return m_invMatTransform;
}

BoxSphereBounds SceneComponent::CalcBounds( [[maybe_unused]] const Matrix& transform )
{
	return BoxSphereBounds( Vector( 0, 0, 0 ), Vector( 0, 0, 0 ), 0.f );
}

void SceneComponent::UpdateBounds()
{
	m_bounds = CalcBounds( GetTransformMatrix() );
}

SceneComponent* SceneComponent::GetAttachParent() const
{
	return m_attachParent;
}

bool SceneComponent::IsAttachedTo( const SceneComponent& component ) const
{
	for ( const SceneComponent* parent = component.GetAttachParent(); parent != nullptr; parent = parent->GetAttachParent() )
	{
		if ( parent == &component )
		{
			return true;
		}
	}

	return false;
}

bool SceneComponent::AttachToComponent( SceneComponent* parent, const AttachmentTrasformRules& attachmentRules )
{
	if ( parent != nullptr )
	{
		if ( parent == this )
		{
			assert( false && "Can't be attached to itself" );
			return false;
		}

		bool alreadAttached = ( GetAttachParent() == parent ) && ( parent->m_attachedChildren.Find( this ) != std::end( parent->m_attachedChildren ) );
		if ( alreadAttached )
		{
			return true;
		}

		CGameObject* myOwner = GetOwner();
		CGameObject* parentOwner = GetOwner();

		if ( myOwner == parentOwner && myOwner && myOwner->GetRootComponent() == this )
		{
			assert( false && "Root component can't be attached to other components in the same actor" );
			return false;
		}

		if ( parent->IsAttachedTo( *this ) )
		{
			assert( false && "Already attached to this component" );
			return false;
		}

		m_attachParent = parent;
		parent->m_attachedChildren.Add( this );

		Transform parentTransform = GetAttachParent()->GetTransform();
		Transform relativeTransform = GetTransform().GetRelativeTransform( parentTransform );

		switch ( attachmentRules.m_translationRule )
		{
		case WeldingRule::KeepWorld:
			m_relativeTransform.SetTranslation( relativeTransform.GetTranslation() );
			break;
		default:
			break;
		}

		switch ( attachmentRules.m_rotationRule )
		{
		case WeldingRule::KeepWorld:
			m_relativeTransform.SetRotation( relativeTransform.GetRotation() );
			break;
		default:
			break;
		}

		switch ( attachmentRules.m_scaleRule )
		{
		case WeldingRule::KeepWorld:
			m_relativeTransform.SetScale3D( relativeTransform.GetScale3D() );
			break;
		default:
			break;
		}

		UpdateTransform();
	}

	return true;
}

void SceneComponent::DetachFromComponent( const DetachmentTrasformRules& detachmentRules )
{
	if ( GetAttachParent() != nullptr )
	{
		GetAttachParent()->m_attachedChildren.Remove( this );
		m_attachParent = nullptr;

		switch ( detachmentRules.m_translationRule )
		{
		case WeldingRule::KeepWorld:
			m_relativeTransform.SetTranslation( GetTransform().GetTranslation() );
			break;
		default:
			break;
		}

		switch ( detachmentRules.m_rotationRule )
		{
		case WeldingRule::KeepWorld:
			m_relativeTransform.SetRotation( GetTransform().GetRotation() );
			break;
		default:
			break;
		}

		switch ( detachmentRules.m_scaleRule )
		{
		case WeldingRule::KeepWorld:
			m_relativeTransform.SetScale3D( GetTransform().GetScale3D() );
			break;
		default:
			break;
		}

		UpdateTransform();
	}
}

void SceneComponent::RebuildTransformMatrix()
{
	if ( m_needRebuildTransformMatrix )
	{
		m_matTransform = m_transform.ToMatrix();
		m_invMatTransform = m_matTransform.Inverse();

		m_needRebuildTransformMatrix = false;
	}
}

void SceneComponent::UpdateTransform()
{
	Transform newTransform;
	if ( GetAttachParent() )
	{
		Transform parentTransform = GetAttachParent()->GetTransform();
		newTransform = parentTransform * m_relativeTransform;
	}
	else
	{
		newTransform = m_relativeTransform;
	}

	bool hasChanged = !m_transform.Equals( newTransform, 1.e-8f );

	if ( hasChanged )
	{
		m_transform = newTransform;
		m_needRebuildTransformMatrix = true;
		PropagateTransformUpdate();
	}
}

void SceneComponent::UpdateRelativeTransform()
{
	if ( GetAttachParent() )
	{
		Transform parentTransform = GetAttachParent()->GetTransform();
		m_relativeTransform = m_transform.GetRelativeTransform( parentTransform );
	}
	else
	{
		m_relativeTransform = m_transform;
	}

	m_needRebuildTransformMatrix = true;
	PropagateTransformUpdate();
}

void SceneComponent::UpdateChildTransform()
{
	for ( SceneComponent* child : m_attachedChildren )
	{
		child->UpdateTransform();
	}
}

void SceneComponent::PropagateTransformUpdate()
{
	UpdateBounds();

	MarkRenderTransformDirty();

	UpdateChildTransform();
}
