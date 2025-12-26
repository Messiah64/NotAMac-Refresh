// =========================================================================
// NotAMac - Final Layout (Based on Layout 1)
// Features:
// - RED/YELLOW/GREEN traffic light buttons (top left)
// - NTP time clock (top right)  
// - Large album art with text vertically aligned to image height
// - Retro Mac aesthetic
// =========================================================================

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TJpg_Decoder.h>

#include <Fonts/FreeSerif12pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>

// Pin definitions
#define TFT_CS     5
#define TFT_RST    3
#define TFT_DC     4

// =========================================================================
// RETRO MAC COLOR PALETTE
// =========================================================================
#define MAC_WHITE         0xFFFF
#define MAC_PLATINUM      0xD69A
#define MAC_LIGHT_GRAY    0xC618
#define MAC_MID_GRAY      0xAD55
#define MAC_DARK_GRAY     0x8410
#define MAC_CHARCOAL      0x4208
#define MAC_BLACK         0x0000

#define MAC_SHADOW        0x8410
#define MAC_HIGHLIGHT     0xFFFF
#define MAC_LOWLIGHT      0x8410

#define MAC_PROGRESS_BG   0xD69A
#define MAC_PROGRESS_FG   0x4208

// Traffic light button colors
#define BTN_RED           0xF800
#define BTN_YELLOW        0xFEA0
#define BTN_GREEN         0x07E0

// Layout constants
#define TITLE_BAR_HEIGHT  18
#define ALBUM_ART_SIZE    68
#define ALBUM_ART_X       8
#define ALBUM_ART_Y       24
#define TEXT_X            82
#define TEXT_MAX_LEN      12
#define PROGRESS_BAR_Y    100
#define CONTENT_MARGIN    8

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// WiFi & Spotify credentials
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* CLIENT_ID = "YOUR_SPOTIFY_CLIENT_ID";
const char* CLIENT_SECRET = "YOUR_SPOTIFY_CLIENT_SECRET";
const char* REFRESH_TOKEN = "YOUR_LONG_REFRESH_TOKEN";


String accessToken;
unsigned long lastTokenRefresh = 0;
const unsigned long TOKEN_REFRESH_INTERVAL = 45 * 60 * 1000;

// NTP Client (GMT+8 Singapore)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 28800);

// Display state
String currentSong = "";
String currentArtist = "";
String currentAlbum = "";
String currentTrackId = "";
String currentTimeStr = "";
float currentProgress = 0;
int currentDurationSec = 0;
int currentProgressSec = 0;
bool needsFullRedraw = true;

// Album art buffer
uint8_t* jpegBuffer = nullptr;
size_t jpegBufferSize = 0;
int albumArtDrawX = 0;
int albumArtDrawY = 0;

int displayWidth = 160;
int displayHeight = 128;

// Connection tracking
int connectionFailures = 0;

// Spotify logo for "no track" screen
static const unsigned char PROGMEM spotifyLogo[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x80,0x00,0x00,0x07,0xff,0xf0,0x00,
    0x00,0x1f,0xff,0xfc,0x00,0x00,0x7f,0xff,0xff,0x00,0x00,0xff,0xff,0xff,0x80,
    0x01,0xff,0xff,0xff,0xc0,0x03,0xff,0xff,0xff,0xe0,0x07,0xff,0xff,0xff,0xf0,
    0x0f,0xff,0xff,0xff,0xf8,0x0f,0xff,0xff,0xff,0xf8,0x1f,0xff,0xff,0xff,0xfc,
    0x1f,0x80,0x01,0xff,0xfc,0x3e,0x00,0x00,0x1f,0xfe,0x3e,0x00,0x00,0x03,0xfe,
    0x3f,0x00,0x00,0x00,0x7e,0x7f,0xff,0xff,0x00,0x3f,0x7f,0xff,0xff,0xf0,0x1f,
    0x7f,0xff,0xff,0xfc,0x1f,0x7f,0x80,0x00,0xff,0xbf,0x7f,0x00,0x00,0x1f,0xff,
    0x7f,0x80,0x00,0x03,0xff,0x7f,0xff,0xff,0x00,0xff,0x7f,0xff,0xff,0xe0,0xff,
    0x7f,0xff,0xff,0xf8,0xff,0x3f,0xc0,0x01,0xff,0xfe,0x3f,0x80,0x00,0x3f,0xfe,
    0x3f,0xdf,0xfc,0x0f,0xfe,0x1f,0xff,0xff,0x87,0xfc,0x1f,0xff,0xff,0xe7,0xfc,
    0x0f,0xff,0xff,0xff,0xf8,0x0f,0xff,0xff,0xff,0xf8,0x07,0xff,0xff,0xff,0xf0,
    0x03,0xff,0xff,0xff,0xe0,0x01,0xff,0xff,0xff,0xc0,0x00,0xff,0xff,0xff,0x80,
    0x00,0x7f,0xff,0xff,0x00,0x00,0x1f,0xff,0xfc,0x00,0x00,0x07,0xff,0xf0,0x00,
    0x00,0x00,0xff,0x80,0x00
};
// =========================================================================
// HELPER FUNCTIONS
// =========================================================================

