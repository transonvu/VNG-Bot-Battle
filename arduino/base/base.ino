#define DEBUG
//#undef DEBUG

#include <ble/BLE.h>
#include <ble/Gap.h>
#include <UARTService.h>
#include <mbed.h>

#define 	DEVICE_NAME		"TRUM_RIOT"
#define 	PASS_LEN		0x08
#define 	UP				0x01
#define 	RIGHT			0x02
#define 	DOWN			0x03
#define 	LEFT			0x04
#define   	BRAKE         	0x0A
#define 	WEAPON		    0x0B
#define   	INVERT        	0x0C

const uint8_t 			PASSWORD[PASS_LEN]      = { 4, 3, 7, 0, 2, 7, 6, 4 };

DigitalOut  Relay_1A(p28);
DigitalOut  Relay_1B(p25);
DigitalOut  Relay_2A(p24);
DigitalOut  Relay_2B(p23);
DigitalOut  Relay_3A(p22);
DigitalOut  Relay_3B(p21);
DigitalOut  Relay_4A(p9);
DigitalOut  Relay_4B(p16);
DigitalOut  Relay_1(p17);
DigitalOut  Relay_2(p18);			// on or off weapon
DigitalOut  Relay_3(p19);
DigitalOut  InvertSignalLed(p20); 	// not use Relay_4

BLE ble;
UARTService *uartServicePtr;
Ticker waitingPswdTicker;

int bitInvert = 0; // not invert
volatile bool isReceivedFirstMsg = true;
volatile bool isValidConnection = true;
volatile uint8_t buffer[255];


void turnOffSystem();

void clockCallback() {
    if (isReceivedFirstMsg) waitingPswdTicker.detach();
    else ble.disconnect(Gap::CONNECTION_TIMEOUT);
}

void disconnectionCallBack(const Gap::DisconnectionCallbackParams_t *params) {
#ifdef DEBUG
	Serial.println("Disconnected!");
	Serial.println("Restarting the advertising process");
#endif
	if (isReceivedFirstMsg) isReceivedFirstMsg = false;
	else waitingPswdTicker.detach();
	isValidConnection = false;
	turnOffSystem();
	ble.startAdvertising();
}

void connectionCallBack(const Gap::ConnectionCallbackParams_t *params) {
#ifdef DEBUG
	Serial.println("Connected!");
#endif
	waitingPswdTicker.attach(&clockCallback, 1); // waiting for password (limited 0.5s)
}

// Nhận được dữ liệu từ master (điện thoại)
void onDataWritten(const GattWriteCallbackParams *params) {
	if (!isReceivedFirstMsg) isReceivedFirstMsg = true;
#ifdef DEBUG
    Serial.print("Length: ");
    Serial.println(params->len);
    Serial.print("Data: ");
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

void turnOffMotors() {
	Relay_1A = 0;
	Relay_1B = 0;
	// Relay_2A = 0;
	// Relay_2B = 0;
	Relay_3A = 0;
	Relay_3B = 0;
	// Relay_4A = 0;
	// Relay_4B = 0;
	delayMicroseconds(100);
}

void turnOffSystem() {
	Relay_2 = 0; // turn off weapon
	// InvertSignalLed = 0;
	turnOffMotors();
}

void runLeftMotor(int cmd) {
	if (cmd == UP || cmd == RIGHT) {
		Relay_1A = 1 ^ bitInvert;
		Relay_1B = 0 ^ bitInvert;
	} else if (cmd == DOWN || cmd == LEFT) {
		// Relay_2A = 0 ^ bitInvert;
		// Relay_2B = 1 ^ bitInvert;
		Relay_1A = 0 ^ bitInvert;
		Relay_1B = 1 ^ bitInvert;
	}
}

void runRightMotor(int cmd) {
	if (cmd == UP || cmd == RIGHT) {
		Relay_3A = 1 ^ bitInvert;
		Relay_3B = 0 ^ bitInvert;
	} else if (cmd == DOWN || cmd == LEFT) {
		// Relay_4A = 0 ^ bitInvert;
		// Relay_4B = 1 ^ bitInvert;
		Relay_3A = 0 ^ bitInvert;
		Relay_3B = 1 ^ bitInvert;
	}
}

void executeCommand() {
	switch (buffer[0]) {
		case WEAPON:
			Relay_2 = Relay_2 ? 0 : 1;
			break;
		case INVERT:
			InvertSignalLed = bitInvert = bitInvert ? 0 : 1;
			break;
		case BRAKE:
			turnOffMotors();
			break;
		case UP: case DOWN: case LEFT: case RIGHT:
			turnOffMotors();
			runLeftMotor(buffer[0]);
			runRightMotor(buffer[0]);
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
	ble.onConnection(connectionCallBack);
	
	// Setup advertising
	ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED);
	ble.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
	ble.accumulateAdvertisingPayload(GapAdvertisingData::SHORTENED_LOCAL_NAME, (const uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME) - 1);
	//  ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
	ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS, (const uint8_t *)UARTServiceUUID_reversed, sizeof(UARTServiceUUID_reversed));
	ble.setAdvertisingInterval(200); 		// 1000ms; in multiples of 0.625ms.
	ble.startAdvertising();
  
	UARTService uartService(ble);
	uartServicePtr = &uartService;
	
	buffer[0] = 0;
	InvertSignalLed = 0;
	turnOffSystem();
}

void loop() {
	ble.waitForEvent();
	if (isReceivedFirstMsg) {
		// Is valid connection?
		verifyConnection();
		// Connected
		if (isValidConnection) executeCommand();
		else ble.disconnect(Gap::CONNECTION_TIMEOUT); // Gap::DisconnectionReason_t::CONNECTION_TIMEOUT = 0x08
	}
}
