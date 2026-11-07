// Simule une sauvegarde interrompue : MSP_SET_FEATURE_CONFIG (3D basculé) envoyé, mais PAS d'EEPROM.
const net = require('net'); const s = net.connect(5761, '127.0.0.1');
let buf = Buffer.alloc(0); const waiters = [];
s.on('data', d => { buf = Buffer.concat([buf, d]); for (;;) { const i = buf.indexOf('$M>'); if (i < 0 || buf.length < i + 6) break; const n = buf[i+3], code = buf[i+4]; if (buf.length < i + 6 + n) break; const p = buf.slice(i+5, i+5+n); buf = buf.slice(i+6+n); const w = waiters.findIndex(x => x.code === code); if (w >= 0) waiters.splice(w,1)[0].res(p); } });
const send = (code, data = []) => { let c = data.length ^ code; for (const b of data) c ^= b; s.write(Buffer.from([36,77,60,data.length,code,...data,c])); };
const req = (code, data = []) => new Promise((res, rej) => { waiters.push({ code, res }); send(code, data); setTimeout(() => rej(new Error('pas de réponse ' + code)), 2000); });
(async () => {
  await new Promise(r => setTimeout(r, 300));
  const f = (await req(36)).readUInt32LE(0); const m = f ^ (1 << 12);
  const mot = async () => { const p = await req(104); return [0,1,2,3].map(i => p.readUInt16LE(2*i)).join(' '); };
  console.log('3D enregistré avant :', !!(f & 4096), ' arrêt relu :', await mot());
  await req(37, [m & 255, (m >> 8) & 255, (m >> 16) & 255, (m >>> 24) & 255]);
  console.log('3D en configuration après (sans EEPROM) :', !!(((await req(36)).readUInt32LE(0)) & 4096), ' arrêt relu :', await mot());
  s.end(); process.exit(0);
})();
