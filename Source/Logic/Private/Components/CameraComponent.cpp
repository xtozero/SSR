#include "stdafx.h"
#include "Components/CameraComponent.h"

#include "common.h"
#include "Core/Timer.h"
#include "Json/json.hpp"
#include "Math/TransformationMatrix.h"

#include <type_traits>

void CameraComponent::LoadProperty( const json::Value& json )
{
	if ( const json::Value* pPos = json.Find( "Position" ) )
	{
		const json::Value& pos = *pPos;

		if ( pos.Size() == 3 )
		{
			Vector origin( static_cast<float>( pos[0].AsReal() ),
						static_cast<float>( pos[1].AsReal() ),
						static_cast<float>( pos[2].AsReal() ) );
			SetRelativePosition( origin );
		}
	}
}

void CameraComponent::Move( const float right, const float up, const float look )
{
	if ( right != 0.f || up != 0.f || look != 0.f )
	{
		Vector deltaMove = ( right * m_rightVector + up * m_upVector + look * m_lookVector );
		const Vector& curPos = GetPosition();
		SetPosition( curPos + deltaMove );
		MarkCameraTransformDirty();
	}
}

void CameraComponent::Move( Vector delta )
{
	Move( delta.x, delta.y, delta.z );
}

void CameraComponent::Rotate( const float pitch, const float yaw, const float roll )
{
	if ( m_enableRotation )
	{
		if ( pitch != 0.f || yaw != 0.f || roll != 0.f )
		{
			const Quaternion& curRotation = GetRotation();
			Quaternion deltaRotation = Quaternion( pitch, yaw, roll );
			Quaternion newRotation = curRotation * deltaRotation;
			SetRotation( newRotation );

			RotationMatrix rotationMat( GetRotation() );

			m_rightVector = rotationMat.TransformVector( Vector::RightVector );
			m_upVector = rotationMat.TransformVector( Vector::UpVector );
			m_lookVector = rotationMat.TransformVector( Vector::ForwardVector );

			m_lookVector = m_lookVector.GetNormalized();

			m_upVector = m_lookVector ^ m_rightVector;
			m_upVector = m_upVector.GetNormalized();

			m_rightVector = m_upVector ^ m_lookVector;
			m_rightVector = m_rightVector.GetNormalized();

			MarkCameraTransformDirty();
		}
	}
}

const Matrix& CameraComponent::GetViewMatrix() const
{
	ReCalcViewMatrix();
	return m_viewMatrix;
}

const Matrix& CameraComponent::GetInvViewMatrix() const
{
	ReCalcViewMatrix();
	return m_invViewMatrix;
}

void CameraComponent::ReCalcViewMatrix() const
{
	if ( m_needRecalc )
	{
		m_viewMatrix = LookFromMatrix( GetPosition(), m_lookVector, m_upVector );
		m_invViewMatrix = m_viewMatrix.Inverse();

		m_needRecalc = false;
	}
}
