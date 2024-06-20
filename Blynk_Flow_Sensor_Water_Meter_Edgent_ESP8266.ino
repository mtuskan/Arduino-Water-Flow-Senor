// Author: Michael Tuskan
// Creation Date: 08-24-2022
// Program Function: HPA Flow Water Sensor + Blynk Application Interface

BlynkTimer timer;
// Fill-in information from your Blynk Template here
#define BLYNK_TEMPLATE_ID "TMPL4swrJD6L"
#define BLYNK_DEVICE_NAME "Quickstart Template"
//#define BLYNK_AUTH_TOKEN "zJ05bXti_20sb4Qqrmw80l0xdiYLLZpt"

#define BLYNK_FIRMWARE_VERSION        "2.5.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG



volatile long     pulseCount = 0;
float             flowRate;
unsigned int      flowMilliLitres;
unsigned long     totalMilliLitres;
float             totalLitres;
float             totalLitresold;
unsigned long     oldTime;       

//____________________________________________________________________________________________________________________________________
#define VPIN_FLOW_RATE       V2
#define PULSE_PIN            4 

void pulseCounter()
{
  pulseCount++;
}

//____________________________________________________________________________________________________________________________________

// Uncomment your board, or configure a custom board in Settings.h
//#define USE_SPARKFUN_BLYNK_BOARD
//#define USE_NODE_MCU_BOARD
//#define USE_WITTY_CLOUD_BOARD
#define USE_WEMOS_D1_MINI

#include "BlynkEdgent.h"

//____________________________________________________________________________________________________________________________________

BLYNK_WRITE(VPIN_TOTAL_LITERS)
{
  totalLitresold = param.asFloat();
}

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(VPIN_RESET)//***param = V0
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
  Blynk.virtualWrite(V1, value);
  
  int resetdata = param.asInt();
  if (resetdata == 0) {
    Serial.println("Clearing Data");
    Blynk.virtualWrite(VPIN_TOTAL_LITERS, 0);
    Blynk.virtualWrite(VPIN_FLOW_RATE, 0);
    flowRate = 0;
    flowMilliLitres = 0;
    totalMilliLitres = 0;
    totalLitres = 0;
    totalLitresold = 0;
  }
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
//  // Change Web Link Button message to "Congratulations!"
//  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
//  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
//  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");

 Blynk.syncVirtual(VPIN_TOTAL_LITERS); //gets last know value of V1 virtual pin
}

void sendtoBlynk()  // In this function we are sending values to blynk server
{
  Blynk.virtualWrite(VPIN_TOTAL_LITERS, totalLitres);          // Total water consumption in liters (L)
  Blynk.virtualWrite(VPIN_FLOW_RATE, flowRate);            // Displays the flow rate for this second in liters / minute (L/min)
  Blynk.virtualWrite(VPIN_FLOW_RATE, flowRate);            // Displays the number of liters flowed in second (mL/Sec)
}

void pulseCounter()
{
  pulseCount++;
}

void flow()
{

  if ((millis() - oldTime) > 1000)   // Only process counters once per second
  {
    detachInterrupt(PULSE_PIN);
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount ) / 8.2; // / FLOW_CALIBRATION;
    oldTime = millis();
    flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;
    totalLitres = totalLitresold + totalMilliLitres * 0.001;
    unsigned int frac;

    // Print the flow rate for this second in liters / minute
    Serial.print("flowrate: ");
    Serial.print(int(flowRate));  // Print the integer part of the variable

    Serial.print(".");             // Print the decimal point
    frac = (flowRate - int(flowRate)) * 10; // Determine the fractional part. The 10 multiplier gives us 1 decimal place.
    Serial.print(frac, DEC) ;      // Print the fractional part of the variable
    Serial.print("L/min");

    Serial.print("  Current Liquid Flowing: ");  // Print the number of liters flowed in this second
    Serial.print(flowMilliLitres);
    Serial.print("mL/Sec");

    Serial.print("  Output Liquid Quantity: ");  // Print the cumulative total of liters flowed since starting
    Serial.print(totalLitres);
    Serial.println("L");

    pulseCount = 0;  // Reset the pulse counter so we can start incrementing again

    attachInterrupt(PULSE_PIN, pulseCounter, FALLING);    // Enable the interrupt again now that we've finished sending output
  }

}
 void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(VPIN_FLOW_RATE, millis() / 1000);
}  

//_____________________________________________________________________________________________________________________________________
void setup()
{
  Serial.begin(9600);
  //delay(100);
  
  //_____________________________________________________________________________________________________________________________________
  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;
  totalLitresold    = 0;

  pinMode(PULSE_PIN,INPUT);
  
  attachInterrupt(PULSE_PIN, pulseCounter, FALLING);

  timer.setInterval(10000L, sendtoBlynk); // send values blynk server every 10 sec
  //_____________________________________________________________________________________________________________________________________

  BlynkEdgent.begin();

}

void loop() {
  BlynkEdgent.run();
  timer.run();
  flow();


}
