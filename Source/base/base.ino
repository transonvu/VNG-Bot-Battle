#define DEBUG
#undef DEBUG

#include <ble/BLE.h>
#include <ble/Gap.h>
#include <UARTService.h>

#define 	DEVICE_NAME		"TRUM_RIOT"
#define 	PASS_LEN		0x08
#define 	UP				0x01
#define 	RIGHT			0x02
#define 	DOWN			0x03
#define 	LEFT			0x04
#define 	ON_WEAPON		0x05
#define 	OFF_WEAPON		0x06


const uint8_t PASSWORD[PASS_LEN] = { 4, 3, 7, 0, 2, 7, 6, 4 };

DigitalOut  Relay_1A(p28);
DigitalOut  Relay_1B(p25);
DigitalOut  Relay_2A(p24);
DigitalOut  Relay_2B(p23);
DigitalOut  Relay_3A(p22);
DigitalOut  Relay_3B(p21);
DigitalOut  Relay_4A(p9);
DigitalOut  Relay_4B(p16);
DigitalOut  Relay_1(p17);
DigitalOut  Relay_2(p18);
DigitalOut  Relay_3(p19);
DigitalOut  Relay_4(p20);

BLE ble;
UARTService *uartServicePtr;

volatile bool isValidConnection = true;
volatile uint8_t buffer[255];

void disconnectionCallBack(const Gap::DisconnectionCallbackParams_t *params) {
#ifdef DEBUG
	Serial.println("Disconnected!");
	Serial.println("Restarting the advertising process");
#endif
	isValidConnection = false;
	ble.startAdvertising();
}

// Nhận được dữ liệu từ master (điện thoại)
void onDataWritten(const GattWriteCallbackParams *params) {
#ifdef DEBUG
    Serial.print("Length: ");
    Serial.println(params->len);
    Serial.print("Length: ");
    for (uint16_t iData = 0; iData < params->len; ++iData) {
		Serial.print(params->data[iData]);
		Serial.print(" ");
    }
    Serial.println(); Serial.println();
#endif
    uint16_t bytesRead = params->len;
	for (uint16_t iData = 0; iData < bytesRead; ++iData) {
		buffer[iData] = params->data[iData];
	}
	
    // ble.updateCharacteristicValue(uartServicePtr->getRXCharacteristicHandle(), params->data, bytesRead); // phản hồi dữ liệu cho master
}

void executesCommand() {
	Serial.println(buffer[0]);
	switch (buffer[0]) {
		case UP:
			break;
		case DOWN:
			break;
		case LEFT:
			break;
		case RIGHT:
			break;
		case ON_WEAPON:
			break;
		case OFF_WEAPON:
			break;
	}
	buffer[0] = 0;
}

void verifyConnection() {
	if (isValidConnection) {}
	else {
		for (uint8_t iPass = 0; iPass < PASS_LEN; ++iPass) {
			if (buffer[iPass] == PASSWORD[iPass]) {}
			else return;
		}
		isValidConnection = true;
	}
}

void setup() {
	//Serial  
	Serial.begin(115200);

	//Init BLE
	ble.init();
	ble.onDisconnection(disconnectionCallBack);
	ble.onDataWritten(onDataWritten);
	
	// Setup advertising
	ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED);
	ble.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
	ble.accumulateAdvertisingPayload(GapAdvertisingData::SHORTENED_LOCAL_NAME, (const uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME) - 1);
	//  ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
	ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS, (const uint8_t *)UARTServiceUUID_reversed, sizeof(UARTServiceUUID_reversed));
	ble.setAdvertisingInterval(200); /* 1000ms; in multiples of 0.625ms. */
	ble.startAdvertising();
  
	UARTService uartService(ble);
	uartServicePtr = &uartService;
}

void loop() {
	ble.waitForEvent();
	// Kiểm tra kết nối có phải kết nối của team không
	verifyConnection();
	// Connected
	if (isValidConnection) executesCommand();
	else ble.disconnect((Gap::DisconnectionReason_t)0x08); // Gap::DisconnectionReason_t::CONNECTION_TIMEOUT = 0x08
}
