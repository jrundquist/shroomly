#include "./http.h"

Http http;
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

void Http::handleSetup(AsyncWebServerRequest *request)
{
  request->send(LittleFS, "/setup.html", "text/html");
}

void Http::init()
{
  // Web Server Root URL
  server.on("/", HTTP_GET, Http::handleSetup);

  server.serveStatic("/", LittleFS, "/");

  server.begin();
}