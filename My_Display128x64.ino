// Adafruit_NeoMatrix example for tiled NeoPixel matrices.  Scrolls
// 'Howdy' across three 10x8 NeoPixel grids that were created using
// NeoPixel 60 LEDs per meter flex strip.

#include "meinsetup.h"
#include <twitch_irc_api.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
WiFiClient wiFiClient;
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti WiFiMulti;
#include <WiFiUdp.h>
#include <NTPClient.h>
//#include <ArduinoJson.h>
#include <SD.h>
#include <SPI.h>
#include <QList.h>
QList<String> myViewer;
QList<String> dateien;

#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
IPAddress server_addr(192,168,0,35);  // IP of the MySQL *server* here
char user[] = "esp";              // MySQL user login username
char passwordd[] = "password";        // MySQL user login password
static char INSERT_SQL[200];
static char SELECT_SQL[200];
static char UPDATE_SQL[200];
static char TRUNC_SQL[50];
File root;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 7200, 60000);

//#include <Arduino.h>
#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display
//U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather ESP8266/32u4 Boards + FeatherWing OLED


TwitchWebsocketIRC twitch_api;

//File root;
int DeletedCount = 0;
int FolderDeleteCount = 0;
int FailCount = 0;
String rootpath = "/";


MySQL_Connection conn(&wiFiClient);

MySQL_Cursor* cursor;




