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

#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
MySQL_Connection conn(&wiFiClient);
MySQL_Cursor* cursor;
IPAddress server_addr(192,168,0,35);  // IP of the MySQL *server* here
char user[] = "esp";              // MySQL user login username
char passwordd[] = "password";        // MySQL user login password

#include <ESP8266WebServer.h>
ESP8266WebServer server(80);

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
    void meineklassen::MyDB::Verbinden() {
      Serial.print("Connecting to SQL...  ");
      if (conn.connect(server_addr, 3306, user, passwordd))
      Serial.println("OK - Datenbank verbunden");
      else
      Serial.println("FAILED.");
      // create MySQL cursor object
      cursor = new MySQL_Cursor(&conn);
    }
    int meineklassen::MyDB::Writelog(String typ, String fil, String way, unsigned int z) {
      int y = 0;
      int yy = 0;
      int x;
      int xx;
      if (conn.connected()) {
        row_values *row = NULL;
        MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
        sprintf(SELECT_SQL, "SELECT content FROM esp.%s WHERE name = '%s';", typ.c_str(), fil.c_str());
        Serial.println(SELECT_SQL);
        cur_mem->execute(SELECT_SQL);
        column_names *columns = cur_mem->get_columns();
        do {
          row = cur_mem->get_next_row();
          if (row != NULL) {
            yy = atol(row->values[0]);
          }
        } while (row != NULL);
        delete cur_mem;
        if (yy == 0) {
          Serial.println(F("insert"));
          sprintf(INSERT_SQL, "INSERT INTO esp.%s (name, content) VALUES ('%s', '%i');", typ.c_str(), fil.c_str(), z);
          Serial.println(INSERT_SQL);
          cursor->execute(INSERT_SQL);
          Serial.println(F("Datenbank verbunden, Eintrag wird erstellt"));
          xx = z;
        } else {
          if (way.equals("add")) {
            xx = yy + z;
          } else {
            xx = z;
          }
          sprintf(UPDATE_SQL, "UPDATE esp.%s SET content = '%i' WHERE name = '%s';", typ.c_str(), xx, fil.c_str());
          Serial.println(UPDATE_SQL);
          cursor->execute(UPDATE_SQL);
        }
      } else {
        Serial.println(F("Datenbank nicht verbunden"));
      }
      return xx;
    }
    String meineklassen::MyDB::List(String fil) {
        int yy = 0;
      if (conn.connected()) {
        row_values *row = NULL;
        long Anzahl = 0;
        String name;
/**        sprintf(COUNT_SQL, "SELECT COUNT(name) AS Anzahl FROM esp.%s;", fil.c_str());
        Serial.println(COUNT_SQL);
          MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
          cur_mem->execute(COUNT_SQL);
          // Fetch the columns (required) but we don't use them.
          column_names *columns = cur_mem->get_columns();
          // Read the row (we are only expecting the one)
          do {
            row = cur_mem->get_next_row();
            if (row != NULL) {
              Anzahl = atol(row->values[0]);
            }
          } while (row != NULL);
          // Deleting the cursor also frees up memory used
          delete cur_mem;
          // Show the result
        Serial.println("Anzahl Chatter");
        Serial.println(Anzahl);
**/
        sprintf(COUNT_SQL, "SELECT * FROM esp.%s;", fil.c_str());
        Serial.println(COUNT_SQL);
          MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
          cur_mem->execute(COUNT_SQL);
          // Fetch the columns (required) but we don't use them.
          column_names *columns = cur_mem->get_columns();
          // Read the row (we are only expecting the one)
          do {
            row = cur_mem->get_next_row();
            if (row != NULL) {
              name += String(row->values[1]) + "  (";
              name += String(row->values[2]) + ")<br />";
            }
          } while (row != NULL);
          // Deleting the cursor also frees up memory used
          delete cur_mem;
          // Show the result
          Serial.println(name);

          return name;
      }
    }
    void meineklassen::MyDB::Reset(String fil) {
      if (conn.connected()) {
        sprintf(TRUNC_SQL, "TRUNCATE TABLE esp.%s;", fil.c_str());
        Serial.println(TRUNC_SQL);
        cursor->execute(TRUNC_SQL);
        Serial.println("Chatter reset");
      }
    }
    void meineklassen::MyWebserver::Start() {
      server.on("/", MyWebserver::handleRoot);
//      server.onNotFound(handleNotFound);
      server.begin();
      Serial.println("HTTP server started");

    }
    void meineklassen::MyWebserver::hc() {
      server.handleClient();

    }
 void meineklassen::MyWebserver::handleRoot() {
  Serial.println(ESP.getFreeHeap());
  String message = "<!DOCTYPE html>\
<html>\
  <head>\
    <meta charset='utf-8' />\
    <style>\
    body {\
      width: 100%;\
      height: 100%;\
      background: #000;\
      overflow: hidden;\
    }\
    .fade {\
      position: relative;\
      width: 100%;\
      min-height: 60vh;\
      top: -25px;\
      background-image: linear-gradient(0deg, transparent, black 75%);\
      z-index: 1;\
    }\
    .star-wars {\
      display: flex;\
      justify-content: center;\
      position: relative;\
      height: 800px;\
      color: #feda4a;\
      font-family: 'Pathway Gothic One', sans-serif;\
      font-size: 300%;\
      font-weight: 300;\
      letter-spacing: 2px;\
      line-height: 150%;\
      perspective: 1000px;\
      text-align: justify;\
    }\
    .crawl {\
      position: relative;\
      top: 9999px;\
      transform-origin: 50% 100%;\
      animation: crawl 60s linear;\
    }\
    .crawl > .title {\
      font-size: 90%;\
      text-align: center;\
    }\
    .crawl > .title h1 {\
      margin: 0 0 100px;\
      text-transform: uppercase;\
    }\
    @keyframes crawl {\
      0% {\
        top: 0;\
        transform: rotateX(20deg)  translateZ(0);\
      }\
      100% { \
        top: -6000px;\
        transform: rotateX(25deg) translateZ(-2500px);\
      }\
    }\
    </style>\
  </head>\
  <body>\
  <div class='fade'></div>\
<section class='star-wars'>\
<div class='crawl'>\
<div class='title'>\
  <h1>Sub-Gifter</h1>\
</div><p>";
message += meineklassen::MyDBInstance.List("subgift");
message += "</p>      \
<div class='title'>\
  <h1>Subs</h1>\
</div><p>";
message += meineklassen::MyDBInstance.List("subs");
message += "</p>      \
<div class='title'>\
  <h1>Bits</h1>\
</div><p>";
message += meineklassen::MyDBInstance.List("bits");
message += "</p>      \
<div class='title'>\
  <h1>Chatter</h1>\
</div><p>";
message += meineklassen::MyDBInstance.List("chatter");
message += "</p>      \
</div>\
</section>\
  </body>\
</html>\
";

  //message += "Welt";
  Serial.println(ESP.getFreeHeap());
  server.send(200, "text/html; charset=UTF-8", message);
  Serial.println(ESP.getFreeHeap());
}
    MyWlan MyWlanInstance;           /* Für die Klassen instance */
    MyNTP MyNTPInstance;
    My MyInstance;
    MyDB MyDBInstance;
    MyVote MyVoteInstance;
    MyGiveaway MyGiveawayInstance;
    MyLED MyLEDInstance;
    MyWebserver MyWebserverInstance;
  }
