# TP2 : Scripts Shell (corrigé)

Tous les scripts sont en `sh` (sur ces machines, `/bin/sh` est `dash`) et commentés ligne par ligne.
Ils ont été testés avec `sh` sur les fichiers de `/pub/FISE_OSSE11/shell`.

## Installation (comme dans le sujet)

```sh
mkdir -pv ~/OSSE11/bin                      # crée le répertoire (et ses parents avec -p)
cp -v tp2_corrige/* ~/OSSE11/bin/           # copie les scripts
chmod -v u+x ~/OSSE11/bin/*                 # droits d'exécution pour le propriétaire
export PATH="$HOME/OSSE11/bin:$PATH"        # PAS d'espace autour du =, $HOME plutôt que ~
```

Les scripts qui en appellent d'autres (`punition3.sh`, `punition.sh`) ou qui sourcent une bibliothèque (`s3*`, `factdiv`) cherchent ces fichiers **dans leur propre répertoire** avec `$(dirname "$0")`.
Le cours le rappelle (diapo 23) : l'argument 0 sert à « retrouver le répertoire d'installation du programme ».
Ils fonctionnent donc quel que soit le répertoire courant.

## Fichiers

| Exercice | Fichier(s) | Rôle |
|---|---|---|
| 1 | `punition1.sh` `punition2.sh` `punition3.sh` `punition.sh` | lignes répétées |
| 2 | `iacmp` | wrapper avec `export` et `exec` |
| 3 | `sortarg1` `sortarg2` `sortarg3` `sortarg4` | tri des arguments |
| 4 | `s3tool.sh` | bibliothèque `s36str`, `s36int`, `isInt` (à sourcer) |
| 5 | `s3s` `s3e` `s3f` `s3d` `s3f1m` | tri de 3 éléments |
| 6 | `sleepsort` | tri par `sleep` |
| 7 | `pidfiles` | utilisateurs et nombre de fichiers, option `-s` |
| 8 | `util.sh` `factdiv` | examen 2017 |
| 9 | `extract.sh` `extract2.sh` | rattrapage 2024-2025 |
| 10 | `cce` | code C auto-exécutable |
| 11 | `shzip` | archive auto-décompressable |

---

## Exercice 1 : premiers scripts

```sh
sh ~/OSSE11/bin/punition1.sh 3 'It works!'   # 3 lignes "It works!"
sh ~/OSSE11/bin/punition2.sh 3 5 '*'         # 3 lignes de 5 étoiles
sh ~/OSSE11/bin/punition.sh                  # 10 lignes de 3 fois la phrase par défaut
sh ~/OSSE11/bin/punition.sh 3 5 '*'
```

À retenir :
- **`'*'` entre quotes.** Sans les quotes, le shell remplace `*` par la liste des fichiers du répertoire courant avant de lancer le script (expansion, diapo 46).
- **`"$1"`, `"$w"` toujours entre guillemets.** Sinon `'Je ne bavarde pas'` serait découpé en plusieurs arguments (diapo 92).
- **Différence entre les versions.** `punition2.sh` utilise deux boucles. `punition3.sh` construit la ligne une seule fois, puis délègue à `punition1.sh`. `punition.sh` n'a aucune boucle : il ne fait que choisir `n`, `m` et `w`.

## Exercice 2 : wrapper

**Q1 : sans modifier son environnement.** On préfixe la commande par les affectations (diapo 42). Elles ne valent que pour cette commande :
```sh
LD_LIBRARY_PATH=/pub/FISE_OSSE11/shell/iacmp/lib \
IacmpDir=/pub/FISE_OSSE11/shell/iacmp \
/pub/FISE_OSSE11/shell/iacmp/iacmp 'chat roux' 'chat blanc' 'chat noir'
```
Le `\` en fin de ligne échappe le saut de ligne, donc la commande continue sur la ligne suivante.

**Q2 : `export` dans le terminal.** Les variables sont marquées et transmises à tous les processus lancés ensuite depuis ce shell. En revanche, elles disparaissent quand on ferme le terminal.

**Q3 : le script `iacmp`.**
- `export` configure l'environnement.
- `exec` remplace le processus du shell par iacmp. Il n'y a pas de processus en plus, et le statut de iacmp devient celui du script.
- `"$@"` transmet les arguments intacts : `'chat roux'` reste un seul argument.

**Q4 : lancer simplement `iacmp`.**
- On ajoute `~/OSSE11/bin` au `PATH`. La commande `iacmp` est alors cherchée dans ce répertoire (diapo 24).
- Le premier essai donne `Permission denied`, parce que le fichier n'a pas le droit `x`. C'est réglé par `chmod u+x ~/OSSE11/bin/iacmp`.
- Le système lit alors la ligne `#!/bin/sh` et lance `/bin/sh ~/OSSE11/bin/iacmp args...` (diapo 54).

