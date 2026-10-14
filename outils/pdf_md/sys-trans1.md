# sys-trans1.pdf

## Diapo 1 : Système d’exploitation

Système d’exploitation
Partie 1: Shell
Le 30 août 2022, SVN-ID 425
30 août 2022

## Diapo 2 : Table des matières

Table des matières
1 Fondement
Organisation
Système de protection
Fichiers et systèmes de fichiers
Processus
Système Unix

## Diapo 3 : Organisation

Organisation
C
P
I
L
S
N
P
A
A
T
I
O
A
R
B
I
L
B
A
S
E
C O
M
M
A
N
D
S
S
Y
S T
E
M
S
E
I
R
L
I
.
M
ON
K
E
RN
E
HARD
WARE
matériel CPU, RAM, contrôleurs et
périphériques.
moniteur Petit programme en ROM, qui
tourne au démarrage de la
machine.
noyau Gère et donne accès au matériel
système Couche de standardisation
applications

## Diapo 4 : Table des matières

Table des matières
1 Fondement
Organisation
Système de protection
Fichiers et systèmes de fichiers
Processus
Système Unix

## Diapo 5 : UID & GID (User & Group IDentifier)

UID & GID (User & Group IDentifier)
Le système de protection est basé sur les identifiants d’utilisateurs UID et
de groupes GID. Ce sont des entiers, des tables permettent de les
convertir en un nom humainement compréhensible.
utilisateur toute personne travaillant sur une machine a ouvert une
session
=⇒1 UID, 1 GID principal et 0 ou plusieurs GID auxiliaires.
1 UID identifie un utilisateur.
2 utilisateurs peuvent appartenir à un même groupe.
programme un programme est lancé par un utilisateur
=⇒1 UID, 1 GID principal et 0 ou plusieurs GID auxiliaires.
fichier il appartient à 1 seul utilisateur et à un seul groupe.
création de fichier il appartient à l’UID et au GID principal de l’utilisateur
(programme) qui l’a créé.

## Diapo 6 : Droits d’un fichiers

Droits d’un fichiers
droits d’accès
propriétaire
masque
format usuel
et groupe
octal
prop.
group
autre
UID
GID
777
rwx
rwx
rwx
101
100
600
rw-
---
---
110
200
700
---
rw-
---
110
200
644
rw-
r--
r--
0
0
755
rwx
r-x
r-x
0
0
Pour un fichier non répertoire
r
accès en lecture
w
accès en écriture
x
il est possible d’essayer de
le lancer
Pour un fichier répertoire
r
les noms des fichiers du
répertoire peuvent être lus
w
un fichier du répertoire peut
être créé ou détruit
x
les fichiers du répertoire
peuvent être accédés

## Diapo 7 : Changement des droits

Changement des droits
chmod masque-octal f1 f2 . . .
chmod 755 tutu
chmod 640 titi
chmod [ugoa] [+-=] [rwx] f1 f2 . . .
chmod a-x tutu
chmod go+rx titi toto

## Diapo 8 : Exercice

Exercice
Le répertoire D contient le fichier F.
mon
répertoire D
fichier F
accès
uid
gid
uid
gid
masque
uid
gid
masque
r
w
*
*
*
*
755
*
*
666
∗
∗
10
20
10
21
6**
11
20
6**
10
20
10
21
5**
11
20
2**
∗
10
20
10
21
1**
11
20
6**
∗
∗
10
20
11
20
*6*
11
20
*6*
10
20
11
20
*5*
11
20
*2*
∗
10
20
11
20
*1*
11
20
*4*
∗
10
20
11
21
**5
10
20
6**
∗
∗
10
20
11
21
**0
10
20
6**
10
20
11
21
**6
10
20
6**

## Diapo 9 : Table des matières

Table des matières
1 Fondement
Organisation
Système de protection
Fichiers et systèmes de fichiers
Processus
Système Unix

## Diapo 10 : Définition

Définition
/
F1 D1
D3
F1
F4
D2
F1
D1
F2
Arborescence de fichiers :
les noeuds : fichiers répertoires
les feuilles : fichiers ou répertoires
vides
la racine le haut de l’arbre
Support physique : disques durs, RAM

## Diapo 11 : Plusieurs systèmes de fichiers (Windows)

Plusieurs systèmes de fichiers (Windows)
D:\
D5
D1
F1
F1
F4
F3
C:\
F1 D1
D3
F1
F4
D2
F1
D1
F2
Sous Windows, les systèmes
de fichiers sont visibles
=⇒identifiés par une lettre
suivie de ’ :’.

## Diapo 12 : Plusieurs systèmes de fichiers (Unix)

Plusieurs systèmes de fichiers (Unix)
D1
D5
F1
F1
/
F1 D1
D3
F1
F4
D2
F1
D1
F2
Un système de fichiers princi-
pal sur le quel sont montés les
systèmes de fichiers auxiliaires
=⇒un seul système de fi-
chiers.

## Diapo 13 : Fichiers . & ..

Fichiers . & ..
D1
D5
F1
F1
/
F1 D1
D3
F1
F4
D2
F1
D1
F2
D1
D5
F1
F1
/
F1 D1
D3
F1
F4
D2
F1
D1
F2
Tout répertoire contient au moins 2 fichiers :
•
alias du répertoire courant.
••
alias du répertoire supérieur.
••
alias du répertoire courant pour la racine.

## Diapo 14 : Types de fichiers

Types de fichiers
répertoire
opérations disponibles : création et suppression de fichiers.
non répertoire
opérations disponibles : lecture, écriture, positionnement du pointeur
(optionnel).
régulier ils sont soit binaire, soit texte, ils ont une fin et le
positionnement est disponible.
spécial ils sont associés à des périphériques et/ou à des drivers (terminal,
disque dur, flux vidéo audio, ...).
lien
leurs contenus sont la référence d’un autre fichier.
Lire/écrire un fichier lien revient à lire/écrire le fichier
référencé.
Un lien peut référencer un autre lien.
Un lien mort : le référencé n’existe pas.

## Diapo 15 : Types de fichiers

