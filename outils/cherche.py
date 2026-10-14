#!/usr/bin/env python3
"""Recherche sémantique locale dans les TP, les corrigés et les PDF du cours.

usage :
    python3 cherche.py "comment rediriger stdout vers un fichier"
    python3 cherche.py --type code "sauvegarder un descripteur"
    python3 cherche.py --type cours "que fait dup2"
    python3 cherche.py --extraits -k 8 "lire caractère par caractère"
    python3 cherche.py                 # mode interactif (plusieurs questions)

Par défaut : une recherche hybride (mots-clés BM25 + embedding) retrouve
les extraits les plus pertinents, puis un
petit LLM local (Qwen3.5 2B quantifié, llama.cpp) rédige une réponse à
partir de ces extraits seulement. --extraits : affiche les extraits, sans LLM.

Tout est automatique :
  - au 1er lancement, crée .venv/ à côté du script et y installe fastembed ;
  - le modèle d'embedding (ONNX, CPU) et le LLM (GGUF) sont téléchargés
    au 1er usage ;
  - les PDF sont d'abord convertis en Markdown dans pdf_md/ (une section
    "## Diapo N" ou "## Page N" par page), c'est ce Markdown qui est indexé ;
  - embeddings et index plein texte (FTS5) sont stockés dans une base
    SQLite locale (.cache/index.db) ;
    à chaque lancement, seuls les fichiers nouveaux ou modifiés sont recalculés.

Léger pour la machine : modèles quantifiés (~130 Mo + ~1,3 Go), 2 threads
au plus, priorité minimale (nice 19).
"""

import os
import sys
import subprocess

ICI = os.path.dirname(os.path.abspath(__file__))
VENV = os.path.join(ICI, ".venv")
VENV_PY = os.path.join(VENV, "bin", "python")


# --- Bootstrap : relance le script dans .venv (créé si besoin) -------------

def bootstrap():
    """Garantit que fastembed est importable : sinon se relance dans .venv,
    en le créant et en y installant fastembed si besoin."""
    dans_venv = os.path.realpath(sys.prefix) == os.path.realpath(VENV)
    if not dans_venv:
        if not os.path.exists(VENV_PY):
            print("[cherche] 1er lancement : création de .venv ...", file=sys.stderr)
            subprocess.check_call([sys.executable, "-m", "venv", VENV])
        os.execv(VENV_PY, [VENV_PY, os.path.abspath(__file__)] + sys.argv[1:])
    try:
        import fastembed  # noqa: F401
    except ImportError:
        print("[cherche] installation de fastembed ...", file=sys.stderr)
        subprocess.check_call([VENV_PY, "-m", "pip", "install", "-q", "fastembed"])
    try:
        import llama_cpp  # noqa: F401
    except ImportError:
        # Paquet précompilé uniquement : pas de compilation (CPU épargné)
        print("[cherche] installation de llama-cpp-python ...", file=sys.stderr)
        subprocess.check_call([VENV_PY, "-m", "pip", "install", "-q",
                               "--only-binary=:all:", "llama-cpp-python",
                               "--extra-index-url",
                               "https://abetlen.github.io/llama-cpp-python/whl/cpu"])


bootstrap()

# Priorité minimale : les autres programmes passent avant
os.nice(19)

import argparse  # noqa: E402
import re  # noqa: E402
import shutil  # noqa: E402
import sqlite3  # noqa: E402

import numpy as np  # noqa: E402

os.environ.setdefault("HF_HUB_DISABLE_PROGRESS_BARS", "1")
from fastembed import TextEmbedding  # noqa: E402

# --- Réglages ---------------------------------------------------------------

# Modèle quantifié : petit, contexte long, préfixes de tâche (voir PREFIXES)
MODELE = "nomic-ai/nomic-embed-text-v1.5-Q"
PREFIXES = {"nomic-ai/nomic-embed-text-v1.5-Q": ("search_document: ", "search_query: "),
            "nomic-ai/nomic-embed-text-v1.5": ("search_document: ", "search_query: ")}
