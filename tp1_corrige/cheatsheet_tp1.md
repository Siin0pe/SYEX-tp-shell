# TP1 : Les bases du Shell (cheatsheet et corrigé)

Le TP1 ne demande aucun script : ce sont des commandes à taper en interactif.
Ce fichier donne, exercice par exercice, **les commandes, comment s'en servir, et les réponses aux questions**, puis un **mémo des commandes** à la fin.
Les numéros de diapos renvoient au cours `sys-trans1.pdf`.

Conventions :
- `sh$` est le prompt : il ne faut pas le taper.
- `<CTRL-X>` signifie appuyer sur Ctrl et X en même temps.
- `# ...` est un commentaire : le shell ignore tout ce qui suit `#`.

---

## 0. Rappels indispensables

| Action | Comment |
|---|---|
| Compléter un nom | `Tab` (1 fois). En cas de conflit, `Tab Tab` affiche les possibilités |
| Rappeler une commande | `↑` / `↓` (historique) |
| Modifier la commande | `←` / `→` |
| Relancer la dernière commande commençant par `str` | `!str` (diapo 44) |
| Arrêter le programme en avant-plan | `<CTRL-C>` |
| Mettre en pause le programme en avant-plan | `<CTRL-Z>` |
| Fin de saisie (fin du flux stdin) | `<CTRL-D>` (sur le prompt, ferme le shell) |
| Lire le manuel | `man cmd`, puis `/mot` pour chercher, `n`/`N` pour l'occurrence suivante/précédente, `q` pour quitter |

**Structure d'une commande (diapo 30).**
```
sh$ cmd arg1 arg2 ... &
```
- `cmd` peut être un chemin (`./a.out`, `/bin/ls`) ou un nom sans `/`. Dans ce dernier cas, il est cherché dans le `PATH`.
- Les arguments sont séparés par des blancs.
- Un `&` final lance la commande en arrière-plan.

**Chemins (diapos 16 à 18).**
- `/a/b` est **absolu** : on part de la racine.
- `a/b` est **relatif** : on part du répertoire courant (CWD).
- `.` = le répertoire courant, `..` = le répertoire parent, `~` = le HOME, `~user` = le HOME de user.

---

## Exercice 1 : `cd`, `pwd`, `HOME`, `xterm`, `exit`

```sh
mkdir -pv ~/sys/tp1      # -p : crée aussi les parents (~/sys) et pas d'erreur si ça existe
                         # -v : verbeux, affiche ce qui est créé
cd ~/sys/tp1             # change le répertoire courant (CWD)
pwd                      # affiche le CWD (Print Working Directory)
cd                       # sans argument : va dans le HOME
pwd
cd -                     # revient dans le répertoire précédent (ici ~/sys/tp1)
```

**Q6 : quatre manières de faire `ls` dans le HOME sans changer de CWD.**
```sh
( cd ; ls )              # cd sans argument = HOME
( cd ~ ; ls )            # ~ = HOME
( cd $HOME ; ls )        # variable d'environnement HOME
( cd ../.. ; ls )        # chemin relatif : marche seulement si on est dans ~/sys/tp1
```
Les **parenthèses** font tourner les commandes dans **un autre processus shell** (diapo 38). Le `cd` ne change que le CWD de ce sous-shell : le CWD du terminal reste le même.
Avec `{ cd ; ls ; }`, le terminal changerait de répertoire.
Encore plus simple, sans aucun `cd` : `ls ~`.

**Q7 à Q12 : terminaux.**
```sh
xterm &                  # nouveau terminal en arrière-plan : le premier reste utilisable
pwd                      # dans chaque terminal : même CWD (le fils hérite du CWD du père)
exit                     # ferme le shell, donc le terminal
xterm                    # sans & : en avant-plan
```

**Q11 : que se passe-t-il sans `&` ?** xterm est en **avant-plan** : le premier terminal attend qu'il se termine. On ne peut plus y taper de commande tant que le 2e xterm est ouvert.

**Q12 :** `<CTRL-D>` dans le 2e terminal. Cela envoie la fin du flux stdin au shell, qui se termine comme avec `exit`.

---

## Exercice 2 : `cp`, `mv`, `mkdir`, `ls`

```sh
cd
cp -v /pub/FISE_OSSE11/shell/fich.dat /tmp/f1     # copie src vers dest (nouveau nom f1)
ls -l /tmp/f1                                     # -l : format long ; la 5e colonne = taille en octets
mkdir -pv /tmp/d1/d2                              # crée d1 et d2 d'un coup
mv -v /tmp/f1 /tmp/d1/d2/f1-de-d2                 # déplace ET renomme
mkdir -pv sys/tp1                                 # chemin relatif : depuis le HOME
cp -rv /tmp/d1/d2 sys/tp1/exo2                    # -r : copie récursive d'une arborescence
mv -v /tmp/d1 sys/tp1/exo2                        # exo2 existe : d1 va DEDANS
ls -l sys/tp1/exo2                                # doit montrer f1-de-d2 et d1
ls -l sys/tp1/exo2{,/d1/d2}/f1-de-d2              # Q8 : les 2 fichiers en une commande
```

