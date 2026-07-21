#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <DHT.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//====================================================
// PIN CONFIGURATION
//====================================================

#define WIFI_CHANNEL 11

#define MQ6_PIN          32
#define MQ7_PIN          33

#define DHTPIN           4
#define DHTTYPE          DHT22

#define FAN_RELAY_PIN    18
#define VALVE_RELAY_PIN  19

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 20, 4);

//====================================================
// THRESHOLD
//====================================================

// sementara
// nanti tinggal dikalibrasi

#define LPG_LIMIT        1000.0
#define CO_LIMIT         50.0

// hysteresis

#define LPG_RESET        10.0
#define CO_RESET         45.0

//====================================================
// ESP NOW RECEIVER MAC
//====================================================

uint8_t receiverMAC[] =
{
  0x88,
  0x57,
  0x21,
  0x2D,
  0x34,
  0x84
};

//====================================================
// DATA STRUCTURE
//====================================================

typedef struct
{

  float lpg;

  float co;

  float temp;

  bool gasAlarm;

  bool fanStatus;

  bool valveStatus;

  uint32_t packetNumber;

}
SensorData;

SensorData dataSend;

//====================================================
// GLOBAL VARIABLE
//====================================================

unsigned long lastSend = 0;

bool relayState = false;

uint32_t packetCounter = 0;

unsigned long lastLCD = 0;
byte lcdPage = 0;

bool sendSuccess = false;

//====================================================
// MQ7 CALIBRATION TABLE
//====================================================

const int adcTable[] =
{
    215,
    217,
    220,
    222,
    225,
    226,
    227,
    228,
    230,
    232,
    235,
    237,
    239,
    242,
    249,
    256,
    262,
    281
};

const float ppmTable[] =
{
      0,
     11,
     14,
     16,
     17,
     19,
     21,
     22,
     23,
     25,
     27,
     28,
     30,
     36,
     43,
     57,
    121,
    197
};

const int tableSize = sizeof(adcTable) / sizeof(adcTable[0]);

//====================================================
// READ ADC AVERAGE
//====================================================

int readAverage(int pin)
{

  long total = 0;

  for(int i=0;i<20;i++)
  {

    total += analogRead(pin);

    delay(5);

  }

  return total / 20;

}

//====================================================
// MQ6 CONVERSION
//====================================================

float mq6ToPPM(int adc)
{
    float ppm = ((adc - 1968) * 1131.2) / 1498.0;

    if (ppm < 0)
        ppm = 0;

    return ppm;
}

//====================================================
// MQ7 CONVERSION
//====================================================

float mq7ToPPM(int adc)
{
    if (adc <= adcTable[0])
        return ppmTable[0];

    if (adc >= adcTable[tableSize - 1])
        return ppmTable[tableSize - 1];

    for (int i = 0; i < tableSize - 1; i++)
    {
        if (adc >= adcTable[i] && adc <= adcTable[i + 1])
        {
            float x1 = adcTable[i];
            float x2 = adcTable[i + 1];

            float y1 = ppmTable[i];
            float y2 = ppmTable[i + 1];

            return y1 + (adc - x1) * (y2 - y1) / (x2 - x1);
        }
    }

    return 0;
}

//====================================================
// ESP NOW CALLBACK
//====================================================

void onSent
(
    const wifi_tx_info_t *info,
    esp_now_send_status_t status
)
{
    sendSuccess = (status == ESP_NOW_SEND_SUCCESS);

    Serial.println(sendSuccess ? "SUCCESS" : "FAILED");
}

//====================================================
// RELAY CONTROL
//====================================================

void updateRelay
(
    float lpg,
    float co
)
{

    // relay ON

    if(
        lpg >= LPG_LIMIT || co >= CO_LIMIT
    )
    {
        relayState = true;
    }

    // relay OFF
    // hysteresis

    else if
    (
        relayState &&
        lpg <= LPG_RESET &&
        co <= CO_RESET
    )
    {
        relayState = false;
    }

    digitalWrite
    (
        FAN_RELAY_PIN,
        relayState
    );

    digitalWrite
    (
        VALVE_RELAY_PIN,
        relayState
    );

}

