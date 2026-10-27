# Partiel 22 janvier 2024 (corrigé)

Sujet : `../sujets/Partiel_OSSE_2023-2024.pdf`. Durée 1h30, quatre exercices indépendants.

| Exercice | Fichier(s) | Rôle |
|---|---|---|
| 2 | `my_ocamlrun.sh` | wrapper de la figure 1 corrigé |
| 3 | `../../tp3/numbering.c` `../../tp3/numbering-file.c` | même exercice que le TP3 (exercice 3) |
| 4 | `cat_while.c` | équivalent C de `cat \| while read line ...` |

```sh
gcc -Wall -Wextra -o cat_while cat_while.c
```

---

## Exercice 1 : `find`

```sh
# (i) fichiers réguliers *.cpp sous /tmp/src (récursif par défaut)
find /tmp/src -type f -name '*.cpp'
# (ii) leur nombre
find /tmp/src -type f -name '*.cpp' | wc -l
# (iii) les lignes contenant <stdio.h> (-F : chaîne fixe ; -H : nom du fichier)
find /tmp/src -type f -name '*.cpp' -exec grep -H -F '<stdio.h>' {} +
# (iv) remplacement en place (le '.' est échappé : sinon il désigne n'importe quel caractère)
find /tmp/src -type f -name '*.cpp' -exec sed -i -e 's/<stdio\.h>/<cstdio>/g' {} +
```
`'*.cpp'` entre quotes : c'est find qui interprète le motif, pas le shell. `{} +` passe plusieurs fichiers à chaque appel. `{} \;` marche aussi, avec un appel par fichier.

---

## Exercice 2 : wrapper `my_ocamlrun.sh`

Script de la figure 1 :
```sh
1  #!/bin/sh
5  check() {
6      first_char=$(printf '%s' "$1" | sed -e 's/\(.\).*/\1/')
7      if [ "$first_char" = "-" ] ; then
8          return 1
9      fi
10     return 0
11 }
13 if [ $# < 1 ] ; then
14     echo 2>&1 'error: at least one argument expected'
15     exit 1
16 fi
18 for arg in $$ do
19     if [ check $arg ] ; then
20         if [ $arg == '-bt' ] ; then
21             echo 2>&1 "option '-bt' is not valid anymore, use '-b -t' instead"
22
23         fi
24     else
25         break
26     fi
27 done
29 exec ocamlrun
```

### 2.1 Fonctionnement de `check`
- `printf '%s' "$1"` écrit l'argument tel quel, sans retour à la ligne et sans interpréter `-n` ou `\`. C'est pour cela que printf est préféré à echo ici.
- `sed -e 's/\(.\).*/\1/'` : `\(.\)` capture le **premier caractère**, `.*` prend tout le reste, et l'ensemble est remplacé par `\1`, le caractère capturé. On obtient donc le premier caractère de l'argument.
- `$( ... )` le range dans `first_char`.
- si ce caractère est `-`, l'argument est une option et la fonction renvoie **1** (faux). Sinon elle renvoie **0** (vrai).

`check` est donc **vraie pour un argument qui n'est PAS une option**, alors qu'elle est utilisée ligne 19 comme « est une option ». Voir 2.2.

### 2.2 Problèmes et correctifs

| Ligne | Problème | Correctif |
|---|---|---|
| 7-10 | statut inversé par rapport à l'usage ligne 19 : `check` renvoie 0 pour un non-option | `return 0` si `-`, `return 1` sinon (ou garder `check` et écrire `if ! check ...`) |
| 13 | `<` dans `[ ]` est une **redirection** (depuis le fichier `1`) | `[ $# -lt 1 ]` |
| 14, 21 | `2>&1` envoie stderr vers stdout : le message part sur la sortie standard | `echo >&2 '...'` |
| 18 | `$$` est le **PID** du shell, pas la liste des arguments | `"$@"` |
| 18 | il manque `;` (ou un retour à la ligne) avant `do` | `for arg in "$@" ; do` |
| 19 | `[ check $arg ]` : `[` (la commande `test`) ne lance pas la fonction, il reçoit la chaîne `check` | `if check "$arg" ; then` |
| 19, 20 | `$arg` sans guillemets : un argument vide ou contenant des espaces casse le test | `"$arg"` |
| 20 | `==` n'est pas POSIX dans `[ ]` (dash le refuse) | `[ "$arg" = '-bt' ]` |
| 22 | après le message d'erreur, il manque l'arrêt : le script continuerait et lancerait ocamlrun | `exit 1` |
| 29 | `exec ocamlrun` sans arguments : on ne transmet rien | `exec ocamlrun "$@"` |
| — | amélioration : si tous les arguments sont des options, la boucle finit sans `break` et il n'y a pas de `filename` | drapeau `found=yes` posé au `break`, et erreur s'il reste à `no` |
| — | amélioration : `check` peut se passer de sed | `case "$1" in -*) return 0 ;; esac ; return 1` |

Comme `"$@"` n'est jamais modifié, le `exec ocamlrun "$@"` final transmet bien tous les arguments, options comprises. `exec` remplace le shell par ocamlrun, donc pas de processus en plus, et le statut renvoyé est celui d'ocamlrun.

Version corrigée : `my_ocamlrun.sh`. Tests avec un faux `ocamlrun` qui affiche ses arguments :
```
-b -t prog.byte x y  ->  ocamlrun recoit: -b -t prog.byte x y
-bt prog.byte        ->  option '-bt' is not valid anymore, ...   (statut 1)
prog.byte -bt        ->  ocamlrun recoit: prog.byte -bt            (-bt est un argument du programme)
-b                   ->  error: filename expected                  (statut 1)
(rien)               ->  error: at least one argument expected     (statut 1)
```

