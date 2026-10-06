#include <TinyGPSPlus.h>
#include <ArduinoJson.h>
String addmqttserver; //tls
#include <PubSubClient.h>
//#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <Ticker.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
const byte DNS_PORT = 53;
DNSServer dnsServer;
WebServer webServer(80); //dns
#include "helpers.h"
#include "global.h"
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
int totalColumns = 16;
int totalRows = 2;
LiquidCrystal_I2C lcd(0x27, totalColumns, totalRows);

//RTC_DS3231 rtc;

#include "secrets.h"   // ACCESS_POINT_NAME / ACCESS_POINT_PASSWORD (see secrets.h.example)
#define AdminTimeOut 180  // Defines the Time in Seconds, when the Admin-Mode will be diabled
#define D0  23//16
#define D1  18 // 5
#define D2  5// 4
#define D3  25// 0
#define D4  14// 2--buzer
/*#define D5  26// 14-RLY1
#define D6  27// 12-RLY2
#define D7  12// 13-RLY 3
#define D8  13 // 15-RLY 4*/ //SWAP Relays from LEDT to RIGHT in DOUBLE side PCB D5-D8
#define D5  13// 14-RLY1
#define D6  12// 12-RLY2
#define D7  27// 13-RLY 3
#define D8  26 // 15-RLY 4
#define D9  15 // 3 -RLY 5
#define D10  2// 1 -RLY 6 START UP PULSE FOUND

#define I1 36
#define I2 39
#define I3 34
#define I4 35
#define I5 32
//SERIAL2
#define RXD2 16
#define TXD2 17
TinyGPSPlus GPS_RL;
int loc = 0;

int sw1=0;
int sw2=0;
int sw3=0;
int sw4=0;
int sw5=0;
int sw6=0;
char sureshkumar = '0';
WiFiClient wifiClient;
//PubSubClient client(espClient);
PubSubClient client(addmqttserver.c_str(), config.mqttPort.toInt(), wifiClient);
const char* ca_cert;
void callback(char* topic, byte* payload, unsigned int length);
//GPS
unsigned long previousMillis = 0;        
const long interval = 5*1000;
String ntpdt;
int GPS_FLAG = 0;//USED FOR STATIC GPS LOC
String LAT;
String LON;
String LAT_1;
String LON_1;
//ultrasonic
const int trigPin_1 = 4; //2
const int echoPin_1 = 2; //4

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
long duration_1;
float distanceCm_1;
int obs = 0;

int usenflag = 0; //RESEY button is press to initiate ultrasonic sensor and send gps loc in tts

