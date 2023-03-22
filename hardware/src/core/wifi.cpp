#include "./wifi.h"

char apName[] = THINGNAME;

bool isAttemptingToConnect = false;
bool credsSetTryConnect = false;

bool bleEnabled = false;
bool hasReceivedInitiator = false;
bool hasRecevedWifiCredentials = false;

/**
 * Create unique device name from MAC address
 **/
void createName()
{
  uint8_t baseMac[6];
  // Get MAC address for WiFi station
  esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
  // Write unique name into apName
  sprintf(apName, "TANK-%02X%02X%02X", baseMac[0], baseMac[1], baseMac[2]);
}

Wifi wifi;

/** Characteristic for digital output */
BLECharacteristic *pCharacteristicWiFi;
BLECharacteristic *pCharacteristicStatus;
BLECharacteristic *pCharacteristicDeviceInfo;
/** BLE Advertiser */
BLEAdvertising *pAdvertising;
/** BLE Service */
BLEService *pService;
/** BLE Server */
BLEServer *pServer;

/** Buffer for JSON string */
// MAx size is 51 bytes for frame:
// {"ssidPrim":"","pwPrim":"","ssidSec":"","pwSec":""}
// + 4 x 32 bytes for 2 SSID's and 2 passwords
StaticJsonDocument<256> jsonBuffer;

/**
 * MyServerCallbacks
 * Callbacks for client connection and disconnection
 */
class MyServerCallbacks : public BLEServerCallbacks
{
  // TODO this doesn't take into account several clients being connected
  void onConnect(BLEServer *pServer)
  {
    Serial.println("BLE client connected");
  };

  void onDisconnect(BLEServer *pServer)
  {
    Serial.println("BLE client disconnected");
    pAdvertising->start();
  }
};

void setStatusCharacteristic()
{
  if (!bleEnabled)
    return;
  String status;
  /** Json object for outgoing data */
  DynamicJsonDocument jsonOut(48);
  if (!hasReceivedInitiator)
  {
    jsonOut["status"] = "AWAITING_PAIRING_INITIATOR";
  }
  else if (!hasRecevedWifiCredentials)
  {
    jsonOut["status"] = "AWAITING_WIFI_CREDENTIALS";
  }
  else if (isAttemptingToConnect)
  {
    jsonOut["status"] = "CONNECTING";
  }
  else
  {
    jsonOut["status"] = wifi.getStatus() == WL_CONNECTED ? "OK" : "DISCONNECTED";
  }
  // Convert JSON object into a string
  serializeJson(jsonOut, status);
  Serial.println("Sending status: " + status);
  pCharacteristicStatus->setValue((uint8_t *)&status[0], status.length());
  pCharacteristicStatus->notify();
};

/**
 * StatusCallbackHandler
 * Callbacks for BLE client read requests for the WiFi connection status
 */
class StatusCallbackHandler : public BLECharacteristicCallbacks
{
  void onRead(BLECharacteristic *pCharacteristic)
  {
    Serial.println("WiFi Status onRead request");
    setStatusCharacteristic();
  };

  void onNotify(BLECharacteristic *pCharacteristic)
  {
    Serial.println("WiFi Status onNotify request");
  };
};

/**
 * WifiCredsCallbackHandler
 * Callbacks for BLE client read/write requests
 */