void printMemory(const char* label) {
    Serial.printf("[MEM] %s - Free: %d\n", label, ESP.getFreeHeap());
}

String truncateText(String text, int maxLen) {
    if (text.length() > maxLen) {
        return text.substring(0, maxLen - 2) + "..";
    }
    return text;
}

String formatTime(int totalSeconds) {
    int mins = totalSeconds / 60;
    int secs = totalSeconds % 60;
    char buf[8];
    sprintf(buf, "%d:%02d", mins, secs);
    return String(buf);
}

// Draw 3D beveled rectangle
void fill3DRect(int x, int y, int w, int h, uint16_t fillColor, bool raised) {
    tft.fillRect(x, y, w, h, fillColor);
    
    uint16_t topLeft = raised ? MAC_HIGHLIGHT : MAC_LOWLIGHT;
    uint16_t bottomRight = raised ? MAC_LOWLIGHT : MAC_HIGHLIGHT;
    
    tft.drawFastHLine(x, y, w, topLeft);
    tft.drawFastVLine(x, y, h, topLeft);
    tft.drawFastHLine(x, y + h - 1, w, bottomRight);
    tft.drawFastVLine(x + w - 1, y, h, bottomRight);
}

// =========================================================================
// WINDOW CHROME (Title bar with traffic lights and time)
// =========================================================================

void drawTitleBar() {
    // Title bar background
    tft.fillRect(0, 0, displayWidth, TITLE_BAR_HEIGHT, MAC_LIGHT_GRAY);
    
    // Bottom border of title bar
    tft.drawFastHLine(0, TITLE_BAR_HEIGHT - 1, displayWidth, MAC_BLACK);
    
    // Traffic light buttons
    // Red (close)
    tft.fillCircle(10, TITLE_BAR_HEIGHT / 2, 5, BTN_RED);
    tft.drawCircle(10, TITLE_BAR_HEIGHT / 2, 5, MAC_DARK_GRAY);
    
    // Yellow (minimize)
    tft.fillCircle(24, TITLE_BAR_HEIGHT / 2, 5, BTN_YELLOW);
    tft.drawCircle(24, TITLE_BAR_HEIGHT / 2, 5, MAC_DARK_GRAY);
    
    // Green (maximize)
    tft.fillCircle(38, TITLE_BAR_HEIGHT / 2, 5, BTN_GREEN);
    tft.drawCircle(38, TITLE_BAR_HEIGHT / 2, 5, MAC_DARK_GRAY);
}

void updateTimeDisplay() {
    timeClient.update();
    String timeStr = timeClient.getFormattedTime().substring(0, 5);  // HH:MM
    
    if (timeStr != currentTimeStr || needsFullRedraw) {
        // Clear time area
        tft.fillRect(displayWidth - 36, 2, 34, 14, MAC_LIGHT_GRAY);
        
        // Draw time
        tft.setFont();
        tft.setTextSize(1);
        tft.setTextColor(MAC_BLACK);
        tft.setCursor(displayWidth - 32, 5);
        tft.print(timeStr);
        
        currentTimeStr = timeStr;
    }
}

// =========================================================================
// JPEG DECODER CALLBACK
// =========================================================================

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
    if (y >= tft.height()) return 0;
    
    int drawX = x + albumArtDrawX;
    int drawY = y + albumArtDrawY;
    
    tft.startWrite();
    tft.setAddrWindow(drawX, drawY, w, h);
    tft.writePixels(bitmap, w * h);
    tft.endWrite();
    
    return 1;
}

