// $50SAT (Eagle-2) RFM22B packets' receiver
// Kristaps Mengelis / x-f, 2014
// 
// 
// based on "$50sat ground station command and receive V25.bas"


#include <SPI.h>
// www.airspayce.com/mikem/arduino/RF22/‎
#include <RF22.h>


#define RFM22_SDN 3 // shutdown pin
char data[RF22_MAX_MESSAGE_LEN];

// Singleton instance of the radio
RF22 rf22;
// RF22 rf22(9);

int cnt = 0;


void setup() {
  Serial.begin(9600);
  Serial.println(F("$50SAT receiver"));

  pinMode(RFM22_SDN, OUTPUT);
  digitalWrite(RFM22_SDN, HIGH); //Turn the rfm22 radio off

  if (setupRFM22B())
    Serial.println(F("setup done"));
  else
    Serial.println(F("setup failed"));
  
}

void loop() {
  Serial.print(cnt++);
  Serial.print(F(": "));

  // Listen for data
  uint8_t buf[RF22_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  
  if (rf22.waitAvailableTimeout(30000)) {
    // Should be a message for us now   
    if (rf22.recv(buf, &len)) {

      Serial.println();
      //Serial.print("packet="); Serial.println((char*)buf);
      Serial.print("packet=");
      for (int i = 0; i < len; i++) {
        Serial.print((char)buf[i]);
        if (buf[i] == '\0') {
          Serial.println();
          break;
        }
      }
      
      // code from RF22 lib examples/rf22_snoop.ino
      // Get and print the headers
      // They will remain valid until setModeRx()is enabled and another message starts
      uint8_t to    = rf22.headerTo();
      uint8_t from  = rf22.headerFrom();
      uint8_t id    = rf22.headerId();
      uint8_t flags = rf22.headerFlags();
      Serial.print("RF22 Message To: 0x");
      Serial.print(to, HEX);
      Serial.print(" From: 0x");
      Serial.print(from, HEX);
      Serial.print(" Id: 0x");
      Serial.print(id, DEC);
      Serial.print(" Flags: 0x");
      Serial.println(flags, HEX);

      // Print the data
      int i, j;
      for (i = 0; i < len; i += 16)
      {
        // Hex
        for (j = 0; j < 16 && i+j < len; j++)
        {
          if (buf[i+j] < 16)
            Serial.print("0"); // Sigh, Serial.print does not know how to pad hex
          Serial.print(buf[i+j], HEX);
          Serial.print(" ");
        }
        // Padding on last block
        while (j++ < 16)
          Serial.print("   ");
          
        Serial.print("   ");
        // ASCII
        for (j = 0; j < 16 && i+j < len; j++)
          Serial.write(isprint(buf[i+j]) ? buf[i+j] : '.');
        Serial.println(""); 
      }
      Serial.println(F("---------------------------------------"));
    }
    
  } //else
    //Serial.print(". ");

  uint8_t rssi = rf22.lastRssi();
  Serial.print("RSSI=");
  Serial.println(rssi);

}


boolean setupRFM22B() {

  digitalWrite(RFM22_SDN, LOW); //Turn the rfm22 radio on
  delay(100);
  
  if (!rf22.init()) {
    Serial.println(F("RF22 init failed"));
    return false;
  }
  

  // ?
  // not sure if needed
  // the idea was that this should set the closest available standard config, 
  // then the registers below will be set to the correct values
  rf22.setModemConfig(RF22::FSK_Rb2Fd5);
  //rf22.setFrequency(437.511); // .507-.512?
  rf22.setTxPower(RF22_TXPOW_1DBM);

  // Tells the receiver to accept messages with any TO address, 
  // not just messages
  // addressed to this node or the broadcast address
  rf22.setPromiscuous(true);


  // This sets up the RFM22B registers to $50SAT defaults, 
  // includes options for data telemetry

  // is this needed?
  // if so, how to set this register?
  //symbol dirmoddeviation = $08 	'for a deviation of approx 5kHz
  //symbol pktdeviation = $08 	'for a deviation of approx 5khz
  // switch to FIFO\Packet handler
  //low NSEL 				
  //HSPIOUT($F1,$23,pktdeviation) 		
  //high NSEL

  
  // Crystal oscillator load capacitance, used to fine tune frequency 
  // Non TX and RX settings, hardware
  // data $60, ($09, capvaluedefault); 
  rf22.spiWrite(0x009, 201);
  // this is an individual setting for any particular RFM22B module
  // you can use the test TX code and an SDR receiver to adjust it to the correct frequency
  // 212 436.496
  // 200 437.5057
  // breakout - .511
  // Si - .512
  
  // Configure GPIO pin for Direct mod (Morse)
  // data $62, ($0D,$10) 
  // rf22.spiWrite(0x00D, 0x10);

  // IF Filter Bandwidth
  // RX only, can change with the data rate and/or deviation
  // data $64, ($1C,$29)
  rf22.spiWrite(0x01C, 0x29);
  // AFC Loop Gearshift Override
  // RX only, can change with the data rate and/or deviation
  // data $66, ($1D,$40)
  rf22.spiWrite(0x01D, 0x40);
  
  // Clock recovery oversampling ratio
  // RX only, can change with the data rate and/or deviation
  // data $68, ($20,$E8)
  rf22.spiWrite(0x020, 0xE8);
  // Clock recovery oversampling ratio
  // RX only, can change with the data rate and/or deviation
  // data $6A, ($21,$60)
  rf22.spiWrite(0x021, 0x60);
  // Clock recovery offset 1
  // RX only, can change with the data rate and/or deviation
  // data $6C, ($22,$20)
  rf22.spiWrite(0x022, 0x20);
  // Clock recovery offset 0
  // RX only, can change with the data rate and/or deviation
  // data $6E, ($23,$C5)
  rf22.spiWrite(0x023, 0xC5);
  // Clock recovery timing loop gain 1
  // RX only, can change with the data rate and/or deviation
  // data $70, ($24,$10)
  rf22.spiWrite(0x024, 0x10);
  // Clock recovery timing loop gain 0
  // RX only, can change with the data rate and/or deviation
  // data $72, ($25,$0F)
  rf22.spiWrite(0x025, 0x0F);
  // AFC Limiter
  // RX only, can change with the data rate and/or deviation
  // data $74, ($2A,$1D)
  rf22.spiWrite(0x02A, 0x1D);
  
  // Data access control
  // TX and RX, same for all data rates
  // data $76, ($30,$8C)
  rf22.spiWrite(0x030, 0x8C);
  // Header control 1
  // TX and RX, same for all data rates
  // data $78, ($32,$8C)
  rf22.spiWrite(0x032, 0x8C);
  // Header control 2
  // TX and RX, same for all data rates	
  // data $7A, ($33,$42)
  rf22.spiWrite(0x033, 0x42);
  // Preamble length
  // TX and RX, same for all data rates
  // data $7C, ($34,$10)
  rf22.spiWrite(0x034, 0x10);
  // Preamble detection control
  // RX only settings, does NOT change with the data rate
  // data $7E, ($35,$22)
  rf22.spiWrite(0x035, 0x22);

  // Transmit header 3, header is 2 characters $5
  // TX mode only
  // data $80, ($3A,"$") 
  rf22.spiWrite(0x03A, 0x24);
  // Transmit header 2
  // TX mode only
  // data $82, ($3B,"5")
  rf22.spiWrite(0x03B, 0x35);
  // Check header 3
  // RX only settings
  // data $84, ($3F,"$")
  rf22.spiWrite(0x03F, 0x24);
  // Check header 2
  // RX only settings
  // data $86, ($40,"5")
  rf22.spiWrite(0x040, 0x35);
  
  // Data rate
  // TX mode only, change with the data rate
  // data $88, ($6E,$08)
  rf22.spiWrite(0x06E, 0x08);
  // Data rate
  // TX mode only, change with the data rate
  // data $8A, ($6F,$31)
  rf22.spiWrite(0x06F, 0x31);
 
  // Modulation mode control 1
  // Frequency, modulation and deviation settings
  // data $8C, ($70,$2C)
  rf22.spiWrite(0x070, 0x2C);
  
  // These symbols determine the TX frequency. These are the settings for 437.505Mhz
  // symbol f1 = $53 			'first part of set frequency in RFM22B register $75
  // symbol f2 = $BB 			'second part of set frequency in reg $76
  // symbol f3 = $A0 			'third part of set frequency in reg $77
  // Frequency band select
  // data $8E, ($75, f1)
  rf22.spiWrite(0x075, 0x53);
  // Nominal carrier frequency 1
  // data $90, ($76, f2)
  rf22.spiWrite(0x076, 0xBB);
  // Nominal carrier frequency 2
  // data $92, ($77, f3)
  rf22.spiWrite(0x077, 0xA0);
  
  // ?
  // TX and RX off, LBT on
  // data $94, ($07,$40)
  rf22.spiWrite(0x007, 0x40);
  
  // AGC on
  // data $96, ($69,$60)
  rf22.spiWrite(0x069, 0x60);
  
  // GPIO1 to input for Lipo Charger
  // data $98, ($0C,$03)
  //rf22.spiWrite(0x00C, 0x03);

  //data $9A, ($FF,$FF) 'Init data end

  delay(100);
  
  return true;
}
