#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

#define BLYNK_TEMPLATE_ID "TMPL64q0Aj8z4"
#define BLYNK_TEMPLATE_NAME "ChickAlert"
#define BLYNK_AUTH_TOKEN "RTbT15XMvSepKBhk_UvLYrNebSht-giX"

#define TEMP_LOW_LIMIT 25.0

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

//====================================================
// PIN CONFIGURATION
//====================================================


#define BUZZER_PIN 25


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

} SensorData;

SensorData receiveData;

//====================================================
// GLOBAL
//====================================================

unsigned long lastPacketTime = 0;
unsigned long lastBuzzerToggle = 0;

bool buzzerState = false;

volatile bool newData = false;

char ssid[] = "SD B LT 1";
char pass[] = "M15ASUZZ";

//====================================================
// CALLBACK RECEIVE
//====================================================

void onReceive(
    const esp_now_recv_info_t *recv_info,
    const uint8_t *incomingData,
    int len)
{

    memcpy(&receiveData, incomingData, sizeof(receiveData));

    // Blynk.virtualWrite(V0, receiveData.lpg);
    // Blynk.virtualWrite(V1, receiveData.co);
    // Blynk.virtualWrite(V2, receiveData.temp);

    // Blynk.virtualWrite(V3, receiveData.gasAlarm);

    // Blynk.virtualWrite(V4, receiveData.fanStatus);

    // Blynk.virtualWrite(V5, receiveData.valveStatus);

    // Blynk.virtualWrite(V6, receiveData.packetNumber);

    // Blynk.virtualWrite(V7, "ONLINE");

    lastPacketTime = millis();

    newData = true;

    Serial.println();
    Serial.println("====================================");
    Serial.println("DATA RECEIVED");
    Serial.println("====================================");

    Serial.print("Sender : ");

    for (int i = 0; i < 6; i++)
    {
        if (recv_info->src_addr[i] < 16)
            Serial.print("0");

        Serial.print(recv_info->src_addr[i], HEX);

        if (i < 5)
            Serial.print(":");
    }

    Serial.println();

    Serial.print("Packet : ");
    Serial.println(receiveData.packetNumber);

    Serial.print("LPG : ");
    Serial.print(receiveData.lpg);
    Serial.println(" ppm");

    Serial.print("CO : ");
    Serial.print(receiveData.co);
    Serial.println(" ppm");

    Serial.print("Temperature : ");
    Serial.print(receiveData.temp);
    Serial.println(" C");

    Serial.print("Gas Alarm : ");
    Serial.println(receiveData.gasAlarm ? "YES" : "NO");

    Serial.print("Fan : ");
    Serial.println(receiveData.fanStatus ? "ON" : "OFF");

    Serial.print("Valve : ");
    Serial.println(receiveData.valveStatus ? "CLOSED" : "OPEN");

    Serial.println("====================================");
}

//====================================================
// SETUP
//====================================================

void setup()
{

    Serial.begin(115200);

    delay(1000);

    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    WiFi.mode(WIFI_STA);

    WiFi.begin(ssid, pass);

    Serial.print("Connecting WiFi");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi Connected");

    uint8_t wifiChannel = WiFi.channel();

    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(wifiChannel, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);

    Serial.print("WiFi Channel : ");
    Serial.println(wifiChannel);

    Blynk.config(BLYNK_AUTH_TOKEN);

    Blynk.connect();

    Serial.println();
    Serial.println("====================================");
    Serial.println("ESP32 RECEIVER");
    Serial.println("====================================");

    Serial.print("Receiver MAC : ");
    Serial.println(WiFi.macAddress());

    if (esp_now_init() != ESP_OK)
    {
        Serial.println("ESP-NOW INIT FAILED");

        while (true)
        {
            delay(100);
        }
    }

    esp_now_register_recv_cb(onReceive);

    lastPacketTime = millis();

    Serial.println("Waiting data...");
}

//====================================================
// LOOP
//====================================================

void loop()
{
    Blynk.run();

    unsigned long now = millis();

    if(newData)
    {
        newData = false;

        Blynk.virtualWrite(V0, receiveData.lpg);
        Blynk.virtualWrite(V1, receiveData.co);
        Blynk.virtualWrite(V2, receiveData.temp);
        Blynk.virtualWrite(V3, receiveData.gasAlarm);
        Blynk.virtualWrite(V4, receiveData.fanStatus);
        Blynk.virtualWrite(V5, receiveData.valveStatus ? 0 : 1);
        Blynk.virtualWrite(V6, receiveData.packetNumber);
        Blynk.virtualWrite(V7, "ONLINE");
    }
    //------------------------------------
    // GAS ALARM (Prioritas 1)
    //------------------------------------

    if(receiveData.gasAlarm)
    {
        if(now - lastBuzzerToggle >= 250)
        {
            lastBuzzerToggle = now;
            buzzerState = !buzzerState;
            digitalWrite(BUZZER_PIN, buzzerState);
        }
    }

    //------------------------------------
    // LOW TEMPERATURE (Prioritas 2)
    //------------------------------------

    else if(receiveData.temp < TEMP_LOW_LIMIT)
    {
        digitalWrite(BUZZER_PIN, HIGH);

        buzzerState = true;

        Serial.println("WARNING : KANDANG DINGIN");
    }

    //------------------------------------
    // COMMUNICATION LOST (Prioritas 3)
    //------------------------------------

    else if(now - lastPacketTime > 10000)
    {
        Blynk.virtualWrite(V7, "OFFLINE");

        if(now - lastBuzzerToggle >= 1000)
        {
            lastBuzzerToggle = now;

            buzzerState = !buzzerState;

            digitalWrite(BUZZER_PIN, buzzerState);
        }
    }

    //------------------------------------
    // NORMAL
    //------------------------------------

    else
    {
        buzzerState = false;

        digitalWrite(BUZZER_PIN, LOW);

        Blynk.virtualWrite(V7, "ONLINE");
    }
}