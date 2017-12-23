#include <LoRaLib.h>

uint8_t cmd[4] = {0, 0, 0, 0};

LoRa lora;

Packet out("01:23:45:67:89:AB:CD:EF", cmd, 4);
Packet in;

void setup() {
  Serial.begin(9600);
  lora.begin();
}

bool sent = true;
void loop() {
  if(Serial.available() > 0) {
    sent = false;
    while(Serial.available()) {
      Serial.readBytes(cmd, 4);
    }
    for(int i = 0; i < 4; i++) {
      out.data[i] = cmd[i];
      Serial.println(cmd[i]);
    }
  }
  
  uint8_t state;
  if(!sent) {
    state = lora.transmit(out);
  }
  
  if((state == ERR_NONE) && (!sent)) {
    sent = true;
    for(int i = 0; i < 4; i++) {
      Serial.write(out.data[i]);
    }
    Serial.println();
  }

  state = lora.receive(in);
  if(state == ERR_NONE) {
    for(int i = 0; i < 4; i++) {
      Serial.write(in.data[i]);
    }
  } else if(state == ERR_CRC_MISMATCH) {
    
  }
}

