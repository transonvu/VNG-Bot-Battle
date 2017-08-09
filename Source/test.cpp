#include <BLE_API.h>

//#define BUTTON    7

#define DEVICE_NAME       "TRUM_RIOT"
#define TXRX_BUF_LEN      2
// Create ble instance
BLE                       ble;
Ticker                    ticker1s;

// The constant uuid of service and characteristics
//Motors: 1 services gồm 5 characteristics
static const uint8_t led_button_service_uuid[]  = {0x00, 0x00, 0xFF, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB};
static const uint8_t led_char_uuid[]            = {0x00, 0x00, 0xFF, 0xF1, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB};  //WRITE-uchar
static const uint8_t button_char_uuid[]         = {0x00, 0x00, 0xFF, 0xF7, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB};   //NOTIFY-uchar
// Used in advertisement
static const uint8_t  vbluno_uuid[]            = {0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xF0, 0xFF, 0x00, 0x00};

// Initialize value of chars
uint8_t led_char_value[TXRX_BUF_LEN]       = {1};
uint8_t button_char_value[TXRX_BUF_LEN]       = {1};
                        
// Create characteristic
GattCharacteristic  led_characteristic(led_char_uuid, led_char_value, 1, TXRX_BUF_LEN, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE );
GattCharacteristic  button_characteristic(button_char_uuid, button_char_value, 1, TXRX_BUF_LEN, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);
GattCharacteristic *led_button_chars[] = {&led_characteristic, &button_characteristic};

//Create service
GattService         led_button_service(led_button_service_uuid, led_button_chars, sizeof(led_button_chars) / sizeof(GattCharacteristic *));

void disconnectionCallback(const Gap::DisParams_t *params) {
	Serial.println("Disconnect");
	ble.startAdvertising();
}

/** @brief  Connect callback
 *
 *  @param[in] *params   params->handle : The ID for this connection
 *                       params->role : PERIPHERAL  = 0x1, // Peripheral Role
 *                                      CENTRAL     = 0x2, // Central Role.
 */
void connectionCallback( const Gap::Params_t *params ) {
	Serial.println("Connected");
}

/** @brief  Hàm callback cho sự kiện ghi dữ liệu từ gatt server
 *          Stop, Go Forward, Go Back, Rotate Left, Rotate Right
 *
 *  @param[in] *Handler   Handler->connHandle : The handle of the connection that triggered the event
 *                        Handler->handle : Attribute Handle to which the write operation applies
 *                        Handler->writeOp : OP_INVALID               = 0x00,  // Invalid operation.
 *                                           OP_WRITE_REQ             = 0x01,  // Write request.
 *                                           OP_WRITE_CMD             = 0x02,  // Write command.            ////
 *                                           OP_SIGN_WRITE_CMD        = 0x03,  // Signed write command.
 *                                           OP_PREP_WRITE_REQ        = 0x04,  // Prepare write request.
 *                                           OP_EXEC_WRITE_REQ_CANCEL = 0x05,  // Execute write request: cancel all prepared writes.
 *                                           OP_EXEC_WRITE_REQ_NOW    = 0x06,  // Execute write request: immediately execute all prepared writes.
 *                        Handler->offset : Offset for the write operation
 *                        Handler->len : Length (in bytes) of the data to write
 *                        Handler->data : Pointer to the data to write
 */
void dataWrittenCallback(const GattWriteCallbackParams *Handler) {
	static uint16_t bytes_read=0;
	if(Handler->handle == led_characteristic.getValueAttribute().getHandle()) {
		ble.readCharacteristicValue(led_characteristic.getValueAttribute().getHandle(), led_char_value, &bytes_read);       // Read the value of characteristic
		digitalWrite(LED, led_char_value[0]);
		Serial.println("Write LED value");  
		Serial.println(led_char_value[0]);
		Serial.println(bytes_read);
	}
}

/**
 * @brief  Set advertisement
 */
void setAdvertisement(void) {
	ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
	ble.accumulateAdvertisingPayload(GapAdvertisingData::SHORTENED_LOCAL_NAME, (const uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME) - 1);
	// Add complete 128bit_uuid to advertisement
	ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS,(const uint8_t *)vbluno_uuid, sizeof(vbluno_uuid));
	// Add complete device name to scan response data
	ble.accumulateScanResponse(GapAdvertisingData::COMPLETE_LOCAL_NAME,(const uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
}

/*
 * @brief  Ticker 1s
 */
void task_handle(void) {
	static volatile uint8_t val_button[TXRX_BUF_LEN]={0};
	Serial.println("Button emulator");
	val_button[0]++;
	ble.updateCharacteristicValue(button_characteristic.getValueAttribute().getHandle(), (const uint8_t*)val_button, 1);
}

/*
 * @brief   Setup for BLE Interface
 */
void setupBLE() {
	// Init timer task
	ticker1s.attach(task_handle, 1);
	// Init BLE
	ble.init();
	ble.onConnection(connectionCallback);
	ble.onDisconnection(disconnectionCallback);
	ble.onDataWritten(dataWrittenCallback);
	// Set advertisement
	setAdvertisement();
	// Set adv_type(enum from 0)
	ble.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
	// Add service
	ble.addService(led_button_service);
	// Set device name
	ble.setDeviceName((const uint8_t *)DEVICE_NAME);
	// Set tx power,valid values are -40, -20, -16, -12, -8, -4, 0, 4
	ble.setTxPower(4);
	// Set adv_interval, 100ms in multiples of 0.625ms.
	ble.setAdvertisingInterval(160);    
	// Set adv_timeout, in seconds
	ble.setAdvertisingTimeout(0);
	// Start advertising
	ble.startAdvertising();
 }

 /*
 *  @brief      Setup routine
 */
void setup() {
	Serial.begin(115200);
	// Init BLE
	setupBLE();
}

void loop() {
	ble.waitForEvent();
}