Types de fichiers
Quelques fichiers spéciaux :
/dev/null
fichier poubelle, capacité infinie.
/dev/zero
fichier sans fin contenant que des octets nuls (0x00).
/dev/random /dev/urandom
fichier sans fin de nombres aléatoires.

## Diapo 16 : Définition

Définition
Suite de noms (nom=chaine de caractères sans ’/’) séparés par au moins
un ’/’ et précédés et terminés par ∅ou plusieurs ’/’ :
[/]nom0/nom1/.../nomn[/]
Un chemin est valide si
les sous chemins "[/].../nomi" pour i < n doivent être des
répertoires de "[/].../nomi−1.
nomn doit être un fichier ou un répertoire du répertoire
[/]nom0/nom1/.../nomn−1.
si le chemin se termine par ’/’, nomn doit être un répertoire.

## Diapo 17 : Chemins absolus

Chemins absolus
Un chemin absolu commence par un ’/’, on part de la racine du système
de fichier.
Parmi les chemins ci-dessous indiquez ceux qui sont identiques et ceux qui
n’en sont pas.
D1
D5
F1
F1
/
F1 D1
D3
F1
F4
D2
F1
D1
F2
/D1/D3/D1
/D1/D3/D1/
/D1/D3/D1/.
///D1//D3////D1
/D2/D5/D1/../../F1
/D1/D2/D5/D1/../../F1/
/../D2/D5//.//D1/../../F1
/D1/D2/D5/D1/../XX/../../F1

## Diapo 18 : Chemins relatifs

Chemins relatifs
Tout programme qui tourne a un répertoire de travail associé qui s’appelle
CWD ou WD (Current Working Directory).
Un chemin relatif ne commence pas par un ’/’, il part du répertoire CWD.
Le chemin absolu du chemin relatif CHE est : CWD/CHE
D1
D5
F1
F1
/
F1 D1
D3
F1
F4
D2
F1
D1
F2
Donnez les chemins relatifs de :
•/D2/F1 : ../F1
•/F1 : ../../F1
•/D1/D3/F1 : ../../D1/D3/F1
•/D2/D5/F1 le plus court ne
commençant pas par ’F’ : ./F1

## Diapo 19 : Quizz

Quizz
1 Il existe un chemin absolu pour chaque fichier.
2 Il existe un chemin absolu unique pour chaque fichier.
3 Soit un CWD et un fichier, il existe toujours un chemin relatif
partant de ce CWD et désignant ce fichier.
4 Les chemins "F" et "./F" donnent toujours le même fichier.
5 Les chemins "../F" et "F" donnent toujours des fichiers différents.
6 Les chemins "D1/F" et "D1/D2/../F" donnent toujours le même
fichier.
7 Les chemins "/F" et "F" donnent toujours des fichiers différents.

## Diapo 20 : Table des matières

Table des matières
1 Fondement
Organisation
Système de protection
Fichiers et systèmes de fichiers
Processus
Système Unix

## Diapo 21 : Définition

Définition
Entité d’exécution =⇒un
programme qui tourne.
Identifié par un numéro PID.
Ils sont organisés en arbre.
Ils sont groupés en session.
Ils sont groupés en groupe
terminal avec un leader.
40
41
42
60
61
70
71
1
20
21
22
23
24
30
31
32
50
51
52
80
81
82
83
90
91
Comme tout programme, il produit des sorties en fonction d’entrées.

## Diapo 22 : Entrées/sorties

Entrées/sorties
Unité d’exécution : code, données,
pile.
Identifiant de processus.
Identifiant d’utilisateur et
identifiants de groupe.
Identifiant de session et de groupe
terminal.
Arguments : tableau de chaines de
caractères.
variables d’environnement : tableau
de chaines de caractères
nom-var=valeur-var
flux d’entrée et de sortie.
statut : valeur entre 0 et 255.
Unite d’execution
CWD PID UID GID0 ...
ARG0 ARG1 ARG2 ...
ENV0 ENV1 ENV2 ...
flux0
flux1
flux2
...flux3
statut

## Diapo 23 : Convention : Arguments

Convention : Arguments
Les arguments passés à un programme sont un tableau de chaine de
caractères. La seule convention est :
Le premier argument est le nom d’invocation du programme
Le premier argument est donc souvent inutilisé mais est utile pour :
1 Écrire des messages d’erreur avec le nom du programme.
2 Retrouver le répertoire d’installation du programme.
3 Écrire un seul programme qui en fonction du nom d’invocation fait
des choses différentes (ex : busybox)
factorisation de code =⇒gain de place

## Diapo 24 : Convention : Variables d’environnement

Convention : Variables d’environnement
Le format (convention) des variables d’environnement passées à un
programme est :
nom-var=valeur-var Quelques variables d’environnement
conventionnelles :
HOME Sa valeur est le chemin absolu du répertoire de travail de
l’utilisateur UID.
TERM Sa valeur est le type de terminal (linux, xterm, vt100, ...).
LANG Sa valeur donne le langage de l’utilisateur et le charset utilisé.
PATH Sa valeur est une suite de chemins séparés par le caractère ’ :’
(ex : PATH=/bin :/usr/bin :/usr/local/bin).
Si on lance un programme par un chemin sans ’/’ (ex : gnu) alors le
système lancera le premier fichier gnu exécutable trouvé dans la
suite de répertoires du PATH (ex : soit les chemins /bin/gnu,
/usr/bin/gnu, /usr/local/bin/gnu).

## Diapo 25 : Convention : Flux d’entrée/sortie

Convention : Flux d’entrée/sortie
Un programme qui démarre dispose de 3 flux d’entrée/sortie :
Flux 0
flux standard d’entrée, ouvert en lecture, abréviation stdin (libc) :
=⇒dédié à l’acquisition de données d’entrée.
Flux 1
flux standard de sortie, ouvert en écriture, abréviation stdout (libc) :
=⇒dédié à l’écriture de données de sortie.
Flux 2
flux standard d’erreur, ouvert en écriture, abréviation stderr (libc) :
=⇒dédié à l’écriture de message d’erreur.

