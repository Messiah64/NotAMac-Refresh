# 🖥️ NotAMac

### A Retro Spotify Display That Thinks It’s a Macintosh

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-ESP32-green.svg)
![Style](https://img.shields.io/badge/style-System%207-lightgrey.svg)

> **“It’s not a Mac — but it absolutely vibes like one.”**

**NotAMac** is a tiny, standalone Spotify “Now Playing” display powered by an **ESP32** and styled after the iconic **Classic Macintosh System 7** interface. It recreates window chrome, 3D bevels, menu bars, and that unmistakable 90s GUI — while pulling **live Spotify data** in real time.

If you’ve ever wanted a Macintosh that plays Spotify and fits in the palm of your hand… this is it.

---

## 📸 What Is NotAMac?

NotAMac is a **desk-friendly Spotify companion display** that shows:

- 🎵 **Current track** title & artist
- 🖼️ **Live album art** (decoded on-device)
- ⏱️ **Track progress bar** (smooth & flicker-free)
- 🕰️ **Real-time clock** synced via NTP
- 🪟 A lovingly recreated **classic Mac UI**

All rendered **entirely in code** — no UI images, no shortcuts.

---

## ✨ Features

**🖥️ Authentic Retro UI**
- Macintosh System 7–inspired window chrome
- Platinum color palette and 1-bit style dithering
- 3D beveled buttons & progress bars
- Traffic-light window controls (for vibes only)

**🎵 Live Spotify Integration**
- Uses Spotify Web API (“Currently Playing”)
- Automatic OAuth **access token refresh**
- Detects track changes vs. progress updates

**🖼️ Real-Time Album Art**
- Downloads album art on demand
- Decodes JPEGs directly on the ESP32 using `TJpg_Decoder`

**⚡ Smart Redraw Logic**
- Full redraw **only when the song changes**
- Progress bar updates independently
- Zero flicker, minimal SPI overhead

**🌐 Robust Networking**
- WiFi auto-reconnect handling
- Graceful offline / reconnect UI states

**🕰️ Precision Time**
- NTP-synced clock in the “menu bar”
- Retro look, modern accuracy

---

## 🧰 Hardware Requirements

### 🔧 Core Components

| Component | Requirement |
|:---|:---|
| **Microcontroller** | XIAO ESP32-C3 *(recommended)* or Standard ESP32 |
| **Display** | 1.44" or 1.8" TFT (ST7735 driver) |
| **Interface** | SPI |
| **Power** | USB or regulated 3.3V |

### 🔌 Display Wiring (SPI)

Wire your ST7735 TFT as follows.

> **⚠️ Note:** This pinout is configured for a standard **ESP32-WROOM**. If you are using the **XIAO ESP32-C3**, please double-check your pin definitions in the code as GPIO numbers may differ.

| TFT Pin | ESP32 Pin | Code Variable |
|:---|:---|:---|
| **CS** | GPIO 5 | `TFT_CS` |
| **RST** | GPIO 3 | `TFT_RST` |
| **DC / A0** | GPIO 4 | `TFT_DC` |
| **SDA / MOSI** | GPIO 23 | Hardware SPI |
| **SCK** | GPIO 18 | Hardware SPI |
| **VCC** | 3.3V | — |
| **GND** | GND | — |

---

## 🚀 Getting Started

### 1️⃣ Install Required Libraries

Install the following via the **Arduino Library Manager**:

* `Adafruit_GFX`
* `Adafruit_ST7735`
* `ArduinoJson`
* `TJpg_Decoder`
* `NTPClient`

### 2️⃣ Spotify Setup (The Secret Sauce 🔐)

To allow NotAMac to access your Spotify account:

1.  Visit the [Spotify Developer Dashboard](https://developer.spotify.com/dashboard/).
2.  Create a new app.
3.  Copy your **Client ID** and **Client Secret**.
4.  Generate a **Refresh Token**. (You can use a "Get Spotify Refresh Token" helper sketch or web tool).

Update these constants at the top of the sketch:

```cpp
const char* WIFI_SSID      = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD  = "YOUR_WIFI_PASSWORD";

const char* CLIENT_ID      = "YOUR_SPOTIFY_CLIENT_ID";
const char* CLIENT_SECRET  = "YOUR_SPOTIFY_CLIENT_SECRET";
const char* REFRESH_TOKEN  = "YOUR_LONG_REFRESH_TOKEN"; ```

🔒 Note: Your refresh token never expires. The code will automatically generate new short-lived access tokens as needed.

3️⃣ Flash the Firmware
Upload the sketch to your ESP32.

If the screen colors look inverted or "static-y", try changing the display initialization line in setup():

C++

tft.initR(INITR_BLACKTAB);
// Alternatives to try: INITR_GREENTAB, INITR_REDTAB, INITR_MINI160x80
🧠 How It Works
🎨 UI Rendering Engine
The UI is fully procedural — no bitmap assets are stored in flash.

fill3DRect() draws raised or sunken UI elements.

Light and dark borders simulate classic 90s depth.

All colors are defined in RGB565 for authenticity and performance.

🖼️ Album Art Pipeline
Rendering JPEGs on a microcontroller is non-trivial. Here is the flow:

Spotify API provides the album art URL.

Download: JPEG is downloaded into a heap buffer (malloc).

Decode: TJpg_Decoder processes the image.

Render: Pixels are streamed directly to the TFT via a callback.

🔄 Main Loop Logic
Every second, NotAMac:

📶 Checks WiFi: Reconnects if dropped.

🔑 Checks Token: Refreshes OAuth token if expired (every 45 mins).

🎵 Checks Song: Queries "Currently Playing".

Same track? → Update progress bar only.

New track? → Full UI redraw + download new album art.

🛠️ Roadmap
[ ] WiFi Manager (No more hardcoded credentials)

[ ] Touch Controls (Play / Pause / Skip via screen tap)

[ ] Lyrics Mode (Karaoke style text)

[ ] Dark Mode (System 7 at night)

[ ] Bluetooth Companion

Pull requests are welcome!

📜 License
MIT License.

Build it. Remix it. Put it on your desk. Just don’t claim it’s a real Mac 😉

<p align="center"> <sub>NotAMac is a love letter to retro computing, embedded graphics, and doing unnecessary things beautifully.</sub>


<sub>If this made you smile, you’re doing it right. 🖥️✨</sub> </p>
