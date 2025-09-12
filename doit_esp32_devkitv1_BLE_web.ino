#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// ===== Adjustable parameters =====
#define SERVICE_UUID               "19b10000-e8f2-537e-4f6c-d104768a1214"
#define SENSOR_CHARACTERISTIC_UUID "19b10001-e8f2-537e-4f6c-d104768a1214"
#define LED_CHARACTERISTIC_UUID    "19b10002-e8f2-537e-4f6c-d104768a1214"

const char* DEVICE_NAME = "ESP32-SoilLED";
const int   LED_PIN     = 2;
const int   SOIL_PIN    = 4;     // use an ADC pin for analog sensors

// Calibration for humidity (%) mapping.
// raw_dry: reading in air/dry soil; raw_wet: reading in water/saturated soil.
// Many soil sensors read LOWER when wet. Adjust after a quick 2-point calibration.
int RAW_DRY = 3200;
int RAW_WET = 1200;

const unsigned long NOTIFY_INTERVAL_MS = 1000;
// ===================================

BLEServer*        pServer      = nullptr;
BLECharacteristic *pCharSensor = nullptr, *pCharLED = nullptr;

bool   connected  = false;
bool   ledOn      = false;
unsigned long lastTs = 0;

// ------- Helpers -------
int readRaw() {
  return analogRead(SOIL_PIN);   // 0..4095
}
int computeHumidityPercent(int raw) {
  // Map raw -> 0..100 using linear interpolation between RAW_DRY and RAW_WET.
  // Handles both normal and inverted relationships.
  float span = (float)(RAW_DRY - RAW_WET);
  if (span == 0) return 0;
  float pct = (RAW_DRY - raw) * 100.0f / span;  // wet -> higher %
  if (pct < 0) pct = 0; if (pct > 100) pct = 100;
  return (int)(pct + 0.5f);
}
String soilText() {
  int raw = readRaw();
  int hum = computeHumidityPercent(raw);
  return String(raw) + "," + String(hum);   // e.g., "1780,63"
}
String ledText() { return ledOn ? "1" : "0"; }
void   applyLed(bool on) {
  ledOn = on;
  digitalWrite(LED_PIN, ledOn ? HIGH : LOW);
}

// ------- Callbacks -------
class ServerCb : public BLEServerCallbacks {
  void onConnect(BLEServer*)    override { connected = true;  Serial.println("[BLE] Connected"); }
  void onDisconnect(BLEServer*) override { connected = false; Serial.println("[BLE] Disconnected"); BLEDevice::startAdvertising(); }
};

// LED: write "1"/"0" only; read echoes "1"/"0"
class LedCb : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* ch) override {
    String cmd = String(ch->getValue()); // normalize to Arduino String
    cmd.trim();
    if (cmd == "1") applyLed(true);
    else if (cmd == "0") applyLed(false);
    else {
      // ignore invalid; just echo current state
    }
    ch->setValue(ledText());
    Serial.print("[LED] state: "); Serial.println(ledText());
  }
  void onRead(BLECharacteristic* ch) override {
    ch->setValue(ledText());
  }
};

// Sensor: on READ, respond with "raw,humidity"
class SensorCb : public BLECharacteristicCallbacks {
  void onRead(BLECharacteristic* ch) override {
    ch->setValue(soilText());
  }
};

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // ADC config (optional):
  // analogReadResolution(12);
  // analogSetPinAttenuation(SOIL_PIN, ADC_11db); // ~0..3.3V

  BLEDevice::init(DEVICE_NAME);

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCb());

  BLEService* svc = pServer->createService(SERVICE_UUID);

  // Sensor: READ + NOTIFY ("raw,humidity"), needs CCCD
  pCharSensor = svc->createCharacteristic(
    SENSOR_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );
  pCharSensor->setCallbacks(new SensorCb());
  pCharSensor->addDescriptor(new BLE2902());
  pCharSensor->setValue(soilText());  // initial payload

  // LED: WRITE + READ ("1"/"0")
  pCharLED = svc->createCharacteristic(
    LED_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_READ
  );
  pCharLED->setCallbacks(new LedCb());
  pCharLED->setValue(ledText());

  svc->start();

  BLEAdvertising* adv = BLEDevice::getAdvertising();
  adv->addServiceUUID(SERVICE_UUID);
  adv->setScanResponse(true);
  BLEDevice::startAdvertising();

  Serial.println("[BLE] Advertising... ready.");
}

void loop() {
  if (connected && millis() - lastTs >= NOTIFY_INTERVAL_MS) {
    lastTs = millis();
    pCharSensor->setValue(soilText());
    pCharSensor->notify();
  }
}
