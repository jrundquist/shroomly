#include "./wifi.h"

Wifi wifi;

void Wifi::init()
{
#if defined(ENABLE_STATIC_IP)
  IPAddress ip(STATIC_IP);
  IPAddress gateway(GATEWAY);
  IPAddress subnet(SUBNET);

  WiFi.config(ip, gateway, subnet);
#endif
  WiFi.hostname(HOSTNAME);
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

void Wifi::connect(String ssid, String pass)
{
  // Connect to WPA/WPA2 network:
  auto status = WiFi.begin(ssid.c_str(), pass.c_str());
  int attempts = 20;
  do
  {
    delay(1000);
    status = WiFi.status();
  } while (((status == WL_IDLE_STATUS) || (status == WL_SCAN_COMPLETED)) && (--attempts > 0));
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