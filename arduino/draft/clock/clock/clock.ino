#define DEBUG
#undef DEBUG

#define MAX_WAITING_TIME 500

volatile bool isCanCount = false;
volatile long tick;

void connSignal() {
    isCanCount = true;
    tick = millis();
}

void setup() {
    Serial.begin(115200);
    DDRD = B00001000; // pinMode(7, OUTPUT)
    attachInterrupt(digitalPinToInterrupt(2), connSignal, CHANGE);
}

void loop() {
    // put your main code here, to run repeatedly:
    if (isCanCount) {
        if ((millis() - tick) >= MAX_WAITING_TIME) {
            isCanCount = false;
            PORTD ^= B00001000;
            #ifdef DEBUG
            Serial.println("Time up!");
            #endif
        }
        #ifdef DEBUG
        Serial.println("Received signal");
        #endif
    }
}

