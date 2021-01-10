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

const uint8_t TX_PIN = 3;
const uint16_t TRANSMITTER_ID = 8888;
const uint16_t RECEIVER_ID = 1234;
const uint16_t DELAY_MS = 1000;

//data to send
const uint8_t arr[3] = { 'd', 'o', 'g' };
const uint16_t UPDATE_DB_CMD = 12;

void sendMsg() {

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

}

void setup() {
	::pinMode(TX_PIN, OUTPUT);
	man.setupTransmit(TX_PIN, MAN_600);
}

void loop() {
	sendMsg();
	::delay(DELAY_MS);
}