class WifiCredsCallbackHandler : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string value = pCharacteristic->getValue();
    if (value.length() == 0)
    {
      return;
    }
    Serial.println("Received over BLE: " + String((char *)&value[0]));

    // Decode data
    int keyIndex = 0;
    for (int index = 0; index < value.length(); index++)
    {
      // value[index] = (char)value[index] ^ (char)apName[keyIndex];
      keyIndex++;
      if (keyIndex >= strlen(apName))
        keyIndex = 0;
    }

    /** Json object for incoming data */
    DynamicJsonDocument doc(1024);

    DeserializationError error = deserializeJson(doc, value);

    if (!error)
    {
      if (doc.containsKey("initiator"))
      {
        auto initiator = doc["initiator"].as<String>();
        hasReceivedInitiator = true;

        Serial.println("Pairing Initiator: " + initiator);
        config.setPairingInitiator(String(initiator));
        hasReceivedInitiator = true;
        setStatusCharacteristic();
      }
      if (doc.containsKey("ssid") &&
          doc.containsKey("pass"))
      {
        auto ssid = doc["ssid"].as<String>();
        auto pw = doc["pass"].as<String>();

        config.setWifiCredentials(ssid.c_str(), pw.c_str());

        Serial.println("Received over bluetooth:");
        Serial.println("primary SSID: " + ssid + " password: " + pw);

        hasRecevedWifiCredentials = true;
        credsSetTryConnect = true;

        // Once setup is complete, reboot the device.
      }
      else if (doc.containsKey("erase"))
      {
        Serial.println("Received erase command");
        config.clearWifiCredentials();
      }
      else if (doc.containsKey("reset"))
      {
        WiFi.disconnect();
        ESP.restart();
      }
    }
    else
    {
      Serial.println("Received invalid JSON");
    }
    jsonBuffer.clear();
  };

  void onRead(BLECharacteristic *pCharacteristic)
  {
    Serial.println("BLE onRead request");
    String wifiCredentials;

    /** Json object for outgoing data */
    DynamicJsonDocument jsonOut(1024);
    auto creds = config.getWifiCredentials();
    jsonOut["ssid"] = creds.first;
    jsonOut["pass"] = creds.second;
    // Convert JSON object into a string
    serializeJson(jsonOut, wifiCredentials);

    // encode the data
    int keyIndex = 0;
    Serial.println("Stored settings: " + wifiCredentials);
    for (int index = 0; index < wifiCredentials.length(); index++)
    {
      // wifiCredentials[index] = (char)wifiCredentials[index] ^ (char)apName[keyIndex];
      keyIndex++;
      if (keyIndex >= strlen(apName))
        keyIndex = 0;
    }
    Serial.println("Stored creds_encoded: " + wifiCredentials);

    pCharacteristicWiFi->setValue((uint8_t *)&wifiCredentials[0], wifiCredentials.length());
    jsonBuffer.clear();
  }
};

/**
 * DeviceInfoCallbackHandler
 * Callbacks for BLE client read requests for the device info
 */
class DeviceInfoCallbackHandler : public BLECharacteristicCallbacks
{
  void onRead(BLECharacteristic *pCharacteristic)
  {
    Serial.println("Device info onRead request");
    String deviceInfo;

    /** Json object for outgoing data */
    DynamicJsonDocument jsonOut(1024);
    jsonOut["name"] = apName;
    jsonOut["deviceID"] = THINGNAME;
    jsonOut["build"] = __DATE__ "_" __TIME__;
    jsonOut["version"] = FW_VERSION;
    // Convert JSON object into a string
    serializeJson(jsonOut, deviceInfo);

    pCharacteristicDeviceInfo->setValue((uint8_t *)&deviceInfo[0], deviceInfo.length());
    jsonBuffer.clear();
  };
};

/**
 * initBLE
 * Initialize BLE service and characteristic
 * Start BLE server and service advertising
 */
void initBLE()
{
  // Create a unique name based on the MAC address.
  createName();

  // Initialize BLE and set output power
  BLEDevice::init(apName);
  BLEDevice::setPower(ESP_PWR_LVL_P9);

  // Create BLE Server
  pServer = BLEDevice::createServer();

  // Set server callbacks
  pServer->setCallbacks(new MyServerCallbacks());

  // Create BLE Service
  pService = pServer->createService(BLEUUID(SERVICE_UUID), 20);

  // Create BLE Characteristic for WiFi settings
  pCharacteristicWiFi = pService->createCharacteristic(
      BLEUUID(WIFI_UUID),
      BLECharacteristic::PROPERTY_WRITE);
  pCharacteristicWiFi->setCallbacks(new WifiCredsCallbackHandler());

  // Create BLE Characteristic for WiFi status
  pCharacteristicStatus = pService->createCharacteristic(
      BLEUUID(STATUS_UUID),
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_NOTIFY);
  pCharacteristicStatus->setCallbacks(new StatusCallbackHandler());

  // Create BLE Characteristic for Device Info
  pCharacteristicDeviceInfo = pService->createCharacteristic(
      BLEUUID(DEVICE_INFO_UUID),
      BLECharacteristic::PROPERTY_READ);
  pCharacteristicDeviceInfo->setCallbacks(new DeviceInfoCallbackHandler());

  // Start the service
  pService->start();

  // Start advertising
  pAdvertising = pServer->getAdvertising();
  // Domestic Appliance
  pAdvertising->setAppearance(0x0900);
  pAdvertising->addServiceUUID(BLEUUID(SERVICE_UUID));
  BLEAdvertisementData advData;
  advData.setName(apName);
  advData.setManufacturerData("Rundquist.tech");
  pAdvertising->setAdvertisementData(advData);
  pAdvertising->start();
  Serial.println("BLE advertising started");
  Serial.println(apName);

  bleEnabled = true;
}

