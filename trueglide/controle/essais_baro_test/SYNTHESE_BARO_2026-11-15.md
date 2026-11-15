# Synthèse baromètre DPS310, 15/11/2026 (Mini Pouce + carte neuve, USB seul)

## Prouvé (journaux Blackbox, debug BARO, carte identifiée par « H DeviceUID » comparé en local, jamais publié)

| Essai | Firmware | Carte | Config | Échantillons | Cadence | Lectures fausses |
|---|---|---|---|---|---|---|
| v1, témoin USB du 15/11 (110043) | cdd6f38 (compteurs, code non relu par l'auditeur) | Mini Pouce | Mini Pouce, PID 4 kHz | 3733 | 19,8/s | 4 (0,107 %) |
| v2 | 3b418e8 (+ lecture MEAS_CFG avant les 6 octets) | Mini Pouce | Mini Pouce, PID 4 kHz | 2425 | 12,6/s | 0 |
| v2 | 3b418e8 | carte neuve | usine, PID 8 kHz | 2538 + 151 | 10,6/s | 0 |
| v3 | a87822b (octets bruts, aucun accès bus ajouté) | Mini Pouce | Mini Pouce, PID 4 kHz | 14 188 | 19,85/s | 104 (0,733 %) |
| v3 | a87822b | carte neuve | usine, PID 8 kHz | 6038 | 19,9/s | 0 |
| v3 | a87822b | carte neuve | Mini Pouce collée (MINI_POUCE_avant_test_v2.txt), PID 4 kHz | 11 952 | 19,87/s | 33 (0,28 %), toutes hors plage, debug[3] = 0 |
| v3, essai B | a87822b | carte neuve | Mini Pouce + dshot_bidir OFF (en-tête bidir 0, denom 2) | 6508 | 19,83/s | 40 (0,61 %) |
| v3, essai C | a87822b | carte neuve | Mini Pouce recollée + UART1 vidé, télémétrie coupée (en-tête bidir 1, bit TELEMETRY absent) | 6065 | 19,91/s | 10 (0,165 %) |

Tous sur la base de la PR #16023 (542c458), USB seul, sans batterie, moteurs arrêtés. Hors tableau : les 3 journaux v1
armés du matin du 15/11 (batterie, hélices retirées, 10 lectures fausses en ≈ 88 s).

1. Nature des 104 lectures fausses (v3 Mini Pouce) : les 6 octets sont les registres 0x01 à 0x06 au lieu de 0x00 à 0x05
   (critère : buf[0] ≠ 0xFE, 104 cas ; dans ces 104 cas buf[5] = 0x54 = PRS_CFG, description seulement, pas un critère :
   TMP_B0 vaut 0x54 environ 1 fois sur 256 dans une lecture normale ; motif exact « décalé d'un registre » dans
   103 sur 104). Aucun drapeau d'erreur I2C sur ces lectures. Les données du capteur sont valides, lues un registre trop loin.
2. Ce motif n'est pas le défaut corrigé par la PR #16023 (adresse de registre sautée : la lecture repartirait du pointeur
   interne, attendu à 0x06 après une lecture 0x00-0x05 ; auto-incrément non vérifié dans la fiche technique).
3. Avec la lecture de MEAS_CFG ajoutée (v2), aucune lecture fausse sur Mini Pouce (0/2425 ; attendu 17,8 au taux v3,
   P ≈ 2×10^-8). Lequel compte, la lecture ajoutée ou la cadence plus basse : inconnu.
4. Même firmware v3, même cadence : Mini Pouce 104/14 188, carte neuve 0/6038 (attendu 44,3, P ≈ 6×10^-20). Le défaut est
   propre à Mini Pouce : sa carte, son câblage ou sa configuration (la carte neuve est nue et en réglages d'usine), dans
   ces conditions (USB seul, firmware de test sur base 542c458). → le point 5 montre que c'est sa configuration.
5. Carte neuve avec la configuration de Mini Pouce collée : 33 lectures décalées sur 11 952 (motif exact), contre 0/6038 en
   configuration d'usine (P ≈ e^-16,7). Journal attribué à la carte neuve par empreinte de l'identifiant (calculée par
   l'auditeur, comparée en local). Le défaut suit la configuration de Mini Pouce, pas la carte ni le montage. Le réglage en
   cause n'est pas identifié. Taux plus bas que sur Mini Pouce (0,28 % contre 0,73 %) : non expliqué. Essai fait sans
   fiche, à la demande de Johann.

6. Recherche du réglage (carte neuve, config Mini Pouce, une modification à la fois, essais faits sans fiche complète, à la
   demande de Johann). Essai A (pid_process_denom 1) impossible : avec dshot_bidir sur F4, Betaflight force
   pid_process_denom ≥ 2 (config/config.c l.669-686, l.683). Essai B (dshot_bidir OFF) : 40/6508, le défaut reste ; les 5
   journaux d'avant (bidir 1) sont écartés. Essai C (osd_uart, vtx_type, vtx_uart, msp_3_uart = NONE, feature -TELEMETRY,
   sur la config Mini Pouce recollée, bidir ON) : 10/6065, le défaut reste. Réserves : seule la télémétrie est prouvée
   par l'en-tête (bit 10) ; les 4 réglages UART1 n'y figurent pas (tapés à la main après un collage raté) : non vérifiés.
   10 observés contre ≈ 16,7 attendus (P(≤10) ≈ 5 %) : ni effet ni absence d'effet de C à conclure. Conclusion : ni
   dshot_bidir seul ni la télémétrie ne suppriment le défaut. Restent : pid_process_denom 2 (essai A' = bidir OFF +
   denom 1) ou un autre réglage hors des 3 groupes.

## Non prouvé

- La cause physique du décalage (pointeur du capteur à 0x01, ou 1er octet perdu avec 7 octets cadencés) : un analyseur
  logique sur SCL/SDA trancherait.
- L'écart v1 (0,107 %) contre v3 (0,733 %) sur la même carte : non expliqué.
- Le comportement sur master sans la PR : non testé.

## Conséquences

- PR #16023 : non concernée (autre défaut, prouvé au banc le 13/11). Commentaire validé, à poster avec le OUI de Johann.
- TRUEGLIDE cb4e3f0 rejette les pressions hors de 300-1200 hPa (barometer.c l.176-177, test l.535-536) : les 104 lectures décalées
  v3 sont hors de cette plage. Une lecture fausse peut toutefois tomber dans la plage (828 hPa le 14/11) : seul le filtre
  des sauts (l.444) peut alors l'écarter, non vérifié pour ce cas. La température n'est pas filtrée (−132,80 °C le 14/11) :
  son effet sur la correction de température de l'altitude est à vérifier.
- Prochain essai, plus tard, seulement si Johann le veut : A' (carte neuve, config Mini Pouce, dshot_bidir OFF +
  pid_process_denom 1), 5 minutes, avec une fiche courte contrôlée.
  A' se compare à B : seul pid_process_denom change (bidir OFF dans les deux). 0 faute en A' désignerait denom 2 ;
  sinon, le réglage est ailleurs dans le diff.
- Mini Pouce remis en TRUEGLIDE cb4e3f0 le 15/11 (version cb4e3f0f2, fichier complet PID doux + anti-desync collé, status :
  CONFIGURED, GYRO ICM42688P, I2C 0 erreur). La carte neuve garde le firmware de test v3 (montée sur aucun drone).
