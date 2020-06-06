// Namespace = meineklassen
// extraKlasse = MyWlan
// Instance = MyWlanInstance
// Methode = Verbinden
/*
    GlobalVariables.cpp Datei
*/
#include "meineklassen.h"        /* Muss natürlich includet werden */
#include "meinsetup.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
WiFiClient wiFiClient;
#include <WiFiUdp.h>
#include <NTPClient.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 7200, 60000);

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif
#define PIN D5
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, 5, 1, PIN,
                            NEO_TILE_ROWS   + NEO_TILE_BOTTOM   + NEO_TILE_LEFT   + NEO_TILE_PROGRESSIVE +
                            NEO_MATRIX_LEFT + NEO_MATRIX_BOTTOM + NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
                            NEO_GRB + NEO_KHZ800);

const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255), matrix.Color(255, 255, 0), matrix.Color(0, 255, 255), matrix.Color(255, 0, 255), matrix.Color(255, 255, 255)
};



namespace meineklassen
{
//    void ExampleMethode(String Var)     /* Die Methode direkt von Global */
//{
/* .......... */
//}

void meineklassen::MyWlan::Verbinden() {
  Serial.println("Hallo Welt");
  WiFi.mode(WIFI_STA);
  Serial.println("Scan starten ...   ");
  int n = WiFi.scanNetworks();                //WiFi.scanNetworks gibt die Anzahl gefundene AP zurück
  Serial.println("Scan beendet");
  if (n == 0)
    Serial.println("keine Netzwerke gefunden");
  else
  {
    Serial.print(n);
    Serial.println("Netzwerke gefunden");
  }
  for (int i = 0; i < n; ++i)        //SSID und RSSI fuer jedes gefundene Netzwerk ausgeben
  {
    //Serial.print(WiFi.SSID(i));
    //Serial.print(" - ");
    //Serial.print(WiFi.RSSI(i));
    //Serial.print(" - ");
    //Serial.println(WiFi.encryptionType(i));
  }

  WiFi.begin(ssid, password);
  Serial.print("WLAN:");
  Serial.println( ssid );
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 1000 );
    Serial.print ( "." );
  }
  Serial.print("IP: ");
  Serial.println( WiFi.localIP() );
  Serial.println("WIFI connected");
}
void meineklassen::MyNTP::Initiieren() {
  // set up timeClient
  timeClient.begin();
  Serial.println("NTP started");
  delay ( 2000 );
  timeClient.update();
  delay ( 3000 );
  Serial.print("Neue Uhrzeit: ");
  Serial.println(timeClient.getFormattedTime());
}
void meineklassen::MyNTP::Updaten() {
  timeClient.update();
  //Serial.println(timeClient.getFormattedTime() + "               ");
}
void meineklassen::MyLED::Zeitanzeige() {
  matrix.fillScreen(0);
  matrix.setTextColor(colors[0]);
  String zeit = timeClient.getFormattedTime();
  matrix.setCursor(0, 0);
  matrix.print(zeit.substring(0, 2));
  matrix.setCursor(14, 0);
  matrix.print(zeit.substring(3, 5));
  matrix.setCursor(28, 0);
  matrix.print(zeit.substring(6, 8));
  matrix.setCursor(10, 0);
  matrix.print(":");
  matrix.setCursor(24, 0);
  matrix.print(":");
  matrix.show();
}

