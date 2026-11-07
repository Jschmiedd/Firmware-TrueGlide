// Essai firmware (SITL) : sécurité du test moteurs par MSP.
const net = require('net');
const s = net.connect(5761, '127.0.0.1');
let buf = Buffer.alloc(0); const waiters = [];
s.on('data', d => { buf = Buffer.concat([buf, d]); for (;;) { const i = buf.indexOf('$M>'); if (i < 0 || buf.length < i + 6) break; const n = buf[i+3], code = buf[i+4]; if (buf.length < i + 6 + n) break; const payload = buf.slice(i+5, i+5+n); buf = buf.slice(i+6+n); const w = waiters.findIndex(x => x.code === code); if (w >= 0) waiters.splice(w,1)[0].res(payload); } });
function send(code, data = []) { const n = data.length; let c = n ^ code; for (const b of data) c ^= b; s.write(Buffer.from([36,77,60,n,code,...data,c])); }
function req(code, data = []) { return new Promise((res, rej) => { waiters.push({ code, res }); send(code, data); setTimeout(() => rej(new Error('pas de réponse ' + code)), 2000); }); }
const motors = async () => { const p = await req(104); return [0,1,2,3].map(i => p.readUInt16LE(2*i)); };
const setMotors = v => { const d = []; for (let i = 0; i < 8; i++) d.push(v & 255, v >> 8); return req(214, d); };
const sleep = ms => new Promise(r => setTimeout(r, ms));
(async () => {
  await sleep(500);
  console.log('départ               :', (await motors()).join(' '));
  await setMotors(1500); console.log('après SET 1500       :', (await motors()).join(' '));
  for (let i = 0; i < 10; i++) { await sleep(200); await req(101); } // le logiciel continue de parler (2 s)
  console.log('2 s avec trafic MSP  :', (await motors()).join(' '), '(doit rester 1500 : test non interrompu)');
  await sleep(1500);
  console.log('1,5 s sans trafic    :', (await motors()).join(' '), '(doit revenir à l\'arrêt)');
  await setMotors(1500); console.log('après SET 1500       :', (await motors()).join(' '));
  await req(250); await sleep(300);
  console.log('après EEPROM_WRITE   :', (await motors()).join(' '), '(doit revenir à l\'arrêt)');
  s.end(); process.exit(0);
})().catch(e => { console.log('ERREUR', e.message); process.exit(1); });
