#pragma once

#include "TextTokenaizer.h"

#include <string>
#include <optional>
#include <vector>

class ShaderTool final
{
public:
	std::string Process( const std::string& src );

private:
	bool CacheLine();
	void SkipLine();

	bool IsBindlessArrayAccess();

	void CollectBindlessVariableName();
	std::string InsertBindlessVariable( const std::string& srcShaderFile );

	std::string_view m_srcLine;

	TextTokenaizer m_src;
	TextTokenaizer m_line;

	std::vector<std::string_view> m_bindlessVariableNames;
};