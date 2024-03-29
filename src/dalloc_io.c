#include <math.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "dalloc_config.h"
#include "dalloc_io.h"
#include "dalloc_io_internal.h"

#define write_log(lvl, fmt) { \
	va_list args; \
	va_start(args, (fmt)); \
	vlog_message((lvl), (fmt), args); \
	va_end(args); \
}

const size_t TIMESTAMP_LEN = 20;

// Only messages equally or more important than this log level will be
// logged. Can be set via set_log_level().
int user_log_level = DALLOC_LOG_LEVEL_WARNING;

int get_msg_type(int log_level, char *buf) {
	if (log_level == DALLOC_LOG_LEVEL_ERROR) {
		strcpy(buf, "ERROR");
		return 5;
	}
	if (log_level == DALLOC_LOG_LEVEL_WARNING) {
		strcpy(buf, "WARNING");
		return 7;
	}
	if (log_level == DALLOC_LOG_LEVEL_INFO) {
		strcpy(buf, "INFO");
		return 4;
	}
	if (log_level == DALLOC_LOG_LEVEL_DIAGNOSTIC) {
		strcpy(buf, "DIAGNOSTIC");
		return 10;
	}
	if (log_level == DALLOC_LOG_LEVEL_DEBUG) {
		strcpy(buf, "DEBUG");
		return 5;
	}
	strcpy(buf, "UNKNOWN");
	return 7;
}

uint32_t pad(uint32_t x, uint16_t n, char *buf) {
	uint16_t digits = (uint16_t)floor(log10(x) + 1);
	if (digits > n) {
		return 1;
	}

	if (digits == n) {
		sprintf(buf, "%d", x);
	} else {
		uint16_t npad = n - digits;
		// npad + 1 to allow for null terminator.
		char zeroes[npad + 1];
		zeroes[npad] = 0;
		for (uint16_t i = 0; i < npad; i++) {
			zeroes[i] = '0';
		}
		sprintf(buf, "%s%d", zeroes, x);
	}

	return 0;
}

void get_timestamp(char buf[TIMESTAMP_LEN]) {
	time_t rawtime;
	time(&rawtime);
	struct tm *timeinfo = localtime(&rawtime);

	// These buffers must be of size N + 1 (for null terminator).
	char year[5];
	pad(timeinfo->tm_year + 1900, 4, year);

	char month[3];
	pad(timeinfo->tm_mon, 2, month);

	char day[3];
	pad(timeinfo->tm_mday, 2, day);

	char hour[3];
	pad(timeinfo->tm_hour, 2, hour);

	char minute[3];
	pad(timeinfo->tm_min, 2, minute);

	char second[3];
	pad(timeinfo->tm_sec, 2, second);

	sprintf(buf, "%s-%s-%s %s:%s:%s", year, month, day, hour, minute, second);

	buf[TIMESTAMP_LEN - 1] = 0;
}

void vlog_message(int log_level, const char *fmt, va_list args) {
	if (log_level > user_log_level) {
		return;
	}

	// The messsage will look like:
	//
	// dalloc 2022-01-31 10:30:00 ERROR: <msg>\n
	//
	// 'dalloc ' = 7 chars
	// 'yyyy-MM-dd hh:mm:ss ' = 19 chars
	// message type length = calculated
	// ': ' = 2 chars
	// $fmt = `strlen(fmt)` chars
	// \n = 1 char

	char timestamp[TIMESTAMP_LEN];
	get_timestamp(timestamp);

	// 'DIAGNOSTIC' = 10 chars
	size_t buf_len = 10;
	char msg_type[buf_len];
	int msg_type_len = get_msg_type(log_level, msg_type);
	for (int i = msg_type_len; i < buf_len; i++) {
		msg_type[i] = 0;
	}

	int message_length = 7 + TIMESTAMP_LEN + msg_type_len + 2 + strlen(fmt) + 1;
	char full_format[message_length];
	sprintf(full_format, "dalloc %s %s: %s\n", timestamp, msg_type, fmt);

	FILE *out = log_level == DALLOC_LOG_LEVEL_ERROR ? stderr : stdout;
	vfprintf(out, full_format, args);
}

void log_message(int log_level, const char *fmt, ...) {
	write_log(log_level, fmt);
}

void log_error(const char *fmt, ...) {
	write_log(DALLOC_LOG_LEVEL_ERROR, fmt);
}

void log_warning(const char *fmt, ...) {
	write_log(DALLOC_LOG_LEVEL_WARNING, fmt);
}

void log_info(const char *fmt, ...) {
	write_log(DALLOC_LOG_LEVEL_INFO, fmt);
}

void log_diag(const char *fmt, ...) {
	write_log(DALLOC_LOG_LEVEL_DIAGNOSTIC, fmt);
}

void log_debug(const char *fmt, ...) {
	write_log(DALLOC_LOG_LEVEL_DEBUG, fmt);
}

void panic(const char *fmt, ...) {
	write_log(DALLOC_LOG_LEVEL_ERROR, fmt);

	if (!robust_mode()) {
		raise(SIGILL);
	}
}

void set_log_level(int log_level) {
	user_log_level = log_level;
}
