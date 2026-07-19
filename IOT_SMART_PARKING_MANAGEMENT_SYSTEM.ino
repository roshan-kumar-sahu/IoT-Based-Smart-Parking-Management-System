/*
=========================================================
          SMART PARKING SYSTEM V2.0 FINAL
---------------------------------------------------------
Developer : Roshan Kumar Sahu
Board     : ESP32 Dev Module
Version   : 3.0
=========================================================
*/
//=========================================================
// BLYNK
//=========================================================

#define BLYNK_TEMPLATE_ID "TMPL3oi9bp_eH"
#define BLYNK_TEMPLATE_NAME "Smart Parking System"
#define BLYNK_AUTH_TOKEN "iYqZfOhyRt17Hqj_t3aby7fP2ousMV5k"

#include <ThingSpeak.h>
#include <WiFiClient.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

//=========================================================
// LCD
//=========================================================

LiquidCrystal_I2C lcd(0x27, 16, 2);

//=========================================================
// SERVO
//=========================================================

Servo gateServo;

//=========================================================
// PIN DEFINITIONS
//=========================================================

// LCD
#define SDA_PIN     21
#define SCL_PIN     22

// Entry / Exit IR
#define ENTRY_IR    13
#define EXIT_IR     25

// Parking Slot IR
#define SLOT1_IR    14
#define SLOT2_IR    27
#define SLOT3_IR    26

// RGB Slot 1
#define S1_RED      32
#define S1_GREEN    33

// RGB Slot 2
#define S2_RED      18
#define S2_GREEN    19

// RGB Slot 3
#define S3_RED      23
#define S3_GREEN    4

// Servo
#define SERVO_PIN   5

// Passive Buzzer
#define BUZZER_PIN  15

//=========================================================
// SERVO SETTINGS
//=========================================================

const byte GATE_CLOSED = 0;
const byte GATE_OPEN   = 60;

//=========================================================
// TIMER VARIABLES
//=========================================================

unsigned long currentMillis = 0;

unsigned long sensorTimer = 0;
unsigned long lcdTimer = 0;
unsigned long rgbTimer = 0;
unsigned long servoTimer = 0;
unsigned long buzzerTimer = 0;

//=========================================================
// SENSOR VARIABLES
//=========================================================

bool entryDetected = false;
bool exitDetected = false;

bool slot1Occupied = false;
bool slot2Occupied = false;
bool slot3Occupied = false;

//=========================================================
// PARKING VARIABLES
//=========================================================

byte freeSlots = 3;
byte targetSlot = 0;


//=========================================================
// PARKING STATISTICS
//=========================================================

byte currentVehicles = 0;

unsigned int totalEntries = 0;

unsigned int totalExits = 0;

bool lastSlot1 = false;
bool lastSlot2 = false;
bool lastSlot3 = false;

const byte TOTAL_SLOTS = 3;

//=========================================================
// GATE VARIABLES
//=========================================================

bool entryLock = false;
bool exitLock = false;

unsigned long gateOpenTime = 0;

//=========================================================
// GATE COMMANDS
//=========================================================

bool openGateCommand = false;
bool closeGateCommand = false;
//=========================================================
// SERVO STATUS
//=========================================================

bool servoIsOpen = false;

//=========================================================
// EXIT MANAGER
//=========================================================

bool exitDisplay = false;

unsigned long exitDisplayTimer = 0;

//=========================================================
// BUZZER VARIABLES
//=========================================================

bool melodyStart = false;
bool fullAlarmStart = false;

byte melodyStep = 0;
bool playWelcomeMelody = false;
bool playFullAlarm = false;
//=========================================================
// BUZZER MANAGER
//=========================================================

bool buzzerPlaying = false;

unsigned long buzzerStartTime = 0;

unsigned long buzzerNoteTime = 0;
//=========================================================
// LCD VARIABLES
//=========================================================

String lcdLine1 = "";
String lcdLine2 = "";

//=========================================================
// LCD PAGE MANAGER
//=========================================================

byte lcdPage = 0;

