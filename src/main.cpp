#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ServoInput.h>

#define ONBOARDLEDPIN 21
#define NUMPIXELS 36
#define PIXELSPIN 7
#define TX 43
#define RX 44

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIXELSPIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel intled(1, ONBOARDLEDPIN, NEO_GRB + NEO_KHZ800);

int brightness = 255;
float angle = 0.0;
float oldangle = 0.0;

ServoInputPin<RX> servo;

void GetColorFromAngle(float angle, uint8_t &r, uint8_t &g, uint8_t &b){
  if(angle < 0) angle = 0;
  if(angle > 180) angle = 180;

  if(angle <= 90){                     //<=0% (90°) LEDs aus
    r = 0;
    g = 0;
    b = 0;
  } else if(angle <= 140){              //0% - 54% rot bis orange
    r = map(angle, 90, 140, 20, 255);
    g = map(angle, 90, 140, 0, 120);
    b = map(angle, 90, 140, 0, 0);
  } else if(angle <= 160){              //54% - 76% orange bis warmweiß
    r = map(angle, 140, 160, 255, 255);
    g = map(angle, 140, 160, 120, 200);
    b = map(angle, 140, 160, 0, 180);
  } else {                              //76% - 100% warmweiß bis blauweiß
    r = map(angle, 160, 180, 255, 204);
    g = map(angle, 160, 180, 200, 232);
    b = map(angle, 160, 180, 180, 255);
  }
}

void setup() {

  Serial.begin(9600);
  //while (!Serial) {
  //  ; // wait for serial port to connect. Needed for native USB port only
  //}
  delay(1000);
  Serial.println();
  Serial.println("\n\nESP32 serial");

  pixels.begin();
  intled.begin();

  //setzen der Helligkeit auf 50%
  pixels.setBrightness(brightness);
  intled.setBrightness(brightness);

  servo.attach();  // attaches the servo input interrupt

  // while (servo.available() == false) {
	// 	Serial.println("Waiting for servo signal...");
	// 	delay(500);
	// }

  angle = 0.0;
}

void loop() {
  if(Serial.available()){
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if(cmd.startsWith("b ")) {
      String valueStr = cmd.substring(2);
      int value = valueStr.toInt();
      if(value >= 0 && value <= 255) {
        brightness = value;
        intled.setBrightness(brightness);
        pixels.setBrightness(brightness);

        Serial.print("Setting brightness to ");
        Serial.println(brightness);

      } else {
        Serial.println("Brightness value must be between 0 and 255");
      }
    } else {
      Serial.println("Unknown command");
    }

  }

  if(servo.available() == true){
    angle = servo.getAngle();  // get angle of servo (0 - 180)
    Serial.print("Servo Angle: ");
    Serial.println(angle);
  }
  
  if(oldangle != angle){
    uint8_t r, g, b;
    
    GetColorFromAngle(angle, r, g, b);
    Serial.print("Color RGB: ");
    Serial.print(r);
    Serial.print(", ");
    Serial.print(g);
    Serial.print(", ");
    Serial.println(b);

    uint32_t color = pixels.Color(r, g, b);

    //setzen der Farbe an der NeoPixel
    intled.clear();
    intled.setPixelColor(0, color);
    //absenden / anzeigen des Farbwertes
    intled.show();

    //set pixels
    pixels.clear();
    for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i, color);
    }
    pixels.show();

    oldangle = angle;
  }
  //einlegen einer kleinen Pause von 500 Millisekunden
  delay(50);
}
