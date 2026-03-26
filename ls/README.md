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

### Tâche 1 : Maybe some parameters? (`hls [FILE]...`)
**Objectif :** Permettre au programme de prendre un ou plusieurs noms de dossiers/fichiers en paramètres, et gérer le formatage strict des erreurs sur `stderr`.

**Concepts clés et fonctions :**
* `argc` et `argv` : Utilisation des paramètres de la fonction `main` pour itérer sur les arguments de la ligne de commande.
* `errno` et `ENOTDIR` : Si `opendir()` échoue, on vérifie la valeur de la variable globale `errno`. Si elle vaut `ENOTDIR`, l'argument fourni est un fichier valide et non un dossier. Le programme affiche alors simplement le nom du fichier.
* `sprintf` combiné à `perror` : Pour reproduire scrupuleusement le message d'erreur natif de `ls` (ex: `./hls: cannot access folder: No such file or directory`) sans utiliser la fonction interdite `strerror()`, on utilise `sprintf` pour formater un préfixe personnalisé que l'on passe ensuite à `perror()`.
* **Exit code :** Le programme stocke l'état d'échec d'un des paramètres et s'assure de retourner `2` à la fin de l'exécution si une erreur d'accès est survenue, respectant ainsi le comportement natif.
