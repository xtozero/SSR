add_definitions(-DCORE_EXPORTS=1)
add_definitions(-DUNICODE=1)
add_definitions(-D_UNICODE=1)
add_definitions(-DNOMINMAX=1)

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
	Public/Filesystem/FileSystem.h)

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
	Public/Math/CXMFloat.h
	Public/Math/Util.h)

set (MEMORY
	Public/Memory/FixedBlockMemoryPool.h
	Public/Memory/MemoryPool.h)

set (MULTITHREAD
	Private/Multithread/TaskScheduler.cpp
	Public/Multithread/TaskScheduler.h)

set (SERIALIZATION
	Public/Serialization/Archive.h
	Public/Serialization/ArchiveUtility.h)

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
source_group("Serialization" FILES ${SERIALIZATION})

add_library(Core STATIC	${COLLECTIONS}
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
						${SERIALIZATION})

target_include_directories(Core
PUBLIC
	./Public/
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
)
							
