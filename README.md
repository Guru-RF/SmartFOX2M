# SmartFOX 2M – ARDF Beacon Fox (144 MHz)

SmartFOX 2M is a compact, battery-powered, self-contained ARDF (Amateur Radio Direction Finding) fox transmitter for the 2-meter band (144 MHz).
When ordered, your SmartFOX comes with the **latest firmware pre-installed**.

Buy here [SmartFOX 2M – ARDF Beacon Transmitter (144 MHz)](https://shop.rf.guru/products/smartfox-2m-ardf)

YouTube [YouTube Playlist](https://www.youtube.com/watch?v=93If1xDU2ds&list=PL2neu3pWcZxienwiAIDyjRAkZ46nNQhHx&index=1)

---

## 🧠 How it Works

When powered on, the SmartFOX executes a predefined sequence of audio, tones, CW/Morse messages, and actions — all configurable via a simple text file.

When plugged into your computer via USB-C, the device shows up as a **USB mass storage drive** labeled `SMARTFOX`.
Edit the `config.txt` file using Notepad, TextEdit, or any other text editor. You can also drop **WAV files** and **RTTTL ringtone files** onto the drive; they become playable from the payload.

---

## 🔋 Power and Status Indicators

- **Battery-powered** with USB-C **charging support**
- **Blue LED** – status indicator:
  - Fast blink on boot
  - Slow heartbeat when idle
  - Solid during TX
  - Double-blink when battery low
  - Brief blip when outside ARDF slot / run window
  - SOS pattern on error
- **Red LED** – charging indicator (lit while charging, off when full)

---

## 🛠 Configuration – `config.txt`

All settings live in `/config.txt` on the USB drive. Delete the file and eject to regenerate a fresh default (⚠ this also triggers a FatFS format if the FS is broken — you'll get 5 seconds to reset before it happens).

### Example `config.txt`

```yaml
# RF
fmFreq: "145.450"
# comma list e.g. "145.450,145.550" enables rotation; empty = no rotation
fmFreqList: ""
fmOffset: "30000"
# 0 = no rotation, else seconds per freq in fmFreqList
freqCycleSec: 0

# CW
cwSpeed: "75"
cwFreq: "750"
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
# runWindow empty = always, or e.g. "09:00-17:00"
runWindow: ""
# currentTime "YYYY-MM-DD HH:MM:SS" seeds the wall clock
currentTime: ""

# Battery (VSYS via ADC) - defaults: GPIO28 = ADC2, 3:1 divider
# set batLowVolts and batCutoffVolts to 0 to disable the watchdog
batLowVolts: 6.5
batCutoffVolts: 6.0
batAdcPin: 28
batDivider: 3.0
sleepBetweenSlots: true

# Payload runs top-to-bottom on a loop
payload: |
  signal
  cwmsg
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
  cwmsg
  nosignal
  wait 20
```

---

## 🎛 Payload Language

Each line in `payload:` is one step. Lines starting with `#` are comments.

### Radio control

| Command | Effect |
| --- | --- |
| `signal` | Enable CLK0 output (carrier on) |
| `nosignal` | Disable CLK0 output (carrier off) |
| `freq <MHz>` | Retune the radio mid-payload, e.g. `freq 145.500` |

### Timing

| Command | Effect |
| --- | --- |
| `wait <seconds>` | Pause for N seconds |
| `repeat <N>` … `end` | Loop the enclosed block N times (nestable) |

### Audio / tones

| Command | Effect |
| --- | --- |
| `tone <hz> <ms>` | Emit a tone, e.g. `tone 800 500` |
| `homing <seconds>` | Two-tone fox warbler for final-approach hunting |
| `ring` / `busy` / `congestion` | PSTN-style tones |
| `pwm` | Legacy — plays `/sample.wav` |

### Files on the USB drive

| Command | Effect |
| --- | --- |
| `play <name.wav>` | Play a 16-bit mono WAV file from the drive |
| `play <name.rtttl>` | Play an RTTTL ringtone file |
| `play <name.mod>` | Play a ProTracker MOD file |
| `random *.wav` | Pick a random `.wav` from the drive and play it |
| `random *.rtttl` | Pick a random ringtone file |
| `random *.mod` | Pick a random MOD file |

### CW

| Command | Effect |
| --- | --- |
| `cw0` | Key-up (no tone) |
| `cw1` | Key-down (solid carrier) |
| `cwmsg` / `cwmsg1` | Send `cwMessage1` |
| `cwmsg2` / `cwmsg3` | Send `cwMessage2` / `cwMessage3` |
| `id` | Send `idCallsign` now |

### Telemetry

| Command | Effect |
| --- | --- |
| `bat` | Speak current battery voltage in CW |
| `temp` | Speak RP2040 internal temperature in CW |

---

## 🎵 Ringtones — RTTTL

RTTTL (Nokia ringtone format) files are one line of text per tune.

**Huge free library:** [ringtone.vulc.in](https://ringtone.vulc.in) — copy any ringtone's text into a new file named `anything.rtttl`, drop it onto the `SMARTFOX` drive, and reference it with `play anything.rtttl` in your payload or `random *.rtttl`.

Example file (`nokia.rtttl`):

```text
Nokia:d=4,o=5,b=180:8e6,8d6,f#,g#,8c#6,8b,d,e,8b,8a,c#,e,a
```

Both `.rtttl` and `.rtx` (RTX extended dialect with loops and styles) are supported.

---

## 🎤 Audio samples — WAV

Drop any **16-bit mono WAV** onto the USB drive and play it with `play yourfile.wav`. The firmware streams directly from flash, so file size is only limited by the filesystem partition (1.5 MB by default).

To convert a regular audio file to the right format on macOS:

```bash
ffmpeg -i input.mp3 -ac 1 -ar 44100 -sample_fmt s16 output.wav
```

---

## 🎹 Tracker music — MOD

Drop any **ProTracker `.mod`** file onto the USB drive and play it with `play song.mod`. The file is loaded into RAM and rendered at 22050 Hz mono, so file size is limited by the free heap (~200 KB practical ceiling on RP2040).

**Where to find MOD files:**

- [The Mod Archive](https://modarchive.org) — tens of thousands of free MODs from the demoscene era.
- [df.lth.se/~fernbom/music/mods](https://www.df.lth.se/~fernbom/music/mods/) — curated classic collection.

A nice reference track that plays cleanly on the SmartFOX is [*"Quest for Fortune" by Hoffman / Triad*](https://modarchive.org/index.php?request=view_by_moduleid&query=61156) — download the `.mod`, drop it on the drive, `play questforfortune.mod` (or whatever you name it).

Only plain 4‑channel ProTracker MODs are supported (no XM/S3M/IT, no tremolo/vibrato effects).

Update `micromod_set_gain()` in [src/mod.cpp](src/mod.cpp) if a particular track sounds too quiet or distorted (default 256 — near the clipping ceiling; drop to 128 if you hear crunch).

---

## 🗣 Serial CLI

While the SmartFOX is connected via USB, a live command line is available on the USB-serial port (115200 baud). Type `help` for the full list. Highlights:

| Command | Effect |
| --- | --- |
| `status` | Summary of current state (freq, slot, battery, temperature, TX permitted) |
| `freq <MHz>` | Retune live |
| `play <file>` / `random <*.ext>` / `tone <hz> <ms>` | Same as payload (carrier auto-on/off) |
| `cw <text>` | Send arbitrary CW |
| `cwmsg [1\|2\|3]` | Send a stored CW message |
| `msg <text>` | Set `cwMessage1` in RAM (not persisted) |
| `pots <ring\|busy\|congestion>` | PSTN tones for ~5 seconds |
| `signal` / `nosignal` | Toggle carrier manually |
| `id` | Send callsign ID now |
| `slot <0..5>` | Change ARDF slot live (0 = continuous) |
| `bat` / `temp` | Print sensor readings to serial |
| `reboot` | Software reset |

---

## 🦊 ARDF Fox-Hunt Mode

Set `ardfSlot` to 1..5 and `ardfCycleMin` (default 5) to run as a proper ARDF fox. The fox transmits for 60 seconds in its assigned slot, then stays silent until the cycle comes around again. Multiple foxes can be placed on the same frequency as long as they use different slots.

For synchronised foxes, seed the wall clock once per fox via `currentTime` in `config.txt` (e.g. `"2026-04-21 09:00:00"`) before deploying. Otherwise slot timing uses uptime from boot.

---

## ⏰ Run Window

Set `runWindow` to limit TX to certain hours, e.g. `"09:00-17:00"`. Outside the window the fox holds `nosignal` and only does the housekeeping tasks.

---

## 🔁 Firmware Update

**Pre‑built UF2 files are attached to every [GitHub release](https://github.com/Guru-RF/SmartFOX2M/releases).** You don't need to build the firmware yourself — grab the `smartfox2m-vX.Y.Z.uf2` from the latest release's **Assets** section.

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

Delete `config.txt` from the `SMARTFOX` drive and eject. On next boot the firmware writes a fresh default file. Other files on the drive (WAVs, RTTTLs) are left alone unless the filesystem itself is corrupted — in which case the device prompts with `formatting (5s to abort via reset)...` on serial before wiping.

---

## 📦 Features Summary

- Battery-powered with USB-C charging and low-battery CW alert
- Blue status LED with distinct patterns for each state
- USB Mass Storage drive for drop-in `.wav`, `.rtttl`, and config editing
- Live serial CLI for on-the-fly frequency, message, and sensor control
- ARDF slot scheduler with optional wall-clock run windows and frequency rotation
- Legal callsign ID watchdog
- Plays WAV audio, RTTTL ringtones (incl. `.rtx`), and ProTracker MOD tracker music
- PSTN-style ring/busy/congestion tones, homing warbler, arbitrary tones
- Nested `repeat`/`end` blocks and `random *.ext` selection in payloads
- Simple drag-and-drop firmware updates via UF2

---

## 📃 License

This project is licensed under the [MIT License](LICENSE).

---

## 🧃 Credits

Developed by [ON6URE – RF.Guru](https://rf.guru)
Firmware, schematics, and documentation licensed under MIT.
