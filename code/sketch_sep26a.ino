#include <LiquidCrystal.h> //Connect LCD
LiquidCrystal lcd(12,11,7,6,5,4); // init LCD

// Initialize Pins
int analogPin = 0;
int chargePin = 9;
int dischargePin = 10; //speeds up discharging process, not necessary though

// Initialize Resistor
int resistorValue = 10000;

// Initialize Timer
unsigned long startTime;
unsigned long elapsedTime;

// Initialize Capacitance Variables
float microFarads;                
float nanoFarads;
float self_cap = 0.025; // set 0 and see how much need to set on display
float last_displayed = 0;

void setup()
{
  lcd.clear();
  pinMode(chargePin, OUTPUT);     
  digitalWrite(chargePin, LOW);  
  Serial.begin(9600); // Necessary to print data to serial monitor over USB
}

float getStandartCap(float valMicroFarads) {
  float in_values[] = {1,1.2,1.5,1.8, 2.2,2.7,3.3,3.9,4.7,5.6,6.8,8.2};
  int sign_count = 0;
  float value = valMicroFarads;
  // проводим значение к диапазону 1..10
  while (value < 1) {
    sign_count--;
    value = (float)value * 10;
    }
  while (value >= 10) {
    sign_count++;
    value = (float)value / 10;
    }  
  
  int i = 0;
  float res_cap = 0;
  float res_value = 10;
  float deference = 0;

  for(i=0;i<=11;i++){  
      deference = abs(value - in_values[i]);
      if (deference < res_value) {
        res_value = deference;
        res_cap = in_values[i];
        } 
      }
   return res_cap * pow(10,sign_count);
  }


void loop()
{
  digitalWrite(chargePin, HIGH); // Begins charging the capacitor
  startTime = micros(); // Begins the timer
  unsigned long maxwait_time = startTime + 200000;
  // 200000 - it's 200ms (nice mac time for charging)
  while(analogRead(analogPin) < 648 && maxwait_time > micros())
  {       
    // Does nothing until capacitor reaches 63.2% of total voltage
  }
  // check - it was timeout or charging
  if (micros() < maxwait_time) {
    // we charged
    if (analogRead(analogPin) >= 648) {

      elapsedTime= (float)(micros() - startTime); // Determines how much time it took to charge capacitor
      microFarads = ((float)elapsedTime / resistorValue);
      if (microFarads <= self_cap) {
        microFarads = 0;        
      } else {
        microFarads = getStandartCap(microFarads - self_cap);
        }
      if (microFarads != last_displayed) {
        if (microFarads > 1) // Determines if units should be micro or nano and prints accordingly
        {
          // some value
          lcd.clear();
          lcd.print("C:    ");
          lcd.print((float)microFarads);       
          lcd.print(" uF");         
        } else if (microFarads == 0) {
          // empty
          lcd.clear();
          lcd.print("C:      --||--  ");
          
        } else {
          // some value
          nanoFarads = (float)(microFarads * 1000.0);      
          lcd.clear();
          lcd.print("C:    ");
          lcd.print(nanoFarads);       
          lcd.print(" nF");          
          
        }
        // fix value for block blinking
        last_displayed = microFarads;
        }
      }
      digitalWrite(chargePin, LOW); // Stops charging capacitor
      pinMode(dischargePin, OUTPUT); 
      digitalWrite(dischargePin, LOW); // Allows capacitor to discharge    
      while(analogRead(analogPin) > 0)
      {
        // Do nothing until capacitor is discharged      
      }
      // wait, some time
      delay(50);
      pinMode(dischargePin, INPUT); // Prevents capacitor from discharging  
      // and wait more :-)
      delay(100);
      }
}