void setup() {

  // set up Serial
  Serial.begin(115200);
  Serial.println(F( "############################################################################" ));
  Serial.println(F( "############################################################################" ));
  Serial.println(F( "############################################################################" ));
  pinMode( D0, OUTPUT );



  u8g2.begin();
  u8g2.enableUTF8Print();    // enable UTF8 support for the Arduino print() function
  u8g2.setFontDirection(0);

  WiFi.mode(WIFI_STA);
  Serial.println(F("Scan starten ...   "));
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_unifont_te);
  u8g2.setCursor(0, 16);
  u8g2.print("WLAN");
  u8g2.setFont(u8g2_font_pcsenior_8f);
  u8g2.setCursor(0, 24);
  u8g2.print("Scan starten...");
  u8g2.setCursor(0, 40);
  u8g2.print("");
  u8g2.sendBuffer();

  int n = WiFi.scanNetworks();                //WiFi.scanNetworks gibt die Anzahl gefundene AP zurück
  Serial.println(F("Scan beendet"));
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_unifont_te);
  u8g2.setCursor(0, 16);
  u8g2.print("WLAN");
  u8g2.setFont(u8g2_font_pcsenior_8f);
  u8g2.setCursor(0, 24);
  u8g2.print("Scan beendet...");
  u8g2.setCursor(0, 40);
  u8g2.print("");
  u8g2.sendBuffer();
  if (n == 0) {
    Serial.println(F("keine Netzwerke gefunden"));
    u8g2.setCursor(0, 40);
    u8g2.print("keine Netzwerke gefunden");
    u8g2.sendBuffer();
  } else
  {
    Serial.print(n);
    Serial.println(F("Netzwerke gefunden"));
    u8g2.setCursor(0, 40);
    u8g2.print(n);
    u8g2.setCursor(24, 40);
    u8g2.print("Netze");
    u8g2.sendBuffer();
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
  Serial.println(F("WIFI connected"));
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_unifont_te);
  u8g2.setCursor(0, 16);
  u8g2.print("WLAN");
  u8g2.setFont(u8g2_font_pcsenior_8f);
  u8g2.setCursor(0, 24);
  u8g2.print(ssid);
  u8g2.setCursor(0, 40);
  u8g2.print("verbunden");
  u8g2.setCursor(0, 48);
  u8g2.print( WiFi.localIP());
  u8g2.sendBuffer();

  delay ( 3000 );
  // set up timeClient
  timeClient.begin();
  Serial.println(F("NTP started"));
  delay ( 2000 );
  timeClient.update();
  delay ( 4000 );
  Serial.print("Neue Uhrzeit: ");
  Serial.println(timeClient.getFormattedTime());
  //  lcd.setCursor(0, 1);
  //  lcd.print("NTP started    ");

  // Initialize SD library
  const int chipSelect = 15;
  while (!SD.begin(chipSelect)) {
    Serial.println(F("Failed to initialize SD library"));
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_unifont_te);
    u8g2.setCursor(0, 16);
    u8g2.print("SD CARD");
    u8g2.setCursor(0, 40);
    u8g2.print("Fehler beim laden");
    u8g2.sendBuffer();    delay(1000);
  }
  //root = SD.open("/chatter/");
  //printDirectory(root, 0);
  //Serial.println("done!");

  /**if (!SD.exists("chatter")) {
    Serial.println("Ordner existiert nicht");
    SD.mkdir("chatter");
  } else {
    Serial.println("Ordner existiert");
    //printFile("myViewer.TXT");
    //loadConfiguration("myViewer.TXT");
  }
  **/
  //myViewer.push_back("AAA"); // Ad item at the front of the line
  //myViewer.push_front("BBB"); // Ad item at the front of the line



  //Connect to twitch via websockets
  twitch_api.connect_to_twitch_websocket();

  //Register generic callbacks
  twitch_api.onNotice(onNoticeCallback);
  twitch_api.onGlobalUserState(onGlobalUserStateCallback);
  twitch_api.onPrivMsg(onPrivMsgCallback);
  twitch_api.onRoomState(onRoomStateCallback);
  //  twitch_api.onUserNotice(onUserNoticeCallback);
  twitch_api.onUserState(onUserStateCallback);
  twitch_api.onHostTarget(onHostTargetCallback);

  //Register custom callbacks
  twitch_api.onChannelHosted(onChannelHostedCallback);
  twitch_api.onBits(onBitsCallback);
  twitch_api.onSub(onSubCallback);
  twitch_api.onReSub(onReSubCallback);
  twitch_api.onSubGift(onSubGiftCallback);
  twitch_api.onAnonSubGift(onAnonSubGiftCallback);
  twitch_api.onSubMysteryGift(onSubMysteryGiftCallback);
  twitch_api.onGiftPaidUpgrade(onGiftPaidUpgradeCallback);
  twitch_api.onRewardGift(onRewardGiftCallback);
  twitch_api.onAnonGiftPaidUpgrade(onAnonGiftPaidUpgradeCallback);
  twitch_api.onBitsBadgeTier(onBitsBadgeTierCallback);
  twitch_api.onCustomReward(onCustomRewardCallback);
  twitch_api.onHighlightedMsg(onHighlightedMsgCallback);
  twitch_api.onModChat(onModChatCallback);


  //Helper Functions
  /*
  A sub plan is returned on subs, resub, gift, etc. You can use the helper function to convert int value to string.
  */
  printf("%s\n", twitch_api.sub_type_to_string(0));
  printf("%s\n", twitch_api.sub_type_to_string(1000));
  printf("%s\n", twitch_api.sub_type_to_string(2000));
  printf("%s\n", twitch_api.sub_type_to_string(3000));
  /*
  Emotes are returned when people say them in chat or in messages when donating. You can use the function to get the image url of the icon.
  Sizes: IMG_SMALL, IMG_MEDIUM, IMG_LARGE
  */
  printf("Emote Image URL: %s\n", twitch_api.get_emote_image_url(301448586, TwitchWebsocketIRC::Emote_Image_Size::IMG_MEDIUM));

  Serial.print("Connecting to SQL...  ");
  if (conn.connect(server_addr, 3306, user, passwordd))
  Serial.println("OK.");
  else
  Serial.println("FAILED.");

  // create MySQL cursor object
  cursor = new MySQL_Cursor(&conn);

}

void loop() {
  if (twitch_api.available()) {
    twitch_api.poll();
  }

  // handle NTP
  timeClient.update();
  String zeit = timeClient.getFormattedTime();
  //u8g2.setFont(u8g2_font_pcsenior_8f);
  //u8g2.setCursor(64, 63);
  //u8g2.print(zeit);
  //u8g2.sendBuffer();



}

