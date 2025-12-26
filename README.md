Here is a clean, visually appealing, and developer-friendly README for your project.

---

# 🖥️ NotAMac: The Retro Spotify Display

> **"It's not a Mac, but it vibes like one."**

Welcome to **NotAMac**! This project turns an ESP32 and a small TFT screen into a tiny, retro-styled "Now Playing" monitor for your Spotify account. Inspired by the classic Macintosh System 7 aesthetic, it features window chrome, 3D bevels, and those iconic traffic light buttons.

---

## ✨ Features

* **Retro Aesthetics:** Custom-drawn UI with the classic "Platinum" color palette, 3D beveled progress bars, and window chrome.
* **Live Album Art:** Downloads and renders Spotify album covers in real-time.
* **Smart Refresh:** Only redraws the screen when the track changes to prevent flickering.
* **Precision Time:** Syncs with NTP to show the current time in the "menu bar."
* **Robust Networking:** Auto-reconnects to WiFi and handles Spotify OAuth token refreshing automatically.

---

## 🛠️ Hardware Requirements

* **Microcontroller:** XIAO ESP32 C3 (has to be this to follow given pinout connections).
* **Display:** 1.8" or 1.44" TFT Display (As long as its using ST7735 Driver).
* **Connections:** Standard SPI wiring.

### 🔌 Pinout Configuration

Based on the code provided, wire your display as follows:

| TFT Pin | ESP32 Pin | Variable Name |
| --- | --- | --- |
| **CS** | GPIO 5 | `TFT_CS` |
| **RST** | GPIO 3 | `TFT_RST` |
| **DC/A0** | GPIO 4 | `TFT_DC` |
| **SDA** | GPIO 23 | *(Hardware MOSI)* |
| **SCK** | GPIO 18 | *(Hardware SCK)* |
| **VCC** | 3.3V |  |
| **GND** | GND |  |

---

## 🚀 Getting Started

### 1. Library Dependencies

Install these libraries via the Arduino Library Manager:

* `Adafruit_GFX`
* `Adafruit_ST7735`
* `ArduinoJson`
* `TJpg_Decoder`
* `NTPClient`

### 2. The "Secret" Sauce (Spotify Auth) 🔐

You need to register an app on the [Spotify Developer Dashboard](https://developer.spotify.com/dashboard/) to get your credentials.

1. Create an App.
2. Get your **Client ID** and **Client Secret**.
3. Generate a **Refresh Token** (You can use a "Get Spotify Refresh Token" utility sketch or web tool for this).
4. Update the top of the sketch:

```cpp
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* CLIENT_ID = "YOUR_SPOTIFY_CLIENT_ID";
const char* CLIENT_SECRET = "YOUR_SPOTIFY_CLIENT_SECRET";
const char* REFRESH_TOKEN = "YOUR_LONG_REFRESH_TOKEN";

```

### 3. Flash It!

Upload the code to your ESP32. If the screen is garbled, check your `tft.initR(INITR_BLACKTAB)` line—different screens use different "tabs" (Green, Red, Black).

---

## 🧠 Under the Hood: How it Works

Here is a quick tour of the code for contributors:

### 🎨 The UI Engine

We don't use images for the UI elements (except the Album Art). Everything is drawn mathematically!

* **`fill3DRect`**: This custom helper function draws rectangles with light and dark borders to simulate the classic raised or lowered 3D look of 90s OS interfaces.
* **Colors**: We use `RGB565` hex codes defined at the top (`MAC_PLATINUM`, `MAC_CHARCOAL`) to keep the palette authentic.

### 🖼️ The Album Art Pipeline

Handling images on microcontrollers is tricky. Here is our flow:

1. **JSON Parsing:** We get the image URL from the Spotify API.
2. **Buffer:** We `malloc` a buffer (`jpegBuffer`) to store the raw JPG bytes.
3. **Decoding:** `TJpg_Decoder` reads that buffer and renders it pixel-by-pixel to the screen using the `tft_output` callback.

### 🔄 The Loop Logic

1. **Check WiFi:** If down, show the "Reconnecting" splash screen.
2. **Check Token:** If 45 mins have passed, hit the API to get a new Access Token using the Refresh Token.
3. **Check Song:** Every 1 second, ask Spotify "What's playing?"
* *Same song?* Just update the progress bar.
* *New song?* Trigger `needsFullRedraw = true` to wipe the screen and download new art.



---

## 🤝 Contributing

Got ideas? We'd love to see them! Here is what's on the To-Do list:

* [ ] **WiFi Manager:** Stop hardcoding WiFi credentials; use a captive portal instead.
* [ ] **Touch Controls:** Use the screen (if touch-enabled) to Play/Pause/Skip.
* [ ] **Lyrics Mode:** A toggle to show scrolling lyrics.

**Fork this repo, make your changes, and submit a Pull Request!**

---

## 📜 License

MIT License. Go wild, build cool stuff, and keep the retro dream alive. 💾
