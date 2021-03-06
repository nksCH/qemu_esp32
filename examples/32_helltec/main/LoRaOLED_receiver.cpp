#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`

#include <SPI.h>
#include <LoRa.h>
// #include "SSD1306.h"

#define SCK     5    // GPIO5  -- SX127x's SCK
#define MISO    19   // GPIO19 -- SX127x's MISO
#define MOSI    27   // GPIO27 -- SX127x's MOSI
#define SS      18   // GPIO18 -- SX127x's CS
#define RST     14   // GPIO14 -- SX127x's RESET
#define DI00 26 // GPIO26 -- SX127x's IRQ(Interrupt Request)

SSD1306 display(0x3c, 4, 15);
String rssi = "RSSI --";
String packSize = "--";
String packet ;

//OLED pins to ESP32 GPIOs via this connection:
//OLED_SDA — GPIO4
//OLED_SCL — GPIO15
//OLED_RST — GPIO16

 

// WIFI_LoRa_32 ports

// GPIO5 — SX1278’s SCK
// GPIO19 — SX1278’s MISO
// GPIO27 — SX1278’s MOSI
// GPIO18 — SX1278’s CS
// GPIO14 — SX1278’s RESET
// GPIO26 — SX1278’s IRQ(Interrupt Request)



#define SS 18
#define RST 14
#define DI0 26
// #define BAND 429E6

// LoRa Settings 
#define BAND 868100000.00
// #define BAND 434500000.00
#define spreadingFactor 7
#define SignalBandwidth 62.5E3
//#define SignalBandwidth 31.25E3
#define PABOOST true


#define codingRateDenominator 8

// ADC? Battery voltage
// const uint8_t vbatPin = 34;
// float VBAT;  // battery voltage from ESP32 ADC read


void loraData(){
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(10 , 16 , "Received "+ packSize + " bytes");
  display.drawStringMaxWidth(10 , 32 , 128, packet);
  display.drawString(10, 0, rssi);  
  display.display();
}

void cbk(int packetSize) {
  packet ="";
  packSize = String(packetSize,DEC);
  for (int i = 0; i < packetSize; i++) { packet += (char) LoRa.read(); }
  rssi = "RSSI " + String(LoRa.packetRssi(), DEC) ;
  loraData();
}


void setup() {
  pinMode(16,OUTPUT);
  digitalWrite(16, LOW); // set GPIO16 low to reset OLED
  delay(50);
  digitalWrite(16, HIGH);

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  
  Serial.begin(115200);
  while (!Serial); //if just the the basic function, must connect to a computer
  delay(1000);
  
  Serial.println("LoRa Receiver");
  display.drawString(5,5,"LoRa Receiver");
  display.display();
  SPI.begin(SCK,MISO,MOSI,SS);
  //SPI.begin(5,19,27,18);
  LoRa.setPins(SS,RST,DI0);

/*     
  pinMode(vbatPin, INPUT);
  VBAT = (120.0/20.0) * (float)(analogRead(vbatPin)) / 1024.0; // LiPo battery voltage in volts
  Serial.println("Vbat = "); Serial.print(VBAT); Serial.println(" Volts");
*/

  if (!LoRa.begin(BAND,PABOOST)) {
    display.drawString(5,25,"Starting LoRa failed!");
    display.display();
    while (1);
  }
  Serial.println("LoRa Initial OK!");
  
  Serial.print("LoRa Frequency: ");
  Serial.println(BAND);
  
  Serial.print("LoRa Spreading Factor: ");
  Serial.println(spreadingFactor);
  LoRa.setSpreadingFactor(spreadingFactor);
  
  Serial.print("LoRa Signal Bandwidth: ");
  Serial.println(SignalBandwidth);
  LoRa.setSignalBandwidth(SignalBandwidth);

  LoRa.setCodingRate4(codingRateDenominator);
  
  display.drawString(0,0,"LoRa Initializing OK!");
  display.drawString(20, 10, "Wait for incomm data...");
  display.display();
  delay(1000);
  //LoRa.onReceive(cbk);
  LoRa.receive();
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packets
    Serial.print("Received packet. ");
    cbk(packetSize);
 #if 0
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(3, 0, "Received packet ");
    display.display();
    
    // read packet
    while (LoRa.available()) {
      String data = LoRa.readString();
      Serial.print(data);
      display.drawString(20,22, data);
      display.display();
    }
    
    // print RSSI of packet
    Serial.print(" with RSSI ");
    Serial.println(LoRa.packetRssi());
    Serial.print(" with SNR ");
    Serial.println(LoRa.packetSnr());
    // display.drawString(0, 45, "RSSI: ");
    // display.drawString(50, 45, (String)LoRa.packetRssi());
    
    display.drawString(0, 45, (String)LoRa.packetRssi() + "dB (" + (String)LoRa.packetSnr() +"dB)");
        
    display.display();
#endif
  }
  delay(10);
}