**Comment marche `{,/d1/d2}` ?** C'est l'expansion `{expr0,expr1}` (diapo 47). Le shell la remplace par deux chaînes, une avec chaque choix (ici le choix vide, puis `/d1/d2`) :
`sys/tp1/exo2/f1-de-d2` et `sys/tp1/exo2/d1/d2/f1-de-d2`.

**Règle de `cp` et `mv`.**
- Si la destination est un **répertoire existant**, la source est mise **dedans**.
- Sinon, la destination est le **nouveau nom**.
- C'est pour cela que le `mv` de la Q6 met `d1` dans `exo2`.

---

## Exercice 3 : `cat`, `echo`, redirection

```sh
cd /tmp
echo -n 'A' > a                            # -n : pas de saut de ligne ; > : stdout vers le fichier a (écrasé)
cat a ; ls -l a                            # contenu : A, taille : 1 octet
cat a a a a a a a a a a > /tmp/1 ; mv /tmp/1 a    # 10 fois le contenu : 10 octets
# ↑ puis Entrée pour la relancer : 100 octets
cat a ; ls -l a
cat a a a a a a a a a a > /tmp/1           # Q6 : 10 × 100 = 1000 'A' dans /tmp/1
```

**Pourquoi un fichier intermédiaire ?** Avec `cat a a > a`, le shell ouvre d'abord `a` en écriture et le **vide** (`>` écrase), puis lance `cat`. `cat` lit alors un fichier vide et on perd tout.

---

## Exercice 4 : la variable `HOME`

```sh
cd /tmp
mkdir -pv ~/sys/exo4
HOME=~/sys/exo4 xterm &      # VAR=val cmd : la variable n'est changée QUE pour cmd (diapo 42)
```
Dans le second terminal :
```sh
cd ; pwd ; echo ~ $HOME      # tout vaut ~/sys/exo4 : cd, ~ et $HOME utilisent HOME
cd /tmp
touch ~/../../f1             # ~ = .../sys/exo4, donc ~/../.. = le VRAI HOME
                             # touch : crée un fichier vide (ou met à jour sa date)
```
Dans le premier terminal :
```sh
cd /tmp
ls -l ~/f1                   # existe, taille 0
rm -v ~/f1                   # supprime
```
Pour fermer le second terminal : `exit` ou `<CTRL-D>`.

**À retenir :** `HOME` n'est qu'une **variable d'environnement**. La changer change ce que `cd` et `~` désignent, mais pas le vrai répertoire personnel.

---

## Exercice 5 : la variable `PATH`

```sh
mkdir -pv ~/bin
cp -v /bin/ls ~/bin/myls
cp -v /bin/cat ~/bin/mycat
myls                         # "command not found" : ~/bin n'est pas dans le PATH
~/bin/myls                   # marche : un chemin avec un / n'utilise pas le PATH
export PATH="$PATH:$HOME/bin"    # ajoute ~/bin À LA FIN du PATH, et marque la variable
myls ; mycat /etc/hostname   # marche maintenant
```

**Fonctionnement (diapo 24).** Une commande **sans `/`** est cherchée dans les répertoires du `PATH`, séparés par `:` et parcourus dans l'ordre. Le premier exécutable trouvé est lancé.

**`$HOME` plutôt que `~` dans `"..."`.** Le sujet écrit `"$PATH:~/bin"`. Or entre guillemets doubles, le `~` n'est pas expansé : le `PATH` contient alors littéralement `~/bin`. Bash s'en accommode, mais les autres programmes ne comprennent pas ce chemin. `$HOME` est, lui, toujours expansé entre `"..."`.

**Q5 : xterm lancé depuis ce terminal.** Oui, `myls` marche : `PATH` est exporté, donc transmis au processus fils xterm, puis au shell qu'il lance.

**Q6 : xterm lancé avec `<ALT-F2>` depuis le bureau.** Non, `myls` ne marche pas. Ce xterm n'est pas un fils de notre shell : il hérite de l'environnement du bureau, qui n'a pas été modifié. Les changements faits par `export` ne valent que pour le shell courant et ses descendants.
Pour rendre le changement permanent, il faudrait l'écrire dans `~/.bashrc` ou `~/.profile`, des scripts **sourcés** au démarrage (diapo 55).

