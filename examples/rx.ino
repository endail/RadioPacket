// MIT License
//
// Copyright (c) 2021 Daniel Robertson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <Manchester.h>
#include <RadioPacket.h>
#include <string.h>

using RadioPacket;

const uint8_t RX_PIN = 3;
const uint32_t SERIAL_BAUD = 115200;
const uint8_t BUFFER_SIZE = 0xff;

uint8_t buffer[BUFFER_SIZE] = {0};

void resetBuffer() {
	::memset(buffer, 0, BUFFER_SIZE);
	man.beginReceiveArray(BUFFER_SIZE, buffer);
}

void setup() {

	::pinMode(RX_PIN, INPUT);

	while(!Serial) {
		::delay(1);
	}

	Serial.begin(SERIAL_BAUD);
	man.setupTransmit(TX_PIN, MAN_600);
	resetBuffer();

}

void loop() {

	if(!man.receiveComplete()) {
		return;
	}

	RadioPacket* p;
	Message* m;

	if(RadioPacket::parse(&p, buffer, BUFFER_SIZE) == RadioPacket::PARSE_OK) {
		if(Message::parse(&m, p->getBodyData(), p->getRawBodyLength()) == Message::PARSE_OK) {
			Serial.println(m->getBodyData());
		}
	}

	delete p;
	delete m;

	resetBuffer();

}
