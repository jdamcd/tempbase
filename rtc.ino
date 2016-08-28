#include <RTCZero.h>
#include <WiFiUdp.h>
#include <SPI.h>

/*
 * Derived from: https://www.arduino.cc/en/Tutorial/WiFiRTC (public domain)
 */

RTCZero rtc;
WiFiUDP udp;
IPAddress timeServer(129, 6, 15, 28);
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[ NTP_PACKET_SIZE];

void syncRtc() {
  rtc.begin();

  unsigned long epoch;
  int numberOfTries = 0, maxTries = 6;
  do {
    epoch = readLinuxEpochUsingNTP();
    numberOfTries++;
  }
  while (epoch == 0 || (numberOfTries > maxTries));

  if (numberOfTries > maxTries) {
    Serial.print("NTP unreachable");
    while (1);
  } else {
    Serial.print("Epoch received: ");
    Serial.println(epoch);
    rtc.setEpoch(epoch);
  }
}

unsigned long getEpoch() {
  return rtc.getEpoch();
}

unsigned long readLinuxEpochUsingNTP() {
  udp.begin(8888);
  sendNTPpacket(timeServer);
  delay(1000);

  if (udp.parsePacket()) {
    udp.read(packetBuffer, NTP_PACKET_SIZE);

    // The timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, extract the two words.
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);

    // Combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900).
    unsigned long secsSince1900 = highWord << 16 | lowWord;

    // Convert NTP time into everyday time.
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800.
    const unsigned long seventyYears = 2208988800UL;

    udp.stop();

    // Subtract seventy years.
    return (secsSince1900 - seventyYears);
  }

  else {
    udp.stop();
    return 0;
  }
}

unsigned long sendNTPpacket(IPAddress & address) {
  memset(packetBuffer, 0, NTP_PACKET_SIZE);

  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;            // Stratum, or type of clock
  packetBuffer[2] = 6;            // Polling Interval
  packetBuffer[3] = 0xEC;         // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // Send a packet requesting a timestamp.
  udp.beginPacket(address, 123); // NTP: port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}
