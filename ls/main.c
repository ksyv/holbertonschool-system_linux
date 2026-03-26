#include "hls.h"

/**
 * process_arg - Lit un dossier ou affiche un fichier
 * @arg: Le nom du dossier ou fichier à lire
 * @prog_name: Le nom du programme exécuté (argv[0])
 * @multiple: 1 s'il y a plusieurs arguments, 0 sinon
 *
 * Return: 0 en cas de succès, 2 en cas d'erreur
 */
int process_arg(const char *arg, const char *prog_name, int multiple)
{
	DIR *dir;
	struct dirent *read;
	char err_buf[512]; /* Buffer pour fabriquer le message d'erreur */

	dir = opendir(arg);
	if (dir == NULL)
	{
		/* Si c'est juste un fichier, on l'affiche et c'est un succès */
		if (errno == ENOTDIR)
		{
			printf("%s\n", arg);
			return (0);
		}
		/* Sinon, c'est une vraie erreur (ex: dossier introuvable) */
		sprintf(err_buf, "%s: cannot access %s", prog_name, arg);
		perror(err_buf);
		return (2); /* ls renvoie 2 pour ce type d'erreur */
	}

	/* Si plusieurs dossiers ont été demandés, on affiche le nom du dossier avant */
	if (multiple)
		printf("%s:\n", arg);

	while ((read = readdir(dir)) != NULL)
	{
		if (read->d_name[0] != '.')
			printf("%s  ", read->d_name);
	}
	printf("\n");
	closedir(dir);
	return (0);
}

/**
 * main - Point d'entrée pour hls
 * @argc: Nombre d'arguments
 * @argv: Tableau des arguments
 *
 * Return: 0 en cas de succès complet, 2 si une erreur est survenue
 */
int main(int argc, char **argv)
{
	int i;
	int exit_code = 0;
	int multiple = (argc > 2); /* Vrai si l'utilisateur a passé 2 dossiers ou plus */

	if (argc == 1)
	{
		/* Aucun argument : on lit le dossier courant */
		exit_code = process_arg(".", argv[0], 0);
	}
	else
	{
		/* On boucle sur tous les arguments tapés */
		for (i = 1; i < argc; i++)
		{
			/* Si un seul dossier échoue, le programme entier devra renvoyer 2 */
			if (process_arg(argv[i], argv[0], multiple) == 2)
				exit_code = 2;
			
			/* Espacement visuel entre les différents dossiers */
			if (multiple && i < argc - 1)
				printf("\n");
		}
	}
	return (exit_code);
}
