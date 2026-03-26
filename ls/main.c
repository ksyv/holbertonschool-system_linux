#include "hls.h"

/**
 * main - Entry point for hls, lists current directory content
 *
 * Return: 0 on success, 1 on failure
 */
int main(void)
{
	DIR *dir;
	struct dirent *read;

	/* 1. On ouvre le dossier courant (".") */
	dir = opendir(".");
	if (dir == NULL)
	{
		perror("hls");
		return (1);
	}

	/* 2. On lit chaque entrée du dossier une par une */
	while ((read = readdir(dir)) != NULL)
	{
		/* Le vrai 'ls' de base cache les fichiers qui commencent par '.' */
		if (read->d_name[0] != '.')
		{
			printf("%s  ", read->d_name);
		}
	}
	printf("\n");

	/* 3. On ferme le dossier proprement pour Valgrind ! */
	closedir(dir);

	return (0);
}
