#include "GameObject/GameObject.h"

#include "Components/Component.h"
#include "Components/InputComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "Core/GameLogic.h"
#include "Core/Timer.h"
#include "GameObject/GameObjectFactory.h"
#include "InterfaceFactories.h"
#include "Json/json.hpp"
#include "Math/Util.h"
#include "Physics/AxisAlignedBox.h"
#include "Physics/BoundingSphere.h"
#include "Physics/OrientedBox.h"
//#include "Render/IRenderer.h"
#include "Scene/DebugOverlayManager.h"

#include <tchar.h>

namespace logic
{
	void CGameObject::OnDeviceRestore( CGameLogic& gameLogic )
	{
		//m_material = INVALID_MATERIAL;
		//m_overrideMaterial = INVALID_MATERIAL;

		//LoadModelMesh( gameLogic );
		//LoadMaterial( gameLogic );
	}

	void CGameObject::Initialize( CGameLogic& gameLogic, World& world )
	{
		m_pWorld = &world;

		for ( std::unique_ptr<Component>& component : m_components )
		{
			component->RegisterComponent();
		}

		RegisterThinkFunction();

		for ( std::unique_ptr<Component>& component : m_components )
		{
			component->RegisterThinkFunction();
		}
	}

	void CGameObject::SetPosition( const Vector& translation )
	{
		if ( m_rootComponent )
		{
			m_rootComponent->SetPosition( translation );
		}
	}

	void CGameObject::SetScale3D( const Vector& scale3D )
	{
		if ( m_rootComponent )
		{
			m_rootComponent->SetScale3D( scale3D );
		}

		// m_body.SetDirty( DF_SCALING );
	}

	void CGameObject::SetRotation( const Quaternion& rotation )
	{
		if ( m_rootComponent )
		{
			m_rootComponent->SetRotation( rotation );
		}

		// m_body.SetOrientation( m_vecRotate );
		// m_body.SetDirty( DF_ROTATION );
	}

	void CGameObject::SetRelativePosition( const Vector& translation )
	{
		if ( m_rootComponent )
		{
			m_rootComponent->SetRelativePosition( translation );
		}
	}

	void CGameObject::SetRelativeScale3D( const Vector& scale3D )
	{
		if ( m_rootComponent )
		{
			m_rootComponent->SetRelativeScale3D( scale3D );
		}
	}

	void CGameObject::SetRelativeRotation( const Quaternion& rotation )
	{
		if ( m_rootComponent )
		{
			m_rootComponent->SetRelativeRotation( rotation );
		}
	}

	const Vector& CGameObject::GetPosition() const
	{
		if ( m_rootComponent )
		{
			return m_rootComponent->GetPosition();
		}

		return Vector::ZeroVector;
	}

	const Vector& CGameObject::GetScale3D() const
	{
		if ( m_rootComponent )
		{
			return m_rootComponent->GetScale3D();
		}

		return Vector::OneVector;
	}

	const Quaternion& CGameObject::GetRotation() const
	{
		if ( m_rootComponent )
		{
			return m_rootComponent->GetRotation();
		}

		return Quaternion::Identity;
	}

	const Vector& CGameObject::GetRelativePosition() const
	{
		if ( m_rootComponent )
		{
			return m_rootComponent->GetRelativePosition();
		}

		return Vector::ZeroVector;
	}

	const Vector& CGameObject::GetRelativeScale3D() const
	{
		if ( m_rootComponent )
		{
			return m_rootComponent->GetRelativeScale3D();
		}

		return Vector::OneVector;
	}

	const Quaternion& CGameObject::GetRelativeRotation() const
	{
		if ( m_rootComponent )
		{
			return m_rootComponent->GetRelativeRotation();
		}

		return Quaternion::Identity;
	}

	Vector CGameObject::GetForwardVector() const
	{
		if ( m_rootComponent )
		{
			return m_rootComponent->GetForwardVector();
		}

		return Vector::ForwardVector;
	}

	Vector CGameObject::GetRightVector() const
	{
		if ( m_rootComponent )
		{
			return m_rootComponent->GetRightVector();
		}

		return Vector::RightVector;
	}

	Vector CGameObject::GetUpVector() const
	{
		if ( m_rootComponent )
		{
			return m_rootComponent->GetUpVector();
		}

		return Vector::UpVector;
	}