RACINE = os.path.dirname(os.path.dirname(ICI))   # SYEX/ (outils/ est dans corrections/)
CACHE = os.path.join(ICI, ".cache")
BASE = os.path.join(CACHE, "index.db")
PDF_MD = os.path.join(ICI, "pdf_md")             # PDF convertis en Markdown
PDF_MD_VERSION = "3"     # à changer si on modifie la conversion : réindexe les PDF
os.environ.setdefault("FASTEMBED_CACHE_PATH", os.path.join(CACHE, "modeles"))

# Fichiers indexés
EXT_CODE = {".c", ".h", ".sh", ".py"}
EXT_TEXTE = {".md", ".txt"}
DOSSIERS_IGNORES = {".git", ".venv", ".cache", "__pycache__", "node_modules"}
TAILLE_MAX = 512 * 1024                          # ignore les gros fichiers

# Découpage du code et du texte : fenêtres de N lignes qui se chevauchent
FENETRE = 30
PAS = 20

# LLM qui rédige la réponse : --llm pour choisir (dépôt, fichier GGUF)
LLMS = {
    # Liquid AI, janv. 2026 : 2x plus rapide, mais répond moins bien (0,73 Go)
    "lfm":  ("LiquidAI/LFM2.5-1.2B-Instruct-GGUF", "LFM2.5-1.2B-Instruct-Q4_K_M.gguf"),
    # Qwen, févr. 2026 : défaut, bon compromis (1,28 Go, ~1 min par réponse)
    "qwen": ("unsloth/Qwen3.5-2B-GGUF", "Qwen3.5-2B-Q4_K_M.gguf"),
    # Qwen 4B : nettement meilleur, mais lent sur 2 cœurs (2,74 Go)
    "qwen4b": ("unsloth/Qwen3.5-4B-GGUF", "Qwen3.5-4B-Q4_K_M.gguf"),
}
LLM_DEFAUT = "qwen"
CONTEXTE = 4096          # tokens (consigne + extraits + réponse)
EXTRAIT_MAX = 1200       # caractères par extrait donné au LLM
REPONSE_MAX = 500        # tokens de réponse au plus

CONSIGNE = """Tu aides un étudiant sur ses TP de programmation système \
Unix (shell et C). On te donne des EXTRAITS numérotés (code, corrigés, \
cours), puis une QUESTION.

Règles :
1. Utilise UNIQUEMENT les extraits. N'invente ni fonction, ni option, ni \
numéro de descripteur.
2. Commence par la réponse directe, en une phrase.
3. Ensuite, si un extrait contient du code utile, recopie TELLES QUELLES \
les quelques lignes utiles (10 lignes au plus) dans un bloc de code. Ne \
recopie jamais un extrait entier.
4. Puis explique en 2 à 4 puces courtes.
5. Termine par une ligne « Source : [n] » où n est le numéro de l'extrait \
utilisé (pas de lien, pas de chemin).
6. Si au moins un extrait parle du sujet de la question, même \
partiellement, réponds avec ce qu'il contient. Seulement si AUCUN extrait \
n'a de rapport avec la question, écris : « Les extraits ne contiennent \
pas la réponse. »

Rappel : le descripteur 0 est l'entrée standard (stdin), 1 la sortie \
standard (stdout), 2 la sortie d'erreur (stderr).
Réponds en français."""

# Recherche hybride : on prend les CANDIDATS meilleurs de chaque méthode,
# puis fusion des rangs (Reciprocal Rank Fusion) : score = somme 1/(RRF_K + rang)
CANDIDATS = 50
RRF_K = 10               # petit : les premiers rangs comptent beaucoup
POIDS_BM25 = 3.0         # l'embedding est faible en français, BM25 domine
MOTS_VIDES = set("""
a à au aux avec ce ces cet cette comment dans de des du elle en est et
faire fait faut il ils je la le les leur lui ma mais me même mes mon ne
on ou où par pas pour pourquoi qu quand que quel quelle quels qui quoi sa
se ses son sont sur ta te tes ton tu un une vos votre vous y sert servir
peut peux dois doit avoir être été fois ça cela est-ce
the a an of to in is how what why do does
""".split())

