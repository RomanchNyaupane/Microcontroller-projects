#include <Arduino.h>
#include <string.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

#define SSID "nwaran_vako_xaina"
#define PASSWORD "12345678ku"
#define WEB_API "AIzaSyBeMyr5JKwJYGv2rWnIR0tgtqQj1FMLQ7w"
#define Data_base_url "https://ecothon-11b92-default-rtdb.asia-southeast1.firebasedatabase.app/"

int read_pin_1 = 34;  //voltage
int read_pin_2 = 35;   //voltage
int read_pin_3 = 32;   //current
int read_pin_4 = 33;  //current
int Relay1 = 13;
int Relay2 = 12;
int sensitivity_cur = 66; //current parameter
int offsetVoltage = 2463; //current parameters
int adcVoltage_cur_1,adcVoltage_cur_2,adc_val_1,adc_val_2; //current parameters
float volt_1,volt_2, cur_1,cur_2;

bool Sign_Up = true;
unsigned long send_Data_prev_millis= 0;

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void wifi_init(){
  WiFi.begin(SSID,PASSWORD);
  Serial.println("Conecting to wifi.");

  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected with IP :");
  Serial.print(WiFi.localIP());
  Serial.println();
}
void Firebase_Config(){
  config.api_key= WEB_API ;
  config.database_url = Data_base_url;  
  if (Firebase.signUp (&config,&auth, "","")){
    Serial.println("Successfull connection to database");
    Sign_Up = true;
  }
  else {
    Serial.printf("%s\n",config.signer.signupError.message.c_str());
  }
  config.token_status_callback  = tokenStatusCallback;
  Firebase.begin(&config,&auth);
  Firebase.reconnectWiFi(true);
}
void firebase_transmit (float data , const char* Path, const char*value){
  if (Firebase.RTDB.setFloat(&fbdo, Path ,data)){
      Serial.println();
      Serial.print(data);
      Serial.print(" ");
      Serial.print(value);
      Serial.print(" -> Data Sent Successfull");    
    }
    else{
      Serial.println();
      Serial.print("Data Sent Failed : ");
      Serial.print(fbdo.errorReason());
    }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(read_pin_1,INPUT);
  pinMode(read_pin_2,INPUT);
  pinMode(read_pin_3,INPUT);
  pinMode(read_pin_4,INPUT);
  Serial.begin(9600);
  wifi_init ();
  Firebase_Config();
}

void loop() {
//voltage
  volt_1 = analogRead(read_pin_1);  //house 1
  volt_1 = float(volt_1*5/1200);
  volt_2 = analogRead(read_pin_2);  //house 2
  volt_2 = float(volt_2*5/1200);
  Serial.print("House1 voltage: ");
  Serial.println(volt_1);
  Serial.print("House2 voltage: ");
  Serial.println(volt_2);

//current
  adc_val_1 = analogRead(read_pin_3); //house 1
  adc_val_2 = analogRead(read_pin_4); //house 2
  cur_1 = (adc_val_1-2960)/5; //2960 is read by sensor when no current flow is detected. j  paye tyai gareko. detect chai hunxa 2960 vanda mathi janxa detect vayesi.
  cur_2 = (adc_val_2-2960)/5; //2960 is read by sensor when no current flow is detected. detect vayesi current ko value j sukai aaos vanera jpt calculation haneko
  Serial.print("House 1 current: ");
  Serial.println(cur_1);
  Serial.print("House 2 current: ");
  Serial.println(cur_2);

//transmission
  if(Firebase.ready()&& Sign_Up && (millis() - send_Data_prev_millis > 5000 || send_Data_prev_millis == 0 )){
    send_Data_prev_millis = millis();
  firebase_transmit(volt_1, "SensorReading/voltage1" , "voltage1");
  firebase_transmit(volt_2, "SensorReading/voltage2" , "voltage2");
  firebase_transmit(cur_1, "SensorReading/current1" , "current1");
  firebase_transmit(cur_2, "SensorReading/current2" , "current2");
  }
}