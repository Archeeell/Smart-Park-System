#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

#include <SPI.h>
#include <MFRC522.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <ESP32Servo.h>

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(
SCREEN_WIDTH,
SCREEN_HEIGHT,
&Wire,
-1);

// PIN CONFIGURATION

// OLED
#define OLED_SDA 21
#define OLED_SCL 22

// IR SENSOR
#define IR_A1 32
#define IR_A2 33

// LED
#define LED_GREEN 26
#define LED_RED   27

// SERVO
#define SERVO_PIN 25

// RC522
#define SS_PIN    5
#define RST_PIN   4


// WIFI
const char* WIFI_SSID = "vivo 1919";
const char* WIFI_PASSWORD = "87654321";


// TELEGRAM
#define BOT_TOKEN "8260361983:AAHWt-L-74tj1F2UFLU1bUPwYUHU2sV_kI4"
#define CHAT_ID   "6432900221"

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// RC522
MFRC522 rfid(SS_PIN, RST_PIN);

// SERVO
Servo gateServo;

// GLOBAL VARIABLE
bool slotA1Terisi = false;
bool slotA2Terisi = false;

int jumlahKosong = 0;
String slotTersedia = "-";
String namaUser = "";
String uidCard = "";

bool wifiConnected = false;
unsigned long lastWifiCheck = 0;
unsigned long lastDisplayUpdate = 0;
unsigned long lastTelegram = 0;
unsigned long lastScan = 0;
const unsigned long WIFI_INTERVAL = 10000;
const unsigned long DISPLAY_INTERVAL = 300;
const unsigned long TELEGRAM_INTERVAL = 3000;
const unsigned long SCAN_INTERVAL = 3000;

void setup()
{

  Serial.begin(115200);

  // PIN MODE
  pinMode(IR_A1, INPUT);
  pinMode(IR_A2, INPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  // OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  // BOOT SCREEN
  display.setCursor(15,15);
  display.println("SMART PARK");
  display.setCursor(30,30);
  display.println("SYSTEM");
  display.display();
  delay(2000);



  // SERVO
  gateServo.attach(SERVO_PIN);
  gateServo.write(0);

  // RC522
  SPI.begin();
  rfid.PCD_Init();

  // WIFI
  client.setInsecure();
  connectWiFi();

  // READY
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("SYSTEM READY");
  display.display();
  delay(1000);
}

void connectWiFi()
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Connecting WiFi...");
  display.display();

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  wifiConnected = true;

  Serial.println();
  Serial.println("WiFi Connected");
  Serial.println(WiFi.localIP());

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("WiFi Connected");
  display.display();

  delay(1000);
}

void reconnectWiFi()
{
  if (millis() - lastWifiCheck < WIFI_INTERVAL)
    return;

  lastWifiCheck = millis();

  if (WiFi.status() != WL_CONNECTED)
  {
    wifiConnected = false;

    Serial.println("Reconnect WiFi...");

    WiFi.disconnect();

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }

    wifiConnected = true;

    Serial.println("WiFi Reconnected");
  }
}

void updateParkingStatus()
{
  slotA1Terisi = digitalRead(IR_A1) == LOW;
  slotA2Terisi = digitalRead(IR_A2) == LOW;

  jumlahKosong = 0;
  slotTersedia = "-";

  if (!slotA1Terisi)
  {
    jumlahKosong++;
    slotTersedia = "A1";
  }

  if (!slotA2Terisi)
  {
    jumlahKosong++;

    if (slotTersedia == "-")
      slotTersedia = "A2";
  }

  digitalWrite(LED_GREEN, jumlahKosong > 0);
  digitalWrite(LED_RED, jumlahKosong == 0);
}

