// Introduction of the Adafruit_MPU6050 library for interacting with MPU6050 sensors
#include <Adafruit_MPU6050.h>
// Introducing the Adafruit_Sensor library, which is the base library for the Adafruit sensor library
#include <Adafruit_Sensor.h>
// Introduction of Wire library for I2C communication, MPU6050 communicates with Arduino via I2C
#include <Wire.h>

//Introduction of the SoftwareSerial library for creating software serial ports
#include <SoftwareSerial.h>
// Introduction of the GD5800_Serial library for communication with the GD5800 MP3 module
#include <GD5800_Serial.h>

// Create an Adafruit_MPU6050 object to operate the MPU6050 sensor
Adafruit_MPU6050 mpu;
// Create a GD5800_Serial object with parameters 8 and 9 being the pins of the software serial port
GD5800_Serial mp3(8, 9);

// Define variables to store X-axis acceleration values
int acceleration_X = 0;
// Define variables to store Y-axis acceleration values
int acceleration_Y = 0;
// Define variables to store Z-axis acceleration values
int acceleration_Z = 0;

// Define X-axis acceleration thresholds for determining whether a specific action is triggered or not
int acceleration_X_threshold = 15;
// Define Y-axis acceleration thresholds for determining whether a specific action is triggered or not
int acceleration_Y_threshold = 15;
// Define Z-axis acceleration thresholds for determining whether a specific action is triggered or not
int acceleration_Z_threshold = 18;

// Define a counter for X-axis acceleration above a threshold value
int x_counter = 0;
// Define a counter for Y-axis acceleration exceeding a threshold value
int y_counter = 0;
// Define the counter for Z-axis acceleration above the threshold value
int z_counter = 0;

// Define an X-axis counter threshold that triggers a specific action when the counter exceeds that value.
int x_counter_threshold = 10;
// Define a Y-axis counter threshold that triggers a specific action when the counter exceeds this value.
int y_counter_threshold = 10;
// Define a Z-axis counter threshold that triggers a specific action when the counter exceeds this value
int z_counter_threshold = 30;

// Define analog pins for pressure sensor connections
int pressSensorPin = A0;
// Define variables to store the current value of the pressure sensor
int pressSensorValue = 0;
// Define variables to store the maximum value of the pressure sensor
int pressSensorValue_Max = 0;

// Define digital pins for button connections
int buttonPin = 2;
// Define Boolean variables to record the state of the buttons
boolean buttonFlag = 0;

// Initialization function, executed only once at the beginning of the program
void setup() 
{
  // Initialize serial communications with a baud rate of 9600
  Serial.begin(9600);
  // Set button pins to input mode
  pinMode(buttonPin, INPUT);

  // Attempt to initialize the MPU6050 sensor, if initialization fails then enter loop
  while (!mpu.begin()) 
  {
    // Prints an error message to the serial port indicating that the MPU6050 is not connected
    Serial.println("MPU6050 not connected!");
    // Delay 1 second and try again
    delay(1000);
  }
  // If the MPU6050 is initialized successfully, print the information to the serial port
  Serial.println("MPU6050 ready!");

  // Initialize GD5800 MP3 module communication with baud rate of 9600
  mp3.begin(9600);
  // Delay 100 milliseconds, wait for module to stabilize
  delay(100);
  // Set the volume of the MP3 module to 28
  mp3.setVolume(28);
  // Delay 100 milliseconds for settings to take effect
  delay(100);
}

// Define a sensor event object for storing sensor data
sensors_event_t event;