# Ménage le CPU et la RAM : 2 threads, petits lots, textes tronqués
THREADS = 2
LOT = 8
MAX_CARACTERES = 2000


# --- Lecture des fichiers ---------------------------------------------------

def est_script(chemin):
    """Fichier sans extension commençant par #! (scripts shell du TP2...)."""
    try:
        with open(chemin, "rb") as f:
            return f.read(2) == b"#!"
    except OSError:
        return False


def lister_fichiers(racine):
    """Renvoie {chemin relatif: (chemin absolu, genre)}."""
    res = {}
    for dossier, sous, fichiers in os.walk(racine):
        # N'indexe pas l'outil lui-même
        if os.path.realpath(dossier) == os.path.realpath(ICI):
            sous[:] = []
            continue
        sous[:] = sorted(d for d in sous
                         if d not in DOSSIERS_IGNORES and not d.startswith("."))
        for nom in sorted(fichiers):
            chemin = os.path.join(dossier, nom)
            ext = os.path.splitext(nom)[1].lower()
            if ext == ".pdf":
                genre = "cours"
            elif os.path.getsize(chemin) > TAILLE_MAX:
                continue
            elif ext in EXT_CODE or (ext == "" and est_script(chemin)):
                genre = "code"
            elif ext in EXT_TEXTE:
                genre = "texte"
            else:
                continue
            res[os.path.relpath(chemin, racine)] = (chemin, genre)
    return res


def morceaux_texte(lignes):
    """Découpe une liste de lignes en fenêtres (debut, fin, texte), 1-based."""
    res = []
    for debut in range(0, len(lignes), PAS):
        bloc = lignes[debut:debut + FENETRE]
        if "".join(bloc).strip():
            res.append((debut + 1, debut + len(bloc), "".join(bloc)))
        if debut + FENETRE >= len(lignes):
            break
    return res


def chemin_md(pdf_rel):
    """outils/pdf_md/<chemin du PDF relatif à SYEX>.md"""
    return os.path.join(PDF_MD, os.path.splitext(pdf_rel)[0] + ".md")


def pdf_vers_md(pdf, md):
    """Convertit le PDF en Markdown : une section par page (ou par diapo).
    Les étapes d'animation (page contenue dans la suivante) sont sautées :
    on garde la dernière version de la diapo."""
    sortie = subprocess.run(["mutool", "draw", "-F", "txt", "-o", "-", pdf],
                            capture_output=True, text=True).stdout
    pages = []
    for p in sortie.split("\f"):
        lignes = [re.sub(r"[ \t]+", " ", l).strip() for l in p.split("\n")]
        pages.append([l for l in lignes if l])
    sections = []
    for i, lignes in enumerate(pages):
        texte = "\n".join(lignes)
        if len(texte) < 40:
            continue
        suivante = "\n".join(pages[i + 1]) if i + 1 < len(pages) else ""
        if texte in suivante:
            continue
        # Pied de page "12 / 131" : numéro de diapo, retiré du texte
        if lignes and re.fullmatch(r"\d+ / \d+", lignes[-1]):
            titre = f"Diapo {lignes[-1].split()[0]}"
            lignes = lignes[:-1]
        else:
            titre = f"Page {i + 1}"
        # Le 1er non-vide sert de sous-titre (souvent le titre de la diapo)
        sous_titre = lignes[0][:80] if lignes else ""
        sections.append((titre, f"## {titre} : {sous_titre}\n\n" + "\n".join(lignes)))
    # Même numéro de diapo plusieurs fois (étapes d'animation qui ne sont
    # pas incluses les unes dans les autres) : on garde la dernière
    derniere = {t: i for i, (t, _) in enumerate(sections)}
    sections = [x for i, (t, x) in enumerate(sections) if derniere[t] == i]
    os.makedirs(os.path.dirname(md), exist_ok=True)
    with open(md, "w", encoding="utf-8") as f:
        f.write(f"# {os.path.basename(pdf)}\n\n" + "\n\n".join(sections) + "\n")


