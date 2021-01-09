# RadioPacket

A packet format for sending/receiving data using the Arduino Manchester library

## Transmitter Code

```cpp

using RadioPacket;

uint16_t TRANSMITTER_ID = 8888;
uint16_t RECEIVER_ID = 1234;

//data to send
uint8_t arr[3] = { 'd', 'o', 'g' };
uint16_t UPDATE_DB_CMD = 12;

Message m(arr, 3);
m.setRawAction(UPDATE_DB_CMD);

RadioPacket p(m.getData(), m.getMessageLength());
p.setRawTransmitterId(TRANSMITTER_ID);
p.setRawReceiverId(RECEIVER_ID);
p.setRawCrc8(p.generateChecksum());

//send packet using Manchester lib
man.transmitArray(
    p.getRawPacketLength(),
    const_cast<uint8_t*>(p.getData()));

```

## Receiver Code

```cpp

using RadioPacket;

const uint8_t BUFFER_SIZE = 0xff;
uint8_t buffer[BUFFER_SIZE];

//aquire data from Manchester lib into buffer...

RadioPacket* p;
Message* m;

if(RadioPacket::parse(&p, buffer, BUFFER_SIZE) == RadioPacket::PARSE_OK) {
    if(Message::parse(&m, p->getBodyData(), p->getRawBodyLength()) == Message::PARSE_OK) {
        Serial.println(m->getBodyData()); //dog
    }
}

```
