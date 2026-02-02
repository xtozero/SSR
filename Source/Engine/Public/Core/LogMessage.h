#pragma once

namespace engine
{
    struct LogEntry
    {
        std::string m_message;
    };

    using LogEntryPred = void (*)( const LogEntry& );

    class ILogMessage
    {
    public:
        template <typename... Args>
        void Log( std::format_string<Args...> format, Args&&... args )
        {
            Log( std::format( format, std::forward<Args>( args )... ) );
        }

        virtual void Log( const std::string& message ) = 0;

        virtual void ForEachLogEntry( LogEntryPred pred ) const = 0;
        virtual void ForLogEntry( int32 begin, int32 end, LogEntryPred pred ) const = 0;

        virtual int32 Num() const = 0;

        virtual ~ILogMessage() = default;

        static ILogMessage& GetInstance();
    };
}