int LOOP;
void setup () {

  // responseJson should contain response data  {"id":"afgaa-383" , "command","STA", "origin":"mobile"}{"request":"success"}
  responseJson.replace("}{", "");
    //SERIAL2
//  Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  EEPROM.begin(700);
  Serial.begin(9600);
  Serial.print("setup() running on core ");
  Serial.println(xPortGetCoreID());
pinMode(D0, OUTPUT);
   digitalWrite(D0, LOW);
   pinMode(D4, OUTPUT);
 // digitalWrite(D4, HIGH);
   pinMode(D5, OUTPUT);
//  digitalWrite(D5, HIGH);
   pinMode(D6, OUTPUT);
//  digitalWrite(D6, HIGH);
   pinMode(D7, OUTPUT);
//  digitalWrite(D7, HIGH);
   pinMode(D8, OUTPUT);
 // digitalWrite(D8, HIGH);
   pinMode(D9, OUTPUT);
 // digitalWrite(D9, HIGH);
  // pinMode(D10, OUTPUT);
 // digitalWrite(D10, HIGH);

 // pinMode(I1, INPUT);
  pinMode(I2, INPUT);
  pinMode(I3, INPUT);
  pinMode(I4, INPUT);
  pinMode(I5, INPUT);//RESET
  //-----------------------------------------Ulsonic-------------------
  pinMode(trigPin_1, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin_1, INPUT); // Sets the echoPin as an Input
  //pinMode(I6, INPUT);
 // pinMode(I7, INPUT);//RESET
  digitalWrite(D4, HIGH);
  delay(100);
  digitalWrite(D4, LOW);
 //-----------------------------------------servo & Ulsonic-------------------
lcd.init(); 
  lcd.backlight(); // use to turn on and turn off LCD back light
  lcd.setCursor(0, 0);
  lcd.print("IoT-blind Safety");
  lcd.setCursor(0, 1);
  lcd.print("& Voice Guidence");
addada1=config.mqttUser+"/f/DEVICE1";
  addada2=config.mqttUser+"/f/DEVICE2";
  addada3=config.mqttUser+"/f/DEVICE3";
  addada4=config.mqttUser+"/f/DEVICE4";
  addada5=config.mqttUser+"/f/REPLY";
  addada6=config.mqttUser+"/f/SERIAL1";
  addada7=config.mqttUser+"/f/CH1";
  addada8=config.mqttUser+"/f/CH2";
  addada9=config.mqttUser+"/f/CH3";
  addada10=config.mqttUser+"/f/CH4";
  addada11=config.mqttUser+"/f/CH5";
  addada12=config.mqttUser+"/f/ADC";
  addada13=config.mqttUser+"/f/GPS/csv";
 // addada13="$DEP$"+config.DeviceName;
//  addada13=config.mqttUser+"/f/"+config.mqttTopic;
  strcpy(test1, config.mqttUser.c_str());
  strcpy(test2, config.mqttPassword.c_str());
  strcpy(test3, config.mqttServer.c_str());
  strcpy(acid, config.mqttTopic.c_str());
  strcpy(topic1,  addada1.c_str());
  strcpy(topic2,  addada2.c_str());
  strcpy(topic3,  addada3.c_str());
  strcpy(topic4,  addada4.c_str());
  strcpy(topic5,  addada5.c_str());
  strcpy(topic6,  addada6.c_str());
  strcpy(topic7,  addada7.c_str());
  strcpy(topic8,  addada8.c_str());
  strcpy(topic9,  addada9.c_str());
  strcpy(topic10,  addada10.c_str());
  strcpy(topic11,  addada11.c_str());
  strcpy(topic12,  addada12.c_str());
  strcpy(topic13,  addada13.c_str());

  Serial.println((char*) acid); // ada clentid
if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(D0, HIGH);
   // digitalWrite(D7, LOW);
    //wifi_flag=1;
    Serial.println("WIFI_FLAG=1");
    StaticJsonBuffer<300> JSONbuffer;
    JsonObject& JSONencoder = JSONbuffer.createObject();
    ReadConfig();
    JsonArray& IPaddress = JSONencoder.createNestedArray("IPaddress");
    IPaddress.add(WiFi.localIP()[0]);
    IPaddress.add(WiFi.localIP()[1]);
    IPaddress.add(WiFi.localIP()[2]);
    IPaddress.add(WiFi.localIP()[3]);
    int lenghtSimple = JSONencoder.measureLength();
    Serial.print("Less overhead JSON message size: ");
    Serial.println(lenghtSimple);
    Serial.println("Connecting to WiFi..");
    char JSONmessageBuffer[300];
    JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);
    //Udp.write(JSONmessageBuffer);
    JSONencoder.printTo(Udp);
  }
  Serial.println("Connected to the WiFi network");
  Serial.println("");
  //wifi_flag=1;
  //Serial.println("WIFI_FLAG=1");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  //Serial.println(WiFi.localIP());
    //ada
    //Subscribe to the onoff topic
  mqtt.subscribe(&relay1);
  mqtt.subscribe(&relay2);
  mqtt.subscribe(&relay3);
  mqtt.subscribe(&relay4);
  mqtt.subscribe(&reply);
  mqtt.subscribe(&serial1);
  mqtt.subscribe(&ch2);
 // mqtt.subscribe(&eetopic);
 //CLIENT ID ADDED
 //PubSubClient (server, port, [callback], client, [stream])
  if(config.cloud == 1 && config.tls == 0)
  {
  client.setServer(addmqttserver.c_str(),config.mqttPort.toInt());
  client.setCallback(callback);
  }
   if(!client.connected()){
    Serial.println("Connecting to cloud MQTT...");
//CLIEND ID ADDED
//boolean connect (clientID, [username, password], [willTopic, willQoS, willRetain, willMessage], [cleanSession])
    if (client.connect(config.mqttTopic.c_str(),config.mqttUser.c_str(),config.mqttPassword.c_str() )) {

      Serial.println("connected");
      digitalWrite(D0, HIGH);
      lcd.setCursor(0, 1);
      lcd.print("DEVICE CONNECTED");
      delay(1000);

    } else {

      Serial.print("failed with state ");
      Serial.print(client.state());
     /* digitalWrite(D0, LOW);
      delay(500);
      digitalWrite(D0, HIGH);*/
      alarm2();
      lcd.clear(); 
      lcd.setCursor(0, 1);
      lcd.print("NETWORK NOTFOUND");
      delay(1000);
     // delay(2000);

    }
  
  }