char* string2char(String command) {
  if (command.length() != 0) {
    char *p = const_cast<char*>(command.c_str());
    return p;
  }
}

/* Generic Callback Functions */
void onNoticeCallback(Notice data) {
  printf("\n---------------------------------------------------\n");
  printf("\n=NOTICE=\nChannel Name: %s\nMsg-id: %s\nMessage: %s\n", data.channel_name, data.msg_id, data.message);
  printf("\n---------------------------------------------------\n");
}
void onGlobalUserStateCallback(GlobalUserState data) {
  printf("\n---------------------------------------------------\n");
  int count = 0;
  printf("\n=GLOBALUSERSTATE=\n");
  printf("\nBadges Info\n");
  while (data.badge_info[count].name != NULL) {
    printf("\t-Badge Name: %s Version: %i\n", data.badge_info[count].name, data.badge_info[count].version);
    count += 1;
  }
  count = 0;
  printf("\nBadges\n");
  while (data.badges[count].name != NULL) {
    printf("\t-Badge Name: %s Version: %i\n", data.badges[count].name, data.badges[count].version);
    count += 1;
  }
  count = 0;
  printf("\nEmote Sets\n");
  while (data.emote_sets[count].emote_id != 0 || count == 0) {
    printf("\t-Emote Id: %i\n", data.emote_sets[count].emote_id);
    count += 1;
  }
  printf("Color: %s\nDisplay Name: %s\nUser Id: %i\n",
  data.color, data.display_name, data.user_id);
  printf("\n---------------------------------------------------\n");
}
void onPrivMsgCallback(PrivMsg data) {
  printf("\n---------------------------------------------------\n");
  int count = 0;
  printf("\n=PRIVMSG=\n");
  printf("\nBadges Info\n");
  while (data.badge_info[count].name != NULL) {
    printf("\t-Badge Name: %s Version: %i\n", data.badge_info[count].name, data.badge_info[count].version);
    count += 1;
  }
  count = 0;
  printf("\nBadges\n");
  while (data.badges[count].name != NULL) {
    printf("\t-Badge Name: %s Version: %i\n", data.badges[count].name, data.badges[count].version);
    count += 1;
  }
  count = 0;
  printf("\nEmote Chat Set\n");
  while (data.emotes[count].start_index_location != 0 && data.emotes[count].end_index_location != 0) {
    printf("\t-Emote Id: %i\n", data.emotes[count].emote.emote_id);
    printf("\t-Emote First Index: %i\n", data.emotes[count].start_index_location);
    printf("\t-Emote Second Index: %i\n", data.emotes[count].end_index_location);
    count += 1;
  }
  printf("Bits: %i\nColor: %s\nDisplay Name: %s\nId: %s\nMessage: %s\nMod: %i\nRoom Id: %i\nTmi Sent Ts: %s\nUser Id: %i\nChannel Name: %s\n",
  data.bits, data.color, data.display_name, data.id, data.message, data.mod, data.room_id, data.tmi_sent_ts, data.user_id, data.channel_name);
  printf("\n---------------------------------------------------\n");

  String text = String(data.message);
  String textzeile[100] = "";
  String tempzeile = "";
  int textzeilen = 0;
  int letztesleerzeichen = 0;
  if (text.length() >= 17) {
    do {
      ++textzeilen;
      tempzeile = text.substring(0, 16);
      letztesleerzeichen = tempzeile.lastIndexOf(' ');
      if (letztesleerzeichen == -1 && text.length() >= 17) {
        letztesleerzeichen = 16;
      }
      textzeile[textzeilen - 1] = text.substring(0, letztesleerzeichen);
      text = text.substring(letztesleerzeichen);
      text.trim();
      Serial.println(textzeile[textzeilen - 1] );
    } while (text.length() >= 17);
    ++textzeilen;
    textzeile[textzeilen - 1] = text;
    Serial.println(textzeile[textzeilen - 1] );
  } else {
    ++textzeilen;
    textzeile[textzeilen - 1] = text;
    Serial.println(textzeile[textzeilen - 1] );
  }

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_unifont_te);
  u8g2.setCursor(0, 16);
  u8g2.print("CHAT");

  u8g2.setFont(u8g2_font_pcsenior_8f);
  u8g2.setCursor(0, 24);
  u8g2.print(data.display_name);
  for (int i = 0; i < textzeilen; i++) {
    u8g2.setCursor(0, 32 + (i * 8));
    u8g2.print(textzeile[i]);
  }
  u8g2.sendBuffer();

  String displayname = String(data.display_name);
  displayname.trim();
  int x = writelog("chatter", displayname, "add", 1);
  Serial.print("Chat-Counter: ");
  Serial.println(x);
  if (x == 1) {
    String nachricht = "Hallo @" + displayname + ". Willkommen im Chat...";
    //twitch_api.send_chat_message(string2char(nachricht));
    //Serial.print("aktive chatter: ");
    //Serial.println(countDirectory("chatter", 0));
  }

  if (text.substring(0, 12).equals("!resetviewer") && displayname.substring(0, 9).equals("bajoraner")) {
    Serial.println("Viewer reset");
    deleteDirectory("chatter", 0);

  }

  if (text.substring(0, 10).equals("!resetbits") && displayname.substring(0, 9).equals("bajoraner")) {
    Serial.println("bits reset");
    deleteDirectory("bits", 0);
  }

  if (text.substring(0, 10).equals("!resetsubs") && displayname.substring(0, 9).equals("bajoraner")) {
    Serial.println("subs reset");
    deleteDirectory("subs", 0);
  }

  if (text.substring(0, 13).equals("!resetsubgift") && displayname.substring(0, 9).equals("bajoraner")) {
    Serial.println("subgift reset");
    deleteDirectory("subgift", 0);
  }


  //Serial.print("aktive bits cheerer: ");
  //Serial.println(countDirectory("bits", 0));

}
void onRoomStateCallback(RoomState data) {
  printf("\n---------------------------------------------------\n");
  printf("\n=ROOMSTATE=\n");
  printf("Emote Only: %i\nFollowers Only: %i\nR9K: %i\nSlow: %i\nSubs Only: %i\nChannel Name: %s\n",
  data.emote_only, data.followers_only, data.r9k, data.slow, data.subs_only, data.channel_name);
  printf("\n---------------------------------------------------\n");
}
/*void onUserNoticeCallback(UserNotice data) {
printf("\n---------------------------------------------------\n");
printf("\n=USERNOTICE=\n");
printf("\nBadges Info\n");
int count = 0;
while (data.badge_info[count].name != NULL) {
printf("\t-Badge Name: %s Version: %i\n", data.badge_info[count].name, data.badge_info[count].version);
count += 1;
}
count = 0;
printf("\nBadges\n");
while (data.badges[count].name != NULL) {
printf("\t-Badge Name: %s Version: %i\n", data.badges[count].name, data.badges[count].version);
count += 1;
}
count = 0;
printf("\nEmote Chat Set\n");
while (data.emotes[count].start_index_location != 0 && data.emotes[count].end_index_location != 0) {
printf("\t-Emote Id: %i\n", data.emotes[count].emote.emote_id);
printf("\t-Emote First Index: %i\n", data.emotes[count].start_index_location);
printf("\t-Emote Second Index: %i\n", data.emotes[count].end_index_location);
count += 1;
}
printf("Color: %s\nDisplay Name: %s\nId: %s\nLogin: %s\nMod: %i\nMsg_Id: %s\nRoom_Id: %i\nSystem_Msg: %s\nTmi Sent Ts: %s\nUser_Id: %i\nChannel_Name: %s\n",
data.color, data.display_name, data.id, data.login, data.mod, data.msg_id, data.room_id, data.system_msg, data.tmi_sent_ts, data.user_id, data.channel_name);
printf("\nmsg_param_cumulative_months: %i\nmsg_param_display_name: %s\nmsg_param_login: %s\nmsg_param_months: %i\nmsg_param_promo_gift_total: %i\n",
data.msg_param_cumulative_months, data.msg_param_display_name, data.msg_param_login, data.msg_param_months, data.msg_param_promo_gift_total);
printf("\nmsg_param_promo_name: %s\nmsg_param_recipient_display_name: %s\nmsg_param_recipient_id: %i\nmsg_param_recipient_user_name: %s\nmsg_param_sender_login: %s\nmsg_param_sender_name: %s\n",
data.msg_param_promo_name, data.msg_param_recipient_display_name, data.msg_param_recipient_id, data.msg_param_recipient_user_name, data.msg_param_sender_login, data.msg_param_sender_name);
printf("\nmsg_param_should_share_streak: %i\nmsg_param_streak_months: %i\nmsg_param_sub_plan: %i\nmsg_param_sub_plan_name: %s\nmsg_param_viewerCount: %i\nmsg_param_ritual_name: %s\nmsg_param_threshold: %i\n",
data.msg_param_should_share_streak, data.msg_param_streak_months, data.msg_param_sub_plan, data.msg_param_sub_plan_name, data.msg_param_viewerCount, data.msg_param_ritual_name, data.msg_param_threshold);
printf("\n---------------------------------------------------\n");
}
*/
void onUserStateCallback(UserState data) {
  printf("\n---------------------------------------------------\n");
  printf("\n=USERSTATE=\n");
  printf("\nBadges Info\n");
  int count = 0;
  while (data.badge_info[count].name != NULL) {
    printf("\t-Badge Name: %s Version: %i\n", data.badge_info[count].name, data.badge_info[count].version);
    count += 1;
  }
  count = 0;
  printf("\nBadges\n");
  while (data.badges[count].name != NULL) {
    printf("\t-Badge Name: %s Version: %i\n", data.badges[count].name, data.badges[count].version);
    count += 1;
  }
  count = 0;
  printf("\nEmote Sets\n");
  while (data.emote_sets[count].emote_id != 0 || count == 0) {
    printf("\t-Emote Id: %i\n", data.emote_sets[count].emote_id);
    count += 1;
  }
  printf("Color: %s\nDisplay Name: %s\nMod: %i\nChannel Name: %s\n",
  data.color, data.display_name, data.mod, data.channel_name);
  printf("\n---------------------------------------------------\n");
}
void onHostTargetCallback(HostTarget data) {
  printf("\n---------------------------------------------------\n");
  printf("\n=HOSTTARGET=\n");
  printf("Hosting Channel Name: %s\nNumber Of Viewers: %i\nChannel Name: %s\n",
  data.hosting_channel_name, data.number_of_viewers, data.channel_name);
  printf("\n---------------------------------------------------\n");
}

