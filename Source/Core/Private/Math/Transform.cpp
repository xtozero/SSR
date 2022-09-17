#include "Transform.h"

#include "TransformationMatrix.h"

using ::DirectX::XMVectorAbs;
using ::DirectX::XMVector3LessOrEqual;
using ::DirectX::XMVector4LessOrEqual;
using ::DirectX::XMVector3Rotate;

Matrix Transform::ToMatrix() const
{
	return ScaleRotationTranslationMatrix( m_scale3D, m_rotation, m_translation );
}

Matrix Transform::ToInverseMatrix() const
{
	return ToMatrix().Inverse();
}

Matrix Transform::ToMatrixNoScale() const
{
	return ScaleRotationTranslationMatrix( Vector::OneVector, m_rotation, m_translation );
}

Matrix Transform::ToInverseMatrixNoScale() const
{
	return ToMatrixNoScale().Inverse();
}

bool Transform::IsRotationNormalized() const
{
	float testValue = abs( 1.f - ( m_rotation | m_rotation ) );
	return testValue < 0.01f;
}

Transform Transform::GetRelativeTransform( const Transform other ) const
{
	if ( other.IsRotationNormalized() == false )
	{
		return Transform::Identity;
	}

	Transform result;

	// ToDo : handle negative scaling
	
	XMVector scale3D = m_scale3D / other.m_scale3D;

	XMVector inverseRot = other.m_rotation.Inverse();
	XMVector rotation = inverseRot * m_rotation;

	XMVector deltaTranslation = m_translation - other.m_translation;
	XMVector vr = XMVector3Rotate( deltaTranslation, inverseRot );
	XMVector translation = vr / other.m_scale3D;

	result.m_scale3D = scale3D;
	result.m_rotation = rotation;
	result.m_translation = translation;

	return result;
}

Transform Transform::GetRelativeTransformReverse( const Transform other ) const
{
	Transform result;

	XMVector scale3D = other.m_scale3D / m_scale3D;

	Quaternion inverseRot = m_rotation.Inverse();
	XMVector rotation = other.m_rotation * inverseRot;

	XMVector vr = XMVector3Rotate( XMVector( m_translation ), rotation );
	XMVector translation = other.m_translation - scale3D * vr;

	result.m_scale3D = scale3D;
	result.m_rotation = rotation;
	result.m_translation = translation;

	return result;
}

void Transform::SetToRelativeTransform( const Transform parentTransform )
{
	assert( parentTransform.IsRotationNormalized() );
	m_scale3D = m_scale3D / parentTransform.m_scale3D;

	XMVector inverseRot = parentTransform.m_rotation.Inverse();
	m_rotation = inverseRot * m_rotation;

	XMVector deltaTranslation = m_translation - parentTransform.m_translation;
	XMVector vr = XMVector3Rotate( deltaTranslation, inverseRot );
	m_translation = vr / parentTransform.m_scale3D;
}

Vector Transform::TransformPosition( const Vector& v ) const
{
	XMVector result = m_scale3D * v;
	result = XMVector3Rotate( result, XMVector( m_rotation ) );
	result += m_translation;

	return result;
}

Vector Transform::TransformPositionNoScale( const Vector& v ) const
{
	XMVector result = XMVector3Rotate( XMVector( v ), XMVector(m_rotation));
	result += m_translation;

	return Vector();
}

Vector Transform::TransformVector( const Vector& v ) const
{
	XMVector result = m_scale3D * v;
	result = XMVector3Rotate( result, XMVector( m_rotation ) );

	return result;
}

Vector Transform::TransformVectorNoScale( const Vector& v ) const
{
	XMVector result = XMVector3Rotate( XMVector( v ), XMVector( m_rotation ) );

	return result;
}

Vector Transform::InverseTransformPosition( const Vector& v ) const
{
	XMVector result = v - m_translation;
	result = XMVector3Rotate( result, m_rotation.Inverse() );
	result /= m_scale3D;

	return result;
}

Vector Transform::InverseTransformPositionNoScale( const Vector& v ) const
{
	XMVector result = v - m_translation;
	result = XMVector3Rotate( result, m_rotation.Inverse() );

	return result;
}

Vector Transform::InverseTransformVector( const Vector& v ) const
{
	XMVector result = XMVector3Rotate( XMVector( v ), m_rotation.Inverse());
	result /= m_scale3D;

	return result;
}

Vector Transform::InverseTransformVectorNoScale( const Vector& v ) const
{
	XMVector result = XMVector3Rotate( XMVector( v ), m_rotation.Inverse() );

	return result;
}

