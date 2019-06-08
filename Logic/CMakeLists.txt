add_definitions(-DLOGIC_EXPORTS=1)
add_definitions(-DUNICODE=1)
add_definitions(-D_UNICODE=1)

set (PRECOMPILED
	"Source/stdafx.cpp"
	"Source/stdafx.h"
	"Source/targetver.h")
	
set (CORE_SOURCE
	"Source/Private/Core/DebugConsole.cpp"
	"Source/Private/Core/dllmain.cpp"
	"Source/Private/Core/GameLogic.cpp"
	"Source/Private/Core/Logic.cpp"
	"Source/Private/Core/Timer.cpp"
	"Source/Public/Core/DebugConsole.h"
	"Source/Public/Core/GameLogic.h"
	"Source/Public/Core/IListener.h"
	"Source/Public/Core/ILogic.h"
	"Source/Public/Core/Timer.h"
	"Source/Public/Core/UtilWindowInfo.h")
	
set (GAME_OBJECT_SOURCE
	"Source/Private/GameObject/Camera.cpp"
	"Source/Private/GameObject/CameraManager.cpp"
	"Source/Private/GameObject/DisplayShaderResourceHelper.cpp"
	"Source/Private/GameObject/GameObject.cpp"
	"Source/Private/GameObject/GameObjectFactory.cpp"
	"Source/Private/GameObject/Light.cpp"
	"Source/Private/GameObject/LightManager.cpp"
	"Source/Private/GameObject/PickingManager.cpp"
	"Source/Private/GameObject/SkyBox.cpp"
	"Source/Public/GameObject/Camera.h"
	"Source/Public/GameObject/CameraManager.h"
	"Source/Public/GameObject/DisplayShaderResourceHelper.h"
	"Source/Public/GameObject/GameObject.h"
	"Source/Public/GameObject/GameObjectFactory.h"
	"Source/Public/GameObject/GameObjectProperty.h"
	"Source/Public/GameObject/Light.h"
	"Source/Public/GameObject/LightManager.h"
	"Source/Public/GameObject/PickingManager.h"
	"Source/Public/GameObject/SkyBox.h")
	
set (PLAYER_OBJECT_SOURCE
	"Source/Private/GameObject/Player.cpp"
	"Source/Public/GameObject/Player.h")
	
set (TEST_SCENE_OBJECT_SOURCE
	"Source/Private/GameObject/BallProjectile.cpp"
	"Source/Public/GameObject/BallProjectile.h")
	
set (MODEL_SOURCE
	"Source/Private/Model/BaseMesh.cpp"
	"Source/Private/Model/DebugMesh.cpp"
	"Source/Private/Model/ModelBuilder.cpp"
	"Source/Private/Model/ModelManager.cpp"
	"Source/Private/Model/ObjMesh.cpp"
	"Source/Private/Model/ObjMeshLoader.cpp"
	"Source/Private/Model/PlyMesh.cpp"
	"Source/Private/Model/PlyMeshLoader.cpp"
	"Source/Public/Model/BaseMesh.h"
	"Source/Public/Model/CommonMeshDefine.h"
	"Source/Public/Model/DebugMesh.h"
	"Source/Public/Model/IMesh.h"
	"Source/Public/Model/IModelBuilder.h"
	"Source/Public/Model/IModelLoader.h"
	"Source/Public/Model/MeshBuildHelper.h"
	"Source/Public/Model/ModelBuilder.h"
	"Source/Public/Model/ModelManager.h"
	"Source/Public/Model/ObjMesh.h"
	"Source/Public/Model/ObjMeshLoader.h"
	"Source/Public/Model/PlyMesh.h"
	"Source/Public/Model/PlyMeshLoader.h"
	"Source/Public/Model/Surface.h")