## Diapo 26 : Statut

Statut
La valeur renvoyée par un programme (0 ≤valeur ≤255) indique si le
programme s’est déroulé sans problème.
Elle peut être récupérée par l’entité qui a lancé le programme.
statut = 0 =⇒ok,
statut ̸= 0 =⇒erreur.

## Diapo 27 : Table des matières

Table des matières
1 Fondement
Organisation
Système de protection
Fichiers et systèmes de fichiers
Processus
Système Unix

## Diapo 28 : Fonction

Fonction
Le Shell est un programme dont les fonctions sont :
Shell interactif C’est l’interface utilisateur standard d’Unix.
Dès qu’un utilisateur ouvre un terminal, il discute avec un
Shell.
Il permet de lancer des commandes simple ou avec des
d’options.
Il permet de chainer des commandes de façon très souple.
Il permet de taper très rapidement grâce à ses mécanismes
d’expansion, la complétion et le rappel de commandes.
Shell script C’est un langage de programmation complet (variables,
alternatives, boucles) dédié à l’écriture de programme système.
Ces programmes lancent et/ou chainent d’autres programmes de
manière automatique.

## Diapo 29 : Historique

Historique
1977 sh (Bourne shell)
1978 csh (C shell)
1981 tcsh (C shell)
1983 ksh (Kom shell)
1988 bash (Bourne again shell)
1990 ash (réécriture du Bourne shell)
1990 zsh
A part csh et tcsh qui ont divergé, les autres sont compatibles avec le
Bourne shell =⇒scripts écrits il y a 40 ans fonctionnent encore.
bash et zsh sont très confortables et très semblables.

## Diapo 30 : Syntaxe

Syntaxe
En avant plan (fg)
sh> chmod 644 gnu/bee <C-R>
sh>
En arrière plan (bg)
sh> sleep 60 & <C-R>
sh>
avec
sh>
l’invite de commande (prompt) écrite par le Shell.
sleep 60 &
chmod 644 gnu/bee la commande tapée par l’utilisateur. C’est une
suite de chaines de caractères séparées par des espaces (séquence
d’au moins un blanc ou tabulation).
chmod et sleep la première chaine de caractères est le programme
à exécuter (chemin relatif ou absolu ou basename avec le
PATH).
644 et 60 1er argument, sa signification dépend de la commande.
gnu/bee 2nd argument, sa signification dépend de la commande.
&
en fin de commande indique l’arrière plan
<C-R> La touche entrée tapée par l’utilisateur. Elle indique au Shell que
la commande est complète et qu’elle doit être exécutée.

## Diapo 31 : Job contrôle

Job contrôle
<CTL-C>
demande au processus en avant plan de se terminer.
<CTL-Z>
demande au processus en arrière plan de suspendre son
exécution.
fg
• fait passer en avant plan le dernier processus lancer en
arrière plan.
• si ce processus était suspendu, il le relance.
bg
relance le processus suspendu en arrière plan

## Diapo 32 : Raccourcis

Raccourcis
Sur l’invite de commande du Shell on peut :
Avec ↑et ↓se déplacer dans l’historique des commandes déjà
exécutées.
Avec ←et →se déplacer dans la commande rappelée et la modifier.
Enfin la touche tabulation déclenche la complétion :
Sur le premier mot de la commande la complétion est faite sur le
PATH
Sur les autres mots de la commande la complétion est faite sur le
système de fichiers.
Si il y a conflit pour compléter, une deuxième tabulation affiche les
possibilités.

## Diapo 33 : Liste

Liste
Les commandes de bases en mode interactif :
cd
: change le CWD.
pwd
: affiche le CWD.
mkdir : création de répertoires
rmdir
: suppression de répertoires
ls
: affiche les informations relatives aux fichiers et répertoires
cp
: copie de fichiers et répertoires
mv
: déplacement/renommage de fichiers et répertoires
rm
: suppression de fichiers et répertoires
which : affiche le path absolu d’une commande
less
: un pageur
chmod : modification des permissions de fichiers et répertoires
wc
: compte le nombre de lettres, mots et lignes d’un fichier
find
: recherche de fichier dans une arborescence
grep
: recherche un motif dans un fichier
sort
: tri de fichier
stat
: affiche les meta-données (taille, type, . . . ) d’un fichier
tar
: archiveur
Les commandes de bases utiles pour les scripts :
echo
: affiche un message sur le flux standard de sortie
read
: lit un message sur le flux standard d’entrée
cat
: concaténation de fichiers
test
: compare des nombres, des chaines ; obtention de propriétés de fichiers
dirname
: obtention des répertoires des chemins (/aa/bb/cc/dd)
basename : obtention des noms de base des chemins (/aa/bb/cc/dd)

## Diapo 34 : Quelques exemples

Quelques exemples
D1
D5
F1
F1
/
F1 D1
D3
F1
F4
D2
F1
D1
F2
4 Détruisez l’arborescence donnée par le
répertoire D2 encadré.
sh> rm /D2
sh>
5 Pourquoi la séquence ci-dessous ne
détruit pas le fichier "-f" ?
sh> ls .
gnu -f bee
sh> rm -f
sh> ls .
gnu -f bee
sh>
Car -f est une option de rm. Comment
le détruire ?
sh> rm ./-f
sh>

## Diapo 35 : Table des matières

Table des matières
2 Shell interactif
Séquence d’instructions
Variables et environnement
Expansions
Redirections

## Diapo 36 : Séquence simple

Séquence simple
La fin d’une commande est indiquée par un <C-R>ou le caractère ’ ;’
sh> gcc 1.c
sh> ./a.out 2 + 3
5
sh>
sh> gcc 1.c ; ./a.out 2 + 3
5
sh>
Si la compilation échoue, la
commande ./a.out est lancée
Le statut renvoyé par la séquence est celui de la dernière commande

## Diapo 37 : Séquence conditionnelle