---

## Exercice 6 : `less`, `cat`, calcul, sous-shell

```sh
cd
cat /pub/FISE_OSSE11/shell/expr-arith.dat      # ((1+1)*3+4)*10
less /pub/FISE_OSSE11/shell/expr-arith.dat     # pageur : espace = page suivante, q = quitter
echo $(( $(cat /pub/FISE_OSSE11/shell/expr-arith.dat) ))   # 100
```

**Lecture de la dernière commande, de l'intérieur vers l'extérieur (diapo 44).**
1. `$(cat fichier)` est remplacé par le contenu du fichier (stdout de `cat`).
2. `$(( ... ))` calcule l'expression arithmétique.
3. `echo` affiche le résultat.

```sh
# Q4 : expression × elle-même. Les guillemets protègent le * et les parenthèses
echo "( $(cat /pub/FISE_OSSE11/shell/expr-arith.dat) ) * ( $(cat /pub/FISE_OSSE11/shell/expr-arith.dat) )" > /tmp/expr1
echo $(( $(cat /tmp/expr1) ))                  # Q5 : 10000

# Q6 : avec une variable. \* : le * échappé n'est pas remplacé par des noms de fichiers
x="($(cat /pub/FISE_OSSE11/shell/expr-arith.dat))"
echo $x \* $x \* $x > /tmp/expr2
echo $(( $(cat /tmp/expr2) ))                  # Q7 : 1000000

# Q8 : cat avec "-" = lire stdin (le clavier) à cet endroit
cat - /tmp/expr1 - /tmp/expr2 - > /tmp/expr3
(<ENTER>
<CTRL-D>)+(<ENTER>
<CTRL-D>)<ENTER>
<CTRL-D>
echo $(( $(cat /tmp/expr3) ))                  # Q9 : 1010000
rm -v /tmp/expr*                               # Q10 : expr* = tous les fichiers commençant par expr
```

**Comment marche `cat -` ?** `cat` concatène ses arguments dans l'ordre, et `-` désigne stdin. Chaque `<CTRL-D>` termine **une** lecture du clavier. On tape donc `(`, puis le contenu de expr1 est ajouté, puis on tape `)+(`, etc.

---

## Exercice 7 : `fg`, `bg`, avant-plan et arrière-plan

