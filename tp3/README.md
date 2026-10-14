# TP3 : Flux (corrigé)

Tous les programmes sont en C, commentés ligne par ligne, et compilés sans avertissement avec `gcc -Wall -Wextra`.
Ils ont été testés avec les commandes du sujet et les fichiers de `/pub/FISE_OSSE11/syscall`.
Les numéros de diapos renvoient au cours `sys-trans2.pdf`.

**Uniquement les fonctions du cours.**
- **Ce qu'on utilise :**
  - flux noyau : `open`, `close`, `read`, `write`, `lseek`, `dup`, `dup2` (diapos 24 à 30) ;
  - flux libc : `fopen`, `fclose`, `fread`, `fwrite`, `feof`, `fflush`, `fprintf` (diapos 40 à 47) ;
  - divers : `setenv` (diapo 62), `system` (diapo 64), `malloc`/`free`, et `errno` + `strerror` pour les erreurs (diapos 17-18).
- **Exceptions imposées par le sujet :** `fstat` (ex 1 Q3), `fseek`/`ftell` (ex 2), `execv` (ex 4), `execl` (ex 5) et `ttyname` (ex 6).
- **Ce qu'on évite :** `stat` et `access` ne sont pas dans le cours. Pour vérifier un fichier, on regarde donc **l'échec des appels système eux-mêmes**. Ils renvoient -1 (ou `NULL`), et `errno` donne la raison précise.

Format des erreurs, comme dans le cours (diapo 18) :
```c
fprintf(stderr, "%s: Fatal: ...: %s\n", argv[0], ..., strerror(errno));
return 1;
```

## Compilation

```sh
# Exercices sans lire.c
for p in delLastByte delLastByte-v2 delLastByte-libc numbering numbering-file \
         iacmp readpasswd1 readpasswd check_parentheses check_parentheses2 ; do
    gcc -Wall -Wextra -o $p $p.c
done
# Exercice 5 : chaque programme est lié avec lire.c fourni
for p in lire lire3exec lire3a lire3b lire3c ; do
    gcc -Wall -Wextra -o $p /pub/FISE_OSSE11/syscall/lire.c $p.c
done
```

## Fichiers

| Exercice | Fichier(s) | Rôle |
|---|---|---|
| 1 | `delLastByte.c` `delLastByte-v2.c` | enlever le dernier octet (flux noyau, `lseek` puis `fstat`) |
| 2 | `delLastByte-libc.c` | idem en flux libc |
| 3 | `numbering.c` `numbering-file.c` | numéroter les lignes (examen 2023-2024) |
| 4 | `iacmp.c` | wrapper avec `setenv` et `execv` |
| 5 | `lire.c` `lire3exec.c` `lire3a.c` `lire3b.c` `lire3c.c` | `execl`, `dup`, `dup2` |
| 6 | `readpasswd1.c` `readpasswd.c` | lecture d'un mot de passe |
| 7 | `check_parentheses.c` `check_parentheses2.c` | vérifier le parenthésage (examen 2024-2025) |

---

## Exercice 1 : flux noyau

```sh
printf 'abc\n' > t
./delLastByte t ; od -c t        # a b c   (le '\n' a disparu)
./delLastByte /nonexist          # ... for writing: No such file or directory
./delLastByte /tmp               # ... for writing: Is a directory
```

**Q1 : `delLastByte.c`.** Il suit les étapes a à g de l'énoncé. Les vérifications sur `f` se font **sans `stat` ni `access`** :

| Défaut de `f` | Détecté par | `strerror(errno)` |
|---|---|---|
| n'existe pas | `open(f, O_WRONLY)` | No such file or directory |
| pas le droit w | `open(f, O_WRONLY)` | Permission denied |
| répertoire | `open(f, O_WRONLY)` | Is a directory |
| pas le droit r | `open(f, O_RDONLY)` | Permission denied |
| pas régulier (FIFO, tty) | `lseek` (diapo 21 : seulement sur un flux qui le supporte) | Illegal seek |
| vide | `t == 0` | |

- **Pourquoi tester l'écriture en premier ?** Le test se fait avec `open(f, O_WRONLY)`, **sans `O_TRUNC`**, ce qui ne modifie pas le fichier. Si on ne découvrait l'absence du droit w qu'à l'étape e, ce ne serait pas grave non plus : l'`open` avec `O_TRUNC` échouerait avant de vider quoi que ce soit.
- **Ne jamais vider le fichier si la lecture a échoué.** On vérifie que `read` a bien renvoyé `t-1` **avant** la réouverture avec `O_TRUNC`. Sinon, on perdrait le contenu.
- **Limite.** Sur une FIFO, `open` est **bloquant** (diapo 20) : il attend qu'un autre processus ouvre l'autre bout. Le programme reste donc bloqué au lieu d'afficher « not a regular file ». C'est sans conséquence pour le TP, où `f` est un fichier ordinaire.

