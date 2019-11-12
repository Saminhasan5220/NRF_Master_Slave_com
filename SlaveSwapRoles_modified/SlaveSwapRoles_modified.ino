// SlaveSwapRoles
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
const byte slaveAddress[5] = {'R', 'x', 'A', 'A', 'A'};
const byte masterAddress[5] = {'T', 'X', 'a', 'a', 'a'};
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
volatile long txID, RXid;
volatile long txFAIL;
void setup() {
  Serial.begin(9600);
  Serial.println("<Arduino is ready>");

  Serial.println("SlaveSwapRoles Starting");

  radio.begin();
  radio.setDataRate( RF24_250KBPS );
  radio.openWritingPipe(masterAddress); // NB these are swapped compared to the master
  radio.openReadingPipe(1, slaveAddress);

  radio.maskIRQ(1, 1, 0); //mask all IRQ triggers except for receive (1 is mask, 0 is no mask)
  attachInterrupt(digitalPinToInterrupt(3) , getRadioData, FALLING); //Create interrupt: 0 for pin 2 or 1 for pin 3, the name of the interrupt function or ISR, and condition to trigger interrupt
  radio.setRetries(3, 5); // delay, count
  radio.startListening();
}

void loop() {
  showRadioData();
  recvWithEndMarker();
  showNewData();
  if (newSerialData) {
    send();

  }


}
void getRadioData() {
  if (radio.available()) { //get data sent from transmit
    radio.read( &dataReceived, sizeof(dataReceived) );
    RXid += 1;
    //Serial.println(dataReceived);

  }
  newRadioData = true;

}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void send() {
  if (debug)Serial.println("Send_called");

  radio.stopListening();
            delayMicroseconds(50);
  bool rslt;
  rslt = radio.write( &receivedSerialChars, sizeof(receivedSerialChars) );
            delayMicroseconds(50);
  radio.startListening();
            delayMicroseconds(50);
  if (rslt) {
    if (debug) {
      Serial.print("Radio Data Sent:");
      Serial.println(receivedSerialChars);
    }

    txID += 1;
  }
  else {

    Serial.println("  Tx failed");
    txFAIL += 1;
  }
  newSerialData = false;
  //if(debug)Serial.println("Done calling SEND");

}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void showNewData() {
  if (newSerialData) {

    //Serial.print("Recieved Serial Data:");
    //Serial.println(receivedSerialChars);
    //    newSerialData = false;
  }

}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void showRadioData() {
  //if (debug)Serial.println("Called  showRadioData");

  if (newRadioData) {
    if (debug)Serial.print("Radio Data received:");
    Serial.println(dataReceived);
  }
  newRadioData = false;

  if (debug)Serial.println("Done calling showRadioData");

}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


// Example 2 - Receive with an end-marker



void recvWithEndMarker() {
  if (debug)Serial.println("Called  recvWithEndMarker");
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
  if (debug) showNewData();

}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