/* Custom Callbacks */
void onChannelHostedCallback(Hostee data) {
  printf("\n---------------------------------------------------\n");
  printf("\n=CHANNEL HOSTED=\n");
  printf("Hostee Display Name: %s\nViewers: %i\n", data.display_name, data.viewer_count);
  printf("\n---------------------------------------------------\n");
}
void onBitsCallback(PrivMsg data) {
  printf("\n---------------------------------------------------\n");
  printf("\n=BITS=\n");
  printf("Bit Donater Name: %s\nBits: %i\n", data.display_name, data.bits);
  printf("\n---------------------------------------------------\n");

  String displayname = String(data.display_name);
  displayname.trim();
  int x = writelog("bits", displayname, "add", data.bits);
  Serial.print("bits-Counter: ");
  Serial.println(x);


}
void onSubCallback(UserNotice data) {
  printf("\n---------------------------------------------------\n");
  printf("\n=SUB=\n");
  printf("Subscribers Name: %s\nTotal Months Subbed: %i\nSub Streak: %i\nSub Type: %i\nSub Plan Name: %s\nShare Sub Streak: %i\n",
  data.display_name, data.msg_param_cumulative_months, data.msg_param_streak_months, data.msg_param_sub_plan, data.msg_param_sub_plan_name, data.msg_param_should_share_streak);
  printf("\n---------------------------------------------------\n");
  String displayname = String(data.display_name);
  displayname.trim();
  int x = writelog("subs", displayname, "same", data.msg_param_cumulative_months);
  Serial.print("Subs-Counter: ");
  Serial.println(x);






}
void onReSubCallback(UserNotice data) {
  printf("\n---------------------------------------------------\n");
  printf("\n=RESUB=\n");
  printf("Subscribers Name: %s\nTotal Months Subbed: %i\nSub Streak: %i\nSub Type: %i\nSub Plan Name: %s\nShare Sub Streak: %i\n",
  data.display_name, data.msg_param_cumulative_months, data.msg_param_streak_months, data.msg_param_sub_plan, data.msg_param_sub_plan_name, data.msg_param_should_share_streak);
  if (!String(data.message).equals("")) {
    printf(data.message);
  }
  printf("\n---------------------------------------------------\n");
  String displayname = String(data.display_name);
  displayname.trim();
  int x = writelog("subs", displayname, "same", data.msg_param_cumulative_months);
  Serial.print("Subs-Counter: ");
  Serial.println(x);


}
void onSubGiftCallback(UserNotice data) {
  printf("\n---------------------------------------------------\n");
  printf("\n=SUB GIFT=\n");
  printf("Gifters Name: %s\nTotal Months Subbed: %i\nRecipient Display Name: %s\nRecipient User Id: %i\nRecipient Username: %s\nSub Plan:%i\nSub Plan Name: %s\n",
  data.display_name, data.msg_param_months, data.msg_param_recipient_display_name, data.msg_param_recipient_id, data.msg_param_recipient_user_name, data.msg_param_sub_plan, data.msg_param_sub_plan_name);
  printf("\n---------------------------------------------------\n");
  String displayname = String(data.display_name);
  displayname.trim();
  int x = writelog("subgift", displayname, "add", 1);
  Serial.print("SubGift-Counter: ");
  Serial.println(x);
  displayname = String(data.msg_param_recipient_display_name);
  displayname.trim();
  x = writelog("subs", displayname, "same", data.msg_param_months);
  Serial.print("Subs-Counter: ");
  Serial.println(x);



}
void onAnonSubGiftCallback(UserNotice data) {
  printf("\n---------------------------------------------------\n");
  printf("\n=ANON SUB GIFT=\n");
  printf("Total Months Subbed: %i\nRecipient Display Name: %s\nRecipient User Id: %i\nRecipient Username: %s\nSub Plan:%i\nSub Plan Name: %s\n",
  data.msg_param_months, data.msg_param_recipient_display_name, data.msg_param_recipient_id, data.msg_param_recipient_user_name, data.msg_param_sub_plan, data.msg_param_sub_plan_name);
  printf("\n---------------------------------------------------\n");
}
void onSubMysteryGiftCallback(UserNotice data) {
  printf("\n---------------------------------------------------\n");
  printf("\n=SUB MYSTERY GIFT=\n");
  printf("\n---------------------------------------------------\n");
}
void onGiftPaidUpgradeCallback(UserNotice data) {
  printf("\n---------------------------------------------------\n");
  printf("\n=GIFT PAID UPGRADE=\n");
  printf("Number of gifts subs given: %i\n Sub Promo Name: %s\n Gifters Login: %s\n Gifters Display Name: %s\n",
  data.msg_param_promo_gift_total, data.msg_param_promo_name, data.msg_param_sender_login, data.msg_param_sender_name);
  printf("\n---------------------------------------------------\n");
}
void onRewardGiftCallback(UserNotice data) {
  printf("\n---------------------------------------------------\n");
  printf("\n=REWARD GIFT=\n");
  printf("\n---------------------------------------------------\n");
}
void onAnonGiftPaidUpgradeCallback(UserNotice data) {
  printf("\n---------------------------------------------------\n");
  printf("\n=ANON GIFT PAID UPGRADE=\n");
  printf("Number of gifts subs given: %i\n Sub Promo Name: %s\n",
  data.msg_param_promo_gift_total, data.msg_param_promo_name);
  printf("\n---------------------------------------------------\n");
}
void onBitsBadgeTierCallback(UserNotice data) {
  printf("\n---------------------------------------------------\n");
  printf("\n=BITS BADGE TIER=\n");
  printf("Bits Badge Tier: %i", data.msg_param_threshold);
  printf("\n---------------------------------------------------\n");
}
void onCustomRewardCallback(PrivMsg data) {
  printf("\n---------------------------------------------------\n");
  printf("\n=CUSTOM REWARD ID=\n");
  printf("Custom Reward Id: %s\n", data.custom_reward_id);
  printf("\n---------------------------------------------------\n");
  u8g2.setFont(u8g2_font_unifont_te);
  u8g2.setCursor(0, 16);
  u8g2.print("REWARD");
  u8g2.sendBuffer();
}
void onHighlightedMsgCallback(PrivMsg data) {
  printf("\n---------------------------------------------------\n");
  printf("\n=HIGHLIGHTED MESSAGE=\n");
  printf("Highlighted Message: %s\n", data.message);
  printf("\n---------------------------------------------------\n");
  u8g2.setFont(u8g2_font_unifont_te);
  u8g2.setCursor(0, 16);
  u8g2.print("HIGHLIGHTED");
  u8g2.sendBuffer();

}
void onModChatCallback(PrivMsg data) {
  printf("\n---------------------------------------------------\n");
  printf("\n=MODCHAT=\n");
  printf("Moderator's Display Name: %s\nModerator's Message: %s\n", data.display_name, data.message);
  printf("\n---------------------------------------------------\n");
  //u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_unifont_te);
  u8g2.setCursor(103, 16);
  //u8g2.print("CHAT");

  //u8g2.setFont(u8g2_font_pcsenior_8f);
  //u8g2.setCursor(0, 24);
  //u8g2.print(data.display_name);
  //u8g2.setCursor(0, 64);
  u8g2.print("MOD");
  u8g2.sendBuffer();

}


