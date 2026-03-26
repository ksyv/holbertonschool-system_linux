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
		if (read->d_name[0] == '.')
		{
			if (!opt->a && !opt->A)
				continue;
			if (opt->A && !opt->a)
			{
				if (read->d_name[1] == '\0')
					continue;
				if (read->d_name[1] == '.' && read->d_name[2] == '\0')
					continue;
			}
		}
		if (opt->one)
			printf("%s\n", read->d_name);
		else
			printf("%s  ", read->d_name);
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
 * _strcmp - Compare deux chaines (sans string.h)
 * @s1: Premiere chaine
 * @s2: Deuxieme chaine
 *
 * Return: Difference entre les caracteres
 */
int _strcmp(const char *s1, const char *s2)
{
	while (*s1 && (*s1 == *s2))
	{
		s1++;
		s2++;
	}
	return (*(unsigned char *)s1 - *(unsigned char *)s2);
}

/**
 * parse_options - Analyse les arguments pour allumer les flags
 * @argc: Nombre d'arguments
 * @argv: Tableau des arguments
 * @opt: Structure d'options a remplir
 *
 * Return: Le nombre d'options trouvees
 */
int parse_options(int argc, char **argv, opt_t *opt)
{
	int i, j, count = 0;

	opt->one = 0;
	opt->a = 0;
	opt->A = 0;

	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-' && argv[i][1] != '\0')
		{
			count++;
			for (j = 1; argv[i][j] != '\0'; j++)
			{
				if (argv[i][j] == '1')
					opt->one = 1;
				else if (argv[i][j] == 'a')
					opt->a = 1;
				else if (argv[i][j] == 'A')
					opt->A = 1;
			}
		}
	}
	return (count);
}

/**
 * main - Point d'entree pour hls
 * @argc: Nombre d'arguments
 * @argv: Tableau d'arguments
 *
 * Return: 0 en cas de succes, 2 si erreur
 */
int main(int argc, char **argv)
{
	int i, j, e_code = 0, mult, opt_c, arg_c = 0;
	int first_dir = 1, has_file = 0;
	const char *args[256];
	opt_t opt;

	opt_c = parse_options(argc, argv, &opt);
	mult = (argc - 1 - opt_c > 1);
	if (argc - 1 - opt_c == 0)
		return (process_arg(".", argv[0], 0, &opt));

	/* 1. Collecter tous les arguments valides */
	for (i = 1; i < argc; i++)
		if (argv[i][0] != '-' || argv[i][1] == '\0')
			args[arg_c++] = argv[i];

	/* 2. Trier les arguments par ordre alphabetique (Bubble sort) */
	for (i = 0; i < arg_c - 1; i++)
		for (j = 0; j < arg_c - i - 1; j++)
			if (_strcmp(args[j], args[j + 1]) > 0)
			{
				const char *tmp = args[j];
				args[j] = args[j + 1];
				args[j + 1] = tmp;
			}

	/* 3. PASSE 1 : Afficher les fichiers purs */
	for (i = 0; i < arg_c; i++)
		if (!is_dir(args[i]))
		{
			if (process_arg(args[i], argv[0], mult, &opt) == 2)
				e_code = 2;
			has_file = 1;
		}

	/* 4. PASSE 2 : Afficher les dossiers */
	for (i = 0; i < arg_c; i++)
		if (is_dir(args[i]))
		{
			if (mult && !first_dir)
				printf("\n");
			else if (has_file && first_dir)
				printf("\n"); /* Le fameux espace manquant ! */

			if (process_arg(args[i], argv[0], mult, &opt) == 2)
				e_code = 2;
			first_dir = 0;
		}
	return (e_code);
}
