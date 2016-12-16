#include <WiFi101.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiSSLClient.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <SPI.h>
#include <WiFi101.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

char ssid[] = "Rehan's iPhone";     //  your network SSID (name) 
char pass[] = "123bihari123";    // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

int keyIndex = 0;            //your network key Index number
char server[] = "http://ripped-stencil.000webhostapp.com";
WiFiClient client;

int getFingerprintIDez();

// Green is Rx and goes to 52
// White is Tx and goes to 51

SoftwareSerial mySerial(52, 51); // Rx, Tx

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup()  
{
  randomSeed(analogRead(0));
  Serial.begin(9600);
  mySerial.begin(9600);

  Serial.println("Adafruit finger detect test");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor 😞");
    while (1);
  }
  Serial.println("Waiting for valid finger...");
}

void loop()                     // run over and over again
{
  char outBuff[128];
  int x = getFingerprintIDez();
  if (x!=-1)
  {
    status = WiFi.begin(ssid, pass);
    //delay(10000);
    Serial.println("check1");
    
     if (client.connect(server, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    int hash = makehash();
    sprintf(outBuff, "POST /firebase.php?arduino=%d HTTP/1.1",hash);
    client.println(outBuff);
    client.println("Host: ripped-stencil.000webhostapp.com");
    client.println("Connection: close");
    client.println();
  }
  else {
    Serial.println("couldnt connect");
  }
  while(true)
  {
   if (client.available()) {
    char c = client.read();
    Serial.write(c);
    break;
   }
  }
    if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();
  }
  WiFi.end();
  }
}

int makehash() {
  int result = 0;
  for (int i =0 ;i < 4; i++) {
    long x = random(10);
    result = result*10 + (int)x;
  }
  Serial.println(result);
  return result;
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence); 
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {

  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID; 
}