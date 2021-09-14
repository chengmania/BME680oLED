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

//actviate the MBE680
Adafruit_BME680 bme; // I2C

//activate the oLED display (TODO: eresearch an other library to lighten the weight of the code.
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {

  pinMode(DANGER, OUTPUT);
  pinMode(MEDIUM, OUTPUT);
  pinMode(OKAY, OUTPUT);
  

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
  

}

void loop() {
  //Grab data from the BME sensor
  bme.performReading();
  
  display.clearDisplay();  // clear display to reset output contents
  delay(20); //give display time to clear
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

  if ((bme.gas_resistance / 1000.0) > 25.00) { 
    digitalWrite(OKAY, HIGH);
    digitalWrite(MEDIUM, LOW);
    digitalWrite(DANGER, LOW);
  }else if ((bme.gas_resistance / 1000.0) < 25.00 && (bme.gas_resistance / 1000.0) > 10 ){
    digitalWrite(OKAY, LOW);
    digitalWrite(MEDIUM, HIGH);
    digitalWrite(DANGER, LOW);
  }else {
    digitalWrite(OKAY, LOW);
    digitalWrite(MEDIUM, LOW);
    digitalWrite(DANGER, HIGH);
  }

  //display.print("Altitude: ");
  //display.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  //display.println(" m");

  //Display data
  display.display();
  delay(2000); // Pause for 2 seconds
}
