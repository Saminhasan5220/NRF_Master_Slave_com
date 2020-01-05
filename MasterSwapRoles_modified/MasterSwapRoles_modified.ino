// MasterSwapRoles
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define CE_PIN 7  
#define CSN_PIN 8
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool debug = false;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const byte slaveAddress[5] = {'R','x','A','A','A'};
const byte masterAddress[5] = {'T','X','a','a','a'};
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
RF24 radio(CE_PIN, CSN_PIN); // Create a Radio
const byte numChars = 32;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int txNum = 0;
char dataReceived[numChars]; // to hold the data from the slave - must match replyData[] in the slave
bool newRadioData = false;


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
char receivedSerialChars[numChars];   // an array to store the received Serial data
boolean newSerialData = false;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
volatile long txID,RXid;
volatile long txFAIL;
void setup() {
    Serial.begin(57600);
    Serial.println("<Arduino is ready>");
    Serial.println("MasterSwapRoles Starting");

    radio.begin();
    radio.setDataRate( RF24_2MBPS);
    radio.setPALevel(RF24_PA_HIGH);
    radio.openWritingPipe(slaveAddress);
    radio.openReadingPipe(1, masterAddress);

    radio.maskIRQ(1,1,0); //mask all IRQ triggers except for receive (1 is mask, 0 is no mask)
    attachInterrupt(digitalPinToInterrupt(2) , getRadioData, FALLING); //Create interrupt: 0 for pin 2 or 1 for pin 3, the name of the interrupt function or ISR, and condition to trigger interrupt
    radio.setRetries(3,5); // delay, count
send();
delay(4000);
}

void loop() {
recvWithEndMarker();
if (newSerialData) {
  send(); 
  newSerialData = false;
  }

    showRadioData();
}
void  getRadioData() {
// count++; //up the receive counter
 if(radio.available()) { //get data sent from transmit
 radio.read( &dataReceived, sizeof(dataReceived) );
 //Serial.println("ISR!!!");
 RXid+=1;
         newRadioData = true;

 }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void send() {
if(debug)Serial.println("Send_called");
        radio.stopListening();
            delayMicroseconds(50);
            bool rslt;
            rslt = radio.write( &receivedSerialChars, sizeof(receivedSerialChars) );
            delayMicroseconds(50);
        radio.startListening();
            delayMicroseconds(50);

        if (rslt) {
        if(debug){ Serial.print("Radio Data Sent:");
        Serial.println(receivedSerialChars);
        }

txID+=1;

}

        else {
          
            Serial.println("  Tx failed");
  txFAIL+=1;
        }
          newSerialData = false;

}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void showNewData() {
    if (newSerialData) {
        Serial.print("Recieved Serial Data:");
        Serial.println(receivedSerialChars);
        //newSerialData = false;
    }
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void showRadioData() {
    if(newRadioData){
              if(debug) Serial.print("Radio Data received:");
        Serial.println(dataReceived);
  newRadioData = false;
}
    }
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------





void recvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;
   
    while (Serial.available() > 0 && newSerialData == false) {
        rc = Serial.read();

        if (rc != endMarker) {
            receivedSerialChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        }
        else {
            receivedSerialChars[ndx] = '\0'; // terminate the string
            ndx = 0;
         newSerialData = true;
        }
    }
            if(debug) showNewData();

}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
