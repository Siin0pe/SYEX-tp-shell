# Partiel 16 janvier 2023 (corrigé)

Sujet : `../sujets/OSSE_sujet_2022-2023.pdf`. Durée 1h30, cinq exercices indépendants.
`../sujets/OSSE_notes_scarylight.pdf` est la page 4 du même sujet (figure 2, `foo.c`), annotée à la main par un étudiant.

| Exercice | Fichier(s) | Rôle |
|---|---|---|
| 2 | `myscp.sh` | wrapper de la figure 1 corrigé |
| 3 | `existants.sh` | affiche les arguments qui sont des fichiers existants |
| 4 | `abc.c` `abc-pipe.c` | programme abc, puis récupération du PID du petit-fils par pipe |
| 5 | `foo.c` `foo2.c` | figure 2 recopiée telle quelle, puis version corrigée |

```sh
for p in abc abc-pipe foo2 ; do gcc -Wall -Wextra -o $p $p.c ; done
gcc -o foo foo.c
```

---

## Exercice 1 : commandes shell

```sh
rmdir /tmp/log                                    # 1.1 rmdir refuse un répertoire non vide
head -n 5 projet.c                                # 1.2
grep -w Array main.ml                             # 1.3 (-w : le mot entier ; sans -w, "ArrayList" passe aussi)
sed -i -e 's/IBD/CBDR11/g' maquette.html          # 1.4 -i : modifie le fichier ; g : toutes les occurrences
find ~ -mindepth 1 -maxdepth 1 -type d | wc -l    # 1.5 sous-répertoires directs, cachés compris
stat -c '%U' /tmp/* | sort -u                     # 1.6 -u : chaque utilisateur une seule fois
```
- 1.5 : pour compter **tous** les sous-répertoires (récursivement), on enlève `-maxdepth 1`. Avec `ls -l ~ | grep '^d' | wc -l`, on oublie les répertoires cachés.
- 1.6 : `/tmp/*` ne voit pas les fichiers cachés. Version complète : `find /tmp -mindepth 1 -maxdepth 1 -exec stat -c '%U' {} + | sort -u`.

---

## Exercice 2 : wrapper `myscp.sh`

### 2.1 Fonctionnement de `check_colon`
```sh
check_colon() {
    src=$1
    dst=$(echo $src | sed -e '1s/://')
    if [ $src = $dst ] ; then
        return 0
    fi
    return 1
}
```
1. range l'argument dans `src` ;
2. `sed -e '1s/://'` : sur la ligne 1, remplace la **première** occurrence de `:` par rien. `dst` contient donc l'argument privé de son premier `:` ;
3. si `src` et `dst` sont égaux, il n'y avait **pas** de `:` et la fonction renvoie **0** (vrai). Sinon elle renvoie **1**.

Donc `check_colon` est **vraie quand l'argument ne contient PAS de `:`**, c'est-à-dire quand le fichier est local. C'est l'inverse de ce que suggèrent son nom et son usage lignes 23-29.
Effet de bord grave : `src` et `dst` sont des variables **globales**. Appeler `check_colon $src` **écrase** le `dst` du script avec `src` sans son `:`. Le `scp $src $dst` final copie alors vers le mauvais fichier.

### 2.2 Problèmes et correctifs

| Ligne | Problème | Correctif |
|---|---|---|
| 1 | `!#/bin/sh` : caractères inversés | `#!/bin/sh` |
| 5-6 | `src`, `dst` globales : la fonction écrase les variables du script | `local`, ou d'autres noms (`orig`, `stripped`) |
| 6 | `echo $src` sans guillemets (espaces, `*`, argument commençant par `-`) | `printf '%s\n' "$1"` |
| 7 | `[ $src = $dst ]` sans guillemets : erreur de syntaxe si vide ou si contient des espaces | `[ "$orig" = "$stripped" ]` |
| 7-10 | statut inversé : vrai si **pas** de `:` | `return 1` si égaux, `return 0` sinon (ou plus simple : `case "$1" in *:*) return 0 ;; esac ; return 1`) |
| 14 | `$@` donne la liste des arguments, pas leur nombre ; `!=` compare des chaînes | `[ $# -ne 2 ]` |
| 15 | `print` n'existe pas en sh ; `>2` redirige vers un **fichier** nommé `2` | `echo "usage: ..." >&2` |
| 16, 26, 31 | `exit(1)` : les parenthèses sont une erreur de syntaxe en sh | `exit 1`, `exit 2`, `exit 3` |
| 19 | `//` n'est pas un commentaire en sh (c'est une commande) | `# giving pretty names ...` |
| 20-21 | `src = $1` : avec des espaces, c'est la **commande** `src` avec les arguments `=` et `$1` | `src="$1"`, `dst="$2"` |
| 23, 24, 29 | `[ check_colon $src ]` : `[` ne lance pas la fonction | `if check_colon "$src" ; then` |
| 23 | il manque `;` avant `then` | `] ; then` → `; then` |
| 25, 30 | `echo >2` : fichier `2` au lieu de stderr | `echo >&2 ...` |
| 25, 30 | quotes simples : `$src` et `$dst` ne sont pas remplacés dans le message | guillemets doubles |
| 35 | `scp $src $dst` sans guillemets | `exec scp "$src" "$dst"` |

