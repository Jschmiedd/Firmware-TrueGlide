// essai de bout en bout : réglages préparés sur la carte simulée, envoyés à une autre carte (même configuration de
// départ) par les commandes de dumpDelta (TrueGlide Configurator, src/js/trueglide/offline/delta.js) ; la carte
// d'arrivée doit finir identique à la carte réglée.
// usage : CARTE_DIR=obj/vraie_carte/SPEEDYBEEF405MINI/wasm DELTA=…/delta.js node essai_delta.cjs config_de_depart.txt
const fs = require('fs'); const path = require('path');
const DOSSIER = path.resolve(process.env.CARTE_DIR || 'obj/vraie_carte/SPEEDYBEEF405MINI/wasm');
const TrueGlideCarte = require(path.join(DOSSIER, 'carte.js'));
const w = ms => new Promise(r => setTimeout(r, ms));

async function carte(eeprom) {
    let sortie = '';
    const M = await TrueGlideCarte({ print: () => {}, printErr: () => {}, arguments: [],
        preRun: [m => { if (eeprom) m.FS.writeFile('/eeprom.bin', eeprom); }],
        tgSortie: (id, d) => { if (id === 0) sortie += Buffer.from(d).toString('latin1'); } });
    M.cmd = async (c, ms = 3000) => {
        sortie = ''; const b = Buffer.from(c + '\r\n', 'latin1'); const p = M._malloc(b.length); M.HEAPU8.set(b, p); M._tg_entree(0, p, b.length); M._free(p);
        const t = Date.now(); while (Date.now() - t < ms) { await w(10); if (/\r?\n# $/.test(sortie)) break; } return sortie.replace(/\r/g, '');
    };
    await w(2500); await M.cmd('#', 500);
    return M;
}
async function sauver(M) { await M.cmd('save', 1500); return M.FS.readFile('/eeprom.bin'); }
const refus = r => r.split('\n').find(l => /###ERROR|Invalid|out of range|Unknown|PARSING FAILED/i.test(l));

(async () => {
    const { dumpDelta } = await import(process.env.DELTA);
    const depart = fs.readFileSync(process.argv[2], 'utf8').replace(/\r/g, '').split('\n').map(l => l.trim()).filter(l => l && !l.startsWith('#') && !/^(save|exit|batch|diff|dump)\b/.test(l));
    // carte « vraie » : configuration de départ
    let A = await carte(null); for (const l of depart) { const r = await A.cmd(l); if (refus(r)) console.log('départ refusé :', l); }
    const eepromDepart = await sauver(A);
    A = await carte(eepromDepart); const dumpVraie = await A.cmd('dump all', 20000);
    // réglages faits hors ligne sur la carte simulée
    const reglages = ['set craft_name = KARRMEN2', 'profile 2', 'set p_pitch = 61', 'set d_roll = 40', 'profile 0',
        'rateprofile 3', 'set roll_srate = 55', 'rateprofile 0', 'serial UART3 2 115200 57600 0 115200', 'feature GPS',
        'set osd_vbat_pos = 2113', 'led 0 1,1::C:0', 'aux 5 1 3 1300 1700 0 0', 'set motor_poles = 12', 'resource LED_STRIP 1 NONE'];
    for (const l of reglages) { const r = await A.cmd(l); if (refus(r)) console.log('réglage refusé :', l, '->', refus(r)); }
    const eepromRegle = await sauver(A);
    A = await carte(eepromRegle); const dumpReglee = await A.cmd('dump all', 20000);
    const commandes = dumpDelta(dumpVraie, dumpReglee);
    console.log('commandes à envoyer :', commandes.length); commandes.forEach(c => console.log('   ', c));
    // envoi à une autre carte partie de la même configuration
    let B = await carte(eepromDepart);
    for (const c of commandes) { const r = await B.cmd(c); if (refus(r)) { console.log('ENVOI REFUSÉ :', c, '->', refus(r)); process.exit(1); } }
    const eepromB = await sauver(B);
    B = await carte(eepromB); const dumpB = await B.cmd('dump all', 20000);
    const reste = dumpDelta(dumpB, dumpReglee);
    console.log(reste.length === 0 ? 'carte d\'arrivée IDENTIQUE à la carte réglée (dump all)' : 'DIFFÉRENCES restantes : ' + reste.join(' ; '));
    process.exit(0);
})();
