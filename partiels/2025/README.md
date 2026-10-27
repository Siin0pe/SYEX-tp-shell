# Partiel 6 janvier 2025 (corrigé)

Sujet : `../sujets/OSSE11-Sujet.pdf` (les pages paires du PDF sont blanches). Durée 1h30, quatre exercices indépendants.

| Exercice | Fichier(s) | Rôle |
|---|---|---|
| 2 | `count_by_pattern.sh` | nombre de fichiers par motif |
| 3 | `../../tp3/check_parentheses.c` `../../tp3/check_parentheses2.c` | même exercice que le TP3 (exercice 7) |
| 4 | `tester.c` | questions 4.1 à 4.3 (squelette complété) |
| 4 | `tester2.c` | questions 4.4 à 4.6 (waitpid, pipe, échec de `execl`) |

```sh
gcc -Wall -Wextra -o tester tester.c
gcc -Wall -Wextra -o tester2 tester2.c
```

---

## Exercice 1 : commandes erronées

**1.1** `wc -l edt.txt` compte les **lignes**. Pour le nombre de caractères : `-c` (octets) ou `-m` (caractères, ce qui diffère en UTF-8 accentué). Avec `<`, wc n'affiche que le nombre, sans le nom du fichier :
```sh
wc -c < edt.txt          # ou wc -m < edt.txt
```

