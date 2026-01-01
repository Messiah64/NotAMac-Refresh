<p align="center">
  <img src="https://img.shields.io/badge/ESP32--C3-XIAO-green?style=for-the-badge&logo=espressif" alt="ESP32-C3"/>
  <img src="https://img.shields.io/badge/Spotify-API-1DB954?style=for-the-badge&logo=spotify&logoColor=white" alt="Spotify"/>
  <img src="https://img.shields.io/badge/Retro-Mac%20Aesthetic-gray?style=for-the-badge&logo=apple&logoColor=white" alt="Retro Mac"/>
  <img src="https://img.shields.io/badge/License-MIT-blue?style=for-the-badge" alt="License"/>
</p>

<h1 align="center">
  🖥️ NotAMac
</h1>

<p align="center">
  <strong>A tiny Spotify Now Playing display disguised as a vintage Macintosh</strong>
</p>

<p align="center">
  <em>Because your music deserves a retro stage ✨</em>
</p>

<p align="center">
  <img src="docs/notamac-hero.jpg" alt="NotAMac Display" width="400"/>
</p>

---

## 🎵 What is NotAMac?

**NotAMac** is a palm-sized Spotify display that lives inside a 3D-printed classic Macintosh case. It shows your currently playing track with album art, song info, and a progress bar—all wrapped in a nostalgic System 7 aesthetic.

Built with the **Seeed Studio XIAO ESP32-C3** and a tiny **ST7735 TFT display**, it's the perfect desk companion for music lovers and retro computing enthusiasts alike.

```
┌────────────────────────────────────┐
│ 🔴 🟡 🟢                     12:34 │
├────────────────────────────────────┤
│  ┌──────────┐   Song               │
│  │          │   Bohemian Rh..      │
│  │  ALBUM   │                      │
│  │   ART    │   Artist             │
│  │          │   Queen              │
│  └──────────┘   Album              │
│                 A Night at t..     │
│  ════════════════════════════════  │
│  1:23                        5:55  │
└────────────────────────────────────┘
```

---

## ✨ Features

| Feature | Description |
|---------|-------------|
| 🎨 **Retro Mac UI** | Classic System 7 aesthetic with traffic light buttons and platinum gray palette |
| 🖼️ **Album Art** | Displays 68×68 pixel album artwork for the current track |
| 📡 **Real-time Updates** | Polls Spotify every 2 seconds for seamless track changes |
| 🕐 **NTP Clock** | Shows current time synced via internet (configurable timezone) |
| 📊 **Progress Bar** | Visual playback progress with elapsed/total time |
| 💾 **Memory Optimized** | Carefully tuned for ESP32-C3's limited RAM (~200KB) |
| 🔄 **Auto-Recovery** | Automatically reconnects WiFi and refreshes tokens |
| 😴 **Idle Screen** | Beautiful "Play a song on Spotify" screen when nothing's playing |

---

## 🛠️ Hardware Required

