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

---

### Tâche 1 : Maybe some parameters? (`hls [FILE]...`)
**Objectif :** Permettre au programme de prendre un ou plusieurs noms de dossiers/fichiers en paramètres, et gérer le formatage strict des erreurs sur `stderr`.

**Concepts clés et fonctions :**
* `argc` et `argv` : Utilisation des paramètres de la fonction `main` pour itérer sur les arguments de la ligne de commande.
* `errno` et `ENOTDIR` : Si `opendir()` échoue, on vérifie la valeur de la variable globale `errno`. Si elle vaut `ENOTDIR`, l'argument fourni est un fichier valide et non un dossier. Le programme affiche alors simplement le nom du fichier.
* `sprintf` combiné à `perror` : Pour reproduire scrupuleusement le message d'erreur natif de `ls` (ex: `./hls: cannot access folder: No such file or directory`) sans utiliser la fonction interdite `strerror()`, on utilise `sprintf` pour formater un préfixe personnalisé que l'on passe ensuite à `perror()`.
* **Exit code :** Le programme stocke l'état d'échec d'un des paramètres et s'assure de retourner `2` à la fin de l'exécution si une erreur d'accès est survenue, respectant ainsi le comportement natif.
* **Précision des messages d'erreur :** `ls` adapte son message selon le type d'erreur. Si `errno` vaut `EACCES` (Permission denied), le message exact doit être `cannot open directory`. Pour les autres erreurs comme `ENOENT` (No such file), le préfixe reste `cannot access`.

---

### Tâche 2 : What about options? (`-1`)
**Objectif :** Implémenter l'option `-1` (qui force l'affichage d'un fichier par ligne) tout en gérant l'ordre aléatoire des arguments dans la commande.

**Concepts clés et implémentation :**
* **L'absence de `getopt` :** La fonction standard d'analyse d'arguments n'étant pas autorisée, nous devons concevoir un *parser* personnalisé.
* **Architecture "Deux passes" :** Le programme effectue une première lecture de `argv` via `parse_options` pour repérer toutes les chaînes commençant par `-` et configurer l'état du programme. La boucle principale dans `main` effectue ensuite une seconde lecture en ignorant les options pour ne traiter que les cibles (fichiers/dossiers).
* **Modularité avec `struct` :** Pour anticiper proprement les tâches futures, une structure `opt_t` est créée. Elle regroupera tous les *flags* (booléens) correspondant à chaque option (`-1`, `-a`, `-l`, etc.). Si le flag `one` est activé, la fonction d'affichage insère un `\n` après chaque fichier au lieu de deux espaces.
* **Ordre d'affichage (Tri implicite) :** Peu importe l'ordre des arguments passés dans le terminal, `ls` affiche toujours les fichiers individuels en premier, puis le contenu des répertoires. Pour reproduire ce comportement, la boucle principale de `main` utilise `lstat()` et la macro `S_ISDIR` pour séparer le traitement en deux passes : une pour les fichiers purs (et les erreurs), une seconde pour les répertoires.
* **Le piège des Liens Symboliques (Symlinks) :** Le tri des arguments cache un piège majeur. La fonction `lstat()` ne suit pas les liens symboliques. Si un dossier est passé sous forme de lien, `lstat` le considèrera comme un fichier standard. Nous avons implémenté un système de "fallback" via `opendir()` dans une fonction `is_dir()` : si `lstat` échoue à prouver que c'est un dossier, on tente de l'ouvrir.

**⚠️ LES PIÈGES INVISIBLES DE LA MOULINETTE (Tâche 2) :**
* **Le mystère de l'octet manquant (`\n`) :** Quand le vrai `ls` affiche des fichiers individuels *avant* des dossiers, il insère toujours **une ligne vide** juste avant d'afficher le titre du tout premier dossier. Sans un flag `has_file` pour gérer cet espace spécifique, le checker rejettera la sortie.

---

### Tâches 3 et 4 : Hidden files (`-a`) et Almost all (`-A`)
**Objectif :** Gérer l'affichage des fichiers cachés avec des niveaux de granularité différents.

**Concepts clés et implémentation :**
* **Extensibilité de l'architecture :** Les options `-a` et `-A` sont ajoutées comme de simples flags booléens dans notre structure `opt_t`.
* **Logique de priorité :** Si l'utilisateur tape `hls -a -A`, le comportement natif de `ls` veut que `-a` prenne le dessus (tout afficher). Notre code gère cela avec la condition `if (opt->A && !opt->a)`.
* **Comparaison de chaînes sans `strcmp` :** La bibliothèque `<string.h>` étant proscrite, l'identification des dossiers spéciaux `.` et `..` se fait via l'inspection directe de la mémoire dans le tableau de caractères `d_name`. On vérifie la position du caractère de fin de chaîne `\0` pour s'assurer que le nom est *exactement* `.` ou `..`.

**⚠️ LES PIÈGES INVISIBLES DE LA MOULINETTE (Tâches 3 & 4) :**
* **Le tri alphabétique des arguments :** `ls` ne traite pas les arguments dans l'ordre où ils sont tapés. Si on tape `ls folder B folder A`, il traitera `A` puis `B`. Les arguments doivent être triés dans un tableau avant d'être envoyés dans les "deux passes".
* **Le tri "Case-Insensitive" (Le Boss Final) :** Attention, le tri natif de Linux ignore la casse ! Dans la table ASCII classique, les majuscules viennent avant les minuscules (`Folder2` serait classé avant `folder1`). Pour contourner ce piège sans utiliser `<string.h>`, nous avons créé une fonction personnalisée `_strcoll` qui abaisse virtuellement la casse des caractères pendant la comparaison, reproduisant ainsi le "dictionary sort" exact attendu par le checker.