**1.2** `sed -e "/Bob/Alice/g"` : il manque la commande `s` (`/Bob/` est une adresse, suivie de `Alice...` qui n'est pas une commande). Et sans `-i`, sed écrit sur la sortie standard sans modifier le fichier :
```sh
sed -i -e 's/Bob/Alice/g' edt.txt
```

**1.3** Trois erreurs :
- `[ i < 10 ]` : `i` sans `$` est la chaîne « i », et `<` est une redirection. Il faut `[ $i -lt 10 ]`.
- `i=$(i+1)` : `$( )` est une **substitution de commande** (elle lance la commande `i+1`). Le calcul arithmétique s'écrit `$(( ))`.
- `od` au lieu de `done`.
```sh
i=1 ; while [ $i -lt 10 ] ; do echo $i ; i=$((i+1)) ; done
```
(ou simplement `seq 1 9`)

**1.4** Trois erreurs :
- `[ grep ... ]` : `[` est la commande `test`, elle ne lance pas grep. `if` teste directement le statut d'une commande.
- il manque `;` avant `then` ;
- `2>&1` redirige stderr vers stdout : le message part sur la sortie standard. Il faut `>&2`.
```sh
if grep -q -w -e 'TODO' readme.md ; then echo >&2 'error: TODO found' ; fi
```
(`-w` : TODO comme **mot** entier.)

**1.5** `>` est une redirection vers un **fichier** (on créerait les fichiers `cut` et `head`). Il faut des pipes `|`. On garde d'abord les 10 premières lignes, puis on extrait la colonne :
```sh
head -n 10 edt.txt | cut -d ' ' -f 2
```

---

## Exercice 2 : `count_by_pattern.sh`

```sh
#!/bin/sh
for pattern in "$@" ; do
    n=$(find . -type f -name "$pattern" | wc -l)
    echo "'$pattern': $((n)) found"
done
```
- `for pattern in "$@"` : avec zéro argument, la boucle ne fait rien. Les guillemets conservent chaque motif intact.
- `-name "$pattern"` entre guillemets : c'est **find** qui interprète `*.c`, pas le shell. Sans guillemets, le shell remplacerait `*.c` par les fichiers `.c` du répertoire courant.
- `-type f` : seulement les fichiers réguliers.
- Limite : un nom de fichier contenant un retour à la ligne compterait double (`wc -l` compte des lignes).

Test : avec `x.c a/y.c a/z.h b/Makefile Makefile` :
```
'*.c': 2 found
'*.h': 1 found
'Makefile': 2 found
'*.sh': 0 found
```

---

## Exercice 3 : `check_parentheses` (flux libc)

C'est l'exercice 7 du TP3 : le code commenté est dans `tp3/check_parentheses.c` (Q3.1) et `tp3/check_parentheses2.c` (Q3.2 à Q3.4).

**3.1** Compteur `cpt` initialisé à 0 et lecture caractère par caractère avec `fread(&c, 1, 1, f) == 1` : `(` fait `cpt++`, `)` fait `cpt--`. Si `cpt < 0`, on affiche `error: ')' is unexpected` et on quitte. À la fin, on vérifie `feof(f)` (sinon c'est une erreur de lecture). Si `cpt > 0`, on affiche `error: '(' is unmatched`.

**3.2** Ajouts pour la position d'une `)` en trop :
- deux variables `int ligne = 1, col = 0;` ;
- pour chaque caractère lu : `col++` ;
- si le caractère est `'\n'` : `ligne++` et `col = 0` (le caractère suivant est en colonne 1) ;
- le message devient `fprintf(stderr, "error: line %d, column %d: ')' is unexpected\n", ligne, col);`.

**3.3** Argument `"-"` : on lit l'entrée standard au lieu d'ouvrir un fichier.
```c
FILE *f = (strcmp(argv[1], "-") == 0) ? stdin : fopen(argv[1], "r");
...
if (f != stdin) fclose(f);    // on ne ferme pas ce qu'on n'a pas ouvert
```
Le reste du code est inchangé, car il ne manipule que `f`.

**3.4** Une `)` en trop se détecte **au moment où on la lit**, car le compteur passe sous 0 : sa position est la position courante. Une `(` en trop, en revanche, ne se détecte qu'**à la fin du fichier** (`cpt > 0`). À ce moment, la position courante est la fin du fichier, et on a oublié où se trouvait la `(` fautive. Le compteur seul ne suffit pas.
Solution : une **pile** de positions (ligne, colonne). Sur `(`, on empile la position courante. Sur `)`, on dépile (la `(` du sommet est fermée). À la fin, les positions restant dans la pile sont exactement les `(` non fermées : on affiche le sommet, ou toutes. La profondeur n'est pas bornée, donc la pile est un tableau dynamique agrandi par `malloc` + recopie (ou `realloc`) quand il est plein. `cpt` est la hauteur de la pile.

---

## Exercice 4 : programme `tester`

### 4.1 `main`
```c
pid_t pid = fork();
if (pid == -1) { perror("[fork]"); exit(3); }
if (pid == 0)
    child(argv[1], argv[2]);        // ne revient jamais

alarm(60);
pause();

/* clean up */
alarm(0);                           // annule l'alarme en cours
waitpid(pid, NULL, 0);              // récupère le fils (évite le zombie)
```

### 4.2 `child`
```c
int fd = open(inputfile, O_RDONLY);
if (fd == -1) { perror("[open]"); exit(1); }
if (dup2(fd, 0) == -1) { perror("[dup2]"); exit(1); }   // stdin = inputfile
close(fd);                                              // 0 suffit désormais
execl(execfile, execfile, (char*)NULL);                 // argv[0], puis NULL
perror("[execl]");                                      // seulement si échec
exit(1);
```

### 4.3 SIGCHLD ne réveille pas `pause()`
L'action par défaut de SIGCHLD est d'**ignorer** le signal. Un signal ignoré n'interrompt pas `pause()`, qui ne rend la main que si un **handler** s'exécute (ou si le processus meurt). Il faut donc attacher un handler, même vide, à SIGCHLD, **avant le `fork`** : un fils très rapide pourrait sinon se terminer avant l'installation.
```c
void chld_handler(int signum) { (void)signum; }
...
if (signal(SIGCHLD, chld_handler) == SIG_ERR) { perror("[signal]"); exit(2); }
```

### 4.4 Pertinence de `pause()`
`pause()` est **fragile** :
- **course** : si le fils se termine entre le `fork` et l'appel à `pause()` (exécutable très rapide, `execl` qui échoue), SIGCHLD est traité **avant** `pause()`. Le père attend alors jusqu'au SIGALRM et affiche un faux `FAIL: timeout` 60 s plus tard ;
- n'importe quel autre signal muni d'un handler réveille aussi `pause()`. Le commentaire « at this point, we know that the child process has terminated » devient alors faux ;
- `pause()` ne donne pas le statut du fils.

Mieux : **`waitpid(pid, &status, 0)`** à la place de `pause()`. Il bloque jusqu'à la fin du fils, même si elle a déjà eu lieu (le fils est alors zombie), donc sans course. Il récupère le statut, et le handler SIGCHLD devient inutile. Le SIGALRM interrompt toujours l'attente au bout de 60 s, puisque `alarm_handler` fait `exit`. (Variante sans course qui garde l'esprit de `pause` : `sigprocmask` + `sigsuspend`.)

Autre défaut : en cas de timeout, `alarm_handler` quitte **sans tuer le fils**, qui continue de tourner. Il faut garder le PID dans une variable globale et faire `kill(pid, SIGKILL)` dans le handler.

### 4.5 Récupérer la sortie dans le père : un **pipe**
- **Nouvelle variable** : `int p[2];`, locale à `main` (ou globale), créée par **`pipe(p)` avant le `fork`** pour que le fils en hérite.
- **Dans le fils** (`child`, avant `execl`) : `close(p[0])`, puis `dup2(p[1], 1)` pour que la sortie standard devienne le pipe, puis `close(p[1])`. L'exécutable écrit alors dans le pipe sans le savoir.
- **Dans le père** (après le `fork`) : `close(p[1])`. C'est indispensable : sinon `read` ne verra jamais la fin de fichier. Ensuite, boucle `read(p[0], ...)` jusqu'au retour 0, qui signifie que le fils a terminé ou fermé sa sortie. On stocke dans un tampon `char buf[...]`, puis `close(p[0])`, `waitpid`, et on compare le tampon à `"42\n"` (avec `strcmp`, en vérifiant qu'il n'y a qu'une ligne).
- Il faut lire **avant** `waitpid`. Si le fils écrit plus que la capacité du pipe (64 Kio), il bloque, et un père qui attendrait sa fin sans lire serait en interblocage.

### 4.6 Signaler au père l'échec de `execl`
Par le **statut de sortie** du fils : après l'échec de `execl`, le fils fait `exit(127)` (code réservé, c'est la convention du shell pour « commande introuvable »). Le père récupère le statut avec `waitpid(pid, &status, 0)` puis teste `WIFEXITED(status) && WEXITSTATUS(status) == 127`.
Limite : l'exécutable testé pourrait lui-même renvoyer 127. Variante robuste : un second pipe dont le bout d'écriture est marqué *close-on-exec* (`fcntl(fd, F_SETFD, FD_CLOEXEC)`). Si `execl` réussit, il est fermé automatiquement et le père lit 0 octet. S'il échoue, le fils y écrit `errno` avant de quitter.

`tester2.c` réunit 4.4, 4.5 et 4.6. Résultats des tests : `tester2 /bin/cat input.txt` (input = `42`) donne `PASS`, `/bin/true` donne `FAIL: wrong answer`, `/nope` donne `FAIL: cannot run '/nope'`, et un script qui dort 100 s donne `FAIL: timeout` au bout de 60 s.
