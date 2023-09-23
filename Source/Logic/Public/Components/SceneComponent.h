#pragma once

#include "Component.h"
#include "Core/WeldingTransformRules.h"
#include "Math/Matrix.h"
#include "Math/Quaternion.h"
#include "Math/Transform.h"
#include "Math/Vector.h"
#include "Physics/BoxSphereBounds.h"
#include "SparseArray.h"

namespace logic
{
	class SceneComponent : public Component
	{
		GENERATE_CLASS_TYPE_INFO( SceneComponent )

	public:
		using Component::Component;

		virtual void DestroyComponent() override;

		virtual void LoadProperty( const json::Value& json ) override;

		void SetPosition( const Vector& translation );
		void SetScale3D( const Vector& scale3D );
		void SetRotation( const Quaternion& rotation );

		void SetRelativePosition( const Vector& translation );
		void SetRelativeScale3D( const Vector& scale3D );
		void SetRelativeRotation( const Quaternion& rotation );

		const Vector& GetPosition() const;
		const Vector& GetScale3D() const;
		const Quaternion& GetRotation() const;

		const Vector& GetRelativePosition() const;
		const Vector& GetRelativeScale3D() const;
		const Quaternion& GetRelativeRotation() const;

		Vector GetForwardVector() const;
		Vector GetRightVector() const;
		Vector GetUpVector() const;

		const Transform& GetTransform() const;

		const Matrix& GetTransformMatrix();
		const Matrix& GetInvTransformMatrix();

		virtual BoxSphereBounds CalcBounds( const Matrix& transform );
		void UpdateBounds();
		const BoxSphereBounds& Bounds() const
		{
			return m_bounds;
		}

		SceneComponent* GetAttachParent() const;
		bool IsAttachedTo( const SceneComponent& component ) const;
		bool AttachToComponent( SceneComponent* parent, const AttachmentTrasformRules& attachmentRules );
		void DetachFromComponent( const DetachmentTrasformRules& detachmentRules );

	private:
		void RebuildTransformMatrix();
		void UpdateTransform();
		void UpdateRelativeTransform();
		void UpdateChildTransform();
		void PropagateTransformUpdate();

		SceneComponent* m_attachParent = nullptr;
		SparseArray<SceneComponent*> m_attachedChildren;

		Transform m_transform;
		Transform m_relativeTransform;

		Matrix m_matTransform;
		Matrix m_invMatTransform;

		BoxSphereBounds m_bounds;

		bool m_needRebuildTransformMatrix = true;
	};
}
