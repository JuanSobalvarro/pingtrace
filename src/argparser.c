#include "argparser.h"

int parse_arguments(int argc, char *argv[], config_t *cfg)
{
    // default values
    cfg->mode = 0;
    cfg->count = 4;
    cfg->size = 32;
    cfg->timeout_ms = 1000;
    cfg->ttl = 64;

    if (argc < 3)
        return -1;

    // -operation mode
    if (strcmp(argv[1], "-p") == 0)
        cfg->mode = 1;
    else if (strcmp(argv[1], "-t") == 0)
        cfg->mode = 2;
    else
        return -1;

    strcpy_s(cfg->host, sizeof(cfg->host), argv[2]);

    // parse flags
    for (int i = 3; i < argc; i++)
    {
        if (strcmp(argv[i], "-count") == 0 && i + 1 < argc)
            cfg->count = atoi(argv[++i]);

        else if (strcmp(argv[i], "-size") == 0 && i + 1 < argc)
            cfg->size = atoi(argv[++i]);

        else if (strcmp(argv[i], "-timeout") == 0 && i + 1 < argc)
            cfg->timeout_ms = atoi(argv[++i]);

        else if (strcmp(argv[i], "-ttl") == 0 && i + 1 < argc)
            cfg->ttl = (uint8_t) atoi(argv[++i]);

        else
            printf("Unknown option ignored: %s\n", argv[i]);
    }

    return 0;
}