**Q3 : `fstat` (`delLastByte-v2.c`).** Comme pour `scanf`, on passe **l'adresse** d'une variable que la fonction remplit :
```c
struct stat st;
fstat(fd, &st);          // remplit st
off_t t = st.st_size;    // taille en octets
```
Bonus : `st.st_mode` donne le type du fichier. `S_ISREG(st.st_mode)` vérifie proprement qu'il est régulier, au lieu de compter sur l'échec de `lseek`.

## Exercice 2 : flux libc

`delLastByte-libc.c` fait le même travail avec les équivalents libc :

| Noyau | libc |
|---|---|
| `open(f, O_RDONLY)` | `fopen(f, "r")` |
| `open(f, O_WRONLY \| O_TRUNC)` | `fopen(f, "w")` |
| test lecture + écriture | `fopen(f, "r+")` : rw, sans troncature ni création (diapo 42) |
| `lseek(fd, 0, SEEK_END)` | `fseek(fp, 0, SEEK_END)` puis `ftell(fp)` |
| `read(fd, tab, t-1)` | `fread(tab, t-1, 1, fp)` : renvoie le nombre d'**éléments** (1 attendu) |
| `close(fd)` | `fclose(fp)` |

Point important : `fwrite` écrit dans le **tampon** de la libc. Le vrai `write` n'a lieu qu'au `fclose` (diapo 43). Une erreur d'écriture, comme un disque plein, n'est donc visible **qu'au retour de `fclose`**, et c'est pour ça qu'on le teste.

## Exercice 3 : numérotation des lignes (examen 2023-2024)

```sh
{ echo 'first line' ; echo 'second line' ; } | ./numbering
# 1       first line
# 2       second line
```

**Q1 : `numbering.c`.**
- Le programme lit un caractère à la fois avec `fread(&c, 1, 1, stdin)`.
- Une variable `debut` vaut 1 quand le prochain caractère est le premier d'une ligne. Dans ce cas, on écrit d'abord `"%d\t"`.
- Après un `'\n'`, on incrémente le numéro et on remet `debut` à 1.
- Avec cette méthode, une ligne vide est numérotée elle aussi, et aucun numéro n'est écrit après le dernier `'\n'`.
- `fread` renvoie 0 aussi bien en fin de fichier qu'en cas d'erreur. `feof` permet de faire la différence (diapos 44-45).

**Q2 : lire un fichier passé en argument.** Voir `numbering-file.c`, qui marque chaque modification avec « Q2 ».
1. `main(int argc, char *argv[])`, et vérifier que `argc == 2`.
2. `FILE *f = fopen(argv[1], "r");`, puis tester `f == NULL` et afficher le message d'erreur avec `strerror(errno)`.
3. Remplacer `stdin` par `f` dans `fread` et `feof`.
4. Ajouter `fclose(f)` à la fin.

## Exercice 4 : wrapper en C

```sh
./iacmp 'chat roux' 'chat blanc' 'chat noir'
```

Le code complété :
```c
setenv("IacmpDir",        "/pub/FISE_OSSE11/shell/iacmp",     1);
setenv("LD_LIBRARY_PATH", "/pub/FISE_OSSE11/shell/iacmp/lib", 1);
execv("/pub/FISE_OSSE11/shell/iacmp/iacmp", argv);
fprintf(stderr, "%s: execv failed: %s\n", argv[0], strerror(errno));
return 1;
```

- **`setenv`.** Le 3e argument (`1`) écrase la valeur si la variable existe déjà. L'environnement modifié est transmis par `execv` (diapo 62).
- **`execv` reçoit `argv` tel quel.** Comme `argv[argc]` vaut `NULL`, ça marche quel que soit le nombre d'arguments. C'est l'équivalent C du `exec ... "$@"` du TP2.
- **Chemin complet obligatoire.** `execv` ne cherche pas dans le `PATH`, contrairement à `execvp` (diapo 57).
- **Le code après `execv` ne s'exécute qu'en cas d'échec.** En cas de succès, il n'y a pas de retour (diapo 56).

## Exercice 5 : `execl`, `dup`, `dup2`

