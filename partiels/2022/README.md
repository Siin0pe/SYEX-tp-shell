# Examen 2021-2022 (corrigé)

Sujet : `../sujets/partieloss2022_sujet.pdf`. Durée 1h30, seul le poly de cours est autorisé.

| Exercice | Fichier(s) | Rôle |
|---|---|---|
| 3 | `premiere_ligne.sh` | affiche n fois la première ligne d'un fichier |
| 4 | `il_fait_beau.c` | 3 fils, sortie toujours « il fait beau » |
| 5 | `wc_parallele_n.c` | `tp-wc-parallele.c` généralisé à N fils |

```sh
for p in il_fait_beau wc_parallele_n ; do gcc -Wall -Wextra -o $p $p.c ; done
```

---

## Exercice 1 : commandes shell (5 points)

**1.** Lignes 3 et 7, avec `head` et `tail` :
```sh
head -n 3 f | tail -n 1 ; head -n 7 f | tail -n 1
```
(`head -n 7 f | tail -n 5` affiche les lignes 3 **à** 7.)

**2.** `export VAR=valeur` (ou `VAR=valeur ; export VAR`) crée une variable d'**environnement**.
Différence : une variable normale n'existe que dans le shell courant. Une variable d'environnement est **copiée dans l'environnement des processus fils** (commandes et scripts lancés depuis ce shell), qui peuvent la lire (`getenv` en C). La copie est à sens unique : un fils qui la modifie ne change pas celle du père.
```sh
X=1 ; sh -c 'echo "[$X]"'          # []  : X n'est pas transmise
export X=1 ; sh -c 'echo "[$X]"'   # [1]
```

**3.** Fichiers de suffixe `.h` ou `.c` de `/tmp` :
```sh
ls -d /tmp/*.[ch]
# ou, sans erreur s'il n'y en a aucun et en excluant les répertoires :
find /tmp -maxdepth 1 -type f -name '*.[ch]'
```

**4.** Nombre de fichiers de `/tmp` contenant `Bob` :
```sh
grep -l -F Bob /tmp/* 2>/dev/null | wc -l
```
`-l` affiche chaque fichier qui correspond **une seule fois** : on compte des fichiers, pas des lignes. `2>/dev/null` masque les messages sur les répertoires et les fichiers illisibles. En récursif : `grep -r -l -F Bob /tmp | wc -l`.

**5.** Troisième champ de chaque ligne d'un CSV :
```sh
cut -d , -f 3 fichier.csv
```

---

## Exercice 2 : sed (2 points)

```sh
sed -e 's/\([A-Z][a-z]\+\).*/\1/'
```
Le motif cherche la **première** majuscule suivie d'**au moins une** minuscule (`[a-z]\+`), et la capture (`\( \)`). `.*` prend ensuite tout le reste de la ligne. Le tout est remplacé par le mot capturé `\1`. Ce qui précède le mot n'est pas touché.
L'effet : on **coupe la ligne juste après le premier mot qui commence par une majuscule**. Une ligne sans un tel mot reste inchangée.

| Entrée | Sortie | Pourquoi |
|---|---|---|
| `Bonjour monsieur` | `Bonjour` | « Bonjour » correspond, «  monsieur » est mangé par `.*` |
| `bonjour Monsieur` | `bonjour Monsieur` | « Monsieur » correspond, mais il n'y a rien après : la ligne ne change pas |
| `comment Allez vous ?` | `comment Allez` | « Allez » correspond, «  vous ? » est supprimé |
| `bien A vous` | `bien A vous` | `A` est suivi d'un espace, pas d'une minuscule : aucune correspondance |

(Vérifié avec GNU sed.)

---

## Exercice 3 : script bash (3 points)

Le sujet corrigé à la main dit « deux arguments `n` et `f` » : afficher `n` fois la première ligne du fichier `f`.
```bash
#!/bin/bash
if [ $# -ne 2 ] ; then
    echo "usage: $0 n f" >&2
    exit 1
fi
n="$1"
f="$2"
case "$n" in
    '' | *[!0-9]*) echo "$0: '$n' is not a non-negative integer" >&2 ; exit 1 ;;
esac
if [ ! -f "$f" ] || [ ! -r "$f" ] ; then
    echo "$0: cannot read file '$f'" >&2
    exit 1
fi
ligne=$(head -n 1 "$f")
i=0
while [ $i -lt $n ] ; do
    printf '%s\n' "$ligne"
    i=$((i+1))
done
```
Les tests portent sur le nombre d'arguments, sur `n` (entier, sinon `[ -lt ]` plante) et sur `f` (fichier régulier lisible). Les messages d'erreur vont sur stderr, avec un statut ≠ 0. On lit la ligne **une seule fois** au lieu de relancer `head` à chaque tour.

---

## Exercice 4 : fork (5 points)

Le père crée trois fils, dans l'ordre : fils 1 écrit `beau`, fils 2 `il`, fils 3 `fait`. La sortie doit **toujours** être `il fait beau`.