unsigned long lcdPageTimer = 0;

const unsigned long LCD_PAGE_DELAY = 3000;   // 3 seconds

//=========================================================
// WIFI
//=========================================================

const char* ssid="Roshan";
const char* pass="12121212";

bool wifiConnected = false;

unsigned long wifiRetryTimer = 0;

//=========================================================
// BlynkTimer
//=========================================================

BlynkTimer timer;

bool blynkConnected = false;
unsigned long blynkRetryTimer = 0;

//=========================================================
// THINGSPEAK
//=========================================================

WiFiClient client;

unsigned long thingSpeakTimer = 0;

bool thingSpeakConnected = false;

unsigned long thingSpeakUploadTimer = 0;

unsigned long CHANNEL_ID = 3421479;

const char WRITE_API_KEY[] = "H5QQPOJVZ2J8UODD";

//=========================================================
// PARKING DURATION
//=========================================================

//unsigned long slotEntryTime[3] = {0, 0, 0};

//unsigned long slotDuration[3] = {0, 0, 0};

//bool slotTimerRunning[3] = {false, false, false};

//=========================================================
// PARKING TIMER & BILLING
//=========================================================

unsigned long slotEntryTime[3] = {0, 0, 0};

unsigned long slotDuration[3] = {0, 0, 0};

unsigned int slotCharge[3] = {0, 0, 0};

bool slotTimerRunning[3] = {false, false, false};

bool slotOccupied[TOTAL_SLOTS];
bool lastSlotOccupied[TOTAL_SLOTS];

const char* slotName[TOTAL_SLOTS] =
{
    "Slot 1",
    "Slot 2",
    "Slot 3"
};

//=========================================================
// BLYNK TIMER
//=========================================================

unsigned long blynkUpdateTimer = 0;

//=========================================================
// EXIT BILL
//=========================================================

byte lastExitSlot = 255;

//=========================================================
// EXIT DISPLAY
//=========================================================

byte exitSlot = 255;

unsigned long exitDuration = 0;

unsigned int exitCharge = 0;

//=========================================================
// REVENUE
//=========================================================

unsigned long totalRevenue = 0;

//=========================================================
// SIMULATION MODE
//=========================================================

bool simulationMode = false;

//simulationMode = false;

//=========================================================
// PARKING HISTORY
//=========================================================

const byte HISTORY_SIZE = 10;

byte historySlot[HISTORY_SIZE];

unsigned long historyDuration[HISTORY_SIZE];

unsigned int historyBill[HISTORY_SIZE];

byte historyIndex = 0;

//=========================================================
// NEW LCD DISPLAY SETTINGS
//=========================================================

void updateLCDPages();
//=========================================================
// STATE MACHINE
//=========================================================

enum ParkingState
{
    IDLE,

    ENTRY_DETECTED,

    OPEN_GATE,

    WAIT_FOR_ENTRY_CLEAR,

    CLOSE_GATE,

    PARKING_FULL
};

ParkingState currentState = IDLE;

//=========================================================
// FUNCTION PROTOTYPES
//=========================================================

// Hardware
void initializeHardware();
void readSensors();

// Parking Logic
void updateParkingStatus();
void updateParkingStatistics();
void updateStateMachine();
void updateExitManager();

// Hardware Output
void updateServo();
void updateBuzzer();
void updateRGB();
void updateLCD();
void showLCD(String line1, String line2);

// IoT
void connectWiFi();
void connectBlynk();
void sendToBlynk();
void sendToThingSpeak();

//Parking timer
void updateParkingTimer();

//Simulation 
void simulationModeHandler();

// Parking History
void updateParkingHistory(byte slot, unsigned long duration, unsigned int bill);

void printParkingHistory();

//=========================================================
// SETUP
//=========================================================

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println("ESP32 Started");

    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);

    Serial.println("Connecting WiFi...");
    Blynk.config(BLYNK_AUTH_TOKEN);

    initializeHardware();

    Serial.println("--------------------------------");
    Serial.println("SMART PARKING V2.0 FINAL");
    Serial.println("SYSTEM READY");
    Serial.println("--------------------------------");

    timer.setInterval(1000L, sendToBlynk);
    ThingSpeak.begin(client);

    Serial.println("ThingSpeak Ready");
}