---

## Exercice 3 : `numbering` (flux libc)

C'est l'exercice 3 du TP3 : `tp3/numbering.c` (Q3.1) et `tp3/numbering-file.c` (Q3.2).

**3.1** On lit caractère par caractère avec `fread(&c, 1, 1, stdin) == 1`. Un drapeau `debut` (initialisé à 1) indique qu'on est au début d'une ligne : on affiche alors `printf("%d\t", num)` avant le caractère. Ensuite, on recopie le caractère. Sur `'\n'`, on fait `num++` et `debut = 1`. À la fin, `feof(stdin)` distingue la fin de fichier d'une erreur. Cette méthode ne limite pas la longueur des lignes, et une dernière ligne sans `\n` est quand même numérotée.

**3.2** Il faut :
1. `int main(int argc, char *argv[])` et vérifier `argc == 2` ;
2. `FILE *f = fopen(argv[1], "r");` et tester `NULL` (message avec `strerror(errno)`) ;
3. remplacer `stdin` par `f` dans `fread` et `feof` ;
4. `fclose(f)` à la fin.

---

## Exercice 4 : `cat | while read line ...`

### 4.1 `left_child` (cat, flux noyau)
```c
close(pipefd[0]);
char c;
while (read(0, &c, 1) == 1)
    write(pipefd[1], &c, 1);
close(pipefd[1]);
exit(0);
```

### 4.2 `right_child` (la boucle while)
```c
close(pipefd[1]);
...
        /* read a character from the read end of pipefd */
        if (read(pipefd[0], &line[index], 1) != 1)
            exit(0);                 // EOF : "read line" échoue, la boucle s'arrête
...
        /* write the expected output on the standard output */
        write(1, "got: '", 6);
        write(1, line, index - 1);   // la ligne sans le '\n'
        write(1, "'\n", 2);
```
(Le test du squelette `index > 1024` laisse écrire `line[1024]`, hors du tableau, au tour suivant. La bonne borne est `index >= 1024`.)

### 4.3 `main`
```c
if (pipe(pipefd) == -1) { perror("pipe"); exit(1); }

pid_t left = fork();
if (left == -1) { perror("fork"); exit(1); }
if (left == 0) left_child();
pid_t right = fork();
if (right == -1) { perror("fork"); exit(1); }
if (right == 0) right_child();

close(pipefd[0]);
close(pipefd[1]);

waitpid(left, NULL, 0);
waitpid(right, NULL, 0);
```

### 4.4 Nombre de processus
- **Avant** le traitement de la ligne `q` : **3** (main, left_child, right_child), sans compter le shell qui a lancé le programme.
- **Après** : **2**. right_child a lu `q\n` : il fait `break` puis `exit`. main est bloqué dans `waitpid`, left_child est bloqué dans `read(0, ...)` en attendant la saisie suivante. Il reste aussi un zombie, right_child, tant que main n'a pas fait le `waitpid` correspondant.

### 4.5 Deux lignes `q` : pourquoi ça s'arrête au deuxième
- Après le premier `q`, right_child est mort. Il était le **dernier lecteur** du pipe : main a fermé `pipefd[0]`, et left_child aussi au début. Le pipe n'a donc plus de lecteur, mais left_child ne le sait pas : il dort dans `read(0, ...)`, sur le terminal.
- Au deuxième `q`, left_child lit `'q'` et appelle `write(pipefd[1], ...)`. Écrire dans un pipe **sans lecteur** fait envoyer **SIGPIPE** par le noyau au processus (sinon, `write` échouerait avec `EPIPE`). L'action par défaut de SIGPIPE est de terminer le processus : left_child meurt.
- Les deux fils sont terminés, les deux `waitpid` de main reviennent et main s'arrête.

Ce n'est pas le contenu `q` qui compte : **n'importe quelle** ligne saisie après le premier `q` provoque l'arrêt (un Ctrl-D aussi, car `read` renvoie 0). C'est exactement le comportement de la commande shell : `cat` ne meurt de SIGPIPE qu'à sa première écriture après la fin de la boucle `while`. Mesuré avec une entrée `Bonjour`, `q`, `q`, `encore`, `fin` espacée d'une seconde : le programme comme la commande shell se terminent à **t = 2 s**, c'est-à-dire au deuxième `q`.

### 4.6 Oubli du `close(pipefd[1])` dans `right_child`
Dans le scénario `q`, cela ne change rien : right_child sort par `break`. Mais right_child garde alors un bout d'**écriture** du pipe qu'il lit lui-même. Si l'utilisateur termine par **Ctrl-D** sans taper `q`, left_child voit EOF, ferme son bout et s'arrête. right_child, lui, ne verra **jamais la fin de fichier** : `read` ne renvoie 0 que si **tous** les bouts d'écriture sont fermés, et il en détient un. Il reste bloqué indéfiniment dans `read`, et main reste bloqué dans `waitpid` : **interblocage**.

### 4.7 Oubli des `close` dans `main`
- main garde `pipefd[0]`, donc le pipe a **toujours un lecteur**. Au deuxième `q`, le `write` de left_child réussit (les octets s'accumulent dans le pipe, que personne ne lit) : **pas de SIGPIPE**, et le programme ne s'arrête plus sur `q`. Il faut un Ctrl-D pour que left_child se termine. Si l'utilisateur tape plus que la capacité du pipe (64 Kio), left_child se bloque dans `write` et tout reste bloqué.
- main garde aussi `pipefd[1]`, donc sans `q`, right_child ne voit jamais EOF (même blocage qu'en 4.6).

Mesuré : avec les `close` retirés de main, la même entrée ne se termine qu'à **t = 4 s**, à la fin de l'entrée (EOF), au lieu de 2 s.
