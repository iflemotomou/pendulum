// τοποθέτηση στη θέση ισορροπίας απομάκρυνση, μέση τιμή 5 ταλαντώσεων
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte sensorPin = 5;
const byte buttonPin = 9;
unsigned long t1 = 0;
unsigned long t2 = 0;
unsigned long periods[5];
int count = 0;
bool measuring = false;
bool crossing = false;
bool prevSensorState = HIGH;

void setup() {
  pinMode(sensorPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Press button");

  Serial.begin(9600);
}

void loop() {
  if (!measuring && digitalRead(buttonPin) == LOW) {
    count = 0;
    measuring = true;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Measuring...");
    Serial.println("Start measuring");
    delay(300); // debouncing
  }

  if (measuring) {
    bool sensorState = digitalRead(sensorPin);

    // Ανίχνευση αλλαγής HIGH → LOW (πτώση σήματος)
    if (prevSensorState == HIGH && sensorState == LOW) {
      if (!crossing) {
        t1 = millis();
        crossing = true;
      } else {
        t2 = millis();
        unsigned long period = t2 - t1;
        periods[count] = period;
        count++;
        Serial.print("T[");
        Serial.print(count);
        Serial.print("] = ");
        Serial.print(period*2/ 1000.0);
        Serial.println(" s");

        if (count >= 5) {
          float avg = 0;
          for (int i = 0; i < 5; i++) {
            avg += periods[i];
          }
          avg /= 5.0;

          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("T avg:");
          lcd.print(avg *2/ 1000.0, 3);
          lcd.setCursor(0, 1);
          lcd.print("Done");

          Serial.print("Average T = ");
          Serial.print(avg / 1000.0);
          Serial.println(" s");

          measuring = false;
        }

        crossing = false;
      }
      delay(50);  // για αποφυγή πολλαπλής ανίχνευσης
    }

    prevSensorState = sensorState;
  }
}
