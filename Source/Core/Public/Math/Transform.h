#pragma once

#include "Matrix.h"
#include "Quaternion.h"
#include "Vector.h"

class Transform
{
public:
	Matrix ToMatrix() const;
	Matrix ToInverseMatrix() const;
	Matrix ToMatrixNoScale() const;
	Matrix ToInverseMatrixNoScale() const;

	bool IsRotationNormalized() const;

	Transform GetRelativeTransform( const Transform other ) const;
	Transform GetRelativeTransformReverse( const Transform other ) const;

	void SetToRelativeTransform( const Transform parentTransform );

	Vector TransformPosition( const Vector& v ) const;
	Vector TransformPositionNoScale( const Vector& v ) const;
	Vector TransformVector( const Vector& v ) const;
	Vector TransformVectorNoScale( const Vector& v ) const;

	Vector InverseTransformPosition( const Vector& v ) const;
	Vector InverseTransformPositionNoScale( const Vector& v ) const;
	Vector InverseTransformVector( const Vector& v ) const;
	Vector InverseTransformVectorNoScale( const Vector& v ) const;

	Quaternion TransformQuaternion( const Quaternion q ) const;
	Quaternion InverseTransformQuaternion( const Quaternion q ) const;

	const Vector& GetScale3D() const;
	void SetScale3D( const Vector& scale3D );

	const Quaternion& GetRotation() const;
	void SetRotation( const Quaternion& rotation );

	const Vector& GetTranslation() const;
	void SetTranslation( const Vector& translation );

	Transform();
	explicit Transform( const Vector& translation );
	explicit Transform( const Quaternion& rotation );
	Transform( const Vector& translation, const Quaternion& rotation, const Vector& scale3D = Vector::OneVector );
	explicit Transform( const Matrix& matrix );

	friend Transform operator*( const Transform& lhs, const Transform& rhs );

	static Transform Multiply( const Transform& lhs, const Transform& rhs );

	friend bool AreRotationsEqual( const Transform& lhs, const Transform& rhs, float tolerance = 1.e-4 );
	friend bool AreTranslationsEqual( const Transform& lhs, const Transform& rhs, float tolerance = 1.e-4 );
	friend bool AreScale3DsEqual( const Transform& lhs, const Transform& rhs, float tolerance = 1.e-4 );

	bool RotationEquals( const Transform& other, float tolerance = 1.e-4 );
	bool TranslationEquals( const Transform& other, float tolerance = 1.e-4 );
	bool Scale3DEquals( const Transform& other, float tolerance = 1.e-4 );
	bool Equals( const Transform& other, float tolerance = 1.e-4 );

	static const Transform Identity;

private:
	void SetFromMatrix( const Matrix& matrix );

	Vector m_scale3D;
	Quaternion m_rotation;
	Vector m_translation;
};