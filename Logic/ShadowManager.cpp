#include "stdafx.h"
#include "ShadowManager.h"

#include "../RenderCore/IRenderer.h"

extern IRenderer* gRenderer;

void CShadowManager::Init( )
{

}

void CShadowManager::SceneBegin( )
{
	//그림자 렌더링에 사용할 조명으로 View 행렬을 만들어 세팅

	//그림자 텍스쳐로 랜더 타겟 변경

	//그림자 렌더링 머티리얼로 전체 오브젝트를 랜더링
}

void CShadowManager::DrawScene( )
{
	//원래 뷰세팅 되돌림

	//백 버퍼로 랜더 타겟 변경
}

void CShadowManager::SceneEnd( )
{

}

CShadowManager::CShadowManager( )
{
}


CShadowManager::~CShadowManager( )
{
}
