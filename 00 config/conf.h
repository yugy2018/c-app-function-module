#ifndef _CONFIG_H_
#define _CONFIG_H_


#define MAX_BUF             4096

#define DEFAULT_CONFIGFILE "./yugy.conf"


typedef struct {
    char *configfile;       /**< @brief name of the config file */
	int daemon;
	int debuglevel;
} s_config;

s_config * config_get_config(void);

void config_init(void);

void config_read(const char *filename);


#endif 	//_CONFIG_H_