//CLIEND ID ADDED
   client.publish(addREPLY.c_str(),"DEVICE Connected");
  // SYS();
  //client.subscribe(config.mqttTopic.c_str());
 // client.subscribe("ADC");//-----MULTIPLE SUB ADDED 28/4/19
  client.subscribe(addDEVICE1.c_str());
  client.subscribe(addDEVICE2.c_str());
  client.subscribe(addDEVICE3.c_str());
  client.subscribe(addDEVICE4.c_str());
 //-- client.subscribe(addDEVICE5.c_str());
 //-- client.subscribe(addDEVICE6.c_str());
 // client.subscribe("CH1");
  client.subscribe(addREPLY.c_str());
   }
void callback(char* topic, byte* payload, unsigned int length) {

  //Serial.print("Topic: ");
  //Serial.println(topic);

 // Serial.print("Message:");
  for (int i = 0; i < length; i++) {
   // Serial.print((char)payload[i]);

       String msg;


    //obtem a string do payload recebido
    for(int i = 0; i < length; i++)
    {
       char c = (char)payload[i];
       msg += c;
    }



    //toma ação dependendo da string recebida:
    //verifica se deve colocar nivel alto de tensão na saída.
    //IMPORTANTE: o Led já contido na placa é acionado com lógica invertida (ou seja,
    //enviar HIGH para o output faz o Led apagar / enviar LOW faz o Led acender)


    //verifica se deve colocar nivel alto de tensão na saída se enviar L e digito, ou nivel baixo se enviar D e digito no topíco LED
     //------------------------AUTO TRUN ON-----------
     if (msg.equals("CAMIP")) // node red CHANGE INTO SMALL TO CAPS
       {
       Serial.println("$GETIP$"); 
       }
        if (msg.equals("EMG")) // Emergency..
       {
       alarm2(); 
       }
}
void loop () {

     if( ttsflag==1)
     {
      TTS();
     }
     if( ttsflag==2)
     {
      G_P_S_TTS();
     }
     if( ttsflag==3)
     {
      USEN_TTS();
     }
      if( usenflag==1)
     {
      U_SEN_1();
     }
   // Read GPIO32 (external button to reset device
   if(config.cloud != 0) // PING WHILE CLOUD IS ENABLED
   {
   PING();
   }
   // U_SEN_1();
    if(digitalRead(I5) == HIGH) { //RESET
        ttsflag = 2;
        usenflag = 1;
        Serial.printf("Reset Button Pressed!\n");  
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(WiFi.localIP());
        lcd.setCursor(0, 1);
        lcd.print("AP>3 & RST>10sec");
       // digitalWrite(D4, HIGH);
        // Key debounce handling
        delay(100);
        int startTime = millis();
        while(digitalRead(I5) == HIGH) delay(50);
        int endTime = millis();

        if ((endTime - startTime) > 10000) {
          // If key pressed for more than 10secs, reset all
          Serial.printf("Reset to factory.\n");
          frestore_2();
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("     DEVICE");
          lcd.setCursor(0, 1);
          lcd.print("FRS-Completed");
          delay(2000);
          ESP.restart();
         // RMakerFactoryReset(2);
        } else if ((endTime - startTime) > 3000) {
          Serial.printf("AP-MODE\n");
          // If key pressed for more than 3secs, but less than 10, reset Wi-Fi
          //RMakerWiFiReset(2);
          AP();
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("     DEVICE");
          lcd.setCursor(0, 1);
          lcd.print("Swtiched to AP ");//delay for to protect connection going to ap mode 
          delay(2000);
        }
    }
if ((WiFi.status() != WL_CONNECTED)&&(wifi_flag==1)) {
      lcd.setCursor(0,0);
      lcd.print("Check your WiFi..");
      lcd.setCursor(15,1);
      lcd.print("N");   
      WiFi.mode(WIFI_STA);
      ConfigureWifi();
      digitalWrite(D0, LOW);
      delay(2000);
      digitalWrite(D0, HIGH);
      alarm3();
  }
int8_t ret;

  mqtt.disconnect();

  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0&&(WiFi.status() == WL_CONNECTED)) // connect will return 0 for connected//ada wifi not found
  {
    Serial.println(mqtt.connectErrorString(ret));

    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
   // delay(200);  // wait 5 seconds
   // retries--;
  //  if (retries == 0)
   // {
    //  ESP.reset();
   // }
     // digitalWrite(D0, LOW);
      lcd.clear(); 
      lcd.setCursor(0, 1);
      lcd.print("NETWORK NOTFOUND");
      //delay(500);
     // digitalWrite(D0, HIGH);
      alarm2();
      return;//ada wifi-NETWORK not found
  }
  Serial.println("Adafruit MQTT Connected!");
  lcd.clear(); 
  lcd.setCursor(0, 1);
  lcd.print("DEVICE CONNECTED");
  REPLY.publish("DEVICE CONNECTED");
  digitalWrite(D0, HIGH);
  CH1.publish("0");
  
}

  //AUTO WIFI-ROUTER CONNECT
