#include <ESP8266WiFi.h>   //wifi模组库文件
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>   //json处理文件

#include <PubSubClient.h> //MQTT库文件
#include "DHT.h"           //温湿度库文件
#define DHTPIN 12 // 定义温湿度引脚 (D6)
#define LED     D4
#define DHTTYPE DHT11  //实例化温湿度传感器

DHT dht(DHTPIN, DHTTYPE);
#define wifi_ssid "cai"     //WIFI 账号
#define wifi_password "542654..."//WIFI 密码
#define mqtt_server "182.92.170.109"  
#define mqtt_user "admin"   //mqtt账号 
#define mqtt_password "542654..." //mqtt密码 
#define humidity_topic "sensor/humidity" //定义湿度主题
#define temperature_celsius_topic "sensor/temperature_celsius"//定义温度主题
#define work_topic "sensor/work"   //定义工作主题

WiFiClient espClient;
PubSubClient client(espClient);

int open_flag = 0;
long lastMsg = 0;
int value = 0;
float h, t;
String  msg ;
String payload = "";              //获取数据储存变量
//String ding_time, nowtime, nowtime2;

char  c[] = "";


void callback(char* topic, byte* payload, unsigned int length)  //接收消息
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    msg = msg + (char)payload[i];
  }
  Serial.println(topic);
  Serial.println(msg);

 
  if (String(topic) == "sensor/work" )  
  {
     if (msg == "0") {
    digitalWrite(LED, HIGH);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)  低电平有效
     } 
    else {
    digitalWrite(LED, LOW);  // Turn the LED off by making the voltage HIGH
     }
  }

   
  msg = "";
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
  

  
  dht.begin();
 
  setup_wifi();
  client.setServer(mqtt_server, 1883); //链接MQTT服务器
  client.setCallback(callback);//数据回调
}
void loop() {
 // float h = dht.readHumidity();  //获取湿度数据
  float t = dht.readTemperature(); //获取温度数据
 int h = random(1,50);  //获取湿度数据
  


  
  if (!client.connected()) {  //如果掉线则重连
    reconnect();
  }  
  client.loop();
  long now = millis();
  if (now - lastMsg > 2000) {   //没两秒执行一次
    lastMsg = now;
    ++value;

    client.publish(temperature_celsius_topic, String(t).c_str(), true); //上传温度数据
    delay(100);
    client.publish(humidity_topic, String(h).c_str(), true); //上传湿度数据
    delay(100);
  
 
  }
 
}
