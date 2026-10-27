# Essai TG-P2 (TRUEGLIDE 2026.6.2) : première carte, contrôle à plat

Date : …… Drone : …… Fait par : ……

Drone : KarRmen (SpeedyBee F405 Mini), posé à plat sur la table, **HÉLICES RETIRÉES**, batterie branchée pour les
moteurs, l'OSD et la tension. Logiciel : TrueGlide Configurator (https://jschmiedd.github.io/TrueGlide-Configurator/app/),
Chrome ou Edge sur ordinateur. Matériel : multimètre, radio appairée, lunettes FPV, câble USB.

## Avant de commencer (rien à cocher)

1. Retirer les 4 hélices.
2. Carte branchée, onglet CLI : taper `diff all`, puis « Enregistrer » le texte (fichier daté). Faire aussi la sauvegarde complète de Betaflight.
3. Mesurer la tension de la batterie au multimètre et la noter dans le cadre du bas.
4. Onglet « Flasher le Firmware » : carte SPEEDYBEEF405MINI, modèle TRUEGLIDE, cocher « Effacement complet de la puce », puis flasher.
5. Rebrancher l'USB, puis « Connexion ».

## TG-P2-1. Flash et identité

- Le flash s'est terminé sans message d'erreur ?
- Le Tableau de bord affiche « TRUEGLIDE 2026.6.2 (1791465a6) » dans la case Firmware ?
- La carte affichée est SPEEDYBEEF405MINI ?
- La case « Fonctions TrueGlide » indique que les fonctions sont désactivées par défaut ?

## TG-P2-2. Capteurs (onglets Capteurs et Installation)

- Le gyroscope ICM-42688-P est détecté, à 8000 mesures par seconde, avec 0 erreur ?
- Après l'étalonnage de l'accéléromètre, drone posé à plat, le modèle 3D est à plat ?
- En penchant le drone en roulis, en tangage puis en lacet, le modèle 3D suit dans le bon sens ?
- Le baromètre est détecté ?

## TG-P2-3. Récepteur et modes

- Dans l'onglet Récepteur, chaque voie (roulis, tangage, lacet, gaz) bouge dans le bon sens ?
- Les voies vont d'environ 1000 à 2000 ?
- Dans l'onglet Modes, chaque interrupteur (ARM, ANGLE…) allume le bon mode ?
- Radio éteinte, le Tableau de bord affiche RX_FAILSAFE ?

## TG-P2-4. Moteurs (HÉLICES RETIRÉES)

- Le protocole moteur est DShot et le DShot bidirectionnel est actif ?
- La vitesse (tr/min) de chaque moteur s'affiche quand il tourne ?

## TG-P2-5. Moteurs, pour chacun (curseur de l'onglet Moteurs, doucement)

- Moteur 1 : il tourne à la bonne place et dans le bon sens ?
- Moteur 2 : il tourne à la bonne place et dans le bon sens ?
- Moteur 3 : il tourne à la bonne place et dans le bon sens ?
- Moteur 4 : il tourne à la bonne place et dans le bon sens ?

## TG-P2-6. OSD et batterie

- L'OSD s'affiche dans les lunettes ?
- La tension affichée est égale à celle du multimètre, à 0,1 V près ?
- Moteurs arrêtés, le courant affiché est proche de 0 A ?

## TG-P2-7. Fonctions TrueGlide (à plat, puis tout remettre comme avant)

| Étape | Action | Attendu |
| 1 | Onglet Protections : changer un réglage, « Enregistrer et redémarrer » | Le réglage est relu après le redémarrage |
| 2 | Onglet Modes | L'interrupteur SPEEDER est proposé |
| 3 | Onglet Caméra : basculer l'interrupteur de la caméra commandée | La sortie (PINIO) change d'état |
| 4 | CLI : `get battery_arm` | Répond une valeur ; sinon, ce firmware n'a pas la batterie basse : passer l'étape 5 |
| 5 | CLI : `set battery_arm_min_cell_voltage = 450` (4,50 V par élément, au-dessus de la batterie), `save`, essayer d'armer | Armement refusé, motif LOW_BATTERY ; remettre ensuite à 0 |

## TG-P2-8. Blackbox

- L'onglet Blackbox voit la mémoire flash de 16 Mo ?
- Après effacement, armer 10 s à plat puis désarmer : un journal est enregistré ?
- Le journal s'ouvre dans le Visualiseur Blackbox ?

## TG-P2-9. Sauvegarde

- Après « Enregistrer et redémarrer », les réglages sont toujours là ?

## À envoyer

### Tension de la batterie au multimètre

### Sortie de la commande « status » dans le CLI [grand]

### Sortie de « diff all » à la fin de l'essai [grand]

### Autres remarques