Version corrigée : `myscp.sh`. Tests avec un faux `scp` :
```
x.txt h:y.txt  ->  scp x.txt h:y.txt
h:x h:y        ->  error: h:x and h:y are both distant   (statut 2)
x y            ->  error: x and y are both local         (statut 3)
x              ->  usage: ...                            (statut 1)
```

---

## Exercice 3 : arguments qui sont des fichiers existants

```sh
#!/bin/sh
for f in "$@" ; do
    if [ -e "$f" ] ; then
        printf '%s\n' "$f"
    fi
done
```
`"$@"` conserve chaque argument intact, même avec des espaces. `-e` teste l'existence (fichier, répertoire, etc.) ; `-f` restreint aux fichiers réguliers. `printf '%s\n'` affiche correctement un nom comme `-n`.

---

## Exercice 4 : programme abc

### 4.1 Code (`abc.c`)
```c
pid_t f = fork();
if (f == 0) {                                    // fils
    pid_t pf = fork();
    if (pf == 0) { write(1, "ccc ", 4); exit(0); }   // petit-fils
    write(1, "bbb ", 4);
    waitpid(pf, NULL, 0);
    exit(0);
}
write(1, "aaa ", 4);                             // père
waitpid(f, NULL, 0);
write(1, "\n", 1);
```

### 4.2 Sorties possibles
Les trois écritures `aaa`, `bbb`, `ccc` sont faites par trois processus **concurrents**, sans aucune contrainte d'ordre entre elles. Le père écrit `aaa` après le fork, le fils écrit `bbb` après son propre fork, et le petit-fils écrit `ccc`. Seul `\n` est forcément le dernier : le père attend le fils, qui attend le petit-fils.
Donc les **6 permutations** sont possibles, suivies de `\n` :
```
aaa bbb ccc    aaa ccc bbb    bbb aaa ccc
bbb ccc aaa    ccc aaa bbb    ccc bbb aaa
```
En pratique, le père continue souvent de s'exécuter juste après le `fork`. Sur 2000 exécutions, on observe `aaa bbb ccc` 1995 fois et `aaa ccc bbb` 5 fois, mais aucun ordre n'est garanti. Chaque `write` est atomique, donc les mots ne se mélangent pas lettre par lettre.

### 4.3 Récupérer le PID du petit-fils par un pipe (`abc-pipe.c`)
- Le **père** crée le pipe (`int p[2]; pipe(p);`) **avant** le premier `fork`, pour que le fils en hérite.
- Le **fils** reçoit le PID du petit-fils comme valeur de retour de son `fork()`. Il ferme `p[0]`, fait `write(p[1], &pf, sizeof(pf))`, puis `close(p[1])`. (Autre solution : le petit-fils écrit `getpid()`.)
- Le **père** ferme `p[1]` (il n'écrit pas), fait `read(p[0], &pf, sizeof(pf))` (bloquant tant que le fils n'a rien écrit), puis `close(p[0])`.
- Le petit-fils ferme `p[1]` s'il n'en a pas l'usage.

---

## Exercice 5 : programme foo

