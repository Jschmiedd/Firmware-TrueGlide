# Signalements à Betaflight : brouillons du 13/11/2026 (NE PAS ENVOYER avant OUI de l'audit et OUI écrit de Johann)

Vérifiés le 13/11 sur Betaflight 2026.6.2 (firmware `e0b7bb01b`, App `091127c`) ET sur master (firmware et App, 13/11).
Recherche de doublons faite (issues et PR) : aucun signalement existant pour ces 5 points. À noter : la PR #14268
(« Maintain state for i2c_ev_handler() for each i2c bus ») a rendu l'état par bus, sans le remettre à zéro après
une erreur.

Envoi : par Johann, depuis son compte GitHub, un signalement par point. Dépôts :
- 1, 2, 3 : `betaflight/betaflight-configurator` (issue « Bug report »)
- 4, 5 : `betaflight/betaflight` (issue « Bug report »)

Pièces jointes à ajouter par Johann : vidéos d'écran du 13/11 (1, 2), vidéo téléphone avec son (2), vidéos A et B (5).

---

## 1. Motors tab: mouse wheel changes a motor value when no slider is selected (betaflight-configurator)

**Title:** Motors tab: scrolling the mouse wheel over a motor column spins that motor up, without the slider being selected

**Describe the bug**
With motor testing enabled, turning the mouse wheel anywhere over a motor's column (not only on the slider thumb, and
without having clicked the slider) changes that motor's value by +25 per wheel event. On a mouse that sends several
wheel events per notch, a few notches took the motor from 1100 to 1775 in a fraction of a second (the limit is the slider
maximum). A user
who only wants to scroll the page can spin a motor up.

**To reproduce** (props off)
1. Motors tab, enable motor testing.
2. Drag motor 1 to ~1100, then click an empty area of the page (no slider focused).
3. Hover over motor 1's column (not the thumb) and scroll up 1–3 notches.

**Expected:** nothing changes unless the slider was deliberately selected.
**Actual:** motor 1 jumps (seen: 1100 → 1625 → 1775). On master, scrolling the page is also blocked over the whole
motor test area, so the wheel can only change motors there.

**Code:** `src/components/tabs/MotorsTab.vue` (2026.6.2: `@wheel.prevent` on each grid `<li>`, lines 414 and 429;
`onSliderWheel` line 1548) — the handler only checks `motorsTestingEnabled`, not that the slider has focus. Same
logic on master (`onSliderWheel`, line 1634).

**Suggested fix:** act on the wheel only when the slider inside the cell has focus
(`event.currentTarget.contains(document.activeElement)`), otherwise let the page scroll.

**Evidence:** screen recording + phone video, SpeedyBee F405 Mini, Betaflight 2026.6.2 firmware, App 2026.6.2.

**Environment:** Betaflight App 2026.6.2 (091127c), Windows 11, Chrome; firmware Betaflight 2026.6.2, SPEEDYBEEF405MINI.

---

## 2. Motors tab: motor test keeps running when the page/window is hidden (betaflight-configurator)

**Title:** Motors tab: motors keep spinning when the tab is hidden (switch tab / minimise window)

**Describe the bug**
With motor testing active, switching to another browser tab (or minimising the app window) leaves the motors spinning
at the last commanded value. Nothing stops the test when the page is hidden, and the firmware keeps the last
`MSP_SET_MOTOR` value with no timeout, so the motors run unattended while the user is looking elsewhere.

**To reproduce** (props off)
1. Motors tab, enable motor testing, master slider ~1140.
2. Click another browser tab with the mouse (mouse only: most keys already stop the test). Wait 5 s.
3. Come back.

**Expected:** the test stops (stop value sent) when the page becomes hidden.
**Actual:** motors run the whole time; the test is still active on return.

**Code:** no `visibilitychange` handling in `src/composables/motors/useMotorTesting.js` (2026.6.2) /
`useMotorTesting.ts` (master), nor in `MotorOutputReorderingDialog.vue` / `EscDshotDirectionDialog.vue`. Firmware:
`MSP_SET_MOTOR` writes `motor_disarmed[]` with no timeout (`src/main/msp/msp.c`).

**Suggested fix:** on `document.visibilitychange` with `document.hidden`, disable motor testing (sends the stop
value) and close the motor dialogs.

**Evidence:** screen recording + phone video with sound (motors audible during the 5 s the other tab is shown).

**Environment:** as in report 1.

---

## 3. Power tab / battery state: current read as unsigned (betaflight-configurator)

**Title:** MSP_BATTERY_STATE amperage parsed as unsigned: −0.02 A is shown as 655.34 A

**Describe the bug**
The firmware sends `MSP_BATTERY_STATE` amperage as a signed 16-bit value (0.01 A steps, −320 A to 320 A), but the App
reads it as unsigned. A slightly negative current at rest (sensor offset) is displayed as ~655 A.

**Code:** firmware `src/main/msp/msp.c` line 852:
`sbufWriteU16(dst, (int16_t)constrain(getAmperage(), -0x8000, 0x7FFF));`.
App `src/js/msp/MSPHelper.js` (2026.6.2) line 413 / `MSPHelper.ts` (master) line 1076:
`batteryState.amperage = data.readU16() / 100;` — while `MSP_ANALOG` (line 378 / 1035) correctly uses `read16()`.

