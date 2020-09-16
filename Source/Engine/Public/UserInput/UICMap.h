#pragma once

#include "UserInput.h"

#include <vector>

class UICMap
{
public:
	void Initialize( );

	USER_INPUT_CODE Convert( unsigned long code );

	bool LoadConfig( const char* fileName );

private:
	void LoadKeyCode( const char* uicAsset, size_t assetSize );

	using CodePair = std::pair<int, USER_INPUT_CODE>;
	std::vector<CodePair> m_codeMap;
};
