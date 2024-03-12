#include <Arduino.h>
#include "PubSubClient.h"
#include "Wifi.h"
#include "RF24.h"


// Varible config radio 
RF24 radio(4,5);

// Varible address node from the room
const uint64_t Address_NodeGate  = 0x1122334400;
uint8_t  Gate_On_Pipe_1 = 1;

// Variable address node from the gate
const uint64_t Address_NodeRoom   = 0x1122334411;
uint8_t  Room_On_Pipe_2 = 2;

char myTxData[32];
char myRXData[32];

typedef struct {
  char ID;
  float nhietDo;
  float doAm;
  float CO2;
} Data_Node_Room_t;



uint32_t lastTime1, lastTime2;

void setup() {
  Serial.begin(115200);

  // Setup NRF24L01
  radio.begin();
  radio.openReadingPipe(Gate_On_Pipe_1, Address_NodeGate);
  radio.openReadingPipe(Room_On_Pipe_2, Address_NodeRoom);
  radio.setChannel(52);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_1MBPS);
  radio.setAutoAck(true);

  lastTime1 = millis();
  lastTime2 = millis();
  radio.printDetails();
}

void loop() {
  // NRF24 Transiver 
  if( (uint32_t)(millis() - lastTime1) > 1000)
  {
    // Time1 - 250: Node Gate, Time 2 - 500: Node Room, Time3 - 750: Node Gate, Time4 - 1000: Node Room
    static bool b_Node_Active = 1;

    radio.stopListening();
    if( b_Node_Active == 0)   // Node Gate
    {
      radio.openWritingPipe(Address_NodeGate);
      myTxData[0] = 'A';
      radio.write(myTxData, 32);
    
    } else {                // Node Room
      radio.openWritingPipe(Address_NodeRoom);
      static uint8_t count = 0;
      myTxData[0] = count;
      myTxData[1] = count++;
      radio.write(myTxData, 32);
    }
    lastTime1 = millis();
    radio.startListening();
  }

  // NRF24 Receive
  if(radio.available())
  {
    radio.read(myRXData, 32);
    Serial.println('x');
    // Node Gate House
    if(myRXData[0] == 'A')  
    {
      Serial.print("Node Gate House");
    }
    // Node Room
    if(myRXData[0] == 'B')
    {
      Data_Node_Room_t Value_Node_Room;
      memcpy(&Value_Node_Room, myRXData, sizeof(Value_Node_Room));
      Serial.print("Nhiet do: ");
      Serial.println(Value_Node_Room.nhietDo);
      Serial.print("Do am: ");
      Serial.println(Value_Node_Room.doAm);
      Serial.print("CO2: ");
      Serial.println(Value_Node_Room.CO2);
      Serial.println("\n");
    }  
  }
}