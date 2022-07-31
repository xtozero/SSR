#include "ShaderFileMerger.h"

#include "TextTokenaizer.h"

#include <fstream>

namespace fs = std::filesystem;

std::optional<std::string> ShaderFileMerger::Merge( const std::filesystem::path& shaderFile )
{
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
                            if ( fs::exists( fullPath ) )
                            {
                                MergeRecursive( fullPath );
                                break;
                            }
                        }
                    }
                }
                else
                {
                    tokenaizer.Seek( pos );
                    m_buffer += tokenaizer.ReadLine( false );
                }
            }

            if ( contents.ends_with( "\n" ) == false )
            {
                m_buffer += "\n\n";
            }
        }
    }
}
