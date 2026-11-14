# Passation de l'audit, 14/11/2026 (Johann change d'auditeur)

À lire en premier par le nouvel auditeur, avec trueglide/REPRISE.md (« Règles »), ETAT.md, et
contributions/MARCHE_A_SUIVRE.md + REGLES_CONTRIBUTIONS.docx.

## 1. ESC Configurator (Chris, stylesuxx/esc-configurator)

| PR | État |
|---|---|
| #588, #589, #590, #591 | Fusionnées dans develop |
| #592 | GitHub = 30f3570. Les 2 demandes de Chris sont faites. « Changes requested » reste affiché jusqu'à sa nouvelle relecture |
| #593 | GitHub = 585badd. **Pas encore relue par Chris.** À REFAIRE (le commit local 0c939eb de l'ancien auditeur est perdu) : retirer 3 lignes, commit au nom de Jschmiedd avec « Co-Authored-By: Claude », puis contrôle, OUI de Johann, envoi |

Les 3 lignes à retirer de la #593 (sur 585badd) :
1. src/Containers/App/index.jsx : `// In passthrough, the answer to the exit is read before the port closes`
   (au-dessus de `await this.serial.exitFourWayInterface(serial.fourWay)`).
2. src/utils/Msp.js : `// A command cancelled because the port closes is not a failure`
   (au-dessus de `if(!(e instanceof PortClosedError))`).
3. src/utils/Msp.js : `// Late answer to another command: skip it, look further`
   (au-dessus de `state = 0;` dans le test expectedCommand).
Résultat attendu (mesuré par l'ancien auditeur, fusionné dans develop 8ed7caa) : 424/424 tests, lint 0 erreur et
29 avertissements (= develop), verif_pr.sh sans point bloquant.

## 2. Betaflight

| PR | État |
|---|---|
| betaflight #16022 (GM3) | Ouverte, approuvée par CodeRabbit, attend les mainteneurs |
| configurator #5789, #5790 | Ouvertes, approuvées par CodeRabbit, attendent les mainteneurs |
| configurator #5791 | Fermée par la limite de 2 PR (blckmn), réouverture automatique quand #5789 et #5790 seront fusionnées |
| betaflight #16023 (I2C) | Brouillon. Commentaire du vol du 14/11 validé, avec C1 (retirer la phrase dshot_bitbang) et C2 (Johann confirme ou non « outdoors, hover and gentle turns » : c'était un stationnaire en intérieur). Pas encore posté. Johann fera un vol extérieur avec le même firmware avant |

## 3. Essai baro (firmware de TEST cdd6f38, jamais dans une PR : pas d'annotations //!<)

- Fiche : essais_baro_test/TG-BARO-TEST_mode_operatoire.md (f143452).
- Séance 1 (journal …20261114_223100, 11,0 s armé au ralenti) : 1 faute à t = 10,966 s, P −6586 hPa,
  T −287,02 °C, avec debug[5], [6] et [7] à 0 pendant tout le journal. Pour cet événement, les hypothèses
  « lecture en erreur utilisée » et « lecture utilisée avant la fin » ne sont PAS confirmées. Piste à vérifier :
  le capteur lui-même (lecture sans test de « data ready », barometer_dps310.c l.257-258).
- Témoin USB (8 Mo, 3 logs) : log 1 tronqué à 0xC5F9D ; analyse log par log à faire.
- À faire : séance 2 (2 passages de 30 s armé, HÉLICES RETIRÉES, 40 s au plus batterie branchée). La carte est
  sous le firmware de test : ne pas voler. Ne pas enregistrer l'onglet Ports de l'App 2026.6.2 avec ce firmware.
- Journaux à ranger seulement masqués (DeviceUID).

## 4. TRUEGLIDE, à appliquer au retour en cb4e3f0

- Fichier complet PID doux + anti-désync : desync/MINI_POUCE_TRUEGLIDE_complet_2026-11-14_PID_doux_anti_desync_cb4e3f0.txt
  (validé, dernière version a2830f5).
- Démag des ESC : déjà en High (14/11, relu).
