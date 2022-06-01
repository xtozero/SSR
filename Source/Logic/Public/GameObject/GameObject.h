#pragma once

#include "Core/ThinkFunction.h"
#include "GameObjectProperty.h"
#include "Math/Matrix.h"
#include "Math/Quaternion.h"
#include "Math/Vector.h"
#include "Physics/Body.h"
#include "Physics/ColliderManager.h"
//#include "Render/Resource.h"
#include "Reflection.h"
#include "Scene/INotifyGraphicsDevice.h"

#include <cstddef>
#include <limits>
#include <memory>
#include <vector>

enum DIRTY_FLAG
{
	DF_POSITION = 1 << 0,
	DF_ROTATION = 1 << 1,
	DF_SCALING = 1 << 2,
};

class BaseMesh;
class CDebugOverlayManager;
class CGameLogic;
class CGameObject;
class Component;
class ICollider;
class InputComponent;
class InputController;
class RenderOption;
class SceneComponent;
class World;

namespace JSON
{
	class Value;
}

class ObjectRelatedRigidBody : public RigidBody
{
public:
	CGameObject* GetGameObject() const { return m_gameObject; }
	const ICollider* GetCollider( int32 type );

	int32 GetDirty() const { return m_dirtyFlag; }
	void SetDirty( int32 dirtyFlag ) { m_dirtyFlag |= dirtyFlag; }
	void ResetDirty() { m_dirtyFlag = 0; }

	explicit ObjectRelatedRigidBody( CGameObject* object ) : m_gameObject( object ) {}

private:
	CGameObject* m_gameObject = nullptr;
	int32 m_dirtyFlag = 0;
};

class CGameObject : IGraphicsDeviceNotify
{
	GENERATE_CLASS_TYPE_INFO( CGameObject )

public:
	virtual void OnDeviceRestore( CGameLogic& gameLogic ) override;
	virtual void Initialize( CGameLogic& gameLogic, World& world );
	void SetPosition( const float x, const float y, const float z );
	void SetPosition( const Vector& pos );
	void SetScale( const float xScale, const float yScale, const float zScale );
	void SetRotate( const Quaternion& rotate );

	size_t GetID() const { return m_id; }
	void SetID( size_t id ) { m_id = id; }

	const Vector& GetPosition();
	const Vector& GetScale();
	const Quaternion& GetRotate();

	const Matrix& GetTransformMatrix();
	const Matrix& GetInvTransformMatrix();

	void UpdateTransform( CGameLogic& gameLogic );

	virtual void Think( float elapsedTime );

	void SetName( const std::string& name ) { m_name = name; }
	const std::string& GetName() const { return m_name; }
	void SetMaterialName( const std::string& pMaterialName );

	COLLIDER::TYPE GetColliderType() const { return m_colliderType; }
	const ICollider* GetDefaultCollider();
	const ICollider* GetCollider( int32 type );
	const std::vector<std::unique_ptr<ICollider>>& GetSubColliders( int32 type );

	void SetColliderType( COLLIDER::TYPE type ) { m_colliderType = type; }

	bool IsPicked() const { return m_isPicked; }
	void SetPicked( bool isPicked ) { m_isPicked = isPicked; }

	//Material GetMaterial( ) { return m_material; }
	//void SetMaterial( Material material ) { m_material = material; }

	//Material GetOverrideMaterial( ) { return m_overrideMaterial; }
	//void SetOverrideMaterial( Material material ) { m_overrideMaterial = material; }

	GAMEOBJECT_PROPERTY GetProperty() const { return m_property; }
	void AddProperty( GAMEOBJECT_PROPERTY property ) { m_property |= property; }
	void RemoveProperty( GAMEOBJECT_PROPERTY property ) { m_property &= ~property; }

	bool WillRemove() const { return HasAnyFlags( m_property, GAMEOBJECT_PROPERTY::REMOVE_ME ); }

	virtual void LoadProperty( CGameLogic& gameLogic, const JSON::Value& json );

	virtual bool IgnorePicking() const { return false; }

	SceneComponent* GetRootComponent() const { return m_rootComponent; }

	void SetInputController( InputController* inputController );

	InputComponent* GetInputComponent();
	void InitializeInputComponent();

	World* GetWorld() const { return m_pWorld; }

	CGameObject();
	~CGameObject();

protected:
	//virtual bool LoadMaterial( CGameLogic& gameLogic );
	virtual void CalcOriginalCollider();
	virtual void SetupInputComponent();

	template <typename T>
	T* CreateComponent( CGameObject& gameObject )
	{
		return new T( &gameObject );
	}

	template <typename T>
	T* GetComponent()
	{
		for ( Component* component : m_components )
		{
			if ( auto concrete = Cast<T>( component ) )
			{
				return concrete;
			}
		}

		return nullptr;
	}

private:
	void UpdateCollider( COLLIDER::TYPE type );
	void UpdateAllCollider();
	void UpdateSubCollider( COLLIDER::TYPE type );

	void RegisterThinkFunction();
	void UnRegisterThinkFunction();

	size_t m_id = std::numeric_limits<size_t>::max();
	World* m_pWorld = nullptr;

	//const RenderOption* m_pRenderOption = nullptr;
	//Material m_material = INVALID_MATERIAL;
	//Material m_overrideMaterial = INVALID_MATERIAL;

	std::string m_name;
	std::string m_materialName;

	bool m_isPicked = false;

	COLLIDER::TYPE m_colliderType = COLLIDER::NONE;
	ICollider* m_originalColliders[COLLIDER::COUNT] = { nullptr, };
	std::unique_ptr<ICollider>	m_colliders[COLLIDER::COUNT];

	std::vector<std::unique_ptr<ICollider>> m_subColliders[COLLIDER::COUNT];

	GAMEOBJECT_PROPERTY m_property = GAMEOBJECT_PROPERTY::NONE;

	// ObjectRelatedRigidBody m_body{ this };

	std::vector<Component*> m_components;

protected:
	GameObjectThinkFunction m_think;

	SceneComponent* m_rootComponent = nullptr;

	InputComponent* m_inputComponent = nullptr;
	InputController* m_inputController = nullptr;

	friend Component;
};

void RemoveObject( CGameObject& object );