### 5.1 Ce que fait `foo`
1. **Arguments** (l. 25-33) : au plus un argument. Si un fichier est donné, il est ouvert et `dup2(fd_in, 0)` le met à la place de l'**entrée standard**. Sinon, foo lit l'entrée standard héritée.
2. **Pipe** (l. 34) : `pipefd`, global, servira aux fils pour envoyer un `int` au père.
3. **Boucle de lecture** (l. 36-46) : `scanf("%s", buf)` lit des **mots** (séparés par des blancs) sur l'entrée standard. Chaque mot est un nom de fichier. Pour chacun, le père **crée un fils** et incrémente `nb`, le nombre de fils.
4. **Chaque fils** appelle `do_the_thing(buf)` (`buf` est une copie privée, faite au `fork`). Elle ouvre le fichier, compte ses **octets** en lisant un caractère à la fois (0 si l'ouverture échoue), et écrit ce nombre (`int`) dans le pipe. Puis le fils fait `exit(0)`.
5. **Collecte** (l. 48-57) : `nb` fois, le père attend la fin d'un fils (`wait(NULL)`), lit un `int` dans le pipe et l'ajoute à `res`. L'ordre n'importe pas, puisque l'addition est commutative.
6. Il affiche `res = <total>` : la **somme des tailles** des fichiers dont les noms ont été lus. Les tailles sont calculées **en parallèle**, un processus par fichier.

Exemple : `ls | ./foo` affiche la taille totale des fichiers du répertoire courant.

> **Bug réel (vérifié)** : quand l'entrée standard est un **fichier ordinaire** (`./foo liste` ou `./foo < liste`), le résultat est faux et varie d'une exécution à l'autre (234, 174, 1918... au lieu de 14). Le fils a hérité d'une copie du `FILE *stdin` du père, et l'offset du fichier est **partagé** depuis le `fork`. Le `exit` du fils ferme sa copie de stdin, et la libc remet alors cet offset partagé à la position logique du fils. Le père relit ensuite des noms déjà traités. Avec un pipe (`cat liste | ./foo`), on ne peut pas revenir en arrière, et le résultat est juste. Correctif : `_exit(0)` dans le fils (voir `foo2.c`).

### 5.2 `do_the_thing` est inefficace
Elle fait **un appel système `read` par octet**, donc un million d'appels pour un fichier d'1 Mo. Or on veut seulement la taille :
```c
int do_the_thing(const char* f) {
    int res = 0;
    int fd = open(f, O_RDONLY);
    if (fd != -1) {
        off_t size = lseek(fd, 0, SEEK_END);   // renvoie la position = la taille
        if (size != -1) res = (int)size;
        close(fd);
    }
    return write(pipefd[1], &res, sizeof(int));
}
```
`fstat(fd, &st)` puis `st.st_size` (ou `stat(f, &st)` sans même ouvrir le fichier) marche aussi. À défaut, lire par **blocs** (`char b[4096]` et additionner les retours de `read`) divise déjà le nombre d'appels par 4096.

### 5.3 Où fermer `pipefd[0]` et `pipefd[1]`
- **`pipefd[0]`** (lecture) :
  - dans chaque **fils**, au tout début de la branche `pid == 0` (ligne 42, avant `do_the_thing`), car les fils ne lisent jamais ;
  - dans le **père**, après la boucle de collecte (après la ligne 57), une fois tous les `int` lus.
- **`pipefd[1]`** (écriture) :
  - dans le **père**, juste **après la boucle de création** (après la ligne 46). Il n'écrit jamais, mais il ne peut pas le fermer plus tôt, sinon les fils suivants n'en hériteraient pas ;
  - dans chaque **fils**, juste après le `write` de `do_the_thing` (avant `exit`).

Intérêt : une fois que le père a fermé `pipefd[1]`, un `read` sur un pipe vide dont tous les fils sont morts renvoie 0 (EOF) au lieu de bloquer indéfiniment. Ici le nombre de lectures est connu (`nb`), donc ce n'est pas bloquant, mais c'est la règle de bonne hygiène.

### 5.4 `ls -R ~ | foo`
- **Résultat faux** : `ls -R` affiche des en-têtes `répertoire:`, des lignes vides et des noms **relatifs** à chaque sous-répertoire, pas des chemins. `open` échoue donc pour presque tout (le fils envoie 0), ou ouvre par hasard un fichier homonyme du répertoire courant. Les noms contenant des espaces sont aussi coupés en plusieurs mots par `scanf`.
- **Explosion du nombre de processus** : le père lance un fils par mot **avant** d'en attendre un seul. Un `$HOME` contient facilement des dizaines de milliers de fichiers (47 696 lignes pour `ls -R /usr/share`), donc autant de processus simultanés en théorie. On peut atteindre la limite de processus de l'utilisateur (`ulimit -u`) : `fork` échoue, foo s'arrête avec `exit(3)` en laissant ses fils orphelins, et la machine est saturée (effet *fork bomb*).
- **Débordement** : `scanf("%s", buf)` ne limite pas la longueur, et un mot de plus de 1023 caractères déborde de `buf`. Il faut écrire `%1023s`.
- Le pipe ne peut pas se bloquer définitivement : chaque fils terminé a déjà écrit son `int`, et le père lit un `int` par `wait`. Mais tant que la boucle de création n'est pas finie, rien n'est lu, et au-delà de 64 Kio (16 384 `int`) les fils restent bloqués dans `write`, ce qui accumule encore plus de processus vivants.
