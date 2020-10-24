#include "stdafx.h"
#include "Scene/SceneConstantBuffers.h"

#include "Core/InterfaceFactories.h"
#include "MultiThread/EngineTaskScheduler.h"
#include "IAga.h"

#include <assert.h>

void SceneConstantBuffers::Initialize( )
{
	assert( IsInRenderThread() );

	m_viewConstantBuffer = TypeConstatBuffer<ViewConstantBufferParameters>::Create( );
}