**Q1 : que fait `lire` ?**
- `read(0, tab, 10)` lit **au plus 10 octets** sur le descripteur **0** (entrée standard) et les range dans `tab`.
- `write(1, tab, nbCarLus)` les recopie sur le descripteur **1** (sortie standard).
- Le message `lire(): nbCarLus = ...` est écrit avec `fprintf(stderr, ...)`, donc sur le descripteur **2**. C'est pour cela qu'il reste à l'écran quand on redirige `> data.out`.
- 0 et 1 ne sont pas des fichiers précis : ce sont les numéros de la table des flux du processus (diapo 24). Ils désignent ce que le shell y a mis (terminal, fichier, pipe).
- `data.in` contient des lignes de 10 octets (9 lettres + `'\n'`). Chaque `lire()` recopie donc exactement une ligne.

**Q2 : `lire.c`.** Le code est celui de l'énoncé.

**Q3 : `lire3exec.c`.** Il appelle `lire()` deux fois, puis `execl("./lire", "lire", (char *)NULL)`.
- **c. Comment sont transmis les flux lors de `execl` ?** La table des descripteurs fait partie du processus, et `exec` ne remplace que le code et les données (l'espace virtuel). Les descripteurs 0, 1 et 2 sont donc **hérités tels quels** : mêmes flux, **même curseur**.
  - Le `lire()` de `./lire` reprend `data.in` là où les deux premiers se sont arrêtés (`ccccccccc`).
  - Il écrit à la suite dans `data.out`.
- **Piège.** Les tampons de la **libc**, eux, sont dans l'espace virtuel et sont **perdus** par `exec`. Si `lire()` utilisait `printf` sans `fflush`, les données en attente disparaîtraient. Ici, `lire()` utilise `write`, et `stderr` n'est pas tamponné (diapo 47), donc rien ne se perd.
- `./lire` doit être dans le répertoire courant : `execl` ne cherche pas dans le `PATH`.

**Q4 : `lire3a.c`.**
```c
int fin  = open(argv[1], O_RDONLY);
int fout = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
dup2(fin, 0);   // 0 accède au fichier argv[1]
dup2(fout, 1);  // 1 accède au fichier argv[2]
close(fin);  close(fout);   // plus utiles : les flux restent ouverts via 0 et 1
```
`dup2(old, new)` fait que `new` désigne le même flux que `old`, en fermant `new` s'il était ouvert (diapo 30). `lire()` ne change pas : il lit toujours le descripteur 0, mais 0 désigne maintenant le fichier. C'est exactement ce que fait le shell pour `< fichier` et `> fichier`.

**Q5 : `lire3b.c`.** Les `dup2` sont placés **après** le 1er `lire()`. Le 1er appel lit donc le pipe (`111111111`) et écrit à l'écran. Les deux suivants, dont celui de `./lire` qui hérite des descripteurs, travaillent sur les fichiers.

**Q6 : `lire3c.c`.** Pour revenir aux flux d'origine, il faut les avoir **sauvegardés**. Une fois le `dup2` fait, plus aucun descripteur ne désigne le pipe ni le terminal.
```c
int save0 = dup(0);   // nouveau descripteur (le 1er libre) sur le même flux que 0
int save1 = dup(1);
dup2(fin, 0);  dup2(fout, 1);
lire();               // 2e appel : sur les fichiers
dup2(save0, 0);       // 0 redevient le pipe
dup2(save1, 1);       // 1 redevient le terminal
close(save0);  close(save1);
execl("./lire", ...); // 3e appel : flux d'origine
```
Le 3e `lire()` lit `222222222`. Le pipe a gardé sa position : le 1er `lire()` en a consommé exactement 10 octets.

## Exercice 6 : lecture d'un mot de passe

**Q1 et Q2 : `readpasswd1.c`.**
- La boucle `read(0, &c, 1)` s'arrête sur `'\n'`, `'\r'` ou à la fin du flux.
- Seuls les 15 premiers caractères sont rangés, mais **tous** sont comptés.
- Le `'\0'` final va dans `pass[15]` si `n >= 16`, et dans `pass[n]` sinon.

```sh
echo 0123456789abcdefghij | ./readpasswd1   # 20 caracteres lus, mot de passe = '0123456789abcde'
```

**Q3 à Q6 : `readpasswd.c`.**
- `ttyname(0)` donne le chemin du terminal, par exemple `/dev/pts/3`. On l'ouvre en `O_RDWR`.
- `system("/bin/stty raw -echo")` passe le terminal en mode raw sans écho. `stty` hérite de notre entrée standard, qui est ce terminal.
- On lit le mot de passe, puis `system("/bin/stty -raw echo")` remet le mode normal.
- **Q4.** On fait `write(tty, "*", 1)` pour chaque caractère reçu, sauf le saut de ligne. En mode raw, `<ENTER>` envoie `'\r'` : c'est pour cela qu'on teste aussi `'\r'`.
- **Q6.** Le caractère 127 correspond à backspace. S'il n'y a rien à effacer (`n == 0`), on l'ignore. Sinon, on fait `n -= 1` et on écrit `"\b \b"` : on recule, on écrase l'étoile par un espace, puis on recule encore.
- **Toujours remettre le terminal en mode normal.** Sinon, le shell reste sans écho : on tape à l'aveugle. Pour réparer, taper `stty sane` puis `<ENTER>`.

**Q5 : `echo mon-pass | ./a.out` permet-il d'entrer un mot de passe ?**
- **Non.** L'entrée standard est un pipe, pas un terminal. `ttyname(0)` renvoie `NULL`, et le programme s'arrête avec `standard input is not a terminal`.
- Sans ce test, `stty` échouerait de toute façon : il n'y a pas de terminal à configurer.
- Ce comportement est voulu : un mot de passe doit être tapé par un humain, pas écrit dans un script ou dans l'historique du shell.
- `readpasswd1`, lui, l'accepte : il n'utilise pas le terminal.

**Pourquoi pas `execv`/`execl` au lieu de `system` ?** `exec` **remplace** le processus par `stty`, et il n'y a pas de retour. Le programme serait terminé avant d'avoir lu le mot de passe. Il faudrait d'abord créer un **autre** processus pour y lancer `stty`, puis attendre sa fin : c'est `fork` + `exec` + `wait`, vu au cours suivant. C'est d'ailleurs ce que fait `system` en interne.

## Exercice 7 : vérification du parenthésage (examen 2024-2025)

```sh
./check_parentheses2 file.txt      # error: line 4, column 2: ')' is unexpected
./check_parentheses2 - < file.txt  # même chose sur l'entrée standard
```

**Q1 : `check_parentheses.c`.**
- Le programme lit chaque caractère avec `fread(&c, 1, 1, f)`.
- Le compteur fait `+1` sur `'('` et `-1` sur `')'`.
- S'il devient `< 0`, le programme affiche `error: ')' is unexpected` et s'arrête.
- S'il est `> 0` à la fin, il affiche `error: '(' is unmatched`.
- Les messages vont sur `stderr`.
- Statuts : 0 si le parenthésage est correct, 1 s'il est incorrect, 2 pour un mauvais usage ou un fichier impossible à lire.

**Q2 : ajouts pour la position d'une `')'` en trop.** Voir `check_parentheses2.c`.
1. Deux variables : `int ligne = 1, col = 0;`.
2. Pour chaque caractère lu, faire `col += 1` **avant** de le traiter.
3. Après un `'\n'`, faire `ligne += 1; col = 0;`.
4. Dans le message de l'étape (3), afficher `ligne` et `col`. C'est la position de la `')'` qui vient d'être lue.

