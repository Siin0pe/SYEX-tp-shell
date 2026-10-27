# Partiel 19 janvier 2026 (corrigé)

Sujet : `../sujets/partiel_osse_2026.pdf`. Durée 1h30, trois exercices indépendants.

| Exercice | Fichier(s) | Rôle |
|---|---|---|
| 1 | `filter_by_keywords.sh` | script de la figure 1 corrigé |
| 2 | `sed_x.c` | simulation de `sed -e 's/x//g' file.txt > file.txt` |
| 3 | `1234.c` `1234-pipe.c` `1234-signal.c` | programme 1234 : version de base, puis avec pipe, puis avec pipe + signal |

```sh
for p in sed_x 1234 1234-pipe 1234-signal ; do gcc -Wall -Wextra -o $p $p.c ; done
```

---

## Exercice 1 : script `filter_by_keywords.sh`

Le script de la figure 1, tel qu'il est donné :

```sh
1  #!sh
4  check() {
5      local res=$(head -n 5 "$1" | sed -n "/$2/p")
6      if [ -z "$res" ] ; then
7          return 0
8      else
9          return 1
10     fi
11 }
13 if [ $# < 2 ] ; then
14     echo 2>&1 "usage: $0 <dirname> <keyword1> [<keyword2> ...]"
15
16 fi
18 $dirname = "$1"
19 pop
21 $skip = no
22 find "$dirname" -mindepth 1 -type f -name '*.txt' \
23 while read $file ; do
24     for keyword in $@ ; do
25         if [ ! check "$file" "$keyword" ] ; then
26             $skip = yes
27             break
28         fi
29     done
30     if [ skip = "no" ] ; then
31         echo "$file"
32     fi
33
```

### 1.1 Fonctionnement de `check`

`check file keyword` :
1. `head -n 5 "$1"` : envoie les 5 premières lignes du fichier `$1` dans le pipe.
2. `sed -n "/$2/p"` : `-n` désactive l'affichage automatique. La commande `/$2/p` affiche seulement les lignes qui correspondent à la regex `$2`. Le mot-clé est inséré dans le script sed : les guillemets doubles laissent le shell remplacer `$2`.
3. `$( ... )` récupère ces lignes dans la variable locale `res`.
4. `[ -z "$res" ]` : si `res` est vide, aucune des 5 lignes ne contient le mot-clé, et la fonction renvoie **0**. Sinon, elle renvoie **1**.

Donc `check` **réussit (0, « vrai » en shell) quand le mot-clé est ABSENT**. C'est l'inverse de ce que son nom et son usage à la ligne 25 laissent attendre (voir 1.2).

> Si la ligne 5 est lue `sed -e "/$2/p"` au lieu de `-n`, c'est encore pire : sed affiche toutes les lignes, et en double celles qui correspondent. `res` n'est donc vide que si le fichier est vide.

### 1.2 Problèmes et correctifs