// The main loop function, the program will keep looping through the code inside this function.
void loop()
{
  // Get data from the MPU6050's acceleration sensor and store it in the event object
  mpu.getAccelerometerSensor()->getEvent(&event);
  // Extract X-axis acceleration value from event object
  acceleration_X = event.acceleration.x;
  // Extract Y-axis acceleration value from event object
  acceleration_Y = event.acceleration.y;
  // Extract the Z-axis acceleration value from the event object
  acceleration_Z = event.acceleration.z;

  // Determine if the absolute value of the X-axis acceleration exceeds the threshold value
  if(abs(acceleration_X) > acceleration_X_threshold)
  {
    // If the threshold is exceeded, the X-axis counter is incremented by 1
    x_counter++;
    // Reset Y-axis and Z-axis counters
    y_counter = 0;
    z_counter = 0;
    // Determine if the X-axis counter exceeds the threshold value
    if(x_counter > x_counter_threshold)
    {
      // Reset X-axis counter if threshold is exceeded
      x_counter = 0;
      // Prints the number 1 to the serial port
      Serial.println("1");
      // Getting the MP3 module to play a file with index 1
      mp3.playFileByIndexNumber(1);
      // Delay 2 seconds
      delay(2000);
    }
  }
  // If the X-axis acceleration does not exceed the threshold, determine whether the absolute value of the Y-axis acceleration exceeds the threshold.
  else if(abs(acceleration_Y) > acceleration_Y_threshold)
  {
    // If the threshold is exceeded, the Y-axis counter is incremented by 1
    y_counter++;
    // Reset X- and Z-axis counters
    x_counter = 0;
    z_counter = 0;
    // Determine if the Y-axis counter exceeds the threshold value
    if(y_counter > y_counter_threshold)
    {
      // If the threshold is exceeded, reset the Y-axis counter
      y_counter = 0;
      // Printing the number 2 to the serial port
      Serial.println("2");
      // Make the MP3 module play a file with index 2
      mp3.playFileByIndexNumber(2);
      // Delay 2 seconds
      delay(2000);
    }
  }

  // Reading the value of the pressure sensor
  pressSensorValue = analogRead(pressSensorPin);
  // Determine if the value of the pressure sensor is greater than 200
  if(pressSensorValue > 200)
  {
    // Reset X-, Y- and Z-axis counters
    x_counter = 0;
    y_counter = 0;
    z_counter = 0;
    
    // Assign the current pressure sensor value to the maximum value variable
    pressSensorValue_Max = pressSensorValue;
    // 100 cycles for obtaining the maximum value of the pressure sensor
    for(int i = 0; i < 100; i++)
    {
      // Reading the value of the pressure sensor
      pressSensorValue = analogRead(pressSensorPin);
      // Determine if the current value is greater than the maximum value
      if(pressSensorValue > pressSensorValue_Max)
      {
        // If greater, update the maximum value
        pressSensorValue_Max = pressSensorValue;
      }
      // Delay of 2 milliseconds
      delay(2);
    }

    // Maps pressure transducer maxima to a volume range of 10 - 30
    int volume = map(pressSensorValue_Max, 200, 750, 10, 30);
    // Setting the volume of the MP3 module to the mapped volume level
    mp3.setVolume(volume);
    // Delay 100 milliseconds for settings to take effect
    delay(100);

    // Determine button status flags
    if(buttonFlag == 0)
    {
      // If the button state is 0, let the MP3 module play the file with index 4
      mp3.playFileByIndexNumber(4);
    }
    else
    {
      // If the button state is 1, let the MP3 module play the file with index 5
      mp3.playFileByIndexNumber(5);
    }
    
    // Delay 2 seconds
    delay(2000);
    // Set the volume of the MP3 module back to 28
    mp3.setVolume(28);
    // Delay 100 milliseconds for settings to take effect
    delay(100);
  }

  // If the value of the pressure sensor does not exceed 200, determine whether the absolute value of the Z-axis acceleration exceeds the threshold value.
  else if(abs(acceleration_Z) > acceleration_Z_threshold)
  {
    // If the threshold is exceeded, the Z-axis counter is incremented by 1
    z_counter++;
    // Reset X- and Y-axis counters
    x_counter = 0;
    y_counter = 0;
    // Determine if the Z-axis counter exceeds the threshold value
    if(z_counter > z_counter_threshold)
    {
      // Reset Z-axis counter if threshold is exceeded
      z_counter = 0;
      // Printing the number 3 to the serial port
      Serial.println("3");
      // Getting the MP3 module to play a file with index 3
      mp3.playFileByIndexNumber(3);
      // Delay 2 seconds
      delay(2000);
    }
  }

  // 以下代码被注释掉，功能是打印当前时间和三轴加速度值
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

  // 判断按钮引脚是否为高电平
  if(digitalRead(buttonPin) == HIGH)
  {
    // 延迟15毫秒，用于消抖
    delay(15);
    // 再次判断按钮引脚是否为高电平
    if(digitalRead(buttonPin) == HIGH)
    {
      // 若为高电平，切换按钮状态标志
      buttonFlag = !buttonFlag;
      // 等待按钮释放
      while(digitalRead(buttonPin) == HIGH){}
    }
  }
}
