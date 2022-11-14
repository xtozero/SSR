#pragma once

#include "ICommandList.h"

#include "GuideTypes.h"
#include "Memory/InlineMemoryAllocator.h"
#include "SizedTypes.h"

#include <memory>
#include <vector>

namespace agl
{
	class GraphicsCommandListsBase
	{
	public:
		IGraphicsCommandList* GetCommandList( uint32 index = 0 );
		virtual Owner<IGraphicsCommandList*> CreateCommandList() = 0;

		virtual void Prepare() = 0;
		virtual void Commit() = 0;

		virtual ~GraphicsCommandListsBase();

	protected:
		std::vector<IGraphicsCommandList*, InlineAllocator<IGraphicsCommandList*, 1>> m_commandLists;
	};
}