## Exercice 3 : boucles, shift, set

```sh
sh sortarg1 'chat roux' 'chat blanc' 'chat noir'   # chat blanc chat noir chat roux
sh sortarg3 c a b a                                # a a b c
```

- **`sortarg1`** passe par des fichiers : `for ... done > /tmp/1`, puis `sort`, puis `while read ... done < /tmp/2`.
- **`sortarg2`** fait pareil avec des `|` uniquement. Le `while read` est dans un groupe `{ }` placé après le pipe.
- **`sortarg3`** répète trois étapes : chercher le minimum, construire `args` sans lui, puis `set -- $args`.

**Pourquoi les `si` doivent-ils être sans espace dans `sortarg3` ?**
Tous les arguments restants sont concaténés dans une seule chaîne `args`, séparés par des espaces. Ensuite, `set -- $args` (sans guillemets) redécoupe cette chaîne **sur les espaces**.
Ainsi `'chat roux'` devient deux arguments, `chat` et `roux`. On ne peut plus distinguer l'espace qui sépare deux arguments de l'espace qui fait partie d'un argument.

**`sortarg4` (variante récursive).** Au lieu de `set --` et de la boucle externe, le script écrit le minimum puis fait `exec sh "$0" $args`. Il se relance sur les arguments restants et s'arrête quand `$# = 0`.

## Exercice 4 : fonctions, `.` (source)

```sh
s36str cc 33 bb 22 aa 11     # command not found : la fonction n'existe pas encore
. ~/OSSE11/bin/s3tool.sh     # "source" ne marche qu'en bash ; "." marche partout
s36str cc 33 bb 22 aa 11     # 11 22 33
s36int 01 03 2               # 01 2 03
isInt -01 ; echo $?          # 0
isInt a01b ; echo $?         # 1
```

**Pourquoi sourcer et ne pas exécuter ?** Exécuter `sh s3tool.sh` lance un **autre** shell. Les fonctions y seraient définies, puis perdues à la fin. Sourcer fait lire le fichier par le shell **courant** (diapo 55), donc les fonctions y restent définies.

**Après une modification de `s3tool.sh`**, il faut le sourcer à nouveau.

**Q5 : comment marche `isInt`, et quel est son défaut ?**
- `sed -r -e '1s/[+-]?[0-9]+//'` supprime, sur la ligne 1, la **première** suite formée d'un signe facultatif et d'au moins un chiffre.
- S'il ne reste rien (`test -z`), la chaîne n'était faite que de cela : c'est un entier.
- S'il reste des caractères (`a01b` donne `ab`), ce n'est pas un entier.
- **Défaut principal :** la chaîne vide est acceptée. `isInt "" ; echo $?` affiche `0`, car il n'y avait rien à supprimer et il ne reste rien. Un argument vide passerait donc la vérification de `s3e`.
- **Autres défauts :**
  - `local` n'est pas du `sh` POSIX strict.
  - Un très grand nombre (plus de 19 chiffres) est accepté mais dépasse les entiers 64 bits du shell.

## Exercice 5 : utilisation de `s3tool.sh`

Codes de retour choisis (conformes aux exemples du sujet) :

| Script | Statut 1 | Statut 2 | Statut 3 |
|---|---|---|---|
| `s3s` | mauvais nombre d'arguments | | |
| `s3e` | mauvais nombre d'arguments | argument non entier | |
| `s3f` | mauvais nombre d'arguments | fichier inexistant | pas un fichier régulier |
| `s3d` | mauvais nombre d'arguments | pas un répertoire | |
| `s3f1m` | mauvais nombre d'arguments | fichier inexistant | fichier illisible |

Méthode commune :
1. Vérifier les arguments. En cas d'erreur, écrire un message sur **stderr** (`1>&2`) et faire `exit n` avec `n ≠ 0`.
2. Calculer une **clé** par argument : la taille (`stat -c '%s'`), le nombre de fichiers (`find | wc -l`) ou le 2nd mot (`read w1 w2 reste < fichier`).
3. Appeler `s36int clé1 val1 clé2 val2 clé3 val3`, ou `s36str` pour un tri lexicographique.