| Component | Specification | Notes |
|-----------|---------------|-------|
| **Microcontroller** | [Seeed Studio XIAO ESP32-C3](https://www.seeedstudio.com/Seeed-XIAO-ESP32C3-p-5431.html) | Tiny, WiFi-enabled, USB-C |
| **Display** | ST7735 1.8" TFT (160×128) | SPI interface, 65K colors |
| **3D Printed Case** | Classic Macintosh design | See [Printing](#-3d-printing) section |
| **USB-C Cable** | For power and programming | Data-capable cable required |

### 💰 Total Cost: ~$15-20 USD

---

## 📌 Wiring Diagram

Connect the ST7735 display to the XIAO ESP32-C3:

```
ST7735 Display          XIAO ESP32-C3
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    VCC  ─────────────►  3.3V
    GND  ─────────────►  GND
    CS   ─────────────►  D3 (GPIO5)
    RST  ─────────────►  D1 (GPIO3)
    DC   ─────────────►  D2 (GPIO4)
    SDA  ─────────────►  D10 (GPIO10/MOSI)
    SCL  ─────────────►  D8 (GPIO8/SCK)
    BL   ─────────────►  3.3V (or PWM pin for brightness control)
```

### 📷 Wiring Photo

<p align="center">
  <img src="docs/wiring.jpg" alt="Wiring Diagram" width="500"/>
</p>

---

## 🚀 Getting Started

### 1️⃣ Prerequisites

- [Arduino IDE 2.x or 3.x](https://www.arduino.cc/en/software) or [PlatformIO](https://platformio.org/)
- ESP32 Board Support Package installed
- A Spotify Premium account (required for API access)

### 2️⃣ Install Required Libraries

In Arduino IDE, go to **Sketch → Include Library → Manage Libraries** and install:

| Library | Author | Version |
|---------|--------|---------|
| `Adafruit GFX Library` | Adafruit | Latest |
| `Adafruit ST7735 and ST7789 Library` | Adafruit | Latest |
| `ArduinoJson` | Benoît Blanchon | 6.x |
| `TJpg_Decoder` | Bodmer | Latest |
| `NTPClient` | Fabrice Weinberg | Latest |

### 3️⃣ Set Up Spotify API Credentials

1. Go to the [Spotify Developer Dashboard](https://developer.spotify.com/dashboard)
2. Click **Create App**
3. Fill in the details:
   - App Name: `NotAMac`
   - Redirect URI: `http://localhost:8888/callback`
4. Note your **Client ID** and **Client Secret**
5. Get your **Refresh Token** using [this guide](https://developer.spotify.com/documentation/web-api/tutorials/code-flow) or use a tool like [Spotify Token Generator](https://github.com/alecchen/spotify-refresh-token)

### 4️⃣ Configure the Code

Open `NotAMac_Final.ino` and update these lines:

```cpp
// WiFi Credentials
const char* WIFI_SSID = "Your_WiFi_Name";
const char* WIFI_PASSWORD = "Your_WiFi_Password";

// Spotify Credentials
const char* CLIENT_ID = "your_client_id_here";
const char* CLIENT_SECRET = "your_client_secret_here";
const char* REFRESH_TOKEN = "your_refresh_token_here";
```

### 5️⃣ Upload & Enjoy!

1. Select **XIAO ESP32C3** from Tools → Board
2. Select the correct COM port
3. Click **Upload**
4. Play something on Spotify and watch the magic! 🎶

---

## 🖨️ 3D Printing

The case is designed to look like a miniature classic Macintosh. 

### Recommended Print Settings

| Setting | Value |
|---------|-------|
| Material | PLA or PETG |
| Layer Height | 0.2mm |
| Infill | 15-20% |
| Supports | Yes (for screen opening) |
| Color | "Platinum" gray for authenticity! |

### STL Files

📁 **[Download STL Files](stl/)** (or find similar designs on Thingiverse/Printables)

Popular compatible models:
- Search "Mini Macintosh" or "Tiny Mac" on [Printables](https://www.printables.com/)
- [Thingiverse Mini Mac](https://www.thingiverse.com/search?q=mini+macintosh)

---

## ⚙️ Configuration Options

### Timezone

The default timezone is **GMT+8 (Singapore)**. To change it:

```cpp
// Change 28800 to your UTC offset in seconds
// Examples: GMT-5 = -18000, GMT+0 = 0, GMT+9 = 32400
NTPClient timeClient(ntpUDP, "pool.ntp.org", 28800);
```

### Polling Interval

Adjust how often the display checks Spotify:

```cpp
// In loop(), change 2000 to your preferred interval (milliseconds)
if (now - lastSongUpdate >= 2000) {  // 2 seconds
```

### Color Scheme

The retro Mac colors are defined at the top of the file:

```cpp
#define MAC_PLATINUM      0xD69A   // Classic Mac gray
#define MAC_LIGHT_GRAY    0xC618   // Title bar
#define BTN_RED           0xF800   // Close button
#define BTN_YELLOW        0xFEA0   // Minimize button  
#define BTN_GREEN         0x07E0   // Maximize button
```

---

## 🐛 Troubleshooting

<details>
<summary><strong>HTTP Error -1 or -11</strong></summary>

These are connection timeout errors. The code includes auto-recovery, but if persistent:
- Check your WiFi signal strength
- Ensure your router isn't blocking the ESP32
- Try increasing timeout values in the code

</details>

<details>
<summary><strong>Album art not displaying</strong></summary>

- Ensure you have enough free heap memory (check Serial Monitor)
- The code uses the smallest Spotify image (64×64) to save memory
- If issues persist, try restarting the device

</details>

<details>
<summary><strong>Colors look wrong/inverted</strong></summary>

Different ST7735 panels have different color configurations. Try changing:
```cpp
TJpgDec.setSwapBytes(true);  // Toggle between true/false
```
Or try different initialization:
```cpp
tft.initR(INITR_GREENTAB);   // Instead of INITR_BLACKTAB
```

</details>

<details>
<summary><strong>Token expired / 401 errors</strong></summary>

Your refresh token may have expired. Generate a new one from the Spotify Developer Dashboard. Refresh tokens can expire if:
- You haven't used the app in 6+ months
- You revoked app access in your Spotify settings

</details>

<details>
<summary><strong>Display shows nothing</strong></summary>

- Check your wiring connections
- Verify the display is receiving 3.3V power
- Try adjusting `TFT_CS`, `TFT_DC`, `TFT_RST` pin definitions

</details>

---

## 🎨 Gallery

<p align="center">
  <img src="docs/gallery-1.jpg" alt="Front View" width="250"/>
  <img src="docs/gallery-2.jpg" alt="Side View" width="250"/>
  <img src="docs/gallery-3.jpg" alt="In Action" width="250"/>
</p>

---

## 🗺️ Roadmap

- [ ] Add physical buttons for play/pause/skip
- [ ] OLED display variant
- [ ] Web configuration portal (no code editing!)
- [ ] Multiple display layouts (switchable)
- [ ] Apple Music support
- [ ] Battery-powered version
- [ ] PCB design for cleaner internals

---

## 🤝 Contributing

Contributions are welcome! Feel free to:

1. 🍴 Fork the repository
2. 🌿 Create a feature branch (`git checkout -b feature/amazing-feature`)
3. 💾 Commit your changes (`git commit -m 'Add amazing feature'`)
4. 📤 Push to the branch (`git push origin feature/amazing-feature`)
5. 🎉 Open a Pull Request

---

## 📜 License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

---

## 🙏 Acknowledgments

- **Seeed Studio** for the amazing XIAO ESP32-C3
- **Spotify** for their Web API
- **Adafruit** for the GFX libraries
- **Bodmer** for TJpg_Decoder
- The **retro computing community** for inspiration
- Everyone who's ever wished their desk had more vintage Mac energy ✨

---

<p align="center">
  <strong>Built with ❤️ and nostalgia</strong>
</p>

<p align="center">
  <a href="https://github.com/yourusername/notamac/stargazers">⭐ Star this repo</a> if you found it helpful!
</p>

<p align="center">
  <img src="docs/notamac-footer.gif" alt="NotAMac Animation" width="200"/>
</p>

---

<p align="center">
  <sub>NotAMac is not affiliated with Apple Inc. The Macintosh design is used for nostalgic/educational purposes.</sub>
</p>