Séquence conditionnelle
Les opérateurs && (et) et || (ou) permettent de chainer 2 commandes en
fonction du statut de la première. L’opérateur ! (non) permet d’inverser la
condition logique.
sh> ./a.out -1 gnu || ./a.out -2 gnu || ./a.out -3 gnu
1 La seconde commande ./a.out n’est lancée que si la première
échoue.
2 La troisième commande ./a.out n’est lancée que si les 2 premières
échouent.
3 Le statut de la séquence est faux (̸= 0) si les 3 commandes échouent.
sh> gcc 1.c && ./a.out 2 + 3
1 La commande ./a.out n’est lancée que si la compilation a réussi.
2 Le statut de la séquence est vrai (= 0) si les 2 commandes
réussissent.

## Diapo 38 : Groupe d’instructions

Groupe d’instructions
Une séquence d’instructions peut être parenthésée soit avec des accolades
(lancée par le Shell courant), soit avec des parenthèses (lancée par un
autre processus Shell).
sh> { true && false ; } || ( false && true ) || { false && false ; }
sh> ( cd gnu/bee ; cp f1 f2 ) # CWD inchangé

## Diapo 39 : Table des matières

Table des matières
2 Shell interactif
Séquence d’instructions
Variables et environnement
Expansions
Redirections

## Diapo 40 : Variables locales

Variables locales
Une variable est identifiée par un nom, l’expression régulière
"[a-zA-Z_][a-zA-Z0-9_]*" définit les noms valides.
Affectation d’une variable
sh> CMD=cd
sh> DIR=bee
sh> SDIR=gnu
sh>
Pas d’espace autour du ’=’.
Valeur d’une variable
sh> $CMD $DIR/$SDIR
ou
sh> ${CMD} ${DIR}/${SDIR}
1 La valeur d’une variable non définie =⇒chaine vide.
2 La forme parenthésée permet la concaténation de variables :
$DIRgnu =⇒DIRgnu non définie
${DIR}gnu =⇒beegnu
3 La forme parenthésée permet de nombreuses extensions :
${DIR :-undef} =⇒undef si DIR non définie.

## Diapo 41 : Variables d’environnement

Variables d’environnement
Au démarrage le Shell crée une variable locale en la marquant pour
chaque variable d’environnement définie.
On peut marquer une variable avec la commande Shell export :
sh> NVE=gnu
sh> export NVE
sh>
sh> export NVE
sh> NVE=gnu
sh>
sh> export NVE=gnu
sh>
Toutes les variables marquées sont transmises aux processus créés.

## Diapo 42 : Variables d’environnement

Variables d’environnement
La commande "export -n NVE" démarque la variable NVE.
Les variables marquées peuvent être visualisées par la commande Shell
export sans argument ou la commande Unix env.
On ajoute une ou plusieurs variables d’environnement à une commande
par :
sh> VE1=gnu VE2=bee cmd gnat
sh>

## Diapo 43 : Table des matières

Table des matières
2 Shell interactif
Séquence d’instructions
Variables et environnement
Expansions
Redirections

## Diapo 44 : Principales expansions

