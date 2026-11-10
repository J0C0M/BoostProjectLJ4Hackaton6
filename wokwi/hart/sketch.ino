/*
 * Smèrrig-hart – prototype voor Hackaton 6 (Niet-zo-single-feestje)
 * Ontwerp: docs/ontwerpdocument.md
 *
 * Bediening in Wokwi:
 *  - STATUS-knop (groen, toets S): single -> bezet -> ingewikkeld
 *  - Hart-tik: klik op de MFRC522, kies een kaart en klik op "Tap"
 *      groene kaart = hart A, gele kaart = hart B, rode kaart = hart C
 *  - LIKE-knop (rood, toets L): binnen 5 s na een tik een like sturen
 *  - Liken twee harten elkaar, dan is het een MATCH (melodie + regenboog)
 *
 * Hart B maken: kopie van dit project, alleen MIJN_ID aanpassen naar 'B'.
 */
#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

// ===================== Per hart aanpassen =====================
const char MIJN_ID = 'A';  // 'A' voor hart A, 'B' voor hart B

// ===================== Netwerk =====================
const char* WIFI_SSID = "Wokwi-GUEST";
const char* BROKER    = "broker.hivemq.com";  // alternatief: "test.mosquitto.org"
const char* PREFIX    = "smerrig013/BoostProject/hart/";  // topics: <PREFIX><id>/status|like|match

// ===================== Pinnen (pinplan §6.2) =====================
const int PIN_RFID_SS     = 5;
const int PIN_RFID_RST    = 4;
const int PIN_RING        = 13;
const int PIN_BUZZER      = 25;
const int PIN_KNOP_STATUS = 32;
const int PIN_KNOP_LIKE   = 33;
const int AANTAL_LEDS     = 12;

// ===================== Harten en kaarten =====================
const int  AANTAL_HARTEN = 3;
const char HARTEN[AANTAL_HARTEN]          = { 'A', 'B', 'C' };
const char* KAART_UID[AANTAL_HARTEN]      = { "11223344", "55667788", "AABBCCDD" };  // groen, geel, rood

// ===================== Statussen =====================
const int   AANTAL_STATUSSEN = 3;
const char* STATUS_NAAM[AANTAL_STATUSSEN] = { "single", "bezet", "ingewikkeld" };
const uint8_t STATUS_RGB[AANTAL_STATUSSEN][3] = { { 0, 255, 0 }, { 255, 0, 0 }, { 255, 170, 0 } };

const unsigned long TIK_GELDIG_MS = 5000;

// ===================== Types (bovenaan, zodat de Arduino-compiler ze kent) =====================
struct Noot { int freq; int duur; };                           // freq 0 = stilte, -1 = einde
struct Knop { int pin; bool ingedrukt; unsigned long sinds; };
enum Modus { M_IDLE, M_GETIKT, M_MELDING, M_LIKE_ONTVANGEN, M_MATCH };

// ===================== Hardware =====================
MFRC522 rfid(PIN_RFID_SS, PIN_RFID_RST);
Adafruit_SSD1306 scherm(128, 64, &Wire, -1);
Adafruit_NeoPixel ring(AANTAL_LEDS, PIN_RING, NEO_GRB + NEO_KHZ800);
WiFiClient netClient;
PubSubClient mqtt(netClient);

// ===================== Toestand =====================
Modus modus = M_IDLE;
unsigned long modusTot = 0;

int  mijnStatus = 0;
int  statusVan[AANTAL_HARTEN]   = { -1, -1, -1 };  // laatst bekende status van andere harten
bool ikLike[AANTAL_HARTEN]      = { false };       // wie heb ik geliket
bool likeVan[AANTAL_HARTEN]     = { false };       // van wie kreeg ik een like
char getiktHart = 0;

String regel1, regel2;
bool schermVuil = true;
unsigned long laatsteTik = 0, laatsteMqttPoging = 0, laatsteLed = 0, laatsteScherm = 0;

// ===================== Hulpfuncties =====================
int hartIndex(char id) {
  for (int i = 0; i < AANTAL_HARTEN; i++)
    if (HARTEN[i] == id) return i;
  return -1;
}

String topicVoor(char id, const char* soort) {
  return String(PREFIX) + id + "/" + soort;
}

void melding(const String& a, const String& b) {
  regel1 = a;
  regel2 = b;
  schermVuil = true;
}

void startModus(Modus m, unsigned long duurMs) {
  modus = m;
  modusTot = millis() + duurMs;
  schermVuil = true;
}

// ===================== Geluid (niet-blokkerend) =====================
const Noot MELODIE_KORT[]  = { { 1319, 60 }, { -1, 0 } };
const Noot MELODIE_LIKE[]  = { { 880, 80 }, { 0, 60 }, { 1175, 140 }, { -1, 0 } };
const Noot MELODIE_MATCH[] = { { 523, 120 }, { 659, 120 }, { 784, 120 }, { 1047, 280 }, { 0, 80 },
                               { 784, 120 }, { 1047, 450 }, { -1, 0 } };
