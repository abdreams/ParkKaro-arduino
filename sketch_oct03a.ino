#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include "RTClib.h"

// Pins Declaration
#define ECHOpin 7
#define TRIGpin 8
int led[] = {2, 3, 4, 5, 6};
byte rowPins[4] = {A0, A1, A2, A3}; // Connect to the row pinouts of the keypad
byte colPins[3] = {9, 10, 11};      // Connect to the column pinouts of the keypad

// Variables Declaration
const byte ROWS = 4; // Four rows
const byte COLS = 3; // Three columns
char hexaKeys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};

// Variables Declaration
long duration; // Variable for the duration of sound wave travel
float distance; // Variable for the distance measurement
int charge = 5;
int amount;
long unsigned int cometime[5] = {0, 0, 0, 0, 0};
long unsigned int gotime[5] = {0, 0, 0, 0, 0};

// Object Declaration
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
LiquidCrystal_I2C lcd(0x20, 20, 4);
RTC_DS1307 rtc;

void setup()
{
    Serial.begin(9600);
    lcd.init();
    lcd.backlight();
    pinMode(TRIGpin, OUTPUT);
    pinMode(ECHOpin, INPUT);
    
    // Set all LED indicators to LOW (empty slots)
    for (int i = 0; i < 5; i++)
    {
        digitalWrite(led[i], LOW);
    }

    Serial.println("Made by ParkKaro");
    lcd.setCursor(0, 0);
    lcd.print("Made by");
    lcd.setCursor(0, 1);
    lcd.print("INSIDIOUS");
    delay(2000);
    Serial.println("Setup complete");

    
    // Display the initial message indicating empty slots
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("6 Slots Empty");
    delay(2000);
    lcd.clear();
    
    if (!rtc.begin())
    {
        Serial.println("Couldn't find RTC");
        Serial.flush();
        abort();
    }
}


// Function for calculating Distance
int dist()
{
    digitalWrite(TRIGpin, LOW);
    delayMicroseconds(4);
    digitalWrite(TRIGpin, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGpin, LOW);
    duration = pulseIn(ECHOpin, HIGH);
    distance = duration * 0.034 / 2;
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    return distance;
}

// Loop Function
void loop()
{
    DateTime now = rtc.now();
    char customKey = customKeypad.getKey();
    if (customKey)
    {
        Serial.println(customKey);
        for (int i = 0; i < 5; i++)
        {
            if (String(customKey) == String(i + 1))
            {
                if (digitalRead(led[i]) == HIGH)
                {
                    digitalWrite(led[i], LOW);
                    gotime[i] = now.unixtime();
                    amount = charge * (gotime[i] - cometime[i]);
                    lcd.setCursor(0, 0);
                    lcd.print("Your Bill Amount is: ");
                    lcd.setCursor(0, 1);
                    lcd.print(amount);
                    Serial.print("Your Bill Amount is: ");
                    Serial.println(amount);
                    lcd.setCursor(0, 2);
                    lcd.print("Thanks for Visit");
                    Serial.println("Thanks for Visit");
                    delay(1000);
                    lcd.clear();
                }
                else
                {
                    for (int j = 0; j < 5; j++)
                    {
                        led[j] = LOW;
                    }
                    lcd.print("The Slot is Already Empty");
                    Serial.println("The Slot is Already Empty");
                    delay(1000);
                    for (int j = 0; j < 5; j++)
                    {
                        led[j] = LOW;
                    }
                }
            }
        }

        if (dist() < 100)
        {
            for (int i = 0; i < 5; i++)
            {
                if (digitalRead(led[i]) == LOW)
                {
                    while (dist() < 100)
                    {
                        lcd.setCursor(0, 0);
                        lcd.print("Park your car at slot ");
                        lcd.print(i + 1);
                        digitalWrite(led[i], HIGH);
                        cometime[i] = now.unixtime();
                        for (int j = 0; j < 5; j++)
                        {
                            led[j] = LOW;
                        }
                        break;
                    }
                }
            }

            if (digitalRead(led[0]) == HIGH && digitalRead(led[1]) == HIGH && digitalRead(led[2]) == HIGH && digitalRead(led[3]) == HIGH && digitalRead(led[4]) == HIGH)
            {
                Serial.println("No More Slots");
                lcd.clear();
                lcd.print("No More Slots");
                lcd.clear();
            }
        }
    }
}
