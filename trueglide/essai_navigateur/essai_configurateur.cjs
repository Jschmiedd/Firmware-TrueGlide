// essai : TRUEGLIDE CONFIGURATOR (page publiée /test/) relié à la carte simulée WebAssembly, dans la page, sans réseau
const { chromium } = require('playwright'); const fs = require('fs'); const path = require('path');
// usage : CARTE_DIR=obj/navigateur node essai_configurateur.cjs (après essai_node.cjs, qui écrit eeprom_collee.bin)
const BASE = 'https://jschmiedd.github.io/TrueGlide-Configurator/'; const DOSSIER = path.resolve(process.env.CARTE_DIR || 'obj/navigateur'); const D = path.join(DOSSIER, 'captures/'); fs.mkdirSync(D, { recursive: true });
(async () => {
  const b = await chromium.launch(); const p = await b.newPage({ locale: 'fr-FR', viewport: { width: 1400, height: 900 } });
  const err = []; p.on('pageerror', e => err.push(e.message));
  p.on('dialog', d => d.accept());
  await p.route('**/__carte/*', r => { const f = path.join(DOSSIER, path.basename(new URL(r.request().url()).pathname));
    r.fulfill({ body: fs.readFileSync(f), contentType: f.endsWith('.wasm') ? 'application/wasm' : f.endsWith('.js') ? 'text/javascript' : 'application/octet-stream' }); });
  await p.addInitScript(() => {
    // fausse connexion « ws://carte-simulee » : la carte tourne dans un Web Worker de la page (WebAssembly)
    const Vrai = window.WebSocket; let eeprom = null;
    const memoire = async () => { if (eeprom) return eeprom; const r = await fetch('/__carte/eeprom_collee.bin'); return eeprom = new Uint8Array(await r.arrayBuffer()); };
    class CarteSimulee {
      constructor() { this.readyState = 0; this.binaryType = 'blob'; this.demarrer(); }
      async demarrer() {
        const mem = await memoire();
        this.w = new Worker('/__carte/carte_worker.js');
        this.w.onmessage = (e) => { const m = e.data;
          if (m.type === 'sortie' && m.port === 0 && this.readyState === 1 && this.onmessage) this.onmessage({ data: new Blob([m.octets]) });
          else if (m.type === 'prete') setTimeout(() => { this.readyState = 1; this.onopen && this.onopen({}); }, 1200); // démarrage du firmware
          else if (m.type === 'redemarrage') { eeprom = m.eeprom; window.redemarrages = (window.redemarrages || 0) + 1; this.close(); } };
        this.w.postMessage({ type: 'demarrer', eeprom: mem.slice() });
      }
      send(d) { if (this.readyState !== 1) return; const b = new Uint8Array(d.buffer ? d.buffer.slice(d.byteOffset, d.byteOffset + d.byteLength) : d); this.w.postMessage({ type: 'entree', octets: b }, [b.buffer]); }
      close() { if (this.readyState === 3) return; this.readyState = 3; if (this.w) this.w.terminate(); this.onclose && this.onclose({}); }
    }
    window.WebSocket = function (url, proto) { return String(url).startsWith('ws://carte-simulee') ? new CarteSimulee() : new Vrai(url, proto); };
  });
  const photo = async (nom) => { await p.screenshot({ path: D + nom + '.png' }); console.log('photo', nom); };
  const ok = async () => { const b = p.getByText('OK', { exact: true }); for (let i = 0; i < await b.count(); i++) { try { await b.nth(i).click({ timeout: 1500 }); } catch (e) {} } };
  const onglet = async (nom) => { await ok(); await p.keyboard.press('Escape'); await p.locator('a,button').filter({ hasText: new RegExp('^\\s*' + nom + '\\s*$') }).first().click(); await p.waitForTimeout(3000); };
  const connecte = async () => (await p.getByText('Déconnecter', { exact: true }).count()) > 0;
  const brancher = async () => { await ok(); const btn = p.locator('button').filter({ hasText: /Mode Virtuel|ws:\/\/|Choisir un port|Carte hors ligne/ }).first(); await btn.click(); await p.waitForTimeout(800);
    await p.getByRole('menuitem').filter({ hasText: /^\s*Choisir un port\s*$/ }).first().click(); await p.waitForTimeout(800);
    const d = p.locator('[role=dialog] input'); if (await d.count()) { await d.first().fill('ws://carte-simulee'); await p.locator('[role=dialog] button').last().click(); }
    for (let i = 0; i < 30 && !(await connecte()); i++) await p.waitForTimeout(500); await p.waitForTimeout(3000); };
  await p.goto(BASE + 'test/');
  await p.evaluate(() => { localStorage.setItem('expertMode', JSON.stringify({ expertMode: true })); localStorage.setItem('showManualMode', JSON.stringify({ showManualMode: true })); });
  await p.goto(BASE + 'test/'); await p.waitForTimeout(4000);
  const t0 = Date.now(); await brancher(); console.log('connecté :', await connecte(), 'en', Date.now() - t0, 'ms');
  await photo('01_accueil');
  console.log('page : tours de boucle en 1 s :', await p.evaluate(async () => { const t = performance.now(); let n = 0; while (performance.now() - t < 1000) { n++; await new Promise(r => setTimeout(r, 0)); } return n; }));
  for (const [n, o] of [['02_ports', 'Ports'], ['03_configuration', 'Configuration'], ['04_moteurs', 'Moteurs'], ['05_osd', 'OSD'], ['06_pid', 'Réglages de PID']]) { try { await onglet(o); await photo(n); } catch (e) { console.log('onglet', o, ':', e.message.split('\n')[0]); await photo('erreur'); console.log((await p.locator('[role=dialog]').allInnerTexts()).join(' / ').slice(0, 400)); } }
  // un changement dans l'interface, enregistré et relu après redémarrage
  await onglet('Configuration');
  const indice = await p.evaluate(() => [...document.querySelectorAll('#content input')].findIndex(i => i.value === 'KarRmen'));
  const nom = p.locator('#content input').nth(indice);
  let avant = null; try { avant = await nom.inputValue(); await nom.fill('KARRMEN2'); console.log('nom avant :', avant); } catch (e) { console.log('nom :', e.message.split('\n')[0]); }
  await p.locator('#content button:visible').filter({ hasText: /Sauvegarder et Redémarrer|Enregistrer et redémarrer/i }).last().click();
  for (let i = 0; i < 40; i++) { await p.waitForTimeout(500); if (await p.evaluate(() => window.redemarrages || 0)) break; }
  console.log('redémarrages de la carte :', await p.evaluate(() => window.redemarrages || 0));
  await p.waitForTimeout(6000); await photo('06c_apres_save'); console.log('dialogue :', (await p.locator('[role=dialog],[role=alertdialog]').allInnerTexts()).join(' / ').slice(0, 300)); console.log('connecté :', await connecte()); if (!(await connecte())) { await ok(); await p.getByText('Connexion', { exact: true }).first().click(); for (let i = 0; i < 30 && !(await connecte()); i++) await p.waitForTimeout(500); await p.waitForTimeout(3000); if (!(await connecte())) await brancher(); }
  console.log('reconnecté :', await connecte());
  await onglet('Configuration'); console.log('nom après redémarrage :', await p.evaluate(() => [...document.querySelectorAll('#content input')].map(i => i.value).filter(v => /KAR/i.test(v)).join(', ')));
  await photo('07_apres_redemarrage');
  console.log('erreurs de page :', err.slice(0, 5)); await b.close();
})();
