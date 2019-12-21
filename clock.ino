#include <ESP8266WiFi.h>   //wifi模组库文件
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>   //json处理文件
#include <NTPClient.h>


#include "SSD1306.h" 
SSD1306  display(0x3c, 5, 4); 


#include <WiFiUdp.h>
#include <PubSubClient.h> //MQTT库文件


#define LED     D4



#define wifi_ssid ""     //WIFI 账号
#define wifi_password ""//WIFI 密码
#define mqtt_server ""  
#define mqtt_user ""   //mqtt账号 
#define mqtt_password "" //mqtt密码 

#define work_topic "sensor/work"   //定义工作主题
String time1,time2;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp1.aliyun.com",60*60*8,30*60*1000);
WiFiClient espClient;
PubSubClient client(espClient);

int open_flag = 0;
long lastMsg = 0;
int value = 0;

String  msg ;
String payload = "";              //获取数据储存变量
//String ding_time, nowtime, nowtime2;

char  c[] = "";


void callback(char* topic, byte* payload, unsigned int length)  //接收消息
{
 
  Serial.print(topic);

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    msg = msg + (char)payload[i];
  }
  Serial.println(topic);
  Serial.println(msg);

  
  

   
}
String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}
void setup_wifi() {     //wifi链接初始化
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
//  display.setFont(ArialMT_Plain_10);
//  display.drawString(0, 0, wifi_ssid);
//  display.display();
  Serial.println(wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
//    display.drawString(0, 10, "Wait wifi connected");
//    display.display();
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void reconnect() {     //设备重连
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientName;
    clientName += "esp8266-";
    uint8_t mac[6];
    WiFi.macAddress(mac);
    clientName += macToStr(mac);
    clientName += "-";
    clientName += String(micros() & 0xff, 16);
    Serial.print("Connecting to ");
    Serial.print(mqtt_server);
    Serial.print(" as ");
    Serial.println(clientName);
    if (client.connect((char*) clientName.c_str())) {
      //if (client.connect((char*) clientName.c_str()), mqtt_user, mqtt_password)) {
      Serial.println("connected");
    
      client.subscribe(work_topic);//订阅工作主题来接收指令
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
 
  pinMode(LED, OUTPUT);    
  timeClient.begin();

  setup_wifi();
  client.setServer(mqtt_server, 1883); //链接MQTT服务器
  client.setCallback(callback);//数据回调

  display.init(); // Initialising the UI will init the display too.
  display.flipScreenVertically();
}
void drawclock() 
{
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, msg);
    display.setFont(ArialMT_Plain_24);
    display.drawString(0, 26, "Date:"+time1);
}
void loop() {
 

    timeClient.update();
    time1=timeClient.getFormattedTime();
    time2=time1.substring(0,5);
    display.clear();
    drawclock() ; 
    display.display();
    delay (2000);
    Serial.println(msg);
    int len;
    len=msg.length();
    Serial.println(len);
    msg=msg.substring(len-5,len);
   Serial.println(msg);
      if(!time2.compareTo(msg))
     {
       Serial.println("ON");Serial.print("LOW");
        digitalWrite(LED,LOW);
      }
      else {
        Serial.println("high");
      digitalWrite(LED,HIGH);  // Turn the LED off by making the voltage HIGH
       }
       
    

   
  
  
  if (!client.connected()) {  //如果掉线则重连
    reconnect();
  }  
  client.loop();
  long now = millis();
  
 
}