Piège évité dans `s3f1m` : on écrit `read w1 k r < fichier` et non `cat fichier | read w1 k r`. Avec le pipe, `read` tourne dans un sous-processus et la variable est perdue (diapo 96).

## Exercice 6 : sleep sort

```sh
time sh sleepsort 3 1 2      # écrit 1 2 3, real ≈ 3 s
```

**Q2 : que se passe-t-il sans `{ }` et sans le `;` final ?**
La ligne devient `sleep n ; echo n &`. Le `&` ne s'applique plus qu'à `echo`. Chaque `sleep` est alors exécuté **en avant-plan, l'un après l'autre**, et les nombres sortent dans l'ordre des arguments, donc non triés. Le temps total vaut la **somme** des ni.

**Q3 : temps d'exécution.** Environ **max(ni)** secondes : tous les `sleep` tournent en parallèle.
Attention : deux nombres trop proches peuvent sortir dans le désordre. L'algorithme n'est pas fiable, c'est une curiosité.

**Q4 : à quoi sert `wait` ?** Il attend la fin des processus lancés en arrière-plan.
Sans `wait`, le script se termine tout de suite et le prompt revient. Les nombres s'affichent alors plus tard, par-dessus le terminal. De plus, quand le script est utilisé dans un pipe (`sleepsort 3 1 2 | ...`), la commande suivante ne voit pas la fin du flux au bon moment.

## Exercice 7 : pidfiles

```sh
sh pidfiles /var/tmp           # utilisateur nombre
sh pidfiles -s /var/tmp        # utilisateur nombre taille_cumulée
```

Algorithme :
1. `find dir -type f -exec stat -c '%U %s' {} \;` donne une ligne `propriétaire taille` par fichier régulier, stockée dans un fichier temporaire `/tmp/pidfiles.$$`. `$$` est le PID, ce qui rend le nom unique.
2. `cut -d ' ' -f 1 | sort -u` donne la liste des utilisateurs sans doublon.
3. Pour chaque utilisateur, une boucle `while read user sz ; do ... done < fichier` compte ses fichiers et additionne leurs tailles.

L'option `-s` est traitée en premier : si `$1` vaut `-s`, on la retient puis on fait `shift`.

## Exercice 8 : examen 2017 (`util.sh`)

**Q1 : que fait `t` ?** `t str` renvoie le statut 0 si `str` est un entier (avec un `+` ou un `-` facultatif) et 1 sinon. C'est `isInt`, écrit avec la syntaxe sed de base : `\?` et `\+` au lieu de `?` et `+` avec `-r`. Il a le même défaut : la chaîne vide est acceptée.

**Q2 : que fait `f` ?** `f x` écrit `x!` (factorielle) sur stdout, par récursion : si `x <= 1` le résultat est 1, sinon `x * f(x-1)`. L'appel récursif se fait dans `$( )`, donc dans un sous-processus. C'est pour cela que les variables **globales** `x` et `n` ne sont pas écrasées par la récursion.

**Q3 : script n!/m!.** Voir `factdiv`.
```sh
sh factdiv 5 3      # 20
sh factdiv 10 0     # 3628800
```
Piège : `t` modifie les variables **globales** `n` et `m`. Si le script stockait ses arguments dans `n` et `m`, l'appel `t "$m"` écraserait `n`. On utilise donc `a` et `b`.
Limite : les entiers du shell sont sur 64 bits, donc `f` déborde au-delà de 20!.

## Exercice 9 : rattrapage 2024-2025 (`extract.sh`)

**Q1 :** voir `extract.sh`, qui suit les 4 étapes de l'énoncé.
Point important : quand aucun fichier ne correspond, `for f in /tmp/*.tar.gz` donne la chaîne `/tmp/*.tar.gz` telle quelle (diapo 46). On teste donc `[ -e "$f" ]` avant d'écrire le message.