//=========================================================
// LOOP
//=========================================================

void loop()
{   
    currentMillis = millis();

connectWiFi();

Blynk.run();

if(simulationMode)
{
    simulationModeHandler();
}

connectBlynk();

timer.run();

    // Read sensors every 100 ms
    if(currentMillis - sensorTimer >= 100)
    {
        sensorTimer = currentMillis;

        readSensors();

        updateParkingStatus();

        updateParkingStatistics();
    }

    // State Machine
    updateStateMachine();

    updateExitManager();

    updateParkingTimer();

    // Managers
    updateServo();

    updateBuzzer();

    //-----------------------------
// Blynk Update Every 1 Second
//-----------------------------
    if(currentMillis - blynkUpdateTimer >= 1000)
    {
    blynkUpdateTimer = currentMillis;

    sendToBlynk();
    }

    // Update RGB every 100 ms
    if(currentMillis - rgbTimer >= 100)
    {
        rgbTimer = currentMillis;

        updateRGB();
    }

    // Update LCD every 200 ms
    if(currentMillis - lcdTimer >= 200)
    {
        lcdTimer = currentMillis;

        updateLCD();
    }

    if(currentMillis - thingSpeakUploadTimer >= 20000)
    {
        thingSpeakUploadTimer = currentMillis;

        if(wifiConnected)
    {
        sendToThingSpeak();
    }
    }
}

//=========================================================
// INITIALIZE HARDWARE
//=========================================================

void initializeHardware()
{
    // I2C
    Wire.begin(SDA_PIN, SCL_PIN);

    // LCD
    lcd.init();
    lcd.backlight();

    // Servo
    gateServo.attach(SERVO_PIN);
    gateServo.write(GATE_CLOSED);

    // IR Sensors
    pinMode(ENTRY_IR, INPUT);
    pinMode(EXIT_IR, INPUT);

    pinMode(SLOT1_IR, INPUT);
    pinMode(SLOT2_IR, INPUT);
    pinMode(SLOT3_IR, INPUT);

    // RGB LEDs
    pinMode(S1_RED, OUTPUT);
    pinMode(S1_GREEN, OUTPUT);

    pinMode(S2_RED, OUTPUT);
    pinMode(S2_GREEN, OUTPUT);

    pinMode(S3_RED, OUTPUT);
    pinMode(S3_GREEN, OUTPUT);

    // Passive Buzzer
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    // Default RGB = GREEN (All Slots Free)
    digitalWrite(S1_RED, LOW);
    digitalWrite(S1_GREEN, HIGH);

    digitalWrite(S2_RED, LOW);
    digitalWrite(S2_GREEN, HIGH);

    digitalWrite(S3_RED, LOW);
    digitalWrite(S3_GREEN, HIGH);

    // Welcome Screen
    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("SMART PARKING");

    lcd.setCursor(0,1);
    lcd.print("INITIALIZING");

    delay(1500);

    lcd.clear();

    readSensors();

    lastSlot1 = slot1Occupied;
    lastSlot2 = slot2Occupied;
    lastSlot3 = slot3Occupied;
}

//=========================================================
// READ ALL SENSORS
//=========================================================

void readSensors()
{
    if(simulationMode)
        return;
    entryDetected = (digitalRead(ENTRY_IR) == LOW);
    exitDetected  = (digitalRead(EXIT_IR) == LOW);

    slot1Occupied = (digitalRead(SLOT1_IR) == LOW);
    slot2Occupied = (digitalRead(SLOT2_IR) == LOW);
    slot3Occupied = (digitalRead(SLOT3_IR) == LOW);

    slotOccupied[0] = slot1Occupied;
    slotOccupied[1] = slot2Occupied;
    slotOccupied[2] = slot3Occupied;
}

//=========================================================
// UPDATE PARKING STATUS
//=========================================================

