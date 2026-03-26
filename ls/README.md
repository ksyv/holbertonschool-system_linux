# Projet : C - ls

## Description
Ce projet consiste à recréer la commande système `ls` de zéro en langage C. L'objectif est de comprendre en profondeur le fonctionnement des appels système POSIX pour la gestion des fichiers et des dossiers (directory streams, structures stat, permissions, etc.).

---

## Tâches et Explications

### Tâche 0 : Let's start with something simple! (`hls`)
**Objectif :** Créer un programme basique qui liste le contenu du dossier courant, en ignorant les fichiers cachés.

**Concepts clés et fonctions :**
* `DIR *` et `struct dirent *` : Les types de base de l'API POSIX pour manipuler les dossiers. Le premier est le flux du dossier ouvert, le second contient les infos d'un fichier lu.
* `opendir(".")` : Ouvre le dossier courant (`.`) et renvoie un pointeur (flux de répertoire).
* `readdir(dir)` : Lit l'entrée suivante dans le flux du dossier ouvert. Retourne `NULL` quand la fin du dossier est atteinte.
* `closedir(dir)` : Ferme le flux pour libérer la mémoire allouée par le système. Étape cruciale pour éviter les fuites de mémoire (memory leaks) sous Valgrind.

**Logique d'implémentation :**
1. Ouvrir le dossier avec `opendir`.
2. Vérifier si l'ouverture a réussi (gestion d'erreur avec `perror` si on n'a pas les droits, par exemple).
3. Boucler avec `readdir` tant qu'il y a des éléments à lire.
4. Pour chaque élément lu, vérifier si la première lettre de son nom (`d_name[0]`) est différente de `.` pour exclure les fichiers cachés.
5. Afficher le nom du fichier.
6. Ne jamais oublier de fermer le dossier avec `closedir`.