	void CGameObject::SetTransform( const Transform& transform )
	{
		if ( m_rootComponent )
		{
			m_rootComponent->SetTransform( transform );
		}
	}

	const Transform& CGameObject::GetTransform() const
	{
		if ( m_rootComponent )
		{
			return m_rootComponent->GetTransform();
		}

		return Transform::Identity;
	}

	const Matrix& CGameObject::GetTransformMatrix()
	{
		if ( m_rootComponent )
		{
			return m_rootComponent->GetTransformMatrix();
		}

		return Matrix::Identity;
	}

	const Matrix& CGameObject::GetInvTransformMatrix()
	{
		if ( m_rootComponent )
		{
			return m_rootComponent->GetInvTransformMatrix();
		}

		return Matrix::Identity;
	}

	void CGameObject::Think( float elapsedTime )
	{
		if ( m_isPicked )
		{
			//m_body.SetVelocity( CXMFLOAT3( 0.f, 0.f, 0.f ) );
			//m_body.SetRotation( CXMFLOAT3( 0.f, 0.f, 0.f ) );
		}
	}

	void CGameObject::SetMaterialName( const std::string& pMaterialName )
	{
		m_materialName = pMaterialName;
	}

	void CGameObject::LoadProperty( const json::Value& json )
	{
		if ( const json::Value* pName = json.Find( "Name" ) )
		{
			SetName( pName->AsString() );
		}

		if ( m_rootComponent )
		{
			m_rootComponent->LoadProperty( json );
		}

		if ( const json::Value* pCompoenetPropertyArray = json.Find( "Components" ) )
		{
			std::vector<const char*> componentNames = pCompoenetPropertyArray->GetMemberNames();
			for ( const char* componentName : componentNames )
			{
				Component* pComponent = GetComponent( Name( componentName ) );
				if ( pComponent == nullptr )
				{
					continue;
				}

				pComponent->LoadProperty( *pCompoenetPropertyArray->Find( componentName ) );
			}
		}
	}

	void CGameObject::SetRootComponent( SceneComponent* component )
	{
		if ( component == nullptr || component->GetOwner() == this )
		{
			m_rootComponent = component;
		}
	}

	void CGameObject::RemoveComponent( const Component* component )
	{
		auto iter = std::remove_if( std::begin( m_components ), std::end( m_components ),
			[component]( std::unique_ptr<Component>& elem )
			{
				return elem.get() == component;
			} );

		m_components.erase( iter, std::end( m_components ) );
	}

	void CGameObject::SetInputController( InputController* inputController )
	{
		m_inputController = inputController;
	}

	InputComponent* CGameObject::GetInputComponent()
	{
		return m_inputComponent;
	}

	void CGameObject::InitializeInputComponent()
	{
		if ( m_inputComponent == nullptr )
		{
			m_inputComponent = CreateComponent<InputComponent>( *this, "InputComponent" );
			SetupInputComponent();
		}
	}

	CGameObject::CGameObject()
	{
		m_think.m_thinkGroup = ThinkingGroup::PrePhysics;
		m_think.m_canEverTick = false;
		m_think.m_thinkInterval = 0.f;
	}

	CGameObject::~CGameObject()
	{
		if ( m_inputController )
		{
			m_inputController->Abandon();
		}

		UnRegisterThinkFunction();

		std::vector<std::unique_ptr<Component>> willRemove;
		willRemove.swap( m_components );

		for ( std::unique_ptr<Component>& component : willRemove )
		{
			component->DestroyComponent();
		}
	}

	void CGameObject::SetupInputComponent()
	{
	}

	Component* CGameObject::GetComponent( const Name& name )
	{
		auto found = m_componentMap.find( name );

		return ( found != std::end( m_componentMap ) ) ? found->second : nullptr;
	}

	void CGameObject::RegisterThinkFunction()
	{
		if ( m_think.m_canEverTick )
		{
			assert( GetWorld() != nullptr );

			m_think.m_target = this;
			m_think.RegisterThinkFunction( *GetWorld() );
		}
	}

	void CGameObject::UnRegisterThinkFunction()
	{
		m_think.UnRegisterThinkFunction();
	}

	void RemoveObject( CGameObject& object )
	{
		object.AddProperty( GameobjectProperty::RemoveMe );
	}
}