Quaternion Transform::TransformQuaternion( const Quaternion q ) const
{
	return m_rotation * q;
}

Quaternion Transform::InverseTransformQuaternion( const Quaternion q ) const
{
	return m_rotation.Inverse() * q;
}

const Vector& Transform::GetScale3D() const
{
	return m_scale3D;
}

void Transform::SetScale3D( const Vector& scale3D )
{
	m_scale3D = scale3D;
}

const Quaternion& Transform::GetRotation() const
{
	return m_rotation;
}

void Transform::SetRotation( const Quaternion& rotation )
{
	m_rotation = rotation;
}

const Vector& Transform::GetTranslation() const
{
	return m_translation;
}

void Transform::SetTranslation( const Vector& translation )
{
	m_translation = translation;
}

Transform::Transform()
{
	m_rotation = Quaternion::Identity;
	m_translation = Vector::ZeroVector;
	m_scale3D = Vector::OneVector;
}

Transform::Transform( const Vector& translation )
{
	m_rotation = Quaternion::Identity;
	m_translation = translation;
	m_scale3D = Vector::OneVector;
}

Transform::Transform( const Quaternion& rotation )
{
	m_rotation = rotation;
	m_translation = Vector::ZeroVector;
	m_scale3D = Vector::OneVector;
}

Transform::Transform( const Vector& translation, const Quaternion& rotation, const Vector& scale3D )
	: m_translation( translation ),
	m_rotation( rotation ),
	m_scale3D( scale3D )
{
}

Transform::Transform( const Matrix& matrix )
{
	SetFromMatrix( matrix );
}

Transform operator*( const Transform& lhs, const Transform& rhs )
{
	return Transform::Multiply( lhs, rhs );
}

Transform Transform::Multiply( const Transform& lhs, const Transform& rhs )
{
	Transform result;

	result.m_scale3D = lhs.m_scale3D * rhs.m_scale3D;
	result.m_rotation = rhs.m_rotation * lhs.m_rotation;

	XMVector scaledTranslation = lhs.m_translation * rhs.m_scale3D;
	XMVector rotatedTranslation = XMVector3Rotate( scaledTranslation, XMVector( rhs.m_rotation ) );
	result.m_translation = rotatedTranslation + rhs.m_translation;

	return result;
}

bool AreRotationsEqual( const Transform& lhs, const Transform& rhs, float tolerance )
{
	XMVector toleranceVec = Vector4( tolerance, tolerance, tolerance, tolerance );
	XMVector rotationSub = XMVectorAbs( lhs.m_rotation - rhs.m_rotation );
	XMVector rotationAdd = XMVectorAbs( lhs.m_rotation + rhs.m_rotation );

	return XMVector4LessOrEqual( rotationSub, toleranceVec ) || XMVector3LessOrEqual( rotationAdd, toleranceVec );
}

bool AreTranslationsEqual( const Transform& lhs, const Transform& rhs, float tolerance )
{
	XMVector toleranceVec = Vector4( tolerance, tolerance, tolerance, 0 );
	XMVector translationDiff = XMVectorAbs( lhs.m_translation - rhs.m_translation );

	return XMVector3LessOrEqual( translationDiff, toleranceVec );
}

bool AreScale3DsEqual( const Transform& lhs, const Transform& rhs, float tolerance )
{
	XMVector toleranceVec = Vector4( tolerance, tolerance, tolerance, 0 );
	XMVector scaleDiff = XMVectorAbs( lhs.m_scale3D - rhs.m_scale3D );

	return XMVector3LessOrEqual( scaleDiff, toleranceVec );
}

bool Transform::RotationEquals( const Transform& other, float tolerance )
{
	return AreRotationsEqual( *this, other, tolerance );
}

bool Transform::TranslationEquals( const Transform& other, float tolerance )
{
	return AreTranslationsEqual( *this, other, tolerance );
}

bool Transform::Scale3DEquals( const Transform& other, float tolerance )
{
	return AreScale3DsEqual( *this, other, tolerance );
}

bool Transform::Equals( const Transform& other, float tolerance )
{
	return RotationEquals( other, tolerance )
		&& TranslationEquals( other, tolerance )
		&& Scale3DEquals( other, tolerance );
}

void Transform::SetFromMatrix( const Matrix& matrix )
{
	Matrix m = matrix;

	m_scale3D = m.ExtractScaling();

	// ToDo : handle negative scaling

	m_rotation = Quaternion( m );
	m_translation = Vector( m[3][0], m[3][1], m[3][2] );
}

const Transform Transform::Identity;