//====================================================
// SERIAL PRINT
//====================================================

void printData
(
    int mq6ADC,
    int mq7ADC
)

{

    Serial.println();

    Serial.println("==============================");

    Serial.print("Packet : ");

    Serial.println(packetCounter);

    Serial.print("MQ6 ADC : ");

    Serial.println(mq6ADC);

    Serial.print("MQ7 ADC : ");

    Serial.println(mq7ADC);

    Serial.print("LPG : ");

    Serial.print(dataSend.lpg);

    Serial.println(" ppm");

    Serial.print("CO : ");

    Serial.print(dataSend.co);

    Serial.println(" ppm");

    Serial.print("Temp : ");

    Serial.print(dataSend.temp);

    Serial.println(" C");

    Serial.print("Fan : ");

    Serial.println
    (
        dataSend.fanStatus ?
        "ON" :
        "OFF"
    );

    Serial.print("Valve : ");

    Serial.println
    (
        dataSend.valveStatus ?
        "CLOSED" :
        "OPEN"
    );

    Serial.println("==============================");

    // Serial.print(packetCounter);
    // Serial.print(",");

    // Serial.print(mq6ADC);
    // Serial.print(",");

    // Serial.print(mq7ADC);
    // Serial.print(",");

    // Serial.print(dataSend.lpg);
    // Serial.print(",");

    // Serial.print(dataSend.co);
    // Serial.print(",");

    // Serial.print(dataSend.temp);
    // Serial.print(",");

    // Serial.print(dataSend.fanStatus);
    // Serial.print(",");

    // Serial.println(dataSend.valveStatus);

}

void updateLCD()
{
    if (millis() - lastLCD < 1000)
        return;

    lastLCD = millis();

    lcd.clear();

    switch (lcdPage)
    {
        //--------------------------------------------------
        // PAGE 1
        //--------------------------------------------------

        case 0:

            lcd.setCursor(0,0);
            lcd.print("LPG:");
            lcd.print(dataSend.lpg,1);
            lcd.print("ppm");

            lcd.setCursor(0,1);
            lcd.print("CO :");
            lcd.print(dataSend.co,1);
            lcd.print("ppm");

            lcd.setCursor(0,2);
            lcd.print("Temp:");
            lcd.print(dataSend.temp,1);
            lcd.print((char)223);
            lcd.print("C");

            break;

        //--------------------------------------------------
        // PAGE 2
        //--------------------------------------------------

        case 1:
            lcd.setCursor(0,0);
            lcd.print("Pkt:");
            lcd.print(packetCounter);

            lcd.setCursor(0,1);

            lcd.print("Fan : ");

            lcd.print
            (
                dataSend.fanStatus ?
                "ON" :
                "OFF"
            );

            lcd.setCursor(0,2);

            lcd.print("Valve:");

            lcd.print
            (
                dataSend.valveStatus ?
                "CLOSE" :
                "OPEN "
            );

            break;

        //--------------------------------------------------
        // PAGE 3
        //--------------------------------------------------

        case 2:

            lcd.setCursor(0,0);

            lcd.print("Alarm:");

            lcd.print
            (
                dataSend.gasAlarm ?
                "YES" :
                "NO "
            );

            lcd.setCursor(0,1);

            lcd.print("Send:");

            lcd.print
            (
                sendSuccess ?
                "SUCCESS" :
                "FAILED "
            );

            break;
    }

    lcdPage++;

    if(lcdPage > 2)
        lcdPage = 0;
}

//====================================================
// SENSOR WARM UP
//====================================================

void sensorWarmUp()
{
    Serial.println();
    Serial.println("==================================");
    Serial.println("MQ SENSOR WARM UP");
    Serial.println("==================================");

    for (int i = 180; i >= 0; i--)
    {
        lcd.clear();

        lcd.setCursor(0, 0);
        lcd.print("Sensor Warm Up");

        lcd.setCursor(0, 1);
        lcd.print("Wait : ");
        lcd.print(i);
        lcd.print(" sec");

        Serial.print("Warm Up : ");
        Serial.print(i);
        Serial.println(" second");

        delay(1000);
    }

    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("Warm Up Done");

    lcd.setCursor(0,1);
    lcd.print("Starting...");

    Serial.println();
    Serial.println("Warm Up Complete");
    Serial.println("Starting System...");

    delay(2000);

    lcd.clear();
}

