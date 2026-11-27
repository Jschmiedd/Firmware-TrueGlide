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

## Résultat (27/11, fait par Johann)
- N° 1 : firmware d'usine Betaflight 4.5.0, `status` après 300 s : 9 erreurs I2C, déjà 9 après 14 s (toutes au démarrage). OUI.
- N° 2, 3 : flash en mode DFU ; le texte de fin « Programmation : SUCCÈS » n'a pas été vu dans une capture ; la version lue ensuite (`TRUEGLIDE / STM32H743 (H743) 2026.6.2 Nov 26 2026 / 19:28:14`, SEQUREH7V2) prouve le flash. OUI.
- N° 4 : ICM42688P, DPS310, FLASH 16M. OUI.
- N° 5 : validé sur la parole de Johann seulement, sans image. OUI.
- N° 6 : vidéos de 30 s et 17 s : altitude entre −1,2 et +0,6 m, sans saut ; les 60 s sont validées sur la parole de Johann seulement, sans image. OUI.
- N° 7 : `status` après 296 s : 0 erreur I2C (usine : 9). OUI.
- Remarque : 1er démarrage après le flash, gyro au repos à −2,9 / −6,6 / +3,2 °/s et lacet −21° en 26 s ; après débranchement et rebranchement carte immobile : 0,00 / 0,24 / 0,98 °/s, lacet −3° en 15 s. Cause probable : calibration faite pendant que la carte bougeait au démarrage. Pas de défaut de carte. À recontrôler en premier à l'essai suivant, carte posée immobile avant de brancher.
- Verdict : TG-H7-A1 réussi. Suite : binaire construit en CI (GCC 13.3.1) avant tout essai avec ESC ou moteurs.
- Complément (27/11) : carte chaude (cœur 42 → 54 °C), lacet −259° en 8 min environ (gyro Z 0,73 °/s). Rebranchée à chaud et immobile : gyro Z 0,00 °/s, lacet 359° → 0° en 92 s environ (≈ 1°/min). Cause : dérive thermique du zéro du gyro calibré à froid. Pas de défaut.