void displayAlbumArt() {
    int x = ALBUM_ART_X;
    int y = ALBUM_ART_Y;
    int size = ALBUM_ART_SIZE;
    
    // Draw border/frame first
    tft.fillRect(x - 2, y - 2, size + 4, size + 4, MAC_BLACK);
    tft.fillRect(x - 1, y - 1, size + 2, size + 2, MAC_DARK_GRAY);
    
    if (jpegBuffer == nullptr || jpegBufferSize == 0) {
        // Placeholder
        tft.fillRect(x, y, size, size, MAC_MID_GRAY);
        tft.setFont();
        tft.setTextSize(3);
        tft.setTextColor(MAC_DARK_GRAY);
        tft.setCursor(x + size/2 - 9, y + size/2 - 12);
        tft.print("?");
        return;
    }
    
    // Clear art area
    tft.fillRect(x, y, size, size, MAC_BLACK);
    
    uint16_t jpegWidth, jpegHeight;
    TJpgDec.getJpgSize(&jpegWidth, &jpegHeight, jpegBuffer, jpegBufferSize);
    
    uint8_t scale = 1;
    if (jpegWidth > size) {
        scale = (jpegWidth + size - 1) / size;
        if (scale > 4) scale = 8;
        else if (scale > 2) scale = 4;
        else if (scale > 1) scale = 2;
    }
    
    uint16_t scaledW = jpegWidth / scale;
    uint16_t scaledH = jpegHeight / scale;
    
    albumArtDrawX = x + (size - scaledW) / 2;
    albumArtDrawY = y + (size - scaledH) / 2;
    
    TJpgDec.setJpgScale(scale);
    TJpgDec.setSwapBytes(false);
    TJpgDec.setCallback(tft_output);
    TJpgDec.drawJpg(0, 0, jpegBuffer, jpegBufferSize);
}

// =========================================================================
// MAIN DISPLAY LAYOUT
// =========================================================================

void drawNowPlayingLayout() {
    // Background
    tft.fillScreen(MAC_PLATINUM);
    
    // Title bar
    drawTitleBar();
    updateTimeDisplay();
    
    // Content area - white background
    tft.fillRect(4, TITLE_BAR_HEIGHT + 2, displayWidth - 8, displayHeight - TITLE_BAR_HEIGHT - 6, MAC_WHITE);
    
    // Outer border
    tft.drawRect(3, TITLE_BAR_HEIGHT + 1, displayWidth - 6, displayHeight - TITLE_BAR_HEIGHT - 4, MAC_BLACK);
    
    // Album art
    displayAlbumArt();
    
    // Calculate vertical positions for text (aligned with image height)
    // Image: y=24, height=68, so spans y=24 to y=92
    // We want 3 text items spread across this: Song, Artist, Album
    int textTop = ALBUM_ART_Y + 4;           // Song at top of image
    int textMid = ALBUM_ART_Y + ALBUM_ART_SIZE / 2 - 4;  // Artist at middle
    int textBot = ALBUM_ART_Y + ALBUM_ART_SIZE - 14;     // Album at bottom
    
    tft.setFont();
    tft.setTextSize(1);
    
    // Song label + value
    tft.setTextColor(MAC_DARK_GRAY);
    tft.setCursor(TEXT_X, textTop);
    tft.print("Song");
    
    tft.setTextColor(MAC_BLACK);
    tft.setCursor(TEXT_X, textTop + 10);
    String songTrunc = truncateText(currentSong, TEXT_MAX_LEN);
    tft.print(songTrunc);
    
    // Artist label + value
    tft.setTextColor(MAC_DARK_GRAY);
    tft.setCursor(TEXT_X, textMid);
    tft.print("Artist");
    
    tft.setTextColor(MAC_BLACK);
    tft.setCursor(TEXT_X, textMid + 10);
    String artistTrunc = truncateText(currentArtist, TEXT_MAX_LEN);
    tft.print(artistTrunc);
    
    // Album label + value
    tft.setTextColor(MAC_DARK_GRAY);
    tft.setCursor(TEXT_X, textBot);
    tft.print("Album");
    
    tft.setTextColor(MAC_BLACK);
    tft.setCursor(TEXT_X, textBot + 10);
    String albumTrunc = truncateText(currentAlbum, TEXT_MAX_LEN);
    tft.print(albumTrunc);
    
    // Progress bar
    drawProgressBar();
}

