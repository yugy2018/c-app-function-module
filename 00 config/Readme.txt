关于读配置模块app记录说明：

[关键配置说明]
	1.配置文件路径宏定义：conf.h :#define DEFAULT_CONFIGFILE "./yugy.conf"
	2.配置文件配置项关键词 conf.c : keywords[]	根据app需求自定义添加
	3.配置内容读取后存到 ：conf.c ：静态全局变量config

[执行流程]
	参考测试程序main.c
	1.获取配置全局变量（config_get_config）
	2.初始化配置全局变量（config_init）
	3.读取配置文件内容（config_read）