def morceaux_pdf(chemin, rel):
    """Convertit le PDF en .md si besoin, puis un morceau par section.
    debut = fin = numéro de diapo ou de page."""
    if not shutil.which("mutool"):
        print(f"[cherche] mutool absent : {chemin} ignoré", file=sys.stderr)
        return []
    md = chemin_md(rel)
    if not os.path.exists(md) or os.path.getmtime(md) < os.path.getmtime(chemin):
        print(f"[cherche]   conversion {rel} -> {os.path.relpath(md, ICI)}",
              file=sys.stderr)
        pdf_vers_md(chemin, md)
    with open(md, encoding="utf-8") as f:
        contenu = f.read()
    res = []
    for section in re.split(r"^## ", contenu, flags=re.M)[1:]:
        m = re.match(r"(Diapo|Page) (\d+)", section)
        num = int(m.group(2)) if m else 0
        res.append((num, num, "## " + section.strip()))
    return res


def morceaux(chemin, rel, genre):
    if genre == "cours":
        return morceaux_pdf(chemin, rel)
    with open(chemin, encoding="utf-8", errors="replace") as f:
        return morceaux_texte(f.readlines())


def signature(chemin):
    st = os.stat(chemin)
    return f"{st.st_size}-{int(st.st_mtime)}"


# --- Base SQLite ------------------------------------------------------------
#
#   fichiers(chemin, signature)            un par fichier indexé
#   morceaux(id, chemin, genre, debut, fin, texte, vecteur)
#       vecteur = embedding float32 normalisé, stocké en BLOB
#   morceaux_fts(texte, chemin)            index plein texte FTS5 (BM25),
#       rowid = morceaux.id
#
# Une base par modèle (table meta) : changer de modèle réindexe tout.

