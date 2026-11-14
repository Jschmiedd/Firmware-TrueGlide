#!/usr/bin/env bash
# Contrôle automatique d'une branche de contribution AVANT tout envoi sur GitHub.
# Usage : verif_pr.sh <dossier du dépôt> <branche de base> [esc|bf-fw|bf-app]
# Ne modifie rien. Affiche les points à vérifier ; code de sortie 1 si un point BLOQUANT est trouvé.
set -u
DEP=${1:?dossier du dépôt}; BASE=${2:?branche de base}; PROJET=${3:-esc}
cd "$DEP" || exit 2
BLOQUANT=0
diffa() { git diff "$BASE"...HEAD "$@"; }

echo "== 1. Taille (une petite correction par sujet)"
diffa --stat | tail -1
echo "   commits : $(git rev-list --count "$BASE"..HEAD)"
[ "$PROJET" != esc ] && [ "$(git rev-list --count "$BASE"..HEAD)" -gt 1 ] && echo "   ATTENTION Betaflight : 1 seul commit souhaité (modèle de PR, point 5)"

echo "== 2. Commentaires ajoutés (règle de Chris : aucun commentaire que le code montre déjà)"
diffa -U0 -- . ':(exclude)*.md' | grep -nE '^\+\s*(//|/\*|\*[^/]|#\s)' | grep -vE '^\S*\+\+\+' || echo "   aucun"
echo "   -> pour CHAQUE ligne : « le code le dit-il déjà ? » Si oui : retirer. Garder seulement un pourquoi invisible."

echo "== 3. Données interdites (identifiant de carte, XPRO, chemins locaux)"
T1=$(diffa -U0 | grep -nE '^\+.*(mcu_id|DeviceUID|[0-9a-fA-F]{24}|XPRO|XPro|/home/|/tmp/|scratchpad)')
T2=$(diffa -U0 | grep -nE '^\+\s*signature(\s|$)')
if [ -n "$T1$T2" ]; then
  printf '%s\n%s\n' "$T1" "$T2" | grep -v '^$'; echo "   BLOQUANT"; BLOQUANT=1
else echo "   aucune"; fi

echo "== 4. Messages ajoutés (traductions, journaux) : style des messages voisins"
diffa -U0 -- '*.json' | grep -E '^\+\s*"' || echo "   aucun"
echo "   -> modèle Chris : « Failed flashing ESC {{index}} - check file type » : court, verbe d'abord, tiret, conseil."

echo "== 5. Auteurs et messages des commits"
git log --format='   %h | %an <%ae> | %s' "$BASE"..HEAD
git log --format='%B' "$BASE"..HEAD | grep -nE '#[0-9]+' | sed 's/^/   référence : /' || true
echo "   -> un « #numéro » dans un message de commit crée un renvoi visible dans cette PR-là."

echo "== 6. Espaces en fin de ligne"
diffa --check && echo "   aucun" || { echo "   BLOQUANT"; BLOQUANT=1; }

echo "== Résultat : $([ $BLOQUANT = 0 ] && echo 'aucun point bloquant, reste la relecture humaine' || echo 'POINT BLOQUANT : ne pas pousser')"
exit $BLOQUANT
