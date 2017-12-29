// Include LoRa library
#include <LoRaLib.h>

// Create instance of the LoRa object
LoRa lora;

// Packet to store data that will be transmitted by LoRa (commands)
uint8_t cmd[4] = {0, 0, 0, 0};
Packet out("01:23:45:67:89:AB:CD:EF", cmd, 4);

// Packet to store data that will be received by LoRa (responses)
uint8_t cmdIn[4] = {0, 0, 0, 0};
Packet in("01:23:45:67:89:AB:CD:EF", cmdIn, 4);

void setup() {
  // Begin communication via USB
  Serial.begin(9600);
  // Begin communication via LoRa
  lora.begin();
}

// This flag will be set when a packet was transmitted sucessfully 
bool sent = true;
void loop() {
  // Check if a new command was received
  if(Serial.available() > 0) {
    // Reset the flag
    sent = false;
    // Read all command bytes and store them to output buffer
    while(Serial.available()) {
      Serial.readBytes(cmd, 4);
    }
    // Copy command data to the output packet payload
    for(int i = 0; i < 4; i++) {
      out.data[i] = cmd[i];
    }
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
    // If the reception was successfull, write all the response data to the USB port
    for(int i = 0; i < 4; i++) {
      Serial.write(in.data[i]);
    }

    // If the response is 0xA0, the last packet arrived malformed and has to be resend
    if(cmdIn[0] == 0xA0) {
      lora.transmit(out);
    }
  }
}
