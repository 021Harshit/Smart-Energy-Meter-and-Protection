/*
==========================================================
      SMART ENERGY METER WITH LOAD PROTECTION
----------------------------------------------------------
Developed for:
Arduino UNO

Features:
✔ Voltage Monitoring
✔ Current Monitoring
✔ Power Calculation
✔ Energy Calculation (Wh & kWh)
✔ Over Voltage Protection
✔ Over Current Protection
✔ Relay Control
✔ Green LED
✔ Red LED
✔ Active Buzzer
✔ Manual Reset Button
✔ 16x2 I2C LCD
==========================================================
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

/************************************************
                PIN DEFINITIONS
************************************************/

const byte VOLTAGE_PIN = A0;
const byte CURRENT_PIN = A1;

const byte RESET_BUTTON = 2;

const byte GREEN_LED = 7;
const byte RED_LED = 8;
const byte BUZZER = 9;
const byte RELAY = 10;


/************************************************
          SIMULATION PARAMETERS
************************************************/

// Voltage Range

const float MIN_VOLTAGE = 180.0;
const float MAX_VOLTAGE = 260.0;

// Current Range

const float MAX_CURRENT = 15.0;


/************************************************
            PROTECTION LIMITS
************************************************/

const float OVER_VOLTAGE_LIMIT = 250.0;

const float OVER_CURRENT_LIMIT = 10.0;


/************************************************
             GLOBAL VARIABLES
************************************************/

float voltage = 0;

float current = 0;

float power = 0;

float energyWh = 0;

float energykWh = 0;


/************************************************
           ADC VARIABLES
************************************************/

int rawVoltage = 0;

int rawCurrent = 0;


/************************************************
           TIMING VARIABLES
************************************************/

unsigned long previousLCD = 0;

unsigned long previousEnergy = 0;

unsigned long previousBlink = 0;


/************************************************
        SYSTEM STATUS VARIABLES
************************************************/

bool fault = false;

bool relayState = true;

bool buzzerState = false;

bool displayPage = false;


/************************************************
         LCD UPDATE INTERVAL
************************************************/

const unsigned long LCD_INTERVAL = 2000;


/************************************************
        ENERGY UPDATE INTERVAL
************************************************/

const unsigned long ENERGY_INTERVAL = 1000;


/************************************************
             SETUP FUNCTION
************************************************/

void setup()
{

  pinMode(GREEN_LED,OUTPUT);

  pinMode(RED_LED,OUTPUT);

  pinMode(BUZZER,OUTPUT);

  pinMode(RELAY,OUTPUT);

  pinMode(RESET_BUTTON,INPUT_PULLUP);

  digitalWrite(RELAY,HIGH);

  digitalWrite(GREEN_LED,HIGH);

  digitalWrite(RED_LED,LOW);

  digitalWrite(BUZZER,LOW);


  lcd.init();

  lcd.backlight();

  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print(" SMART ENERGY ");

  lcd.setCursor(0,1);
  lcd.print("    METER");

  delay(2000);

  lcd.clear();

}
/************************************************
      FUNCTION : Read Average ADC Value
************************************************/

int readAverageADC(byte pin)
{
  long sum = 0;

  for(int i = 0; i < 20; i++)
  {
    sum += analogRead(pin);
    delay(2);
  }

  return sum / 20;
}


/************************************************
      FUNCTION : Read Voltage
************************************************/

void readVoltage()
{
  rawVoltage = readAverageADC(VOLTAGE_PIN);

  voltage = MIN_VOLTAGE +
            ((float)rawVoltage / 1023.0) *
            (MAX_VOLTAGE - MIN_VOLTAGE);
}


/************************************************
      FUNCTION : Read Current
************************************************/

void readCurrent()
{
  rawCurrent = readAverageADC(CURRENT_PIN);

  current = ((float)rawCurrent / 1023.0) *
            MAX_CURRENT;
}


/************************************************
      FUNCTION : Calculate Power
************************************************/

void calculatePower()
{
  power = voltage * current;
}


/************************************************
      FUNCTION : Calculate Energy
************************************************/

void calculateEnergy()
{
  unsigned long currentMillis = millis();

  if(currentMillis - previousEnergy >= ENERGY_INTERVAL)
  {
    previousEnergy = currentMillis;

    // Watt-second to Watt-hour

    energyWh += power / 3600.0;

    energykWh = energyWh / 1000.0;
  }
}


