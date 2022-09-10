#pragma once

#include "NameTypes.h"

#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

class CommandLine
{
public:
	void Parse( char* argv );
	const std::vector<Name>* GetArguments( const Name& name ) const;
	bool IsExist( const Name& name ) const;

private:
	std::map<Name, std::vector<Name>> m_related;
	std::set<Name> m_unrelated;
};

CommandLine* CreateCommandLine();
void DestroyCommandLine( CommandLine* commandLine );