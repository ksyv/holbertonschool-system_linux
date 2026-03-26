#include "hls.h"

/**
 * process_arg - Lit un dossier ou affiche un fichier
 * @arg: Le nom du dossier ou fichier à lire
 * @prog_name: Le nom du programme exécuté (argv[0])
 * @multiple: 1 s'il y a plusieurs dossiers, 0 sinon
 * @opt: Pointeur vers la structure d'options
 *
 * Return: 0 en cas de succès, 2 en cas d'erreur
 */
int process_arg(const char *arg, const char *prog_name, int multiple, opt_t *opt)
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
			/* C'est ICI que l'option -1 entre en jeu ! */
			if (opt->one)
				printf("%s\n", read->d_name);
			else
				printf("%s  ", read->d_name);
		}
	}
	/* Si on n'est pas en mode -1, on doit faire un retour à la ligne à la fin */
	if (!opt->one)
		printf("\n");

	closedir(dir);
	return (0);
}

/**
 * parse_options - Analyse les arguments pour allumer les bons flags
 * @argc: Nombre d'arguments
 * @argv: Tableau des arguments
 * @opt: Structure d'options à remplir
 *
 * Return: Le nombre d'arguments qui sont des options (commençant par '-')
 */
int parse_options(int argc, char **argv, opt_t *opt)
{
	int i, j, opt_count = 0;

	opt->one = 0; /* On initialise tout à 0 (faux) par défaut */

	for (i = 1; i < argc; i++)
	{
		/* Si le mot commence par '-' et n'est pas juste un tiret vide */
		if (argv[i][0] == '-' && argv[i][1] != '\0')
		{
			opt_count++;
			/* On parcourt chaque lettre de l'option */
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
 * main - Point d'entrée pour hls
 * @argc: Nombre d'arguments
 * @argv: Tableau d'arguments
 *
 * Return: 0 en cas de succès complet, 2 si une erreur est survenue
 */
int main(int argc, char **argv)
{
	int i, exit_code = 0, multiple, opt_count, file_count;
	int first_dir = 1;
	opt_t opt;

	/* 1. On scanne toutes les options tapées */
	opt_count = parse_options(argc, argv, &opt);
	
	/* 2. On déduit combien il reste de vrais noms de dossiers/fichiers */
	file_count = (argc - 1) - opt_count;
	multiple = (file_count > 1);

	/* 3. S'il n'y a que des options et pas de dossier, on lit "." par défaut */
	if (file_count == 0)
	{
		exit_code = process_arg(".", argv[0], 0, &opt);
	}
	else
	{
		/* 4. On relit argv, mais on ignore les options cette fois */
		for (i = 1; i < argc; i++)
		{
			if (argv[i][0] != '-' || argv[i][1] == '\0')
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
