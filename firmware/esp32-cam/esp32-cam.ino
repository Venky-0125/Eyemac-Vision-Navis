#include "esp_camera.h"
#include "SPI.h"
#include "driver/rtc_io.h"
#include "ESP32_MailClient.h"
#include <FS.h>
#include <SPIFFS.h>
#include <WiFi.h>

//
// WARNING!!! Make sure that you have either selected ESP32 Wrover Module,
//            or another board which has PSRAM enabled
//

// Select camera model
//#define CAMERA_MODEL_WROVER_KIT
//#define CAMERA_MODEL_ESP_EYE
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE
#define CAMERA_MODEL_AI_THINKER
//#define ENABLE_FACE_DETECTION 1 //-----------AUTO FACE RECOGANATION
//#define ENABLE_FACE_RECOGNITION 1
#define Relay 2
#define PIR 14
#define Red 13
#define Green 12
#include "camera_pins.h"
// Wi-Fi, SMTP and recipient settings live in secrets.h (NOT committed).
// Copy secrets.h.example -> secrets.h and fill in your own values.
#include "secrets.h"
#define smtpServerPort        465
#define emailSubject          "EYEMAC VISION NAVIS - Emergency image captured"

// Photo File Name to save in SPIFFS
#define FILE_PHOTO "/photo.jpg"

void startCameraServer();

boolean matchFace = false;
boolean activateRelay = false;
long prevMillis=0;
int interval = 5000;

boolean EactivateRelay = false;
long EprevMillis=0;
int Einterval = 40000;

String addip;
int flag = 0;
int i=0;
String  serialstr;
int eflag=0;
int cam_flag = 0;
int ip_flag = 0;
void setup() {
   WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  pinMode(Relay,OUTPUT);
  pinMode(PIR,INPUT_PULLUP);
  pinMode(Red,OUTPUT);
  pinMode(Green,OUTPUT);
 // digitalWrite(Relay,LOW);
  digitalWrite(Red,HIGH);
  digitalWrite(Green,LOW);
  delay(15000);
  Serial.begin(9600);
  Serial.setDebugOutput(true);
  Serial.println();

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  //initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);//flip it back
    s->set_brightness(s, 1);//up the blightness just a bit
    s->set_saturation(s, -2);//lower the saturation
  }
  //drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);

#if defined(CAMERA_MODEL_M5STACK_WIDE)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

 WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);
/*  if(WiFi.status() != WL_CONNECTED) {
    // delay(5000);
    initWiFi();
  }*/
  Serial.println("");
  Serial.println("$LCD$CAM READY...");
  if(WiFi.status() == WL_CONNECTED) {
    startCameraServer(); 
  addip= "$LCD$"+WiFi.localIP();
  Serial.print("$LCD$");
  Serial.print(WiFi.localIP());
  Serial.println(addip);
}
if (!SPIFFS.begin(true)) {
    //Serial.println("An Error has occurred while mounting SPIFFS");
    Serial.println("$LCD$PHOTO MEM ERR");
    ESP.restart();
  }
  else {
    delay(500);
    //Serial.println("SPIFFS mounted successfully");
    Serial.println("$LCD$PHOTO MEM RDY..");
  }
startCameraServer();
}

void loop() {
 /* if(WiFi.status() != WL_CONNECTED) {
    // delay(5000);
    initWiFi();
    cam_flag=0;
  }
  if((cam_flag ==0) && (WiFi.status() == WL_CONNECTED))
  {
    startCameraServer();
    cam_flag=1;
  }*/
   //WiFi.mode(WIFI_STA);
   if(WiFi.status() == WL_CONNECTED) {
    digitalWrite(Green,HIGH);
   }
   if(WiFi.status() != WL_CONNECTED) {
    digitalWrite(Green,LOW);
   }
   digitalWrite(Red,LOW);
/* if((flag == 0)||(flag==1)) {

  i++;
  if(i==500)
  {
   Serial.printf("$LCD$ %s\n",WiFi.localIP().toString().c_str());
    flag=1;
  }
  if(i==5000)
  {
  Serial.printf("$LCD$ %s\n",WiFi.localIP().toString().c_str());
    flag=2; 
  }
  }*/
      if (Serial.available() > 0){
      serialstr = Serial.readString();
      if (serialstr.startsWith("$GETIP$"))
      {
         
      Serial.printf("$LCD$ %s\n",WiFi.localIP().toString().c_str());
 
      }
      }
//Serial.print(WiFi.localIP());
  if(matchFace==true && activateRelay==false)
  {
    activateRelay=true;
    //digitalWrite(Relay,HIGH);
    digitalWrite(Green,HIGH);
    digitalWrite(Red,LOW);
    prevMillis=millis();
    }
    if (activateRelay == true && millis()-prevMillis > interval)
    {
      activateRelay=false;
      matchFace=false;
     // digitalWrite(Relay,LOW);
      digitalWrite(Green,LOW);
      digitalWrite(Red,HIGH);
      }    
    if(matchFace==false && EactivateRelay==false && eflag==1)
    {
        digitalWrite(Relay,HIGH);
        EactivateRelay=true;
        EprevMillis=millis();
        eflag=0;
       capturePhotoSaveSpiffs();
        sendPhoto();
        digitalWrite(Relay,LOW);
      //  EactivateRelay==true;
      //  EprevMillis=millis();
       //Serial.println("matchFace==false" );
        } 
     if (EactivateRelay == true && millis()-EprevMillis > Einterval)
    {
      EactivateRelay=false;
    // Serial.println("EprevMilli" );
         }
        // if ((digitalRead (Relay) == LOW && EactivateRelay==false)||(digitalRead (PIR) == LOW && EactivateRelay==false))
        if (digitalRead (PIR) == LOW && EactivateRelay==false)
         {
         digitalWrite(Relay,HIGH); 
          EactivateRelay=true;
        EprevMillis=millis();
       // eflag=0;
      capturePhotoSaveSpiffs();
      sendPhoto(); 
      digitalWrite(Relay,LOW);
         }
      /*   if (digitalRead (Relay) == LOW && ip_flag == 0)
         {
          ip_flag = 1;
          Serial.printf("$CAM$ %s\n",WiFi.localIP().toString().c_str());
          delay(2000);
         }
          if (digitalRead (Relay) == HIGH && ip_flag == 1)
         {
          ip_flag = 0;
         // Serial.printf("$CAM$ %s\n",WiFi.localIP().toString().c_str());
         }*/

}

