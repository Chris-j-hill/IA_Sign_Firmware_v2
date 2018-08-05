/*
  Multiple Serial test

  Receives from the main serial port, sends to the others.
  Receives from serial port 1, sends to the main serial (Serial 0).

  This example works only with boards with more than one serial like Arduino Mega, Due, Zero etc.

  The circuit:
  - any serial device attached to Serial port 1
  - Serial Monitor open on Serial port 0

  created 30 Dec 2008
  modified 20 May 2012
  by Tom Igoe & Jed Roach
  modified 27 Nov 2015
  by Arturo Guadalupi

  This example code is in the public domain.
*/


void setup() {
  // initialize both serial ports:
  Serial.begin(115200);
  Serial1.begin(19200, SERIAL_8N2);

}

void loop() {
  // read from port 1, send to port 0:
  if (Serial1.available()) {
    Serial.println();
    int inByte = Serial1.read();


    Serial.print(inByte);
    Serial.print("\t");
    Serial.println((char)inByte);
  }
//
//  // read from port 0, send to port 1:
//  if (Serial.available()) {
//    int inByte = Serial.read();
//    Serial1.write(inByte);
//  }
}