Les trois fils s'exécutent en **concurrence** : sans synchronisation, n'importe quel ordre peut sortir. Un `wait` dans le père ne suffit pas, car les fils sont déjà lancés et écrivent dès qu'ils le peuvent. Des `sleep` rendraient le bon ordre très probable, mais pas certain.

Solution sûre, en flux noyau uniquement : **un jeton qui circule dans des pipes**. `read` sur un pipe vide est **bloquant**.
- pipe `a` : de fils 2 vers fils 3 ; pipe `b` : de fils 3 vers fils 1 ;
- fils 2 : `write(1, "il ", 3)`, puis écrit un octet dans `a` ;
- fils 3 : `read` sur `a` (attend fils 2), `write(1, "fait ", 5)`, puis écrit un octet dans `b` ;
- fils 1 : `read` sur `b` (attend fils 3), `write(1, "beau\n", 5)` ;
- père : crée les deux pipes **avant** les `fork`, ferme tous les bouts après, puis `wait(NULL)` trois fois.

Chaque processus ferme les bouts dont il n'a pas besoin. Si un fils mourait sans écrire, le `read` de celui qui l'attend recevrait EOF au lieu de bloquer indéfiniment. Code complet : `il_fait_beau.c`. Vérifié : 500 exécutions sur 500 donnent `il fait beau`.

(Autre solution acceptable, mais qui n'est plus « créer trois fils puis attendre » : le père crée fils 2 et l'attend, puis fils 3 et l'attend, puis fils 1. Les fils sont alors séquentiels et le parallélisme disparaît.)

---

## Exercice 5 : `tp-wc-parallele.c` (5 points)

### 1. Comportement
Le programme **compte les lignes** (caractères `'\n'`) d'un fichier, comme `wc -l`, en coupant le travail en **deux moitiés traitées en parallèle** par deux fils.
1. `stat(argv[1], &info)` récupère la taille du fichier, `info.st_size`. En cas d'échec, message sur stderr avec `strerror(errno)` et `exit(1)`.
2. `access(argv[1], O_RDONLY)` doit vérifier les droits de lecture. (Bug : `O_RDONLY` est un drapeau de `open` qui vaut 0, c'est-à-dire `F_OK`. On teste donc seulement l'**existence**. Il faudrait `R_OK`.)
3. `sz` = taille, `sz2 = sz/2` = milieu. `tmp = malloc(sz2+2)` dans `main` ne sert à rien : chaque fils refait son propre `malloc`, et on a une fuite.
4. `pipe(f2p)` : canal **fils → père** (« f2p »), créé avant les `fork` pour être hérité.
5. Deux `fork()`. Le premier fils traite `[0, sz2[`, le second `[sz2, sz[`. Le père n'entre pas dans le `if`. Le fils, lui, ne revient jamais de `fils()`, qui se termine par `exit(0)`.
6. **Chaque fils** (`fils(fn, deb, fin)`) alloue `fin-deb` octets, ouvre le fichier (ce qui lui donne son **propre** offset), se place en `deb` avec `lseek(fd, deb, SEEK_SET)`, lit `fin-deb` octets avec un seul `read`, et compte les `'\n'`. Il affiche son PID et son compte, écrit `cnt` (un `int`) dans `f2p[1]`, puis `exit(0)`.
7. Le **père** lit deux `int` dans `f2p[0]`. `read` bloque tant qu'un fils n'a pas écrit, et l'ordre d'arrivée importe peu, puisqu'on additionne. Il affiche la somme, qui est le nombre total de lignes, puis fait `wait(NULL)` deux fois pour récupérer les fils.

Exemple : `./tp-wc-parallele /etc/services` affiche `pid: cnt=...` deux fois (dans un ordre quelconque), puis le total, égal à `wc -l < /etc/services`.

Autres faiblesses : `argc` n'est pas vérifié (`argv[1]` peut être `NULL`). Les retours de `open`, `read` et `pipe` ne sont pas testés. `read` peut lire moins que demandé. Les bouts inutiles du pipe ne sont pas fermés : sans conséquence ici, car le père fait exactement deux lectures. Enfin, le découpage en octets ne pose pas de problème pour compter des `'\n'`, car chaque octet est dans exactement une tranche.

### 2. Modification du nombre de fils
Le listing utilise **déjà deux fils**. La question vise donc à comprendre ce qui dépend du nombre de fils, pour passer à N fils (l'annotation manuscrite du sujet évoque 3). Il faut changer trois choses :
1. **découpage** : la tranche `k` (0 ≤ k < N) va de `k*sz/N` à `(k+1)*sz/N`. La dernière finit exactement en `sz`, donc aucun octet n'est oublié ;
2. **création** : une boucle `for (k = 0; k < N; k++) if (fork() == 0) fils(argv[1], k*sz/N, (k+1)*sz/N);` ;
3. **collecte** : N lectures d'un `int` dans `f2p[0]` (additionnées), puis N `wait(NULL)`.

Avec un seul fils (N = 1), le fils lit tout le fichier et le parallélisme disparaît. Version N fils : `wc_parallele_n.c`. Vérifié avec N = 3 sur `/etc/services` : 130 + 125 + 106 = **361**, comme `wc -l`.
