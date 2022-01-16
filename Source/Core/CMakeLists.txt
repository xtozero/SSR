add_definitions(-DCORE_EXPORTS=1)
add_definitions(-DUNICODE=1)
add_definitions(-D_UNICODE=1)
add_definitions(-DNOMINMAX=1)

set (ASSET
	Private/Asset/AssetFactory.cpp
	Private/Asset/AssetLoader.cpp
	Public/Asset/AssetFactory.h
	Public/Asset/IAssetLoader.h
	Public/Asset/IAssetLoader.h
	Public/Asset/IAsyncLoadableAsset.h)

set (COLLECTIONS
	Public/Collections/BitArray.h
	Public/Collections/SparseArray.h)

set (DATA_STRUCTURE
	Private/DataStructure/EnumStringMap.cpp
	Public/DataStructure/DoubleLinkedList.h
	Public/DataStructure/EnumStringMap.h
	Public/DataStructure/SingleLInkedList.h)
	
set (DATA_TYPE
	public/DataType/GuideTypes.h
	Public/DataType/SizedTypes.h)

set (DEBUG_TOOL
	Public/DebugTool/DebugUtil.h)

set (DELEGATE
	Public/Delegate/Delegate.h)
	
set (FILESYSTEM
	Private/Filesystem/FileSystem.cpp
	Public/Filesystem/FileSystem.h
	Public/Filesystem/FileSystemCore.h)

set (HASH
	Public/Hash/Crc32Hash.h
	Public/Hash/HashUtil.h)

set (JSON
	Public/JSON/json.hpp)

set (LIBRARY_TOOL
	Private/LibraryTool/Interfacefactories.cpp
	Public/LibraryTool/common.h
	Public/LibraryTool/Interfacefactories.h)

set (Math
	Private/Math/Matrix.cpp
	Private/Math/Matrix3X3.cpp
	Private/Math/Plane.cpp
	Private/Math/Quaternion.cpp
	Private/Math/Vector.cpp
	Private/Math/Vector2.cpp
	Private/Math/Vector4.cpp
	Private/Math/XMMatrix.cpp
	Private/Math/XMVector.cpp
	Public/Math/Matrix.h
	Public/Math/Matrix3X3.h
	Public/Math/Plane.h
	Public/Math/Quaternion.h
	Public/Math/Rect.h
	Public/Math/TransformationMatrix.h
	Public/Math/Util.h
	Public/Math/Vector.h
	Public/Math/Vector2.h
	Public/Math/Vector4.h
	Public/Math/XMMatrix.h
	Public/Math/XMVector.h)

set (MEMORY
	Private/Memory/StackMemoryAllocator.cpp
	Public/Memory/FixedBlockMemoryPool.h
	Public/Memory/MemoryPool.h
	Public/Memory/StackMemoryAllocator.h)

set (MULTITHREAD
	Private/Multithread/TaskScheduler.cpp
	Private/Multithread/TaskSchedulerCore.cpp
	Public/Multithread/TaskScheduler.h
	Public/Multithread/TaskSchedulerCore.h)

set (PHYSICS
	Private/Physics/Aaboundingbox.cpp
	Private/Physics/Body.cpp
	Private/Physics/BoundingCone.cpp
	Private/Physics/BoundingSphere.cpp
	Private/Physics/BoxSphereBounds.cpp
	Private/Physics/CollisionUtil.cpp
	Private/Physics/CollideNarrow.cpp
	Private/Physics/Contacts.cpp
	Private/Physics/ForceGenerator.cpp
	Private/Physics/OrientedBoundingBox.cpp
	Private/Physics/Particle.cpp
	Private/Physics/ParticleContacts.cpp
	Private/Physics/ParticleForceGenerator.cpp
	Private/Physics/ParticleLinks.cpp
	Private/Physics/ParticleWorld.cpp
	Private/Physics/ColliderManager.cpp
	Public/Physics/Aaboundingbox.h
	Public/Physics/Body.h
	Public/Physics/BoundingCone.h
	Public/Physics/BoundingSphere.h
	Public/Physics/BoxSphereBounds.h
	Public/Physics/CollideBroad.h
	Public/Physics/CollisionUtil.h
	Public/Physics/CollideNarrow.h
	Public/Physics/Contacts.h
	Public/Physics/ForceGenerator.h
	Public/Physics/Frustum.h
	Public/Physics/ICollider.h
	Public/Physics/Movement.h
	Public/Physics/OrientedBoundingBox.h
	Public/Physics/Particle.h
	Public/Physics/ParticleContacts.h
	Public/Physics/ParticleForceGenerator.h
	Public/Physics/ParticleLinks.h
	Public/Physics/ParticleWorld.h
	Public/Physics/Ray.h
	Public/Physics/ColliderManager.h)

set (SERIALIZATION
	Public/Serialization/Archive.h
	Public/Serialization/ArchiveUtility.h)

source_group("Asset" FILES ${ASSET})
source_group("Collections" FILES ${COLLECTIONS} )		
source_group("DataStructure" FILES ${DATA_STRUCTURE})
source_group("DataType" FILES ${DATA_TYPE})
source_group("DebugTool" FILES ${DEBUG_TOOL})
source_group("Delegate" FILES ${DELEGATE})
source_group("Filesystem" FILES ${FILESYSTEM})
source_group("Hash" FILES ${HASH})
source_group("Json" FILES ${JSON})
source_group("LibraryTool" FILES ${LIBRARY_TOOL})
source_group("Math" FILES ${Math})
source_group("Memory" FILES ${MEMORY})
source_group("MultiThread" FILES ${MULTITHREAD})
source_group("Physics" FILES ${PHYSICS})
source_group("Serialization" FILES ${SERIALIZATION})

add_library(Core STATIC	${ASSET}
						${COLLECTIONS}
						${DATA_STRUCTURE}
						${DATA_TYPE}
						${DEBUG_TOOL}
						${DELEGATE}
						${FILESYSTEM}
						${HASH}
						${JSON}
						${LIBRARY_TOOL}
						${Math}
						${MEMORY}
						${MULTITHREAD}
						${SERIALIZATION}
						${PHYSICS})

target_include_directories(Core
PUBLIC
	./Public/
	./Public/Asset
	./Public/Collections
	./Public/DataStructure
	./Public/DataType
	./Public/DebugTool
	./Public/Delegate
	./Public/Filesystem
	./Public/Hash
	./Public/LibraryTool
	./Public/Memory
	./Public/Multithread
	./Public/Serialization
PRIVATE
	./Public/Physics
	./Public/Math
)
							
