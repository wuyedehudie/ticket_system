#ifndef __TCS_LOG_H__
#define __TCS_LOG_H__

#include <string>
#include <cstdint>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdarg.h>
#include <time.h>
#include <map>
#include "utils/singleton.h"

/**
*@brief流式方式，将日志级别写入logger
*/
#define TCS_LOG_LEVEL(logger, level) \
    if (logger->getLevel() <= level) \
        tcs::LogEventWrap(tcs::LogEvent::ptr(new tcs::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, 0, \
                        0, time(0)))).getSS()

#define TCS_LOG_DEBUG(logger) TCS_LOG_LEVEL(logger,tcs::LogLevel::DEBUG)

#define TCS_LOG_INFO(logger) TCS_LOG_LEVEL(logger,tcs::LogLevel::INFO)

#define TCS_LOG_WARN(logger) TCS_LOG_LEVEL(logger,tcs::LogLevel::WARN)

#define TCS_LOG_ERROR(logger) TCS_LOG_LEVEL(logger,tcs::LogLevel::ERROR)

#define TCS_LOG_FATAL(logger) TCS_LOG_LEVEL(logger,tcs::LogLevel::FATAL)

/**
 * @brief 使用格式化方式将日志级别level的日志写入到logger
 */
#define TCS_LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->getLevel() <= level) \
        tcs::LogEventWrap(tcs::LogEvent::ptr(new tcs::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, 0,\
                0, time(0))))).getEvent()->format(fmt, __VA_ARGS__)

#define TCS_LOG_FMT_DEBUG(logger, fmt, ...) TCS_LOG_FMT_LEVEL(logger, tcs::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define TCS_LOG_FMT_INFO(logger, fmt, ...)  TCS_LOG_FMT_LEVEL(logger, tcs::LogLevel::INFO, fmt, __VA_ARGS__)
#define TCS_LOG_FMT_WARN(logger, fmt, ...)  TCS_LOG_FMT_LEVEL(logger, tcs::LogLevel::WARN, fmt, __VA_ARGS__)
#define TCS_LOG_FMT_ERROR(logger, fmt, ...) TCS_LOG_FMT_LEVEL(logger, tcs::LogLevel::ERROR, fmt, __VA_ARGS__)
#define TCS_LOG_FMT_FATAL(logger, fmt, ...) TCS_LOG_FMT_LEVEL(logger, tcs::LogLevel::FATAL, fmt, __VA_ARGS__)
#define TCS_LOG_ROOT() tcs::LoggerMgr::GetInstance()->getRoot()
#define TCS_LOG_NAME(name) tcs::LoggerMgr::GetInstance()->getLogger(name)

namespace tcs {
    class Logger;
    class LoggerManager;


    //1.日志级别,日志内容其二
    class LogLevel {
    public:
        enum Level {
            UNKNOW = 0,
            DEBUG = 1,
            INFO = 2,
            WARN = 3,
            ERROR = 4,
            FATAL = 5,
        };

        static const char* ToString(LogLevel::Level level);
    };

    //1.日志事件,日志内容其一
    class LogEvent {
    public:
        typedef std::shared_ptr<LogEvent> ptr;
        LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* file, int32_t line, uint32_t elapse
                , uint32_t threadId, uint32_t fiber, uint64_t time);

        const char* getFile()	const { return m_file; }
        int32_t getLine()		const { return m_line; }
        uint32_t getElapse()    const { return m_elapse; }
        uint32_t getThreadId()	const { return m_threadId; }
        uint32_t getFiberId()	const { return m_fiberId; }
        uint64_t getTime()		const { return m_time; }
        const std::string& getThreadName() const { return m_ThreadName; }
        const std::string getContent() const { return m_ss.str(); }
        std::shared_ptr<Logger> getLogger() const { return m_logger; }
        LogLevel::Level getLevel() const { return m_level; }
        std::stringstream& getSS() { return m_ss; }
        /**
        * @brief 格式化写入日志内容
        */
        void format(const char* fmt, ...);
        void format(const char* fmt, va_list al);

