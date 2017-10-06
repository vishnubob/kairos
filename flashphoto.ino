//#include "CameraControl.h"
#include "Firmata.h"
#define FIRMATA_FIRMWARE_MAJOR_VERSION 0
#define FIRMATA_FIRMWARE_MINOR_VERSION 1

void callback_string_data(char *data)
{
  String _data(data);
  _data = "<" + _data + ">";
  Firmata.sendString(_data.c_str());
}

void callback_sysex_start(byte command, byte argc, byte *argv)
{
  Firmata.sendSysex(command, argc, argv);
}

void setup()
{
  Firmata.setFirmwareVersion(FIRMATA_FIRMWARE_MAJOR_VERSION, FIRMATA_FIRMWARE_MINOR_VERSION);
  Firmata.attach(STRING_DATA, callback_string_data);
  Firmata.attach(START_SYSEX, callback_sysex_start);
  Firmata.begin(57600);
}

void loop()
{
  while (Firmata.available())
    Firmata.processInput();
}
