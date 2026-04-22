# SmartFOX – ARDF Beacon Firmware (2M + 80M, unified)

SmartFOX is a compact, battery-powered, self-contained ARDF (Amateur Radio Direction Finding) fox transmitter. This firmware supports both **SmartFOX 2M** (144 MHz FM with audio/CW modulation via sidetone) and **SmartFOX 80M** (3.5 MHz HF CW with direct carrier keying) from a single codebase.

At boot the firmware auto-detects the hardware:

- 80M PCB → HF CW mode (Si5351 direct keying, 8 mA drive, no VCXO, no audio playback)
- 2M PCB → VHF FM mode (VCXO modulation, 2 mA drive, sidetone-keyed CW, WAV / RTTTL / MOD playback)

The `config.txt` written on first boot is tailored to the detected band (different default frequency, offset, payload). You can also override detection via a hidden CLI command — see [Developer overrides](#-developer-overrides).

When ordered, your SmartFOX comes with the **latest firmware pre-installed**.

Buy here [SmartFOX 2M – ARDF Beacon Transmitter (144 MHz)](https://shop.rf.guru/products/smartfox-2m-ardf)

Buy here [SmartFOX 80M – ARDF Beacon Transmitter (3.5 MHz)](https://shop.rf.guru/products/smartfox-80m-ardf)

YouTube [Playlist](https://www.youtube.com/watch?v=93If1xDU2ds&list=PL2neu3pWcZxienwiAIDyjRAkZ46nNQhHx&index=1)

---

## 🧠 How it Works

When powered on, the SmartFOX executes a predefined sequence of audio, tones, CW/Morse messages, and actions — all configurable via a simple text file.

When plugged into your computer via USB-C, the device shows up as a **USB mass storage drive** labeled `SMARTFOX`. Edit `config.txt` with any text editor. On 2M hardware you can also drop **WAV files**, **RTTTL ringtone files**, and **ProTracker MOD files** onto the drive — they become playable by name or via random selection.

While the drive is mounted by a host, the firmware pauses TX automatically (`[usb] drive mounted by host — TX paused`) and resumes on eject (`[usb] drive unmounted — TX resumed`). You can still trigger a test run manually from the serial CLI with `payload`.

Every boot the firmware also sweeps and deletes macOS metadata (`.DS_Store`, `._*`, `.Trashes/`, `.fseventsd/`, `.Spotlight-V100/` …) so the drive stays clean and `random *.wav` never accidentally picks up a `._foo.wav` AppleDouble stub.

---

## 🔋 Power and Status Indicators

- **1S 3.7 V LiPo** with USB-C **charging support**
- **Blue LED** status indicator:
  - Fast blink on boot
  - Slow heartbeat when idle
  - Solid during TX
  - Double-blink when battery low
  - Brief blip when outside ARDF slot / run window
  - SOS pattern on fatal error
- **Red LED** – charging indicator (lit while charging, off when full)

The battery watchdog is **debounced** (needs 3 consecutive below-threshold readings before the hard cutoff fires) and **sanity-clamped** (readings outside 1.0–6.0 V are treated as "floating pin / wrong divider" and skipped). Each check prints `[bat] reading: X.XX V` on serial so you can calibrate against a multimeter.

---

## 🛠 Configuration – `config.txt`

All settings live in `/config.txt` on the USB drive. Delete it and eject to regenerate a fresh default for the detected band. If the filesystem itself is corrupted, the firmware prints `formatting (5s to abort via reset)...` on serial before auto-reformatting — giving you a window to pull power if you didn't intend that.

### Default `config.txt` (2M)

```yaml
# SmartFOX 2M — VHF FM beacon (145 MHz band)
# RF
fmFreq: "145.450"
# comma list e.g. "145.450,145.550" enables rotation; empty = no rotation
fmFreqList: ""
fmOffset: "30000"
# 0 = no rotation, else seconds per freq in fmFreqList
freqCycleSec: 0

# CW (sidetone-keyed over FM carrier)
cwWPM: 24
cwFreq: "750"
# CW tone volume 0-100 (% of max). Lower if your receiver crackles.
cwVolume: 100
cwMessage1: "VVV de ON6URE  LOCATOR IS JO20cw  PWR IS 32mW  ANT IS A NAGOYA MINI VERTICAL"
cwMessage2: ""
cwMessage3: ""

# Legal callsign ID watchdog (0 = disabled)
idCallsign: "ON6URE"
idIntervalMin: 10

# ARDF: 0 = continuous TX, 1..5 = which slot in the cycle
ardfSlot: 0
ardfCycleMin: 5

# Schedule (RTC seeded once at boot from currentTime)
runWindow: ""
currentTime: ""

# Battery watchdog (1S 3.7 V LiPo) — set both to 0 to disable.
batLowVolts: 3.5
batCutoffVolts: 3.0
sleepBetweenSlots: true

payload: |
  signal
  cwmsg1
  wait 1
  homing 8
  wait 1
  random *.rtttl
  wait 1
  random *.wav
  wait 1
  cw0
  wait 2
  cw1
  wait 1
  cwmsg1
  nosignal
  wait 20
```

### Default `config.txt` (80M)

```yaml
# SmartFOX 80M — HF CW beacon (3.5 MHz band)
# RF (CW keying = Si5351 output enabled/disabled directly)
fmFreq: "3.580"
fmFreqList: ""
fmOffset: "2000"
freqCycleSec: 0

# CW (carrier is keyed directly on HF — no sidetone / volume settings)
cwWPM: 24
cwMessage1: "VVV de ON6URE  QRP CW BEACON ON 80M"
cwMessage2: ""
cwMessage3: ""

idCallsign: "ON6URE"
idIntervalMin: 10

ardfSlot: 0
ardfCycleMin: 5

runWindow: ""
currentTime: ""

batLowVolts: 3.5
batCutoffVolts: 3.0
sleepBetweenSlots: true

payload: |
  cwmsg1
  wait 5
  id
  wait 20
```

### CW speed — `cwWPM`

Morse speed is specified in **Words Per Minute** (PARIS standard). The firmware converts internally: `dit_ms = 1200 / WPM`.

| WPM | dit (ms) |
| --- | -------- |
| 12  | 100      |
| 18  | 67       |
| 20  | 60       |
| 24  | 50       |
| 30  | 40       |
| 40  | 30       |

### Battery thresholds

Hardware is fixed: VSYS sensed on **GPIO28 (ADC2)** through a **100 kΩ / 100 kΩ** resistive divider (2:1 ratio). Setting either threshold to 0 disables that check — set both to 0 on the bench with no battery.

Standard 1S LiPo thresholds:

- `batLowVolts: 3.5` — CW "LOBAT" alert every check, LED double-blink
- `batCutoffVolts: 3.0` — shuts down CLK0 permanently; device reboots into error state

---

## 🎛 Payload Language

Each line in `payload:` is one step. Lines starting with `#` are comments. Blocks of `repeat <N>` … `end` repeat N times and nest freely.

### Radio control

| Command               | 2M  | 80M | Effect                                                                |
| --------------------- | --- | --- | --------------------------------------------------------------------- |
| `signal` / `nosignal` | ✓   | —   | Enable / disable CLK0 output (VHF only — HF keys the carrier in `cw`) |
| `freq <MHz>`          | ✓   | ✓   | Retune the radio mid-payload, e.g. `freq 3.560`                       |

### Timing

| Command              | Effect                                     |
| -------------------- | ------------------------------------------ |
| `wait <seconds>`     | Pause for N seconds                        |
| `repeat <N>` … `end` | Loop the enclosed block N times (nestable) |

### Audio / tones (2M only — no PWM audio on 80M)

| Command                        | Effect                                          |
| ------------------------------ | ----------------------------------------------- |
| `tone <hz> <ms>`               | Emit a tone, e.g. `tone 800 500`                |
| `homing <seconds>`             | Two-tone fox warbler for final-approach hunting |
| `ring` / `busy` / `congestion` | PSTN-style tones                                |
| `pwm`                          | Legacy — plays `/sample.wav`                    |

On 80M hardware these commands are skipped with `[80M] skipping audio/signal cmd: <cmd>` on serial — so pasting a 2M payload into an 80M drive still runs the CW parts cleanly.

### Files on the USB drive (2M only)

| Command             | Effect                                          |
| ------------------- | ----------------------------------------------- |
| `play <name.wav>`   | Play a 16-bit mono WAV file from the drive      |
| `play <name.rtttl>` | Play an RTTTL ringtone file                     |
| `play <name.mod>`   | Play a ProTracker MOD file                      |
| `random *.wav`      | Pick a random `.wav` from the drive and play it |
| `random *.rtttl`    | Pick a random ringtone file                     |
| `random *.mod`      | Pick a random MOD file                          |

### CW (both bands)

| Command                        | Effect                                                        |
| ------------------------------ | ------------------------------------------------------------- |
| `cw0` / `cw1`                  | Key-up / key-down (manual)                                    |
| `cwmsg1` / `cwmsg2` / `cwmsg3` | Send `cwMessageN`                                             |
| `cwmsg`                        | Alias for `cwmsg1` (back-compat)                              |
| `id`                           | Send `idCallsign` now (also auto-fires every `idIntervalMin`) |

### Telemetry (both bands)

| Command | Effect                                  |
| ------- | --------------------------------------- |
| `bat`   | Speak current battery voltage in CW     |
| `temp`  | Speak RP2040 internal temperature in CW |

---

## 🎵 Ringtones — RTTTL (2M only)

RTTTL (Nokia ringtone format) files are one line of text per tune.

**Huge free library:** [ringtone.vulc.in](https://ringtone.vulc.in) — copy any ringtone's text into a new file named `anything.rtttl`, drop it onto the `SMARTFOX` drive, and reference it with `play anything.rtttl` in your payload or `random *.rtttl`.

Example file (`nokia.rtttl`):

```text
Nokia:d=4,o=5,b=180:8e6,8d6,f#,g#,8c#6,8b,d,e,8b,8a,c#,e,a
```

Both `.rtttl` and `.rtx` (RTX extended dialect with loops and styles) are supported. Parsing is handled by [ArminJo/PlayRtttl](https://github.com/ArminJo/PlayRtttl) so you get full format coverage including flats, German-H, default-tempo/duration/octave, and loop counts.

---

## 🎤 Audio samples — WAV (2M only)

Drop any **16-bit mono WAV** onto the USB drive and play it with `play yourfile.wav`. The firmware streams directly from flash, so file size is only limited by the filesystem partition (1.5 MB by default).

To convert a regular audio file to the right format on macOS:

```bash
ffmpeg -i input.mp3 -ac 1 -ar 44100 -sample_fmt s16 output.wav
```

---

## 🎹 Tracker music — MOD (2M only)

Drop any **ProTracker `.mod`** file onto the USB drive and play it with `play song.mod`. The file is loaded into RAM and rendered at 22050 Hz mono via [micromod](https://github.com/martincameron/micromod), so file size is limited by the free heap (~200 KB practical ceiling on RP2040).

**Where to find MOD files:**

- [The Mod Archive](https://modarchive.org) — tens of thousands of free MODs from the demoscene era.
- [df.lth.se/~fernbom/music/mods](https://www.df.lth.se/~fernbom/music/mods/) — curated classic collection.

A nice reference track that plays cleanly on the SmartFOX is [_"Quest for Fortune" by Hoffman / Triad_](https://modarchive.org/index.php?request=view_by_moduleid&query=61156).

Only plain 4‑channel ProTracker MODs are supported (no XM/S3M/IT, no tremolo/vibrato effects). Update `micromod_set_gain()` in [src/mod.cpp](src/mod.cpp) if a particular track sounds too quiet or distorted (default 256 — near the clipping ceiling; drop to 128 if you hear crunch).

---

## 🔇 Click-free audio

All audio on pin 15 goes through a single PWM slice — tones, CW sidetone, WAV, MOD, pots. Between two audible events the pin is parked at **50 % duty at 100 kHz** (inaudible, same average DC as during a tone), so the FM modulator never sees a step change. The tail click that used to appear at the end of WAV / tone / homing is gone.

On 2M only, the `cwVolume` setting (0–100 %) further reduces CW sidetone amplitude by lowering the PWM duty cycle away from 50 %. Useful if your receiver crackles on the loudest CW transitions — try `cwVolume: 50` first.

---

## 🗣 Serial CLI

While the SmartFOX is connected via USB-CDC, a live command line is available (115 200 baud). Type `help` for the band-aware command list (80M mode hides 2M-only commands). Highlights:

| Command                                             | Effect                                                                  |
| --------------------------------------------------- | ----------------------------------------------------------------------- |
| `help`                                              | Band-aware command listing                                              |
| `status`                                            | Summary (freq, ARDF slot, wall clock, battery, temp, shouldTx)          |
| `ls`                                                | List files on the drive with sizes                                      |
| `freq [MHz]`                                        | Without arg: print current freq. With arg: retune CLK0 live.            |
| `play <file>` / `random <*.ext>` / `tone <hz> <ms>` | Same as payload (carrier auto on/off). **2M only**                      |
| `pots <ring\|busy\|congestion>`                     | PSTN tone for ~5 s. **2M only**                                         |
| `cw <text>`                                         | Send arbitrary CW text                                                  |
| `cwmsg [1\|2\|3]`                                   | Send a stored CW message                                                |
| `msg <1\|2\|3>`                                     | Print the current value of `cwMessageN`                                 |
| `msg <1\|2\|3> <text>`                              | Set `cwMessageN` in RAM (no echo, not persisted)                        |
| `signal` / `nosignal`                               | Toggle carrier manually. **2M only**                                    |
| `id`                                                | Send callsign ID now                                                    |
| `slot <0..5>`                                       | Change ARDF slot live (0 = continuous)                                  |
| `bat` / `temp`                                      | Print sensor readings to serial                                         |
| `time`                                              | Print current UTC wall clock                                            |
| `settime <ts>`                                      | Set wall clock (EPOCH / ISO 8601 / RFC 822/1036/1123/2822/3339)         |
| `payload`                                           | Run the configured payload once (manual trigger while drive is mounted) |
| `formatdisk`                                        | Wipe the filesystem (3 s warning, then reboots — everything gone)       |
| `reboot`                                            | Software reset                                                          |

The CLI echoes what you type and handles backspace, so it works as a real shell. Newline is added automatically after `cw`, `cwmsg`, and `id` so long CW messages don't leave you without a prompt.

---

## 🦊 ARDF Fox-Hunt Mode

Set `ardfSlot` to 1..5 and `ardfCycleMin` (default 5) to run as a proper ARDF fox. The fox transmits for 60 seconds in its assigned slot, then stays silent until the cycle comes around again. Multiple foxes can be placed on the same frequency as long as they use different slots.

For synchronised foxes, seed the wall clock once per fox via `currentTime` in `config.txt` (e.g. `"2026-04-21 09:00:00"`) before deploying. Without a seed, slot timing uses seconds since boot.

---

## ⏰ Run Window

Set `runWindow` to limit TX to certain hours, e.g. `"09:00-17:00"`. Outside the window the fox holds `nosignal`, the LED switches to a "scheduled-off" blip pattern, and only housekeeping tasks run.

---

## 🔁 Firmware Update

**Pre‑built UF2 files are attached to every [GitHub release](https://github.com/Guru-RF/SmartFOX2M/releases).** You don't need to build the firmware yourself — grab `smartfox2m-vX.Y.Z.uf2` from the latest release's **Assets** section.

To flash it:

1. Open `config.txt` on the `SMARTFOX` drive
2. Replace its contents with the single word `firmwareupdate`
3. Save the file
4. **Eject or safely remove** the `SMARTFOX` drive
5. The device will reboot into **UF2 bootloader mode** and appear as a drive named `RPI-RP2`
6. Drag the downloaded `smartfox2m-*.uf2` file onto the `RPI-RP2` drive
7. The firmware will update and reboot automatically
   ✅ Your configuration (`config.txt`) remains untouched

### Building from source (optional)

If you want to build your own firmware, you need [PlatformIO](https://platformio.org/install) (`pip install platformio`):

```bash
pio run -e pico                 # build firmware → .pio/build/pico/firmware.uf2
pio run -e pico -t upload       # build + flash via USB (device must be in UF2 mode)
```

Every push to `main` and every tag is also built automatically via GitHub Actions (`.github/workflows/build.yml`). Pushing a tag like `v1.2.3` triggers an attached UF2 on the matching release.

---

## 🔄 Restoring Default Configuration

Delete `config.txt` from the `SMARTFOX` drive and eject. On next boot the firmware writes a fresh default file for whichever band it detected (2M or 80M). Other files on the drive (WAVs, RTTTLs, MODs) are left alone.

If you want a completely clean start, use `formatdisk` from the serial CLI — it wipes the whole filesystem, then reboots and regenerates the default `config.txt`.

---

## 🛠 Developer overrides

For testing 80M firmware behaviour on 2M hardware (or vice versa), two **hidden** serial-CLI commands let you override the GP17-based band detection. They're not listed in `help`:

| Command                       | Effect                                                                                                                                                                                                                    |
| ----------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `force80`                     | Creates `/force80.flag` on the drive, wipes `config.txt`, reboots. On next boot the band detector returns `BAND_80M` regardless of GP17, so the 80M default config is written (3.580 MHz, HF CW keying, minimal payload). |
| `unforce80` (alias `force2m`) | Removes `/force80.flag`, wipes `config.txt`, reboots. Band then falls back to whatever GP17 reports.                                                                                                                      |

Internally the firmware picks the Si5351 init path and CW keying mechanism purely by the configured `fmFreq` value:

- `fmFreq < 30 MHz` → HF path: `drive_strength 8 mA`, no VCXO setup, CW keying via `si5351.output_enable()` (OOK).
- `fmFreq ≥ 30 MHz` → VHF path: VCXO enabled, `drive_strength 2 mA`, CW keying via pin-15 audio sidetone (FM modulation).

So if you want to force a specific mode without touching GP17 or the force flag, just set the appropriate `fmFreq` in `config.txt` and reboot.

---

## 📦 Features Summary

- One firmware, two bands: auto-detects 2M (VHF FM sidetone CW) vs 80M (HF direct-keyed CW) via GP17
- 1S 3.7 V LiPo with USB-C charging, debounced + sanity-clamped low-battery watchdog
- Blue status LED with distinct patterns for boot / idle / TX / low-bat / off-schedule / error
- USB Mass Storage drive for drop-in `.wav`, `.rtttl`, `.mod`, and config editing
- Auto-cleanup of macOS metadata every boot (no more `.Trashes` filling the drive)
- Live serial CLI with filename-style `ls`, freq query/retune, per-slot CW message editing, wall-clock set, manual payload trigger, and `formatdisk`
- ARDF slot scheduler with UTC-epoch wall clock, optional `runWindow`, and frequency rotation
- Legal callsign ID watchdog injects between payload steps at a configured interval
- Plays WAV audio, RTTTL ringtones (incl. `.rtx` with loops/styles), and ProTracker MOD tracker music
- PSTN-style ring/busy/congestion tones, homing warbler, arbitrary tones
- Nested `repeat`/`end` blocks and `random *.ext` selection in payloads
- Click-free audio engine on pin 15 (single PWM slice, inaudible idle, adjustable `cwVolume`)
- CW speed specified in industry-standard WPM (PARIS), not raw milliseconds
- Simple drag-and-drop firmware updates via UF2 (or CI auto-built UF2 on every release)

---

## 📃 License

This project is licensed under the [MIT License](LICENSE).

---

## 🧃 Credits

Developed by [ON6URE – RF.Guru](https://rf.guru)
Firmware, schematics, and documentation licensed under MIT.
