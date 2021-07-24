#include <WiFiNINA.h>
#include <Servo.h>;
#include <string.h>;

char ssid[] = "Apt2f";
char pass[] = "comiccon213";

unsigned long commandTimer;
unsigned long stateTimer;

const int commandLoopTime = 15000;
const int stateLoopTime = 30*60*60*1000;

int HTTP_PORT =80;
String HTTP_METHOD = "GET";
char HOST_NAME[] = "auto-stove.herokuapp.com";

int status = WL_IDLE_STATUS;
int SERVO_PIN = 9;
int HALL_EFFECT_PIN=2;
Servo servo;
int prevState;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  Serial.begin(9600);
  delay(100);
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
  //server.begin();

  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(HALL_EFFECT_PIN, INPUT);
  
  servo.attach(SERVO_PIN);
  prevState = digitalRead(HALL_EFFECT_PIN);
  unsigned long temp = millis();
  postState(prevState);
  Serial.println(millis()-temp);
  Serial.println("time for state post");
  commandTimer = millis();
  stateTimer = millis();

}

void loop() {
  digitalWrite(LED_BUILTIN, LOW);
  commandLoop();
  stateLoop();

}

void stateLoop(){
  int currentState = digitalRead(HALL_EFFECT_PIN);
  if(millis() - stateTimer >= stateLoopTime){
    //unsigned long temp = millis();
    postState(currentState);
    
    Serial.println("reposting state 30s");
  }
  if (prevState != currentState){
    unsigned long temp = millis();
    postState(currentState);
    Serial.println(millis()-temp);
    prevState = currentState;
    Serial.println("state changed");
  }
}

void commandLoop(){
  String cmd = "";
  if(millis() - commandTimer >= commandLoopTime){
    unsigned long temp = millis();
    cmd = getCommand();
    Serial.println(millis()-temp);
    Serial.println("time to get command");
    cmd.trim();
    //Serial.println(cmd.compareTo("turnOff"));
    //Serial.println(cmd.substring(0,6));
    commandTimer = millis();
  }
 // Serial.println(cmd);

  if (!cmd.compareTo("turnOff")){
    Serial.println("here");
    turnOff();
    postTurnedOff();
  }
}

void turnOff(){
  int state = digitalRead(HALL_EFFECT_PIN);
  while (state == HIGH){
    servo.write(75);
    state = digitalRead(HALL_EFFECT_PIN);
    digitalWrite(LED_BUILTIN, HIGH);
  }
  digitalWrite(LED_BUILTIN, LOW);
  servo.write(90);
}

void postTurnedOff(){
  String PATH_NAME = "/turnOff/?command=completed";
  WiFiClient client;
  if(client.connect(HOST_NAME, HTTP_PORT)){
    Serial.println("Connected to AutoStove to notify completion");
    client.println(HTTP_METHOD +" " + PATH_NAME + " HTTP/1.1");
    client.println("Host: " + String(HOST_NAME));
    client.println("Connection: close");
    client.println();
    while(client.connected()){
      //String line = client.readStringUntil('\n');
      
      if(client.available()){
        char c = client.read();
        Serial.print(c);
      }
    }
    client.stop();
    Serial.println();
    Serial.println("disconnected");
  } else {
    Serial.println("Connection failed");
  }
}

void postState(int state){
  String PATH_NAME = "/postState/?state=" + String(state);
  WiFiClient client;
  if(client.connect(HOST_NAME, HTTP_PORT)){
    Serial.println("Connected to AutoStove");
    client.println(HTTP_METHOD +" " + PATH_NAME + " HTTP/1.1");
    client.println("Host: " + String(HOST_NAME));
    client.println("Connection: close");
    client.println();
    while(client.connected()){
      //String line = client.readStringUntil('\n');
      
      if(client.available()){
        char c = client.read();
        Serial.print(c);
      }
    }
    client.stop();
    Serial.println();
    Serial.println("disconnected");
  } else {
    Serial.println("Connection failed");
  }
}

String getCommand(){
  String PATH_NAME = "/getCommand/";
  WiFiClient client;
  if(client.connect(HOST_NAME, HTTP_PORT)){
    Serial.println("Connected to AutoStove Server for Commands");
    client.println(HTTP_METHOD+" "+PATH_NAME+" HTTP/1.1");
    client.println("Host: " + String(HOST_NAME));
    client.println("Connection: close");
    client.println();
    while(client.connected()){
      String line = client.readStringUntil('\n');
      if (line == "\r"){
        break;
      }
//      if(client.available()){
//        char c = client.read();
//        Serial.print(c);
//      }
    }
    String op;
    while (client.available()){
      op += (char)client.read();
    }
    Serial.println(op);
    client.stop();
    Serial.println();
    Serial.println("disconnected");
    return op;
  } else{
    Serial.println("Connection failed");
    return "No Command";
  }
}
