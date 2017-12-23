// Include all libraries
#include <LoRaLib.h>
#include <SoftwareSerial.h>

// Create instance of the LoRa object
LoRa lora;

// Packet to store data that will be transmitted by LoRa (commands)
uint8_t cmdOut[4] = {0, 0, 0, 0};
Packet out("01:23:45:67:89:AB:CD:EF", cmdOut, 4);

// Packet to store data that will be received by LoRa (responses)
uint8_t cmdIn[4] = {0, 0, 0, 0};
Packet in("01:23:45:67:89:AB:CD:EF", cmdIn, 4);

// Software Serial port for the Bluetooth module
SoftwareSerial port(4, 5);

void setup() {
  //Serial.begin(9600);
  // Begin communication via Bluetooth
  port.begin(9600);
  // Begin communication via LoRa
  lora.begin();
}

// This flag will be set when a packet was transmitted sucessfully 
bool sent = true;
void loop() {
  // Check if a new command was received
  if(port.available() > 0) {
    // Reset the flag
    sent = false;
    // Read all command bytes and store them to output buffer
    while(port.available()) {
      port.readBytes(cmdOut, 4);
    }
    // Copy command data to the output packet payload
    for(int i = 0; i < 4; i++) {
      out.data[i] = cmdOut[i];
    }
    /*Serial.print("command:\t");
    for(int i = 0; i < 4; i++) {
      Serial.print(cmdOut[i], HEX);
      Serial.print('\t');
    }
    Serial.println();*/
  }

  // Variable to store LoRa communication state
  uint8_t state;
  
  // If the current packet wasn't sent yet, send it
  if(!sent) {
    state = lora.transmit(out);
  }

  // If the packet was sent successfully, set the sent flag
  if((state == ERR_NONE) && (!sent)) {
    sent = true;
  }

  // Try to receive a packet
  state = lora.receive(in);
  if(state == ERR_NONE) {
    // If the reception was successfull, write all the response data to the Bluetooth port
    for(int i = 0; i < 4; i++) {
      port.write(in.data[i]);
      cmdIn[i] = in.data[i] & 0xFF;
    }

    // If the response is 0xA0, the last packet arrived malformed and has to be resend
    if(cmdIn[0] == 0xA0) {
      lora.transmit(out);
    }
    
    /*Serial.print("response:\t");
    for(int i = 0; i < 4; i++) {
      Serial.print(cmdIn[i], HEX);
      Serial.print('\t');
    }
    Serial.println();*/
    
  } else if(state == ERR_CRC_MISMATCH) {
    //Serial.println("CRC error");
  } else if(state == ERR_RX_TIMEOUT) {
    //Serial.println("RX timeout");
  }
}

