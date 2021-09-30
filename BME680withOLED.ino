/*Code 'Created'  by Chengmania on a Saturday (it was really copy pasta'ed
   For a twitch Stream in Sepembeter for 2021
   Wiring:

    oLED   SCL  -> UNO A5
    oLED   SDA  -> UNO A4
    oLED   GND  -> Ground
    oLED   VCC  -> UNO 3.3V

    BME680 SCL  -> UNO A5
    BME680 SDA  -> UNO A4
    BME680 GND  -> Ground
    BME680 VCC  -> UNO 5V

    BUTTON PIN -> UNO 3

*/

//Libraries for I2C instances of the 128x32 oLED and BME680

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SEALEVELPRESSURE_HPA (1013.25)  //set sea level for sampling purposes
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define DANGER 8
#define MEDIUM 7
#define OKAY   6

#define BUTTONPIN 3

//actviate the MBE680
Adafruit_BME680 bme; // I2C

//activate the oLED display (TODO: eresearch an other library to lighten the weight of the code.
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//switch value setup
volatile int switchValue = 0;
#define switched                            true // value if the button switch has been pressed
#define triggered                           true // controls interrupt handler
#define interrupt_trigger_type            FALLING // interrupt triggered on a RISING input

void setup() {

  pinMode(DANGER, OUTPUT);
  pinMode(MEDIUM, OUTPUT);
  pinMode(OKAY, OUTPUT);
  pinMode(BUTTONPIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTONPIN), switchMenu, interrupt_trigger_type);
  //                                    button pin, function for switching, type of trigger ie: RISING, FALLING, etc

  //initialize the BME680
  bme.begin();

  // the library initializes this with an Adafruit splash screen.
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

  //Set text size and position
  display.setTextSize(0);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text

  digitalWrite(DANGER,HIGH);
  delay(200);
  digitalWrite(MEDIUM,HIGH);
  delay(200);
  digitalWrite(OKAY,HIGH);
  delay(200);
  display.display();
  delay(2000);
}

void loop() {
  //Grab data from the BME sensor
  bme.performReading();

  display.clearDisplay();  // clear display to reset output contents
  delay(20); //give display time to clear


  switch (switchValue) {
    case 1: //Temp Dipslay
      display.setTextSize(0);
      display.setCursor(30, 0);
      display.print("Temperature:");
      display.setCursor(23, 15);
      display.setTextSize(2);
      display.print(bme.temperature);
      display.println(" C");
      break;
    case 2: //Humidity
      display.setTextSize(0);
      display.setCursor(37, 0);
      display.print("Humidity:");
      display.setCursor(23, 15);
      display.setTextSize(2);
      display.print(bme.humidity);
      display.println(" %");
      break;
    case 3: //Pressure
      display.setTextSize(0);
      display.setCursor(37, 5);
      display.print("Pressure:");
      display.setCursor(30, 20);
      display.print(bme.pressure / 100.0);
      display.print(" hPa");
      break;
    case 4: //gas
      display.setTextSize(0);
      display.setCursor(55, 5);
      display.print("Gas:");
      display.setCursor(30, 20);
      display.print(bme.gas_resistance / 1000.0);
      display.println(" KOhms");
      break;
    case 5: //altitude
      display.setTextSize(0);
      display.setCursor(42, 5);
      display.print("Altitude: ");
      display.setCursor(45, 20);
      display.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
      display.println(" m");
      break;
    default:  //summary screen
      display.setTextSize(0);
      display.setCursor(0, 0);            // Start at top-left corner
      //Enter Display data into on oLED display buffer
      display.print("Temperature: ");
      display.print(bme.temperature);
      display.println(" C");

      display.print("Humidity:    ");
      display.print(bme.humidity);
      display.println(" %");

      display.print("Pressure: ");
      display.print(bme.pressure / 100.0);
      display.println(" hPa");

      display.print("Gas:      ");
      display.print(bme.gas_resistance / 1000.0);
      display.println(" KOhms");

      //display.print("Altitude: ");
      //display.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
      //display.println(" m");
      break;
  }

  if ((bme.gas_resistance / 1000.0) > 25.00) {
    digitalWrite(OKAY, HIGH);
    digitalWrite(MEDIUM, LOW);
    digitalWrite(DANGER, LOW);
  } else if ((bme.gas_resistance / 1000.0) < 25.00 && (bme.gas_resistance / 1000.0) > 10 ) {
    digitalWrite(OKAY, LOW);
    digitalWrite(MEDIUM, HIGH);
    digitalWrite(DANGER, LOW);
  } else {
    digitalWrite(OKAY, LOW);
    digitalWrite(MEDIUM, LOW);
    digitalWrite(DANGER, HIGH);
  }
  //Display data
  display.display();
  //delay(2000); // Pause for 2 seconds
}

void switchMenu() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200)
  {
    switchValue++;
    switchValue > 5 ? switchValue = 0 : switchValue = switchValue; //short hand If statement to stay below 5
  }
  last_interrupt_time = interrupt_time;
}
