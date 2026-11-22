# Marche à suivre : contributions externes (Chris / ESC Configurator, Betaflight)

Règle de base : **celui qui écrit ne contrôle pas son propre travail.** Une discussion écrit, l'autre contrôle,
Johann donne le OUI écrit avant chaque envoi sur GitHub. Aucune étape n'est sautée, même pour une seule ligne.

## Phase A. Avant d'écrire

1. Relire les règles du projet visé (tableau en bas), et la liste des remarques déjà reçues du relecteur.
2. Chercher les doublons : PR et issues ouvertes et fermées sur le même sujet.
3. Délimiter **un seul sujet**. S'il y en a plusieurs, prévoir plusieurs PR.
4. Écrire le test qui **échoue** sur la branche de base, et le garder.
5. **Chercher une fonction qui existe déjà** pour chaque calcul nouveau (`src/utils/helpers`, `src/sources`, et les fonctions ajoutées par nos PR déjà fusionnées) : la réutiliser au lieu de refaire le même calcul.

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
3 bis. Pour chaque calcul nouveau : « une fonction du dépôt le fait-elle déjà ? » (chercher son nom et ce qu'elle renvoie dans `src/utils/helpers` et `src/sources`). Si oui : NON, avec le nom de la fonction à utiliser.
4. Pour chaque message ajouté : comparer aux messages voisins.
5. Relire le texte de la PR : chaque chiffre a sa preuve (journal, test, ligne de code), sinon « non vérifié ».
6. Répondre OUI ou NON, avec les corrections exactes.

## Phase E. Fiche d'explication pour Johann

1. Une fiche en français : le défaut, chaque ligne changée, la preuve.
2. Johann la lit et doit pouvoir expliquer la PR **sans outil** (règle Betaflight, AI_POLICY).

## Phase E bis. Johann relit le diff dans VS Code (guidé par la discussion qui écrit)

1. VS Code, extension « GitHub Pull Requests » : ouvrir la branche ou la PR, onglet des fichiers changés.
2. Pour chaque fichier, lire chaque ligne verte (ajoutée) et rouge (retirée), avec la fiche d'explication à côté.
3. Pour chaque commentaire ajouté, se poser la question de Chris : « le code le dit-il déjà ? ».
4. Regarder les soulignements de l'extension ESLint : aucun nouveau.
5. Si besoin, Copilot Chat sur le diff, seulement pour CHERCHER des problèmes, jamais pour écrire :
   « Liste les commentaires qui répètent ce que le code dit déjà », « Ce message ressemble-t-il aux messages voisins ? ».
   Ses réponses sont vérifiées comme celles de toute IA.
   Copilot Chat envoie le code à un service extérieur : seulement sur le diff d'une PR publique, jamais avec un
   fichier TRUEGLIDE privé, une sauvegarde, un journal non masqué ni des données XPRO.
6. Ne pas lancer « Copilot code review » sur une PR déjà ouverte chez Chris ou Betaflight sans leur accord.
7. Noter les questions ; la discussion qui écrit y répond avant le OUI.

## Phase F. Envoi

0 bis. **Auteur et committer de chaque commit : Jschmiedd <242924782+Jschmiedd@users.noreply.github.com>** (règles de Johann,
   14/11 et 21/11). **Aucune ligne « Co-Authored-By », aucun lien de session, aucune mention d'IA** dans les commits, le code,
   les textes de PR et les réponses. `verif_pr.sh` bloque un autre auteur, une ligne « Co-Authored-By » et un lien de session. Les commits anciens
   de la #588 à la #595 restent tels quels (« on ne modifie pas les anciennes, ce sera fait à partir des prochaines »).
0. **Une seule discussion demande le OUI d'envoi à Johann : celle qui pousse.** L'autre contrôle avant et ne pose
   pas la question (malentendu du 14/11 : deux demandes croisées pendant le repas).
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

## Autres projets prévus (demande de Johann, 14/11)

Les règles de chaque projet sont lues et sourcées **le jour où une contribution y commence** (CONTRIBUTING,
AGENTS, modèle de PR, politique IA, remarques des mainteneurs). Rien n'est écrit de mémoire. Dépôts et branches
par défaut vérifiés le 14/11 (`git ls-remote`) ; la branche de base d'une PR est à confirmer dans les règles du projet.

| Projet | Dépôt officiel | Branche par défaut | Règles |
|---|---|---|---|
| INAV (base de TRUENAV) | iNavFlight/inav | maintenance-10.x | à lire |
| ArduPilot | ArduPilot/ardupilot | master | à lire |
| Mission Planner | ArduPilot/MissionPlanner | master | à lire |
| ExpressLRS | ExpressLRS/ExpressLRS | master | à lire |
| EdgeTX | EdgeTX/edgetx | main | à lire |
| Bluejay | bird-sanctuary/bluejay | main | à lire |
| AM32 | am32-firmware/AM32 | main | à lire |
| Blackbox Explorer | betaflight/blackbox-log-viewer | master | à lire (règles Betaflight probables, à confirmer) |

## Remarques reçues (à appliquer partout)

Source : reviews et commentaires de code de stylesuxx lus sur GitHub le 19/11 (PR #588 à #593, aucun commentaire de conversation de sa part), mails (règles seulement, sans citation). Heures en UTC.

| Date | Relecteur | PR | Remarque (texte exact) → règle | Appliquée à |
|---|---|---|---|---|
| 11/11 13:53 | Chris | #588 | Review « changes requested » : « for the normal flash path the error never reaches the UI […] Without `await`, the catch never sees the rejection […] Could you please change it to `return await flashTarget(target, flash);` and add a test that calls `writeHex()` with failing `read()`? » → **Règle : `return await` dans un `try` pour que le `catch` voie l'erreur ; un test pour chaque chemin d'échec corrigé** [review](https://github.com/stylesuxx/esc-configurator/pull/588#pullrequestreview-5859822014) | #588 (corrigé dans les branches ARM et SiLabs, réponse du 11/11 15:49 UTC) |
| 11/11 13:59 | Chris | #589 | Approuvée sans remarque | — |
| 11/11 14:02 | Chris | #590 | Code, `src/Components/Log/index.jsx` l. 59 : « Implicit. Please remove this comment. » → **Règle : pas de commentaire quand le code le dit déjà** [commentaire](https://github.com/stylesuxx/esc-configurator/pull/590#discussion_r4538814241) | #590 (approuvée le 14/11, 1580c7e), #592, #593, branches 4 à 7 |
| 11/11 14:08 | Chris | #591 | Review « changes requested » : « For `FLASH*FAILED` I'd prefer skipping instead of blocking everything. With the current change the user can't write settings (or melodies) to the healthy ESCs anymore. Could you: skip ESCs marked `FLASH*FAILED` when writing settings or restoring defaults, logging `writeSettingsBlockedFlashFailed` for each one skipped; take the master settings from the first healthy ESC instead of the first available one » → **Règle : ne jamais bloquer tout l'utilisateur pour un seul ESC en défaut ; sauter et journaliser** [review](https://github.com/stylesuxx/esc-configurator/pull/591#pullrequestreview-5860019717) | #591 (14b1ab9) |
| 11/11 15:49 et 15:58 | Chris | #591 | Code, `src/Containers/App/index.jsx` l. 601 : « Please keep the explicit version instead of the ternary operator - it's easier to read for non AI... » puis review : « Just this one more nit pick with the ternary operator. » → **Règle : pas d'opérateur ternaire, version explicite `if` / `else`** [commentaire](https://github.com/stylesuxx/esc-configurator/pull/591#discussion_r4540030537), [review](https://github.com/stylesuxx/esc-configurator/pull/591#pullrequestreview-5861584173) | #591 (ce6903b, approuvée le 11/11 16:37 UTC, dcade50) ; à appliquer partout |
| 11/11 | Chris (mail privé, non cité) | toutes | Règles : passer la PR en « Ready for Review » quand elle est prête ; tester rigoureusement ; garder un code petit et maintenable | toutes |
| 14/11 17:50 | Chris | #592 | Code, `src/utils/Errors.js` l. 115 : « Please Remove the comment, I think the error as is is explicit enough and thus makes the comment rather redundant. » [commentaire](https://github.com/stylesuxx/esc-configurator/pull/592#discussion_r4574941277) | #592 (30f3570, approuvée le 16/11) |
| 14/11 17:53 | Chris | #592 | Code, `src/translations/en/log.json` l. 23 : « This reads a bit awkwardly, maybe re-formulate to something like: Gave up flashing ESC {{index}} - no progress for {{seconds}} s, check the ESC power » → **Règle : messages courts et directs, même forme que les messages voisins** [commentaire](https://github.com/stylesuxx/esc-configurator/pull/592#discussion_r4574953998) | #592 (30f3570), #593 |
| 14/11 | (préventif) | #592, #593 | reformulation de 3 commentaires maladroits : 30f3570 (#592), 585badd (#593), poussés le 14/11 vers 19 h 25 UTC avec le OUI de Johann (donné dans la discussion de l'auditeur), avant le contrôle indépendant de la phase D ; contrôlés après coup : commentaires seulement, verif_pr.sh sans point bloquant | #592, #593 |
| 14/11 | Johann (mail à Chris) | #593 | Règle comprise de #590 et #592 : un commentaire seulement quand le code ne peut pas montrer le pourquoi ; 3 commentaires qui répétaient le code retirés | #593 (191203a) ; à appliquer partout |
| 16/11 19:33 (rédigés le 14/11) | Chris | #593 | Review « changes requested ». Code, `src/utils/Serial.js` l. 215 : « The assignment does not really protect anything, nicer to read would be: `await this.writer.releaseLock(); this.writer = null` […] not sure it is really needed here, code is pretty self explanatory. » Code, `src/Containers/App/index.jsx` l. 272 : « The passthrough recovery a few lines below still uses the old behaviour. […] Can you please use `exitFourWayInterface(true)` and `this.getApiVersion()` there too? » → **Règles : pas de variable intermédiaire qui ne protège rien ; appliquer la même correction partout où le même cas existe** [Serial.js](https://github.com/stylesuxx/esc-configurator/pull/593#discussion_r4575013978), [App/index.jsx](https://github.com/stylesuxx/esc-configurator/pull/593#discussion_r4575022520) | #593 (68681f3, 4aa1608, réponse du 16/11 20:50 UTC). **État au 19/11 : toujours « changes requested » → demander une nouvelle relecture (« Re-request review »)** |
| 22/11 | Chris | #595 | Review « changes requested ». Code, `src/Components/Flash/CommonSettings/index.jsx` : « I think this is the same what `getCommonSettingNames(master)` from `utils/helpers/Settings` does. Could you please use that instead? Computing it inside `onSettingsUpdate` (or adding it to the `useCallback` deps) would also make the ref unnecessary. » → **Règle : réutiliser une fonction qui existe déjà au lieu de refaire le même calcul (ici, une fonction ajoutée par notre propre #591) ; contrôle ajouté aux phases A.5 et D.3 bis** [commentaire](https://github.com/stylesuxx/esc-configurator/pull/595#discussion_r4658620797) | #595 (commit « refactor(settings): use getCommonSettingNames… ») ; PR du fichier de réglages ; à appliquer partout |
