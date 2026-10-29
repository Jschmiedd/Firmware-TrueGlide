// Fiche TG-P3 déroulée dans le vrai logiciel en ligne (/app/), carte F405 Mini en TRUEGLIDE 2026.6.2 (f9fc6d5)
// simulée sur l'ordinateur (TCP 5761, pont WebSocket 5800), mémoire des réglages vide (comme après le flash).
// Pour chaque question qui se voit dans le logiciel : la vérification (OK / ÉCHEC) et une photo de l'écran, ce qu'il
// faut regarder entouré en rouge. Les questions de vol sont « à voir sur le vrai drone ».
// Usage : node TG-P3_derouler.cjs <URL> <diff all Betaflight> <dossier des photos .png>
const { chromium } = require('playwright'); const fs = require('fs');
const URL = process.argv[2] || 'https://jschmiedd.github.io/TrueGlide-Configurator/app/';
const DIFF = fs.readFileSync(process.argv[3], 'utf8');
const P = (process.argv[4] || __dirname + '/photos_png') + '/';
const REVISION = 'f9fc6d5';
fs.mkdirSync(P, { recursive: true });
const res = [];
(async () => {
  const b = await chromium.launch(); const p = globalThis.pp = await b.newPage({ viewport: { width: 1400, height: 900 }, locale: 'fr-FR' });
  const err = []; p.on('pageerror', e => err.push(e.message));
  p.on('dialog', d => d.accept());
  const ok = async () => { const fe = p.locator('[role=dialog] button').filter({ hasText: /^\s*(Fermer|OK)\s*$/ }); for (let i = 0; i < await fe.count(); i++) { try { await fe.nth(i).click({ timeout: 800 }); } catch (e) {} } };
  const onglet = async (nom) => { await ok(); await p.locator('a,button').filter({ hasText: new RegExp('^\\s*' + nom + '\\s*$') }).first().click(); await p.waitForTimeout(3500); await ok(); };
  const connecte = async () => (await p.getByText('Déconnecter', { exact: true }).count()) > 0;
  const portBtn = () => p.locator('button').filter({ hasText: /Mode virtuel|Sélection manuelle|ws:\/\/|Choisir un port|Virtual Mode/i }).first();
  const brancher = async () => { await ok(); if (await connecte()) return; await portBtn().click(); await p.waitForTimeout(700); await p.getByRole('menuitem').filter({ hasText: /^\s*Choisir un port\s*$/ }).first().click(); await p.waitForTimeout(700);
    const d = p.locator('[role=dialog] input'); if (await d.count()) { await d.first().fill('ws://localhost:5800'); await p.locator('[role=dialog] button').last().click(); }
    for (let i = 0; i < 40 && !(await connecte()); i++) await p.waitForTimeout(500); await p.waitForTimeout(5000); };
  // après save ou exit la carte redémarre ; si la page n'a pas vu la coupure, « Déconnecter » comme le ferait Johann
  const reconnecter = async () => { for (let i = 0; i < 16 && (await connecte()); i++) await p.waitForTimeout(500);
    if (await connecte()) { await p.getByText('Déconnecter', { exact: true }).first().click().catch(() => {}); for (let i = 0; i < 20 && (await connecte()); i++) await p.waitForTimeout(500); }
    await p.waitForTimeout(3000);
    for (let essai = 0; essai < 4 && !(await connecte()); essai++) { await p.keyboard.press('Escape'); await p.waitForTimeout(2000); await brancher().catch(e => console.log('reconnexion : ' + e.message.split('\n')[0])); }
    if (!(await connecte())) throw new Error('carte simulée non reconnectée'); await ok(); };
  const texte = async () => (await p.locator('#content').innerText()).replace(/\s+/g, ' ');
  const zone = () => p.locator('textarea[name=commands]');
  const cli = async (cmd, attente = 2500) => { await zone().fill(cmd); await zone().press('Enter'); await p.waitForTimeout(attente); const t = await p.locator('#content').innerText(); return t.slice(t.lastIndexOf(cmd)); };
  const sauver = async () => { await zone().fill('save'); await zone().press('Enter'); await reconnecter(); };
  // photo : ce qu'il faut regarder entouré en rouge, puis l'entourage retiré
  const photo = async (id, cibles = []) => {
    await ok();
    for (const c of cibles) { try { await c.first().scrollIntoViewIfNeeded({ timeout: 2000 }); await c.first().evaluate(e => { e.dataset.tgAvant = e.style.outline; e.style.outline = '4px solid #e00'; e.style.outlineOffset = '2px'; }); } catch (e) {} }
    await p.waitForTimeout(300); await p.screenshot({ path: P + id + '.png' });
    for (const c of cibles) { try { await c.first().evaluate(e => { e.style.outline = e.dataset.tgAvant || ''; }); } catch (e) {} }
  };
  const note = async (id, ok_, detail, cibles) => { res.push({ id, ok: ok_, detail }); console.log((ok_ === null ? 'À VOIR ' : ok_ ? 'OK     ' : 'ÉCHEC  ') + id + ' : ' + detail); if (cibles) await photo(id, cibles); };
  const txt = (s) => p.locator('#content').getByText(s, { exact: false });
  const dernier = (s) => p.locator('#content').getByText(s, { exact: false }).last();

  await p.goto(URL);
  await p.evaluate(() => { localStorage.clear(); localStorage.setItem('showManualMode', JSON.stringify({ showManualMode: true })); localStorage.setItem('expertMode', JSON.stringify({ expertMode: true })); });
  await p.goto(URL); await p.waitForTimeout(5000); await ok();

  // Avant de commencer, étapes 3 à 5 : connexion (fenêtre Danger), diff all collé d'un coup, Ctrl+F « ERROR », feature -LED_STRIP, save
  await brancher();
  console.log(((await p.locator('[role=dialog]').filter({ hasText: /Danger/ }).count()) ? 'VU      ' : 'PAS VU  ') + 'fenêtre Danger à la connexion');
  await ok();
  await onglet('Ligne de commande \\(CLI\\)');
  const avant = (await p.locator('#content').innerText()).length;
  await zone().fill(DIFF); await zone().press('Enter'); await p.waitForTimeout(25000);
  const sortie = (await p.locator('#content').innerText()).slice(Math.max(0, avant - 50));
  const erreurs = sortie.split('\n').filter(l => /ERROR|Invalid|Unknown command/i.test(l));
  await photo('avant-4_cli', [dernier('# restore original')]);
  await note('TG-P3-1.3', erreurs.length === 0, `diff all collé d'un coup : ${erreurs.length} ligne ERROR ${erreurs.slice(0, 3).join(' | ')}`);
  const led = await cli('feature -LED_STRIP');
  console.log((/Disabled LED_STRIP|LED_STRIP/i.test(led) ? 'OK     ' : 'ÉCHEC  ') + 'feature -LED_STRIP : ' + led.replace(/\s+/g, ' ').slice(0, 80));
  await sauver();

  // Partie 1 : Tableau de bord
  await onglet('Tableau de bord'); await p.waitForTimeout(3000);
  let t = await texte();
  const fw = (t.match(/Firmware (TRUEGLIDE [0-9.]+ \([0-9a-f]+\))/) || [])[1] || '?';
  await note('TG-P3-1.1', fw === `TRUEGLIDE 2026.6.2 (${REVISION})`, `case Firmware : « ${fw} »`, [p.locator('#content div').filter({ has: p.getByText('Chercher la mise à jour') }).filter({ has: p.getByText('TRUEGLIDE 2026.6.2') }).last()]);
  const f = (t.match(/Fonctions TrueGlide(.{0,250})/) || [])[1] || '';
  await note('TG-P3-1.2', /Protection anti-surchauffe Désactivé/.test(f) && /Relance moteur \(désynchronisation\) Désactivé/.test(f) && /Sorties commandées \(PINIO\) Désactivé/.test(f), f.slice(0, 200).trim(),
    [p.locator('#content div').filter({ has: p.getByText('Ouvrir les fonctions TrueGlide') }).filter({ has: p.getByText('Relance moteur (désynchronisation)') }).last()]);

  // Partie 2 : debug_mode = BARO, Blackbox, debug_mode = NONE
  await onglet('Ligne de commande \\(CLI\\)');
  await cli('set debug_mode = BARO'); await sauver();
  await onglet('Ligne de commande \\(CLI\\)');
  const g1 = await cli('get debug_mode'); const v1 = (g1.match(/debug_mode = \S+/) || ['?'])[0];
  await note('TG-P3-2.1', v1 === 'debug_mode = BARO', v1, [dernier(v1)]);
  const st = await cli('status', 3000);
  const flash = (st.match(/FLASH:[^\n]*/) || ['?'])[0];
  await note('TG-P3-2.2', null, 'altitude OSD pendant 30 s armé : à voir dans les lunettes');
  await zone().fill('exit'); await zone().press('Enter'); await reconnecter();
  await onglet('Blackbox');
  await note('TG-P3-2.3', (await p.getByText('Effacer la mémoire interne').count()) > 0 && (await p.getByText(/Enregistrer sous/).count()) > 0,
    'boutons « Effacer la mémoire interne » et « Enregistrer sous… »', [p.getByText('Effacer la mémoire interne'), p.getByText(/Enregistrer sous/)]);
  await onglet('Ligne de commande \\(CLI\\)');
  await cli('set debug_mode = NONE'); await sauver();
  await onglet('Ligne de commande \\(CLI\\)');
  const g2 = await cli('get debug_mode'); const v2 = (g2.match(/debug_mode = \S+/) || ['?'])[0];
  await note('TG-P3-2.4', v2 === 'debug_mode = NONE', v2, [dernier(v2)]);
  // TG-P2-8.1 : la carte simulée a maintenant la mémoire de la vraie carte (8 Mo)
  await cli('status', 3000);
  await note('TG-P2-8.1', /FLASH: JEDEC ID=0x00ef4017 8M/.test(flash), 'status : ' + flash, [dernier('FLASH: JEDEC')]);
  await zone().fill('exit'); await zone().press('Enter'); await reconnecter();

  // Parties 3 à 5 : vrai matériel
  for (const id of ['3.1', '3.2', '4.1', '4.2', '4.3', '4.4', '4.5', '5.1', '5.2', '5.3', '5.4', '5.5']) await note('TG-P3-' + id, null, 'à voir sur le vrai drone');

  // Partie 6
  await onglet('Blackbox');
  await note('TG-P3-6.1', (await p.getByText(/Enregistrer sous/).count()) > 0, 'barre de la mémoire (vide sur la carte simulée) et « Enregistrer sous… »',
    [p.locator('#content').getByText(/Espace (utilisé|libre)/).first().locator('xpath=../..'), p.getByText(/Enregistrer sous/)]);
  await onglet('Visualiseur Blackbox'); t = await texte();
  const ouvrir = p.locator('#content').getByText(/Open log file|Ouvrir/i).first();
  await note('TG-P3-6.2', (await ouvrir.count()) > 0, 'bouton pour ouvrir le fichier du journal : ' + ((await ouvrir.count()) ? (await ouvrir.innerText()).trim() : 'introuvable'), [ouvrir]);
  await onglet('Ligne de commande \\(CLI\\)');
  const da = await cli('diff all', 5000);
  await note('TG-P3-6.3', /board_name SPEEDYBEEF405MINI/.test(da) && /# TRUEGLIDE .*\(f9fc6d5/.test(da) && !/^feature LED_STRIP/m.test(da) && !/debug_mode/.test(da),
    'diff all : ' + ((da.match(/# TRUEGLIDE[^\n]*/) || ['?'])[0]) + ' ; LED_STRIP ' + (/^feature LED_STRIP/m.test(da) ? 'activé' : 'désactivé') + ' ; debug_mode ' + (/debug_mode/.test(da) ? 'présent' : 'absent'),
    [dernier('board_name SPEEDYBEEF405MINI')]);
  await zone().fill('exit'); await zone().press('Enter'); await reconnecter();

  console.log('\nRÉSUMÉ : ' + res.filter(r => r.ok === true).length + ' OK, ' + res.filter(r => r.ok === false).length + ' ÉCHEC, ' + res.filter(r => r.ok === null).length + ' à voir sur le vrai drone ; erreurs de page : ' + err.length);
  fs.writeFileSync(P + 'resultats.json', JSON.stringify(res, null, 1));
  await b.close();
})().catch(async e => { console.log('ARRÊT : ' + e.message.split('\n').slice(0, 3).join(' | ')); try { await globalThis.pp.screenshot({ path: P + 'arret.png' }); } catch (x) {} process.exit(1); });
