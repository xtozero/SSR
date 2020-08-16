add_definitions(-DLOGIC_EXPORTS=1)
add_definitions(-DUNICODE=1)
add_definitions(-D_UNICODE=1)
add_definitions(-DNOMINMAX=1)

set (PRECOMPILED
	"Private/stdafx.cpp"
	"Public/stdafx.h"
	"Public/targetver.h")
	
set (CORE_SOURCE
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
	
set (GAME_OBJECT_SOURCE
	"Private/GameObject/Camera.cpp"
	"Private/GameObject/CameraManager.cpp"
	"Private/GameObject/DisplayShaderResourceHelper.cpp"
	"Private/GameObject/GameObject.cpp"
	"Private/GameObject/GameObjectFactory.cpp"
	"Private/GameObject/Light.cpp"
	"Private/GameObject/LightManager.cpp"
	"Private/GameObject/PickingManager.cpp"
	"Private/GameObject/SkyBox.cpp"
	"Public/GameObject/Camera.h"
	"Public/GameObject/CameraManager.h"
	"Public/GameObject/DisplayShaderResourceHelper.h"
	"Public/GameObject/GameObject.h"
	"Public/GameObject/GameObjectFactory.h"
	"Public/GameObject/GameObjectProperty.h"
	"Public/GameObject/Light.h"
	"Public/GameObject/LightManager.h"
	"Public/GameObject/PickingManager.h"
	"Public/GameObject/SkyBox.h")
	
set (PLAYER_OBJECT_SOURCE
	"Private/GameObject/Player.cpp"
	"Public/GameObject/Player.h")
	
set (TEST_SCENE_OBJECT_SOURCE
	"Private/GameObject/BallProjectile.cpp"
	"Public/GameObject/BallProjectile.h")
	
set (MODEL_SOURCE
	"Private/Model/BaseMesh.cpp"
	"Private/Model/DebugMesh.cpp"
	"Private/Model/ModelBuilder.cpp"
	"Private/Model/ModelManager.cpp"
	"Private/Model/ObjMesh.cpp"
	"Private/Model/ObjMeshLoader.cpp"
	"Private/Model/PlyMesh.cpp"
	"Private/Model/PlyMeshLoader.cpp"
	"Public/Model/BaseMesh.h"
	"Public/Model/CommonMeshDefine.h"
	"Public/Model/DebugMesh.h"
	"Public/Model/IMesh.h"
	"Public/Model/IModelBuilder.h"
	"Public/Model/IModelLoader.h"
	"Public/Model/MeshBuildHelper.h"
	"Public/Model/ModelBuilder.h"
	"Public/Model/ModelManager.h"
	"Public/Model/ObjMesh.h"
	"Public/Model/ObjMeshLoader.h"
	"Public/Model/PlyMesh.h"
	"Public/Model/PlyMeshLoader.h"
	"Public/Model/Surface.h")

set (PHYSICS_SOURCE
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

set (SCENE_SOURCE
	"Private/Scene/AtmosphereScattering.cpp"
	"Private/Scene/DebugOverlayManager.cpp"
	"Private/Scene/DynamicBuffer.cpp"
	"Private/Scene/RenderView.cpp"
	"Private/Scene/ScreenBlurManager.cpp"
	"Private/Scene/ShadowManager.cpp"
	"Private/Scene/SSRManager.cpp"
	"Public/Scene/AtmosphereScattering.h"
	"Public/Scene/ConstantBufferDefine.h"
	"Public/Scene/DebugOverlayManager.h"
	"Public/Scene/DynamicBuffer.h"
	"Public/Scene/INotifyGraphicsDevice.h"
	"Public/Scene/RenderView.h"
	"Public/Scene/ScreenBlurManager.h"
	"Public/Scene/ShadowManager.h"
	"Public/Scene/SSRManager.h")
	
set (UI_SOURCE
	"Private/UI/ImUI.cpp"
	"Private/UI/ImUI_Draw.cpp"
	"Public/UI/ImUI.h")
	
set (UNIT_TEST_SOURCE
	"Private/Scene/ComputeShaderUnitTest.cpp"
	"Public/Scene/ComputeShaderUnitTest.h")
	
set (USER_INPUT_SOURCE
	"Private/UserInput/MouseController.cpp"
	"Public/UserInput/MouseController.h")
	
set (WORLD_SOURCE
	"Private/World/World.cpp"
	"Private/World/WorldLoader.cpp"
	"Public/World/World.h"
	"Public/World/WorldLoader.h")
	
source_group("GameObject" FILES ${GAME_OBJECT_SOURCE})
source_group("GameObject\\Player" FILES ${PLAYER_OBJECT_SOURCE})
source_group("GameObject\\TestScene" FILES ${TEST_SCENE_OBJECT_SOURCE})
source_group("Model" FILES ${MODEL_SOURCE})
source_group("Physics" FILES ${PHYSICS_SOURCE})
source_group("Render" FILES ${SCENE_SOURCE})
source_group("UI" FILES ${UI_SOURCE})
source_group("UintTest" FILES ${UNIT_TEST_SOURCE})
source_group("UserInput" FILES ${USER_INPUT_SOURCE})
source_group("World" FILES ${WORLD_SOURCE})

include_directories(./Public)
include_directories(../Core/Public)
include_directories(../Shared/Public)
include_directories(../Engine/Public)
include_directories(../RenderCore/Public)
include_directories(../Shaders/Public)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
link_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_library(Logic SHARED 	${PRECOMPILED}
							${CORE_SOURCE}
							${GAME_OBJECT_SOURCE}
							${PLAYER_OBJECT_SOURCE}
							${TEST_SCENE_OBJECT_SOURCE}
							${MODEL_SOURCE}
							${PHYSICS_SOURCE}
							${SCENE_SOURCE}
							${UI_SOURCE}
							${UNIT_TEST_SOURCE}
							${USER_INPUT_SOURCE}
							${WORLD_SOURCE})
							
target_link_libraries(Logic RenderCore)
							
add_custom_command(TARGET Logic POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/\$\(Configuration\)/Logic.dll" "${CMAKE_SOURCE_DIR}/Program/Binaries/Logic.dll")