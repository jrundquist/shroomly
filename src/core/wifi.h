

#pragma once

#include <WiFi.h>
#include "../include/common.h"

class Wifi
{
public:
  void init();
  void startAP();
  void stopAP();
  void connect(String ssid = "", String pass = "");
  void disconnect();

  int getMode();
  int getRSSI();
  int getRSSI(int n);
  int getStatus();

  int scanNetworks();
  int scanNetworks(bool async);
  int scanComplete();
  void scanDelete();
  int getChannel(int n);
  int getEncryptionType(int n);

  String getSSID();
  String getSSID(int n);
  String getBSSIDstr(int n);
  IPAddress getLocalIp();
};

extern Wifi wifi;