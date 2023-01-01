// OLED DISPLAY
#include <U8glib.h>
// SERVO MOTORS
#include <Servo.h>

U8GLIB_SH1106_128X64 oled(U8G_I2C_OPT_NONE);

// PIN CONFIGURATION
#define ECHO_PIN 2
#define TRIG_PIN 3
#define PSERVO_PIN 4
#define FSERVO_PIN 5
#define YELLOW_PIN 6
#define GREEN_PIN 7
#define RED_PIN 8
#define BUZZER_PIN 9
#define PUMP_PIN 10
#define TEMP_PIN A0
#define MOIST_PIN A1
#define WATER_PIN A2
#define TURB_PIN A3

// GLOBAL VARIABLES.
unsigned int pServoStart = 0, fServoStart = 0;
int timeMics = 0, distance, moisture, water;
float tempC, turb;

// SERVO MOTORS
Servo pServo;  // Poultry Servo
Servo fServo;  // Fish Servo

// GET CURRENT RUNTIME
int getCurrentTime()
{
  return millis() / 1000;
}

// CALCULATE DISTANCE WITH ULTRASONIC
int calculateDistance()
{
  for (int i = 0; i < 10; i++)
  {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    timeMics += pulseIn(ECHO_PIN, HIGH);
  }
  timeMics = timeMics / 10;
  distance = timeMics / 29 / 2;
  return distance;
}

// CALCULATE TEMPERATURE
void calculateTemp()
{
  float sensorTemp, avgTemp = 0;
  int count = 0;

  while (count < 500)
  {
    sensorTemp = analogRead(TEMP_PIN);
    if (sensorTemp != 0.00)
    {
      avgTemp = avgTemp + sensorTemp;
      count++;
    }
  }
  avgTemp = avgTemp / 500;
  tempC = avgTemp * 0.48828125;
  // Serial.println(tempC);
}

// LED CONTROL
void tempLedControl()
{
  if (tempC < 40 && tempC > 15)
  {
    digitalWrite(YELLOW_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(RED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }
  else if (tempC >= 40)
  {
    digitalWrite(YELLOW_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
  }
  else
  {
    digitalWrite(YELLOW_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(RED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }
}

// CALCULATE MOISTURE
void calculateMoisture()
{
  for (int i = 0; i < 10; i++)
  {
    moisture += analogRead(MOIST_PIN);
  }
  moisture = moisture / 10;
  moisture = map(moisture, 1023, 0, 0, 100);

  if (moisture <= 20)
  {
    digitalWrite(PUMP_PIN, HIGH);

  }
  else
  {
    digitalWrite(PUMP_PIN, LOW);
  }
}

// FISH WATER LEVEL
void checkWater()
{
  water = analogRead(WATER_PIN);
  water = map(water, 0, 1023, 0, 100);
}

// WATER QUALITY MONITOR
void checkWaterQuality() {
  for (int i = 0; i < 10; i++)
  {
    turb += analogRead(TURB_PIN);
  }
  turb = turb / 10;
  turb = turb * (5.0 / 1023.0);
  Serial.println(turb);
}

// OLED DISPLAY
void showInfo()
{
  oled.setFont(u8g_font_profont12);

  oled.setPrintPos(0, 10);
  oled.print("TEMP: ");
  oled.print(tempC);
  oled.print(" C");

  oled.setPrintPos(0, 20);
  oled.print("WATER: ");
  if (moisture <= 20)
  {
    oled.print("NO WATER");
  }
  else
  {
    oled.print("OKAY");
  }

  oled.setPrintPos(0, 30);
  oled.print("FISH WATER: ");
  if (water <= 20)
  {
    oled.print("NO WATER");
  }
  else
  {
    oled.print("OKAY");
  }

  oled.setPrintPos(0, 40);
  oled.print("WATER Q: ");
  oled.print(turb);
}

void setup()
{
  // SERIAL COMMUNICATION
  Serial.begin(9600);
  // ULTRASONIC
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  // SERVO MOTORS
  
  pServo.attach(PSERVO_PIN);
  pServo.write(0);
  fServo.attach(FSERVO_PIN);
  fServo.write(0);

  // TEMPERATURE
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // PUMPS
  pinMode(PUMP_PIN, OUTPUT);
  oled.begin();
}

void loop()
{
  // POULTRY FEEDING
  if (calculateDistance() <= 10)
  {
    if (getCurrentTime() - pServoStart >= 15)
    {
      pServo.write(45);
      delay(1000);
      pServo.write(0);
      pServoStart = getCurrentTime();
    }
  }

  // FISH FEEDING
  if (getCurrentTime() - fServoStart >= 30)
  {
    fServo.write(45);
    delay(1000);
    fServo.write(0);
    fServoStart = getCurrentTime();
  }

  // TEMPERATURE MONITORING
  calculateTemp();
  if (tempC > 10 && tempC < 50)
  {
    tempLedControl();
  }

  // WATER MONITORING
  calculateMoisture();
  checkWater();
  checkWaterQuality();

  // OLED
  oled.firstPage();
  do
  {  
    showInfo();
  }
  while (oled.nextPage());
  oled.firstPage();
}