void updateParkingStatus()
{
    freeSlots = 0;

    if (!slot1Occupied) freeSlots++;
    if (!slot2Occupied) freeSlots++;
    if (!slot3Occupied) freeSlots++;

    if (!slot1Occupied)
        targetSlot = 1;
    else if (!slot2Occupied)
        targetSlot = 2;
    else if (!slot3Occupied)
        targetSlot = 3;
    else
        targetSlot = 0;

    //Serial.print("Current: ");
    //Serial.print(currentVehicles);

    //Serial.print(" | Entry: ");
    //Serial.print(totalEntries);

    //Serial.print(" | Exit: ");
    //Serial.println(totalExits);
}

//=========================================================
// UPDATE PARKING STATISTICS
//=========================================================

void updateParkingStatistics()
{

    // Slot-1

    if(!lastSlot1 && slot1Occupied)
    {
        if(currentVehicles < TOTAL_SLOTS)
         {
          currentVehicles++;
         }
        totalEntries++;
        Serial.println("Vehicle Parked -> Slot 1");

        slotEntryTime[0] = currentMillis;

        slotTimerRunning[0] = true;

        Serial.println("Slot-1 Timer Started");
    }

    if(lastSlot1 && !slot1Occupied)
    {
        if(currentVehicles > 0)
        {
         currentVehicles--;
        }

        totalExits++;

        Serial.println("Vehicle Left -> Slot 1");
        lastExitSlot = 0;

        slotDuration[0] = currentMillis - slotEntryTime[0];

        slotCharge[0] = slotDuration[0] / 1000;

        slotTimerRunning[0] = false;
        exitSlot = 0;
        exitDuration = slotDuration[0];
        exitCharge = slotCharge[0];

        Serial.print("Slot-1 Duration : ");
        Serial.print(slotDuration[0] / 1000);
        Serial.println(" sec");

        Serial.print("Bill : ₹");
        Serial.println(slotCharge[0]);
        totalRevenue += slotCharge[0];
        updateParkingHistory(1, slotDuration[0], slotCharge[0]);

        printParkingHistory();
    }
    // Slot-2

    if(!lastSlot2 && slot2Occupied)
    {
        if(currentVehicles < TOTAL_SLOTS)
         {
          currentVehicles++;
         }
        totalEntries++;
        Serial.println("Vehicle Parked -> Slot 2");

        slotEntryTime[1] = currentMillis;

        slotTimerRunning[1] = true;

        Serial.println("Slot-2 Timer Started");
    }

    if(lastSlot2 && !slot2Occupied)
    {
        if(currentVehicles > 0)
        {
         currentVehicles--;
        }

        totalExits++;

        Serial.println("Vehicle Left -> Slot 2");
        lastExitSlot = 1;

        slotDuration[1] = currentMillis - slotEntryTime[0];

        slotCharge[1] = slotDuration[1] / 1000;

        slotTimerRunning[1] = false;
        exitSlot = 1;
        exitDuration = slotDuration[1];
        exitCharge = slotCharge[1];

        Serial.print("Slot-2 Duration : ");
        Serial.print(slotDuration[1] / 1000);
        Serial.println(" sec");

        Serial.print("Bill : ₹");
        Serial.println(slotCharge[1]);
        totalRevenue += slotCharge[1];

        updateParkingHistory(2, slotDuration[1], slotCharge[1]);
        printParkingHistory();
    }

    // Slot-3

    if(!lastSlot3 && slot3Occupied)
    {
        if(currentVehicles < TOTAL_SLOTS)
         {
          currentVehicles++;
         }
        totalEntries++;
        Serial.println("Vehicle Parked -> Slot 3");

        slotEntryTime[2] = currentMillis;

        slotTimerRunning[2] = true;

        Serial.println("Slot-3 Timer Started");
    }

    if(lastSlot3 && !slot3Occupied)
    {
        if(currentVehicles > 0)
        {
         currentVehicles--;
        }

        totalExits++;

        Serial.println("Vehicle Left -> Slot 3");
        lastExitSlot = 2;

        slotDuration[2] = currentMillis - slotEntryTime[2];

        slotCharge[2] = slotDuration[2] / 1000;

        slotTimerRunning[2] = false;
        exitSlot = 2;
        exitDuration = slotDuration[2];
        exitCharge = slotCharge[2];

        Serial.print("Slot-3 Duration : ");
        Serial.print(slotDuration[2] / 1000);
        Serial.println(" sec");

        Serial.print("Bill : ₹");
        Serial.println(slotCharge[2]);
        totalRevenue += slotCharge[2];

        updateParkingHistory(3, slotDuration[2], slotCharge[2]);
        printParkingHistory();
    }

    lastSlot1 = slot1Occupied;
    lastSlot2 = slot2Occupied;
    lastSlot3 = slot3Occupied;

    //slotEntryTime[0] = millis();
    //slotTimerRunning[0] = true;

    //slotTimerRunning[0] = false;

    //slotParkingTime[0] = millis() - slotEntryTime[0];

    //slotCharge[0] = slotParkingTime[0] / 1000;
    
}

