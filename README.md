
---

````markdown
# 🖥️ NotAMac  
### *A Retro Spotify Display for the Modern Age*

> **“It’s not a Mac — but it absolutely thinks it is.”**

**NotAMac** is a tiny, desk-friendly Spotify “Now Playing” display powered by an **ESP32** and styled after the iconic **Classic Macintosh System 7** interface.  
It brings back window chrome, 3D bevels, menu bars, and that unmistakable 90s GUI — while streaming **live Spotify data** in real time.

If you’ve ever wanted a Macintosh that plays Spotify but fits in the palm of your hand…  
this is it.

---

## 📸 What Is This Thing?

NotAMac is a **standalone Spotify companion display** that shows:

- 🎵 Current track title & artist  
- 🖼️ Live album art (decoded on-device!)
- ⏱️ Track progress bar (smooth + flicker-free)
- 🕰️ Real-time clock synced via NTP  
- 🪟 A lovingly recreated classic Mac UI  

All rendered **entirely in code** — no UI images, no shortcuts.

---

## ✨ Features at a Glance

🖥️ **Authentic Retro UI**
- Macintosh System 7–inspired window chrome
- Platinum color palette
- 3D beveled buttons & progress bars
- Traffic-light window controls (for vibes only)

🎵 **Live Spotify Integration**
- Pulls “Currently Playing” data via Spotify Web API
- Automatic OAuth **access token refresh**
- Detects track changes vs progress updates

🖼️ **Real Album Art Rendering**
- Downloads album art JPEGs on demand
- Decodes and renders them pixel-by-pixel using `TJpg_Decoder`

⚡ **Smart Redraw Logic**
- Full redraw **only when the song changes**
- Progress bar updates independently
- Zero flicker, minimal SPI overhead

🌐 **Rock-Solid Networking**
- WiFi auto-reconnect handling
- Graceful fallback UI when offline

🕰️ **Precision Timekeeping**
- NTP-synced clock displayed in the “menu bar”
- Retro, but accurate 😉

---

## 🧰 Hardware Requirements

### 🔧 Core Components

| Part | Requirement |
|----|----|
| **Microcontroller** | XIAO ESP32-C3 *(required — pinout specific)* |
| **Display** | 1.44" or 1.8" TFT (ST7735 driver) |
| **Interface** | SPI |
| **Power** | USB or 3.3V regulated supply |

---

## 🔌 Display Wiring (SPI)

Wire your ST7735 TFT like this:

| TFT Pin | ESP32-C3 Pin | Code Constant |
|------|------|------|
| **CS** | GPIO 5 | `TFT_CS` |
| **RST** | GPIO 3 | `TFT_RST` |
| **DC / A0** | GPIO 4 | `TFT_DC` |
| **SDA / MOSI** | GPIO 23 | Hardware SPI |
| **SCK** | GPIO 18 | Hardware SPI |
| **VCC** | 3.3V | — |
| **GND** | GND | — |

⚠️ **Important:** This pinout is fixed in the sketch — changing boards will require rewiring or code edits.

---

## 🚀 Getting Started

### 1️⃣ Install Required Libraries

Install these via **Arduino Library Manager**:

- `Adafruit_GFX`
- `Adafruit_ST7735`
- `ArduinoJson`
- `TJpg_Decoder`
- `NTPClient`

---

### 2️⃣ Spotify Setup (The Secret Sauce 🔐)

To let NotAMac talk to Spotify, you’ll need API credentials.

#### 🔑 Steps:
1. Go to the **Spotify Developer Dashboard**
2. Create a new app
3. Copy your:
   - **Client ID**
   - **Client Secret**
4. Generate a **Refresh Token**
   - (Use a helper sketch or web-based token generator)

#### ✍️ Update These Constants in the Sketch:

```cpp
const char* WIFI_SSID     = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

const char* CLIENT_ID     = "YOUR_SPOTIFY_CLIENT_ID";
const char* CLIENT_SECRET = "YOUR_SPOTIFY_CLIENT_SECRET";
const char* REFRESH_TOKEN = "YOUR_LONG_REFRESH_TOKEN";
````

> 🔒 Your refresh token never expires — NotAMac handles access tokens automatically.

---

### 3️⃣ Flash the Firmware

Upload the sketch to your ESP32-C3.

If colors look **wrong or inverted**, try changing the display init mode:

```cpp
tft.initR(INITR_BLACKTAB);
// Try: INITR_GREENTAB, INITR_REDTAB, INITR_MINI160x80
```

Different ST7735 panels use different “tabs”.

---

## 🧠 Under the Hood

### 🎨 UI Rendering Engine

The UI is **100% procedural** — no PNGs, no sprites.

* `fill3DRect()` simulates raised/sunken UI elements
* Light & shadow borders recreate 90s GUI depth
* Colors are defined using authentic RGB565 values:

  * `MAC_PLATINUM`
  * `MAC_CHARCOAL`
  * `MAC_WHITE`

This keeps memory usage low and style consistent.

---

### 🖼️ Album Art Pipeline

Rendering JPEGs on a microcontroller isn’t trivial — here’s how it works:

1. 🎯 Spotify API returns album art URL
2. 📥 JPEG is downloaded into a heap buffer (`malloc`)
3. 🧩 `TJpg_Decoder` decodes it on the fly
4. 🖥️ Pixels are streamed directly to the TFT via callback

No SD card. No frame buffer. Just vibes.

---

### 🔄 Main Loop Logic

Every second, NotAMac:

1. 📶 Checks WiFi status
2. 🔑 Refreshes Spotify access token (if needed)
3. 🎵 Queries “Currently Playing”
4. 🧠 Decides:

   * **Same song?** → Update progress bar only
   * **New song?** → Full UI redraw + new album art

Efficient, responsive, and flicker-free.

---

## 🛠️ Roadmap / Ideas

Want to help push this further? Here’s what’s next:

* [ ] 📡 WiFi Manager (no hardcoded credentials)
* [ ] 👆 Touch support (Play / Pause / Skip)
* [ ] 🎤 Lyrics mode
* [ ] 🌙 Dark Mode (System 7 at night)
* [ ] 🔵 Bluetooth companion app

PRs welcome. Cool ideas encouraged.

---

## 📜 License

**MIT License**

Build it. Remix it. Put it on your desk.
Just don’t claim it’s a real Mac 😉

---

## 💾 Final Words

NotAMac is a love letter to:

* Retro computing
* Embedded graphics
* Doing unnecessary things *beautifully*

If this made you smile, you’re doing it right 🖥️✨

```