**Q2 : passer le répertoire en argument.** Modifications à apporter (voir `extract2.sh`) :
1. Vérifier le nombre d'arguments (0 ou 1).
2. Remplacer `d=/tmp` par `d="${1:-/tmp}"`. On obtient l'argument s'il est donné, `/tmp` sinon (diapo 40).
3. Vérifier que `$d` est un répertoire et que `$d/etudiants.txt` est lisible.
4. Écrire **tous** les chemins à partir de `"$d"` : `"$d/correction"`, `"$d/etudiants.txt"`, `"$d/$u.tar.gz"`, `"$d"/*.tar.gz`. Ils doivent être entre guillemets au cas où le chemin contiendrait des espaces.

Si le script initial utilise déjà une variable pour `/tmp`, seules les étapes 1 à 3 sont à faire.

## Exercice 10 : `#!cce`

```sh
cp /pub/FISE_OSSE11/shell/hello.c .
# ajouter "#!cce" en 1re ligne, 1re colonne
chmod u+x hello.c
./hello.c 2 good ; echo "status = $?"          # good good, status = 0
./hello.c 2 good x ; echo "status = $?"        # too many args., status = 1
SILENCE=1 ./hello.c 2 good                     # sans le #warning
DEBUG=1 ./hello.c 2 good                       # compilé avec -g, lancé dans gdb
```

Fonctionnement : avec `#!cce` en tête, `./hello.c 2 good` fait lancer par le système `cce ./hello.c 2 good` (diapo 54).
Ensuite, `cce` procède ainsi :
1. `sed -e 1d` retire la ligne `#!cce`, que gcc ne comprendrait pas.
2. `gcc -x c -` compile le reste lu sur stdin vers `/tmp/$$.a.out`.
3. Le programme est lancé avec `"$@"`, puis l'exécutable est supprimé.

**Attention au `#!`.** Le chemin écrit après `#!` est utilisé **tel quel** par le système, **sans passer par le `PATH`**.
- `#!cce` ne marche donc que si `cce` est dans le répertoire courant.
- Pour que ça marche partout, écrire le chemin absolu : `#!/home/<login>/OSSE11/bin/cce`.

## Exercice 11 : `shzip`

**Q1 : base64.**
```sh
echo 'aaa' > file1
base64 file1                    # YWFhCg==
base64 file1 > /tmp/file1.64
base64 -d /tmp/file1.64         # aaa
cat /tmp/file1.64 | base64 -d   # aaa (lit stdin si pas de fichier)
```

**Q2 à Q8 :** tout est dans `shzip` (version finale, avec l'option `-z`).
```sh
sh shzip /tmp/archive.shz 123          # crée l'archive (avertissements sur stderr)
cat /tmp/archive.shz                   # vérification à l'œil
mkdir '/tmp/new testdir'
/tmp/archive.shz '/tmp/new testdir'    # restauration (archive exécutable grâce au #!)
sh shzip -z /tmp/archive.shz 123       # version compressée (gzip puis base64)
```

Les points clés :
- **Un here document par fichier.** Pour chaque fichier, l'archive contient `base64 -d > "$1/nom" << 'EOF'`, puis le contenu encodé, puis `EOF`. Grâce à `'EOF'` entre quotes, rien n'est expansé dans le contenu.
- **`\$1` dans `shzip`.** Le `\` empêche l'expansion au moment de la création. L'archive contient donc littéralement `$1`, qui vaudra le répertoire destination au moment de la restauration (Q4).
- **`encodedir` est récursive.**
  - Elle parcourt `"$dir"/*` et `"$dir"/.*`, car `*` seul ne donne pas les fichiers cachés.
  - Elle **saute `.` et `..`** : sinon la récursion serait infinie (Q6).
  - Pour un sous-répertoire, elle écrit `mkdir -p` puis s'appelle sur le sous-répertoire.
  - Les variables sont `local` : sans cela, l'appel récursif écraserait `dir` et `rel` de l'appelant.
- **Fichiers ignorés.** Les liens, FIFO et fichiers spéciaux sont ignorés, avec un avertissement sur **stderr**. Comme seul stdout est redirigé dans l'archive, les avertissements n'y entrent pas.
- **Q7.** L'entête `#!/bin/sh` est ajouté en tête, puis `chmod u+x` rend l'archive exécutable.
- **Q8.** Avec `-z`, `gzip -c fichier | base64` à la création, et `base64 -d << 'EOF' | gunzip > "$1/nom"` à la restauration.

Limite connue : un nom de fichier contenant `"`, `$` ou `` ` `` casserait le code généré, car ces caractères restent actifs entre guillemets doubles (diapo 49).
