#include "stdafx.h"
#include <tchar.h>
#include "DebugConsole.h"
#include "GameLogic.h"
#include "GameObject.h"
#include "../Shared/Util.h"
#include "../RenderCore/Direct3D11.h"
#include "../RenderCore/BaseMesh.h"
#include "../RenderCore/DebugMesh.h"
#include <ctime>
#include "Timer.h"

IRenderer* gRenderer = CreateDirect3D11Renderer ();

void CGameLogic::StartLogic ( void )
{
	//게임 로직 수행 전처리
}

void CGameLogic::ProcessLogic ( void )
{
	//게임 로직 수행
	FOR_EACH_VEC( m_gameObjects, iter )
	{
		( *iter )->Think( );
	}
}

void CGameLogic::EndLogic ( void )
{
	//게임 로직 수행 후처리
	m_mainCamera.UpdateToRenderer( gRenderer );
	SceneBegin( );
	DrawScene( );
	SceneEnd( );
}

bool CGameLogic::InitShaders( void )
{
	DebugMsg( "Start Initialize Shaders\n" );

	ON_FAIL_RETURN( gRenderer->CreateVertexShader( _T( "../Shader/vsTutorial.fx" ), "vs_4_0" ) );
	ON_FAIL_RETURN( gRenderer->CreatePixelShader( _T( "../Shader/psTutorial.fx" ), "ps_4_0" ) );

	DebugMsg( "Complete Initialize Shaders\n" );

	return true;
}

bool CGameLogic::LoadScene( void )
{
	m_gameObjects.erase( m_gameObjects.begin( ), m_gameObjects.end( ) );
	auto keyValue = m_sceneLoader.LoadSceneFromFile( _T( "../Script/TestScene.txt" ), m_gameObjects );

	if ( !keyValue )
	{
		return false;
	}

	InitCameraProperty( keyValue );

	return true;
}

void CGameLogic::SceneBegin( void )
{
	gRenderer->SceneBegin( );
}

void CGameLogic::DrawScene( void )
{
	FOR_EACH_VEC( m_gameObjects, object )
	{
		UpdateWorldMatrix( object->get( ) );
		( *object )->Render( );
	}
}

void CGameLogic::SceneEnd( void )
{
	gRenderer->SceneEnd( );
}

void CGameLogic::UpdateWorldMatrix( const CGameObject* object )
{
	if ( object )
	{
		gRenderer->UpdateWorldMatrix( object->GetTransformMatrix( ) );
	}
}

void CGameLogic::InitCameraProperty( std::shared_ptr<KeyValueGroup> keyValue )
{
	CKeyValueIterator finded = keyValue->FindKeyValue( _T( "Camera Pos" ) );

	if ( finded != nullptr )
	{
		std::vector<String> param;
		UTIL::Split( finded->GetString( ), param, ' ' );

		if ( param.size( ) == 3 )
		{
			m_mainCamera.SetOrigin( D3DXVECTOR3( _ttof( param[0].c_str( ) ),
				_ttof( param[1].c_str( ) ),
				_ttof( param[2].c_str( ) ) ) );
		}
	}
}

bool CGameLogic::Initialize ( HWND hwnd, UINT wndWidth, UINT wndHeight )
{
	srand( time( NULL ) );

	ON_FAIL_RETURN( gRenderer->InitializeRenderer( hwnd, wndWidth, wndHeight ) );
	ON_FAIL_RETURN( InitShaders( ) );
	ON_FAIL_RETURN( gRenderer->InitMaterial( ) );
	ON_FAIL_RETURN( LoadScene( ) );
	gRenderer->PushViewPort( 0.0f, 0.0f, static_cast<float>( wndWidth ), static_cast<float>( wndHeight ) );
	
	IRenderView* view = gRenderer->GetCurrentRenderView( );
	
	if ( view )
	{
		view->CreatePerspectiveFovLHMatrix( D3DXToRadian( 60 ),
											static_cast<float>( wndWidth ) / wndHeight,
											1.f,
											1500.f );
	}
	else
	{
		return false;
	}

	m_mouseController.AddListener( &m_mainCamera );

	return true;
}

void CGameLogic::UpdateLogic ( void )
{
	// 한 프레임의 시작 ElapsedTime 갱신
	CTimer::GetInstance ().Tick ();

	StartLogic ();
	ProcessLogic ();
	EndLogic ();
}

bool CGameLogic::HandleWindowMessage( const MSG& msg )
{
	switch ( msg.message )
	{
	case WM_KEYDOWN:
	case WM_KEYUP:
		HandleWIndowKeyInput( msg.message, msg.wParam, msg.lParam );
		return true;
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_MOUSEWHEEL:
		HandleWIndowMouseInput( msg.message, msg.wParam, msg.lParam );
		return true;
		break;
	default:
		//Message UnHandled;
		return false;
		break;
	}
}

void CGameLogic::HandleWIndowKeyInput( const int message, const WPARAM wParam, const LPARAM lParam )
{
	switch ( message )
	{
	case WM_KEYDOWN:
		{
					 TCHAR keyName[256] = { 0, };
					 GetKeyNameText( lParam, keyName, 256 );
					 DebugMsg( "%s\n", keyName );
					 switch ( wParam )
					 {
					 case VK_UP:
						 break;
					 case VK_DOWN:
						 break;
					 case VK_RIGHT:
						 break;
					 case VK_LEFT:
						 break;
					 default:
						 break;
					 }
		}
	default:
		break;
	}
}

void CGameLogic::HandleWIndowMouseInput( const int message, const WPARAM wParam, const LPARAM lParam )
{
	CWinProcMouseInputTranslator translator;
	MOUSE_INPUT_INFO& input = translator.TranslateInput( message, wParam, lParam );

	m_mouseController.ProcessInput( input );
}

CGameLogic::CGameLogic( )
{
	ShowDebugConsole( );
}

CGameLogic::~CGameLogic ( )
{
}