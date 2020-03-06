
/** @file debug.h
    @brief Debug output routines
*/

#ifndef _WIFIDOG_DEBUG_H_
#define _WIFIDOG_DEBUG_H_

#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define LOG_PATH "/tmp/debug.log"

#if 0	/** debug level**/
		LOG_EMERG		0	/* system is unusable */
		LOG_ALERT		1	/* action must be taken immediately */
		LOG_CRIT		2	/* critical conditions */
		LOG_ERR			3	/* error conditions */
		LOG_WARNING		4	/* warning conditions */
		LOG_NOTICE		5	/* normal but significant condition */
		LOG_INFO		6	/* informational */
		LOG_DEBUG		7	/* debug-level messages */
#endif

typedef struct _debug_conf {
    int debuglevel;      /**< @brief Debug information verbosity */
    int log_stderr;      /**< @brief Output log to stdout */
    int log_syslog;      /**< @brief Output log to syslog */
    int syslog_facility; /**< @brief facility to use when using syslog for logging */
} debugconf_t;

extern debugconf_t debugconf;


/** Used to output messages.
 * The messages will include the filename and line number, and will be sent to stderr if so configured in the config file 
 * @param level Debug level
 * @param format... sprintf like format string
 */

#define debug(level, format...) _debug(__FILENAME__, __LINE__, level, format)

/** @internal */
void _debug(const char *, int, int, const char *, ...);

#endif /* _DEBUG_H_ */
