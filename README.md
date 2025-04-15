# SmartFOX 2M – ARDF Beacon Fox (144 MHz)

SmartFOX 2M is a compact, battery powerd, self-contained ARDF (Amateur Radio Direction Finding) fox transmitter for the 2-meter band (144 MHz).  
When ordered, your SmartFOX comes with the **latest firmware pre-installed**.

[SmartFOX 2M – ARDF Beacon Transmitter (144 MHz)](https://shop.rf.guru/products/smartfox-2m-ardf)
---

## 🧠 How it Works

When powered on, the SmartFOX executes a predefined sequence of audio, tones, and cw/morse messages, and actions — all configurable via a simple YAML file.

When plugged into your computer via USB-C, the device shows up as a **USB mass storage drive** labeled `SMARTFOX`.  
Edit the `config.yaml` file using **Notepad** or any other text editor.

---

## 🔋 Power and Status Indicators

- **Battery-powered** with USB-C **charging support**
- **Blue LED** – status indicator (also blinks CW if sending CW messages)
- **Red LED** – charging indicator (lit while charging, off when full)

---

## 🛠 Configuration – `config.yaml`

Here's a breakdown of the editable parameters:

```yaml
# Frequency in MHz
fmFreq: "145.450"

# Frequency offset in Hz
fmOffset: "30000"

# CW (Morse code) speed in WPM
cwSpeed: "75"

# CW tone frequency in Hz
cwFreq: "750"

# Morse message to transmit
cwMessage: "VVV de ON6URE LOCATOR IS JO20cw PWR IS 32mW ANT IS A NAGOYA MINI VERTICAL"

# Sequence of actions (one per line, can include delays and function triggers)
payload: |
  signal
  wait 5
  pwm
  wait 1
  ring
  wait 1
  busy
  wait 1
  congestion
  wait 1
  nokia
  wait 1
  pacman
  wait 1
  doom
  wait 1
  supermario
  wait 1
  babyelephantwalk
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

## 🔁 Firmware Update

To update the firmware:

1. Open `config.yaml`
2. Replace its contents with:
   ```
   firmwareupdate
   ```
3. Save the file
4. **Eject or safely remove** the `SMARTFOX` drive
5. The device will reboot into **UF2 bootloader mode** and appear as a drive named `RPI-RP2`
6. Drag the new `smartfox2m.uf2` file into the `RPI-RP2` drive
7. The firmware will update and reboot automatically  
   ✅ **Your configuration (`config.yaml`) will remain untouched**

---

## 🔄 Restoring Default Configuration

If you delete the `config.yaml` file and eject the `SMARTFOX` drive, the device will automatically regenerate a default config file the next time it boots up.

---

## 📦 Features Summary

- Battery-powered with USB-C charging
- Status and charging LEDs
- Easy configuration via USB (`config.yaml`)
- Transmits CW (Morse) and classic ringtones
- Fully customizable action sequence
- Simple drag-and-drop firmware updates

---

## 📃 License

This project is licensed under the [MIT License](LICENSE).

---

## 🧃 Credits

Developed by [ON6URE – RF.Guru](https://rf.guru)  
Firmware, schematics, and documentation licensed under MIT.
