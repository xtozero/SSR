#include "CommandLists.h"

namespace agl
{
	IGraphicsCommandList* GraphicsCommandListsBase::GetCommandList( uint32 index )
	{
		if ( m_commandLists.size() <= index )
		{
			m_commandLists.resize( index + 1 );
		}

		if ( m_commandLists[index] == nullptr )
		{
			m_commandLists[index] = CreateCommandList();
		}

		return m_commandLists[index];
	}

	GraphicsCommandListsBase::~GraphicsCommandListsBase()
	{
		for ( IGraphicsCommandList* commandList : m_commandLists )
		{
			delete commandList;
		}
	}
}
