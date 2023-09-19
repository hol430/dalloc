#ifndef _DALLOC_IO_H_
#define _DALLOC_IO_H_

#define DALLOC_LOG_LEVEL_DEBUG 5
#define DALLOC_LOG_LEVEL_DIAGNOSTIC 4
#define DALLOC_LOG_LEVEL_INFO 3
#define DALLOC_LOG_LEVEL_WARNING 2
#define DALLOC_LOG_LEVEL_ERROR 1
#define DALLOC_LOG_LEVEL_NONE 0

/*
Set the log level. Only messages above this log level will be logged.
*/
void set_log_level(int log_level);

/*
Write a log message of the specified log level.

@param log_level: Log level (error/warning/...).
@param fmt: printf-style format string.
*/
void log_message(int log_level, const char* fmt, ...);

/*
Write an error message. This is a wrapper around log_message().

@param fmt: printf-style format string.
*/
void log_error(const char* fmt, ...);

/*
Write a warning message. This is a wrapper around log_message().

@param fmt: printf-style format string.
*/
void log_warning(const char* fmt, ...);

/*
Write an info message. This is a wrapper around log_message().

@param fmt: printf-style format string.
*/
void log_info(const char* fmt, ...);

/*
Write a diagnostic message. This is a wrapper around log_message().

@param fmt: printf-style format string.
*/
void log_diag(const char* fmt, ...);

/*
Write a debug message. This is a wrapper around log_message().

@param fmt: printf-style format string.
*/
void log_debug(const char* fmt, ...);

/*
Write an error message and abort program execution. If this is being
called, things are looking rather grim indeed.
*/
void panic(const char* fmt, ...);

#endif // _DALLOC_IO_H_