| Ligne | Problème | Correctif |
|---|---|---|
| 1 | `#!sh` : le shebang doit être un chemin absolu | `#!/bin/sh` |
| 5 | `$2` est interprété comme une **regex** sed : `.`, `*` ou `[` changent le sens, et un `/` dans le mot-clé casse la commande (injection dans le script sed) | `grep -q -F -e "$2"` (`-F` : chaîne fixe ; `-q` : seulement le statut) |
| 5 | `local` n'est pas POSIX (il marche avec dash et bash) | inutile avec grep : plus de variable |
| 6-10 | statut inversé : renvoie 0 quand le mot-clé est absent | renvoyer 0 quand il est présent. Avec grep, c'est direct : `head -n 5 "$1" \| grep -q -F -e "$2"` |
| 13 | `<` dans `[ ]` est une **redirection** (lit le fichier `2`), pas une comparaison | `[ $# -lt 2 ]` |
| 14 | `2>&1` envoie stderr vers stdout : le message part sur la sortie **standard** | `echo "..." >&2` (c'est `1>&2`) |
| 15 | il manque `exit` : le script continue après le message d'usage | `exit 1` |
| — | on ne vérifie pas que `$1` est un répertoire | `[ -d "$1" ] \|\| { echo ... >&2 ; exit 1 ; }` |
| 18 | `$dirname = "$1"` : pas de `$` à gauche, pas d'espaces autour de `=`. Le shell tenterait d'exécuter une commande nommée d'après la valeur de `$dirname` | `dirname="$1"` |
| 19 | `pop` n'existe pas | `shift` (enlève `$1`, `"$@"` ne contient plus que les mots-clés) |
| 21, 26 | même erreur d'affectation que ligne 18 | `skip=no`, `skip=yes` |
| 21 | **logique** : `skip` n'est initialisé qu'une fois, avant la boucle. Dès qu'un fichier est écarté, tous les suivants le sont aussi | mettre `skip=no` **au début du corps** du `while` |
| 22 | `-mindepth 1` ne limite pas la profondeur : on descend dans les sous-répertoires | `-maxdepth 1` |
| 22 | `\` en fin de ligne continue la commande mais il manque le **pipe** : `find ... while` est une seule commande absurde | `find ... -name '*.txt' \|` |
| 23 | `read $file` : `read` attend un **nom** de variable, pas sa valeur | `read file`, et mieux `IFS= read -r file` (garde les espaces en début ou fin de nom et les `\`) |
| 24 | `$@` sans guillemets : un mot-clé contenant un espace serait coupé en deux | `"$@"` |
| 25 | `[ ! check ... ]` : `[` est la commande `test`. Elle ne sait pas exécuter une fonction : elle reçoit `check` comme une chaîne | `if ! check "$file" "$keyword" ; then` (avec `check` corrigée : vrai = présent) |
| 30 | `skip` sans `$` : on compare la chaîne littérale « skip » à « no », donc c'est toujours faux | `[ "$skip" = "no" ]` |
| 33 | il manque le `done` qui ferme le `while` | `done` |

Remarque : le `while` est à droite d'un pipe, il tourne donc dans un sous-shell. Ce n'est pas gênant ici, car `skip` n'est utilisé que dans la boucle.

Version corrigée : `filter_by_keywords.sh`. Test (fichiers `d/1.txt` = « a foo / bar », `d/2.txt` = « foo », `d/sub/4.txt` = « foo bar », `d/6.txt` = « foo bar » en ligne 6) :

```
$ sh filter_by_keywords.sh d foo bar
d/1.txt
$ sh filter_by_keywords.sh d foo
d/1.txt
d/2.txt
```

---

## Exercice 2 : `sed -e 's/x//g' file.txt > file.txt`

### 2.1 Flux noyau ou libc ?
**Flux libc** : l'ébauche manipule des `FILE*` (`apply_sed(FILE* input, FILE* output)`, `FILE* open_input(...)`). On utilise donc `fopen`, `fread`, `fwrite` et `fclose`.

### 2.2 `apply_sed`
```c
void apply_sed(FILE* input, FILE* output) {
    char ch;
    while (fread(&ch, 1, 1, input) == 1)
        if (ch != 'x')
            fwrite(&ch, 1, 1, output);
}
```

### 2.3 `open_input` et `open_output`
```c
FILE* open_input(const char* filepath) {
    FILE* f = fopen(filepath, "r");          // lecture seule
    if (f == NULL) { perror(filepath); exit(1); }
    return f;
}

FILE* open_output(const char* filepath) {
    FILE* f = fopen(filepath, "w");          // écriture seule (O_WRONLY|O_CREAT|O_TRUNC)
    if (f == NULL) { perror(filepath); exit(1); }
    return f;
}
```
(Avec `"w"`, un fichier absent est **créé** : l'erreur « n'existe pas » ne peut se produire qu'en lecture. En écriture, l'échec vient des droits ou d'un répertoire inexistant.)

### 2.4 Ordre des appels
Le shell découpe la ligne de commande, puis met en place les **redirections avant de lancer** la commande. `> file.txt` est donc ouvert par le shell, dans le processus fils, avant le `exec` de sed. Ensuite seulement, sed ouvre `file.txt` en lecture et applique la substitution.
Ordre fidèle : **`open_output`, puis `open_input`, puis `apply_sed`**.

### 2.5 `main`
```c
int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s file\n", argv[0]);
        exit(1);
    }
    FILE* output = open_output(argv[1]);   // la redirection > d'abord
    FILE* input  = open_input(argv[1]);    // puis sed ouvre le fichier
    apply_sed(input, output);
    fclose(input);                         // clean up
    fclose(output);
    return 0;
}
```

### 2.6 La troncature
Ouvrir un fichier existant en écriture seule (`"w"`, ou `O_TRUNC` en flux noyau) le **tronque** : sa taille est ramenée à 0 octet et son contenu est perdu, avant même la première écriture.

### 2.7 Impact et défaut de la commande
`open_output` tronque `file.txt` **avant** que `open_input` ne l'ouvre. Quand `apply_sed` lit, le fichier est déjà vide : `fread` renvoie 0 tout de suite, rien n'est écrit, et `file.txt` reste **vide**.
Défaut de la commande shell : **elle efface le fichier au lieu de le modifier**. On ne peut pas lire et rediriger vers le même fichier dans une seule commande. Correctifs : passer par un fichier temporaire (`sed -e 's/x//g' file.txt > tmp && mv tmp file.txt`) ou utiliser `sed -i -e 's/x//g' file.txt`.

Vérifié : `printf 'xaxbx\nyx\n' > file.txt ; ./sed_x file.txt` donne un fichier de **0 octet**.

---

## Exercice 3 : programme 1234

### 3.1 Code
Voir `1234.c`. L'essentiel :
```c
pid_t f1 = fork();
if (f1 == 0) {                         // fils1
    write(1, "1 ", 2);
    pid_t pf = fork();
    if (pf == 0) { write(1, "4444 ", 5); exit(0); }   // petit-fils
    write(1, "333 ", 4);
    waitpid(pf, NULL, 0);
    exit(0);
}
pid_t f2 = fork();
if (f2 == 0) { write(1, "22 ", 3); exit(0); }          // fils2
waitpid(f1, NULL, 0);                  // père
waitpid(f2, NULL, 0);
write(1, "\n", 1);
```
(On teste aussi `fork() == -1` à chaque fois, voir le fichier.)

### 3.2 Sorties possibles
Contraintes d'ordre : `1` avant `333` et avant `4444`, car fils1 écrit `1` avant de créer petit-fils. `\n` vient en dernier, car le père attend ses deux fils et fils1 attend petit-fils. `22` peut sortir n'importe où avant `\n`, et `333`/`4444` dans n'importe quel ordre.
Soit 2 ordres pour (333, 4444) × 4 places pour 22 = **8 sorties** :
```
1 333 4444 22      1 4444 333 22
1 333 22 4444      1 4444 22 333
1 22 333 4444      1 22 4444 333
22 1 333 4444      22 1 4444 333
```
(chacune suivie du `\n`). Observé sur 1000 exécutions : `1 22 333 4444` (881), `1 333 22 4444` (64), `1 333 4444 22` (42), `22 1 333 4444` (11), `1 22 4444 333` (2). Les autres ordres sont possibles mais rares.

### 3.3 Le pipe (`1234-pipe.c`)
**(a)** Dans le **père, avant le premier `fork`**. fils2 hérite du pipe directement. petit-fils en hérite via fils1, qui le reçoit du père puis le transmet à son propre fork.

**(b)**
```c
int p[2];
if (pipe(p) == -1) { perror("pipe"); exit(1); }
```

**(c)** Côté fils2 :
```c
close(p[0]);              // fils2 ne lit pas
write(1, "22 ", 3);
write(p[1], "x", 1);      // débloque petit-fils
close(p[1]);
```

**(d)** Côté petit-fils :
```c
char c;
read(p[0], &c, 1);        // bloque tant que fils2 n'a rien écrit
close(p[0]);
write(1, "4444 ", 5);
```

**(e)** Fermer chaque bout dès qu'il est inutile :
- **père** : `p[0]` et `p[1]` juste après le **second fork** (création de fils2). Plus tôt, les fils n'en hériteraient pas.
- **fils1** : `p[1]` dès le début, car ni lui ni petit-fils n'écrivent. `p[0]` juste **après le fork de petit-fils**, qui en a hérité.
- **fils2** : `p[0]` dès le début, `p[1]` après l'écriture de `x`.
- **petit-fils** : `p[0]` après le `read` (`p[1]` est déjà fermé, puisque fils1 l'a fermé avant le fork).

Si fils2 mourait sans écrire, le `read` de petit-fils renverrait 0 (fin de fichier) : il faut que **tous** les bouts d'écriture soient fermés, d'où l'importance de ces `close`.

### 3.4 Sorties avec le pipe
Nouvelle contrainte : `22` avant `4444`. Il reste `1 < 333`, `1 < 4444` et `22 < 4444`, soit **5 sorties** :
```
1 22 333 4444      1 22 4444 333      1 333 22 4444
22 1 333 4444      22 1 4444 333
```
Observé : `1 22 333 4444`, `1 333 22 4444` et `22 1 333 4444`. `4444` n'apparaît plus jamais avant `22`.

### 3.5 Le signal (`1234-signal.c`)
**(a)** Le père reçoit le PID de fils1 comme valeur de retour de son `fork()`. Comme fils1 est créé **avant** fils2, la variable `f1` contient déjà ce PID au moment du second `fork`, et fils2 en hérite dans sa copie de la mémoire.

**(b)** Côté fils1, juste après la création de petit-fils :
```c
pause();                  // attend SIGUSR1
write(1, "333 ", 4);
```

**(c)** Côté fils2, juste après l'écriture :
```c
write(1, "22 ", 3);
kill(f1, SIGUSR1);
```

**(d)** Il faut **installer un handler pour SIGUSR1**. L'action par défaut de SIGUSR1 est de **terminer** le processus : sans handler, fils1 serait tué au lieu d'être réveillé, et n'écrirait jamais `333`.
```c
void handler(int sig) { (void)sig; }
...
signal(SIGUSR1, handler);  // dans le père, AVANT les fork
```
On l'installe dans le père avant les `fork` (le handler est hérité). Si fils1 l'installait lui-même, fils2 pourrait envoyer le signal avant, et fils1 serait tué.

Piège restant : si SIGUSR1 arrive **avant** que fils1 n'atteigne `pause()`, par exemple quand fils2 est très rapide, le handler s'exécute tout de suite. `pause()` attend ensuite un signal qui ne viendra plus : **interblocage**. Le programme affiche `22 1 4444 ` puis reste bloqué, car le père attend fils1 indéfiniment. Parade : un drapeau mis à 1 par le handler (`while (!recu) pause();`, c'est ce que fait `1234-signal.c`). La version sans aucune course utilise `sigprocmask` + `sigsuspend`.

### 3.6 Sorties avec pipe + signal
Désormais `333` et `4444` sortent tous deux après `22`, et `1` reste avant `333` et `4444`. (1, 22) dans un ordre quelconque, puis (333, 4444) dans un ordre quelconque, soit **4 sorties** :
```
1 22 333 4444      1 22 4444 333
22 1 333 4444      22 1 4444 333
```
Observé sur 300 exécutions : `1 22 333 4444` (211), `1 22 4444 333` (87), `22 1 333 4444` (2). Avec un simple `pause()` sans drapeau, le cas `22 1 ...` peut provoquer l'interblocage décrit en 3.5.
