// Fiche TG-P2B (contrôle du baromètre) déroulée dans le vrai logiciel en ligne (/app/), carte F405 Mini en TRUEGLIDE 2026.6.2 (f9fc6d5)
// simulée sur l'ordinateur (TCP 5761, pont WebSocket 5800), mémoire des réglages vide (comme après le flash).
// Pour chaque question qui se voit dans le logiciel : la vérification (OK / ÉCHEC) et une photo de l'écran, ce qu'il
// faut regarder entouré en rouge. Ce qui se voit dans les lunettes est « à voir sur le vrai drone ».
// Usage : node TG-P2B_derouler.cjs <URL> <diff all de test (28 octobre)> <dossier des photos .png>
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
  const photo = async (id, cibles = [], textes = []) => {
    await ok();
    for (const c of cibles) { try { await c.first().scrollIntoViewIfNeeded({ timeout: 2000 }); await c.first().evaluate(e => { e.dataset.tgAvant = e.style.outline; e.style.outline = '4px solid #e00'; e.style.outlineOffset = '2px'; }); } catch (e) {} }
    // textes : un cadre rouge posé sur la dernière occurrence du texte (même s'il est coupé en plusieurs morceaux colorés)
    for (const t of textes) {
      const vu = await p.evaluate((t) => {
        const racine = document.querySelector('#content'); const w = document.createTreeWalker(racine, NodeFilter.SHOW_TEXT);
        const noeuds = []; let tout = ''; let n;
        while ((n = w.nextNode())) { noeuds.push([n, tout.length]); tout += n.nodeValue; }
        const i = tout.lastIndexOf(t); if (i < 0) return false;
        const pos = (k) => { for (let j = noeuds.length - 1; j >= 0; j--) if (noeuds[j][1] <= k) return [noeuds[j][0], k - noeuds[j][1]]; };
        const r = document.createRange(); const [a, ao] = pos(i); const [z, zo] = pos(i + t.length - 1);
        r.setStart(a, ao); r.setEnd(z, zo + 1); a.parentElement.scrollIntoView({ block: 'center' });
        const b = r.getBoundingClientRect(); const d = document.createElement('div'); d.className = 'tg-cadre';
        Object.assign(d.style, { position: 'fixed', left: (b.left - 4) + 'px', top: (b.top - 3) + 'px', width: (b.width + 8) + 'px', height: (b.height + 6) + 'px', border: '3px solid #e00', zIndex: 99999, pointerEvents: 'none' });
        document.body.appendChild(d); return true;
      }, t);
      if (!vu) console.log('        (texte à entourer introuvable : ' + t + ')');
    }
    await p.waitForTimeout(300); await p.screenshot({ path: P + id + '.png' });
    await p.evaluate(() => document.querySelectorAll('.tg-cadre').forEach(e => e.remove()));
    for (const c of cibles) { try { await c.first().evaluate(e => { e.style.outline = e.dataset.tgAvant || ''; }); } catch (e) {} }
  };
  const note = async (id, ok_, detail, cibles, textes) => { res.push({ id, ok: ok_, detail }); console.log((ok_ === null ? 'À VOIR ' : ok_ ? 'OK     ' : 'ÉCHEC  ') + id + ' : ' + detail); if (cibles) await photo(id, cibles, textes); };
  const txt = (s) => p.locator('#content').getByText(s, { exact: false });
  const dernier = (s) => p.locator('#content').getByText(s, { exact: false }).last();
  // CLI : historique effacé avant une commande, pour que sa réponse soit en haut de l'écran
  const vider = async () => { await p.getByRole('button', { name: "Effacer l'historique" }).click().catch(() => {}); await p.waitForTimeout(500); };
  // entoure en rouge la ligne du CLI qui contient ce texte (la dernière) : élément le plus petit qui le contient
  const ligne = (s) => p.locator('#content').locator('xpath=.//*[contains(normalize-space(.), "' + s + '") and not(./*[contains(normalize-space(.), "' + s + '")])]').last();

  await p.goto(URL);
  await p.evaluate(() => { localStorage.clear(); localStorage.setItem('showManualMode', JSON.stringify({ showManualMode: true })); localStorage.setItem('expertMode', JSON.stringify({ expertMode: true })); });
  await p.goto(URL); await p.waitForTimeout(5000); await ok();

  // Avant de commencer, étapes 3 à 5 : connexion (fenêtre Danger), diff all de test collé d'un coup (il finit par save)
  await brancher();
  console.log(((await p.locator('[role=dialog]').filter({ hasText: /Danger/ }).count()) ? 'VU      ' : 'PAS VU  ') + 'fenêtre Danger à la connexion');
  await ok();
  await onglet('Ligne de commande \\(CLI\\)');
  const avant = (await p.locator('#content').innerText()).length;
  // le fichier finit par save : en simulation, ce save est envoyé à part, après la photo (sinon la page est déjà déconnectée)
  await zone().fill(DIFF.replace(/\r/g, '').replace(/\n# save configuration\nsave\s*(#\s*)?$/, '\n')); await zone().press('Enter'); await p.waitForTimeout(20000);
  const sortie = (await p.locator('#content').innerText()).slice(Math.max(0, avant - 50));
  const erreurs = sortie.split('\n').filter(l => /ERROR|Invalid|Unknown command/i.test(l));
  console.log((erreurs.length ? 'ÉCHEC  ' : 'OK     ') + `avant-4 : diff all collé d'un coup, ${erreurs.length} ligne ERROR ${erreurs.slice(0, 3).join(' | ')}`);
  await photo('avant-4_cli', [ligne('restore original battery_profile')]);
  await sauver();
  await onglet('Ligne de commande \\(CLI\\)');
  await cli('set debug_mode = BARO'); await sauver();

  // Partie 1
  await onglet('Tableau de bord'); await p.waitForTimeout(3000);
  let t = await texte();
  const fw = (t.match(/Firmware (TRUEGLIDE [0-9.]+ \([0-9a-f]+\))/) || [])[1] || '?';
  await note('TG-P2B-1.1', fw === `TRUEGLIDE 2026.6.2 (${REVISION})`, `case Firmware : « ${fw} »`, [p.locator('#content div').filter({ has: p.getByText('Chercher la mise à jour') }).filter({ has: p.getByText('TRUEGLIDE 2026.6.2') }).last()]);
  await onglet('Ligne de commande \\(CLI\\)');
  await vider(); const g1 = await cli('get debug_mode'); const v1 = (g1.match(/debug_mode = \S+/) || ['?'])[0];
  await note('TG-P2B-1.2', v1 === 'debug_mode = BARO', v1, [], [v1]);
  await vider(); const st = await cli('status', 3000);
  const baro = (st.match(/BARO:[^\n]*/) || ['?'])[0]; const i2c = (st.match(/I2C=\d+ \(\d+ errors\)/) || ['?'])[0];
  await note('TG-P2B-1.3', /BARO: DPS310/.test(baro) && i2c === 'I2C=1 (0 errors)', `${baro} ; ${i2c} ; ${(st.match(/FLASH:[^\n]*/) || ['?'])[0]}`, [], ['BARO: DPS310', 'I2C=1 (0 errors)']);
  await zone().fill('exit'); await zone().press('Enter'); await reconnecter();

  // Partie 2
  await onglet('Blackbox');
  const effacer = p.getByText('Effacer la mémoire interne');
  await effacer.first().click().catch(() => {}); await p.waitForTimeout(4000); await ok();
  t = await texte();
  const libre = (t.match(/Espace libre\s*[0-9.,]+\s*[kKMG]?B/) || ['?'])[0];
  await note('TG-P2B-2.1', !/Espace utilisé/.test(t) && /Espace libre 8\.0MB/.test(libre), `après « Effacer la mémoire interne » : ${/Espace utilisé/.test(t) ? '« Espace utilisé » affiché' : 'pas d\'« Espace utilisé »'} ; ${libre}`,
    [effacer, p.locator('#content').getByText(/Espace libre/).first().locator('xpath=..').locator('div').first()]);
  await note('TG-P2B-2.2', null, 'altitude OSD pendant 30 s armé : à voir dans les lunettes');
  await note('TG-P2B-2.3', (await p.getByText(/Enregistrer sous/).count()) > 0, 'bouton « Enregistrer sous… » (la carte simulée n\'a pas volé : mémoire vide)', [], ['Enregistrer sous']);

  // Partie 3
  await onglet('Ligne de commande \\(CLI\\)');
  await cli('set debug_mode = NONE'); await sauver();
  await onglet('Ligne de commande \\(CLI\\)');
  await vider(); const g2 = await cli('get debug_mode'); const v2 = (g2.match(/debug_mode = \S+/) || ['?'])[0];
  await note('TG-P2B-3.1', v2 === 'debug_mode = NONE', v2, [], [v2]);
  await vider(); const da = await cli('diff all', 5000);
  await note('TG-P2B-3.2', /board_name SPEEDYBEEF405MINI/.test(da) && /# TRUEGLIDE [^\n]*\(f9fc6d5/.test(da) && !/debug_mode/.test(da),
    'diff all : ' + ((da.match(/# TRUEGLIDE[^\n]*/) || ['?'])[0]) + ' ; debug_mode ' + (/debug_mode/.test(da) ? 'présent' : 'absent'), [ligne('# TRUEGLIDE /')]);
  await zone().fill('exit'); await zone().press('Enter'); await reconnecter();

  console.log('\nRÉSUMÉ : ' + res.filter(r => r.ok === true).length + ' OK, ' + res.filter(r => r.ok === false).length + ' ÉCHEC, ' + res.filter(r => r.ok === null).length + ' à voir sur le vrai drone ; erreurs de page : ' + err.length);
  fs.writeFileSync(P + 'resultats.json', JSON.stringify(res, null, 1));
  await b.close();
})().catch(async e => { console.log('ARRÊT : ' + e.message.split('\n').slice(0, 3).join(' | ')); try { await globalThis.pp.screenshot({ path: P + 'arret.png' }); } catch (x) {} process.exit(1); });
