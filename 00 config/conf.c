#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <syslog.h>
#include <unistd.h>

#include "conf.h"

#define CONFPRT printf

#define CONFDBG(level, format...) do{}while(0);


/** @internal
 * Holds the current configuration of the gateway */
static s_config config;

/** @internal
 The different configuration options */
typedef enum {
    oBadOption,
    oDaemon,
    oDebugLevel,
} OpCodes;

/** @internal
 The config file keywords for the different configuration options */
static const struct {
    const char *name;
    OpCodes opcode;
} keywords[] = {
    {"daemon", oDaemon}, 
    {"debuglevel", oDebugLevel},
	{NULL, oBadOption},
};



/** Duplicates a string or die if memory cannot be allocated
 * @param s String to duplicate
 * @return A string in a newly allocated chunk of heap.
 */
char * safe_strdup(const char *s)
{
    char *retval = NULL;
    if (!s) 
	{
        CONFDBG(LOG_CRIT, "safe_strdup called with NULL which would have crashed strdup. Bailing out");
        exit(1);
    }
    retval = strdup(s);
    if (!retval) 
	{
        CONFDBG(LOG_CRIT, "Failed to duplicate a string: %s.  Bailing out", strerror(errno));
        exit(1);
    }
    return (retval);
}

s_config * config_get_config(void)
{
    return &config;
}

/** @internal
Parses a boolean value from the config file
*/
static int parse_boolean_value(char *line)
{
    if (strncasecmp(line, "yes",3) == 0) {
        return 1;
    }
    if (strncasecmp(line, "no",2) == 0) {
        return 0;
    }
    if (strcmp(line, "1") == 0) {
        return 1;
    }
    if (strcmp(line, "0") == 0) {
        return 0;
    }

    return -1;
}

/** @internal
Parses a single token from the config file
*/
static OpCodes config_parse_token(const char *cp, const char *filename, int linenum)
{
    int i;

    for (i = 0; keywords[i].name; i++)
        if (strcasecmp(cp, keywords[i].name) == 0)
            return keywords[i].opcode;

    CONFDBG(LOG_ERR, "%s: line %d: Bad configuration option: %s", filename, linenum, cp);
    return oBadOption;
}

/** Sets the default config parameters and initialises the configuration system */
void config_init(void)
{
    CONFDBG(LOG_DEBUG, "Setting default config parameters");
    config.configfile = safe_strdup(DEFAULT_CONFIGFILE);
}

/**
@param filename Full path of the configuration file to be read 
*/
void config_read(const char *filename)
{
    FILE *fd;
    char line[MAX_BUF], *s, *p1, *p2;
    int linenum = 0, opcode, value;
    size_t len;

    CONFDBG(LOG_INFO, "Reading configuration file '%s'", filename);

    if (!(fd = fopen(filename, "r"))) 
	{
        CONFDBG(LOG_ERR, "Could not open configuration file '%s', " "exiting...", filename);
        exit(1);
    }

    while (!feof(fd) && fgets(line, MAX_BUF, fd)) 
	{
        linenum++;
        s = line;

        if (s[strlen(s) - 1] == '\n')
            s[strlen(s) - 1] = '\0';

		/** skip leading blank spaces **/
		for (; isblank(*s); s++) ;

		/** get config option **/
        if ((p1 = strchr(s, ' '))) {
            p1[0] = '\0';
        } else if ((p1 = strchr(s, '\t'))) {
            p1[0] = '\0';
        }

        if (p1) 
		{
            p1++;

            // Trim leading spaces
            len = strlen(p1);
            while (*p1 && len) {
                if (*p1 == ' ')
                    p1++;
                else
                    break;
                len = strlen(p1);
            }

			/** Strip trailing spaces **/
            if ((p2 = strchr(p1, ' '))) {
                p2[0] = '\0';
            } else if ((p2 = strstr(p1, "\r\n"))) {
                p2[0] = '\0';
            } else if ((p2 = strchr(p1, '\n'))) {
                p2[0] = '\0';
            }
        }

        if (p1 && p1[0] != '\0') 
		{
            if ((strncmp(s, "#", 1)) != 0) 
			{
                CONFDBG(LOG_DEBUG, "Parsing token: %s, " "value: %s", s, p1);
                opcode = config_parse_token(s, filename, linenum);

                switch (opcode) 
				{
                case oDaemon:
					//printf("Parsing token: %s, " "value: %s\n", s, p1);

                    config.daemon = parse_boolean_value(p1);
                    break;

				case oDebugLevel:
					//printf("Parsing token: %s, " "value: %s\n", s, p1);
					config.debuglevel = atoi(p1);
					break;
						
                
                case oBadOption:
                    /* FALL THROUGH */
                default:
                    printf("Bad option on line %d " "in %s>>", linenum, filename);
					printf("Parsing token: %s, " "value: %s\n", s, p1);

					//CONFDBG(LOG_ERR, "Exiting...");
                    //exit(-1);	/**I think we can't exit event read unknow config options**/
                    break;
                }
            }
        }
    }

    fclose(fd);
}



