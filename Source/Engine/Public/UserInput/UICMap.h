#pragma once

#include "SizedTypes.h"
#include "UserInput.h"

#include <vector>

namespace engine
{
	class UICMap
	{
	public:
		void Initialize();

		UserInputCode Convert( uint32 code );

		bool LoadConfig( const char* fileName );

	private:
		void LoadKeyCode( const char* uicAsset, size_t assetSize );

		using CodePair = std::pair<uint32, UserInputCode>;
		std::vector<CodePair> m_codeMap;
	};
}
