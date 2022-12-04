#include "./camera.h"

Camera camera;

Adafruit_VC0706 cam = Adafruit_VC0706(&cameraconnection);

void Camera::init()
{
  // Try to locate the camera
  if (cam.begin())
  {
    Serial.println("Camera Found:");
  }
  else
  {
    Serial.println("No camera found?");
    return;
  }
  // Print out the camera version information (optional)
  char *reply = cam.getVersion();
  if (reply == 0)
  {
    Serial.print("Failed to get version");
  }
  else
  {
    Serial.println("-----------------");
    Serial.print(reply);
    Serial.println("-----------------");
  }
}

Photo Camera::takePhoto()
{
  // Set the picture size - you can choose one of 640x480, 320x240 or 160x120
  // Remember that bigger pictures take longer to transmit!

  // cam.setImageSize(VC0706_640x480); // biggest
  cam.setImageSize(VC0706_320x240); // medium
  // cam.setImageSize(VC0706_160x120); // small

  // You can read the size back from the camera (optional, but maybe useful?)
  uint8_t imgsize = cam.getImageSize();
  Serial.print("Image size: ");
  if (imgsize == VC0706_640x480)
    Serial.println("640x480");
  if (imgsize == VC0706_320x240)
    Serial.println("320x240");
  if (imgsize == VC0706_160x120)
    Serial.println("160x120");

  if (!cam.takePicture())
  {
    Serial.println("Failed to snap!");
    return Photo{0, [](uint8_t n) -> uint8_t *
                 {
                   return 0;
                 }};
  }
  else
  {
    Serial.println("Picture taken!");
  }

  uint32_t jpglen = cam.frameLength();

  return Photo{jpglen, [](uint8_t n) -> uint8_t *
               {
                 return cam.readPicture(n);
               }};
}

void Camera::takePhoto(String filename)
{

  auto picture = this->takePhoto();
  Serial.print("Storing ");
  Serial.print(picture.first, DEC);
  Serial.println(" byte image.");

  if (storage.fileExists(filename))
  {
    Serial.print("Removing existing file... ");
    storage.deleteFile(filename);
    if (!storage.fileExists(filename))
      Serial.println("OK");
    else
      Serial.println("Failed!");
  }

  storage.writeFile(filename.c_str(), picture.first, picture.second);
}