void drawProgressBar() {
    int barX = CONTENT_MARGIN;
    int barY = PROGRESS_BAR_Y;
    int barW = displayWidth - CONTENT_MARGIN * 2;
    int barH = 8;
    
    // Background bar (inset look)
    fill3DRect(barX, barY, barW, barH, MAC_PROGRESS_BG, false);
    
    // Progress fill
    int fillW = (int)((barW - 4) * currentProgress / 100.0);
    if (fillW > 0) {
        tft.fillRect(barX + 2, barY + 2, fillW, barH - 4, MAC_PROGRESS_FG);
    }
    
    // Time labels below
    tft.setFont();
    tft.setTextSize(1);
    tft.setTextColor(MAC_DARK_GRAY);
    
    // Current time (left)
    tft.setCursor(barX, barY + barH + 4);
    tft.print(formatTime(currentProgressSec));
    
    // Total duration (right)
    String durStr = formatTime(currentDurationSec);
    int durWidth = durStr.length() * 6;
    tft.setCursor(barX + barW - durWidth, barY + barH + 4);
    tft.print(durStr);
}

void updateProgressBar() {
    int barX = CONTENT_MARGIN;
    int barY = PROGRESS_BAR_Y;
    int barW = displayWidth - CONTENT_MARGIN * 2;
    int barH = 8;
    
    // Clear and redraw progress area only
    tft.fillRect(barX, barY, barW, barH + 14, MAC_WHITE);
    
    // Background bar
    fill3DRect(barX, barY, barW, barH, MAC_PROGRESS_BG, false);
    
    // Progress fill
    int fillW = (int)((barW - 4) * currentProgress / 100.0);
    if (fillW > 0) {
        tft.fillRect(barX + 2, barY + 2, fillW, barH - 4, MAC_PROGRESS_FG);
    }
    
    // Time labels
    tft.setFont();
    tft.setTextSize(1);
    tft.setTextColor(MAC_DARK_GRAY);
    
    tft.setCursor(barX, barY + barH + 4);
    tft.print(formatTime(currentProgressSec));
    
    String durStr = formatTime(currentDurationSec);
    int durWidth = durStr.length() * 6;
    tft.setCursor(barX + barW - durWidth, barY + barH + 4);
    tft.print(durStr);
}

// =========================================================================
// NO TRACK PLAYING SCREEN
// =========================================================================

void displayNoTrackPlaying() {
    tft.fillScreen(MAC_PLATINUM);
    
    drawTitleBar();
    updateTimeDisplay();
    
    // Content area
    tft.fillRect(4, TITLE_BAR_HEIGHT + 2, displayWidth - 8, displayHeight - TITLE_BAR_HEIGHT - 6, MAC_WHITE);
    tft.drawRect(3, TITLE_BAR_HEIGHT + 1, displayWidth - 6, displayHeight - TITLE_BAR_HEIGHT - 4, MAC_BLACK);
    
    // Spotify logo
    int logoX = (displayWidth - 40) / 2;
    int logoY = 36;
    tft.drawBitmap(logoX, logoY, spotifyLogo, 40, 40, BTN_GREEN);
    
    // Decorative line
    tft.drawFastHLine(30, 82, displayWidth - 60, MAC_MID_GRAY);
    
    // Message
    tft.setFont();
    tft.setTextSize(1);
    tft.setTextColor(MAC_DARK_GRAY);
    
    const char* line1 = "Play a song";
    int16_t x1, y1;
    uint16_t w, h;
    tft.getTextBounds(line1, 0, 0, &x1, &y1, &w, &h);
    tft.setCursor((displayWidth - w) / 2, 90);
    tft.print(line1);
    
    const char* line2 = "on Spotify";
    tft.getTextBounds(line2, 0, 0, &x1, &y1, &w, &h);
    tft.setCursor((displayWidth - w) / 2, 102);
    tft.print(line2);
    
    // Dots
    tft.fillCircle(displayWidth/2 - 15, 116, 2, MAC_MID_GRAY);
    tft.fillCircle(displayWidth/2, 116, 2, BTN_GREEN);
    tft.fillCircle(displayWidth/2 + 15, 116, 2, MAC_MID_GRAY);
    
    currentTrackId = "NO_TRACK";
}

// =========================================================================
// SPLASH SCREEN
// =========================================================================