Principales expansions
Le Shell a beaucoup de commandes implicites qu’il étend pour les
exécuter. Les principales sont :
$vname La valeur de la variable vname.
${vname...} Une valeur déduite de la valeur de la variable vname.
$$
Le PID du Shell.
$ ?
Le statut de la dernière commande exécutée.
!n
La ni `eme commande de l’historique.
!str
La dernière commande entrée commancant par str.
$[ expr ]
$(( expr ))
Le résultat de l’expression arithmétique expr.
‘ cmd gnu bee ... ‘
$( cmd gnu bee ... )
Le flux stdout de la commande cmd gnu bee ....

## Diapo 45 : Quelques exemples

Quelques exemples
sh> !gcc
gcc -I . -L /usr/local/lib bee.c
sh>
sh> i=10
sh> i=$[2 * $i + 1]
sh> echo $i $[$i + 1]
21 22
sh>
sh> cmd $(cat gnu/bee) $[2<<10]
sh>
sh> cmd $(ls)
sh>

## Diapo 46 : Principe

Principe
Soit une commande :
str0 $[str1 $(str2 str3) ] str4 $str5 ${str6 :-str7 str8} str9
La plupart des stri sont considérées comme des expressions régulières sur
le système de fichiers moins ceux commençant par ’.’.
1 Si un ou plusieurs fichiers sont compatibles avec cette expression
régulière, alors stri est remplacée par ces fichiers séparés par un
espace.
2 Si aucun fichier n’est compatible avec cette expression régulière,
alors stri est conservée telle quelle.
sh> ls
sh> echo ∗
∗
sh>
sh> ls
bee gnu
sh> echo ∗
bee gnu
sh>

## Diapo 47 : Format des expressions régulières

Format des expressions régulières
∼str en début de chaine (str sans /) donne le HOME de l’utilisateur str.
∼/
en début de chaine donne le HOME.
∼
en début de chaine et la chaine n’a qu’un caractère donne le HOME.
*
0 ou plusieurs caractères sauf le ’/’.
?
1 caractère sauf le ’/’.
[ m0m1m2...]
1 caractère appartenant à au moins un motif mi. mi est
soit un caractère (ex : x),
soit une séquence de 3 caractères (ex : E-K =⇒{E F G H I J K}).
{expr0,expr1,...} correspond au choix à une des expressions.
autre caractère le caractère.

## Diapo 48 : Quelques exemples

Quelques exemples
sh> ls /[a-zA-Z]*/[a-zA-Z]*/*.[024]*
sh> gcc src/*.c
sh> wc -l prj/*.[ch]
sh> wc -l prj/*{.c,.h}
sh> ls
echo main.c
sh> *
main.c
sh>

## Diapo 49 : Échappement

Échappement
\c
Le caractère c devient un caractère standard.
sh> echo \$FILES
$FILES
sh>
sh> ls gnu\ bee
ls : gnu bee : No such file or directory
sh>
"str" Dans la chaine de caractères str :
les espaces sont échappés,
les expansions de fichiers sont échappées,
les expansions de commandes sont toujours actives.
sh> ls "gnu bee"
ls : gnu bee : No such
file or directory
sh>
sh> FILES="/*"
sh> echo "$FILES ${FILES} $[1+1]"
/* /* 2
sh>
’str’
Dans la chaine de caractères str tout est échappé.

## Diapo 50 : Table des matières

Table des matières
2 Shell interactif
Séquence d’instructions
Variables et environnement
Expansions
Redirections

## Diapo 51 : Principe

Principe
sh> cmd1 gnu bee < f1 cmd2 gnat > f2
Unite d’execution de cmd2
CWD UID GID0 ...
PID
cmd2 gnat
HOME=... PATH=... TERM=...
stdout
stdin
stderr
statut
Unite d’execution de cmd1
CWD UID GID0 ...
PID
cmd1 gnu bee
HOME=... PATH=... TERM=...
stderr
stdout
stdin
statut
Unite d’execution de bash
CWD UID GID0 ...
PID
/bin/bash −i
HOME=... PATH=... TERM=...
stderr
stdout
stdin
statut
tty
tty
pipe
tty
F
2
1
F

## Diapo 52 : Syntaxe

Syntaxe
cmd ... >> path redirige le flux stdout vers le fichier path.
Si le fichier path existe, les écritures se feront à la fin du fichier.
Si le fichier path n’existe pas, il est créé.
Il doit être accessible en écriture.
cmd ... 2>> path similaire à ">> path" mais pour le le flux stderr.
cmd ... 1>&2
redirige le flux stdout sur stderr.
cmd ... 2>&1
redirige le flux stderr sur stdout.
cmd ... <<EOF
hello
word
EOF
initialise le flux stdin à hello word.
Les redirections peuvent être écrites n’importe où sur la ligne de
commande.

## Diapo 53 : Table des matières

Table des matières
3 Shell script
Mon premier script
Instructions de contrôle
Quelques indispensables
Création de Builtin commande
Exemple complet
Astuces et pièges

## Diapo 54 : Exécution

Exécution
Fichier hello.sh :
1
#!/ bin / bash
2
3
echo −n "␣␣" Hello
4
echo World
Pour lancer le script :
Méthode 1
sh> bash hello.sh
Méthode 2 rendre le script exécutable
(chmod) puis sh> ./hello.sh
# ! doivent être les 2 premiers caractères
du script.
Le système lance la commande suivant
"# !" en ajoutant l’argument
"./hello.sh" à la fin.
Méthode 3
sh> bash < hello.sh
Les méthodes 1 et 2 sont équivalentes.
Dans la méthode 3 le flux stdin du Shell est le script lui même.
1
echo −n "␣␣" Hello
2
cat
3
echo World
=⇒Hello echo World

## Diapo 55 : "Sourcer" un script

"Sourcer" un script
Dans un Shell qui tourne, on peut à tout moment "sourcer" un script par
la commande "." ou "source"
sh> source hello.sh ou sh> . hello.sh
Dans ce cas, le Shell interrompt sa lecture du flux stdin, il lit le script à la
place. A la fin du script, il se remet à lire le flux stdin.
Dans ce cas, le script peut modifier les données du Shell (ex : PATH).
Les scripts Shell de démarrage (.bashrc, .profile, ...) sont sourcés. Ceci
permet de configurer son Shell.

## Diapo 56 : Table des matières

Table des matières
3 Shell script
Mon premier script
Instructions de contrôle
Quelques indispensables
Création de Builtin commande
Exemple complet
Astuces et pièges

## Diapo 57 : Alternative : Syntaxe

Alternative : Syntaxe
1
i f
cmd−cond
2
then
3
. . .
4
else
5
. . .
6
f i
1
i f
cmd−cond
; then
2
. . .
3
else
4
. . .
5
f i
La clause then est obligatoire.
La clause else est facultative.
Le ; est quasi obligatoire sinon then est compris comme un argument
de cmd-cond.
cmd-cond est soit une commande simple ou un groupe de
commandes.
Si la commande cmd-cond renvoie le statut 0 (ok) la clause then est
exécutée.
Si la commande cmd-cond renvoie un statut ̸= 0 (pas ok) la clause
else est e éc tée

## Diapo 58 : Alternative : Exemple 1

Alternative : Exemple 1
Tester si le fichier file contient main.
1
i f
grep −q main
f i l e
;
then
2
echo main dans
f i l e
3
else
4
echo pas de main dans
f i l e
5
f i
Si file n’existe pas grep écrit un message d’erreur d’où :
1
i f
grep −q main
f i l e
2> /dev/ n u l l
;
then
2
echo main dans
f i l e
3
else
4
echo pas de main dans
f i l e
5
echo ou
f i l e
non
a c c e s s i b l e
6
f i

## Diapo 59 : Alternative : Exemple 2

Alternative : Exemple 2
Écrire oui si on est en 2016 ?
1
i f
date
|
grep −q 2016
;
then
2
echo oui
3
f i
Enfin pour les petites demi alternatives, on peut aussi utiliser les
opérateurs logiques && et ||.
1
date
|
grep −q 2016 && echo oui

## Diapo 60 : Cas : Cas d’une constante

Cas : Cas d’une constante
1
case mot in
2
motif
)
3
. . .
4
; ;
5
motif
|
motif
)
6
. . .
7
; ;
8
. . .
9
esac
Format des motifs :
∼str
en début de chaine (str sans /) donne le
HOME de l’utilisateur str.
∼/
en début de chaine donne le HOME.
∼
en début de chaine et la chaine n’a qu’un
caractère donne le HOME.
*
0 ou plusieurs caractères.
?
1 caractère.
[ m0m1m2...] 1 caractère défini par les mi. mi
est soit un caractère (ex : x), soit une
séquence de caractères (ex : E-K =⇒{E
F G H I J K}).
La première clause dont un motif décrit le mot est exécutée puis le
contrôle reprend après le cas.
Si aucun motif ne décrit le mot, aucune clause n’est exécutée et le
contrôle reprend après le cas.

## Diapo 61 : Cas : Choix conditionnel d’expressions booléennes

Cas : Choix conditionnel d’expressions booléennes
1
i f
cmd−cond1
; then
2
. . .
3
e l i f
cmd−cond2
; then
4
. . .
5
e l i f
cmd−cond3
; then
6
. . .
7
else
8
. . .
9
f i
elif permet d’emboiter des si sans
multiplier les fin de si.
Choisit la première clause dont la
condition cmd-condi est vraie.
Si aucune cmd-condi n’est vraie
exécute la clause else.
La clause else est facultative.

## Diapo 62 : Cas : Exemple

Cas : Exemple
Tester si la variable n est un nombre de 1 à 3 chiffres.
1
case $n in
2
[0 −9])
good=1 ; ;
3
[0 −9][0 −9]) good=1 ; ;
4
[0 −9][0 −9][0 −9])
5
good=1 ; ;
6
∗)
good=0 ; ;
7
esac
1
case $n in
2
[0 −9]
|
[0 −9][0 −9]
|
3
[0 −9][0 −9][0 −9]
)
4
good=1 ; ;
5
∗)
good=0 ; ;
6
esac
Déterminer si le mot donné par la variable m commence, se termine ou
contient la chaine de caractères ia.
1
debut =0;
f i n =0; mil =0;
2
case $m in
3
i a ∗)
debut=1 ; ;
4
∗i a
)
f i n =1 ; ;
5
∗i a ∗)
mil=1 ; ;
6
esac

## Diapo 63 : While : Syntaxe

While : Syntaxe
1
while cmd−cond
2
do
3
. . .
4
. . .
5
done
1
while cmd−cond
; do
2
. . .
3
. . .
4
done
Le ; est quasi obligatoire sinon “do” est compris comme un argument
de cmd-cond.
Si la commande cmd-cond renvoie le statut 0 (ok), le corps de la
boucle est exécuté puis le contrôle reprend en début de boucle.
Si la commande cmd-cond renvoie un statut ̸= 0 (pas ok), le
contrôle reprend après la boucle.
Dans le corps de boucle les commandes suivantes sont disponibles :
continue branche en début de boucle,
break donne le contrôle après la boucle.

## Diapo 64 : While : Exemple

While : Exemple
La commande ping −c 1 host permet de tester si la machine host est
accessible via le réseau.
Écrire un script qui toutes les 30 secondes émet 5 beeps si la machine
192.168.1.10 n’est pas accessible.
1
while
true
; do
2
s l e e p
30
3
ping −c 1 192.168.1.10 > /dev/ n u l l 2>&1 | |
\
4
echo −e −n "\b\b\b\b\b"
5
done

## Page 100 : For Syntaxe

For Syntaxe
1
for v in
str −l i s t
2
do
3
. . .
4
. . .
5
done
1
for v in
str −l i s t
; do
2
. . .
3
. . .
4
done
Le ; est quasi obligatoire sinon “do” est compris comme un élément de
str-list.
str-list est un suite de chaines de caractères stri séparées par un ou
plusieurs espaces.
La variable v prend dans l’ordre de str-list toutes les valeurs stri et le corps
de boucle est exécuté pour chaque valeur.
Dans le corps de boucle les commandes suivantes sont disponibles :
continue branche en début de boucle,
break donne le contrôle après la boucle.
La chaine de caractères {n..m} est expansée en tous les nombres de n à m
compris.
La chaine de caractères {n..m..i} est expansée en tous les nombres de n 65 / 97

## Diapo 66 : For Exemple

For Exemple
Écrire Hello World lettre par lettre.
1
for
l
in H e
l
l
o "␣" W o r
l
d ; do
2
echo −n $ l
3
done
4
echo
Écrire 10 lignes contenant chacune 5 fois silence.
1
for
i
in
{1..10}
; do
2
for
j
in
{19..15}
; do
3
echo −n
s i l e n c e
"␣"
4
done
5
echo
6
done

## Diapo 67 : For Exemple

For Exemple
Ecrire sur une ligne les noms de base des fichiers du répertoire donné par
la variable dir, suffixés entre parenthèses de leur nombre de lignes.
1
for bn in $ ( cd
$ d i r
;
l s )
; do
2
echo −n "$bn ( " $ (wc −l < $ d i r /$bn 2> /dev/ n u l l ) "
3
done
4
echo

## Diapo 68 : Table des matières

Table des matières
3 Shell script
Mon premier script
Instructions de contrôle
Quelques indispensables
Création de Builtin commande
Exemple complet
Astuces et pièges

## Diapo 69 : Commande test

Commande test
Pour pouvoir écrire des programmes, il faut pouvoir faire des tests :
le fichier str existe-t-il ?
le fichier str est il un répertoire ?
les chaines str1 et str2 sont-elles égales ?
n1 est-il inférieur à n2 ?
. . .
la commande test répond à ce besoin.
Les arguments de la commande définissent une expression booléenne, elle
renvoie un statut de 0 si l’expression est vraie et de 1 sinon.
sh> test arg0 arg1 arg2 ...
ou
sh> [ arg0 arg1 arg2 ... ]

## Diapo 70 : test : Arguments

test : Arguments
-e file le fichier file existe et n’est pas un lien mort.
-f file le fichier file∗∗existe et est régulier.
-d file le fichier file∗∗existe et est un répertoire.
-h file le fichier file existe et est lien symbolique.
-r file le fichier file∗∗existe et peut être lu (file peut être un répertoire).
-w file le fichier file∗∗existe et peut être écrit (file peut être un
répertoire).
-x file le fichier file∗∗existe et peut être exécuté (file peut être un
répertoire).
! expr
opérateur booléen non.
expr -o expr opérateur booléen ou.
expr -a expr opérateur booléen et.
\( et \) permet de parenthèser une expression booléenne.
Note ∗∗: ou dans le cas d’un lien, le fichier pointé final.

## Diapo 71 : test : Exemples

test : Exemples
Déterminez si les chaines de caractères définies par les variables S1 et S2
sont égales :
1
i f
test $S1 = $S2
;
then
2
echo S1 = S2
3
f i
1
i f
[
$S1 = $S2
]
;
then
2
echo S1 = S2
3
f i
Sauf si S1 est la chaine vide ou S1 n’est pas définie, l’expansion sera :
1
i f
test = $S2
;
then
2
echo S1 = S2
3
f i
1
i f
[
= $S2
]
;
then
2
echo S1 = S2
3
f i
Dans ce cas, la commande test écrit un message d’erreur "mauvais
formatage de l’expression" et renvoie un statut différent de ∅. D’où la
correction :
1
i f
test "$S1" = "$S2"
;
then
2
echo S1 = S2
3
f i
1
i f
[
"$S1" = "$S2"
]
;
the
2
echo S1 = S2
3
f i

## Diapo 72 : test : Exemples

test : Exemples
Déterminez si l’entier défini par la variable n est compris entre 10 et 23
compris :
1
i f
test 10 −l e
$n −a $n −l e
23
;
then
2
echo oui
3
f i
ou
1
i f
[
10 −l e
$n −a $n −l e
23
]
;
then
2
echo oui
3
f i

## Diapo 73 : test : Exemples

test : Exemples
Écrire ok si le fichier $f est régulier et accessible en lecture, écriture et
non accessible en exécution :
1
i f
test −f
$f −a −r
$f −a −w $f −a ! −x $f
;
then
2
echo ok
3
f i
ou
1
i f
test −f
$f && [ −r
$f −a −w $f −a ! −x $f
]
;
2
echo ok
3
f i

## Diapo 74 : Autres commandes

Autres commandes
read v1 v2 v3
Lit une ligne dans le flux stdin et affecte le contenu la ligne dans les
variables vi. La ligne est considérée comme n chaines de caractères
séparées par un ou plusieurs espaces. Soit s1 s2 s3 le contenu d’une ligne
inst.
var.
v1
v2
v3
v4
read
read v1
s1 s2 s3
read v1 v2
s1
s2 s3
read v1 v2 v3
s1
s2
s3
read v1 v2 v3 v4
s1
s2
s3
""

## Diapo 75 : Arguments : Expansions dédiées

Arguments : Expansions dédiées
Quand on lance un script Shell, comment récupère-t-on ses arguments
sh> ./script petit lapin
sh>
1 #!/ bin / bash
2
3
t a i l l e=... # arg1
( p e t i t )
4
animal=... # arg2
( l a p i n )
Dans un script, on a les expansions suivantes :
$#
le nombre d’arguments.
$0
le nom de la commande.
$1
le premier argument.
$2
le second argument.
$i
le ii `eme argument.
$*
la liste des arguments séparés par un espace.
"$@" la liste des arguments quotés séparés par un espace.