// Check if photo capture was successful
bool checkPhoto( fs::FS &fs ) {
  File f_pic = fs.open( FILE_PHOTO );
  unsigned int pic_sz = f_pic.size();
  return ( pic_sz > 100 );
}

// Capture Photo and Save it to SPIFFS
void capturePhotoSaveSpiffs( void ) {
  camera_fb_t * fb = NULL; // pointer
  bool ok = 0; // Boolean indicating if the picture has been taken correctly

  do {
    // Take a photo with the camera
    Serial.println("Taking a photo...");
  //  Serial.println("$LCD$TAKING PHOTO");

    fb = esp_camera_fb_get();
    if (!fb) {
      //Serial.println("Camera capture failed");
      Serial.println("$LCD$CAPTURE FAILED");
      return;
    }

    // Photo file name
    //Serial.printf("Picture file name: %s\n", FILE_PHOTO);
    File file = SPIFFS.open(FILE_PHOTO, FILE_WRITE);

    // Insert the data in the photo file
    if (!file) {
      Serial.println("Failed to open file in writing mode");
    }
    else {
      file.write(fb->buf, fb->len); // payload (image), payload length
      Serial.print("The picture has been saved in ");
      Serial.print(FILE_PHOTO);
      Serial.print(" - Size: ");
      Serial.print(file.size());
      Serial.println(" bytes");
     // Serial.println("$LCD$PHOTO SAVED");
    }
    // Close the file
    file.close();
    esp_camera_fb_return(fb);

    // check if file has been correctly saved in SPIFFS
    ok = checkPhoto(SPIFFS);
  } while ( !ok );
}

void sendPhoto( void ) {
  // Preparing email
  Serial.println("$LCD$Sending email...");
  // Set the SMTP Server Email host, port, account and password
  smtpData.setLogin(smtpServer, smtpServerPort, emailSenderAccount, emailSenderPassword);
  
  // Set the sender name and Email
  smtpData.setSender("ESP32-CAM", emailSenderAccount);
  
  // Set Email priority or importance High, Normal, Low or 1 to 5 (1 is highest)
  smtpData.setPriority("High");

  // Set the subject
  smtpData.setSubject(emailSubject);
    
  // Set the email message in HTML format
  smtpData.setMessage("<h2>EYEMAC VISION NAVIS - emergency image captured.</h2>", true);
  smtpData.addRecipient(emailRecipient3);
  smtpData.addRecipient(emailRecipient4);

  // Add attach files from SPIFFS
  smtpData.addAttachFile(FILE_PHOTO, "image/jpg");
  // Set the storage type to attach files in your email (SPIFFS)
  smtpData.setFileStorageType(MailClientStorageType::SPIFFS);

  smtpData.setSendCallback(sendCallback);
  
  // Start sending Email, can be set callback function to track the status
  if (!MailClient.sendMail(smtpData))
    Serial.println("Error sending Email, " + MailClient.smtpErrorReason());
   Serial.println("$LCD$" + MailClient.smtpErrorReason());

  // Clear all data from Email object to free memory
  smtpData.empty(); 
}

// Callback function to get the Email sending status
void sendCallback(SendStatus msg) {
  //Print the current status
  Serial.println("$LCD$" + msg.info());
  digitalWrite(Red,HIGH);
}
void ConfigureWifi()
{
 WiFi.begin(ssid, password);
}
void initWiFi() {
  WiFi.mode(WIFI_STA);
  ConfigureWifi();
  Serial.print("Connecting to WiFi ..");
 // lcd.setCursor(0, 1);
 // lcd.print("Searching Router");
  while (WiFi.status() != WL_CONNECTED) {
    //Serial.print('.');
    delay(5000);
    WiFi.mode(WIFI_STA);
    ConfigureWifi();
  }
}

