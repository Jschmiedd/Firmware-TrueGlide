# Essai TG-A1-7 : tests moteurs restants et mode 3D (firmware ee050eb, logiciel 1cf71f8 sur /test/)

Date : …… Drone : Mini Pouce …… Fait par : ……

**HÉLICES RETIRÉES, drone fixé.** Page /test/ (version 1cf71f8 en bas à droite). Curseur tiré à la souris. Si un moteur ne s'arrête pas : **débrancher la
batterie tout de suite**. À faire AVANT de remettre la carte en Betaflight.

## TG-A1-7A. Page cachée (mode normal)

1. Onglet Moteurs, batterie branchée, « Je comprends les risques » sur oui.
2. Maître vers 1100 : les moteurs tournent.
3. Cliquer sur un autre onglet du navigateur.

- À l'étape 3, les 4 moteurs se sont arrêtés tout de suite ?

![Page cachée](photos/TG-A1-7/7A_page_cachee.jpg)

## TG-A1-7B. Fenêtre « Réordonner les moteurs »

1. Revenir sur TrueGlide, onglet Moteurs. Bouton « Réordonner les moteurs ».
2. « Je comprends les risques » sur oui : un moteur tourne.
3. Cliquer sur un autre onglet du navigateur.

- À l'étape 3, la fenêtre s'est fermée et le moteur s'est arrêté ?

![Réordonner](photos/TG-A1-7/7B_reordonner.jpg)

## TG-A1-7C. Mode 3D

1. Batterie BRANCHÉE (un ESC sans batterie ne répond pas), hélices retirées. Onglet ESC : lire les 4 ESC, faire une
   capture d'écran (sens d'origine).
2. Sens des 4 ESC sur « Bidirectionnel » (si le sens d'origine est inversé : « Bidirectionnel inversé »), écrire.
3. Onglet Moteurs : Mode 3D sur oui, « Sauvegarder et Redémarrer ».
4. « Je comprends les risques » sur oui.
5. Monter un peu le curseur du MOTEUR 1 SEULEMENT (pas le Maître), puis le descendre un peu sous le milieu.
6. « Arrêter les moteurs ».
7. Remettre : Mode 3D sur non, « Sauvegarder et Redémarrer » ; onglet ESC : sens d'origine (capture), écrire ;
   puis débrancher la batterie.

- À l'étape 4, les curseurs sont au milieu (1500) et aucun moteur ne tourne ?
- À l'étape 5, le moteur 1 tourne dans un sens en haut, dans l'autre sens en bas ?
- À l'étape 5, les moteurs 2, 3 et 4 n'ont PAS bougé du tout ?
- À l'étape 6, tous les moteurs se sont arrêtés ?
- À l'étape 7, curseurs revenus en bas (1000) et ESC au sens d'origine ?

![Mode 3D](photos/TG-A1-7/7C_3D.jpg)
