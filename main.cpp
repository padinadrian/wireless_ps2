/**
 * File:    main.cpp
 * Author:  Adrian Padin (padin.adrian@gmail.com)
 */

/* ===== Includes ===== */
#include "mbed.h"
#include "ble/BLE.h"

/* ===== Global Variables ===== */
DigitalOut led1(P0_4, 1);
DigitalOut led2(P0_5, 1);
Ticker ticker;

// Service UUIDs
uint16_t customServiceUUID  = 0xA000;
uint16_t readCharUUID       = 0xA001;
uint16_t writeCharUUID      = 0xA002;

// Device local name
const static char DEVICE_NAME[] = "Adrian DualShock";

// Custom UUID (FFFF is reserved for development)
static const uint16_t uuid16_list[] = {0xFFFF};

// Setup custom characteristics
static uint8_t readValue[10] = {5};
ReadOnlyArrayGattCharacteristic<uint8_t, sizeof(readValue)> readChar(readCharUUID, readValue);

static uint8_t writeValue[10] = {0};
WriteOnlyArrayGattCharacteristic<uint8_t, sizeof(writeValue)> writeChar(writeCharUUID, writeValue);

// Setup custom service
GattCharacteristic *characteristics[] = {
    &readChar,
    &writeChar
};
GattService customService(
    customServiceUUID,
    characteristics,
    sizeof(characteristics) / sizeof(GattCharacteristic *)
);

/* ===== Functions ===== */

/**
 * Periodic callback
 */
void periodicCallback(void)
{
    led2 = !led2; // Do blinky on LED1 while we're waiting for BLE events.

//    static uint32_t counter = 0;
//    counter++;
//    BLE::Instance(BLE::DEFAULT_INSTANCE).gattServer().write(
//        readChar.getValueHandle(),
//        (uint8_t *)(&counter),
//        sizeof(counter)
//    );
}

/**
 *  Restart advertising when client disconnects.
 */
void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *)
{
    ticker.detach();
    BLE::Instance(BLE::DEFAULT_INSTANCE).gap().startAdvertising();
}

/**
 * Start collecting data when client connects.
 */
void connectionCallback(const Gap::ConnectionCallbackParams_t *)
{
    ticker.attach(periodicCallback, 1); // blink LED every second
}

/**
 * Handle reads from readCharacteristic
 */
void readCharCallback(const GattReadCallbackParams *params)
{
    led2 = !led2;
}

/**
 *  Handle writes to writeCharacteristic
 */
void writeCharCallback(const GattWriteCallbackParams *params)
{
    static uint32_t counter = 0;
    counter++;

    /* Check to see what characteristic was written, by handle */
    if(params->handle == writeChar.getValueHandle()) {
        /* toggle LED if only 1 byte is written */
        if(params->len == 1) {
            led1 = params->data[0];
            (params->data[0] == 0x00) ? printf("led on\n\r") : printf("led off\n\r"); // print led toggle
        }
        /* Print the data if more than 1 byte is written */
        else {
            printf("Data received: length = %d, data = 0x",params->len);
            for(int x=0; x < params->len; x++) {
                printf("%x", params->data[x]);
            }
            printf("\n\r");
        }
        /* Update the readChar with the value of writeChar */
//        BLE::Instance(BLE::DEFAULT_INSTANCE).gattServer().write(readChar.getValueHandle(), params->data, params->len);
        BLE::Instance(BLE::DEFAULT_INSTANCE).gattServer().write(
            readChar.getValueHandle(),
            (uint8_t *)(&counter),
            sizeof(counter)
        );
    }
}

/**
 * Initialization callback
 */
void bleInitComplete(BLE::InitializationCompleteCallbackContext *params)
{
    BLE &ble          = params->ble;
    ble_error_t error = params->error;

    if (error != BLE_ERROR_NONE) {
        return;
    }

    // Set event callbacks
    ble.gap().onConnection(connectionCallback);
    ble.gap().onDisconnection(disconnectionCallback);
    ble.gattServer().onDataWritten(writeCharCallback);
    ble.gattServer().onDataRead(readCharCallback);

    // Setup advertising
    // BLE only, no classic BT
    ble.gap().accumulateAdvertisingPayload(
        GapAdvertisingData::BREDR_NOT_SUPPORTED |
        GapAdvertisingData::LE_GENERAL_DISCOVERABLE
    );

    // Configure GAP settings
    // Advertising Type
    ble.gap().setAdvertisingType(
        GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED
    );
    // Device Name
    ble.gap().accumulateAdvertisingPayload(
        GapAdvertisingData::COMPLETE_LOCAL_NAME,
        (uint8_t *)DEVICE_NAME,
        sizeof(DEVICE_NAME)
    );
    // UUID's broadcast in advertising packet
    ble.gap().accumulateAdvertisingPayload(
        GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS,
        (uint8_t *)uuid16_list,
        sizeof(uuid16_list)
    );
    // Advertising interval
    ble.gap().setAdvertisingInterval(100);  // 100ms

    // Add our custom service
    ble.addService(customService);

    // Start advertising
    ble.gap().startAdvertising();
}

/**
 *  Main loop
 */
int main(void)
{
    // Initialize stuff
//    printf("\n\r********* Starting Main Loop *********\n\r");

    BLE& ble = BLE::Instance(BLE::DEFAULT_INSTANCE);
    ble.init(bleInitComplete);

    // SpinWait for initialization to complete.
    // This is necessary because the BLE object is used in the main loop below.
    while (!ble.hasInitialized()) { /* spin loop */ }

    //Infinite loop waiting for BLE interrupt events
    while (true) {
        ble.waitForEvent();
    }
}

