//HEADER FILES
//------------
#include <Canbus.h>
#include <defaults.h>
#include <SPI.h>
#include <EasyNextionLibrary.h>
#include <trigger.h>
#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>
//DEFINITIONS
//-----------
// PID addresses for data collection
//------------------------------------
#define PID_RPM 0x0C
#define PID_COOLANT 0x05
#define PID_SPEED 0x0D
#define PID_VOLTAGE 0x42
#define PID_MAP 0x10 
// PID request and reply identification 
//------------------------------------
#define PID_REQUEST 0x7DF
#define PID_REPLY 0x7E8
//VARIABLES
//---------
uint8_t PID[] = {0x0C, 0x05, 0x0D, 0x14};

EasyNex myNex(Serial);
//FUNCTIONS
//--------
//Serial Protocol Interface Functions
void SPIStart(); //begins SPI communication
void SPIEnd(); //Ends SPI communication
void SPIWrite(uint8_t addr, uint8_t data); // SPI write to registers a specific value
uint8_t SPIRead(uint8_t addr); //read values from regsiters
//ECU message reciever and transciever function
void ECURequest(uint8_t pid_value);

//MAIN
//----
void setup() {
   //Start serial communication
  Serial.begin(115200);
   if(
  Canbus.init(CANSPEED_500))
  {
    Serial.print("CAN");
  }
  else
  {
    Serial.print("No CAN");
  }
  delay(5000);
  
  

  
  

}
void loop() {
   
  // put your main code here, to run repeatedly:
    ECURequest(PID_RPM);
    ECURequest(PID_COOLANT);
    ECURequest(PID_VOLTAGE);
  
     
      
     
    
     
   
  
}
//FUNCTIONS DEFINED 
//-----------------
//SPI initilized functions 
void SPIStart()
{
  SPI.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE0));
  digitalWrite(10,LOW); //chip select 

}
void SPIEnd()
{
  digitalWrite(10, HIGH); 
  SPI.endTransaction();
  
}
uint8_t SPIRead(uint8_t addr)
{
  SPIStart();
  uint8_t data;
  SPI.transfer(SPI_READ);
  SPI.transfer(addr);
  data = SPI.transfer(0xff);
  SPIEnd();
  return data; 
}
void SPIWrite(uint8_t addr, uint8_t data)
{
  SPIStart();
  SPI.transfer(SPI_WRITE);
  SPI.transfer(addr);
  SPI.transfer(data);
  SPIEnd();
  return data;
}

void ECURequest(uint8_t pid_value)
{
  tCAN* message = (tCAN*)malloc(sizeof(tCAN));
  message->id = 0x7DF;
  message->header.rtr = 0;
  message->header.length = 8;
  message->data[0] = 0x02;
  message->data[1] = 0x01;
  message->data[2] = pid_value;
  message->data[3] = 0x00;
  message->data[4] = 0x00;
  message->data[5] = 0x00;
  message->data[6] = 0x00;
  message->data[7] = 0x00;
  
  int data_rpm;
  int data_temp;
  float data_speed;
  double  data_volt;
  uint8_t dataThree;
  uint8_t dataFour;
  int dat;
  char buffer[456];
  
  mcp2515_bit_modify(CANCTRL, (1<<REQOP2)|(1<<REQOP1)|(1<<REQOP0), 0);
  if(mcp2515_send_message(message))
  {
    
    if(mcp2515_check_message())
    {
      if(mcp2515_get_message(message))
      {
        
        /*
        switch(message->data[2])
        {
          //Coolant temperature
          case PID_COOLANT:
            dataThree = SPIRead(RXB0D3);
            data_temp = message->data[3] - 40;
            Serial.print("e_temp.txt=\"");
            Serial.print(data_temp);
            Serial.print("\"");
            Serial.write(0xff);
            Serial.write(0xff);
            Serial.write(0xff);

           
            break;
          
           //Rotations per minute
          case PID_RPM:
            dataThree = SPIRead(RXB0D3); uint8_t dataFour = SPIRead(RXB0D4);
            data_rpm = ((message->data[3]*256) + message->data[4])/4;
            Serial.print("rpm.val=");
            Serial.print(22);
            Serial.write(0xff);
            Serial.write(0xff);
            Serial.write(0xff);
            
           
            break;
           //Engine Speed
          case PID_SPEED:
            dataThree = SPIRead(RXB0D3);
            data_speed = message->data[3];
            
            
            break;
          //Battery Boltage
          case PID_VOLTAGE:
            data_volt =  message->data[3]*0.005;
            
            break;
        }    */
        if(message->data[2] == PID_RPM)
        {
          dataThree = SPIRead(RXB0D3); uint8_t dataFour = SPIRead(RXB0D4);
          data_rpm = ((message->data[3]*256) + message->data[4])/4;
          myNex.writeNum("rpm.val", data_rpm);
          if(data_rpm <= 4500){
            myNex.writeNum("rpmbar1.val",data_rpm / 45);
            myNex.writeNum("rpmbar2.val", 0);
          }
          else           
          {
            myNex.writeNum("rpmbar1.val",100);
            myNex.writeNum("rpmbar2.val",(data_rpm - 4500) / 45);
          }
          /*
          else if(data_rpm >= 7900)
          {
            myNex.writeNum("rpmbar1", 100);
            myNex.writeNum("rpmbar2", data_rpm /30);
            myNex.writeNum("tps_bar", 100);
          }
        */
        }else if(message->data[2] == PID_COOLANT)
        {
          dataThree = SPIRead(RXB0D3);
          data_temp = message->data[3] - 40;
          
          myNex.writeStr("e_temp.txt", (String)data_temp);
          
        }
        else if(message->data[2] == PID_VOLTAGE)
        {
          data_volt = (256*message->data[3] + message->data[4])/1000;
          myNex.writeStr("v_bat.txt", (String)data_volt);
          
        }
      }else
      {
        Serial.print("no message");
      }
    }else
    {
      Serial.print("no message");
      
      
    }
  }else
  {
   Serial.print("no message");
  }
  /*
  if(data_rpm != 0)
  {
    Serial.print("rpm.val=");
    Serial.print(data_rpm);
  
    Serial.write(0xff);
    Serial.write(0xff);
    Serial.write(0xff);
    
  }
  */
  

 



  
  free(message);
}
