#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

const uint8_t PIN_D5 = D5;
const uint8_t PIN_D6 = D6;
const uint8_t PIN_D7 = D7;
const uint8_t PIN_D8 = D8;

const char* WIFI_SSID = "IoT";
const char* WIFI_PASSWORD = "VeryL0ngPas$wd!2015";
const int WIFI_NUM_OF_RETRIES = 20;
const uint32_t NETWORK_ERROR_RECOVERY_DELAY = 600000000;

const uint8_t HTTP_REST_PORT = 80;

String deviceType;
String deviceId;
String deviceTags;
String baseUrl;
ESP8266WebServer httpRestServer(HTTP_REST_PORT);

void initWiFi() {
  Serial.println();
  Serial.println("WiFi connect");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int retriesCount = 0;
  while (WiFi.status() != WL_CONNECTED) {
    retriesCount++;
    if (retriesCount > WIFI_NUM_OF_RETRIES) {
      Serial.println();
      Serial.print("Max number of retries for WiFi exceeded ");
      Serial.println(WIFI_NUM_OF_RETRIES);
      Serial.print("Sleeping for ");
      Serial.println(NETWORK_ERROR_RECOVERY_DELAY);
      ESP.deepSleep(NETWORK_ERROR_RECOVERY_DELAY);
    }
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("WiFi connected. IP ");
  Serial.println(WiFi.localIP());  
}

String createResourceUrl(String resourceName) {
  return baseUrl + "/" + resourceName;
}

String createStateResponseBody() {
   String resp = "{\"deviceType\":\"" + deviceType
    + "\", \"deviceId\":\"" + deviceId
    + "\", \"deviceTags\": [" + deviceTags + "]"
    + ", \"plug1\":" + digitalRead(PIN_D5)
    + ", \"plug2\":" + digitalRead(PIN_D6)
    + ", \"plug3\":" + digitalRead(PIN_D7)
    + ", \"plug4\":" + digitalRead(PIN_D8)
    + "}";
  Serial.println("Resp: " + resp + "\n");
  return resp;
}

void getDeviceState() {
    httpRestServer.send(200, F("application/json"), createStateResponseBody());
}

void updateDeviceState() {
  String postBody = httpRestServer.arg("plain");
  Serial.println(postBody);
  StaticJsonDocument<100> jsonDoc;
  DeserializationError error = deserializeJson(jsonDoc, postBody);
  if (error) {
    String errorMsg = error.c_str();
    Serial.println(errorMsg);
    String errorResp = "{\"errorCode\":\"0001\", \"errorMsg\": \"" + errorMsg + "\"}";
    httpRestServer.send(400, F("application/json"), errorResp);
  } else {
    JsonObject postObj = jsonDoc.as<JsonObject>();
      if (postObj.containsKey(F("plug1"))) {
        uint8_t pinUpdatedState = postObj[F("plug1")];
        digitalWrite(PIN_D5, pinUpdatedState);
      }        
      if (postObj.containsKey(F("plug2"))) {
        uint8_t pinUpdatedState = postObj[F("plug2")];
        digitalWrite(PIN_D6, pinUpdatedState);
      }
      if (postObj.containsKey(F("plug3"))) {
        uint8_t pinUpdatedState = postObj[F("plug3")];
        digitalWrite(PIN_D7, pinUpdatedState);
      }
      if (postObj.containsKey(F("plug4"))) {
        uint8_t pinUpdatedState = postObj[F("plug4")];
        digitalWrite(PIN_D8, pinUpdatedState);
      }
      httpRestServer.send(200, F("application/json"), createStateResponseBody());
  }      
}

void restServerRouting() {    
  httpRestServer.on(createResourceUrl("state"), HTTP_GET, getDeviceState);
  httpRestServer.on(createResourceUrl("state"), HTTP_POST, updateDeviceState);
}

void setup() {
    // put your setup code here, to run once:
  Serial.begin(115200);  
  deviceType = "switch";
  deviceId = "0001";
  deviceTags = "\"SRD-05VDC-SL-C\"";
  baseUrl = "/cg/actuator/" + deviceType + "/" + deviceId;
  pinMode(PIN_D5, OUTPUT);
  pinMode(PIN_D6, OUTPUT);
  pinMode(PIN_D7, OUTPUT);
  pinMode(PIN_D8, OUTPUT);
  restServerRouting();
  httpRestServer.begin();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    initWiFi();
  }
  httpRestServer.handleClient();
      // put your main code here, to run repeatedly:
    // digitalWrite(PIN_D5, HIGH);
    // delay(1000);
    // digitalWrite(PIN_D5, LOW);
    // delay(1000);
    // digitalWrite(PIN_D6, HIGH);
    // delay(1000);
    // digitalWrite(PIN_D6, LOW);
    // delay(1000);
    // digitalWrite(PIN_D7, HIGH);
    // delay(1000);
    // digitalWrite(PIN_D7, LOW);
    // delay(1000);
    // digitalWrite(PIN_D8, HIGH);
    // delay(1000);
    // digitalWrite(PIN_D8, LOW);
    // delay(1000);
}