/************************************************
      FUNCTION : Read All Electrical Values
************************************************/

void updateElectricalParameters()
{
  readVoltage();

  readCurrent();

  calculatePower();

  calculateEnergy();
}


/************************************************
      FUNCTION : Voltage is Safe?
************************************************/

bool voltageSafe()
{
  if(voltage > OVER_VOLTAGE_LIMIT)
  {
    return false;
  }

  return true;
}


/************************************************
      FUNCTION : Current is Safe?
************************************************/

bool currentSafe()
{
  if(current > OVER_CURRENT_LIMIT)
  {
    return false;
  }

  return true;
}


/************************************************
      FUNCTION : System Healthy?
************************************************/

bool systemHealthy()
{
  if(voltageSafe() && currentSafe())
  {
    return true;
  }

  return false;
}
/************************************************
          LCD DISPLAY - PAGE 1
************************************************/

void displayPage1()
{
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("V:");
  lcd.print(voltage,1);
  lcd.print("V ");

  lcd.print("I:");
  lcd.print(current,1);
  lcd.print("A");

  lcd.setCursor(0,1);
  lcd.print("Relay:");

  if(relayState)
    lcd.print("ON ");
  else
    lcd.print("OFF");
}


/************************************************
          LCD DISPLAY - PAGE 2
************************************************/

void displayPage2()
{
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("P:");
  lcd.print(power,0);
  lcd.print("W");

  lcd.setCursor(0,1);
  lcd.print("E:");
  lcd.print(energykWh,3);
  lcd.print("kWh");
}


/************************************************
          LCD FAULT DISPLAY
************************************************/

void displayFault()
{
  lcd.clear();

  lcd.setCursor(0,0);

  if(voltage > OVER_VOLTAGE_LIMIT)
  {
    lcd.print("OVER VOLTAGE");
  }
  else if(current > OVER_CURRENT_LIMIT)
  {
    lcd.print("OVER CURRENT");
  }

  lcd.setCursor(0,1);
  lcd.print("PRESS RESET");
}


/************************************************
        NORMAL SYSTEM OPERATION
************************************************/

void normalOperation()
{
  relayState = true;

  digitalWrite(RELAY,HIGH);

  digitalWrite(GREEN_LED,HIGH);

  digitalWrite(RED_LED,LOW);

  digitalWrite(BUZZER,LOW);
}


/************************************************
         FAULT OPERATION
************************************************/

void faultOperation()
{
  relayState = false;

  digitalWrite(RELAY,LOW);

  digitalWrite(GREEN_LED,LOW);

  digitalWrite(RED_LED,HIGH);

  digitalWrite(BUZZER,HIGH);
}


/************************************************
      CHECK FOR FAULT CONDITIONS
************************************************/

void checkProtection()
{

  if(voltage > OVER_VOLTAGE_LIMIT)
  {
    fault = true;
  }

  if(current > OVER_CURRENT_LIMIT)
  {
    fault = true;
  }

}


/************************************************
        RESET BUTTON FUNCTION
************************************************/

void checkResetButton()
{

  if(digitalRead(RESET_BUTTON)==LOW)
  {

    delay(30);

    if(digitalRead(RESET_BUTTON)==LOW)
    {

      while(digitalRead(RESET_BUTTON)==LOW);

      if(systemHealthy())
      {
        fault=false;
      }

    }

  }

}


/************************************************
       UPDATE LCD DISPLAY
************************************************/

void updateLCD()
{

  if(millis()-previousLCD<LCD_INTERVAL)
    return;

  previousLCD=millis();

  if(fault)
  {
    displayFault();
    return;
  }

  displayPage=!displayPage;

  if(displayPage)
    displayPage1();
  else
    displayPage2();

}
/************************************************
                 MAIN LOOP
************************************************/

void loop()
{

  // Read all electrical parameters

  updateElectricalParameters();

  // Check if any fault exists

  checkProtection();

  // If system is healthy

  if(!fault)
  {
      normalOperation();
  }
  else
  {
      faultOperation();

      checkResetButton();
  }

  // Update LCD

  updateLCD();

}