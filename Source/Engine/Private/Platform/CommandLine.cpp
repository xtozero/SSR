#include "CommandLine.h"

#include "StringUtility.h"

namespace engine
{
	void CommandLine::Parse( char* argv )
	{
		std::vector<std::string> splited = SplitString( argv );
		std::vector<Name>* option = nullptr;

		for ( auto& arg : splited )
		{
			bool bDash = arg.starts_with( "-" );
			bool bDoubleDash = arg.starts_with( "--" );

			if ( bDash )
			{
				option = &m_related[Name( arg.substr( 1 ) )];
			}
			else if ( bDoubleDash )
			{
				option = &m_related[Name( arg.substr( 2 ) )];
			}
			else
			{
				if ( option != nullptr )
				{
					option->emplace_back( arg );
				}
				else
				{
					m_unrelated.emplace( arg );
				}
			}
		}
	}

	const std::vector<Name>* CommandLine::GetArguments( const Name& name ) const
	{
		auto found = m_related.find( name );
		if ( found != std::end( m_related ) )
		{
			return &found->second;
		}

		return nullptr;
	}

	bool CommandLine::IsExist( const Name& name ) const
	{
		auto IsExist_Lambda = [name]( const auto& container )
			{
				auto found = container.find( name );
				if ( found != std::end( container ) )
				{
					return true;
				}

				return false;
			};

		return IsExist_Lambda( m_related ) || IsExist_Lambda( m_unrelated );
	}

	CommandLine* CreateCommandLine()
	{
		return new CommandLine;
	}

	void DestroyCommandLine( CommandLine* commandLine )
	{
		delete commandLine;
	}
}
