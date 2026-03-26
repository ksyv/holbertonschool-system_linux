#include "hls.h"

/**
 * print_long - Affiche les details d'un fichier (format ls -l)
 * @path: Chemin complet vers le fichier pour lstat
 * @name: Nom du fichier a afficher a l'ecran
 */
void print_long(const char *path, const char *name)
{
	struct stat st;
	struct passwd *pwd;
	struct group *grp;
	char *t;

	if (lstat(path, &st) == -1)
		return;

	/* Type de fichier et Permissions */
	printf("%c", S_ISDIR(st.st_mode) ? 'd' : S_ISLNK(st.st_mode) ? 'l' : '-');
	printf("%c", (st.st_mode & S_IRUSR) ? 'r' : '-');
	printf("%c", (st.st_mode & S_IWUSR) ? 'w' : '-');
	printf("%c", (st.st_mode & S_IXUSR) ? 'x' : '-');
	printf("%c", (st.st_mode & S_IRGRP) ? 'r' : '-');
	printf("%c", (st.st_mode & S_IWGRP) ? 'w' : '-');
	printf("%c", (st.st_mode & S_IXGRP) ? 'x' : '-');
	printf("%c", (st.st_mode & S_IROTH) ? 'r' : '-');
	printf("%c", (st.st_mode & S_IWOTH) ? 'w' : '-');
	printf("%c ", (st.st_mode & S_IXOTH) ? 'x' : '-');

	/* Liens, Proprietaire, Groupe, Taille */
	pwd = getpwuid(st.st_uid);
	grp = getgrgid(st.st_gid);
	printf("%lu ", (unsigned long)st.st_nlink);
	printf("%s %s ", pwd ? pwd->pw_name : "unknown", grp ? grp->gr_name : "unknown");
	printf("%lu ", (unsigned long)st.st_size);

	/* Date de modification (ex: "Mar 26 13:01") */
	t = ctime(&st.st_mtime);
	printf("%.12s ", t + 4);

	/* Nom du fichier */
	printf("%s\n", name);
}