**Q3 : argument `"-"`.**
```c
FILE *f;
if (strcmp(argv[1], "-") == 0)
    f = stdin;                 // déjà ouvert : pas de fopen
else
    f = fopen(argv[1], "r");   // + test de NULL comme avant
...
if (f != stdin) fclose(f);     // on ne ferme que ce qu'on a ouvert
```
Le reste du code ne change pas, puisqu'il ne travaille que sur `f`.

**Q4 : pourquoi une `'('` en trop est-elle plus difficile à situer ?**
- Une `')'` en trop est détectée **au moment où on la lit** : sa position est la position courante.
- Une `'('` en trop n'est détectée **qu'à la fin du fichier**, quand le compteur est encore `> 0`. Sa position est alors oubliée depuis longtemps.
- De plus, on ne sait pas laquelle signaler : le compteur dit seulement *combien* de `'('` sont ouvertes, pas *lesquelles*. Dans `( ( )`, c'est la 1re qui n'est pas fermée, pas la 2e.

**Solution : une pile de positions.**
- Pour chaque `'('`, on **empile** sa position (ligne, colonne).
- Pour chaque `')'`, on **dépile**, car elle ferme la `'('` la plus récente. Si la pile est vide, la `')'` est en trop.
- À la fin, les positions restant dans la pile sont exactement celles des `'('` non fermées. On signale le sommet, la dernière ouverte : c'est `line 2, column 1` dans l'exemple.
- La hauteur de la pile joue le rôle du compteur.

Dans `check_parentheses2.c`, la pile est faite de deux tableaux alloués avec `malloc`. Quand elle est pleine, on alloue deux fois plus grand, on recopie et on libère l'ancienne zone.
Relire le fichier à la fin ne marcherait pas avec `"-"` : un pipe ne se relit pas, puisque `lseek`/`fseek` y sont impossibles.
