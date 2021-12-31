#pragma once

#include "Matrix.h"
#include "Quaternion.h"
#include "Vector.h"

#include <DirectXMath.h>
#include <utility>

struct ScaleMatrix : public Matrix
{
public:
	ScaleMatrix( float scale ) :
		Matrix( XMMatrix( DirectX::XMMatrixScaling( scale, scale, scale ) ) ) {}

	ScaleMatrix( float scaleX, float scaleY, float scaleZ ) :
		Matrix( XMMatrix( DirectX::XMMatrixScaling( scaleX, scaleY, scaleZ ) ) ) {}

	ScaleMatrix( const Vector& scale ) :
		Matrix( XMMatrix( DirectX::XMMatrixScalingFromVector( XMVector( scale ) ) ) ) {}

	using Matrix::Matrix;
};

struct RotateMatrix : public Matrix
{
public:
	RotateMatrix( float pitch, float yaw, float roll ) :
		Matrix( XMMatrix( DirectX::XMMatrixRotationRollPitchYaw( pitch, yaw, roll ) ) ) {}

	RotateMatrix( const Vector& rotate ) :
		Matrix( XMMatrix( DirectX::XMMatrixRotationRollPitchYawFromVector( XMVector( rotate ) ) ) ) {}

	RotateMatrix( const Quaternion& quat ) :
		Matrix( XMMatrix( DirectX::XMMatrixRotationQuaternion( XMVector( quat ) ) ) ) {}

	using Matrix::Matrix;
};

struct TranslationMatrix : public Matrix
{
public:
	TranslationMatrix( float deltaX, float deltaY, float deltaZ ) :
		Matrix( XMMatrix( DirectX::XMMatrixTranslation( deltaX, deltaY, deltaZ ) ) ) {}

	TranslationMatrix( const Vector& delta ) :
		Matrix( XMMatrix( DirectX::XMMatrixTranslationFromVector( XMVector( delta ) ) ) ) {}

	using Matrix::Matrix;
};

struct ScaleRotationTranslationMatrix : public Matrix
{
public:
	ScaleRotationTranslationMatrix( const Vector& scale, const Quaternion& rotate, const Vector& delta ) :
		Matrix( XMMatrix( DirectX::XMMatrixAffineTransformation( XMVector( scale ), XMVector( Vector::ZeroVector ), XMVector( rotate ), XMVector( delta ) ) ) ) {}

	using Matrix::Matrix;
};

struct LookFromMatrix : public Matrix
{
public:
	LookFromMatrix( const Vector& eyePosition, const Vector& eyeDirection, const Vector& upDirection ) :
		Matrix( XMMatrix( DirectX::XMMatrixLookToLH( XMVector( eyePosition ), XMVector( eyeDirection ), XMVector( upDirection ) ) ) ) {}

	using Matrix::Matrix;
};

struct LookAtMatrix : public Matrix
{
public:
	LookAtMatrix( const Vector& eyePosition, const Vector& focusPosition, const Vector& upDirection ) :
		Matrix( XMMatrix( DirectX::XMMatrixLookAtLH( XMVector( eyePosition ), XMVector( focusPosition ), XMVector( upDirection ) ) ) ) {}

	using Matrix::Matrix;
};

struct OrthoMatrix : public Matrix
{
public:
	OrthoMatrix( const std::pair<float, float>& screenSize, float nearPlane, float farPlane ) :
		Matrix( XMMatrix( DirectX::XMMatrixOrthographicLH( screenSize.first, screenSize.second, nearPlane, farPlane))) {}

	OrthoMatrix( float left, float right, float bottom, float top, float nearPlane, float farPlane ) :
		Matrix( XMMatrix( DirectX::XMMatrixOrthographicOffCenterLH( left, right, bottom, top, nearPlane, farPlane ) ) ) {}

	using Matrix::Matrix;
};

struct PerspectiveMatrix : public Matrix
{
public:
	PerspectiveMatrix( const std::pair<float, float>& screenSize, float nearPlane, float farPlane ) :
		Matrix( XMMatrix( DirectX::XMMatrixPerspectiveLH( screenSize.first, screenSize.second, nearPlane, farPlane ) ) ) {}

	PerspectiveMatrix( float fov, float aspect, float nearPlane, float farPlane ) :
		Matrix( XMMatrix( DirectX::XMMatrixPerspectiveFovLH( fov, aspect, nearPlane, farPlane ) ) ) {}

	using Matrix::Matrix;
};