**Suggested fix:** `data.read16() / 100` for `MSP_BATTERY_STATE` amperage.

**Evidence:** seen on a SpeedyBee F405 Mini at rest (−0.02 A → 655.34 A).

---

## 4. Gimbal (Caddx GM3) passthrough sends 1 instead of the header byte (betaflight)

**Title:** gimbal_control.c: `GIMBAL_CMD_S && 0xff` writes 1 instead of the first header byte

**Describe the bug**
In `src/main/io/gimbal_control.c` (2026.6.2 line 281, master line 282), when a first header byte is not followed by
the second one, the parser forwards the bytes to the gimbal but writes `GIMBAL_CMD_S && 0xff`, a logical AND that
evaluates to 1, instead of the header byte.

**Suggested fix:** `serialWrite(gimbalSerialPort, GIMBAL_CMD_S & 0xff);`

**Evidence:** code review; checked in a simulator (SITL). Not tested with a physical GM3.

---

## 5. STM32F4 I2C: state of an aborted transfer leaks into the next one — next read skips the register address (betaflight)

**Title:** F4 I2C driver: after a bus error the next read skips the register address and returns wrong data (DPS310: absurd pressure)

**Describe the bug**
In `src/platform/STM32/bus_i2c_stm32f4xx.c`, `i2c_er_handler()` aborts the current job on AF/BERR/ARLO and sets
`state->busy = 0` (line 320) but does not reset `i2c_ev_state[device]`. `subaddress_sent` stays 1 when the error hits
during the data phase of a register read. On the next transfer, the START handler (line 336:
`if (state->reading && (ev_state->subaddress_sent || 0xFF == state->reg))`) sends the address in receive mode directly,
without writing the register address, so the device returns data from wherever its internal pointer is. `i2cInit()`
only clears `pDev->state` (line 453), not `i2c_ev_state`, so even a reinit does not clear it. One wrong read follows
each aborted read. Barometer code ignores the error flag (`busBusy(..., NULL)`), so the wrong data is used.

**Observed:** on a SpeedyBee F405 Mini (DPS310 on I2C1), with a Betaflight 2026.6.2-based build (same I2C driver), occasional readings of −7540 hPa / 258 °C (pressure and
temperature wrong together, i.e. bytes read from the wrong registers), altitude jumps of hundreds/thousands of metres.

**Reproduction (test build, not for flight):** Betaflight 2026.6.2 + a test-only patch that aborts a 6-byte read after
its 3rd data byte every 5 s, like the AF/BERR branch of `i2c_er_handler()` (STOP, interrupts off, `error = true`,
`busy = 0`), with ACK turned off before STOP so that the bus is released cleanly.
- Build A (no fix): the debug BARO pressure value drops below −10000 every ~5 s; `status` shows 0 I2C errors (bus not stalled).
- Build B (same + fix): pressure stable at 961 hPa, temperature 18 °C.
Patch and videos attached.

**Suggested fix:** reset the event state when a job is aborted and on init:
```c
// i2c_er_handler(), before state->busy = 0;
memset(&i2c_ev_state[device], 0, sizeof(i2c_ev_state[device]));
// i2cInit(), after memset(&pDev->state, ...)
memset(&i2c_ev_state[device], 0, sizeof(i2c_ev_state[device]));
```
Related reports:
- #15587: same board as here (SPEEDYBEEF405MINI, DPS310), baro jumping to −6000 m then 600 m, also at rest in the
  sensors tab; fine on 2025.12.5. Closed as a duplicate of #15533 and "Fixed in #15538". But this board's default is
  `dshot_bitbang = OFF` (`SPEEDYBEEF405MINI/config.h` line 98), and #15538 only changes the bitbang code, so with the
  default setting it does not run on this board.
- #15533: bisected to the DShot bitbang change 7f2ce39 (FLYWOOF405S_AIO, then a SpeedyBee F405 AIO). A later comment
  there reports a SpeedyBee F405 V4 (DPS310 on I2C1, board default bitbang OFF) on 2026.6.2: baro values go wrong as
  soon as DShot300 / bidirectional DShot is enabled; with bitbang ON there are still occasional spikes (baro
  temperature to 4000, pressure down to 300).
- #15824: NYX405V1 (DPS368 on I2C1, board default bitbang OFF), random altitude spikes on 2026.6.2, worse with
  bidirectional DShot and ESC_SENSOR. Its reporter could not reproduce on master and closed it as a duplicate of
  #15533.

Pressure and temperature wrong together is what this driver issue can produce (one 6-byte read taken from the
wrong registers). Master may have removed what triggers
the I2C errors, but the driver code that runs after an error is unchanged on master (same lines as above). Whatever
causes the I2C error, this report is about what the driver does after one.

---

## Texte commun à mettre en fin de chaque signalement

> Found while developing TRUEGLIDE, a firmware and configurator derived from Betaflight. Analysis and this report
> were prepared with the help of an AI assistant; the bench tests and videos are mine.
