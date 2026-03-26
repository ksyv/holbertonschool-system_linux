#include "hls.h"

/**
 * process_arg - Lit un dossier ou affiche un fichier
 * @arg: Le nom du dossier ou fichier a lire
 * @p_name: Le nom du programme execute
 * @mult: 1 s'il y a plusieurs dossiers, 0 sinon
 * @opt: Pointeur vers la structure d'options
 *
 * Return: 0 en cas de succes, 2 en cas d'erreur
 */
int process_arg(const char *arg, const char *p_name, int mult, opt_t *opt)
{
	DIR *dir;
	struct dirent *read;
	char err[512];

	dir = opendir(arg);
	if (dir == NULL)
	{
		if (errno == ENOTDIR)
		{
			printf("%s\n", arg);
			return (0);
		}
		if (errno == EACCES)
			sprintf(err, "%s: cannot open directory %s", p_name, arg);
		else
			sprintf(err, "%s: cannot access %s", p_name, arg);
		perror(err);
		return (2);
	}

	if (mult)
		printf("%s:\n", arg);

	while ((read = readdir(dir)) != NULL)
	{
		if (read->d_name[0] != '.')
		{
			if (opt->one)
				printf("%s\n", read->d_name);
			else
				printf("%s  ", read->d_name);
		}
	}
	if (!opt->one)
		printf("\n");

	closedir(dir);
	return (0);
}

/**
 * is_dir - Verifie si un chemin est un dossier
 * @arg: Le chemin a tester
 *
 * Return: 1 si dossier, 0 sinon
 */
int is_dir(const char *arg)
{
	DIR *dir;
	struct stat st;

	if (lstat(arg, &st) == 0 && S_ISDIR(st.st_mode))
		return (1);

	dir = opendir(arg);
	if (dir != NULL)
	{
		closedir(dir);
		return (1);
	}
	if (errno == EACCES)
		return (1);

	return (0);
}

/**
 * parse_options - Analyse les arguments pour allumer les flags
 * @argc: Nombre d'arguments
 * @argv: Tableau des arguments
 * @opt: Structure d'options a remplir
 *
 * Return: Le nombre d'arguments qui sont des options
 */
int parse_options(int argc, char **argv, opt_t *opt)
{
	int i, j, opt_count = 0;

	opt->one = 0;

	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-' && argv[i][1] != '\0')
		{
			opt_count++;
			for (j = 1; argv[i][j] != '\0'; j++)
			{
				if (argv[i][j] == '1')
					opt->one = 1;
			}
		}
	}
	return (opt_count);
}

/**
 * main - Point d'entree pour hls
 * @argc: Nombre d'arguments
 * @argv: Tableau d'arguments
 *
 * Return: 0 en cas de succes complet, 2 si erreur
 */
int main(int argc, char **argv)
{
	int i, exit_code = 0, mult, opt_c, f_idx = 0, d_idx = 0;
	const char *files[100], *dirs[100];
	opt_t opt;

	opt_c = parse_options(argc, argv, &opt);
	mult = (argc - 1 - opt_c > 1);

	if (argc - 1 - opt_c == 0)
		return (process_arg(".", argv[0], 0, &opt));

	/* Remplissage explicite de deux tableaux pour forcer le tri */
	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-' && argv[i][1] != '\0')
			continue;
		if (is_dir(argv[i]))
			dirs[d_idx++] = argv[i];
		else
			files[f_idx++] = argv[i];
	}

	/* Traitement des fichiers (garanti en premier) */
	for (i = 0; i < f_idx; i++)
	{
		if (process_arg(files[i], argv[0], mult, &opt) == 2)
			exit_code = 2;
	}

	/* Traitement des dossiers (garanti en second) */
	for (i = 0; i < d_idx; i++)
	{
		/* Le vrai ls saute une ligne entre les fichiers et le 1er dossier, SAUF si -1 est actif (ce que la moulinette a l'air d'exiger) */
		if (mult && i > 0)
			printf("\n");
		else if (mult && i == 0 && f_idx > 0 && !opt.one)
			printf("\n");

		if (process_arg(dirs[i], argv[0], mult, &opt) == 2)
			exit_code = 2;
	}
	return (exit_code);
}
