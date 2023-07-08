#include "ImguiEditor.h"

#include "LibraryTool/InterfaceFactories.h"

bool ImguiEditor::BootUp( IPlatform& platform )
{
	m_logicDll = LoadModule( "Logic.dll" );
	if ( m_logicDll == nullptr )
	{
		return false;
	}

	m_logic = GetInterface<ILogic>();
	if ( m_logic )
	{
		return m_logic->BootUp( platform );
	}

	return false;
}

void ImguiEditor::Update()
{
	m_logic->Update();
}

void ImguiEditor::Pause()
{
	m_logic->Pause();
}

void ImguiEditor::Resume()
{
	m_logic->Resume();
}

void ImguiEditor::HandleUserInput( const UserInput& input )
{
	m_logic->HandleUserInput( input );
}

void ImguiEditor::AppSizeChanged( IPlatform& platform )
{
	m_logic->AppSizeChanged( platform );
}

ImguiEditor::~ImguiEditor()
{
	ShutdownModule( m_logicDll );
}

Owner<IEditor*> CreateEditor()
{
	return new ImguiEditor();
}

void DestroyEditor( Owner<IEditor*> pEditor )
{
	delete pEditor;
}
