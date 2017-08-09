#define DEBUG
#undef DEBUG

#include <BLE_API.h>
#include <UARTService.h>

#define DEVICE_NAME "TRUM_RIOT"

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

volatile uint8_t g_cmd;

void disconnectionCallBack(const Gap::DisconnectionCallbackParams_t *params) {
#ifdef DEBUG
	Serial.println("Disconnected!");
	Serial.println("Restarting the advertising process");
#endif
	ble.startAdvertising();
}

// Nhận được dữ liệu từ master (điện thoại)
void onDataWritten(const GattWriteCallbackParams *params) {
#ifdef DEBUG
    Serial.print("Length: ");
    Serial.println(params->len);
    Serial.print("Length: ");
    for (int iData = 0; iData < params->len; ++iData) {
      Serial.print(params->data[iData]);
      Serial.print(" ");
    }
    Serial.println(); Serial.println();
#endif
    uint16_t bytesRead = params->len;
    g_cmd = params->data[0];
    // ble.updateCharacteristicValue(uartServicePtr->getRXCharacteristicHandle(), params->data, bytesRead); // phản hồi dữ liệu cho master
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

// the loop function runs over and over again forever
void loop() {
	ble.waitForEvent();

	Serial.println(g_cmd);

	g_cmd = 0;
}