## Diapo 76 : Arguments : Exemples

Arguments : Exemples
Écrire le script "somme n1 n2 ..." qui écrit sur le flux stdout la ligne "n1
+ n2 ... + n".
1 #!/ bin / bash
2 #
3 # usage : somme n1 n2 n3
. . .
4
5
f i r s t =1
6
for n in $∗
; do
7
test
$ f i r s t = 0 && echo −n +
8
echo −n $n
9
f i r s t =0;
10
done

## Diapo 77 : Arguments : Commandes dédiées I

Arguments : Commandes dédiées I
shift Équivalent à "shift 1".
shift n Décale les arguments de n positions vers la gauche.
La table ci-dessous donne les valeurs des $1, $2, $3, $4, $5 en
supposant que leurs valeurs initiales sont vi et que les valeurs des
variables 6,7, . . . sont la chaine de caractères vide.
inst.
var.
$1
$2
$3
$4
$5
echo $*
v1
v2
v3
v4
v5
shift 1
v2
v3
v4
v5
shift 2
v3
v4
v5
shift 3
v4
v5
shift 4
v5
shift 5
shift 6
...

## Diapo 78 : Arguments : Commandes dédiées II

Arguments : Commandes dédiées II
set -- str1 str2 . . . strn
Affecte stri à $i pour i inférieur ou égal à n.
Affecte la chaine vide à $i pour i supérieur n.

