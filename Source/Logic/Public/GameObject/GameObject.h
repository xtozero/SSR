#pragma once

#include "Core/ThinkFunction.h"
#include "GameObjectProperty.h"
#include "Math/Matrix.h"
#include "Math/Quaternion.h"
#include "Math/Vector.h"
#include "NameTypes.h"
#include "Reflection.h"
#include "Scene/INotifyGraphicsDevice.h"

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
	class BaseMesh;
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

	class CDebugOverlayManager;
	class CGameLogic;
	class CGameObject;
	class Component;
	class InputComponent;
	class InputController;
	class SceneComponent;
	class World;

	class CGameObject : IGraphicsDeviceNotify
	{
		GENERATE_CLASS_TYPE_INFO( CGameObject )

	public:
		virtual void OnDeviceRestore( CGameLogic& gameLogic ) override;
		virtual void Initialize( CGameLogic& gameLogic, World& world );

		size_t GetID() const { return m_id; }
		void SetID( size_t id ) { m_id = id; }

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

		const Matrix& GetTransformMatrix();
		const Matrix& GetInvTransformMatrix();

		virtual void Think( float elapsedTime );

		void SetName( const std::string& name ) { m_name = name; }
		const std::string& GetName() const { return m_name; }
		void SetMaterialName( const std::string& pMaterialName );

		bool IsPicked() const { return m_isPicked; }
		void SetPicked( bool isPicked ) { m_isPicked = isPicked; }

		GameobjectProperty GetProperty() const { return m_property; }
		void AddProperty( GameobjectProperty property ) { m_property |= property; }
		void RemoveProperty( GameobjectProperty property ) { m_property &= ~property; }

		bool WillRemove() const { return HasAnyFlags( m_property, GameobjectProperty::RemoveMe ); }

		void LoadProperty( const json::Value& json );

		virtual bool IgnorePicking() const { return false; }

		SceneComponent* GetRootComponent() const { return m_rootComponent; }
		void SetRootComponent( SceneComponent* component );
		void RemoveComponent( const Component* component );

		void SetInputController( InputController* inputController );

		InputComponent* GetInputComponent();
		void InitializeInputComponent();

		World* GetWorld() const { return m_pWorld; }

		CGameObject();
		virtual ~CGameObject();

	protected:
		virtual void SetupInputComponent();

		template <typename T>
		T* CreateComponent( CGameObject& gameObject, const char* name )
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
		std::string m_materialName;

		bool m_isPicked = false;

		GameobjectProperty m_property = GameobjectProperty::None;

		std::vector<std::unique_ptr<Component>> m_components;
		std::map<Name, Component*> m_componentMap;

	protected:
		GameObjectThinkFunction m_think;

		SceneComponent* m_rootComponent = nullptr;

		InputComponent* m_inputComponent = nullptr;
		InputController* m_inputController = nullptr;

		friend Component;
	};

	void RemoveObject( CGameObject& object );
}