void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.print(entry.size(), DEC);
      Serial.println("\t\t");
      time_t ft = entry.getLastWrite();
      struct tm *tm = localtime(&ft);
      // US format.  Feel free to convert to your own locale...
      //Serial.println("%02d-%02d-%02d %02d:%02d:%02d\n", tm->tm_mon + 1, tm->tm_mday, tm->tm_year % 100, tm->tm_hour, tm->tm_min, tm->tm_sec);
    }
    entry.close();
  }
}


void deleteDirectory(String fil, int numTabs) {
  if (conn.connected()) {
    sprintf(TRUNC_SQL, "TRUNCATE TABLE esp.%s;", fil.c_str());
    Serial.println(TRUNC_SQL);
    cursor->execute(TRUNC_SQL);

  }

/**
  root = SD.open("/");
  delay(2000);

  rm(root, rootpath);

  if( !DeletedCount && !FailCount && !FolderDeleteCount ){

  }
  else{
    Serial.print("Deleted ");
    Serial.print(DeletedCount);
    Serial.print(" file");
    if( DeletedCount != 1 ){
      Serial.print("s");
    }
    Serial.print(" and ");
    Serial.print(FolderDeleteCount);
    Serial.print(" folder");
    if ( FolderDeleteCount != 1 ){
      Serial.print("s");
    }
    Serial.println(" from SD card.");
    if( FailCount > 0 ){
      Serial.print("Failed to delete ");
      Serial.print(FailCount);
      Serial.print(" item");
      if( FailCount != 1 ){
        Serial.print("s");
      }
    }
  }

**/

/**String fullfilename;
Serial.println("001");
  if (SD.exists(fil)) {
    Serial.println("002");
    Serial.println("Lösche Dateien in " + fil + "...");
    Serial.println("003");
    File dir = SD.open(fil);
    Serial.println("004");
    while (true) {
      Serial.println("005");
      File entry =  dir.openNextFile();
      Serial.println("006");
      if (! entry) {
        Serial.println("007");
        // no more files
        // return to the first file in the directory
        dir.rewindDirectory();
        Serial.println("008");
        entry.close();
        Serial.println("009");
        break;
        Serial.println("010");
      }
      Serial.println("011");
      Serial.println(ESP.getFreeHeap());
      Serial.println("012");
      Serial.println(entry.name());
      Serial.println("013");
       //fullfilename = fil + "/" + entry.name();
       Serial.println("014");
      entry.close();
      Serial.println("015");
      //delay(100);
      Serial.println("016");
    //SD.remove(fullfilename);
    Serial.println("017");
  //delay(1000);
  Serial.println("018");

    }
    Serial.println("019");
    dir.close();
    Serial.println("020");
    delay(100);
    Serial.println("021");
  }
  Serial.println("022");
**/

}