| Commande / touche | Effet |
|---|---|
| `cmd &` | lance cmd en arrière-plan : le shell rend la main tout de suite |
| `<CTRL-C>` | demande au processus **en avant-plan** de se terminer |
| `<CTRL-Z>` | met **en pause** le processus en avant-plan, et le shell reprend la main |
| `fg` | ramène en avant-plan le dernier processus en arrière-plan (et le relance s'il était en pause) |
| `bg` | relance **en arrière-plan** le processus mis en pause |

```sh
atril /pub/FISE_OSSE11/Supports/sys-poly.pdf     # puis <CTRL-C> : atril se ferme
                                                 # <CTRL-C> à nouveau : rien, bash ignore cette demande
atril /pub/FISE_OSSE11/Supports/sys-poly.pdf &   # <CTRL-C> : atril ne se ferme pas (il n'est pas en avant-plan)
fg                                               # atril revient en avant-plan
```

**Q5 : on tape `ls` pendant qu'atril est en avant-plan, puis on quitte atril.** Rien ne s'exécute pendant qu'atril tourne : les caractères tapés sont **mis en attente** par le terminal. Quand atril se termine, bash reprend la main, lit `ls` + Entrée et l'exécute.

**Q7 : pourquoi `xterm` en avant-plan n'est pas une bonne idée.** Le premier terminal est bloqué tant que le nouveau est ouvert. La solution :
```sh
xterm          # bloque le terminal
<CTRL-Z>       # met xterm en pause (la fenêtre ne répond plus)
bg             # le relance en arrière-plan : les deux terminaux sont utilisables
```

---

## Exercice 8 : `cut`, `head`, `tail`, `grep`, `sed`

Fichier d'exemple `select.dat` (colonnes séparées par **une tabulation** : nom, date, destination) :
```
Joe	10/12/1990	Strasbourg
Alice	05/03/1998	Nantes
Bob	01/08/2001	Amsterdam
...
```
Toutes ces commandes lisent un fichier, ou **stdin** si on n'en donne pas, et écrivent sur **stdout**. On peut donc les enchaîner avec `|`.

### `cut` : sélectionner des colonnes
```sh
cut -f 3 select.dat              # 3e colonne (séparateur par défaut : tabulation)
cut -f 1,3 select.dat            # colonnes 1 et 3
cut -f 3,1 select.dat            # Q2c : MÊME résultat que 1,3 : cut garde l'ordre du fichier
cut -d : -f 4 /etc/passwd        # Q2d : -d : choisit le séparateur ':' ; 4e champ = GID
```

### `head`, `tail`, `grep` : sélectionner des lignes
```sh
head -n 2 select.dat             # 2 premières lignes
tail -n 3 select.dat             # 3 dernières lignes
grep -e Alice select.dat         # lignes contenant Alice
```
Options utiles de `grep` :
- `-v` : lignes qui ne contiennent **pas** le motif.
- `-w` : mot entier.
- `-c` : compte les lignes trouvées.
- `-l` : affiche seulement les noms de fichiers.
- `-n` : numéros de ligne.
- `-q` : silencieux, seul le statut compte.
- `-i` : insensible à la casse.

### `sed` : éditeur de flux

Forme générale : `sed -e 'ADRESSE ACTION' fichier`.

| Commande | Signification |
|---|---|
| `sed -e '4,$d'` | supprime (`d`) les lignes 4 à la dernière (`$`) : reste les 3 premières |
| `sed -e '1,3d'` | supprime les lignes 1 à 3 |
| `sed -e '/Alice/d'` | supprime les lignes contenant Alice |
| `sed -e '/Alice/!d'` | supprime les lignes qui ne contiennent **pas** Alice (`!` inverse) |
| `sed -n -e '/Alice/p'` | `-n` : n'affiche rien par défaut ; `p` affiche les lignes sélectionnées |
| `sed -e 's/Alice/Anne/g'` | substitue (`s`) ; `g` = toutes les occurrences de la ligne |
| `sed -e 's,01/08/2001,03/08/2001,'` | autre séparateur (`,`) car les motifs contiennent `/` |
| `sed -e '/Alice/q'` | `q` = quitter après cette ligne |
| `sed -i -e 's/a/b/g' f` | `-i` : modifie le fichier **sur place** |

Motifs (expressions régulières de sed) :

| Motif | Sens |
|---|---|
| `.` | n'importe quel caractère |
| `*` | le motif précédent, 0 fois ou plus (`.*` = n'importe quelle suite) |
| `[^\t]` | tout caractère sauf la tabulation |
| `\t` | tabulation |
| `^` / `$` | début / fin de ligne |
| `\( ... \)` | mémorise un morceau, réutilisé ensuite avec `\1`, `\2`... |

```sh
nb=$(wc -l < select.dat)                 # nombre de lignes
sed -e "$((nb-1)),\$d" select.dat        # Q4c : sans les 2 dernières lignes
sed -e 's/\t.*//' select.dat             # 1re colonne : supprime de la 1re tabulation à la fin
sed -e 's/[^\t]*\t[^\t]*\t\([^\t]*\)/\1/' select.dat            # 3e colonne
sed -e 's/\([^\t]*\)\t[^\t]*\t\([^\t]*\)/\1\t\2/' select.dat    # colonnes 1 et 3
sed -e 's/\([^\t]*\)\t[^\t]*\t\([^\t]*\)/\2\t\1/' select.dat    # échange 1 et 3
```

**Q4d : que se passe-t-il si on oublie le `\` devant `$` ?** Entre `"..."`, le shell expanse les variables. `$d` est alors pris pour la variable `d`, non définie, donc vide. sed reçoit `5,` et répond : `sed: -e expression n°1, caractère 2: « , » inattendue`. Le `\$` empêche l'expansion : sed reçoit bien le `$` qui veut dire « dernière ligne ».

**Q5 : à vous de jouer.**
```sh
grep -e Alice select.dat | head -n 1                    # a. 1re ligne avec Alice
sed -n -e '/Alice/{p;q}' select.dat                     # b. affiche la 1re ligne avec Alice puis quitte
sed -e '/Alice/!d' select.dat | head -n 1               # b. variante avec head
grep -e Alice select.dat | head -n 1 | cut -f 3         # c. Nantes
sed -n -e '/Alice/s/.*\t//p' select.dat | head -n 1     # d. Nantes (supprime tout jusqu'à la dernière tabulation)
grep -e Alice select.dat | tail -n 1 | cut -f 3         # e. Nice (dernière ligne avec Alice)
```

---

## Exercice 9 : `wc`, `stat`, taille d'un fichier

```sh
wc -c /pub/FISE_OSSE11/shell/gen-projet.sh       # "3464 /pub/.../gen-projet.sh" : taille + nom
wc -c < /pub/FISE_OSSE11/shell/gen-projet.sh     # "3464" : wc lit stdin, il ne connaît pas le nom
sz=$(wc -c < /pub/FISE_OSSE11/shell/gen-projet.sh) ; echo "sz = $sz"
sz=$(stat -c '%s' /pub/FISE_OSSE11/shell/gen-projet.sh) ; echo "sz = $sz"
```

**Pourquoi préférer `stat` ?** `wc` lit **tout** le fichier pour compter, ce qui est très lent sur un gros fichier. `stat` lit seulement les **méta-données** (diapo 33), dont la taille.

**Q2b : taille réellement occupée sur le disque.** C'est le nombre de blocs (`%b`) multiplié par la taille d'un bloc (`%B`) :
```sh
echo $(( $(stat -c '%b * %B' /pub/FISE_OSSE11/shell/gen-projet.sh) ))   # 8 * 512 = 4096
```
`stat` écrit `8 * 512`, et `$(( ))` le calcule.

Formats utiles de `stat -c` :
- `%s` taille, `%U` propriétaire, `%G` groupe
- `%a` droits en octal, `%A` droits en `rwx`
- `%F` type de fichier, `%n` nom

---

## Exercice 10 : `sort`, `uniq`, `wc`

```sh
cat /pub/FISE_OSSE11/shell/unsort.dat
sort /pub/FISE_OSSE11/shell/unsort.dat > /tmp/1          # Q2 : trie les lignes
wc -l /pub/FISE_OSSE11/shell/unsort.dat                  # "12 /pub/.../unsort.dat"
wc -l < /tmp/1                                           # "12"
```

**Q3 : différence entre les deux syntaxes.**
- Avec un **argument**, `wc` ouvre lui-même le fichier et affiche son nom.
- Avec `<`, c'est le **shell** qui ouvre le fichier et le branche sur stdin. `wc` ne connaît pas le nom et n'affiche que le nombre, ce qui est pratique pour `x=$(wc -l < f)`.

```sh
uniq /tmp/1 > /tmp/2                                     # Q4 : supprime les lignes identiques CONSÉCUTIVES (d'où le tri avant)
sort -u /pub/FISE_OSSE11/shell/unsort.dat                # Q5 : -u = trier ET supprimer les doublons
n1=$(wc -l < /tmp/1) ; n2=$(wc -l < /tmp/2) ; echo $((n1 - n2))   # Q6 : lignes enlevées
echo $(( $(sort /pub/FISE_OSSE11/shell/unsort.dat | wc -l) - $(sort -u /pub/FISE_OSSE11/shell/unsort.dat | wc -l) ))   # Q7
```

Options utiles de `sort` :
- `-n` : tri numérique.
- `-r` : ordre inverse.
- `-u` : sans doublons.
- `-k N` : trier à partir du champ N.
- `-t SEP` : choisit le séparateur de champs.

---

## Exercice 11 : `find`

Syntaxe : `find REPERTOIRE FILTRES... ACTIONS...`. Sans action, c'est `-print` (afficher le chemin).

```sh
find $HOME -print                      # tout le HOME, chemins absolus
( cd ; find . -print )                 # chemins relatifs (souvent plus utile)
find . -type f -print                  # fichiers réguliers (f) ; d = répertoires ; l = liens
find . -type d -print
find . -type f -size +10k -print       # taille > 10 ko (+ = plus de, - = moins de)
find . -type f -size -100c -print      # taille < 100 octets (c = octets)
find . -type f -name '*.[hc]' -print   # nom de base finissant par .c ou .h (-iname : sans casse)
```

**Q7 : pourquoi les `'` autour de `*.[hc]` ?** Sans quotes, le **shell** expanse `*.[hc]` avant de lancer find (diapo 46), avec les fichiers du répertoire courant.
- S'il y a `a.c` et `b.h` dans le répertoire courant, find reçoit `-name a.c b.h` : erreur ou mauvais résultat.
- Avec les quotes, find reçoit le motif intact et l'applique dans toute l'arborescence.

```sh
find /pub/FISE_OSSE11/shell/projet -type f -name '*.c' -exec grep -w -l -e strcmp {} \;
find /pub/FISE_OSSE11/shell/projet -type f -name '*.c' -exec grep -w -n -H -e strcmp {} \;   # Q9
```

**Comment marche `-exec`.**
- `-exec CMD ... {} \;` lance CMD **une fois par fichier** trouvé, en remplaçant `{}` par son chemin.
- Le `\;` marque la fin de la commande. Il est échappé pour que le shell ne le prenne pas pour un séparateur de commandes.
- Q9 : `grep -n` ajoute les numéros de ligne, et `-H` force l'affichage du nom de fichier.

Autres filtres utiles :
- `-user nom` : appartient à nom.
- `-empty` ou `-size 0` : fichier vide.
- `-maxdepth N` / `-mindepth N` : profondeur de recherche.
- `!` : négation.
- `-o` : ou.

**Les filtres s'enchaînent comme un ET.** Une action n'est faite que si tout ce qui précède est vrai. Un `-exec grep -q ...` sert donc de **filtre** : il est vrai si grep trouve le motif.

---

## Exercice 12 : applications de `find`

```sh
P=/pub/FISE_OSSE11/shell/projet
find $P -type f -name '*.[ch]' | wc -l                                  # Q1 : 11 fichiers
find $P -type f -name '*.[ch]' -exec cat {} \; | wc -l                  # Q2 : 9099 lignes
find $P -type f -name '*.[ch]' -exec cat {} \; | grep -w -c atoi        # Q3 : 2 lignes avec atoi
```

```sh
rm -rfv /tmp/projet
cp -rv $P /tmp/projet                                                   # Q4

find /tmp/projet -type f -name '*.[ch]' -exec cp {} {}.orig \;          # a. sauvegarde f.c en f.c.orig
find /tmp/projet -type f -name '*.orig' | wc -l                         # b. doit valoir...
find /tmp/projet -type f -name '*.[ch]' | wc -l                         #    ...ce nombre
find /tmp/projet -type f -name '*.[ch]' \
     -exec sed -i -e 's/dupont/Dupont/g' -e 's/jean/Jean/g' {} \;       # c. remplacement sur place
find /tmp/projet -type f -name '*.[ch]' -exec grep -w -l -e jean -e dupont {} \;   # d. ne doit rien afficher
find /tmp/projet -type f -name '*.[ch]' \
     -exec grep -q -w Jean {} \; -print                                 # e. 1) liste ceux qui contiennent Jean
find /tmp/projet -type f -name '*.[ch]' \
     -exec grep -q -w Jean {} \; -exec cp {}.orig {} \;                 # e. 2) les restaure depuis .orig
find /tmp/projet -type f -exec grep -w -l -e Jean {} \;                 # f. plus rien
find /tmp/projet -type f -exec grep -w -l -e Dupont {} \;               # g. liste.c et liste.h
```

Attention en **d** : la commande du sujet (`find /tmp/projet -type f ...`, sans `-name`) cherche aussi dans les `.orig`, les `Makefile` et `bug.c.save`, qui contiennent encore `jean` ou `dupont`. Elle affiche donc ces fichiers. Pour vérifier **seulement les .c et .h**, on ajoute `-name '*.[ch]'` : là, rien ne doit s'afficher.
De même en **e**, on limite à `*.[ch]` pour ne pas tenter de restaurer des `.orig.orig`.
Résultat attendu en e : `bug.c` et `data.h` sont restaurés.

**Q5 : recopier seulement les répertoires, les .c et les .h.** On se place dans le projet pour avoir des chemins relatifs (`./src/...`), réutilisés sous `/tmp/proj` :
```sh
( cd $P ; find . -type d -exec mkdir -p /tmp/proj/{} \; )               # 1) arborescence des répertoires
( cd $P ; find . -type f -name '*.[ch]' -exec cp {} /tmp/proj/{} \; )   # 2) fichiers .c et .h
```

---

## Exercice 13 : `tar`

| Option | Sens |
|---|---|
| `-c` | créer une archive |
| `-x` | extraire |
| `-t` | lister le contenu |
| `-f F` | fichier archive F (`-` = stdin/stdout) |
| `-v` | verbeux |
| `-C dir` | travailler dans dir au lieu du CWD |
| `-z` / `-j` | compression gzip / bzip2 |

```sh
( cd $(dirname /pub/FISE_OSSE11/shell/projet) ; tar -cvf /tmp/projet.tar projet )   # Q1
tar -C $(dirname /pub/FISE_OSSE11/shell/projet) -cvf /tmp/projet.tar projet         # Q1 (même chose avec -C)
tar -tf /tmp/projet.tar          # Q2 : liste les fichiers
tar -tvf /tmp/projet.tar         #      avec détails (droits, taille, date)
tar -C /tmp -xf /tmp/projet.tar  # Q3 : extrait dans /tmp
rm -rfv /tmp/projet /tmp/projet.tar                                         # Q4
( cd /pub/FISE_OSSE11/shell ; tar -cvf /tmp/code.tar $(find projet -type f -name '*.[ch]') )   # Q5
```

**Pourquoi `cd` dans le répertoire parent ?** L'archive stocke les chemins **tels qu'on les donne**. Avec `projet`, l'archive contient `projet/...` et pourra être extraite n'importe où. Avec `/pub/...`, elle contiendrait tout le chemin.
Q5 : `$(find ...)` est remplacé par la liste des fichiers .c et .h, qui deviennent les arguments de tar.

---

## Exercice 14 : examen 2022-2023

```sh
rmdir /tmp/log                               # 1. rmdir refuse un répertoire non vide
head -n 5 projet.c                           # 2.
grep -w Array main.ml                        # 3. (-w : le mot entier)
sed -i -e 's/IBD/CBDR11/g' maquette.html     # 4. -i : modifie le fichier, g : toutes les occurrences
find ~ -mindepth 1 -type d | wc -l           # 5. tous les sous-répertoires (récursivement)
find ~ -mindepth 1 -maxdepth 1 -type d | wc -l   # 5. variante : seulement les sous-répertoires directs
find /tmp -mindepth 1 -exec stat -c '%U' {} \; 2>/dev/null | sort -u   # 6. propriétaires, triés, sans doublon
```
Pour la 5, `-mindepth 1` exclut le HOME lui-même.

---

## Exercice 15 : simplification de commandes (examen 2017)

| Commande | Version simplifiée | Pourquoi |
|---|---|---|
| `cat < 1 \| cat \| cat > 2` | `cat 1 > 2` (ou `cp 1 2`) | les `cat` intermédiaires recopient juste le flux |
| `x="1$(echo 2 3)1"` | `x="12 31"` | `$(echo 2 3)` vaut `2 3` |
| `cat < $(echo 1)` | `cat 1` (ou `cat < 1`) | `$(echo 1)` vaut `1` |
| `cat "$(echo 1 > 2)2"` | `echo 1 > 2 ; cat 2` | `$(echo 1 > 2)` écrit `1` dans le fichier `2` et vaut une chaîne vide, donc on fait `cat 2` |

---

## Exercice 16 : révisions

```sh
P=/pub/FISE_OSSE11/shell/projet
gcc -c $P/src/util/bug.c 2>&1 | less           # 1. 2>&1 : stderr dans le pipe aussi ; on regarde la 1re erreur
cp -v $P/src/include/data.h ~                  # 2.
find $P -type f -size 0                        # 3. (ou -empty) : rien n'est affiché, donc aucun fichier vide
find $P -type f -name '*.c' -exec stat -c '%s' {} \; | sort -n -r | head -n 1   # 4. 117073
max=$(find $P -type f -name '*.c' -exec stat -c '%s' {} \; | sort -n -r | head -n 1)
find $P -type f -name '*.c' -size ${max}c      # 5. fichiers .c de cette taille
find /pub/FISE_OSSE11 -type f -exec stat -c '%U' {} \; | sort -u                # 6.
find /pub/FISE_OSSE11 -type f -user christophe.mouilleron | wc -l              # 7.
echo $(( $(find /pub/FISE_OSSE11 -type f -user christophe.mouilleron -exec stat -c '%s +' {} \;) 0 ))   # 8.
find $P -name '*.c' -exec grep -l -e '#include.*table\.h' {} \;                # 9.
( cd $(dirname $P) ; tar -cvf /tmp/burger.tar \
    $(find projet -type f -exec grep -q -e 'Patrice Burger' {} \; -print) )    # 10.
sed -e 's,\([0-9][0-9]\)/\([0-9][0-9]\)/\([0-9]\{4\}\),\3/\2/\1,' /pub/FISE_OSSE11/shell/select.dat   # 11. jj/mm/aaaa vers aaaa/mm/jj
sed -e 's,\([0-9][0-9]\)/\([0-9][0-9]\)/\([0-9]\{4\}\),\3/\2/\1,' /pub/FISE_OSSE11/shell/select.dat | sort -k 2   # 12.
```

Explications :
- **1.** La 1re erreur vient de `liste.h` (inclus par `bug.c`), ligne 4 : `Typedef` au lieu de `typedef`, d'où `error: expected ';' before 'struct'`.
- **8.** Chaque `stat` écrit `taille +`. On obtient `12 + 300 + ... + 0` (le `0` termine l'expression), que `$(( ))` additionne.
- **11.** On mémorise jour `\1`, mois `\2` et année `\3` (`\{4\}` = exactement 4 fois), puis on les réécrit dans l'ordre `\3/\2/\1`.
- **12.** Au format `aaaa/mm/jj`, l'ordre alphabétique est aussi l'ordre chronologique. `sort -k 2` trie à partir du 2e champ, la date.

---

## Exercice 17 : `/dev/pts/*`, `tty`, `stty`

```sh
tty                          # affiche le fichier spécial du terminal, ex : /dev/pts/3
xterm -name A &              # terminal A ; y taper tty pour avoir /dev/pts/N-A
stty erase m                 # (dans A) la touche m devient "effacer"
stty eof f                   # (dans A) la touche f devient "fin de fichier" (comme <CTRL-D>)
cat                          # (dans A) taper aaa, Entrée, puis f : cat se termine
stty -g                      # affiche la configuration du terminal (à comparer avec un autre)
stty erase 0x7f              # restaure : retour arrière = caractère 0x7f (DEL)
stty eof 0x04                # restaure : fin de fichier = 0x04 (<CTRL-D>)
cat > /dev/pts/N-B           # (dans A) ce qu'on tape dans A s'affiche dans B ; <CTRL-D> pour finir
echo 'hello world' > /dev/pts/N-A   # (dans B) écrit dans le terminal A
```

Le terminal est un **fichier spécial** (diapo 15) : on peut y écrire avec une redirection, comme dans n'importe quel fichier, si on en a le droit.

**Q12 : `stty -echo ; read -p 'Password: ' pass ; stty echo ; echo`.**
1. `stty -echo` désactive l'**écho** : les caractères tapés ne s'affichent plus à l'écran.
2. `read -p 'Password: ' pass` affiche l'invite `Password: `, puis lit une ligne de stdin dans la variable `pass`. Le mot de passe est saisi sans être affiché.
3. `stty echo` réactive l'écho. Sinon le terminal resterait « muet ».
4. `echo` ajoute le saut de ligne. L'Entrée tapée n'a pas été affichée non plus, donc sans ce `echo` le prompt suivant serait sur la même ligne.

---

# Mémo des commandes

## Fichiers et répertoires
| Commande | Usage | Options clés |
|---|---|---|
| `cd [dir]` | change le CWD | sans arg : HOME ; `-` : répertoire précédent |
| `pwd` | affiche le CWD | |
| `ls [f...]` | liste | `-l` détail, `-a` fichiers cachés, `-R` récursif |
| `mkdir d` | crée un répertoire | `-p` parents, `-v` |
| `rmdir d` | supprime un répertoire **vide** | |
| `cp src dst` | copie | `-r` récursif, `-v` |
| `mv src dst` | déplace ou renomme | `-v` |
| `rm f` | supprime | `-r` récursif, `-f` sans erreur, `-v` ; `rm ./-f` pour un fichier nommé `-f` |
| `touch f` | crée un fichier vide | |
| `chmod` | droits | `chmod 644 f`, `chmod u+x f`, `chmod go-w f` |
| `stat -c FMT f` | méta-données | `%s` `%U` `%a` `%b` `%B` |
| `find` | recherche | `-type` `-name` `-size` `-user` `-exec ... {} \;` |
| `tar` | archives | `-c` `-x` `-t` `-f` `-v` `-C` `-z` |

## Contenu des fichiers
| Commande | Usage |
|---|---|
| `cat f...` | concatène vers stdout (`-` = stdin) |
| `less f` | pageur (`q` pour quitter, `/` pour chercher) |
| `head -n N` / `tail -n N` | N premières / dernières lignes |
| `wc` | `-l` lignes, `-w` mots, `-c` octets |
| `grep -e MOTIF` | lignes qui contiennent le motif (`-v -w -c -l -n -q -i -H`) |
| `cut -f N [-d SEP]` | colonnes |
| `sed -e 'SCRIPT'` | édition de flux (`d`, `p`, `q`, `s///g`, `-n`, `-i`) |
| `sort` | tri (`-n -r -u -k -t`) |
| `uniq` | supprime les doublons **consécutifs** |

## Shell
| Syntaxe | Sens |
|---|---|
| `cmd > f` / `>> f` | stdout vers f (écrase / ajoute à la fin) |
| `cmd < f` | stdin depuis f |
| `cmd 2> f` / `2>&1` | stderr vers f / stderr vers stdout |
| `cmd1 \| cmd2` | stdout de cmd1 vers stdin de cmd2 |
| `cmd1 ; cmd2` | séquence |
| `cmd1 && cmd2` | cmd2 seulement si cmd1 réussit |
| `cmd1 \|\| cmd2` | cmd2 seulement si cmd1 échoue |
| `( ... )` / `{ ... ; }` | groupe dans un sous-shell / dans le shell courant |
| `$(cmd)` | remplacé par le stdout de cmd |
| `$(( expr ))` | calcul entier |
| `VAR=val` / `export VAR` | variable locale / la marquer pour l'environnement |
| `VAR=val cmd` | variable changée seulement pour cmd |
| `* ? [abc] {x,y} ~` | expansions de noms de fichiers (diapo 47) |
| `\c` `"..."` `'...'` | échappement : d'un caractère / partiel / total (diapo 49) |
| `cmd &`, `fg`, `bg`, `<CTRL-Z>`, `<CTRL-C>` | contrôle des processus |
| `$?` | statut de la dernière commande (0 = ok) |
