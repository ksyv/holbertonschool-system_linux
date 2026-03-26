#include "hls.h"

/**
 * process_arg - Lit un dossier ou affiche un fichier
 * @arg: Le nom du dossier ou fichier a lire
 * @prog_name: Le nom du programme execute
 * @multiple: 1 s'il y a plusieurs dossiers, 0 sinon
 * @opt: Pointeur vers la structure d'options
 *
 * Return: 0 en cas de succes, 2 en cas d'erreur
 */
int process_arg(const char *arg, const char *prog_name, int multiple,
opt_t *opt)
{
	DIR *dir;
	struct dirent *read;
	char err_buf[512];

	dir = opendir(arg);
	if (dir == NULL)
	{
		if (errno == ENOTDIR)
		{
			printf("%s\n", arg);
			return (0);
		}
		else if (errno == EACCES)
			sprintf(err_buf, "%s: cannot open directory %s", prog_name, arg);
		else
			sprintf(err_buf, "%s: cannot access %s", prog_name, arg);

		perror(err_buf);
		return (2);
	}

	if (multiple)
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
 * is_dir - Verifie si un chemin est un dossier (gere les symlinks)
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
 * parse_options - Analyse les arguments pour allumer les bons flags
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
	int i, exit_code = 0, multiple, opt_count, file_count;
	int first_dir = 1;
	opt_t opt;

	opt_count = parse_options(argc, argv, &opt);
	file_count = (argc - 1) - opt_count;
	multiple = (file_count > 1);
	if (file_count == 0)
	{
		exit_code = process_arg(".", argv[0], 0, &opt);
	}
	else
	{
		for (i = 1; i < argc; i++)
		{
			if (argv[i][0] == '-' && argv[i][1] != '\0')
				continue;

			if (!is_dir(argv[i]))
			{
				if (process_arg(argv[i], argv[0], multiple, &opt) == 2)
					exit_code = 2;
			}
		}
		for (i = 1; i < argc; i++)
		{
			if (argv[i][0] == '-' && argv[i][1] != '\0')
				continue;
			if (is_dir(argv[i]))
			{
				if (multiple && !first_dir)
					printf("\n");
				if (process_arg(argv[i], argv[0], multiple, &opt) == 2)
					exit_code = 2;
				first_dir = 0;
			}
		}
	}
	return (exit_code);
}