void rm(File dir, String tempPath) {
  while(true) {
    File entry =  dir.openNextFile();
    String localPath;

    Serial.println("");
    if (entry) {
      if ( entry.isDirectory() )
      {
        localPath = tempPath + entry.name() + rootpath + '\0';
        char folderBuf[localPath.length()];
        localPath.toCharArray(folderBuf, localPath.length() );
        rm(entry, folderBuf);


        if( SD.rmdir( folderBuf ) )
        {
          Serial.print("Deleted folder ");
          Serial.println(folderBuf);
          FolderDeleteCount++;
        }
        else
        {
          Serial.print("Unable to delete folder ");
          Serial.println(folderBuf);
          FailCount++;
        }
      }
      else
      {
        localPath = tempPath + entry.name() + '\0';
        char charBuf[localPath.length()];
        localPath.toCharArray(charBuf, localPath.length() );

        if( SD.remove( charBuf ) )
        {
          Serial.print("Deleted ");
          Serial.println(localPath);
          DeletedCount++;
        }
        else
        {
          Serial.print("Failed to delete ");
          Serial.println(localPath);
          FailCount++;
        }

      }
    }
    else {
      // break out of recursion
      break;
    }
  }
}


int countDirectory(String fil, int numTabs) {
/**  if (SD.exists(fil)) {
    int x = 0;
    //Serial.println("Zähle Dateien in " + fil + "...");
    File dir = SD.open(fil);
    while (true) {
      File entry =  dir.openNextFile();
      if (! entry) {
        // no more files
        break;
      }
      for (uint8_t i = 0; i < numTabs; i++) {
        Serial.print('\t');
      }
      //Serial.println(entry.name());
      String fullfilename = fil + "/" + entry.name();
      x++;
      entry.close();
    }
    dir.close();
    return x;
  }
  **/
}


