#include "ShaderTool.h"

#include <cassert>

std::string ShaderTool::Process( const std::string& src )
{
	m_src.Parse( src.data(), src.size() );

	std::string shaderFile;
	shaderFile.reserve( src.size() );

	CacheLine();
	while ( m_line.CanRead() )
	{
		std::string_view token = ReadToken();
		if ( token == "DefineBindlessIndices" )
		{
			CollectBindlessVariableName();
		}
		else
		{
			shaderFile += m_srcLine;
			SkipLine();
		}
	}

	return InsertBindlessVariable( shaderFile );
}

void ShaderTool::CacheLine()
{
	if ( m_line.CanRead() )
	{
		return;
	}

	m_srcLine = m_src.ReadLine( false );
	m_line.Parse( m_srcLine.data(), m_srcLine.size() );
}

void ShaderTool::SkipLine()
{
	m_srcLine = m_src.ReadLine( false );
	m_line.Parse( m_srcLine.data(), m_srcLine.size() );
}

std::string_view ShaderTool::ReadToken()
{
	m_line.SkipWhiteSpace();
	CacheLine();

	m_line.SkipWhiteSpace();
	return m_line.ReadWord();
}

void ShaderTool::CollectBindlessVariableName()
{
	std::string_view token = ReadToken();

	assert( token == "{" );

	auto RemoveSemicolon = []( const std::string_view& s )
		{
			const char* begin = s.data();
			for ( int32 i = 0; i < s.size(); ++i )
			{
				if ( s[i] == ';' )
				{
					return std::string_view( begin, i );
				}
			}

			return s;
		};

	for ( token = ReadToken(); token != "};"; token = ReadToken() )
	{
		if ( token == "int" )
		{
			continue;
		}
		else
		{
			token = RemoveSemicolon( token );
			m_bindlessVariableName.emplace_back( token );
		}
	}
}

std::string ShaderTool::InsertBindlessVariable( const std::string& srcShaderFile )
{
	constexpr size_t MB = 1024 * 1024;

	std::string bindlessVariable;
	bindlessVariable.reserve( 1 * MB);

	/* Sample
	* DefineBindlessIndices
	* {
	*
	* }
	*/
	bindlessVariable += "\nDefineBindlessIndices\n{\n";

	for ( const std::string_view& name : m_bindlessVariableName )
	{
		bindlessVariable += "\tint ";
		bindlessVariable += name;
		bindlessVariable += ";\n";
	}

	bindlessVariable += "}\n";

	std::string shaderFile;
	shaderFile.reserve( bindlessVariable.size() + srcShaderFile.size() );

	TextTokenaizer tokenaizer;
	tokenaizer.Parse( srcShaderFile.data(), srcShaderFile.size() );

	while ( tokenaizer.CanRead() )
	{
		std::string_view line = tokenaizer.ReadLine( false );

		shaderFile += line;

		// Insert after DefineBindlessIndices definition
		if ( line.starts_with( "#define DefineBindlessIndices" ) )
		{
			shaderFile += bindlessVariable;
		}
	}

	return shaderFile;
}
