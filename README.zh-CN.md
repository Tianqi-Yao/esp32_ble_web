<p align="right">
  <a href="./README.md">English</a> | 简体中文
</p>

# esp32_ble_web

用 **Web Bluetooth** 在浏览器里连接 ESP32：  
- 开/关板载 LED  
- 读取土壤湿度（原始值 + 百分比）

## 文件说明
- `doit_esp32_devkitv1_BLE_web.ino` – ESP32 固件（BLE Service + 两个 Characteristic）
- `index.html` – 网页（默认入口）
- `esp32_ble_web_no_css.html` – **无 CSS 极简版**，结构与逻辑基本一致

## 协议与 UUID
- **Service UUID**：`19b10000-e8f2-537e-4f6c-d104768a1214`
- **LED Characteristic**（WRITE/READ，文本）：发送/回显 **`"1"` 或 `"0"`**
- **Sensor Characteristic**（READ/NOTIFY，文本）：
  - 推荐发送：`"raw,humidity"`（例如 `1780,63`）

> 固件默认引脚：LED = **GPIO2**；土壤传感器 = **GPIO4（模拟）**  

## 如何使用
1. **烧录固件**  
   - Arduino IDE 选择 ESP32 开发板 → 打开 `doit_esp32_devkitv1_BLE_web.ino` → 烧录。  
2. **打开网页**  
   - 直接打开 `index.html`（或部署到 GitHub Pages，见下）。  
   - 浏览器：**Chrome/Edge/Android** 直接可用；**iOS** 请用 **Bluefy** App 打开。  
3. **连接与操作**  
   - 点击 **Connect** → 选择你的 ESP32。  
   - **LED**：切换开关 → 向特征写入 `"1"`/`"0"`，并从特征 **READ** 回显状态。  
   - **Sensor**：点 **Read Now** 获取一次；打开 **Auto** 可持续更新（支持 Notify 时订阅，否则每秒轮询）。  
   - 使用 `index.html` 时，可在页面输入 **Wet/Dry** 原始值做一次两点标定，自动计算湿度百分比。

## GitHub Pages（可选）
1. 仓库 **Settings → Pages**  
2. **Source** 选 `Deploy from a branch`，分支 `main`，目录 `/root`，保存  
3. 等待部署完成，访问：  
   `https://tianqi-yao.github.io/esp32_ble_web/`
