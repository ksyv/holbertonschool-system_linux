#ifndef HLS_H
#define HLS_H

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>

/**
 * struct opt_s - Structure pour stocker les options activées
 * @one: 1 si l'option -1 est activée, 0 sinon
 */
typedef struct opt_s
{
	int one;
} opt_t;

int process_arg(const char *arg, const char *prog_name, int multiple, opt_t *opt);
int parse_options(int argc, char **argv, opt_t *opt);

#endif /* HLS_H */
