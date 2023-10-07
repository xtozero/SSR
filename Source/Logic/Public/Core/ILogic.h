#pragma once

#include "common.h"
#include "GuideTypes.h"
#include "SizedTypes.h"

namespace engine
{
	class IPlatform;
	struct UserInput;
}

namespace logic
{
	class GameClientViewport;
	class CGameObject;

	class ILogic
	{
	public:
		virtual bool BootUp( engine::IPlatform& ) = 0;
		virtual void Update() = 0;
		virtual void Pause() = 0;
		virtual void Resume() = 0;
		virtual void HandleUserInput( const engine::UserInput& ) = 0;
		virtual void HandleTextInput( uint64 text, bool bUnicode ) = 0;
		virtual void AppSizeChanged( engine::IPlatform& platform ) = 0;
		virtual GameClientViewport* GetGameClientViewport() = 0;
		virtual bool LoadWorld( const char* filePath ) = 0;
		virtual void UnloadWorld() = 0;

		virtual ~ILogic() = default;
	};

	Owner<ILogic*> CreateGameLogic();
	void DestroyGameLogic( Owner<ILogic*> pGameLogic );
}
