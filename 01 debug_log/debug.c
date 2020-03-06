
/** @file debug.c
    @brief Debug output routines
*/

#include <stdio.h>
#include <errno.h>
#include <syslog.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

#include "debug.h"

debugconf_t debugconf = {
    .debuglevel = LOG_INFO,
    .log_stderr = 1,
    .log_syslog = 1,
    //.syslog_facility = LOG_LOCAL0 ,
};


/** @internal
Do not use directly, use the debug macro */
void
_debug(const char *filename, int line, int level, const char *format, ...)
{
    char buf[28];
    va_list vlist;
    time_t ts;
    sigset_t block_chld;

    time(&ts);
    
    if (debugconf.debuglevel >= level) 
	{
        sigemptyset(&block_chld);
        sigaddset(&block_chld, SIGCHLD);
        sigprocmask(SIG_BLOCK, &block_chld, NULL);

        if (level <= LOG_WARNING) {          
            fprintf(stderr, "[%d][%.24s][%u](%s:%d) ", level, ctime_r(&ts, buf), getpid(),
                filename, line);
            va_start(vlist, format);
            vfprintf(stderr, format, vlist);
            va_end(vlist);
            fputc('\n', stderr); 
            fflush(stderr);
        } else if (debugconf.log_stderr) {
            fprintf(stderr, "[%d][%.24s][%u](%s:%d) ", level, ctime_r(&ts, buf), getpid(),
                filename, line);
            va_start(vlist, format);
            vfprintf(stderr, format, vlist);
            va_end(vlist);
            fputc('\n', stderr);
            fflush(stderr);
        }

        if (debugconf.log_syslog) {
			#if 0	/** 用系统函数需要加以配置系统文件，嵌入式系统配置/使用起来不便使用 **/
            openlog("nassoft", LOG_PID, debugconf.syslog_facility);
            va_start(vlist, format);
            vsyslog(level, format, vlist);
            va_end(vlist);
            closelog();
			#else
			FILE * fp;
			
			va_list ap;
			va_start(ap, format);
			
			fp = fopen (LOG_PATH, "a+");

			fprintf(fp, "[%d][%.24s][%u](%s:%d) ", level, ctime_r(&ts, buf), getpid(),
                filename, line);
			vfprintf(fp, format, ap);
			
			fclose(fp);
			va_end(ap);
			#endif	
        }
        
        sigprocmask(SIG_UNBLOCK, &block_chld, NULL);
    }
}
