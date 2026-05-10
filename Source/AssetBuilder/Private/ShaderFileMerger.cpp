#include "ShaderFileMerger.h"

#include "TextTokenaizer.h"

#include <fstream>

namespace fs = std::filesystem;

namespace
{
    bool IsComments( const std::string_view& token )
    {
        return token.starts_with( "//" ) || token.ends_with( "/*" );
    }
}

std::optional<std::string> ShaderFileMerger::Merge( const std::filesystem::path& shaderFile )
{
    m_buffer.clear();
    m_mergedFiles.clear();

    constexpr size_t MB = 1024 * 1024;
    m_buffer.reserve( 2 * MB );

    if ( fs::exists( shaderFile ) )
    {
        MergeRecursive( shaderFile );
    }

    if ( m_buffer.empty() )
    {
        return {};
    }

    return std::move( m_buffer );
}

ShaderFileMerger::ShaderFileMerger( const std::filesystem::path& includeFilesDirectory )
    : m_includeFileDirectory( includeFilesDirectory )
{
}

void ShaderFileMerger::MergeRecursive( const std::filesystem::path& shaderFile )
{
    if ( fs::exists( shaderFile ) )
    {
        std::ifstream file( shaderFile );
        if ( file.good() )
        {
            const fs::path candidateDirectories[] = {
                shaderFile.parent_path(),
                m_includeFileDirectory
            };

            std::string contents;
            std::getline( file, contents, static_cast<char>( EOF ) );

            TextTokenaizer tokenaizer;
            tokenaizer.Parse( contents.data(), contents.size() );

            auto ReadToken = [&tokenaizer]() mutable
            {
                tokenaizer.SkipWhiteSpace();
                return tokenaizer.ReadWord();
            };

            while ( tokenaizer.CanRead() )
            {
                const char* pos = tokenaizer.Tell();
                uint32 lineNumber = tokenaizer.GetLineNumber();

                std::string_view token = ReadToken();

                if ( token.starts_with( "#include" ) )
                {
                    std::string_view includeFile = ReadToken();
                    if ( ( includeFile.starts_with( "\"" ) || includeFile.ends_with( "\"" ) )
                        || ( includeFile.starts_with( "<" ) || includeFile.ends_with( ">" ) ) )
                    {
                        std::string_view trim = includeFile.substr( 1, includeFile.size() - 2 );
                        
                        for ( const fs::path& directory : candidateDirectories )
                        {
                            fs::path fullPath = directory / trim;
                            bool notMerged = m_mergedFiles.contains( fullPath ) == false;
                            if ( notMerged && fs::exists( fullPath ) )
                            {
                                m_mergedFiles.emplace( fullPath );
                                MergeRecursive( fullPath );
                                break;
                            }
                        }
                    }
                }
                else if ( IsComments( token ) )
                {
                    uint32 nunSkippedLine = tokenaizer.GetLineNumber() - lineNumber;
                    for ( uint32 i = 1; i < nunSkippedLine; ++i )
                    {
                        m_buffer += "\n";
                    }

                    // Skip comments
                    if ( token.starts_with( "/*" ) )
                    {
                        while ( tokenaizer.CanRead() && ( token.ends_with( "*/" ) == false ) )
                        {
                            token = ReadToken();
                        }
                        assert( token.ends_with( "*/" ) );
                    }
                    else
                    {
                        tokenaizer.SkipUntilNewline();
                    }
                }
                else
                {
                    m_buffer += std::string_view( pos, tokenaizer.Tell() );
                }
            }

            if ( contents.ends_with( "\n" ) == false )
            {
                m_buffer += "\n\n";
            }
        }
    }
}
