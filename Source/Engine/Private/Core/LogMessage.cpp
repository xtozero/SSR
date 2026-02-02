#include "LogMessage.h"

#include <chrono>
#include <format>
#include <shared_mutex>
#include <vector>

namespace engine
{
    class LogMessage : public ILogMessage
    {
    public:
        virtual void Log( const std::string& message ) override;

        virtual void ForEachLogEntry( LogEntryPred pred ) const override;
        virtual void ForLogEntry( int32 begin, int32 end, LogEntryPred pred ) const override;

        virtual int32 Num() const override;

    private:
        static LogEntry NewLogEntry( const std::string& message );

        mutable std::shared_mutex m_logEntriesMutex;

        std::vector<LogEntry> m_logEntries;
    };

    void LogMessage::Log( const std::string& message )
    {
        LogEntry newEntry = NewLogEntry( message );

        std::unique_lock<std::shared_mutex> lock( m_logEntriesMutex );
        m_logEntries.emplace_back( std::move( newEntry ) );
    }

    void LogMessage::ForEachLogEntry( LogEntryPred pred ) const
    {
        std::shared_lock<std::shared_mutex> lock( m_logEntriesMutex );

        for ( const LogEntry& entry : m_logEntries )
        {
            pred( entry );
        }
    }

    void LogMessage::ForLogEntry( int32 begin, int32 end, LogEntryPred pred ) const
    {
        std::shared_lock<std::shared_mutex> lock( m_logEntriesMutex );

        for ( int i = begin; i < end; ++i )
        {
            pred( m_logEntries[i] );
        }
    }

    int32 LogMessage::Num() const
    {
        return static_cast<int32>( m_logEntries.size() );
    }

    LogEntry LogMessage::NewLogEntry( const std::string& message )
    {
        auto now = std::chrono::system_clock::now();
        std::chrono::zoned_time local( std::chrono::current_zone(), now );

        std::string logMessage = std::format( "[{:%Y-%m-%d %H:%M:%S}] {}", local, message );

        LogEntry newEntry = {
            .m_message = std::move( logMessage ),
        };

        return newEntry;
    }

    ILogMessage& ILogMessage::GetInstance()
    {
        static LogMessage logMessage;
        return logMessage;
    }
}
