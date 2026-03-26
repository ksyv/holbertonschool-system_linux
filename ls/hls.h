#ifndef HLS_H
#define HLS_H

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <pwd.h>   /* Pour getpwuid */
#include <grp.h>   /* Pour getgrgid */
#include <time.h>  /* Pour ctime */

/**
 * struct opt_s - Structure pour stocker les options activées
 * @one: 1 si l'option -1 est activée, 0 sinon
 * @a: 1 si l'option -a est activée, 0 sinon
 * @A: 1 si l'option -A est activée, 0 sinon
 * @l: 1 si l'option -l est activée, 0 sinon
 */
typedef struct opt_s
{
	int one;
	int a;
	int A;
	int l;
} opt_t;

int process_arg(const char *arg, const char *prog_name, int multiple, opt_t *opt);
int is_dir(const char *arg);
int _strcoll(const char *s1, const char *s2);
int parse_options(int argc, char **argv, opt_t *opt);
void print_long(const char *path, const char *name);

#endif /* HLS_H */