int writelog(String typ, String fil, String way, unsigned int z) {
  int y = 0;
  int yy = 0;
  int x;
  int xx;

/**
  if (!SD.exists(typ)) {
    Serial.println(F("Ordner existiert nicht, wird erstellt..."));
    SD.mkdir(typ);
  }
  //prüfen ob chatter vorhanden ggf. anlegen
  String filename = typ + "/" + fil + ".txt";
  //Serial.println(filename);
  if (!SD.exists(filename)) {
    //Serial.println("Datei existiert nicht");
    File file = SD.open(filename, FILE_WRITE);
    if (!file) {
      Serial.println(F("Failed to create file"));
      return z;
    }
    file.write(z);
    file.close();
    x = z;
  } else {
    //Serial.println("Datei existiert");
    File file = SD.open(filename, FILE_READ);
    if (file) {
      // read from the file until there's nothing else in it:
      while (file.available()) {
        y = file.read();
      }
      //Serial.println(y);
      file.close();
    }
    if (way.equals("add")) {
      x = y + z;
    } else {
      x = z;
    }
    SD.remove(filename);
    file = SD.open(filename, FILE_WRITE);
    if (file) {
      // read from the file until there's nothing else in it:
      //Serial.println(x);
      file.write(x);
      file.close();
    } else {
      Serial.println(F("Failed to create file"));
      return x;
    }
  }
**/

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
    } else {
      xx = yy + z;
      sprintf(UPDATE_SQL, "UPDATE esp.%s SET content = '%i' WHERE name = '%s';", typ.c_str(), xx, fil.c_str());
      Serial.println(UPDATE_SQL);
      cursor->execute(UPDATE_SQL);
      //        UPDATE esp.%s SET `content` = '%i' WHERE 'name' = '$s';
    }




  } else {
    Serial.println(F("Datenbank nicht verbunden"));
  }



  return xx;
}
