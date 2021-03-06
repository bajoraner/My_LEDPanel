#include "meineklassen.h"

#include <twitch_irc_api.h>
TwitchWebsocketIRC twitch_api;



int summe = 0;


void setup() {
  Serial.begin(115200);
  Serial.println( "############################################################################" );
  Serial.println( "############################################################################" );
  Serial.println( "############################################################################" );
  pinMode( D0, OUTPUT );
  /* Global::Example Klasse ansprechen und mit Start() sprichst du die Methode in der Klasse an */
  meineklassen::MyWlanInstance.Verbinden();
  meineklassen::MyNTPInstance.Initiieren();
  meineklassen::MyLEDInstance.Start();
  //Connect to twitch via websockets
  twitch_api.connect_to_twitch_websocket();
  //Register custom callbacks
  twitch_api.onCustomReward(onCustomRewardCallback);
  twitch_api.onPrivMsg(onPrivMsgCallback);
}

void loop() {
  meineklassen::MyNTPInstance.Updaten();
  meineklassen::MyLEDInstance.Zeitanzeige();
  if (twitch_api.available()) {
    twitch_api.poll();
  }
  if (meineklassen::MyVoteInstance.voteactive == 1)
  {
    if (meineklassen::MyVoteInstance.vote[0] > 0)
    {
      meineklassen::MyVoteInstance.Auswerten();
    }
  }


  delay(100);
}

void onCustomRewardCallback(PrivMsg data) {
  Serial.println("--------------------------------------------------------------------");
  Serial.print("Reward: ");
  Serial.println(String(data.custom_reward_id));
  int rewardfarbe = 0;
  String conrewid = String(data.custom_reward_id);
  if (conrewid.equals("35173dc7-f4f0-4fd8-811e-352bdd994217")) {
    String reward = "Text_rot";
    Serial.println("rot");
    rewardfarbe = 0;
  }
  if (conrewid.equals("e5d12170-a31b-49a4-b319-b4ec0f764b00")) {
    String reward = "Text_blau";
    Serial.println("blau");
    rewardfarbe = 2;
  }
  if (conrewid.equals("7906cfbe-b638-4735-a12e-a02b3853a798")) {
    String reward = "Text_grün";
    Serial.println("grün");
    rewardfarbe = 1;
  }
  if (conrewid.equals("96ad7cc2-c395-476a-be86-12f7ff5d83a0")) {
    String reward = "jemanden_nennen";
    Serial.println("nennen");
    rewardfarbe = 3;
  }
  String rewardusername = String(data.display_name);
  String rewardtext = String(data.message);
  int rewardhelligkeit = 20;
  int rewardxtimes = 3;
  String rewardausgabe = rewardusername + ": " + rewardtext;
  String rewardrueckmeldung = "@" + String(data.display_name) + " vielen Dank für das einlösen der Kanalpunkte... Text wird jetzt 3x angezeigt";
  Serial.println(rewardrueckmeldung);
  twitch_api.send_chat_message(meineklassen::MyInstance.string2char(rewardrueckmeldung));
  meineklassen::MyLEDInstance.LEDLauftext(rewardfarbe, rewardausgabe, rewardhelligkeit, rewardxtimes, 30);
  Serial.println("--------------------------------------------------------------------");
}

