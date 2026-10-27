# Annales OSSE / Shell et système d'exploitation (corrigés)

Les sujets (PDF scannés) sont dans `sujets/`. Ils sont ignorés par git (`*.pdf` dans `.gitignore`).
Tout le code C compile sans avertissement avec `gcc -Wall -Wextra`, et a été testé sous Linux (Ubuntu 22.04, glibc 2.35).

| Dossier | Examen | Sujet | Thèmes |
|---|---|---|---|
| `2022/` | 2021-2022 | `partieloss2022_sujet.pdf` | head/tail, export, grep/cut, sed avec capture, script bash, 3 fils synchronisés, `tp-wc-parallele.c` |
| `2023/` | 16 janvier 2023 | `OSSE_sujet_2022-2023.pdf` (+ `OSSE_notes_scarylight.pdf`, page 4 annotée) | commandes, wrapper `scp` à corriger, processus abc, pipe, programme foo (fork + pipe) |
| `2024/` | 22 janvier 2024 | `Partiel_OSSE_2023-2024.pdf` | find, wrapper `ocamlrun` à corriger, numbering (flux libc), `cat \| while read`, SIGPIPE |
| `2025/` | 6 janvier 2025 | `OSSE11-Sujet.pdf` | commandes à corriger, `find -name`, check_parentheses (flux libc), tester (fork/exec/dup2/alarm/SIGCHLD) |
| `2026/` | 19 janvier 2026 | `partiel_osse_2026.pdf` | script à corriger, `sed ... > même fichier` (troncature), 1234 (fork, pipe, SIGUSR1) |

Les exercices identiques au TP3 renvoient à son corrigé : `numbering` (2024, ex 3) et `check_parentheses` (2025, ex 3).

Révisions shell :
- `cheatsheet_commandes.md` : une section par commande (arborescence de la syntaxe, options, exemples expliqués), les structures de script sh et une checklist « script à corriger ».
- `entrainement_shell.md` : exercices type partiel avec corrigés repliés (commandes, commandes erronées, sed à expliquer, scripts à corriger et à écrire).

## Les pièges qui reviennent chaque année
- **Scripts à corriger** : `[ $# < 2 ]` (c'est une redirection, il faut `-lt`) ; `echo 2>&1` (il faut `>&2`) ; `$var = val` (il faut `var=val`, sans espaces) ; `[ fonction args ]` (il faut `if fonction args`) ; fonction de test au **statut inversé** (0 = vrai) ; `$@` ou `$$` au lieu de `"$@"` ; `exit` oublié après un message d'erreur ; `==` dans `[ ]` ; guillemets oubliés.
- **Processus** : sans synchronisation, **tous** les ordres d'écriture compatibles avec les `fork`/`wait` sont possibles. On énumère les contraintes, puis les ordres qui les respectent.
- **Pipes** : on crée le pipe **avant** le `fork`. Chacun ferme les bouts qu'il n'utilise pas : `read` ne renvoie 0 (EOF) que si **tous** les écrivains ont fermé, et `write` sans aucun lecteur provoque **SIGPIPE**.
- **Signaux** : un handler doit être installé **avant** que le signal puisse arriver (donc avant le `fork`). SIGUSR1 tue par défaut, et SIGCHLD est ignoré par défaut, donc il ne réveille pas `pause()`. `pause()` est sujet à une course si le signal arrive avant : on préfère `waitpid`, ou `sigsuspend`.
- **Redirections** : le shell ouvre `> f` (et tronque `f`) **avant** de lancer la commande. `cmd f > f` vide donc `f`.
