add_definitions(-DLOGIC_EXPORTS=1)
add_definitions(-DUNICODE=1)
add_definitions(-D_UNICODE=1)
add_definitions(-DNOMINMAX=1)

set (ASSET_LOADER
	"Private/AssetLoader/AssetFactory.cpp"
	"Private/AssetLoader/AssetLoader.cpp"
	"Public/AssetLoader/AssetFactory.h"
	"Public/AssetLoader/AssetLoader.h"
	"Public/AssetLoader/IAsyncLoadableAsset.h"
)
	
set (COMPOENETS
	"Private/Components/CameraComponent.cpp"
	"Private/Components/Component.cpp"
	"Private/Components/PrimitiveComponent.cpp"
	"Private/Components/SceneComponent.cpp"
	"Private/Components/StaticMeshComponent.cpp"
	"Public/Components/CameraComponent.h"
	"Public/Components/Component.h"
	"Public/Components/PrimitiveComponent.h"
	"Public/Components/SceneComponent.h"
	"Public/Components/StaticMeshComponent.h")
	
set (CORE
	"Private/Core/DebugConsole.cpp"
	"Private/Core/dllmain.cpp"
	"Private/Core/GameLogic.cpp"
	"Private/Core/Logic.cpp"
	"Private/Core/ModuleInit.cpp"
	"Private/Core/Timer.cpp"
	"Public/Core/DebugConsole.h"
	"Public/Core/GameLogic.h"
	"Public/Core/IListener.h"
	"Public/Core/ILogic.h"
	"Public/Core/Timer.h"
	"Public/Core/UtilWindowInfo.h")
	
set (GAME_OBJECT
	"Private/GameObject/DisplayShaderResourceHelper.cpp"
	"Private/GameObject/GameClientViewport.cpp"
	"Private/GameObject/GameObject.cpp"
	"Private/GameObject/GameObjectFactory.cpp"
	"Private/GameObject/Light.cpp"
	"Private/GameObject/LightManager.cpp"
	"Private/GameObject/PickingManager.cpp"
	"Private/GameObject/SkyBox.cpp"
	"Private/GameObject/StaticMeshGameObject.cpp"
	"Public/GameObject/DisplayShaderResourceHelper.h"
	"Public/GameObject/GameClientViewport.h"
	"Public/GameObject/GameObject.h"
	"Public/GameObject/GameObjectFactory.h"
	"Public/GameObject/GameObjectProperty.h"
	"Public/GameObject/Light.h"
	"Public/GameObject/LightManager.h"
	"Public/GameObject/PickingManager.h"
	"Public/GameObject/SkyBox.h"
	"Public/GameObject/StaticMeshGameObject.h")

set (MATERIAL
	"Private/Material/Material.cpp"
	"Public/Material/Material.h")
	
set (MODEL
	"Private/Model/BaseMesh.cpp"
	"Private/Model/StaticMesh.cpp"
	"Private/Model/DebugMesh.cpp"
	"Private/Model/ModelBuilder.cpp"
	"Private/Model/ModelManager.cpp"
	"Private/Model/PlyMesh.cpp"
	"Private/Model/PlyMeshLoader.cpp"
	"Public/Model/BaseMesh.h"
	"Public/Model/StaticMesh.h"
	"Public/Model/CommonMeshDefine.h"
	"Public/Model/DebugMesh.h"
	"Public/Model/IModelBuilder.h"
	"Public/Model/IModelLoader.h"
	"Public/Model/MeshBuildHelper.h"
	"Public/Model/MeshDescription.h"
	"Public/Model/ModelBuilder.h"
	"Public/Model/ModelManager.h"
	"Public/Model/PlyMesh.h"
	"Public/Model/PlyMeshLoader.h"
	"Public/Model/Surface.h")

set (PHYSICS
	"Private/Physics/Aaboundingbox.cpp"
	"Private/Physics/Body.cpp"
	"Private/Physics/BoundingCone.cpp"
	"Private/Physics/BoundingSphere.cpp"
	"Private/Physics/CollisionUtil.cpp"
	"Private/Physics/CollideNarrow.cpp"
	"Private/Physics/Contacts.cpp"
	"Private/Physics/ForceGenerator.cpp"
	"Private/Physics/OrientedBoundingBox.cpp"
	"Private/Physics/Particle.cpp"
	"Private/Physics/ParticleContacts.cpp"
	"Private/Physics/ParticleForceGenerator.cpp"
	"Private/Physics/ParticleLinks.cpp"
	"Private/Physics/ParticleWorld.cpp"
	"Private/Physics/Ray.cpp"
	"Private/Physics/ColliderManager.cpp"
	"Public/Physics/Aaboundingbox.h"
	"Public/Physics/Body.h"
	"Public/Physics/BoundingCone.h"
	"Public/Physics/BoundingSphere.h"
	"Public/Physics/CollideBroad.h"
	"Public/Physics/CollisionUtil.h"
	"Public/Physics/CollideNarrow.h"
	"Public/Physics/Contacts.h"
	"Public/Physics/ForceGenerator.h"
	"Public/Physics/Frustum.h"
	"Public/Physics/ICollider.h"
	"Public/Physics/Movement.h"
	"Public/Physics/OrientedBoundingBox.h"
	"Public/Physics/Particle.h"
	"Public/Physics/ParticleContacts.h"
	"Public/Physics/ParticleForceGenerator.h"
	"Public/Physics/ParticleLinks.h"
	"Public/Physics/ParticleWorld.h"
	"Public/Physics/Ray.h"
	"Public/Physics/ColliderManager.h")

