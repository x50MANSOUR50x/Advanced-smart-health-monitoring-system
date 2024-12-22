#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include "DHT.h"
#include "MAX30100_PulseOximeter.h"

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          53         // Configurable, see typical pin layout above
#define DHTPIN 2

#define DHTTYPE DHT22

#define REPORTING_PERIOD_MS     500

void home_screen(void);
void onBeatDetected(void);


MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
LiquidCrystal_I2C lcd(0x27,20,4);
DHT dht(DHTPIN, DHTTYPE);

byte omarUID[4] = {0x33, 0x7F, 0xFB, 0x0D};
byte ahmedUID[4] = {0x73, 0xFD, 0x6D, 0x25};

bool omar;
bool ahmed;

byte smile[] = {
  B00000,
  B00000,
  B01010,
  B00000,
  B10001,
  B01110,
  B00000,
  B00000
};
byte mod[] = {
  B00000,
  B00000,
  B01010,
  B00000,
  B11111,
  B00000,
  B00000,
  B00000
};
byte sad[] = {
  B00000,
  B00000,
  B01010,
  B00000,
  B01110,
  B10001,
  B00000,
  B00000
};

PulseOximeter pox;
uint32_t tsLastReport = 0;

unsigned char bbm;
unsigned char sp02;
unsigned char ahmed_counter = 3;
unsigned char omar_counter = 3;
unsigned char counter = 0;

void setup() {
	Serial.begin(115200);		

  lcd.init();
  lcd.backlight();
  
  home_screen();

	while (!Serial);		
	SPI.begin();			
	mfrc522.PCD_Init();

  if (!pox.begin()) {
    // Serial.println("FAILED");
    for (;;);
  } else {
    // Serial.println("SUCCESS");
  }
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  pox.setOnBeatDetectedCallback(onBeatDetected);

  dht.begin();
}

void loop() {
  pox.update();
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    bbm = pox.getHeartRate();
    sp02 = pox.getSpO2();

    // Serial.println(bbm);
    // Serial.println(sp02);

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  if (isnan(h) || isnan(t) || isnan(f)) {
    // Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  // float hic = dht.computeHeatIndex(t, h, false);

  if(omar_counter % 2 == 0)
  {
    counter = 1;
    ahmed_counter = 3;
  }
  else if(ahmed_counter % 2 == 0)
  {
    counter = 2;
    omar_counter = 3;
  }
  else
  {
    counter = 3;
  }

  switch (counter)
  {
    case 1:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Welcome, Omar");
      lcd.setCursor(0 , 1);
      lcd.print("BPM : ");
      lcd.print(bbm);
      lcd.setCursor(0 , 2);
      lcd.print("Sp02: ");
      lcd.print(sp02);
      lcd.print("%");
      lcd.setCursor(0, 3);
      lcd.print(F("Hum:"));
      lcd.print(int(h));
      lcd.print(F("% Temp:"));
      lcd.print(int(t));
      lcd.print(F("C "));
      lcd.print(int(f));
      lcd.print(F("F"));
      if(bbm > 0)
      {
      Serial.print("Omar");
      Serial.print(',');
      Serial.print(bbm);
      Serial.print(',');
      Serial.print(sp02);
      Serial.print(',');
      Serial.print(h);
      Serial.print(',');
      Serial.print(t);
      Serial.print(',');
      Serial.print(f);
      Serial.println(); 
      }
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Welcome, Ahmed"); 
      lcd.setCursor(0 , 1);
      lcd.print("BPM : ");
      lcd.print(bbm);
      lcd.setCursor(0 , 2);
      lcd.print("Sp02: ");
      lcd.print(sp02);
      lcd.print("%");
      lcd.setCursor(0, 3);
      lcd.print(F("Hum:"));
      lcd.print(int(h));
      lcd.print(F("% Temp:"));
      lcd.print(int(t));
      lcd.print(F("C "));
      lcd.print(int(f));
      lcd.print(F("F"));
      if(bbm > 0)
      {
      Serial.print("Ahmed");
      Serial.print(',');
      Serial.print(bbm);
      Serial.print(',');
      Serial.print(sp02);
      Serial.print(',');
      Serial.print(h);
      Serial.print(',');
      Serial.print(t);
      Serial.print(',');
      Serial.print(f);
      Serial.println(); 
      }
      break;
      default:
      home_screen();
      break;
  } 
    tsLastReport = millis();
    
      if (pox.getSpO2() >= 96) {
      lcd.setCursor(15 , 1);
      lcd.write(1);                 
    }
    else if (pox.getSpO2() <= 95 && pox.getSpO2() >= 91) {
      lcd.setCursor(15 , 1);
      lcd.write(2);                 
    }
    else if (pox.getSpO2() <= 90) {
      lcd.setCursor(15 , 1);
      lcd.write(3);
    }

	if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}
  // omar = mfrc522.uid.uidByte[0] == omarUID[0] && mfrc522.uid.uidByte[1] == omarUID[1] && mfrc522.uid.uidByte[2] == omarUID[2] && mfrc522.uid.uidByte[3] == omarUID[3];
  // Serial.println(omar);
  // ahmed = mfrc522.uid.uidByte[0] == ahmedUID[0] && mfrc522.uid.uidByte[1] == ahmedUID[1] && mfrc522.uid.uidByte[2] == ahmedUID[2] && mfrc522.uid.uidByte[3] == ahmedUID[3];
  // Serial.println(ahmed);
  if(mfrc522.uid.uidByte[0] == omarUID[0] && mfrc522.uid.uidByte[1] == omarUID[1] && mfrc522.uid.uidByte[2] == omarUID[2] && mfrc522.uid.uidByte[3] == omarUID[3])
  {
    omar_counter ++;
  }
  else if(mfrc522.uid.uidByte[0] == ahmedUID[0] && mfrc522.uid.uidByte[1] == ahmedUID[1] && mfrc522.uid.uidByte[2] == ahmedUID[2] && mfrc522.uid.uidByte[3] == ahmedUID[3])
  {
    ahmed_counter ++;
  }
  // else
  // {
  //   counter = 2;
  // }
  mfrc522.PICC_HaltA();
  // Serial.print(counter);
}

}

void home_screen(void)
{
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Waiting for client..");
}


void onBeatDetected(void)
{

  // Serial.println("Beat!!!");

}