    private:
        const char* m_file = nullptr;	//文件名
        int32_t m_line = 0;		//行号
        uint32_t m_elapse = 0;			//考虑内存对齐增加这一句,程序启动到现在的毫秒数
        uint32_t m_threadId = 0;		//线程id
        uint32_t m_fiberId = 0;			//协程id
        uint64_t m_time = 0;				//时间戳
        std::string m_ThreadName;
        std::stringstream m_ss;			//
        std::shared_ptr<Logger> m_logger;
        LogLevel::Level m_level;

    };

    //2.日志包装器，留给用户扩展
    class LogEventWrap {
    public:
        LogEventWrap(LogEvent::ptr e);
        ~LogEventWrap();
        LogEvent::ptr getEvent()const { return m_event; };
        std::stringstream& getSS();
    private:
        LogEvent::ptr m_event;
    };

    //1.日志格式器
    class LogFormatter {
    public:
        typedef std::shared_ptr<LogFormatter> ptr;
        LogFormatter(const std::string& pattern);
        std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
        std::ostream& format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
    public:
        class FormatItem {
        public:
            typedef std::shared_ptr<FormatItem> ptr;
            virtual ~FormatItem() {}
            virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
        };
        void init();
        bool isError() { return m_error; }
        const std::string getPattern() const { return m_pattern; }
    private:
        std::string m_pattern;
        std::vector<FormatItem::ptr> m_items;
        bool m_error = false;
    };

    //1.日志输出地，基类
    class LogAppender
    {
        friend class Logger;
    public:
        typedef std::shared_ptr<LogAppender> ptr;
        virtual ~LogAppender() {}

        virtual void log(std::shared_ptr<Logger> loggger, LogLevel::Level, LogEvent::ptr event) = 0;
        void setFormatter(LogFormatter::ptr val) { m_formatter = val; }
        LogFormatter::ptr getFormatter() const { return m_formatter; }
    protected:
        LogLevel::Level m_level = LogLevel::DEBUG;
        LogFormatter::ptr m_formatter;
        //是否有自己的日志格式器
        bool m_hasFormatter = false;
    };

    //1.日志器
    class Logger : public std::enable_shared_from_this<Logger> {
        friend class LoggerManager;
    public:
        typedef std::shared_ptr<Logger> ptr;

        Logger(const std::string& name = "root");
        void log(LogLevel::Level level, LogEvent::ptr event);


        //2.五个debug的事件处理，然后跳到.cc文件实现
        void debug(LogEvent::ptr event);
        void info(LogEvent::ptr event);
        void warn(LogEvent::ptr event);
        void error(LogEvent::ptr event);
        void fatal(LogEvent::ptr event);

        void addAppender(LogAppender::ptr appender);
        void delAppender(LogAppender::ptr appender);
        void clearAppender();

        LogLevel::Level getLevel() { return m_level; }
        void setLevel(LogLevel::Level val) { m_level = val; }
        const std::string& getName() { return m_name; }

        void setFormatter(LogFormatter::ptr val);
        void setFormatter(const std::string& val);
    private:
        std::string m_name;											//日志名称
        LogLevel::Level m_level;									//日志级别
        std::list<LogAppender::ptr> m_appenders;	//Appender集合
        LogFormatter::ptr m_formatter;
        Logger::ptr m_root;
    };

    //2.Appender的两个子类，对应控制台和文件两个日志输出地
    class StdoutLogAppender :public LogAppender {
    public:
        typedef std::shared_ptr<StdoutLogAppender> ptr;
        void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
    };
    class FileLogAppender : public LogAppender {
    public:
        typedef std::shared_ptr<FileLogAppender> ptr;
        FileLogAppender(const std::string& filename);
        void log(std::shared_ptr<Logger> logger, LogLevel::Level, LogEvent::ptr event) override;
        //文件重新打开成功返回true
        bool reopen();
    private:
        std::string m_filename;
        std::ofstream m_filestream;
        uint64_t m_lastTime = 0;
    };

    //8.日志管理器，管理所有logger
    class LoggerManager {
    public:
        LoggerManager();
        void init();

        Logger::ptr getRoot() const { return m_root; }
        Logger::ptr getLogger(const std::string& name);
    private:
        std::map<std::string, Logger::ptr> m_loggers;
        Logger::ptr m_root;
    };

    typedef tcs::Singleton<LoggerManager> LoggerMgr;
}//namespace tcs

static auto rootlog = TCS_LOG_NAME("rootlog");

#endif