## Diapo 79 : Arguments : Exemples

Arguments : Exemples
Écrire le script "punition n word1 word2 ..." qui écrit sur le flux stdout
n lignes contenant les mots wordi :
1 #!/ bin / bash
2 #
3 # usage :
p u n i t i o n n word1 word2
. . .
4
5
n=$1
6
s h i f t
7
i =0
8
while
test
$ i −l t
$n
; do
9
echo $∗
10
i=$ [ i +1]
11
done

## Diapo 80 : Table des matières

Table des matières
3 Shell script
Mon premier script
Instructions de contrôle
Quelques indispensables
Création de Builtin commande
Exemple complet
Astuces et pièges

## Diapo 81 : Création de Builtin commande

Création de Builtin commande
1
#!/ bin / bash
2
. . .
3
# usage :
plus
n1 n2 n3
. . .
4
function
plus (
)
5
{
6
s=0
7
while
test $# −gt 0
;
do
8
s=$ [ $s+$1 ]
9
s h i f t
10
done
11
echo $s
12
}
13
. . .
14
plus
10 12 100
15
. . .
16
s=$ ( plus 1 2 3)
17
. . .
La commande exit peut être appelée
dans une builtin commande, elle
termine le script.
La commande return ne peut pas être
appelée en dehors d’une builtin
commande.

## Diapo 82 : Table des matières

Table des matières
3 Shell script
Mon premier script
Instructions de contrôle
Quelques indispensables
Création de Builtin commande
Exemple complet
Astuces et pièges

## Diapo 83 : Introduction

Introduction
Pour illustrer ce chapitre, nous allons prendre l’exemple :
Réalisez le script "ecp f1 f2" qui copie le fichier régulier f1 dans f2. Le
chemin de f2 est éventuellement créé.

## Diapo 84 : Entête

Entête
1
#!/ bin / bash
2
#
3
# usage :
ecp
f1
f2
4
# f o n c t i o n :
copie
f1
dans
f2
5
#
en
creant
l e
chemin
f2
ligne 1 automatique
écrivez l’usage et la
fonction (fixe les idées et
aide la reprise)

## Diapo 85 : Ligne de commande