const Noot* melodie = nullptr;
int nootIndex = 0;
unsigned long nootTot = 0;

void speel(const Noot* m) {
  melodie = m;
  nootIndex = 0;
  nootTot = 0;
}

void updateGeluid() {
  if (!melodie || millis() < nootTot) return;
  const Noot& n = melodie[nootIndex++];
  if (n.freq < 0) {
    noTone(PIN_BUZZER);
    melodie = nullptr;
    return;
  }
  if (n.freq == 0) noTone(PIN_BUZZER);
  else tone(PIN_BUZZER, n.freq);
  nootTot = millis() + n.duur;
}

// ===================== MQTT =====================
void publiceerStatus() {
  if (!mqtt.connected()) return;
  mqtt.publish(topicVoor(MIJN_ID, "status").c_str(), STATUS_NAAM[mijnStatus], true);  // retained
}

void startMatch(char met) {
  melding("MATCH!  <3", String("met hart ") + met);
  startModus(M_MATCH, 6000);
  speel(MELODIE_MATCH);
  Serial.printf("MATCH met hart %c\n", met);
}

void onBericht(char* topic, byte* data, unsigned int len) {
  String t = topic;
  String p;
  for (unsigned int i = 0; i < len; i++) p += (char)data[i];
  if (!t.startsWith(PREFIX)) return;
  t = t.substring(strlen(PREFIX));  // bv. "B/status"
  if (t.length() < 3) return;

  char over = t[0];
  String soort = t.substring(2);

  if (soort == "status") {
    int i = hartIndex(over);
    if (i < 0) return;
    for (int s = 0; s < AANTAL_STATUSSEN; s++)
      if (p == STATUS_NAAM[s]) statusVan[i] = s;
    return;
  }

  if (over != MIJN_ID || p.length() == 0) return;  // like/match voor een ander hart
  char van = p[0];
  int i = hartIndex(van);
  if (i < 0) return;

  if (soort == "like") {
    likeVan[i] = true;
    melding("Iemand vindt je", "leuk!  <3");  // anoniem: we laten niet zien wie
    startModus(M_LIKE_ONTVANGEN, 2500);
    speel(MELODIE_LIKE);
    Serial.println("Like ontvangen (anoniem)");
  } else if (soort == "match") {
    startMatch(van);
  }
}

void updateNetwerk() {
  if (WiFi.status() != WL_CONNECTED) return;  // WiFi verbindt op de achtergrond
  if (mqtt.connected()) {
    mqtt.loop();
    return;
  }
  if (millis() - laatsteMqttPoging < 3000) return;
  laatsteMqttPoging = millis();

  String clientId = String("smerrig-") + MIJN_ID + "-" + String(random(0xffff), HEX);
  Serial.print("MQTT verbinden... ");
  if (mqtt.connect(clientId.c_str())) {
    Serial.println("OK");
    mqtt.subscribe((String(PREFIX) + "+/status").c_str());
    mqtt.subscribe(topicVoor(MIJN_ID, "like").c_str());
    mqtt.subscribe(topicVoor(MIJN_ID, "match").c_str());
    publiceerStatus();
    schermVuil = true;
  } else {
    Serial.printf("mislukt (code %d)\n", mqtt.state());
  }
}

// ===================== Acties =====================
void volgendeStatus() {
  mijnStatus = (mijnStatus + 1) % AANTAL_STATUSSEN;
  publiceerStatus();
  melding("Status:", STATUS_NAAM[mijnStatus]);
  startModus(M_MELDING, 1500);
  speel(MELODIE_KORT);
  Serial.printf("Status: %s\n", STATUS_NAAM[mijnStatus]);
}

void verwerkTik(const String& uid) {
  Serial.println("Kaart getikt: " + uid);
  int i = -1;
  for (int k = 0; k < AANTAL_HARTEN; k++)
    if (uid == KAART_UID[k]) i = k;

  if (i < 0) {
    melding("Onbekend hart", uid);
    startModus(M_MELDING, 2000);
    return;
  }
  char id = HARTEN[i];
  if (id == MIJN_ID) {
    melding("Dit is je", "eigen hart :)");
    startModus(M_MELDING, 2000);
    return;
  }

  getiktHart = id;
  String s = statusVan[i] >= 0 ? STATUS_NAAM[statusVan[i]] : "?";
  melding(String("Hart ") + id + ": " + s, "LIKE = like sturen");
  startModus(M_GETIKT, TIK_GELDIG_MS);
  speel(MELODIE_KORT);
}

