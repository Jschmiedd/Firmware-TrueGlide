# Fiche TG-H7-A1 : premier essai TRUEGLIDE sur la SEQURE H743 V2

But : vérifier que TRUEGLIDE démarre et voit tous les capteurs sur la H743. USB seul. Ce n'est pas un vol.

Firmware : `trueglide_2026.6.2_STM32H743_SEQUREH7V2.hex`, compilé le 26/11 depuis le dépôt (1e8e0d7, même code que le Mini Pouce). Construit en GCC 13.2.1, pas en CI : valable pour cet essai USB seulement.
SHA256 : `cd008c00991c3a8b73866941e40b27fdcbda5609582ca5e846fa435d93cadc0a`
Firmware d'essai : jamais en vol.

## Avant
1. Sauvegarde d'usine (dump all) enregistrée sur ton PC. **Fait le 26/11.**
2. Carte seule, en USB. Aucun ESC, aucun moteur, aucune batterie.
3. Mesure de départ, firmware d'usine : 9 erreurs I2C après 14 s (26/11).

## Règle
Une seule réponse NON = on s'arrête. On remet le firmware d'usine.

## Essai
| N° | Geste | Attendu | OUI / NON |
|---|---|---|---|
| 1 | Firmware d'usine : laisser branché 60 s, puis CLI `status`. Noter « I2C Errors » | Nombre noté (mesure de départ) | |
| 2 | Onglet « Flasher le Firmware » : « Charger un Firmware [Local] », choisir le fichier .hex. Cocher « Effacement complet de la puce ». Flasher. Fenêtre « Effacement complet de la puce activé… Continuer ? » → **OK**. Après le flash, fenêtre « Restaurer maintenant la sauvegarde… ? » → **ANNULER** (ce sont les réglages d'usine 4.5.0) | « Programmation : SUCCÈS » | |
| 3 | Se reconnecter. Lire la version en haut | TRUEGLIDE 2026.6.2, SEQUREH7V2 | |
| 4 | CLI `status` | GYRO=ICM42688P, BARO=DPS310, FLASH 16M | |
| 5 | Onglet Installation : pencher la carte vers l'avant, puis à droite | Le modèle suit dans le bon sens | |
| 6 | Onglet Capteurs : bouton « Afficher les données des capteurs en direct », puis le graphe d'altitude. Laisser la carte immobile 60 s | Pas de saut de plusieurs mètres | |
| 7 | CLI `status` après ces 60 s. Noter « I2C Errors » | Pas plus d'erreurs I2C par minute qu'au n° 1. Sinon NON : noter, retour au firmware d'usine | |

## Après
1. Envoyer : la sortie de `status` (n° 1 et n° 7) et une capture du graphe d'altitude (onglet Capteurs).
2. Pour partager les réglages : `diff all bare` seulement. Les `status` et réglages restent privés, jamais sur GitHub.
3. Aucun ESC ni moteur avec ce firmware : il faudra d'abord un firmware construit en CI.
