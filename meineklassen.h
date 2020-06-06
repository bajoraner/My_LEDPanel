/*
    GlobalVariables.h Datei --- Pragma Once immer in der .h mit eintragen,
    verhindert das Problem mit der mehrfachen Einbindung "#include"
    Es wird auch "Include-Guard" gennant
*/
// Namespace = meineklassen
// extraKlasse = MyWlan
// Instance = MyWlanInstance
// Methode = Verbinden


#pragma once //* include guard *//
#include <Arduino.h>                /* Würde ich immer mit in die .h eintragen */
#include <WString.h>                /* Würde ich immer mit in die .h eintragen */


namespace meineklassen
{
    class MyWlan;             /* Interne Klasse Definieren */
    extern MyWlan MyWlanInstance;       /* Damit wird es eine Instance und kann mit dem include der .h überall angesprochen werden */

    class MyNTP;             
    extern MyNTP MyNTPInstance;       

    class My;             
    extern My MyInstance;       

    class MyVote;
    extern MyVote  MyVoteInstance;       

    class MyGiveaway;
    extern MyGiveaway  MyGiveawayInstance;       

    class MyLED;
    extern MyLED  MyLEDInstance;       


    //const TestText = "Hallo vom Test";  /* Muss eine const sein, wenn du die GlobalVariables mehrfach includes */
 
    //void ExampleMethode(String Var = "nichts");     /* Methode muss auch in die GlobalVariables.cpp eingetragen werden */
}
 
class meineklassen::MyWlan
{
    public:                        /* Natürlich kann man es auch private: machen, kannst du dann aber nur in der Klasse selber verwenden */
    void Verbinden();                  /* Methode in der Klasse */
    //unsigned int TestVar = 0;      /* Variable in der Klasse */
};
 
class meineklassen::MyNTP
{
    public:   
    void Initiieren();             
    void Updaten();                
};

class meineklassen::My
{
    public:                        
    void ChatAusgabe(int textfarbe, String text, int helligkeit, String username, String message);             
    int Chataddieren(int zahl);     
    char* string2char(String command);     
    int doppelte = 0;      
};

class meineklassen::MyVote
{
    public:                        
    int vote[4] = {0, 0, 0, 0 };  
    int voteactive = 0;
    String useridvote = "";
    void Aktivieren();             
    void Resetten();                
    void Voten(int voting, String displayname);                
    void Auswerten();                
};

class meineklassen::MyGiveaway
{
    public:                        
    int ga[1] = {0};  
    int gaactive = 0;
    String userids = "";
    String giveawayname = "";
    void Aktivieren(String text);             
    void Stoppen();                
    void Resetten();                
    void Teilnehmen(String displayname);                
    String Auswerten();                
};

class meineklassen::MyLED
{
    public:                        
    void LEDLauftext(int farbe, String ausgabe, int helligkeit, int xtimes, int del);   
    void LEDTextblinken(int farbe, String ausgabe, int helligkeit, int xtimes, int del);   
    void LEDText(int farbe, String ausgabe, int helligkeit, int xtimes, int del);   
    void LEDBalken(int farbe, int anzahl, int helligkeit, int xtimes, int del);   
    void LEDVoting(int farbe, int* anzahl, int helligkeit, int xtimes, int del);   
    void Zeitanzeige();
             
    void Start();
};

using namespace meineklassen;             /* Damit die Klasse angesprochen wird, sobald du die .h includest */
 
 
 
