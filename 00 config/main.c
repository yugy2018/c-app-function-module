#include <stdio.h>

#include "conf.h"

int
main(int argc, char **argv)
{
	s_config *config = config_get_config();

    config_init();

    config_read(config->configfile);

	printf("\nmain() :configfile:%s\n",config->configfile);
	printf("main() :daemon:%d\n",config->daemon);
	printf("main() :debuglevel:%d\n",config->debuglevel);



    return 0;
}