void meineklassen::My::ChatAusgabe(int textfarbe, String text, int helligkeit, String username, String message) {
  Serial.println(textfarbe);
  Serial.println(text);
  Serial.println(helligkeit);
  Serial.println(username);
  Serial.println(message);
}
int meineklassen::My::Chataddieren(int zahl) {
  Serial.print("vor berechnung: ");
  Serial.println(zahl);
  ++zahl;
  meineklassen::MyInstance.doppelte = zahl * 2;
  Serial.print("nach berechnung: ");
  Serial.println(zahl);
  return zahl;
}
char* meineklassen::My::string2char(String command) {
  if (command.length() != 0) {
    char *p = const_cast<char*>(command.c_str());
    return p;
  }
}
void meineklassen::MyVote::Aktivieren() {
  meineklassen::MyVoteInstance.Resetten();
  meineklassen::MyVoteInstance.voteactive = 1;
  Serial.println(meineklassen::MyVoteInstance.voteactive);
}
void meineklassen::MyVote::Resetten() {
  meineklassen::MyVoteInstance.vote[0] = 0;
  meineklassen::MyVoteInstance.vote[1] = 0;
  meineklassen::MyVoteInstance.vote[2] = 0;
  meineklassen::MyVoteInstance.vote[3] = 0;
  meineklassen::MyVoteInstance.voteactive = 0;
  meineklassen::MyVoteInstance.useridvote = "";
  Serial.println(meineklassen::MyVoteInstance.voteactive);
}
void meineklassen::MyVote::Voten(int voting, String displayname) {
  String subtext = meineklassen::MyVoteInstance.useridvote;
  int restvon = 0;
  int restbis = subtext.length();
  int bereitsgevotet = 0;
  if ( restbis - restvon >= 1 ) {
    do {
      if ( subtext.substring(subtext.indexOf('<') + 1, subtext.indexOf('>')) == displayname ) {
        bereitsgevotet = 1;
      }
      restvon = subtext.indexOf('>') + 1;
      restbis = subtext.length();
      if ( restbis - restvon == 0 ) {
        break;
      }
      subtext = subtext.substring(restvon, restbis);
    } while (restbis - restvon >= 1);
  }
  if (bereitsgevotet == 0) {
    meineklassen::MyVoteInstance.useridvote = meineklassen::MyVoteInstance.useridvote + "<" + displayname + ">";
    ++meineklassen::MyVoteInstance.vote[voting];
    ++meineklassen::MyVoteInstance.vote[0];
    String ausgabe = String(meineklassen::MyVoteInstance.vote[0]) + " Votes";
    Serial.println(ausgabe);
//        meineklassen::MyLEDInstance.LEDText(2, ausgabe, 100, 1, 1000);

  } else {
    Serial.println("bereitsgevotet");
  }
  //  Serial.println(meineklassen::MyVoteInstance.useridvote);
}
void meineklassen::MyVote::Auswerten() {
  Serial.print(" 1=");
  Serial.print(meineklassen::MyVoteInstance.vote[1]);
  Serial.print(" 2=");
  Serial.print(meineklassen::MyVoteInstance.vote[2]);
  Serial.print(" 3=");
  Serial.print(meineklassen::MyVoteInstance.vote[3]);
  Serial.print(" Gesamt=");
  Serial.println(meineklassen::MyVoteInstance.vote[0]);

  Serial.print(" 1=");
  Serial.print(100 / meineklassen::MyVoteInstance.vote[0] * meineklassen::MyVoteInstance.vote[1]);
  Serial.print("% 2=");
  Serial.print(100 / meineklassen::MyVoteInstance.vote[0] * meineklassen::MyVoteInstance.vote[2]);
  Serial.print("% 3=");
  Serial.print(100 / meineklassen::MyVoteInstance.vote[0] * meineklassen::MyVoteInstance.vote[3]);
  Serial.println("%");
  meineklassen::MyLEDInstance.LEDVoting(0, meineklassen::MyVoteInstance.vote, 100, 1, 2000);
}
void meineklassen::MyGiveaway::Aktivieren(String text) {
  meineklassen::MyGiveawayInstance.Resetten();
  meineklassen::MyGiveawayInstance.gaactive = 1;
  meineklassen::MyGiveawayInstance.giveawayname = "!" + text.substring(16, text.length() - 1);

  Serial.println(meineklassen::MyGiveawayInstance.gaactive);
  Serial.println(meineklassen::MyGiveawayInstance.giveawayname);
}
void meineklassen::MyGiveaway::Stoppen() {
  meineklassen::MyGiveawayInstance.gaactive = 2;
}
String meineklassen::MyGiveaway::Auswerten() {
  randomSeed(analogRead(0));
  int anzteilnehmer = meineklassen::MyGiveawayInstance.ga[0] - 1;
  String  teilnehmender[99] = {};
  String subtext = meineklassen::MyGiveawayInstance.userids;
  int restvon = 0;
  int restbis = subtext.length();
  int teilnehmer = 0;
  if ( restbis - restvon >= 1 ) {
    do {
      Serial.println(teilnehmer);
      Serial.println(subtext.substring(subtext.indexOf('<') + 1, subtext.indexOf('>')));
      teilnehmender[teilnehmer] = String(subtext.substring(subtext.indexOf('<') + 1, subtext.indexOf('>')));
      teilnehmer++;
      restvon = subtext.indexOf('>') + 1;
      restbis = subtext.length();
      if ( restbis - restvon == 0 ) {
        break;
      }
      subtext = subtext.substring(restvon, restbis);
    } while (restbis - restvon >= 1);
  }

  int gewinnercnt = random(teilnehmer);
  //int gewinnercnt = random(1, 20);
  Serial.print("Teilnehmer: ");
  Serial.println(teilnehmer);
  Serial.print("Gewinner: ");
  Serial.println(teilnehmender[gewinnercnt]);

  for (int i = 0; i < 40; ++i) {
    int gewinnercnt = random(teilnehmer);
    Serial.print(i);
    Serial.print(". Auslosung: ");
    Serial.println(teilnehmender[gewinnercnt]);
    meineklassen::MyLEDInstance.LEDText(0, teilnehmender[gewinnercnt], 20, 1, 30);
    meineklassen::MyLEDInstance.LEDBalken(1, i, 20, 1, 30);

    int d = (i * i) ;
    delay ( d );
  }
  Serial.print("Gewinner: ");
  Serial.println(teilnehmender[gewinnercnt]);


  meineklassen::MyGiveawayInstance.gaactive  = 2;
  String gewinner = teilnehmender[gewinnercnt];
  return gewinner;
}
void meineklassen::MyGiveaway::Resetten() {
  meineklassen::MyGiveawayInstance.ga[0] = 0;
  meineklassen::MyGiveawayInstance.gaactive = 0;
  meineklassen::MyGiveawayInstance.userids = "";

  Serial.println(meineklassen::MyGiveawayInstance.gaactive);
}
void meineklassen::MyGiveaway::Teilnehmen(String displayname) {
  String subtext = meineklassen::MyGiveawayInstance.userids;
  int restvon = 0;
  int restbis = subtext.length();
  int bereitsteilgenommen = 0;
  if ( restbis - restvon >= 1 ) {
    do {
      if ( subtext.substring(subtext.indexOf('<') + 1, subtext.indexOf('>')) == displayname ) {
        bereitsteilgenommen = 1;
      }
      restvon = subtext.indexOf('>') + 1;
      restbis = subtext.length();
      if ( restbis - restvon == 0 ) {
        break;
      }
      subtext = subtext.substring(restvon, restbis);
    } while (restbis - restvon >= 1);
  }
  if (bereitsteilgenommen == 0) {
    meineklassen::MyGiveawayInstance.userids = meineklassen::MyGiveawayInstance.userids + "<" + displayname + ">";
    ++meineklassen::MyGiveawayInstance.ga[0];
    String ausgabe = String(meineklassen::MyGiveawayInstance.ga[0]) + " User";
    Serial.println(ausgabe);
    meineklassen::MyLEDInstance.LEDText(0, ausgabe, 20, 1, 1000);

  } else {
    Serial.println("bereitsteilgenommen");
  }
  Serial.println(meineklassen::MyGiveawayInstance.userids);
}
void meineklassen::MyLED::LEDLauftext(int farbe, String text, int helligkeit, int  xtimes, int del) {
  Serial.println("Ausgabe auf LEDs");
  Serial.println(farbe);
  Serial.println(colors[farbe]);
  matrix.setTextColor(colors[farbe]);
  int breite = text.length() * 6 + matrix.width();
  for (int y = xtimes; y >= 1; y--) {
    for (int x = breite; x >= 0; x--) {
      int pos = x - text.length() * 6;
      matrix.fillScreen(0);
      matrix.setCursor(pos, 0);
      matrix.print(text);
      matrix.show();
      delay(del);
    }
  }
}
void meineklassen::MyLED::LEDBalken(int farbe, int anzahl, int helligkeit, int  xtimes, int del) {
  matrix.setTextColor(colors[farbe]);
  for (int y = 0; y <= anzahl; y++) {
    matrix.setPixelColor((y * 8), 200, 0, 0 );
  }
  matrix.show();
  delay(del);
}
void meineklassen::MyLED::LEDTextblinken(int farbe, String text, int helligkeit, int  xtimes, int del) {
  matrix.setTextColor(colors[farbe]);
  for (int y = xtimes; y >= 1; y--) {
    matrix.setBrightness(helligkeit);
    matrix.fillScreen(0);
    matrix.setCursor(0, 0);
    matrix.print(text);
    matrix.show();
    delay(del);
    matrix.fillScreen(0);
    matrix.show();
    delay(del);
  }
  matrix.setBrightness(30);
}
void meineklassen::MyLED::LEDText(int farbe, String text, int helligkeit, int  xtimes, int del) {
  matrix.setTextColor(colors[farbe]);
  for (int y = xtimes; y >= 1; y--) {
    matrix.setBrightness(helligkeit);
    matrix.fillScreen(0);
    matrix.setCursor(0, 0);
    matrix.print(text);
    matrix.show();
    delay(del);
  }
  matrix.setBrightness(30);
}
void meineklassen::MyLED::LEDVoting(int farbe, int* votes, int helligkeit, int  xtimes, int del) {
  if (votes[0] >= 1) {
    int proz1 = 10000 / votes[0] * votes[1] / 100 ;
    int proz2 = 10000 / votes[0] * votes[2] / 100 ;
    int proz3 = 10000 / votes[0] * votes[3] / 100 ;
    int pix1 = proz1 * 3.2;
    int pix2 = proz2 * 3.2;
    int pix3 = proz3 * 3.2;
    int pix12 = pix1 + pix2;
    int pix123 = pix1 + pix2 + pix3;
    matrix.setBrightness(helligkeit);
    matrix.fillScreen(0);
    matrix.setTextColor(colors[0]);
    matrix.setCursor(0, 0);
    matrix.print(votes[1]);
    matrix.setTextColor(colors[2]);
    matrix.setCursor(14, 0);
    matrix.print(votes[2]);
    matrix.setTextColor(colors[1]);
    matrix.setCursor(28, 0);
    matrix.print(votes[3]);
    matrix.show();
    delay(del);

    matrix.setBrightness(helligkeit);
    matrix.fillScreen(0);
    matrix.setTextColor(colors[0]);
    matrix.setCursor(0, 0);
    matrix.print(proz1);
    matrix.setTextColor(colors[2]);
    matrix.setCursor(14, 0);
    matrix.print(proz2);
    matrix.setTextColor(colors[1]);
    matrix.setCursor(28, 0);
    matrix.print(proz3);
    matrix.show();
    delay(del);



    for (int i = 0; i <= pix1; i++) {
      matrix.setPixelColor(i, 20, 0, 0);
    }
    for (int i = pix1; i <= pix12; i++) {
      matrix.setPixelColor(i, 0, 0, 20);
    }
    for (int i = pix12; i <= 320; i++) {
      matrix.setPixelColor(i, 0, 20, 0);
    }
    matrix.show();
    delay(del);
    matrix.setBrightness(30);
  }
}
void meineklassen::MyLED::Start() {
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(30);
  matrix.setTextColor(colors[0]);
}
MyWlan MyWlanInstance;           /* Für die Klassen instance */
MyNTP MyNTPInstance;
My MyInstance;
MyVote MyVoteInstance;
MyGiveaway MyGiveawayInstance;
MyLED MyLEDInstance;
}
