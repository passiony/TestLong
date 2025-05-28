#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include <SoftwareSerial.h>
#include <GD5800_Serial.h>

Adafruit_MPU6050 mpu;
GD5800_Serial mp3(8, 9);

int acceleration_X = 0;
int acceleration_Y = 0;
int acceleration_Z = 0;

int acceleration_X_threshold = 15;
int acceleration_Y_threshold = 15;
int acceleration_Z_threshold = 18;

int x_counter = 0;
int y_counter = 0;
int z_counter = 0;

int x_counter_threshold = 10;
int y_counter_threshold = 10;
int z_counter_threshold = 30;

int pressSensorPin = A0;
int pressSensorValue = 0;
int pressSensorValue_Max = 0;

int buttonPin = 2;
boolean buttonFlag = 0;
void setup() 
{
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);

  while (!mpu.begin()) 
  {
    Serial.println("MPU6050 not connected!");
    delay(1000);
  }
  Serial.println("MPU6050 ready!");

  mp3.begin(9600);
  delay(100);
  mp3.setVolume(28);
  delay(100);
}

sensors_event_t event;

void loop()
{
  mpu.getAccelerometerSensor()->getEvent(&event);
  acceleration_X = event.acceleration.x;
  acceleration_Y = event.acceleration.y;
  acceleration_Z = event.acceleration.z;

  if(abs(acceleration_X) > acceleration_X_threshold)
  {
    x_counter++;
    y_counter = 0;
    z_counter = 0;
    if(x_counter > x_counter_threshold)
    {
      x_counter = 0;
      Serial.println("1");
      mp3.playFileByIndexNumber(1);
      delay(2000);
    }
  }
  else if(abs(acceleration_Y) > acceleration_Y_threshold)
  {
    y_counter++;
    x_counter = 0;
    z_counter = 0;
    if(y_counter > y_counter_threshold)
    {
      y_counter = 0;
      Serial.println("2");
      mp3.playFileByIndexNumber(2);
      delay(2000);
    }
  }

  pressSensorValue = analogRead(pressSensorPin);
  if(pressSensorValue > 200)
  {
    x_counter = 0;
    y_counter = 0;
    z_counter = 0;
    
    pressSensorValue_Max = pressSensorValue;
    for(int i = 0; i < 100; i++)
    {
      pressSensorValue = analogRead(pressSensorPin);
      if(pressSensorValue > pressSensorValue_Max)
      {
        pressSensorValue_Max = pressSensorValue;
      }
      delay(2);
    }

    int volume = map(pressSensorValue_Max, 200, 750, 10, 30);
    mp3.setVolume(volume);
    delay(100);

    if(buttonFlag == 0)
    {
      mp3.playFileByIndexNumber(4);
    }
    else
    {
      mp3.playFileByIndexNumber(5);
    }
    
    delay(2000);
    mp3.setVolume(28);
    delay(100);
  }

  else if(abs(acceleration_Z) > acceleration_Z_threshold)
  {
    z_counter++;
    x_counter = 0;
    y_counter = 0;
    if(z_counter > z_counter_threshold)
    {
      z_counter = 0;
      Serial.println("3");
      mp3.playFileByIndexNumber(3);
      delay(2000);
    }
  }

  // Serial.print("[");
  // Serial.print(millis());
  // Serial.print("] X: ");
  // Serial.print(acceleration_X);
  // Serial.print(", Y: ");
  // Serial.print(acceleration_Y);
  // Serial.print(", Z: ");
  // Serial.print(acceleration_Z);
  // Serial.println(" m/s^2");
  // delay(100);

  if(digitalRead(buttonPin) == HIGH)
  {
    delay(15);
    if(digitalRead(buttonPin) == HIGH)
    {
      buttonFlag = !buttonFlag;
      while(digitalRead(buttonPin) == HIGH){}
    }
  }
}