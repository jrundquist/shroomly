#pragma once

#include <Wire.h>
#include <SPI.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Stream.h>
#include <ArduCAM.h>

#define CS_PIN 27

#define BUF_SIZE size_t(2048)

class CameraStream : public Stream
{
public:
  CameraStream(ArduCAM &camera) : cam(camera), buf_pos(0), buf_len(0), total_read(0), is_header(false), is_done(false)
  {
    // total_len = size_t(round(1024. * 65));
    total_len = cam.read_fifo_length();
    remaining_len = size_t(total_len);
    log_d("CameraStream initialized with FIFO size %d", total_len);

    readNextChunk();
    next_byte = buf[buf_pos++];
  }

  int read() override
  {
    // Return -1 if no data available
    if (available() <= 0)
      return -1;

    // Store the next byte in a local variable
    int result = next_byte;

    // Advance to the next byte
    readNextByte();

    // Increment the total number of bytes read
    total_read++;

    // Return the next byte
    return result;
  }

  size_t readBytes(uint8_t *buff, size_t len) override
  {
    size_t count = 0;
    while (count < len && available() > 0)
    {
      int byte = read();
      if (byte == -1)
      {
        break;
      }
      buff[count++] = byte;
    }
    return count;
  }

  int peek() override
  {
    if (available() <= 0)
    {
      return -1;
    }

    // Return the next byte without advancing the read position
    return next_byte;
  }

  size_t write(uint8_t) override
  {
    return 0;
  }

  void flush() override {}

  int available() override
  {
    return total_len - total_read;
  }

private:
  void readNextByte()
  {
    if (available() <= 0)
    {
      // No more data available
      next_byte = -1;
      return;
    }

    // Get the next byte from the buffer
    next_byte = buf[buf_pos++];

    if (buf_pos >= buf_len)
    {
      // Read the next chunk from the camera's FIFO
      readNextChunk();
    }
  }

  void readNextChunk()
  {
    // Read the next chunk from the camera's FIFO
    log_d("Reading next chunk from SPI interface");
    buf_len = min(remaining_len, BUF_SIZE);
    size_t len = buf_len;
    remaining_len -= buf_len;
    cam.CS_HIGH();
    cam.CS_LOW();
    cam.set_fifo_burst();

    int skips = 0;
    int i = 0;
    while (i < len)
    {
      // Serial.println("   >> " + String(len));
      if (is_done)
      {
        buf[i++] = 0x00;
        skips++;
        continue;
      }
      prev_byte = byte;
      byte = SPI.transfer(0x00);
      transfers++;
      if ((prev_byte == 0xFF) && (byte == 0xD9))
      {
        log_d("found end of file. All remaining bytes will be 0x00");
        // Found the end of the file. Save the bytes.
        buf[i++] = byte; // save the last  0XD9
        cam.CS_HIGH();
        cam.flush_fifo();
        // From now on, only output zeros
        is_done = true;
      }

      if (is_header == true)
      {
        buf[i++] = byte;
      }
      else if ((prev_byte == 0xFF) && (byte == 0xD8))
      {
        // Found the start of the header info. Save the bytes.
        log_d("found JPEG header @ %d", transfers - 2);
        is_header = true;
        buf[i++] = prev_byte;
        buf[i++] = byte;
      }
    }

    log_d("finished reading next buffer of size %d, remaining %d, total %d", buf_len, remaining_len, total_len);
    cam.CS_HIGH();
    buf_pos = 0;
  }

  ArduCAM &cam;
  uint8_t buf[BUF_SIZE];
  int next_byte = -1;
  size_t buf_pos, buf_len, remaining_len, total_len, total_read;
  bool is_header, is_done;
  int transfers = 0;

  uint8_t byte = 0x00;
  uint8_t prev_byte = 0x00;
};

class Camera
{
private:
  uint32_t imageId = 0;

public:
  void captureImage();
  uint32_t imageLength();

  template <typename T>
  void writeImage(T &outputStream);
  void uploadImage(String url);

  String getImageId()
  {
    return String(this->imageId);
  }

  void init();
  void loop();
};

extern Camera camera;