class Index:
    def __init__(self, modele, racine):
        self.modele_nom = modele
        self.racine = racine
        self.doc_pre, self.req_pre = PREFIXES.get(modele, ("", ""))
        self._modele = None
        os.makedirs(CACHE, exist_ok=True)
        self.db = sqlite3.connect(BASE)
        self.db.executescript("""
            CREATE TABLE IF NOT EXISTS meta(cle TEXT PRIMARY KEY, valeur TEXT);
            CREATE TABLE IF NOT EXISTS fichiers(chemin TEXT PRIMARY KEY, signature TEXT);
            CREATE TABLE IF NOT EXISTS morceaux(
                id INTEGER PRIMARY KEY, chemin TEXT, genre TEXT,
                debut INTEGER, fin INTEGER, texte TEXT, vecteur BLOB);
            CREATE INDEX IF NOT EXISTS morceaux_chemin ON morceaux(chemin);
            CREATE VIRTUAL TABLE IF NOT EXISTS morceaux_fts USING fts5(
                texte, chemin, tokenize="unicode61 remove_diacritics 2");
        """)
        # Racine ou modèle différent de la dernière fois : on repart de zéro
        config = f"{modele}|{racine}"
        ligne = self.db.execute("SELECT valeur FROM meta WHERE cle='config'").fetchone()
        if ligne is None or ligne[0] != config:
            self.vider()
            self.db.execute("INSERT OR REPLACE INTO meta VALUES('config', ?)", (config,))
            self.db.commit()
        # Conversion des PDF changée : on réindexe seulement les PDF
        ligne = self.db.execute("SELECT valeur FROM meta WHERE cle='pdf_md'").fetchone()
        if ligne is None or ligne[0] != PDF_MD_VERSION:
            self.db.execute("DELETE FROM fichiers WHERE chemin LIKE '%.pdf'")
            shutil.rmtree(PDF_MD, ignore_errors=True)     # reconvertit tout
            self.db.execute("INSERT OR REPLACE INTO meta VALUES('pdf_md', ?)",
                            (PDF_MD_VERSION,))
            self.db.commit()
        # Index plein texte absent ou incomplet : on le reconstruit (rapide,
        # aucun calcul d'embedding)
        nm = self.db.execute("SELECT COUNT(*) FROM morceaux").fetchone()[0]
        nf = self.db.execute("SELECT COUNT(*) FROM morceaux_fts").fetchone()[0]
        if nm != nf:
            self.db.execute("DELETE FROM morceaux_fts")
            self.db.execute("INSERT INTO morceaux_fts(rowid, texte, chemin) "
                            "SELECT id, texte, chemin FROM morceaux")
            self.db.commit()

    def vider(self):
        self.db.execute("DELETE FROM fichiers")
        self.db.execute("DELETE FROM morceaux")
        self.db.execute("DELETE FROM morceaux_fts")
        self.db.commit()

    @property
    def modele(self):
        if self._modele is None:
            print(f"[cherche] chargement du modèle {self.modele_nom} ...",
                  file=sys.stderr)
            self._modele = TextEmbedding(self.modele_nom, threads=THREADS)
        return self._modele

    def encoder(self, textes, prefixe):
        textes = [prefixe + t[:MAX_CARACTERES] for t in textes]
        v = np.array(list(self.modele.embed(textes, batch_size=LOT)), dtype=np.float32)
        # Normalisés : la similarité cosinus devient un simple produit scalaire
        return v / (np.linalg.norm(v, axis=1, keepdims=True) + 1e-9)

    def mettre_a_jour(self):
        actuels = lister_fichiers(self.racine)
        connus = dict(self.db.execute("SELECT chemin, signature FROM fichiers"))

        # Fichiers disparus : on les retire
        for rel in set(connus) - set(actuels):
            self.supprimer(rel)
            self.db.execute("DELETE FROM fichiers WHERE chemin=?", (rel,))

        a_faire = [rel for rel, (c, _) in actuels.items()
                   if connus.get(rel) != signature(c)]
        if a_faire:
            print(f"[cherche] indexation de {len(a_faire)} fichier(s) ...",
                  file=sys.stderr)
        for n, rel in enumerate(a_faire, 1):
            chemin, genre = actuels[rel]
            ms = morceaux(chemin, rel, genre)
            # Le nom du fichier aide le modèle (ex : "lire3a.c")
            vects = self.encoder([f"{rel}\n{t}" for _, _, t in ms], self.doc_pre) \
                if ms else []
            # Un fichier à la fois, validé tout de suite : si on interrompt,
            # le travail déjà fait est gardé
            self.supprimer(rel)
            for (d, f, t), v in zip(ms, vects):
                cur = self.db.execute(
                    "INSERT INTO morceaux(chemin, genre, debut, fin, texte, vecteur) "
                    "VALUES(?, ?, ?, ?, ?, ?)", (rel, genre, d, f, t, v.tobytes()))
                self.db.execute("INSERT INTO morceaux_fts(rowid, texte, chemin) "
                                "VALUES(?, ?, ?)", (cur.lastrowid, t, rel))
            self.db.execute("INSERT OR REPLACE INTO fichiers VALUES(?, ?)",
                            (rel, signature(chemin)))
            self.db.commit()
            print(f"[cherche]   {n}/{len(a_faire)} {rel} ({len(ms)} morceaux)",
                  file=sys.stderr)
        self.db.commit()

    def supprimer(self, rel):
        """Retire les morceaux d'un fichier (table et index plein texte)."""
        self.db.execute("DELETE FROM morceaux_fts WHERE rowid IN "
                        "(SELECT id FROM morceaux WHERE chemin=?)", (rel,))
        self.db.execute("DELETE FROM morceaux WHERE chemin=?", (rel,))

    def stats(self):
        nf = self.db.execute("SELECT COUNT(*) FROM fichiers").fetchone()[0]
        nm = self.db.execute("SELECT COUNT(*) FROM morceaux").fetchone()[0]
        return nf, nm

    def chercher(self, question, k, genre):
        """Recherche hybride. Renvoie les k meilleurs morceaux :
        [(score RRF, (chemin, genre, debut, fin, texte, vecteur))]."""
        sql = "SELECT id, chemin, genre, debut, fin, texte, vecteur FROM morceaux"
        if genre == "code":
            sql += " WHERE genre='code' OR chemin LIKE '%.md'"
        elif genre:
            sql += f" WHERE genre='{genre}'"
        lignes = {l[0]: l[1:] for l in self.db.execute(sql)}
        if not lignes:
            return []

        # 1. Embedding : similarité cosinus (vecteurs normalisés)
        ids = list(lignes)
        vect = np.frombuffer(b"".join(lignes[i][5] for i in ids), dtype=np.float32)
        vect = vect.reshape(len(ids), -1)
        q = self.encoder([question], self.req_pre)[0]
        rang_emb = [ids[i] for i in np.argsort(-(vect @ q))[:CANDIDATS]]

        # 2. Mots-clés : BM25 de FTS5 sur les mots de la question (mots
        #    vides retirés). Le nom du fichier compte double (lseek, sleepsort...)
        mots = [m for m in re.findall(r"\w+", question.lower())
                if len(m) > 1 and m not in MOTS_VIDES]
        rang_bm25 = []
        if mots:
            # Racinisation grossière : les mots longs sont cherchés par
            # préfixe ("sauvegarder" -> sauvegard* trouve "sauvegardés")
            requete = " OR ".join(f'"{m[:-2]}"*' if len(m) >= 7 else f'"{m}"'
                                  for m in mots)
            for (i,) in self.db.execute(
                    "SELECT rowid FROM morceaux_fts WHERE morceaux_fts MATCH ? "
                    "ORDER BY bm25(morceaux_fts, 1.0, 2.0) LIMIT ?",
                    (requete, 4 * CANDIDATS)):
                if i in lignes:
                    rang_bm25.append(i)
                    if len(rang_bm25) >= CANDIDATS:
                        break

        # 3. Fusion des rangs (RRF) : bien classé par les deux = en tête
        scores = {}
        for classement, poids in ((rang_emb, 1.0), (rang_bm25, POIDS_BM25)):
            for r, i in enumerate(classement):
                scores[i] = scores.get(i, 0.0) + poids / (RRF_K + r + 1)

        res, vus = [], set()
        for i in sorted(scores, key=scores.get, reverse=True):
            chemin, _, debut = lignes[i][:3]
            # Évite deux fenêtres qui se chevauchent dans le même fichier
            cle = (chemin, debut // FENETRE)
            if cle in vus:
                continue
            vus.add(cle)
            # Score affiché sur 100 : 100 = 1er des deux classements
            res.append((100 * scores[i] * (RRF_K + 1) / (1 + POIDS_BM25), lignes[i]))
            if len(res) >= k:
                break
        return res


# --- Rédaction de la réponse par le LLM ------------------------------------

def charger_llm(nom):
    from huggingface_hub import hf_hub_download
    from llama_cpp import Llama
    depot, fichier = LLMS[nom]
    print(f"[cherche] chargement du LLM {fichier} ...", file=sys.stderr)
    chemin = hf_hub_download(depot, fichier,
                             cache_dir=os.path.join(CACHE, "modeles"))
    return Llama(model_path=chemin, n_ctx=CONTEXTE, n_threads=THREADS,
                 n_threads_batch=THREADS, n_batch=256, verbose=False)


def sans_reflexion(flux):
    """Retire un éventuel bloc <think>...</think> (modèles Qwen3.x) du flux
    de texte, tout en laissant passer le reste au fil de l'eau."""
    tampon, dedans = "", None
    for t in flux:
        if dedans is False:
            yield t
            continue
        tampon += t
        if dedans is None:
            debut = tampon.lstrip()
            if "<think>".startswith(debut[:7]) and len(debut) < 7:
                continue                      # on ne sait pas encore
            dedans = debut.startswith("<think>")
            if not dedans:
                yield tampon
                continue
        if "</think>" in tampon:
            dedans = False
            reste = tampon.split("</think>", 1)[1].lstrip()
            if reste:
                yield reste
    if dedans is None and tampon:
        yield tampon


def rediger(llm, question, resultats):
    """Donne les extraits et la question au LLM ; écrit la réponse au fil
    de l'eau."""
    blocs = []
    for n, (_, (chemin, genre, debut, fin, texte, _)) in enumerate(resultats, 1):
        ou = f"{chemin} (diapo/page {debut})" if genre == "cours" \
            else f"{chemin}:{debut}-{fin}"
        blocs.append(f"[{n}] {ou}\n{texte[:EXTRAIT_MAX].strip()}")
    # La question avant ET après les extraits : les petits modèles oublient
    # facilement ce qu'on leur demande quand le contexte est long
    messages = [
        {"role": "system", "content": CONSIGNE},
        {"role": "user", "content": f"QUESTION : {question}\n\nEXTRAITS :\n\n"
                                    + "\n\n".join(blocs)
                                    + f"\n\nQUESTION : {question}"},
    ]
    flux = llm.create_chat_completion(messages, stream=True,
                                      max_tokens=REPONSE_MAX,
                                      temperature=0.1, repeat_penalty=1.1)
    textes = (m["choices"][0]["delta"].get("content") or "" for m in flux)
    for t in sans_reflexion(textes):
        print(t, end="", flush=True)
    print()


# --- Affichage --------------------------------------------------------------

GRAS, GRIS, BLEU, RAZ = ("\033[1m", "\033[90m", "\033[34m", "\033[0m") \
    if sys.stdout.isatty() else ("", "", "", "")


def afficher(score, ligne):
    chemin, genre, debut, fin, texte, _ = ligne
    ou = f"{chemin} (diapo/page {debut})" if genre == "cours" \
        else f"{chemin}:{debut}-{fin}"
    print(f"{GRAS}{BLEU}{ou}{RAZ}  {GRIS}score {score:.0f}/100{RAZ}")
    texte = texte.rstrip("\n")
    if genre == "cours":
        print("    " + texte[:600].replace("\n", "\n    "))
    else:
        for n, l in enumerate(texte.split("\n"), debut):
            print(f"{GRIS}{n:5d}{RAZ} {l}")
    print()


def main():
    p = argparse.ArgumentParser(
        description="Recherche sémantique locale dans les TP et le cours.")
    p.add_argument("question", nargs="*", help="la consigne / question")
    p.add_argument("-k", type=int, default=None,
                   help="nombre d'extraits (4 pour le LLM, 5 avec --extraits)")
    p.add_argument("--extraits", action="store_true",
                   help="affiche les extraits sans passer par le LLM")
    p.add_argument("--llm", choices=sorted(LLMS), default=LLM_DEFAUT,
                   help=f"LLM qui rédige la réponse ({LLM_DEFAUT})")
    p.add_argument("--type", choices=["code", "cours", "texte"],
                   help="ne chercher que dans ce type de source")
    p.add_argument("--racine", default=RACINE,
                   help=f"dossier à indexer (défaut : {RACINE})")
    p.add_argument("--modele", default=MODELE, help=f"modèle ({MODELE})")
    p.add_argument("--reindex", action="store_true",
                   help="vide la base et recalcule tout")
    a = p.parse_args()
    if a.k is None:
        a.k = 5 if a.extraits else 4

    index = Index(a.modele, os.path.abspath(a.racine))
    if a.reindex:
        index.vider()
    index.mettre_a_jour()
    nf, nm = index.stats()
    print(f"[cherche] {nm} morceaux indexés dans {nf} fichiers", file=sys.stderr)

    llm = None

    def repondre(q):
        nonlocal llm
        resultats = index.chercher(q, a.k, a.type)
        if a.extraits or not resultats:
            for score, ligne in resultats:
                afficher(score, ligne)
            return
        if llm is None:
            llm = charger_llm(a.llm)
        rediger(llm, q, resultats)
        # Sources, pour aller vérifier
        print(f"\n{GRIS}sources :{RAZ}")
        for n, (score, (chemin, genre, debut, fin, _, _)) in enumerate(resultats, 1):
            ou = f"{chemin} (diapo/page {debut})" if genre == "cours" \
                else f"{chemin}:{debut}-{fin}"
            print(f"  [{n}] {ou}  {GRIS}{score:.0f}/100{RAZ}")

    if a.question:
        repondre(" ".join(a.question))
        return
    # Mode interactif
    while True:
        try:
            q = input("question> ").strip()
        except (EOFError, KeyboardInterrupt):
            print()
            return
        if q:
            repondre(q)


if __name__ == "__main__":
    main()
