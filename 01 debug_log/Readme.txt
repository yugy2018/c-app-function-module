关于日志记录功能模块记录说明：
	可以输出日志内容到当前终端，也可以输出到日志文件。二者为可配置，修改debugconf结构体即可

[关键变量说明]
	1.日志文件路径宏定义：debug.h :#define LOG_PATH "/tmp/debug.log"
	2.debug输出功能定义结构体：
		debugconf_t debugconf = {
    .debuglevel = LOG_INFO,		//当前debug等级（需要输出的内容等级要 <= debuglevel）
    .log_stderr = 1,					//是否输出到标准错误输出口 1：输出  0：不输出
    .log_syslog = 1,					//是否输出到日志文件
    .syslog_facility = LOG_LOCAL0 ,	//调用系统日志函数时才用到，现在我们不用
		};
	
[执行流程]
	参考测试程序main.c
	直接调用debug函数即可，第一个参数是打印等级，后面为可变参数(后面参数和printf使用一样)
