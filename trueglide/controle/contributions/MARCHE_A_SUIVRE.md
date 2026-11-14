# Marche à suivre : contributions externes (Chris / ESC Configurator, Betaflight)

Règle de base : **celui qui écrit ne contrôle pas son propre travail.** Une discussion écrit, l'autre contrôle,
Johann donne le OUI écrit avant chaque envoi sur GitHub. Aucune étape n'est sautée, même pour une seule ligne.

## Phase A. Avant d'écrire

1. Relire les règles du projet visé (tableau en bas), et la liste des remarques déjà reçues du relecteur.
2. Chercher les doublons : PR et issues ouvertes et fermées sur le même sujet.
3. Délimiter **un seul sujet**. S'il y en a plusieurs, prévoir plusieurs PR.
4. Écrire le test qui **échoue** sur la branche de base, et le garder.

## Phase B. Écrire

1. Copier le style du code voisin : noms, messages, structure.
2. **Aucun commentaire que le code montre déjà.** Un commentaire seulement pour un pourquoi invisible
   (mesure, contrainte matérielle, piège).
3. Messages et traductions : même forme que les messages voisins (modèle de Chris :
   « Failed flashing ESC {{index}} - check file type » : court, l'action d'abord, un tiret, un conseil).
4. Aucune donnée privée : identifiant de carte (mcu_id, signature, DeviceUID), XPRO, chemins locaux.

## Phase C. Contrôles automatiques (celui qui écrit)

1. Tests du projet : tous réussis, et le nouveau test échoue sans la correction.
2. Lint : 0 erreur, aucun avertissement nouveau (comparer avant / après).
3. `trueglide/controle/contributions/verif_pr.sh <dépôt> <base> <esc|bf-fw|bf-app>` : aucun point BLOQUANT.
4. Joindre la sortie complète du script au message de contrôle.

## Phase D. Contrôle indépendant (l'autre discussion)

1. Refaire la correction ou la relire ligne par ligne, sur la même base.
2. Relancer tests, lint et `verif_pr.sh` lui-même.
3. Pour chaque commentaire ajouté : « le code le dit-il déjà ? ». Si oui : NON.
4. Pour chaque message ajouté : comparer aux messages voisins.
5. Relire le texte de la PR : chaque chiffre a sa preuve (journal, test, ligne de code), sinon « non vérifié ».
6. Répondre OUI ou NON, avec les corrections exactes.

## Phase E. Fiche d'explication pour Johann

1. Une fiche en français : le défaut, chaque ligne changée, la preuve.
2. Johann la lit et doit pouvoir expliquer la PR **sans outil** (règle Betaflight, AI_POLICY).

## Phase F. Envoi

1. Johann donne son **OUI écrit**.
2. Un seul envoi groupé : toutes les corrections d'un coup (chaque envoi relance une relecture).
3. Juste après : vérifier sur GitHub que le commit affiché est le bon et que les contrôles sont verts.

## Phase G. Après une remarque du relecteur

1. Appliquer la remarque **à toutes les PR ouvertes chez lui et aux branches prêtes**, pas seulement à la ligne
   montrée. Ajouter la remarque au tableau « remarques reçues » ci-dessous.
2. Refaire les phases C, D et F.
3. Ne jamais cliquer « Resolve » sur un fil de relecteur ou de robot. Prévenir le relecteur selon la règle de
   Johann (mail court pour Chris ; bouton « Re-request review » possible sur GitHub).

## Règles par projet (sources)

| Point | Chris (esc-configurator) | Betaflight firmware / App |
|---|---|---|
| Branche de base | `develop` (README, « Contributing ») | `master` (modèle de PR, point 2) |
| Taille | petite PR, un sujet | « one feature » par PR (modèle, point 4) ; une seule préoccupation (AGENTS.md) |
| Commits | libre | **1 seul commit** souhaité, sinon squash (modèle, point 5) |
| Tests | « provide tests where it makes sense » (README) | test qui échoue sans la correction (AGENTS.md) ; firmware : `make test`, `make EXTRA_FLAGS=-Werror checks` ; App : `yarn lint`, `vitest run` |
| Lint | règles du dépôt, avertissements à corriger si possible (README) | aucun nouvel avertissement |
| Commentaires | aucun commentaire évident (#590, #592) | pourquoi seulement, sans paraphrase (AGENTS.md de l'App) |
| Texte | anglais simple | ASD-STE100, phrases courtes (AGENTS.md) |
| IA | — | autorisée, sans obligation de la déclarer ; l'auteur doit tout comprendre (AI_POLICY.md) |
| Essai réel | banc | pilotes et capteurs : journal Blackbox d'un vrai vol avant « ready » (AGENTS.md) |
| Nombre de PR ouvertes | — | App : 2 PR ouvertes au plus pour un nouveau contributeur (blckmn, #5791, 14/11 ; label « Closed - exceeds allowed PR count »). Firmware : non vérifié, rester à 2 par prudence |

## Remarques reçues (à appliquer partout)

| Date | Relecteur | PR | Remarque | Appliquée à |
|---|---|---|---|---|
| 13/11 | Chris | #590 | « Implicit. Please remove this comment. » | #590, #592, #593, branches 4 à 7 |
| 14/11 | Chris | #592 | commentaire au-dessus de l'erreur redondant ; message à reformuler (« Gave up flashing ESC {{index}} - no progress for {{seconds}} s, check the ESC power ») | #592, #593 |
| 14/11 | (préventif) | #592, #593 | reformulation de 3 commentaires maladroits : 30f3570 (#592), 585badd (#593), pas encore poussés, en attente du OUI de Johann | #592, #593 |