void displaySplashScreen() {
    tft.fillScreen(MAC_WHITE);
    
    int centerY = displayHeight / 2;
    
    tft.drawBitmap(5, centerY - 20, spotifyLogo, 40, 40, BTN_GREEN);
    
    tft.setFont(&FreeSerif12pt7b);
    tft.setTextColor(MAC_BLACK);
    tft.setCursor(50, centerY + 10);
    tft.print("Not A Mac");
    
    tft.setFont();
}

void updateSplashStatus(const char* status) {
    tft.fillRect(0, displayHeight - 15, displayWidth, 15, MAC_WHITE);
    
    tft.setFont();
    tft.setTextSize(1);
    tft.setTextColor(MAC_DARK_GRAY);
    
    int16_t x1, y1;
    uint16_t w, h;
    tft.getTextBounds(status, 0, 0, &x1, &y1, &w, &h);
    tft.setCursor((displayWidth - w) / 2, displayHeight - 12);
    tft.print(status);
}

// =========================================================================
// NETWORK FUNCTIONS
// =========================================================================

bool downloadAlbumCover(String imageUrl) {
    printMemory("Before download");
    
    if (jpegBuffer != nullptr) {
        free(jpegBuffer);
        jpegBuffer = nullptr;
        jpegBufferSize = 0;
    }
    
    delay(50);
    
    WiFiClientSecure client;
    client.setInsecure();
    client.setTimeout(10);
    
    HTTPClient http;
    http.begin(client, imageUrl);
    http.setTimeout(8000);
    
    int httpCode = http.GET();
    
    if (httpCode == 200) {
        int len = http.getSize();
        Serial.printf("Image size: %d bytes\n", len);
        
        if (len > 0 && len < 30000 && ESP.getFreeHeap() > (len + 20000)) {
            jpegBuffer = (uint8_t*)malloc(len);
            
            if (jpegBuffer) {
                WiFiClient* stream = http.getStreamPtr();
                size_t bytesRead = 0;
                unsigned long startTime = millis();
                
                while (http.connected() && bytesRead < len && (millis() - startTime) < 8000) {
                    size_t avail = stream->available();
                    if (avail) {
                        bytesRead += stream->readBytes(jpegBuffer + bytesRead, min(avail, (size_t)(len - bytesRead)));
                    }
                    delay(1);
                }
                
                jpegBufferSize = bytesRead;
                http.end();
                connectionFailures = 0;
                Serial.printf("Downloaded %d bytes OK\n", bytesRead);
                return (bytesRead == len);
            }
        }
    } else {
        Serial.printf("Image download failed: %d\n", httpCode);
    }
    
    http.end();
    return false;
}

bool getAccessToken() {
    Serial.println("Getting access token...");
    
    WiFiClientSecure client;
    client.setInsecure();
    client.setTimeout(10);
    
    HTTPClient http;
    http.begin(client, "https://accounts.spotify.com/api/token");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.setTimeout(10000);
    
    String body = "grant_type=refresh_token&refresh_token=" + String(REFRESH_TOKEN) +
                  "&client_id=" + String(CLIENT_ID) +
                  "&client_secret=" + String(CLIENT_SECRET);
    
    int httpCode = http.POST(body);
    
    if (httpCode == 200) {
        String response = http.getString();
        http.end();
        
        StaticJsonDocument<512> doc;
        deserializeJson(doc, response);
        accessToken = doc["access_token"].as<String>();
        lastTokenRefresh = millis();
        connectionFailures = 0;
        Serial.println("Token OK!");
        return true;
    }
    
    Serial.printf("Auth failed: %d\n", httpCode);
    http.end();
    return false;
}