void verwerkLike() {
  if (modus != M_GETIKT) {
    melding("Tik eerst een", "hart aan");
    startModus(M_MELDING, 1500);
    return;
  }
  if (!mqtt.connected()) {
    melding("Offline:", "like niet verstuurd");
    startModus(M_MELDING, 2000);
    return;
  }
  int i = hartIndex(getiktHart);
  mqtt.publish(topicVoor(getiktHart, "like").c_str(), String(MIJN_ID).c_str());
  ikLike[i] = true;
  Serial.printf("Like verstuurd naar hart %c\n", getiktHart);

  if (likeVan[i]) {  // wederzijds: match!
    mqtt.publish(topicVoor(getiktHart, "match").c_str(), String(MIJN_ID).c_str());
    startMatch(getiktHart);
  } else {
    melding("Like verstuurd", String("naar hart ") + getiktHart);
    startModus(M_MELDING, 2000);
    speel(MELODIE_KORT);
  }
}

// ===================== Invoer =====================
Knop knopStatus = { PIN_KNOP_STATUS, false, 0 };
Knop knopLike   = { PIN_KNOP_LIKE, false, 0 };

bool nieuwIngedrukt(Knop& k) {
  bool nu = digitalRead(k.pin) == LOW;
  if (nu != k.ingedrukt && millis() - k.sinds > 40) {
    k.ingedrukt = nu;
    k.sinds = millis();
    return nu;
  }
  return false;
}

void leesKaart() {
  if (millis() - laatsteTik < 800) return;
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;
  String uid;
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(rfid.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();
  rfid.PICC_HaltA();
  laatsteTik = millis();
  verwerkTik(uid);
}

// ===================== Uitvoer =====================
uint32_t kleurVoorStatus(int s, float helderheid) {
  if (s < 0) return ring.Color(80 * helderheid, 80 * helderheid, 80 * helderheid);
  return ring.Color(STATUS_RGB[s][0] * helderheid, STATUS_RGB[s][1] * helderheid, STATUS_RGB[s][2] * helderheid);
}

void updateLeds() {
  if (millis() - laatsteLed < 25) return;
  laatsteLed = millis();
  unsigned long t = millis();

  switch (modus) {
    case M_IDLE:
    case M_MELDING: {  // rustig "ademen" in de eigen statuskleur
      float h = 0.35 + 0.65 * (sin(t / 600.0) + 1) / 2;
      ring.fill(kleurVoorStatus(mijnStatus, h));
      break;
    }
    case M_GETIKT: {  // statuskleur van het getikte hart met een rondlopend wit lichtje
      int i = hartIndex(getiktHart);
      ring.fill(kleurVoorStatus(statusVan[i], 0.6));
      ring.setPixelColor((t / 80) % AANTAL_LEDS, ring.Color(255, 255, 255));
      break;
    }
    case M_LIKE_ONTVANGEN:  // roze knipperen
      ring.fill((t / 150) % 2 ? ring.Color(255, 20, 147) : 0);
      break;
    case M_MATCH:  // draaiende regenboog
      for (int i = 0; i < AANTAL_LEDS; i++)
        ring.setPixelColor(i, ring.gamma32(ring.ColorHSV(t * 40 + i * 65536L / AANTAL_LEDS)));
      break;
  }
  ring.show();
}

void updateScherm() {
  if (!schermVuil && millis() - laatsteScherm < 1000) return;
  schermVuil = false;
  laatsteScherm = millis();

  scherm.clearDisplay();
  scherm.setTextColor(SSD1306_WHITE);
  scherm.setTextSize(1);
  scherm.setCursor(0, 0);
  scherm.printf("Hart %c  %s", MIJN_ID, STATUS_NAAM[mijnStatus]);
  scherm.setCursor(0, 10);
  if (WiFi.status() != WL_CONNECTED) scherm.print("WiFi verbinden...");
  else if (!mqtt.connected()) scherm.print("offline");
  else scherm.print("online");
  scherm.drawLine(0, 20, 127, 20, SSD1306_WHITE);

  scherm.setCursor(0, 28);
  scherm.println(regel1);
  scherm.setCursor(0, 42);
  scherm.println(regel2);
  scherm.display();
}

// ===================== Setup & loop =====================
void setup() {
  Serial.begin(115200);
  Serial.printf("Smerrig-hart %c start\n", MIJN_ID);

  pinMode(PIN_KNOP_STATUS, INPUT_PULLUP);
  pinMode(PIN_KNOP_LIKE, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);

  ring.begin();
  ring.setBrightness(80);
  ring.show();

  if (!scherm.begin(SSD1306_SWITCHCAPVCC, 0x3C)) Serial.println("OLED niet gevonden");

  SPI.begin();
  rfid.PCD_Init();

  WiFi.begin(WIFI_SSID, "", 6);
  mqtt.setServer(BROKER, 1883);
  mqtt.setCallback(onBericht);

  melding("Tik een hart aan", "om te beginnen");
}

void loop() {
  updateNetwerk();

  if (nieuwIngedrukt(knopStatus)) volgendeStatus();
  if (nieuwIngedrukt(knopLike)) verwerkLike();
  leesKaart();

  if (modus != M_IDLE && millis() > modusTot) {
    modus = M_IDLE;
    melding("Tik een hart aan", "om te beginnen");
  }

  updateLeds();
  updateGeluid();
  updateScherm();
  delay(5);
}
