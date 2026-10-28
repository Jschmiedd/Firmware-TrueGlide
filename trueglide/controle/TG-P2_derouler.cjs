// Fiche TG-P2 déroulée dans le vrai logiciel en ligne (/app/), carte F405 Mini en TRUEGLIDE 2026.6.2 (1791465a6)
// simulée sur l'ordinateur (TCP 5761, pont WebSocket 5800). Pour chaque question : la vérification (OK / ÉCHEC) et
// une photo de l'écran, ce qu'il faut regarder entouré en rouge. Photos : photos/<question>.png ; résultats : resultats.json
const { chromium } = require('playwright'); const fs = require('fs'); const D = __dirname + '/';
const URL = process.argv[2] || 'https://jschmiedd.github.io/TrueGlide-Configurator/app/';
const DIFF = fs.readFileSync(process.argv[3], 'utf8');
fs.mkdirSync(D + 'photos', { recursive: true });
const res = [];
(async () => {
  const b = await chromium.launch(); const p = globalThis.pp = await b.newPage({ viewport: { width: 1400, height: 900 }, locale: 'fr-FR' });
  const err = []; p.on('pageerror', e => err.push(e.message));
  const vus = []; p.on('dialog', d => { vus.push(d.message()); d.accept(); });
  const ok = async () => { const fe = p.locator('[role=dialog] button').filter({ hasText: /^\s*(Fermer|OK)\s*$/ }); for (let i = 0; i < await fe.count(); i++) { try { await fe.nth(i).click({ timeout: 800 }); } catch (e) {} } };
  const onglet = async (nom) => { await ok(); await p.locator('a,button').filter({ hasText: new RegExp('^\\s*' + nom + '\\s*$') }).first().click(); await p.waitForTimeout(3500); await ok(); };
  const connecte = async () => (await p.getByText('Déconnecter', { exact: true }).count()) > 0;
  const portBtn = () => p.locator('button').filter({ hasText: /Mode virtuel|Sélection manuelle|ws:\/\/|Choisir un port|Virtual Mode/i }).first();
  const brancher = async () => { await ok(); if (await connecte()) return; await portBtn().click(); await p.waitForTimeout(700); await p.getByRole('menuitem').filter({ hasText: /^\s*Choisir un port\s*$/ }).first().click(); await p.waitForTimeout(700);
    const d = p.locator('[role=dialog] input'); if (await d.count()) { await d.first().fill('ws://localhost:5800'); await p.locator('[role=dialog] button').last().click(); }
    for (let i = 0; i < 40 && !(await connecte()); i++) await p.waitForTimeout(500); await p.waitForTimeout(5000); };
  const reconnecter = async () => { for (let i = 0; i < 20 && (await connecte()); i++) await p.waitForTimeout(500); await p.waitForTimeout(3000);
    if (!(await connecte())) { await p.locator('button').filter({ hasText: /^\s*Connexion\s*$/ }).first().click().catch(() => {}); for (let i = 0; i < 40 && !(await connecte()); i++) await p.waitForTimeout(500); await p.waitForTimeout(5000); }
    for (let essai = 0; essai < 3 && !(await connecte()); essai++) { await p.waitForTimeout(3000); await brancher(); } await ok(); };
  const texte = async () => (await p.locator('#content').innerText()).replace(/\s+/g, ' ');
  const zone = () => p.locator('textarea[name=commands]');
  const cli = async (cmd, attente = 2500) => { await zone().fill(cmd); await zone().press('Enter'); await p.waitForTimeout(attente); const t = await p.locator('#content').innerText(); return t.slice(t.lastIndexOf(cmd)); };
  // photo : ce qu'il faut regarder entouré en rouge, puis l'entourage retiré
  const photo = async (id, cibles = []) => {
    await ok();
    for (const c of cibles) { try { await c.first().scrollIntoViewIfNeeded({ timeout: 2000 }); await c.first().evaluate(e => { e.dataset.tgAvant = e.style.outline; e.style.outline = '4px solid #e00'; e.style.outlineOffset = '2px'; }); } catch (e) {} }
    await p.waitForTimeout(300); await p.screenshot({ path: D + 'photos/' + id + '.png' });
    for (const c of cibles) { try { await c.first().evaluate(e => { e.style.outline = e.dataset.tgAvant || ''; }); } catch (e) {} }
  };
  const note = async (id, ok_, detail, cibles) => { res.push({ id, ok: ok_, detail }); console.log((ok_ === null ? 'À VOIR ' : ok_ ? 'OK     ' : 'ÉCHEC  ') + id + ' : ' + detail); await photo(id, cibles); };
  const txt = (s) => p.locator('#content').getByText(s, { exact: false });

  await p.goto(URL);
  await p.evaluate(() => { localStorage.clear(); localStorage.setItem('showManualMode', JSON.stringify({ showManualMode: true })); localStorage.setItem('expertMode', JSON.stringify({ expertMode: true })); });
  await p.goto(URL); await p.waitForTimeout(5000); await ok();

  // carte neuve comme après le flash : la fenêtre « Danger » s'ouvre à la connexion
  await brancher();
  const danger = p.locator('[role=dialog]').filter({ hasText: /Danger/ });
  if (await danger.count()) { await p.screenshot({ path: D + 'photos/avant-6_danger.png' }); console.log('VU      fenêtre Danger à la connexion (accéléromètre pas calibré)'); }
  await ok();
  // Avant de commencer, étape 7 : coller le diff all, save
  await onglet('Ligne de commande \\(CLI\\)');
  const lignes = DIFF.replace(/\r/g, '').split('\n').map(l => l.trim()).filter(l => l && !l.startsWith('#'));
  for (const l of lignes) { await zone().fill(l); await zone().press('Enter'); await p.waitForTimeout(60); }
  await p.waitForTimeout(3000);
  const nErr = ((await p.locator('#content').innerText()).match(/ERROR|Invalid|Unknown command/gi) || []).length;
  console.log(`${nErr ? 'ÉCHEC  ' : 'OK     '}avant-7 : ${lignes.length} lignes collées, ${nErr} ERROR`);
  await p.screenshot({ path: D + 'photos/avant-7_cli.png' });
  await zone().fill('save'); await zone().press('Enter'); await reconnecter();

  // Partie 1
  await onglet('Tableau de bord'); await p.waitForTimeout(3000);
  let t = await texte();
  await note('TG-P2-1.1', null, 'message de fin du flash : à voir pendant le vrai flash', []);
  const fw = (t.match(/Firmware (TRUEGLIDE [0-9.]+ \([0-9a-f]+\))/) || [])[1] || '?';
  await note('TG-P2-1.2', fw === 'TRUEGLIDE 2026.6.2 (1791465)', `case Firmware : « ${fw} »`, [p.locator('#content div').filter({ has: p.getByText('Chercher la mise à jour') }).filter({ has: p.getByText('TRUEGLIDE 2026.6.2') }).last()]);
  await note('TG-P2-1.3', /SPEEDYBEEF405MINI/.test(t), 'SPEEDYBEEF405MINI', [txt('SPEEDYBEEF405MINI ·')]);
  const f = (t.match(/Fonctions TrueGlide(.{0,250})/) || [])[1] || '';
  await note('TG-P2-1.4', /Protection anti-surchauffe Désactivé/.test(f) && /Relance moteur \(désynchronisation\) Désactivé/.test(f) && /Speeder \(angle de croisière\) 60°/.test(f) && /Sorties commandées \(PINIO\) Désactivé/.test(f), f.slice(0, 200).trim(),
    [p.locator('#content div').filter({ has: p.getByText('Ouvrir les fonctions TrueGlide') }).filter({ has: p.getByText('Relance moteur (désynchronisation)') }).last()]);

  // Partie 2
  await onglet('Ligne de commande \\(CLI\\)');
  const st = await cli('status', 3000);
  await note('TG-P2-2.1', /GYRO: \(1\) ICM42688P enabled/.test(st) && /ACC: ICM42688P/.test(st) && /BARO: DPS310/.test(st), (st.match(/GYRO:[^\n]*\n[^\n]*\n[^\n]*/) || ['?'])[0].replace(/\n/g, ' / '), [txt('GYRO: (1) ICM42688P')]);
  const rate = +((st.match(/GYRO rate: (\d+)/) || [])[1] || 0);
  await note('TG-P2-2.2', rate > 7800 && rate < 8200 && /I2C=1 \(0 errors\)/.test(st), `GYRO rate: ${rate} ; ${(st.match(/I2C=\d+ \(\d+ errors\)/) || ['?'])[0]}`, [txt('GYRO rate:')]);
  const flash = (st.match(/FLASH:[^\n]*/) || ['?'])[0];
  // partie 4 (CLI)
  const g1 = await cli('get motor_pwm_protocol'); const v1 = (g1.match(/motor_pwm_protocol = \S+/) || ['?'])[0];
  await note('TG-P2-4.1', /DSHOT(300|600)/.test(v1), v1, [txt(v1)]);
  const g2 = await cli('get dshot_bidir'); const v2 = (g2.match(/dshot_bidir = \S+/) || ['?'])[0];
  await note('TG-P2-4.2', v2 === 'dshot_bidir = ON', v2, [txt(v2)]);
  await cli('status', 3000);
  await note('TG-P2-8.1', /FLASH: JEDEC ID=0x00ef4018 16M/.test(flash), flash, [txt('FLASH: JEDEC')]);
  await zone().fill('exit'); await zone().press('Enter'); await reconnecter();

  // Partie 2.3 / 2.4 : Installation
  await onglet('Capteurs'); t = await texte();
  const cal = p.locator('#content').getByRole('button', { name: /^\s*Calibrer\s*$/ });
  await note('TG-P2-2.3', /Accéléromètre/.test(t) && (await cal.count()) > 0, (await cal.count()) ? 'case Accéléromètre, bouton « Calibrer »' : 'bouton introuvable', [cal]);
  await note('TG-P2-2.1b', /ICM42688P/.test(t) && /DPS310/.test(t), 'onglet Capteurs : Gyro 1 ICM42688P, Accéléromètre ICM42688P, Baromètre DPS310', [p.locator('#content').getByText('Matériel Capteur').locator('xpath=..')]);
  await onglet('Installation');
  await note('TG-P2-2.4', null, 'modèle 3D : à voir en penchant le vrai drone', [p.locator('#content canvas')]);

  // Partie 3 : Récepteur, Modes, Tableau de bord
  await onglet('Récepteur');
  await note('TG-P2-3.1', null, 'barres des voies : à voir avec la radio', [p.locator('#content').getByText(/Roll|Roulis/).first()]);
  await note('TG-P2-3.2', null, 'valeurs 1000 à 2000 : à voir avec la radio', []);
  await onglet('Modes');
  await note('TG-P2-3.3', /ARM/.test(await texte()), 'liste des modes (ARM, ANGLE…) affichée', [p.locator('#content').getByText(/^ARM$/).first()]);
  await onglet('Tableau de bord'); t = await texte();
  await note('TG-P2-3.4', /RX_FAILSAFE/.test(t), (t.match(/RX_FAILSAFE :[^.]*\./) || ['?'])[0], [txt('RX_FAILSAFE')]);

  // Partie 4.3 / 5 : Moteurs
  await onglet('Moteurs'); t = await texte();
  await note('TG-P2-4.1m', /DSHOT(300|600)/.test(t) && /Protocole ESC\/moteur/.test(t), 'case Fonctionnalités ESC/Moteur : ' + ((t.match(/DSHOT\d+ Protocole ESC\/moteur/) || ['?'])[0]), [p.locator('#content').getByText('Protocole ESC/moteur').locator('xpath=..')]);
  const bidir = p.locator('#content').getByText('DShot bi-directionnel').locator('xpath=ancestor::*[.//button[@role="switch"]][1]');
  const bidirOn = await bidir.locator('button[role=switch]').first().getAttribute('aria-checked').catch(() => null);
  await note('TG-P2-4.2m', bidirOn === 'true', 'interrupteur « DShot bi-directionnel » : ' + bidirOn, [bidir]);
  await note('TG-P2-4.3', /Je comprends les risques/.test(t), 'case « Je comprends les risques » ; la vitesse s\'affiche sous chaque moteur quand il tourne', [txt('Je comprends les risques')]);
  for (let m = 1; m <= 4; m++) await note('TG-P2-5.' + m, null, `moteur ${m} : à voir sur le vrai drone (curseur ${m})`, []);

  // Partie 6
  await note('TG-P2-6.1', null, 'OSD : à voir dans les lunettes', []);
  await onglet('Tableau de bord');
  await note('TG-P2-6.2', null, 'tension : case Drone, ligne Batterie (« Non mesurée » sans batterie)', [txt('Batterie')]);
  await onglet('Alimentation et batterie');
  const etat = p.locator('#content div').filter({ has: p.getByText('Etat alimentation') }).filter({ has: p.getByText('Intensité', { exact: true }) }).last();
  await note('TG-P2-6.2m', null, 'case « Etat alimentation », ligne Tension (batterie branchée)', [etat.getByText('Tension', { exact: true }).locator('xpath=..')]);
  await note('TG-P2-6.3', null, 'case « Etat alimentation », ligne Intensité (batterie branchée, moteurs arrêtés)', [etat.getByText('Intensité', { exact: true }).locator('xpath=..')]);

  // Partie 7 : Protections
  await onglet('Protections');
  const champ = p.locator('#content').getByText('Température limite des ESC', { exact: true }).locator('xpath=ancestor::*[.//input][1]').locator('input').first();
  const bat = /Batterie basse/.test(await texte());
  await champ.fill('80'); await champ.press('Tab'); await p.waitForTimeout(500);
  await photo('TG-P2-7.1a', [champ]);
  await p.getByRole('button', { name: 'Enregistrer et redémarrer' }).click(); await reconnecter();
  await onglet('Protections');
  await note('TG-P2-7.1', (await champ.inputValue()) === '80', `relu après redémarrage : ${await champ.inputValue()}${bat ? ' (ATTENTION : case Batterie basse visible)' : ' ; pas de case Batterie basse, comme prévu'}`, [champ]);
  await onglet('Tableau de bord'); t = await texte();
  await note('TG-P2-7.2', /Protection anti-surchauffe Activé · 80 °C/.test(t), (t.match(/Protection anti-surchauffe \S+( · 80 °C)?/) || ['?'])[0], [p.locator('#content div').filter({ has: p.getByText('Ouvrir les fonctions TrueGlide') }).filter({ has: p.getByText('Relance moteur (désynchronisation)') }).last()]);
  await onglet('Protections'); await champ.fill('0'); await champ.press('Tab'); await p.waitForTimeout(500);
  await p.getByRole('button', { name: 'Enregistrer et redémarrer' }).click(); await reconnecter();
  await onglet('Tableau de bord'); t = await texte();
  await note('TG-P2-7.3', /Protection anti-surchauffe Désactivé/.test(t), (t.match(/Protection anti-surchauffe \S+/) || ['?'])[0], [p.locator('#content div').filter({ has: p.getByText('Ouvrir les fonctions TrueGlide') }).filter({ has: p.getByText('Relance moteur (désynchronisation)') }).last()]);
  await onglet('Modes');
  const sp = p.locator('#content').getByText(/^SPEEDER$/);
  await note('TG-P2-7.4', (await sp.count()) > 0, 'ligne SPEEDER', [sp]);
  await onglet('Caméra');
  await p.locator('#content button[role=combobox]').first().click(); await p.waitForTimeout(800);
  t = (await p.locator('body').innerText()).replace(/\s+/g, ' ');
  const sel = p.getByRole('option').filter({ hasText: /PINIO 1/ }).first();
  await note('TG-P2-7.5', /Sortie commutable/.test(t) && /PINIO 1/.test(t), (t.match(/PINIO 1[^·]{0,25}/) || ['PINIO 1 absent'])[0], [sel]);
  await p.keyboard.press('Escape');

  // Partie 8
  await onglet('Blackbox');
  await note('TG-P2-8.2', (await p.getByText('Effacer la mémoire interne').count()) > 0, 'bouton « Effacer la mémoire interne » ; puis Déconnecter, armer 10 s, désarmer, Connexion', [p.getByText('Effacer la mémoire interne')]);
  await note('TG-P2-8.3', (await p.getByText(/Enregistrer sous/).count()) > 0, 'bouton « Enregistrer sous… », puis onglet Visualiseur Blackbox', [p.getByText(/Enregistrer sous/)]);
  await onglet('Tableau de bord'); t = await texte();
  console.log('VU      armement bloqué tant que connecté : ' + (/MSP :/.test(t) ? 'oui (motif MSP)' : 'non'));
  await photo('TG-P2-8.2b', [txt('MSP :')]);

  // Partie 9
  await onglet('Modes');
  await note('TG-P2-9.1', /ARM/.test(await texte()), 'interrupteurs de l\'onglet Modes après coupure', []);
  await onglet('Ligne de commande \\(CLI\\)');
  const da = await cli('diff all', 5000);
  await note('TG-P2-9.2', /# diff all|batch start|board_name/.test(da), 'diff all affiché', [txt('board_name SPEEDYBEEF405MINI')]);
  await zone().fill('exit'); await zone().press('Enter'); await reconnecter();

  // Avant de commencer, étape 5 : Flasher le Firmware (sans flasher)
  await p.getByText('Déconnecter', { exact: true }).first().click(); await p.waitForTimeout(3000);
  await onglet('Flasher le Firmware');
  await p.getByRole('button', { name: 'Chercher la mise à jour' }).click(); await p.waitForTimeout(6000);
  await p.getByRole('button', { name: 'Télécharger' }).click(); await p.waitForTimeout(15000);
  t = await texte();
  console.log((/téléchargé et vérifié/.test(t) ? 'OK     ' : 'ÉCHEC  ') + 'avant-5 : ' + ((t.match(/Version publiée[^)]*\)/) || ['?'])[0]) + ' ; ' + ((t.match(/[^.]*téléchargé et vérifié/) || ['?'])[0]));
  await photo('avant-5_flasher', [p.getByRole('button', { name: 'Chercher la mise à jour' }), p.getByText('Effacement complet de la puce')]);

  console.log('\nRÉSUMÉ : ' + res.filter(r => r.ok === true).length + ' OK, ' + res.filter(r => r.ok === false).length + ' ÉCHEC, ' + res.filter(r => r.ok === null).length + ' à voir sur le vrai drone ; erreurs de page : ' + err.length);
  fs.writeFileSync(D + 'resultats.json', JSON.stringify(res, null, 1));
  await b.close();
})().catch(async e => { console.log('ARRÊT : ' + e.message.split('\n').slice(0, 3).join(' | ')); try { await globalThis.pp.screenshot({ path: D + 'arret.png' }); } catch (x) {} process.exit(1); });