set (PHYSICS_SOURCE
	"Source/Private/Physics/Aaboundingbox.cpp"
	"Source/Private/Physics/Body.cpp"
	"Source/Private/Physics/BoundingCone.cpp"
	"Source/Private/Physics/BoundingSphere.cpp"
	"Source/Private/Physics/CollisionUtil.cpp"
	"Source/Private/Physics/CollideNarrow.cpp"
	"Source/Private/Physics/Contacts.cpp"
	"Source/Private/Physics/ForceGenerator.cpp"
	"Source/Private/Physics/OrientedBoundingBox.cpp"
	"Source/Private/Physics/Particle.cpp"
	"Source/Private/Physics/ParticleContacts.cpp"
	"Source/Private/Physics/ParticleForceGenerator.cpp"
	"Source/Private/Physics/ParticleLinks.cpp"
	"Source/Private/Physics/ParticleWorld.cpp"
	"Source/Private/Physics/Ray.cpp"
	"Source/Private/Physics/ColliderManager.cpp"
	"Source/Private/Physics/World.cpp"
	"Source/Public/Physics/Aaboundingbox.h"
	"Source/Public/Physics/Body.h"
	"Source/Public/Physics/BoundingCone.h"
	"Source/Public/Physics/BoundingSphere.h"
	"Source/Public/Physics/CollideBroad.h"
	"Source/Public/Physics/CollisionUtil.h"
	"Source/Public/Physics/CollideNarrow.h"
	"Source/Public/Physics/Contacts.h"
	"Source/Public/Physics/ForceGenerator.h"
	"Source/Public/Physics/Frustum.h"
	"Source/Public/Physics/ICollider.h"
	"Source/Public/Physics/Movement.h"
	"Source/Public/Physics/OrientedBoundingBox.h"
	"Source/Public/Physics/Particle.h"
	"Source/PUblic/Physics/ParticleContacts.h"
	"Source/Public/Physics/ParticleForceGenerator.h"
	"Source/Public/Physics/ParticleLinks.h"
	"Source/Public/Physics/ParticleWorld.h"
	"Source/Public/Physics/Ray.h"
	"Source/Public/Physics/ColliderManager.h"
	"Source/Public/Physics/World.h")

set (SCENE_SOURCE
	"Source/Private/Scene/AtmosphereScattering.cpp"
	"Source/Private/Scene/DebugOverlayManager.cpp"
	"Source/Private/Scene/DynamicBuffer.cpp"
	"Source/Private/Scene/RenderView.cpp"
	"Source/Private/Scene/SceneLoader.cpp"
	"Source/Private/Scene/ScreenBlurManager.cpp"
	"Source/Private/Scene/ShadowManager.cpp"
	"Source/Private/Scene/SSRManager.cpp"
	"Source/Public/Scene/AtmosphereScattering.h"
	"Source/Public/Scene/ConstantBufferDefine.h"
	"Source/Public/Scene/DebugOverlayManager.h"
	"Source/Public/Scene/DynamicBuffer.h"
	"Source/Public/Scene/INotifyGraphicsDevice.h"
	"Source/Public/Scene/RenderView.h"
	"Source/Public/Scene/SceneLoader.h"
	"Source/Public/Scene/ScreenBlurManager.h"
	"Source/Public/Scene/ShadowManager.h"
	"Source/Public/Scene/SSRManager.h")
	
set (UI_SOURCE
	"Source/Private/UI/ImUI.cpp"
	"Source/Private/UI/ImUI_Draw.cpp"
	"Source/Public/UI/ImUI.h")
	
set (UNIT_TEST_SOURCE
	"Source/Private/Scene/ComputeShaderUnitTest.cpp"
	"Source/Public/Scene/ComputeShaderUnitTest.h")
	
set (USER_INPUT_SOURCE
	"Source/Private/UserInput/MouseController.cpp"
	"Source/Public/UserInput/MouseController.h")
	
source_group("GameObject" FILES ${GAME_OBJECT_SOURCE})
source_group("GameObject\\Player" FILES ${PLAYER_OBJECT_SOURCE})
source_group("GameObject\\TestScene" FILES ${TEST_SCENE_OBJECT_SOURCE})
source_group("Model" FILES ${MODEL_SOURCE})
source_group("Physics" FILES ${PHYSICS_SOURCE})
source_group("Render" FILES ${SCENE_SOURCE})
source_group("UI" FILES ${UI_SOURCE})
source_group("UintTest" FILES ${UNIT_TEST_SOURCE})
source_group("UserInput" FILES ${USER_INPUT_SOURCE})

include_directories(./Source)
include_directories(./Source/Public)
include_directories(../Shared/Public)
include_directories(../Engine/Source/Public)
include_directories(../RenderCore/Source/Public)
include_directories(../RenderCore/Shader)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib)

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
							${USER_INPUT_SOURCE})
							
add_custom_command(TARGET Logic POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_BINARY_DIR}/\$\(Configuration\)/Logic.lib" "${CMAKE_CURRENT_SOURCE_DIR}/../lib/Logic.lib"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_BINARY_DIR}/\$\(Configuration\)/Logic.dll" "${CMAKE_CURRENT_SOURCE_DIR}/../bin/Logic.dll")