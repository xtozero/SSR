#pragma once

#include "SceneComponent.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"

namespace json
{
	class Value;
}

namespace logic
{
	class CGameLogic;
	class IRenderer;

	class CameraComponent : public SceneComponent
	{
		GENERATE_CLASS_TYPE_INFO( CameraComponent )

	public:
		using SceneComponent::SceneComponent;

		virtual void LoadProperty( const json::Value& json ) override;

		const Vector& GetForwardVector() const { return m_lookVector; }
		const Vector& GetRightVector() const { return m_rightVector; }
		const Vector& GetUpVector() const { return m_upVector; }

		void Move( const float right, const float up, const float look );
		void Move( Vector delta );
		void Rotate( const float pitch, const float yaw, const float roll );

		const Matrix& GetViewMatrix() const;
		const Matrix& GetInvViewMatrix() const;
		void SetEnableRotation( bool isEnable ) { m_enableRotation = isEnable; }

	private:
		void ReCalcViewMatrix() const;
		void MarkCameraTransformDirty()
		{
			m_needRecalc = true;
		}

	private:
		mutable Matrix m_viewMatrix = Matrix::Identity;;
		mutable Matrix m_invViewMatrix = Matrix::Identity;;

		Vector m_lookVector = Vector::ForwardVector;
		Vector m_upVector = Vector::UpVector;
		Vector m_rightVector = Vector::RightVector;

		mutable bool m_needRecalc = false;

		bool m_enableRotation = true;
	};
}
