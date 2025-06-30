#include "ShaderTool.h"

#include <cassert>

std::string ShaderTool::Process( const std::string& src )
{
	m_src.Parse( src.data(), src.size() );

	std::string shaderFile;
	shaderFile.reserve( src.size() );

	while ( CacheLine() )
	{
		while ( m_line.CanRead() )
		{
			if ( IsBindlessArrayAccess() )
			{
				CollectBindlessVariableName();
			}

			m_line.GetNextChar();
		}

		shaderFile += m_srcLine;
	}

	return InsertBindlessVariable( shaderFile );
}

bool ShaderTool::CacheLine()
{
	m_srcLine = m_src.ReadLine( false );
	m_line.Parse( m_srcLine.data(), m_srcLine.size() );

	return m_line.CanRead();
}

void ShaderTool::SkipLine()
{
	m_srcLine = m_src.ReadLine( false );
	m_line.Parse( m_srcLine.data(), m_srcLine.size() );
}

bool ShaderTool::IsBindlessArrayAccess()
{
	return m_line.MatchNextString( "Tex2DArray[", std::strlen( "Tex2DArray[" ) )
			|| m_line.MatchNextString( "Tex2D[", std::strlen( "Tex2D[" ) )
			|| m_line.MatchNextString( "Tex3D[", std::strlen( "Tex3D[" ) )
			|| m_line.MatchNextString( "TexCube[", std::strlen( "TexCube[" ) )
			|| m_line.MatchNextString( "Samplers[", std::strlen( "Samplers[" ) );
}

void ShaderTool::CollectBindlessVariableName()
{
	m_line.SkipWhiteSpace();

	const char* nameStart = m_line.Tell();
	size_t len = 0;

	for ( char c = m_line.GetNextChar(); c != ']'; c = m_line.GetNextChar() )
	{
		++len;
	}

	if ( len == 0 )
	{
		return;
	}

	m_bindlessVariableNames.emplace_back( nameStart, len );
}

std::string ShaderTool::InsertBindlessVariable( const std::string& srcShaderFile )
{
	if ( m_bindlessVariableNames.empty() )
	{
		return srcShaderFile;
	}

	constexpr size_t MB = 1024 * 1024;

	std::string bindlessVariable;
	bindlessVariable.reserve( 1 * MB);

	/* Sample
	 * #if SupportsBindless
	 * int Foo;
	 * int Bar;
	 * #endif
	 */
	bindlessVariable += "#if SupportsBindless == 1\n";
	for ( const std::string_view& name : m_bindlessVariableNames )
	{
		bindlessVariable += "int\t";
		bindlessVariable += name;
		bindlessVariable += ";\n";
	}
	bindlessVariable += "#endif\n\n";

	std::string shaderCode = bindlessVariable + srcShaderFile;

	std::string pattern = R"(((?:int|\[)[\t ]*)()";
	for ( auto iter = std::begin( m_bindlessVariableNames ); iter != std::end( m_bindlessVariableNames ); ++iter )
	{
		if ( iter != std::begin( m_bindlessVariableNames ) )
		{
			pattern += "|";
		}

		pattern += *iter;
	}
	pattern += ")";

	RegexReplace( shaderCode, pattern, "$1" + std::string( agl::BindlessIndexTag ) + "$2" );

	return shaderCode;
}