//=========================================================
// STATE MACHINE
//=========================================================

void updateStateMachine()
{

    switch(currentState)
    {

        //-----------------------------------------
        case IDLE:

    // Vehicle Entry
            if(entryDetected && !entryLock)
                {
                    entryLock = true;
                    currentState = ENTRY_DETECTED;
                }
        break;

        //-----------------------------------------
        case ENTRY_DETECTED:
        //-----------------------------------------

            if(freeSlots > 0)
            {

                openGateCommand = true;

                playWelcomeMelody = true;
                fullAlarmStart = false;
                currentState = OPEN_GATE;

            }

            else
            {

                playFullAlarm = true;
                playWelcomeMelody = false;

                currentState = PARKING_FULL;

            }

        break;

        //-----------------------------------------
        case OPEN_GATE:
        //-----------------------------------------

            if(!entryDetected)
            {

                currentState = WAIT_FOR_ENTRY_CLEAR;

            }

        break;

        //-----------------------------------------
        case WAIT_FOR_ENTRY_CLEAR:
        //-----------------------------------------

            if(currentMillis-gateOpenTime>=1000)
            {

                closeGateCommand = true;

                currentState = CLOSE_GATE;

            }

        break;

        //-----------------------------------------
        case CLOSE_GATE:
        //-----------------------------------------

            if(!openGateCommand && !closeGateCommand)
            {

                currentState = IDLE;

            }

        break;

        //-----------------------------------------
        case PARKING_FULL:
        //-----------------------------------------

            if(!entryDetected)
            {
                currentState = IDLE;

            }

        break;

    }
    // Reset Entry Lock
        if(!entryDetected)
         {
          entryLock = false;
         }

// Reset Exit Lock
        if(!exitDetected)
         {
          exitLock = false;
         }

}

//=========================================================
// EXIT MANAGER
//=========================================================

void updateExitManager()
{

    if(exitDetected && !exitLock)
    {

        exitLock = true;

        exitDisplay = true;

        exitDisplayTimer = currentMillis;

        Serial.println("Vehicle Leaving");

    }

    if(!exitDetected)
    {

        exitLock = false;

    }

    if(exitDisplay)
    {

        if(currentMillis-exitDisplayTimer>=2000)
        {

            exitDisplay=false;

        }

    }

}

//=========================================================
// SERVO MANAGER
//=========================================================

void updateServo()
{
    // Open Gate
    if(openGateCommand)
    {
        gateServo.write(GATE_OPEN);

        gateOpenTime = currentMillis;

        servoIsOpen = true;

        openGateCommand = false;

        Serial.println("Gate Open");
    }

    // Close Gate
    if(closeGateCommand)
    {
        gateServo.write(GATE_CLOSED);

        servoIsOpen = false;

        closeGateCommand = false;

        Serial.println("Gate Closed");

        entryLock = false;
    }
}

//=========================================================
// BUZZER MANAGER
//=========================================================

