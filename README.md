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

## Packet Format

0             1        2               4            6

[PACKET LENGTH][VERSION][TRANSMITTER ID][RECEIVER ID]


6        7            8     9

[FRAGMENT][BODY LENGTH][CRC8]


9

[BODY DATA]

## Message Format

0           2       4

[BODY LENGTH][ACTION]


4

[BODY DATA]

## Extending Messages

```cpp
/**
 * MeasurementMessage holds a single 16 bit integer
 */
class MeasurementMessage : public Message {

protected:
    static const uint8_t _MEASUREMENT_OFFSET = 0;
    static const uint8_t _ACTION_VALUE = 1;

public:
    MeasurementMessage() : Message() {
        this->setRawAction(_ACTION_VALUE);
        this->resizeBody(sizeof(uint16_t));
    }

    uint16_t getMeasurement() const noexcept {
        return this->_data.getUInt16(this->fromBaseBodyOffset(
            _MEASUREMENT_OFFSET));
    }

    void setMeasurement(const uint16_t m) noexcept {
        this->_data.setUInt16(m, this->fromBaseBodyOffset(
            _MEASUREMENT_OFFSET));
    }

};

// example use
// read analog value of A1 into message object
MeasurementMessage mm;
mm.setMeasurement(::analogRead(A1));

// then encapsulate the message into a RadioPacket as
// in the example transmitter code above

// when receiving, cast Message to a MeasurementMessage
// where m is a Message* as in the example receiver code above
MeasurementMessage* mm = static_cast<MeasurementMessage*>(m);
mm->getMeasurement();
```
