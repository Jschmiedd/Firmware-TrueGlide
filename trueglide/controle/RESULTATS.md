# Résultats des fiches d'essai TrueGlide

Les fiches `.docx` sont des formulaires vierges : Johann les remplit à la main pendant l'essai et donne ses réponses
dans la discussion. Ce tableau garde les résultats, avec leur preuve (journal Blackbox, `status`, `diff all`) et la
ligne de `trueglide/REPRISE.md` où ils sont détaillés. Une fonction n'est dite « testée » que si sa ligne est ici, sur
le firmware où elle a été testée. Tableau ajouté après l'audit indépendant du 06/11/2026 (constat 12).

| Fiche | Date | Firmware | Résultat | Preuves | Détail |
|---|---|---|---|---|---|
| TG-P2 (carte à plat, F405 Mini) | 28/10/2026 | 1791465 | 32 « oui » sur 32 | `diff all` avant / après identiques (sauf acc_calibration, LED_STRIP) | REPRISE, 28 octobre |
| TG-P2B (baromètre, banc) | 29/10/2026 | f9fc6d5 | 8 « oui » sur 8 | journal 20261029_093729 : 3 mesures fausses écartées | REPRISE, 29 octobre |
| TG-P3 (premiers vols, fonctions TrueGlide coupées) | 29/10/2026 | f9fc6d5 | 16 « oui », 2.2 non cochée (batterie pas pleine) | journal 20261029_162404 (2 vols ANGLE, 1 ACRO) | REPRISE, 29 octobre |
| TG-P3A (fonctions TrueGlide au banc, sans hélices) | 30/10/2026 | avant 24a71eb | A1, A2, A3, A4, A5 « oui » | OSD G0030/G0031, Blackbox SPEEDER, capteurs Temp 21-24 sur la radio | REPRISE, 30/10 |
| TG-P2B refait | 31/10/2026 | 24a71eb | firmware, debug_mode, DPS310 OK ; baromètre stable | journal 8,5 s, debug[4] = 0 ; LED_STRIP coupée : F2 pas prouvé | REPRISE, 31/10 |
| TG-P2C (baromètre avec LED_STRIP) | 31/10/2026 | 24a71eb | défaut du baromètre plus reproduit | 31,7 s armé, status 0 erreur I2C, debug[4] = 0 | REPRISE, 31/10 |
| TG-V1 (premier vol en intérieur) | 03/11/2026 | 24a71eb | tout « oui » | Blackbox 50,7 s sans défaut, status 0 erreur I2C | REPRISE, 03/11 |
| TG-P3B (fonctions TrueGlide en vol) | — | — | **PAS FAIT** | — | fiche prête |

## À refaire sur le firmware actuel

- **Relance moteur (TG-P3A-3)** : testée le 30/10 sur un firmware d'avant 24a71eb ; le code de la relance a changé le
  31/10. Elle n'est donc **pas testée sur 24a71eb** : à refaire au banc avant de la dire testée.
- **ALT HOLD avec le baromètre** : à voir en vol dans TG-P3B.
