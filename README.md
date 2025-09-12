<p align="right">
  English | <a href="./README.zh-CN.md">简体中文</a>
</p>

# esp32_ble_web

Web Bluetooth demo with **ESP32**:  
- Toggle onboard LED (send/receive text `1` or `0`)  
- Read soil moisture sensor value (`raw` + `humidity %`)  

## Files
- `doit_esp32_devkitv1_BLE_web.ino` – ESP32 firmware (BLE Service + two Characteristics)
- `index.html` – main webpage (with simple styling)
- `esp32_ble_web_no_css.html` – minimal version without CSS

## UUIDs
- **Service UUID**: `19b10000-e8f2-537e-4f6c-d104768a1214`
- **LED Characteristic** (WRITE/READ, text): accepts `"1"` or `"0"`
- **Sensor Characteristic** (READ/NOTIFY, text):  
  - Payload format: `"raw,humidity"` (example: `1780,63`)

> Default pins: LED = **GPIO2**, Soil Sensor = **GPIO4 (ADC)**  

## How to Use
1. **Flash firmware**  
   - Open `.ino` in Arduino IDE, select ESP32 board, upload.
2. **Open the webpage**  
   - Launch `index.html` (or visit `https://tianqi-yao.github.io/esp32_ble_web/`).  
   - Chrome/Edge/Android work directly; **iOS** requires **Bluefy** App.
3. **Connect and control**  
   - Click **Connect** → pick your ESP32.  
   - **LED**: toggle checkbox → sends `"1"`/`"0"` and reads back.  
   - **Soil sensor**: click **Read Now** or enable **Auto** (notify if supported, else polling).