//====================================================
// SETUP
//====================================================

void setup()
{

    Serial.begin(115200);

    Wire.begin(21, 22);

    lcd.init();
    lcd.backlight();

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Chicken Alert");

    lcd.setCursor(0, 1);
    lcd.print("Receiver Ready");

    delay(2000);

    lcd.clear();

    delay(1000);

    dht.begin();

    sensorWarmUp();

    pinMode(FAN_RELAY_PIN, OUTPUT);
    pinMode(VALVE_RELAY_PIN, OUTPUT);

    // Relay OFF saat startup
    digitalWrite(FAN_RELAY_PIN, LOW);
    digitalWrite(VALVE_RELAY_PIN, LOW);

    WiFi.mode(WIFI_STA);

    delay(300);



    Serial.println();
    Serial.println("==================================");
    Serial.println("ESP32 SENSOR NODE");
    Serial.println("==================================");

    Serial.print("Sender MAC : ");
    Serial.println(WiFi.macAddress());

    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);

    if (esp_now_init() != ESP_OK)
    {
        Serial.println("ESP-NOW INIT FAILED");

        while (true)
        {
            delay(100);
        }
    }

    esp_now_register_send_cb(onSent);

    esp_now_peer_info_t peerInfo = {};

    memcpy(peerInfo.peer_addr, receiverMAC, 6);

    peerInfo.channel = WIFI_CHANNEL;

    peerInfo.encrypt = false;

    if (esp_now_is_peer_exist(receiverMAC))
    {
        esp_now_del_peer(receiverMAC);
    }

    esp_err_t peerResult =
        esp_now_add_peer(&peerInfo);

    if (peerResult == ESP_OK)
    {
        Serial.println("Receiver Added");
    }
    else
    {
        Serial.print("Add Peer Error : ");
        Serial.println(peerResult);

        while (true)
        {
            delay(100);
        }
    }

}

//====================================================
// LOOP
//====================================================

void loop()
{

    if (millis() - lastSend < 2000)
        return;

    lastSend = millis();

    //------------------------------------------------
    // READ SENSOR
    //------------------------------------------------

    int mq6ADC = readAverage(MQ6_PIN);

    int mq7ADC = readAverage(MQ7_PIN);

    float temperature = dht.readTemperature() + 1.2; 
    //1.4 adalah nilai offset dengan sensor komersil

    if (isnan(temperature))
    {
        Serial.println("DHT ERROR");
        return;
    }

    //------------------------------------------------
    // CONVERT
    //------------------------------------------------

    dataSend.lpg = mq6ToPPM(mq6ADC);

    dataSend.co = mq7ToPPM(mq7ADC);

    dataSend.temp = temperature;

    //------------------------------------------------
    // RELAY CONTROL
    //------------------------------------------------

    updateRelay
    (
        dataSend.lpg,
        dataSend.co
    );

    dataSend.gasAlarm = relayState;

    dataSend.fanStatus = relayState;

    dataSend.valveStatus = relayState;

    //------------------------------------------------
    // PACKET NUMBER
    //------------------------------------------------

    packetCounter++;

    dataSend.packetNumber = packetCounter;

    //------------------------------------------------
    // SEND DATA
    //------------------------------------------------

    esp_err_t result =
        esp_now_send
        (
            receiverMAC,
            (uint8_t *)&dataSend,
            sizeof(dataSend)
        );

    if (result != ESP_OK)
    {
        Serial.print("esp_now_send Error : ");
        Serial.println(result);
    }

    //------------------------------------------------
    // SERIAL
    //------------------------------------------------

    printData
    (
        mq6ADC,
        mq7ADC
    );

    updateLCD();

}