void displayHome()
{
  if (millis() - lastDisplayUpdate < DISPLAY_INTERVAL)
    return;

  lastDisplayUpdate = millis();

  display.clearDisplay();

  display.setTextSize(1);

  display.setCursor(0, 0);
  display.println("SMART PARK SYSTEM");

  display.print("A1 : ");
  display.println(slotA1Terisi ? "TERISI" : "KOSONG");

  display.print("A2 : ");
  display.println(slotA2Terisi ? "TERISI" : "KOSONG");

  display.print("Kosong : ");
  display.println(jumlahKosong);

  display.print("Lokasi : ");
  display.println(slotTersedia);

  display.display();
}

void displayWelcome()
{
  display.clearDisplay();
  display.setCursor(0, 5);
  display.println("WELCOME");
  display.println();
  display.println(namaUser);
  display.println();
  display.print("Slot : ");
  display.println(slotTersedia);
  display.display();
}

void displayFull()
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(5, 20);
  display.println("PENUH");
  display.setTextSize(1);
  display.display();
}

void displayAccessDenied()
{
  display.clearDisplay();
  display.setCursor(0, 5);
  display.println("NFC");
  display.println();
  display.println("AKSES DITOLAK");
  display.display();
}

void openGate()
{
  gateServo.write(90);
  delay(3000);
  gateServo.write(0);
}

void sendTelegram()
{
  if (millis() - lastTelegram < TELEGRAM_INTERVAL)
    return;

  lastTelegram = millis();

  String msg = "";
  msg += "SMART PARK SYSTEM\n\n";
  msg += "Halo ";
  msg += namaUser;
  msg += " 👋\n\n";
  msg += "Selamat datang.\n\n";
  msg += "Slot Parkir Anda : ";
  msg += slotTersedia;
  msg += "\n\n";
  msg += "Silakan menuju lokasi tersebut.\n";
  msg += "Gate telah dibuka.\n\n";
  msg += "Terima kasih.";

  bot.sendMessage(CHAT_ID, msg, "");
}

void sendFullTelegram()
{
  bot.sendMessage(
      CHAT_ID,
      "SMART PARK SYSTEM\n\nParkiran sedang penuh.\nSilakan menunggu.",
      "");
}

void sendDeniedTelegram()
{
  String msg = "";
  msg += "SMART PARK SYSTEM\n\n";
  msg += "Terdeteksi NFC tidak dikenal.\n";
  msg += "Akses ditolak.";

  bot.sendMessage(CHAT_ID, msg, "");
}

String getUserName(String uid)
{
  uid.replace(" ", "");

  uid.toUpperCase();

  // DAFTAR UID
  if(uid == "AF57B81F")
      return "Master";

  if(uid == "14F13EA3")
      return "Admin";

  if(uid == "D3431E1A")
      return "Archell";

  if(uid == "D34615E4")
      return "Adrian";

  return "";
}

String readUID()
{
  String uid = "";

  for(byte i = 0; i < rfid.uid.size; i++)
  {
      if(rfid.uid.uidByte[i] < 0x10)
          uid += "0";

      uid += String(rfid.uid.uidByte[i], HEX);
  }

  uid.toUpperCase();

  return uid;
}

void handleRFID()
{
  // Anti Double Scan
  if(millis() - lastScan < SCAN_INTERVAL)
      return;

  // Card Present ?
  if(!rfid.PICC_IsNewCardPresent())
      return;

  if(!rfid.PICC_ReadCardSerial())
      return;

  lastScan = millis();
  uidCard = readUID();

  Serial.println();
  Serial.print("UID : ");
  Serial.println(uidCard);

  namaUser = getUserName(uidCard);

  // UID Tidak Dikenal
  if(namaUser == "")
  {
      Serial.println("Access Denied");
      displayAccessDenied();
      sendDeniedTelegram();
      delay(2000);
      rfid.PICC_HaltA();
      return;
  }

  // Slot Penuh
  if(jumlahKosong == 0)
  {
      displayFull();
      sendFullTelegram();
      delay(2000);
      rfid.PICC_HaltA();
      return;
  }

  // Slot Tersedia
  displayWelcome();
  sendTelegram();
  openGate();
  rfid.PICC_HaltA();
}

void loop()
{
  reconnectWiFi();
  updateParkingStatus();
  displayHome();
  handleRFID();
}