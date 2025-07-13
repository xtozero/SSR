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

#include <tchar.h>

namespace logic
{
	void GameObject::Initialize( [[maybe_unused]] GameLogic& gameLogic, World& world )
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

	void GameObject::SetPosition( const Vector& translation )
	{
		if ( m_rootComponent )
		{
			m_rootComponent->SetPosition( translation );
		}
	}

	void GameObject::SetScale3D( const Vector& scale3D )
	{
		if ( m_rootComponent )
		{
			m_rootComponent->SetScale3D( scale3D );
		}
	}

	void GameObject::SetRotation( const Quaternion& rotation )
	{
		if ( m_rootComponent )
		{
			m_rootComponent->SetRotation( rotation );
		}
	}

	void GameObject::SetRelativePosition( const Vector& translation )
	{
		if ( m_rootComponent )
		{
			m_rootComponent->SetRelativePosition( translation );
		}
	}

	void GameObject::SetRelativeScale3D( const Vector& scale3D )
	{
		if ( m_rootComponent )
		{
			m_rootComponent->SetRelativeScale3D( scale3D );
		}
	}

	void GameObject::SetRelativeRotation( const Quaternion& rotation )
	{
		if ( m_rootComponent )
		{
			m_rootComponent->SetRelativeRotation( rotation );
		}
	}

	const Vector& GameObject::GetPosition() const
	{
		if ( m_rootComponent )
		{
			return m_rootComponent->GetPosition();
		}

		return Vector::ZeroVector;
	}

	const Vector& GameObject::GetScale3D() const
	{
		if ( m_rootComponent )
		{
			return m_rootComponent->GetScale3D();
		}

		return Vector::OneVector;
	}

	const Quaternion& GameObject::GetRotation() const
	{
		if ( m_rootComponent )
		{
			return m_rootComponent->GetRotation();
		}

		return Quaternion::Identity;
	}

	const Vector& GameObject::GetRelativePosition() const
	{
		if ( m_rootComponent )
		{
			return m_rootComponent->GetRelativePosition();
		}

		return Vector::ZeroVector;
	}

	const Vector& GameObject::GetRelativeScale3D() const
	{
		if ( m_rootComponent )
		{
			return m_rootComponent->GetRelativeScale3D();
		}

		return Vector::OneVector;
	}

	const Quaternion& GameObject::GetRelativeRotation() const
	{
		if ( m_rootComponent )
		{
			return m_rootComponent->GetRelativeRotation();
		}

		return Quaternion::Identity;
	}

	Vector GameObject::GetForwardVector() const
	{
		if ( m_rootComponent )
		{
			return m_rootComponent->GetForwardVector();
		}

		return Vector::ForwardVector;
	}

	Vector GameObject::GetRightVector() const
	{
		if ( m_rootComponent )
		{
			return m_rootComponent->GetRightVector();
		}

		return Vector::RightVector;
	}

	Vector GameObject::GetUpVector() const
	{
		if ( m_rootComponent )
		{
			return m_rootComponent->GetUpVector();
		}

		return Vector::UpVector;
	}

	void GameObject::SetTransform( const Transform& transform )
	{
		if ( m_rootComponent )
		{
			m_rootComponent->SetTransform( transform );
		}
	}

	const Transform& GameObject::GetTransform() const
	{
		if ( m_rootComponent )
		{
			return m_rootComponent->GetTransform();
		}

		return Transform::Identity;
	}

	const Matrix& GameObject::GetTransformMatrix()
	{
		if ( m_rootComponent )
		{
			return m_rootComponent->GetTransformMatrix();
		}

		return Matrix::Identity;
	}

	const Matrix& GameObject::GetInvTransformMatrix()
	{
		if ( m_rootComponent )
		{
			return m_rootComponent->GetInvTransformMatrix();
		}

		return Matrix::Identity;
	}

	void GameObject::Think( [[maybe_unused]] float elapsedTime )
	{
	}

	void GameObject::LoadProperty( const json::Value& json )
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

	void GameObject::SetRootComponent( SceneComponent* component )
	{
		if ( component == nullptr || component->GetOwner() == this )
		{
			m_rootComponent = component;
		}
	}

	void GameObject::RemoveComponent( const Component* component )
	{
		std::erase_if( m_components, [component]( std::unique_ptr<Component>& elem )
			{
				return elem.get() == component;
			} );
	}

	void GameObject::SetInputController( InputController* inputController )
	{
		m_inputController = inputController;
	}

	InputComponent* GameObject::GetInputComponent()
	{
		return m_inputComponent;
	}

	void GameObject::InitializeInputComponent()
	{
		if ( m_inputComponent == nullptr )
		{
			m_inputComponent = CreateComponent<InputComponent>( *this, "InputComponent" );
			SetupInputComponent();
		}
	}

	GameObject::GameObject()
	{
		m_think.m_thinkGroup = ThinkingGroup::PrePhysics;
		m_think.m_canEverTick = false;
		m_think.m_thinkInterval = 0.f;
	}

	GameObject::~GameObject()
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

	void GameObject::SetupInputComponent()
	{
	}

	Component* GameObject::GetComponent( const Name& name )
	{
		auto found = m_componentMap.find( name );

		return ( found != std::end( m_componentMap ) ) ? found->second : nullptr;
	}

	void GameObject::RegisterThinkFunction()
	{
		if ( m_think.m_canEverTick )
		{
			assert( GetWorld() != nullptr );

			m_think.m_target = this;
			m_think.RegisterThinkFunction( *GetWorld() );
		}
	}

	void GameObject::UnRegisterThinkFunction()
	{
		m_think.UnRegisterThinkFunction();
	}

	void RemoveObject( GameObject& object )
	{
		object.AddProperty( GameobjectProperty::RemoveMe );
	}
}