void updateBuzzer()
{

    //-------------------------------
    // Welcome Melody
    //-------------------------------
    // Full alarm has highest priority
    if(playFullAlarm)
    {
      playWelcomeMelody = false;
      buzzerPlaying = false;
    } 
    if(playWelcomeMelody)
    {

        if(!buzzerPlaying)
        {
            buzzerPlaying = true;
            melodyStep = 0;
            buzzerNoteTime = currentMillis;
        }

        if(currentMillis - buzzerNoteTime >= 180)
        {
            buzzerNoteTime = currentMillis;

            switch(melodyStep)
            {

                case 0:
                    tone(BUZZER_PIN,523);
                    break;

                case 1:
                    tone(BUZZER_PIN,659);
                    break;

                case 2:
                    tone(BUZZER_PIN,784);
                    break;

                case 3:

                    noTone(BUZZER_PIN);

                    playWelcomeMelody = false;

                    buzzerPlaying = false;

                    melodyStep = 0;

                    break;

            }

            melodyStep++;

        }

    }

    //-------------------------------
    // Parking Full Alarm
    //-------------------------------

    if(playFullAlarm)
    {

        if(!fullAlarmStart)
        {
            fullAlarmStart = true;

            buzzerStartTime = currentMillis;

            tone(BUZZER_PIN,1000);
        }

        if(currentMillis-buzzerStartTime>=5000)
        {

            noTone(BUZZER_PIN);

            playFullAlarm=false;

            fullAlarmStart=false;

        }

    }

}

//=========================================================
// RGB MANAGER
//=========================================================

void updateRGB()
{
    digitalWrite(S1_RED, slot1Occupied);
    digitalWrite(S1_GREEN, !slot1Occupied);

    digitalWrite(S2_RED, slot2Occupied);
    digitalWrite(S2_GREEN, !slot2Occupied);

    digitalWrite(S3_RED, slot3Occupied);
    digitalWrite(S3_GREEN, !slot3Occupied);
}

//=========================================================
// LCD HELPER
//=========================================================

void showLCD(String line1, String line2)
{
    if (lcdLine1 == line1 && lcdLine2 == line2)
        return;

    lcdLine1 = line1;
    lcdLine2 = line2;

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print(line1);

    lcd.setCursor(0, 1);
    lcd.print(line2);
}

//=========================================================
// LCD PAGE MANAGER
//=========================================================

void updateLCDPages()
{
    if(currentMillis - lcdPageTimer < LCD_PAGE_DELAY)
        return;

    lcdPageTimer = currentMillis;

    lcdPage++;

    if(lcdPage > 4)
        lcdPage = 0;
}

//=========================================================
// LCD MANAGER
//=========================================================

void updateLCD()
{  
    if(exitDisplay)
    {
    String line1 = "S" + String(exitSlot + 1);

    String line2 = String(exitDuration / 1000) + "s Rs." + String(exitCharge);

    showLCD(line1, line2);

    return;
    }

     if(currentState == PARKING_FULL)
     {
     showLCD("PARKING FULL","SORRY");
     return;
     }

     if(currentState == OPEN_GATE || currentState == WAIT_FOR_ENTRY_CLEAR)
     {
      showLCD("WELCOME", "GO TO S" + String(targetSlot));
    return;
     }
    if (freeSlots == 0)
    {
        showLCD("PARKING FULL", "SORRY");
        return;
    }

    updateLCDPages();

switch(lcdPage)
{
    case 0:
    {
        String slots;
        slots.reserve(10);

        if (!slot1Occupied) slots += "S1 ";
        if (!slot2Occupied) slots += "S2 ";
        if (!slot3Occupied) slots += "S3 ";

        showLCD("FREE : " + String(freeSlots), slots);
    }
    break;

    case 1:
        showLCD("VEHICLES",
                "Now : " + String(currentVehicles));
    break;

    case 2:
        showLCD("ENTRY:" + String(totalEntries),
                "EXIT :" + String(totalExits));
    break;

    case 3:
        showLCD("TODAY EARN",
                "Rs." + String(totalRevenue));
    break;

    case 4:
        showLCD(
            wifiConnected ? "WiFi : ON" : "WiFi : OFF",
            blynkConnected ? "Blynk : ON" : "Blynk : OFF"
        );
    break;
}
}