Ligne de commande
6
# anal yse
des
args
7
i f
test $# != 2
;
then
8
echo "$0 : ␣. . . " 1>&2
9
exit 1
10
f i
11
s r c="$1"
12
des="$2"
vérifiez le nombre
d’arguments
message d’erreur clair +
sur stderr + exit avec
statut d’erreur
donnez des noms
significatifs aux arguments

## Diapo 86 : Test des arguments

Test des arguments
13
# t e s t
de
s r c
14
i f
!
test −f " $src "
15
−a −r " $src " then
16
echo "$0 : ␣. . . " 1>&2
17
exit 1
18
f i
vérifie si src est lisible et
n’est pas un répertoire.
message d’erreur clair +
sur stderr + exit avec
statut d’erreur

## Diapo 87 : Test des arguments

Test des arguments
19
# t e s t
de
dest
20
i f
test −d " $des "
;
then
21
des=" $des /$ ( basename␣" $src " ) "
22
f i
23
i f
test −h " $des " &&
24
! rm " $des " 2> /dev/ n u l l
then
25
echo "$0 : ␣. . . " 1>&2 ;
exit 1
26
f i
27
i f
test −d " $des "
;
then
28
echo "$0 : ␣. . . " 1>&2 ;
exit 1
29
f i
calcule le vrai nom
du fichier
destination
vérifie que ce n’est
pas un répertoire
le supprime si c’est
un lien

## Diapo 88 : Corps

Corps
30
# des
e s t
r e g u l i e r
ou n ’ e x i s t e
pas
31
i f
!
mkdir −p "$ ( dirname ␣" $des " ) " \
32
2> /dev/ n u l l
;
then
33
echo "$0 : ␣. . . " 1>&2 ;
exit 1
34
f i
35
i f
!
cp " $src " " $des " \
36
2>/dev/ n u l l
then
37
echo "$0 : ␣. . . " 1>&2 ;
exit 1
38
f i
39
exit 0
création des
répertoires du
fichier destination
besoin (mkdir -p)
laisse cp faire le j
le exit 0 final n’e
pas nécessaire.

## Diapo 89 : Debug

Debug
Pour déboguer, le bash propose l’option -x qui affiche toutes les
commandes expansées.
Pour l’activer, il faut soit lancer le script cmd par
bash -x cmd arg1 arg2 . . .
ou ajouter -x à la premiere ligne du script :
# !bash -x
ou insérer un "set -x" dans le script, le mode debug sera actif après cette
commande.
Le mode debug peut être désactivé par l’exécution de la commande "set
+x".
Pour déboguer, le bash propose aussi l’option -v qui affiche toutes les
commandes mais sans les expanser. Elle s’active de la même manière et
les 2 options sont cumulables (set -xv).

## Diapo 90 : Table des matières

Table des matières
3 Shell script
Mon premier script
Instructions de contrôle
Quelques indispensables
Création de Builtin commande
Exemple complet
Astuces et pièges

## Diapo 91 : Césure de lignes

Césure de lignes
Soit la ligne : if test −f f && rm f 2>/dev/null ; then
1
i f
test −f
f && rm f
2
2>/dev/ n u l l
;
then
1
i f
test −f
f && rm f \
2
2>/dev/ n u l l
;
then
1
i f
test −f
f && rm
2
f 2>/dev/ n u l l
;
then
1
i f
test −f
f && rm \
2
f 2>/dev/ n u l l
;
then
1
i f
test −f
f &&
2
rm f 2>/dev/ n u l l
;
then
OK
=⇒En cas de doute, un \ ne peut pas faire de mal.

## Diapo 92 : Espace dans les Expansions

Espace dans les Expansions
1
x=8
2
i f
test
$x
;
then
VRAI : x est non vide
1
x="a␣b"
2
i f
test
$x
;
then
FAUX : x est vide + message
d’erreur de test
1
x="1␣=␣0"
2
i f
test
$x
;
then
FAUX : x est vide
=⇒double quoter $x ("$x")

## Diapo 93 : Espace dans les Expansions

Espace dans les Expansions
1
p="bee␣old / gnat ␣gnu"
2
i f
test −d $ ( dirname "$p" )
ERREUR : test -d bee old =⇒
message d’erreur
1
p="bee␣old / gnat ␣gnu"
2
i f
test −d "$ ( dirname ␣"$p" ) " OK : test -d "bee old"
=⇒double quoter $(cmd) ("$(cmd)")

## Diapo 94 : Espace dans les Expansions

Espace dans les Expansions
1
set -- "a" "b" "c"
2
for
s
in $∗
; do
OK : 3 fois la boucle
1
set -- "a" "b" "c␣d"
2
for
s
in $∗
; do
ERREUR : 4 fois la boucle
1
set -- "a" "b" "c␣d"
2
for
s
in "$∗"
; do
ERREUR : 1 fois la boucle
1
set -- "a" "b" "c␣d"
2
for
s
in "$@"
; do
OK : 3 fois la boucle
=⇒ne pas utiler $* mais ("$@")

## Diapo 95 : Variable dans un sous processus

Variable dans un sous processus
1
( cd
. .
;
test "$x"
| |
\
2
x=$ ( echo ∗)
)
ARGH ! : x est inchangée
1
test "$x"
| |
\
2
x=$ ( cd
. .
;
echo ∗)
OK : si x était vide, x est
initialisée aux fichiers de ..
1
cd
. .
;
test "$x"
| |
\
2
x=$ ( echo ∗)
;
cd −
OK : mais plus lourd

## Diapo 96 : Variable dans un sous processus

Variable dans un sous processus
1
read x y < f
OK : lit la 1i `ere ligne de f dans x
et y
1
cat
f
|
read x y
ARGH ! read est fait dans un sous
processus
Les principaux éléments de syntaxe qui créent des sous processus sont :
( ), $( ) et

## Diapo 97 : Utilisation de read

Utilisation de read
1
x=0 ;
a=0
2
sed /^#/d f1
|
\
3
while read a b
; do
4
x=$a
5
. . .
6
done
OK : lit tout le fichier f1 ligne par
ligne en sautant les lignes
commençant par #. Que valent
ces variables après le done ?
x
0.
a
0.
