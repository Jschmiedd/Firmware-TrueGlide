// essai : la carte simulée en WebAssembly, pilotée par la mémoire (sans réseau)
const fs = require('fs');
// usage : CARTE_DIR=obj/navigateur node essai_node.cjs fichier_diff.txt
const path = require('path');
const DOSSIER = path.resolve(process.env.CARTE_DIR || 'obj/navigateur');
const TrueGlideCarte = require(path.join(DOSSIER, 'carte.js'));
const w = ms => new Promise(r => setTimeout(r, ms));
async function demarrer(eeprom) {
  let sortie = '', octets = [];
  const M = await TrueGlideCarte({
    print: () => {}, printErr: () => {}, arguments: [],
    preRun: [m => { if (eeprom) m.FS.writeFile('/eeprom.bin', eeprom); }],
    tgSortie: (id, data) => { if (id === 0) { sortie += Buffer.from(data).toString('latin1'); octets.push(...data); } },

  });
  M.envoyer = s => { const b = Buffer.from(s, 'latin1'); const p = M._malloc(b.length); M.HEAPU8.set(b, p); M._tg_entree(0, p, b.length); M._free(p); };
  M.lire = () => { const s = sortie; sortie = ''; return s; };
  M.octets = () => { const o = octets; octets = []; return o; };
  await w(1500);
  return M;
}
async function cmd(M, c) { M.lire(); M.envoyer(c + '\r\n'); for (let i = 0; i < 200; i++) { await w(10); if (/\r?\n# $/.test(sortie_(M))) break; } return M.lire(); }
function sortie_(M) { const s = M.lire(); M._tampon = (M._tampon || '') + s; return M._tampon; }
async function cmd2(M, c) { M._tampon = ''; M.lire(); M.envoyer(c + '\r\n'); for (let i = 0; i < 300; i++) { await w(10); M._tampon += M.lire(); if (/\r?\n# $/.test(M._tampon)) break; } return M._tampon; }
(async () => {
  const t0 = Date.now();
  let M = await demarrer(null);
  // MSP_API_VERSION (1) en MSP v1
  M.octets(); M.envoyer('$M<\x00\x01\x01'); await w(200);
  console.log('MSP_API_VERSION ->', Buffer.from(M.octets()).toString('hex'), '(démarrage', Date.now() - t0, 'ms)');
  M.lire(); M.envoyer('#'); await w(300); M.lire();
  console.log((await cmd2(M, 'version')).replace(/\r/g, '').trim());
  const lignes = fs.readFileSync(process.argv[2], 'utf8').replace(/\r/g, '').split('\n').map(l => l.trim()).filter(l => l && !l.startsWith('#') && !/^(save|exit|defaults|batch|diff|dump)\b/.test(l));
  const ko = [];
  const t1 = Date.now();
  for (const l of lignes) { const r = await cmd2(M, l); const e = r.split(/\r?\n/).find(x => /###ERROR|Invalid|out of range|Unknown|PARSING FAILED/i.test(x)); if (e) ko.push(l + ' -> ' + e); }
  console.log('collage :', lignes.length - ko.length, 'acceptées /', ko.length, 'refusées en', Date.now() - t1, 'ms'); ko.forEach(x => console.log('  ', x));
  M.envoyer('save\r\n'); await w(1500);
  const eeprom = M.FS.readFile('/eeprom.bin'); fs.writeFileSync(path.join(DOSSIER, 'eeprom_collee.bin'), eeprom);
  console.log('eeprom.bin :', eeprom.length, 'octets');
  // redémarrage avec la mémoire gardée
  M = await demarrer(eeprom);
  M.lire(); M.envoyer('#'); await w(300); M.lire();
  const diff = await cmd2(M, 'diff all');
  const lu = new Set(diff.replace(/\r/g, '').split('\n').map(x => x.trim()));
  const manque = lignes.filter(l => !lu.has(l));
  console.log('relues après redémarrage :', lignes.length - manque.length, '/', lignes.length); manque.forEach(x => console.log('   diffère :', x));
  fs.writeFileSync(path.join(DOSSIER, 'diff_relu.txt'), diff);
  process.exit(0);
})();