//====================================================
// WIFI MANAGER
//====================================================

void connectWiFi()
{
    static bool wasConnected = false;

    if (WiFi.status() == WL_CONNECTED)
    {
        wifiConnected = true;

        if (!wasConnected)
        {
            Serial.println("================================");
            Serial.println("WiFi Connected");
            Serial.print("IP Address : ");
            Serial.println(WiFi.localIP());
            Serial.println("================================");

            wasConnected = true;
        }

        return;
    }

    wifiConnected = false;
    wasConnected = false;

    if (currentMillis - wifiRetryTimer < 5000)
        return;

    wifiRetryTimer = currentMillis;

    Serial.println("Connecting to WiFi...");

    WiFi.begin(ssid, pass);
}

//====================================================
// BLYNK MANAGER
//====================================================

void connectBlynk()
{
    static bool wasConnected = false;

    if (WiFi.status() != WL_CONNECTED)
    {
     blynkConnected = false;

     if (wasConnected)
      {
        Serial.println("Blynk Disconnected");
        wasConnected = false;
      }

     return;
    }

    if (Blynk.connected())
    {
     blynkConnected = true;

     if (!wasConnected)
      {
        Serial.println("Blynk Connected");
        wasConnected = true;
      }

     return;
    }

    if (currentMillis - blynkRetryTimer < 5000)
        return;

    blynkRetryTimer = currentMillis;

    Serial.println("Connecting to Blynk...");

    if (Blynk.connect(3000))   // 3 second timeout
    {
        blynkConnected = true;
        Serial.println("Blynk Connected");
    }
    else
    {
        blynkConnected = false;
        Serial.println("Blynk Connection Failed");
    }
}

//====================================================
// SEND TO BLYNK
//====================================================

void sendToBlynk()
{
    if (!Blynk.connected())
        return;

    // Existing Data
    Blynk.virtualWrite(V0, freeSlots);

    Blynk.virtualWrite(V1, slot1Occupied ? 1 : 0);

    Blynk.virtualWrite(V2, slot2Occupied ? 1 : 0);

    Blynk.virtualWrite(V3, slot3Occupied ? 1 : 0);

    Blynk.virtualWrite(V4, currentVehicles);

    Blynk.virtualWrite(V5, servoIsOpen ? "OPEN" : "CLOSED");

    //===========================
    // Version 3.0
    //===========================

    Blynk.virtualWrite(V6, totalEntries);

    Blynk.virtualWrite(V7, totalExits);

    Blynk.virtualWrite(V8, totalRevenue);

    // Live Timer (Seconds)
    Blynk.virtualWrite(V9, slotDuration[0] / 1000);

    Blynk.virtualWrite(V10, slotDuration[1] / 1000);

    Blynk.virtualWrite(V11, slotDuration[2] / 1000);

    // Live Bill
    Blynk.virtualWrite(V12, slotCharge[0]);

    Blynk.virtualWrite(V13, slotCharge[1]);

    Blynk.virtualWrite(V14, slotCharge[2]);

    // Occupancy %
    byte occupancy = (currentVehicles * 100) / TOTAL_SLOTS;

    Blynk.virtualWrite(V15, occupancy);
}

//====================================================
// SEND TO thingspeak
//====================================================