void getCurrentSong() {
    printMemory("Before API call");
    
    WiFiClientSecure client;
    client.setInsecure();
    client.setTimeout(10);
    
    HTTPClient http;
    http.begin(client, "https://api.spotify.com/v1/me/player/currently-playing");
    http.addHeader("Authorization", "Bearer " + accessToken);
    http.setTimeout(8000);
    
    int httpCode = http.GET();
    
    if (httpCode == 200) {
        connectionFailures = 0;
        String response = http.getString();
        http.end();
        
        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson(doc, response);
        
        if (!err) {
            String newTrackId = doc["item"]["id"] | "";
            bool trackChanged = (newTrackId != currentTrackId);
            
            String newSong = doc["item"]["name"] | "Unknown";
            String newArtist = doc["item"]["artists"][0]["name"] | "Unknown";
            String newAlbum = doc["item"]["album"]["name"] | "Unknown";
            
            int progress_ms = doc["progress_ms"] | 0;
            int duration_ms = doc["item"]["duration_ms"] | 1;
            float newProgress = (float)progress_ms / (float)duration_ms * 100.0;
            
            currentProgressSec = progress_ms / 1000;
            currentDurationSec = duration_ms / 1000;
            
            if (trackChanged || currentTrackId == "NO_TRACK") {
                currentTrackId = newTrackId;
                currentSong = newSong;
                currentArtist = newArtist;
                currentAlbum = newAlbum;
                currentProgress = newProgress;
                
                // Download new album art (use smallest image - index 2)
                const char* imageUrl = doc["item"]["album"]["images"][2]["url"] | "";
                if (strlen(imageUrl) > 0) {
                    downloadAlbumCover(String(imageUrl));
                }
                
                needsFullRedraw = true;
            } else {
                // Just update progress
                if (abs(newProgress - currentProgress) >= 0.5) {
                    currentProgress = newProgress;
                    updateProgressBar();
                }
                
                // Update time in title bar
                updateTimeDisplay();
            }
        }
        
    } else if (httpCode == 204) {
        connectionFailures = 0;
        http.end();
        
        if (currentTrackId != "NO_TRACK") {
            displayNoTrackPlaying();
        } else {
            updateTimeDisplay();
        }
        
    } else if (httpCode == 401) {
        http.end();
        Serial.println("Token expired, refreshing...");
        getAccessToken();
        
    } else {
        connectionFailures++;
        Serial.printf("HTTP Error: %d (failures: %d)\n", httpCode, connectionFailures);
        http.end();
        
        if (connectionFailures >= 3) {
            Serial.println("Reconnecting WiFi...");
            WiFi.disconnect();
            delay(1000);
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
            
            int attempts = 0;
            while (WiFi.status() != WL_CONNECTED && attempts < 20) {
                delay(500);
                attempts++;
            }
            
            if (WiFi.status() == WL_CONNECTED) {
                Serial.println("WiFi reconnected!");
                connectionFailures = 0;
                getAccessToken();
            }
        }
    }
}

// =========================================================================
// SETUP
// =========================================================================

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n========================================");
    Serial.println("NotAMac - Final Layout");
    Serial.println("========================================\n");
    
    printMemory("Boot");
    
    // Initialize display
    tft.initR(INITR_BLACKTAB);
    tft.setRotation(1);
    displayWidth = tft.width();
    displayHeight = tft.height();
    
    Serial.printf("Display: %dx%d\n", displayWidth, displayHeight);
    
    // Splash screen
    displaySplashScreen();
    delay(1500);
    
    // Connect WiFi
    updateSplashStatus("Connecting...");
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("\nWiFi Failed!");
        updateSplashStatus("WiFi Failed!");
        delay(2000);
        ESP.restart();
    }
    
    Serial.println("\nWiFi Connected!");
    Serial.println(WiFi.localIP());
    updateSplashStatus("WiFi Connected!");
    delay(500);
    
    // NTP
    timeClient.begin();
    timeClient.update();
    
    // Auth
    updateSplashStatus("Authenticating...");
    getAccessToken();
    
    delay(300);
    
    // Initial song fetch
    getCurrentSong();
    
    printMemory("Setup complete");
}

// =========================================================================
// LOOP
// =========================================================================

void loop() {
    unsigned long now = millis();
    
    if (WiFi.status() == WL_CONNECTED) {
        // Refresh token every 45 minutes
        if (now - lastTokenRefresh >= TOKEN_REFRESH_INTERVAL) {
            Serial.println("Refreshing token...");
            getAccessToken();
        }
        
        // Update song every 2 seconds
        static unsigned long lastSongUpdate = 0;
        if (now - lastSongUpdate >= 1000) {
            getCurrentSong();
            lastSongUpdate = now;
        }
        
        // Full redraw if needed
        if (needsFullRedraw) {
            if (currentTrackId != "NO_TRACK" && currentTrackId.length() > 0) {
                drawNowPlayingLayout();
            }
            needsFullRedraw = false;
        }
        
    } else {
        Serial.println("WiFi disconnected!");
        displaySplashScreen();
        updateSplashStatus("Reconnecting...");
        
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 30) {
            delay(500);
            attempts++;
        }
        
        if (WiFi.status() == WL_CONNECTED) {
            getAccessToken();
            getCurrentSong();
        }
    }
    
    delay(100);
}
