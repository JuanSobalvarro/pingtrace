#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"

int parse_arguments(int argc, char *argv[], config_t *cfg);

#endif // ARG_PARSER_H