void sendToThingSpeak()
{
    ThingSpeak.setField(1, freeSlots);

    ThingSpeak.setField(2, slot1Occupied ? 1 : 0);

    ThingSpeak.setField(3, slot2Occupied ? 1 : 0);

    ThingSpeak.setField(4, slot3Occupied ? 1 : 0);

    ThingSpeak.setField(5, currentVehicles);

    ThingSpeak.setField(6, (int)totalEntries);

    ThingSpeak.setField(7, (int)totalExits);

    // 0 = Closed
    // 1 = Open
    ThingSpeak.setField(8, servoIsOpen ? 1 : 0);

    int response = ThingSpeak.writeFields(CHANNEL_ID, WRITE_API_KEY);

    if(response == 200)
     {
     Serial.println("ThingSpeak Upload Success");
     }
else
    {
     Serial.print("ThingSpeak Error : ");
     Serial.println(response);
    }

    Serial.print("S1=");
Serial.print(slotDuration[0] / 1000);

Serial.print(" S2=");
Serial.print(slotDuration[1] / 1000);

Serial.print(" S3=");
Serial.println(slotDuration[2] / 1000);
}

//=========================================================
// UPDATE PARKING TIMER
//=========================================================

void updateParkingTimer()
{
    static unsigned long displayTimer = 0;

    for(byte i = 0; i < TOTAL_SLOTS; i++)
    {
        if(slotTimerRunning[i])
        {
            // Live Parking Time
            slotDuration[i] = currentMillis - slotEntryTime[i];

            // Demo Billing
            slotCharge[i] = slotDuration[i] / 1000;
        }
    }

    // Serial Monitor Update Every 1 Second
    if(currentMillis - displayTimer >= 1000)
    {
        displayTimer = currentMillis;

        for(byte i = 0; i < TOTAL_SLOTS; i++)
        {
            if(slotTimerRunning[i])
            {
                Serial.print("Slot-");
                Serial.print(i + 1);

                Serial.print(" | Time : ");
                Serial.print(slotDuration[i] / 1000);

                Serial.print(" sec | Bill : ₹");

                Serial.println(slotCharge[i]);
            }
        }
    }
}

//=========================================================
// SIMULATION MODE
//=========================================================

void simulationModeHandler()
{
    if(!Serial.available())
        return;

    String cmd = Serial.readStringUntil('\n');

    cmd.trim();
    cmd.toUpperCase();

    if(cmd == "P1")
    {
        slot1Occupied = true;
        Serial.println("SIM -> Slot-1 Parked");
    }

    else if(cmd == "L1")
    {
        slot1Occupied = false;
        Serial.println("SIM -> Slot-1 Left");
    }

    else if(cmd == "P2")
    {
        slot2Occupied = true;
        Serial.println("SIM -> Slot-2 Parked");
    }

    else if(cmd == "L2")
    {
        slot2Occupied = false;
        Serial.println("SIM -> Slot-2 Left");
    }

    else if(cmd == "P3")
    {
        slot3Occupied = true;
        Serial.println("SIM -> Slot-3 Parked");
    }

    else if(cmd == "L3")
    {
        slot3Occupied = false;
        Serial.println("SIM -> Slot-3 Left");
    }

    else if(cmd == "R")
    {
        slot1Occupied = false;
        slot2Occupied = false;
        slot3Occupied = false;

        Serial.println("Simulation Reset");
    }
}

//=========================================================
// PARKING HISTORY MANAGMENT
//=========================================================

void updateParkingHistory(byte slot, unsigned long duration, unsigned int bill)
{
    historySlot[historyIndex] = slot;

    historyDuration[historyIndex] = duration;

    historyBill[historyIndex] = bill;

    historyIndex++;

    if(historyIndex >= HISTORY_SIZE)
    {
        historyIndex = 0;
    }
}

//=========================================================
// PRINT PARKING HISTORY
//=========================================================

void printParkingHistory()
{
    Serial.println();
    Serial.println("========== PARKING HISTORY ==========");

    for(byte i = 0; i < HISTORY_SIZE; i++)
    {
        if(historyBill[i] == 0)
            continue;

        Serial.print("#");
        Serial.print(i + 1);

        Serial.print("  Slot : S");
        Serial.print(historySlot[i]);

        Serial.print("  Time : ");
        Serial.print(historyDuration[i] / 1000);

        Serial.print(" sec");

        Serial.print("  Bill : ₹");
        Serial.println(historyBill[i]);
    }

    Serial.println("=====================================");
    Serial.println();
}
