# RadioPacket

A packet format for sending/receiving data using the [Arduino Manchester library](https://github.com/mchr3k/arduino-libs-manchester)

[![lint status](https://github.com/endail/RadioPacket/workflows/arduino-lint/badge.svg?event=push)](https://github.com/endail/RadioPacket/actions?query=workflow%3Aarduino-lint)

## [Transmitter Code](https://github.com/endail/RadioPacket/blob/main/examples/tx/tx.ino)

```cpp
using RadioPacket;

Message m(arr, sizeof(arr));
m.setRawAction(UPDATE_DB_CMD);

RadioPacket p(&m);
p.setRawTransmitterId(TRANSMITTER_ID);
p.setRawReceiverId(RECEIVER_ID);
p.setRawCrc8(p.generateChecksum());

man.transmitArray(
    p.getRawPacketLength(),
    const_cast<uint8_t*>(p.getData()));
```

## [Receiver Code](https://github.com/endail/RadioPacket/blob/main/examples/rx/rx.ino)

```cpp
RadioPacket* p;
Message* m;

if(RadioPacket::parse(&p, buffer, BUFFER_SIZE) == RadioPacket::PARSE_OK) {
    if(Message::parse(&m, p->getBodyData(), p->getRawBodyLength()) == Message::PARSE_OK) {
        Serial.println(m->getBodyData());
    }
}

delete p;
delete m;
```
