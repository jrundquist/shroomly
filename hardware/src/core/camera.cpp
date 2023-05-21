#include "./camera.h"

// This demo can only work on OV5640_MINI_5MP_PLUS or OV5642_MINI_5MP_PLUS platform.
#if !(defined(OV5642_MINI_5MP_PLUS))
#error Please select the hardware platform and camera module in the ../libraries/ArduCAM/memorysaver.h file
#endif

namespace
{
  // set pin 7 as the slave select for the digital pot:
  // #define SD_CS 9
  bool is_header = false;
  bool is_init = false;

  int total_time = 0;
  uint8_t resolution = OV2640_800x600;
  ArduCAM myCAM(OV5642, CS_PIN);

  template <typename T>
  void writeImage(T &outputStream)
  {
    // Write the image data to the output stream
    int i = 0;
    uint8_t temp = 0, temp_last = 0;
    uint32_t length = 0;
    bool is_header = false;

    byte buf[256];
    length = myCAM.read_fifo_length();
    Serial.print(F("The fifo length is :"));
    Serial.println(length, DEC);

    if (length >= MAX_FIFO_SIZE) // 8M
    {
      Serial.println("Over size.");
      return;
    }
    if (length == 0) // 0 kb
    {
      Serial.println(F("Size is 0."));
      return;
    }
    myCAM.CS_LOW();
    myCAM.set_fifo_burst();
    while (length--)
    {
      temp_last = temp;
      temp = SPI.transfer(0x00);
      // Read JPEG data from FIFO
      if ((temp == 0xD9) && (temp_last == 0xFF)) // If find the end ,break while,
      {
        buf[i++] = temp; // save the last  0XD9
        // Write the remain bytes in the buffer
        myCAM.CS_HIGH();
        outputStream.write(buf, i);
        Serial.println(F("Image save OK."));
        is_header = false;
        i = 0;
      }
      if (is_header == true)
      {
        // Write image data to buffer if not full
        if (i < 256)
          buf[i++] = temp;
        else
        {
          // Write 256 bytes image data to file
          myCAM.CS_HIGH();
          outputStream.write(buf, 256);
          i = 0;
          buf[i++] = temp;
          myCAM.CS_LOW();
          myCAM.set_fifo_burst();
        }
      }
      else if ((temp == 0xD8) & (temp_last == 0xFF))
      {
        is_header = true;
        buf[i++] = temp_last;
        buf[i++] = temp;
      }
    }
    Serial.println("Image Sent OK");
    myCAM.flush_fifo();
  }
}

Camera camera;

void Camera::init()
{
  if (is_init)
    return;
  // put your setup code here, to run once:
  uint8_t vid, pid;
  uint8_t temp;

  Wire.begin();
  Serial.println(F("ArduCAM Start!"));
  // set the CS as an output:
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);
  // initialize SPI:
  SPI.begin();

  pinMode(12, OUTPUT);
  digitalWrite(12, HIGH);

  // Reset the CPLD
  myCAM.write_reg(0x07, 0x80);
  delay(100);
  myCAM.write_reg(0x07, 0x00);
  delay(100);

  int attempts = 0;

  while (attempts++ < 10)
  {
    // Check if the ArduCAM SPI bus is OK
    myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
    temp = myCAM.read_reg(ARDUCHIP_TEST1);
    if (temp != 0x55)
    {
      Serial.println(F("SPI interface Error!"));
      Serial.println(temp);
      delay(1000);
      continue;
    }
    else
    {
      Serial.println(F("SPI interface OK."));
      break;
    }
  }

  attempts = 0;
  while (attempts++ < 10)
  {
    // Check if the camera module type is OV5642
    myCAM.rdSensorReg16_8(OV5642_CHIPID_HIGH, &vid);
    myCAM.rdSensorReg16_8(OV5642_CHIPID_LOW, &pid);
    if ((vid != 0x56) || (pid != 0x42))
    {
      Serial.println(F("Can't find OV5642 module!"));
      delay(1000);
      continue;
    }
    else
    {
      Serial.println(F("OV5642 detected."));
      break;
    }
  }
  // Initialize SD Card
  // Change to JPEG capture mode and initialize the OV5640 module
  myCAM.set_format(JPEG);
  myCAM.InitCAM();
  myCAM.set_bit(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);
  myCAM.OV5642_set_JPEG_size(OV5642_1600x1200);

  myCAM.flush_fifo();
  myCAM.clear_fifo_flag();

  is_init = true;
}

void Camera::captureImage()
{
  Serial.println(F("start capture."));
  myCAM.start_capture();
  auto total_time = millis();
  while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK))
    ;
  total_time = millis() - total_time;
  Serial.println(F("CAM Capture Done."));
  Serial.print(F("capture total_time used (in miliseconds):"));
  Serial.println(total_time, DEC);
}

uint32_t Camera::imageLength()
{
  return myCAM.read_fifo_length();
}

void Camera::loop()
{
}

template <typename T>
void Camera::writeImage(T &outputStream)
{
  ::writeImage(outputStream);
}