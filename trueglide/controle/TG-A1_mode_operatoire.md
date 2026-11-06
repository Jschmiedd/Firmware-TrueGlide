# Essai TG-A1 (corrections de l'audit du 06/11/2026) : envoi hors ligne et relance moteur, au banc, sans hélices

Date : …… Drone : …… Fait par : ……

Drone : Mini Pouce (SpeedyBee F405 Mini, TRUEGLIDE 24a71eb), **HÉLICES RETIRÉES**. Logiciel : la page d'essai
https://jschmiedd.github.io/TrueGlide-Configurator/test/ (pas /app/), ouverte avec **Ctrl + F5**. Parties 1 à 3 :
USB seulement, batterie débranchée. Partie 4 : batterie branchée, posé à plat et tenu. À la fin, tout est comme au
départ. Fiche qui contrôle sur la vraie carte ce que le simulateur ne peut pas montrer (voir
`REPONSE_AUDIT_2026-11-06.md`).

## Avant de commencer (rien à cocher)

1. Retirer les 4 hélices. Batterie débranchée.
2. Ouvrir la page d'essai, Ctrl + F5. Brancher l'USB, « Connexion ».
3. Onglet « Ligne de commande (CLI) » : taper `diff all`, Entrée, puis « Enregistrer sous » (sauvegarde de départ).

## TG-A1-1. Envoi depuis la carte hors ligne : sauvegarde et relecture

1. Tableau de bord : « Récolter cette carte ». Attendre « Carte récoltée ».
2. « Déconnecter ». En haut, choisir la carte hors ligne du Mini Pouce, puis « Connexion ».
3. Onglet Configuration : « Nom de l'appareil » → `AUDIT`. « Enregistrer et redémarrer ».
4. « Déconnecter ». Choisir le port USB de la carte, « Connexion ».
5. Tableau de bord : « Envoyer les changements préparés ». Lire la liste, puis « OK ».
6. Attendre la fin (la carte redémarre et se reconnecte seule, jusqu'à 45 secondes).

- À l'étape 5, un fichier « …_sauvegarde_avant_envoi_….txt » est arrivé dans les Téléchargements de l'ordinateur ?
- Après l'étape 6, le message dit « Carte relue après l'enregistrement : configuration en place. » ?
- Le Tableau de bord affiche maintenant le nom AUDIT ?

## TG-A1-2. La carte a changé depuis la récolte : question avant d'écraser

1. Tableau de bord : « Récolter cette carte » (nouvelle récolte, avec le nom AUDIT).
2. Onglet CLI : taper `set small_angle = 30`, Entrée, puis `save`, Entrée (la vraie carte change après la récolte).
   « Connexion » si le logiciel s'est déconnecté.
3. « Déconnecter ». Carte hors ligne du Mini Pouce, « Connexion ». Onglet Configuration : « Nom de l'appareil » →
   `Mini Pouce`. « Enregistrer et redémarrer ».
4. « Déconnecter ». Port USB, « Connexion ». Tableau de bord : « Envoyer les changements préparés », « OK ».
5. Une deuxième question s'ouvre : lire, puis **« Annuler »**.

- À l'étape 5, la question dit « La vraie carte a changé depuis la récolte » et montre la ligne `set small_angle = 30` ?
- Après « Annuler », le message dit « Envoi annulé : rien n'a été écrit » ?
- Le Tableau de bord affiche toujours le nom AUDIT (rien n'a été envoyé) ?

## TG-A1-3. Onglet CLI en carte hors ligne : seulement des réglages envoyés

Pour la F405 Mini, la carte hors ligne est la carte simulée (le vrai firmware tourne dans le navigateur) : son CLI
répond comme la vraie carte. Ce qu'on contrôle : une commande moteur tapée hors ligne ne part jamais vers la vraie carte.

1. « Déconnecter ». Carte hors ligne du Mini Pouce, « Connexion ».
2. Onglet CLI : taper `motor 0 1100`, Entrée. Puis `set small_angle = 180`, Entrée (sa valeur de départ).
3. « Déconnecter ». Port USB, « Connexion ». Tableau de bord : « Envoyer les changements préparés ». Lire la liste
   « Ce qui partira », puis « OK ». À la deuxième question (carte changée), « OK » cette fois.

- À l'étape 2, l'écran du CLI affiche les réponses (il n'est pas vide) ?
- À l'étape 3, la liste « Ce qui partira » contient `small_angle` (et le nom de l'appareil), mais **aucune** ligne
  `motor` ?
- Après la fin, le Tableau de bord affiche de nouveau le nom « Mini Pouce » ?
- Onglet CLI : `get small_angle` répond `small_angle = 180` (comme au départ) ?

## TG-A1-4. Relance d'un moteur calé (refait TG-P3A-3 sur le firmware actuel)

On vérifie qu'elle ne se déclenche pas à tort quand les moteurs tournent normalement.

1. CLI : taper `set motor_desync_recovery = ON`, Entrée, puis `save`, Entrée. « Déconnecter », débrancher l'USB.
2. Brancher la batterie, lunettes allumées, drone tenu. Armer, puis pendant 30 secondes monter et descendre les gaz,
   doucement puis vite, plusieurs fois. Désarmer. Débrancher la batterie.
3. Brancher l'USB, « Connexion ». CLI : taper `set motor_desync_recovery = OFF`, Entrée, puis `save`, Entrée.
   « Connexion ».

- Pendant les 30 secondes, l'OSD n'a jamais affiché « M1 RESTART », « M2 RESTART », « M3 RESTART » ni « M4 RESTART » ?
- Après l'étape 3, Tableau de bord : Relance moteur sur « Désactivé » ?

## TG-A1-5. Fin (USB branché, batterie débranchée)

- Onglet CLI : taper `diff all`, Entrée : à part l'ordre, c'est le même que la sauvegarde de départ ?
- Les fichiers « …_sauvegarde_avant_envoi_….txt » sont gardés (à envoyer) ?

## À envoyer

### Fichiers envoyés (sauvegardes avant envoi, diff all de départ et de fin)

### Réponse de « diff all » (partie 5) [grand]

### Autres remarques (messages affichés, ce qui a surpris)