void initWiFi() {
  WiFi.mode(WIFI_STA);
  ConfigureWifi();
  delay(2000);//for ap2ssid
 /* WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname(hostname);*/
  Serial.print("Connecting to WiFi ..");
  lcd.setCursor(0, 1);
  lcd.print("Searching Router");
  while (WiFi.status() != WL_CONNECTED) {
    //Serial.print('.');
    alarm3();
    delay(3000);
    WiFi.mode(WIFI_STA);
    ConfigureWifi();
    delay(2000);//for ap2ssid
  }
void Task1code( void * pvParameters ){
 
 // Serial.print("Task1 running on core ");
//  Serial.println(xPortGetCoreID());
  for(;;){
    server.handleClient(); //webserver in core2
    UDP();
    webSocket.loop();
if (Serial2.available() > 0)
   {
    
   //Serial.write(Serial2.read());
if (GPS_RL.encode(Serial2.read())){
  
     post_ada();
  
   }
   }
    delay(1);
 //  Serial.print("Task1 running on core ");
 // Serial.println(xPortGetCoreID());
  } 

  }

//---------------------------DUC
//---------------ADA MQTT PING
void PING()
{
   unsigned long PING_currentMillis = millis();
 if( PING_FLAG ==1){
 PING_previousMillis = PING_currentMillis;
  PING_FLAG =0;
 }
    if (PING_currentMillis - PING_previousMillis>= PING_interval) {
        PING_previousMillis = PING_currentMillis;
      /* if (mqtt.ping())
       {
        Serial.println("ping to ada_mqttt");
       }*/
        mqtt.disconnect();

        PING_FLAG = 1;
        Serial.println("ping to mqttt");
        lcd.clear();
 }
}
//---------------------GPS
void displayInfo()
{
  //Serial.print(F("Location: ")); 
  if (GPS_RL.location.isValid())
  {
   /* Serial.print(GPS_RL.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(GPS_RL.location.lng(), 6);*/
     LAT = String(GPS_RL.location.lat(), 6); // int to string
     LON = String(GPS_RL.location.lng(), 6); // int to string
     LAT_1 = String(GPS_RL.location.lat(), 4); // int to string
     LON_1= String(GPS_RL.location.lng(), 4); // int to string
   // Serial.println(LAT.c_str());
    addada14= "23,"+LAT+","+LON+",0";
    addada16="https://www.google.com/maps?q="+LAT_1+","+LON_1;
   // Serial.println(addada14.c_str());
    GPS.publish(addada14.c_str());
    client.publish(addGPS.c_str(),addada14.c_str());
  }
  else
  {
  
   // Serial.print(F("INVALID"));
  }
   /* digitalWrite(D4, HIGH); //buzzer
    delay(1000);
    digitalWrite(D4, LOW);*/
}
//---------------------GPS
//-----------------------ultrasonic
//-----------------------ultrasonic
      void alarm3()
  {
    if(config.LED_G == true)
    {
          digitalWrite(D0, HIGH);
          digitalWrite(D4, HIGH);
          delay(100);
          digitalWrite(D0, LOW);
          digitalWrite(D4, LOW);
    }
    else{
      delay(100);
    }
    }
    //---------------alarm2 for netwoek not found-----------
          void alarm2()
  {
    if(config.LED_G == true)
    {
          digitalWrite(D0, HIGH);
          digitalWrite(D4, HIGH);
          delay(500);
          digitalWrite(D0, LOW);
          digitalWrite(D4, LOW);
    }
    else{
          digitalWrite(D0, HIGH);
          delay(500);
          digitalWrite(D0, LOW);
          }
    }
void UDP()
{
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
     
      udpip = Udp.remoteIP().toString().c_str(); 
      udpport = Udp.remotePort();//reciving udp ip storage
    //  Serial.printf("Received %d bytes from %s, port %d\n", packetSize, udpip.c_str(), udpport);----
    
     // Serial.println("AWSOME");-------
      
    // receive incoming UDP packets
   IPAddress myIP = Udp.remoteIP();//super easy way to segrigate ip in byte array
      WriteConfig();
      config.udpIP[0]= myIP[0];
      config.udpIP[1]= myIP[1];
      config.udpIP[2]= myIP[2];
      config.udpIP[3]= myIP[3];
      config.udpPort = udpport;
      EEPROM.write(432,config.udpIP[0]);
      EEPROM.write(433,config.udpIP[1]);
      EEPROM.write(434,config.udpIP[2]);
      EEPROM.write(435,config.udpIP[3]);
      WriteStringToEEPROM(436,config.udpPort);
      EEPROM.commit();
    //Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());//----
   // Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);//---------
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
    Serial.printf("Received Message: %s\n", incomingPacket);
void G_P_S()
{
  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();

  JSONencoder["lat"] = LAT.c_str();
  JSONencoder["lon"] = LON.c_str(); 
  int lenghtSimple = JSONencoder.measureLength();
  char JSONmessageBuffer[300];
  JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  client.publish(addCH3.c_str(), JSONmessageBuffer);
 // JSONencoder.printTo(Serial);
 
}
//-----------------------------------------------------------https://www.google.com/maps?q=11.0168,76.9558
void G_P_S_url()
{
  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();

  JSONencoder["LOC"] = addada16.c_str();
  //JSONencoder["lon"] = LON.c_str(); 
  int lenghtSimple = JSONencoder.measureLength();
  char JSONmessageBuffer[300];
  JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  client.publish(addCH3.c_str(), JSONmessageBuffer);
 // JSONencoder.printTo(Serial);
 
}

void G_P_S_TTS()
{
  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();
  JSONencoder["lat"] = LAT_1.c_str();
  JSONencoder["lon"] = LON_1.c_str(); 
  char JSONmessageBuffer[300];
  JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
 // client.publish(addREPLY.c_str(), JSONmessageBuffer);
 // pubsubssl_client.publish(addREPLY.c_str(), JSONmessageBuffer);
 // REPLY.publish(JSONmessageBuffer);//ada
 // ReadConfig();
 // IPAddress SendIP(config.udpIP[0],config.udpIP[1],config.udpIP[2],config.udpIP[3]); //UDP Broadcast IP data sent to all devicess on same network;//for node_red
 // Udp.beginPacket(SendIP, config.udpPort.toInt());//node_red_broadcast data
     Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
     // Udp.write(replyPacekt1,50);
        JSONencoder.printTo(Udp);
        Udp.endPacket();
        // ttsflag=0;
 // JSONencoder.printTo(Serial); 
 // Serial.println();   
 
}
void USEN_TTS()
{
  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();
  JSONencoder["obstacle"] = obs;
   char JSONmessageBuffer[300];
  JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
 // client.publish(addREPLY.c_str(), JSONmessageBuffer);
 // pubsubssl_client.publish(addREPLY.c_str(), JSONmessageBuffer);
 // REPLY.publish(JSONmessageBuffer);//ada
 // ReadConfig();
 // IPAddress SendIP(config.udpIP[0],config.udpIP[1],config.udpIP[2],config.udpIP[3]); //UDP Broadcast IP data sent to all devicess on same network;//for node_red
 // Udp.beginPacket(SendIP, config.udpPort.toInt());//node_red_broadcast data
     Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
     // Udp.write(replyPacekt1,50);
        JSONencoder.printTo(Udp);
        Udp.endPacket();
        // ttsflag=0;
 // JSONencoder.printTo(Serial); 
 // Serial.println();   
 
}

void post_ada()
{

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
  previousMillis = currentMillis;
     // U_SEN_1();
      displayInfo(); 
      G_P_S_url();
    }
 
}
void U_SEN_1()
{
   digitalWrite(trigPin_1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin_1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_1, LOW);

  duration_1 = pulseIn(echoPin_1, HIGH);
  distanceCm_1 = duration_1 * SOUND_SPEED/2;
  //obs = distanceCm_1;
  Serial.print("Distance_1 (cm): ");
  Serial.println(distanceCm_1);
 
  if((distanceCm_1 > 2)&&(distanceCm_1 <30))
  {
  obs = distanceCm_1;
  ttsflag = 3;
  digitalWrite(D4, HIGH); //buzzer
  delay(100);
  digitalWrite(D4, LOW);
 // JSONencoder.printTo(Serial); 
 // Serial.println();   
     //delay(1000);
  }
}
