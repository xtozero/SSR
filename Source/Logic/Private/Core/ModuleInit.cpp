#include "Core/GameLogic.h"
#include "InterfaceFactories.h"

namespace
{
	logic::ILogic* GGameLogic = nullptr;

	void* GetGameLogic( )
	{
		return GGameLogic;
	}
}

namespace logic
{
	LOGIC_FUNC_DLL void BootUpModules()
	{
		RegisterFactory<ILogic>( &GetGameLogic );

		GGameLogic = CreateGameLogic();
	}

	LOGIC_FUNC_DLL void ShutdownModules()
	{
		DestroyGameLogic( GGameLogic );

		UnregisterFactory<ILogic>();
	}
}