set (PLAYER_OBJECT
	"Private/GameObject/Player.cpp"
	"Public/GameObject/Player.h")

set (PRECOMPILED
	"Private/stdafx.cpp"
	"Public/stdafx.h"
	"Public/targetver.h")

set (RENDERING
	"Private/Rendering/RenderOption.cpp"
	"Private/Rendering/Shader.cpp"
	"Public/Rendering/RenderOption.h"
	"Public/Rendering/Shader.h"
)

set (SCENE
	"Private/Scene/AtmosphereScattering.cpp"
	"Private/Scene/DebugOverlayManager.cpp"
	"Private/Scene/DynamicBuffer.cpp"
	"Private/Scene/ScreenBlurManager.cpp"
	"Private/Scene/ShadowManager.cpp"
	"Private/Scene/SSRManager.cpp"
	"Public/Scene/AtmosphereScattering.h"
	"Public/Scene/ConstantBufferDefine.h"
	"Public/Scene/DebugOverlayManager.h"
	"Public/Scene/DynamicBuffer.h"
	"Public/Scene/INotifyGraphicsDevice.h"
	"Public/Scene/ScreenBlurManager.h"
	"Public/Scene/ShadowManager.h"
	"Public/Scene/SSRManager.h")
	
set (TEST_SCENE_OBJECT
	"Private/GameObject/BallProjectile.cpp"
	"Public/GameObject/BallProjectile.h")
	
set (UI
	"Private/UI/ImUI.cpp"
	"Private/UI/ImUI_Draw.cpp"
	"Public/UI/ImUI.h")
	
set (UNIT_TEST
	"Private/Scene/ComputeShaderUnitTest.cpp"
	"Public/Scene/ComputeShaderUnitTest.h")
	
set (USER_INPUT
	"Private/UserInput/MouseController.cpp"
	"Public/UserInput/MouseController.h")
	
set (WORLD
	"Private/World/World.cpp"
	"Private/World/WorldLoader.cpp"
	"Public/World/World.h"
	"Public/World/WorldLoader.h")

source_group("AssetLoader" FILES ${ASSET_LOADER} )	
source_group("Components" FILES ${COMPOENETS})
source_group("Core" FILES ${CORE})
source_group("GameObject" FILES ${GAME_OBJECT})
source_group("GameObject\\Player" FILES ${PLAYER_OBJECT})
source_group("GameObject\\TestScene" FILES ${TEST_SCENE_OBJECT})
source_group("Material" FILES ${MATERIAL})
source_group("Model" FILES ${MODEL})
source_group("Physics" FILES ${PHYSICS})
source_group("Render" FILES ${SCENE}
							${RENDERING})
source_group("UI" FILES ${UI})
source_group("UintTest" FILES ${UNIT_TEST})
source_group("UserInput" FILES ${USER_INPUT})
source_group("World" FILES ${WORLD})

include_directories(./Public)
include_directories(../Aga/Public/Common)
include_directories(../Core/Public)
include_directories(../Shared/Public)
include_directories(../Engine/Public)
include_directories(../RenderCore/Public)
include_directories(../Shaders/Public)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
link_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_library(Logic SHARED 	${ASSET_LOADER}
							${COMPOENETS}
							${CORE}
							${GAME_OBJECT}
							${MATERIAL}
							${MODEL}
							${PHYSICS}
							${PLAYER_OBJECT}
							${PRECOMPILED}
							${RENDERING}
							${SCENE}
							${TEST_SCENE_OBJECT}
							${UI}
							${UNIT_TEST}
							${USER_INPUT}
							${WORLD})
							
target_link_libraries(Logic RenderCore)
							
add_custom_command(TARGET Logic POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/\$\(Configuration\)/Logic.dll" "${CMAKE_SOURCE_DIR}/Program/Binaries/Logic.dll")