void Wifi::init()
{
#if defined(ENABLE_STATIC_IP)
  IPAddress ip(STATIC_IP);
  IPAddress gateway(GATEWAY);
  IPAddress subnet(SUBNET);

  WiFi.config(ip, gateway, subnet);
#endif
  WiFi.hostname(HOSTNAME);
  // initBLE();
}

void Wifi::startAP()
{

  WiFi.setAutoReconnect(true);
  WiFi.softAP(AP_SSID, AP_PASS);

  Serial.println("Started access point.");
  Serial.print("Ip: ");
  Serial.println(WiFi.softAPIP());
  Serial.print("Starting initial scan... ");
  WiFi.scanNetworks(true);
  Serial.println("OK");
}

void Wifi::stopAP()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    return;
  }

  WiFi.setAutoReconnect(true);
  WiFi.softAPdisconnect(true);

  Serial.println("Stopped access point.");
}

void Wifi::startBluetoothPairing()
{
  // TODO Maybe enable the pairing service.
}

void Wifi::connect()
{
  WiFi.mode(WIFI_STA);
  isAttemptingToConnect = true;
  setStatusCharacteristic();
  if (WiFi.status() == WL_CONNECTED)
  {
    WiFi.disconnect();
  }
  // Connect to WPA/WPA2 network:
  auto cfg = config.getWifiCredentials();
  auto status = WiFi.begin(cfg.first.c_str(), cfg.second.c_str());
  Serial.print("[connect] Connecting to `");
  Serial.print(cfg.first.c_str());
  Serial.print("` with password `");
  Serial.print(cfg.second.c_str());
  Serial.println("`");
  int attempts = MAX_CONNECTION_ATTEMPTS;
  while (((status == WL_DISCONNECTED) || (status == WL_IDLE_STATUS)) && (--attempts > 0))
  {
    Serial.println("[connect] status - " + String(status));
    setStatusCharacteristic();
    Serial.println("[connect] delaying - " + String(1000 + (20 - attempts) * 500));
    delay(1000 + (MAX_CONNECTION_ATTEMPTS - attempts) * 500);
    status = WiFi.status();
  };
  if (status == WL_CONNECTED)
  {
    Serial.println("[connect] Connected");
    Serial.print("[connect] IP address: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("[connect] Failed to connect");
  }
  isAttemptingToConnect = false;
  setStatusCharacteristic();
}

void Wifi::disconnect()
{
  WiFi.disconnect();
}

int Wifi::getMode()
{
  return WiFi.getMode();
}

int Wifi::getStatus()
{
  return WiFi.status();
}

int Wifi::getRSSI()
{
  return WiFi.RSSI();
}

int Wifi::getRSSI(int n)
{
  return WiFi.RSSI(n);
}

float Wifi::getQuality()
{
  int dBm = wifi.getRSSI();
  if (dBm <= -100)
    return 0.0;
  else if (dBm >= -50)
    return 1.0;
  else
    return (2 * (dBm + 100)) / 100.0;
}

int Wifi::scanNetworks()
{
  return WiFi.scanNetworks();
}

int Wifi::scanNetworks(bool async)
{
  return WiFi.scanNetworks(async);
}

int Wifi::scanComplete()
{
  return WiFi.scanComplete();
}

void Wifi::scanDelete()
{
  WiFi.scanDelete();
}

int Wifi::getChannel(int n)
{
  return WiFi.channel(n);
}

int Wifi::getEncryptionType(int n)
{
  return WiFi.encryptionType(n);
}

String Wifi::getSSID()
{
  return WiFi.SSID();
}

String Wifi::getSSID(int n)
{
  return WiFi.SSID(n);
}

String Wifi::getBSSIDstr(int n)
{
  return WiFi.BSSIDstr(n);
}

IPAddress Wifi::getLocalIp()
{
  return WiFi.localIP();
}

void Wifi::loop()
{
  if (credsSetTryConnect)
  {
    credsSetTryConnect = false;
    connect();
  }
}