void onPrivMsgCallback(PrivMsg data) {
  Serial.println("####################################################################");
  Serial.println("####################################################################");
  String text = String(data.message);
  text.toLowerCase();
  String displayname = String(data.display_name);

  if (text.substring(0, 5).equals("!code")) {
    String ausgabe = "-CODE-";
    Serial.println(ausgabe);
    meineklassen::MyLEDInstance.LEDTextblinken(1, ausgabe, 200, 25, 30);
  }
  else if (text.substring(0, 9).equals("!giveaway") && displayname.substring(0, 9).equals("bajoraner")) {
    if (text.substring(0, 15).equals("!giveaway start")) {
      meineklassen::MyGiveawayInstance.Aktivieren(text);
      String ausgabe = "Giveaway ist gestartet, teilnehmen mit: " + meineklassen::MyGiveawayInstance.giveawayname;
      twitch_api.send_chat_message(meineklassen::MyInstance.string2char(ausgabe));
      meineklassen::MyLEDInstance.LEDLauftext(0, ausgabe, 30, 1, 30);
    }
    if (text.substring(0, 14).equals("!giveaway stop")) {
      meineklassen::MyGiveawayInstance.Stoppen();
      String ausgabe = "Giveaway ist gestoppt";
      twitch_api.send_chat_message(meineklassen::MyInstance.string2char(ausgabe));
      meineklassen::MyLEDInstance.LEDLauftext(0, ausgabe, 30, 1, 30);
    }
    if (text.substring(0, 16).equals("!giveaway raffle")) {
      String ausgabe = "Gewinner wird ausgelost...";
      Serial.println(ausgabe);
      twitch_api.send_chat_message(meineklassen::MyInstance.string2char(ausgabe));
      String gewinner = meineklassen::MyGiveawayInstance.Auswerten();
      ausgabe = "Gewonnen hat: " + gewinner;
      Serial.println(ausgabe);
      twitch_api.send_chat_message(meineklassen::MyInstance.string2char(ausgabe));
      meineklassen::MyLEDInstance.LEDLauftext(0, ausgabe, 30, 3, 30);
    }
    if (text.substring(0, 13).equals("!giveaway off")) {
      meineklassen::MyGiveawayInstance.Resetten();
    }
  } else if (text.substring(0, (meineklassen::MyGiveawayInstance.giveawayname.length() - 1)) == meineklassen::MyGiveawayInstance.giveawayname.substring(0, (meineklassen::MyGiveawayInstance.giveawayname.length() - 1))) {
    meineklassen::MyGiveawayInstance.Teilnehmen(displayname);
  }
  else if (text.substring(0, 13).equals("!activatevote") && displayname.substring(0, 9).equals("bajoraner")) {
    meineklassen::MyVoteInstance.Aktivieren();
    Serial.println(meineklassen::MyVoteInstance.voteactive);
    String ausgabe = "Voting wurde aktiviert - 1, 2 oder eine 3 zum Abstimmen in den Chat";
    Serial.println(ausgabe);
    twitch_api.send_chat_message(meineklassen::MyInstance.string2char(ausgabe));
    meineklassen::MyLEDInstance.LEDLauftext(2, ausgabe, 30, 1, 30);
  }
  else if (text.substring(0, 10).equals("!resetvote") && displayname.substring(0, 9).equals("bajoraner")) {
    String ausgabe = "Voting wurde beendet...";
    Serial.println(ausgabe);
    twitch_api.send_chat_message(meineklassen::MyInstance.string2char(ausgabe));
    meineklassen::MyLEDInstance.LEDLauftext(2, ausgabe, 30, 1, 30);
    ausgabe = "1=" + String(meineklassen::MyVoteInstance.vote[1]) + "x 2=" + String(meineklassen::MyVoteInstance.vote[2]) + "x 3=" + String(meineklassen::MyVoteInstance.vote[3]) + "x";
    twitch_api.send_chat_message(meineklassen::MyInstance.string2char(ausgabe));
    meineklassen::MyVoteInstance.Resetten();
    Serial.println(meineklassen::MyVoteInstance.voteactive);
  }
  else if (text.substring(0, 1).equals("1") && meineklassen::MyVoteInstance.voteactive == 1) {
    meineklassen::MyVoteInstance.Voten(1, displayname);
    String ausgabe = "Vote 1";
    meineklassen::MyLEDInstance.LEDText(2, ausgabe, 30, 1, 1000);
  }
  else if (text.substring(0, 1).equals("2") && meineklassen::MyVoteInstance.voteactive == 1) {
    meineklassen::MyVoteInstance.Voten(2, displayname);
    String ausgabe = "Vote 2";
    meineklassen::MyLEDInstance.LEDText(2, ausgabe, 30, 1, 1000);
  }
  else if (text.substring(0, 1).equals("3") && meineklassen::MyVoteInstance.voteactive == 1) {
    meineklassen::MyVoteInstance.Voten(3, displayname);
    String ausgabe = "Vote 3";
    meineklassen::MyLEDInstance.LEDText(2, ausgabe, 30, 1, 1000);
  }
  else {

    String ausgabe = " -CHAT- ";
    Serial.println(ausgabe);
    meineklassen::MyLEDInstance.LEDText(2, ausgabe, 100, 1, 100);

    //farbe, text, helligkeit, xtimes, delay
  }




  /*
     Textfarbe (2)
     Text
     Helligkeit
     data.display_name
     data.message
  */
  //meineklassen::MyInstance.ChatAusgabe(2, "Text", 20, data.display_name, data.message);


  //Serial.print("vor Methode: ");
  //Serial.println(summe);
  //summe = meineklassen::MyInstance.Chataddieren(summe);
  //Serial.print("nach Methode: ");
  //Serial.println(summe);
  //Serial.println(meineklassen::MyInstance.doppelte);
  Serial.println("####################################################################");
  Serial.println("####################################################################");

}
