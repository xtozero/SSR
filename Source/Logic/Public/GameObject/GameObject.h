#pragma once

#include "Core/ThinkFunction.h"
#include "GameObjectProperty.h"
#include "Math/Matrix.h"
#include "Math/Quaternion.h"
#include "Math/Vector.h"
#include "NameTypes.h"
#include "Reflection.h"

#include <cstddef>
#include <limits>
#include <map>
#include <memory>
#include <vector>

namespace json
{
	class Value;
}

namespace rendercore
{
	class RenderOption;
}

namespace logic
{
	enum Dirtyflag : uint8
	{
		Position = 1 << 0,
		Rotation = 1 << 1,
		Scaling = 1 << 2,
	};

	class GameLogic;
	class GameObject;
	class Component;
	class InputComponent;
	class InputController;
	class SceneComponent;
	class World;

	class GameObject
	{
		GENERATE_CLASS_TYPE_INFO( GameObject )

	public:
		virtual void Initialize( GameLogic& gameLogic, World& world );

		size_t GetId() const { return m_id; }
		void SetId( size_t id ) { m_id = id; }

		LOGIC_DLL void SetPosition( const Vector& translation );
		LOGIC_DLL void SetScale3D( const Vector& scale3D );
		LOGIC_DLL void SetRotation( const Quaternion& rotation );

		void SetRelativePosition( const Vector& translation );
		void SetRelativeScale3D( const Vector& scale3D );
		void SetRelativeRotation( const Quaternion& rotation );

		LOGIC_DLL const Vector& GetPosition() const;
		LOGIC_DLL const Vector& GetScale3D() const;
		LOGIC_DLL const Quaternion& GetRotation() const;

		const Vector& GetRelativePosition() const;
		const Vector& GetRelativeScale3D() const;
		const Quaternion& GetRelativeRotation() const;

		Vector GetForwardVector() const;
		Vector GetRightVector() const;
		Vector GetUpVector() const;

		LOGIC_DLL void SetTransform( const Transform& transform );
		LOGIC_DLL const Transform& GetTransform() const;

		const Matrix& GetTransformMatrix();
		const Matrix& GetInvTransformMatrix();

		virtual void Think( float elapsedTime );

		void SetName( const std::string& name ) { m_name = name; }
		const std::string& GetName() const { return m_name; }

		GameobjectProperty GetProperty() const { return m_property; }
		void AddProperty( GameobjectProperty property ) { m_property |= property; }
		void RemoveProperty( GameobjectProperty property ) { m_property &= ~property; }

		bool WillRemove() const { return HasAnyFlags( m_property, GameobjectProperty::RemoveMe ); }

		void LoadProperty( const json::Value& json );

		SceneComponent* GetRootComponent() const { return m_rootComponent; }
		void SetRootComponent( SceneComponent* component );
		void RemoveComponent( const Component* component );

		template <typename T>
		void GetComponents( std::vector<T*>& outComponents )
		{
			outComponents.reserve( m_components.size() );

			for ( std::unique_ptr<Component>& component : m_components )
			{
				if ( auto concrete = Cast<T>( component.get() ) )
				{
					outComponents.emplace_back( concrete );
				}
			}
		}

		void SetInputController( InputController* inputController );

		InputComponent* GetInputComponent();
		void InitializeInputComponent();

		World* GetWorld() const { return m_pWorld; }

		LOGIC_DLL void SetSelected( bool selected );
		bool IsSelected() const { return m_selected; }

		GameObject();
		virtual ~GameObject();

	protected:
		virtual void SetupInputComponent();

		template <typename T>
		T* CreateComponent( GameObject& gameObject, const char* name )
		{
			return new T( &gameObject, name );
		}

		template <typename T>
		T* GetComponent()
		{
			for ( std::unique_ptr<Component>& component : m_components )
			{
				if ( auto concrete = Cast<T>( component.get() ) )
				{
					return concrete;
				}
			}

			return nullptr;
		}

		Component* GetComponent( const Name& name );

	private:
		void RegisterThinkFunction();
		void UnRegisterThinkFunction();

		size_t m_id = std::numeric_limits<size_t>::max();
		World* m_pWorld = nullptr;

		std::string m_name;

		GameobjectProperty m_property = GameobjectProperty::None;

		std::vector<std::unique_ptr<Component>> m_components;
		std::map<Name, Component*> m_componentMap;

		bool m_selected = false;

	protected:
		GameObjectThinkFunction m_think;

		SceneComponent* m_rootComponent = nullptr;

		InputComponent* m_inputComponent = nullptr;
		InputController* m_inputController = nullptr;

		friend Component;
	};

	void RemoveObject( GameObject& object );
}
