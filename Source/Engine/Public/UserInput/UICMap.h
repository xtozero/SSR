#pragma once

#include "UserInput.h"

#include <vector>

namespace JSON
{
	class Value;
}

class UICMap
{
public:
	void Initialize( );

	USER_INPUT_CODE Convert( unsigned long code );

	bool LoadConfig( const char* fileName );

private:
	void LoadKeyCode( const JSON::Value& json );

	using CodePair = std::pair<unsigned long, USER_INPUT_CODE>;
	std::vector<CodePair> m_codeMap;
};
