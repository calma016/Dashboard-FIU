#include <Canbus.h>
#include <defaults.h>
#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>
// PID addresses for data collection
//------------------------------------
#define PID_RPM 0x0C
#define PID_COOLANT 0x05
#define PID_SPEED 0x0D
#define PID_VOLTAGE 0x14
// PID request and reply identification 
//------------------------------------
#define PID_REQUEST 0x7DF
#define PID_REPLY 0x7E8
uint8_t PID[] = {0x0C, 0x05, 0x0D, 0x14};

void setup() {
  Serial.begin(115200); //Start serial communication
  Serial.println("Starting CAN");
  Serial.println("-------------");
  if(Canbus.init(CANSPEED_500))
  {
    Serial.println("CAN INIT"); 
  }
  else
  {
    Serial.println("CAN CAN'T INIT");
  }
  delay(500);

  

}
void loop() {
   
  // put your main code here, to run repeatedly:
  char buffer[426];
  for(int i = 0; i < 4; i+=1)
  {
  tCAN* message = (tCAN*)malloc(sizeof(tCAN));
  message->id = 0x7DF;
  message->header.rtr = 0;
  message->header.length = 8;
  message->data[0] = 0x02;
  message->data[1] = 0x01;
  message->data[2] = PID[i];
  message->data[3] = 0x00;
  message->data[4] = 0x00;
  message->data[5] = 0x00;
  message->data[6] = 0x00;
  message->data[7] = 0x00;
  int data_rpm;
  int data_temp;
  float data_speed;
  float  data_volt;
  int dat;
  char buffer[456];

  
  mcp2515_bit_modify(CANCTRL, (1<<REQOP2)|(1<<REQOP1)|(1<<REQOP0), 0);
  if(mcp2515_send_message(message))
  {
    if(mcp2515_check_message())
    {
      if(mcp2515_get_message(message))
      {
        switch(message->data[2])
        {
          case PID_COOLANT:
            data_temp = message->data[3] - 40;
           
           

           
            break;
          case PID_RPM:
            data_rpm = ((message->data[3]*256) + message->data[4])/4;
            Serial.print("rpm.val=");
            Serial.print(data_rpm);
            Serial.write(0xff);
            Serial.write(0xff);
            Serial.write(0xff);
            break;
          case PID_SPEED:
            data_speed = message->data[3];
            
            break;
          case PID_VOLTAGE:
            data_volt =  message->data[3]*0.005;
            
            break;
        }         
      }
    }
    
  }
            Serial.print("rpm.val=");
            Serial.print(data_rpm);
            Serial.write(0xff);
            Serial.write(0xff);
            Serial.write(0xff);

            
            Serial.print("e_temp.txt=\"");
            Serial.print(data_temp);
            Serial.print("\"");
            Serial.write(0xff);
            Serial.write(0xff);
            Serial.write(0xff);
  free(message);
  

  }
}
