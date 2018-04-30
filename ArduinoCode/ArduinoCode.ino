
#include <Servo.h>
#include <SoftwareSerial.h>

#define RxD 13
#define TxD 12

SoftwareSerial btSerial(RxD, TxD);

Servo servoLeft,servoRight;
//Values for directions, 1500 = stop for both
// 1700 =  forward for right, backwards for left. Both at max speed.
// 1300 = backwards for right, forward for left. Both at max speed.

long duration = 50;


void setup() 
{
  servoLeft.attach(10);
  servoRight.attach(8);
  Serial.begin(9600);

  pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);
  
  setupBlueToothConnection();
}

void loop() 
{
  servoLeft.writeMicroseconds(1500);
  servoRight.writeMicroseconds(1500);
  
  String MotionDuration = "", recieved = "";
  char choice = ' ';
  
  if(btSerial.available() > 0)
  {
    recieved = btSerial.readString();
    choice = recieved[0];
    
    for (int i = 2; i < recieved.length(); i++)
    {
      MotionDuration +=  recieved[i];
    }
    
    Serial.println(MotionDuration);
    Serial.println(choice);
    
    duration = MotionDuration.toInt();
    
    if (choice == 'F')
    {
      btSerial.print("FRONT \n");
      servoLeft.writeMicroseconds(1400);
      servoRight.writeMicroseconds(1700);
      delay(duration);
    }
    
    else if (choice == 'B')
    {
       btSerial.print("BACK \n");
         servoLeft.writeMicroseconds(1600);
         servoRight.writeMicroseconds(1300);
         delay(duration);
     }

      else if (choice == 'L')
      {
        btSerial.print("LEFT \n");
        servoLeft.writeMicroseconds(1400);
        servoRight.writeMicroseconds(1300);
        delay(duration);
      }

      else if (choice == 'R')
      {
        btSerial.print("RIGHT \n");
        servoLeft.writeMicroseconds(1600);
        servoRight.writeMicroseconds(1700);
        delay(duration);
      }
      
       btSerial.print("STOP \n");
         servoLeft.writeMicroseconds(1500);
         servoRight.writeMicroseconds(1500);
  }

}




void setupBlueToothConnection()
{
 btSerial.begin(9600); //Set BluetoothBee BaudRate to default baud rate 38400
 btSerial.print("\r\n+STWMOD=0\r\n"); //set the bluetooth work in slave mode
 btSerial.print("\r\n+STNA=SeeedBTSlave\r\n"); //set the bluetooth name as "SeeedBTSlave"

 btSerial.print("\r\n+STPIN=0000\r\n");//Set SLAVE pincode"0000"
 btSerial.print("\r\n+STOAUT=1\r\n"); // Permit Paired device to connect me
 btSerial.print("\r\n+STAUTO=0\r\n"); // Autoconnection should be forbidden here
delay(2000); // This delay is required.
 btSerial.print("\r\n+INQ=1\r\n"); //make the slave bluetooth inquirable
//Serial.println("The slave bluetooth is inquirable!");
delay(2000); // This delay is required.
 btSerial.flush();
}
