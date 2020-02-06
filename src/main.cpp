//Ejemplo MQTT-WebSocket & ESP8266 con sensor PIR y módulo RELE 

//--------------Librerias---------------------------------------- 

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <stdlib.h>
 
//-------------------VARIABLES GLOBALES--------------------------
int contconexion = 0;

const char* ssid = "MQTT";
const char* password = "12345678";
const char* mqtt_server = "192.168.0.103";
const int mqttPort = 1883;
 
WiFiClient espClient;
PubSubClient client(espClient);

char RELE[50];
int Rele = 16;
int PIR = 5;
int ESTADO_PIR = LOW;

unsigned long previousMillis = 0;

//---------------------------  CALLBACK ------------------------------------------------

void callback(char* topic, byte* payload, unsigned int length) {
 char PAYLOAD[5] = "    ";
  
  Serial.print("Mensaje recibido: [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if (String(topic) ==  String(RELE)) 
  {
        if (payload[1] == 'F')
        {
         digitalWrite(Rele, HIGH);
        }
        
        if (payload[1] == 'N')
        {
          digitalWrite(Rele, LOW);
        }
  }


}

//------------------------------------------------------------------------------

void reconnect() {
  // Loop hasta que nos reconectemos
    while (!client.connected()) 
    {
        Serial.print("Conectando MQTT...");
              
        if (client.connect("IoT-ESP8266")) 
        {
          Serial.println("Conectado");
          client.subscribe(RELE);
          client.subscribe("inTopic");
        } 
        
        else 
        {
          Serial.print("Error de conexion");
          Serial.print(client.state());
          delay(2000);
        }
    }
}

//--------------------------------------------------------------------------------------

void setup() {
 
 //Rele 
  pinMode(Rele, OUTPUT);
  digitalWrite(Rele, HIGH);
 // sensor PIR 
  pinMode(PIR, INPUT);
  
  Serial.begin(115200);
 
  // Conexión WIFI
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED and contconexion <50) 
  { //Cuenta hasta 50 si no se puede conectar lo cancela
    ++contconexion;
    delay(500);
    Serial.print(".");
  }
      if (contconexion <50) 
    {
   
        Serial.println("");
        Serial.println("WiFi conectado");
        Serial.println(WiFi.localIP());
    }
  
      else 
      { 
          Serial.println("");
          Serial.println("Error de conexion");
      }
  
    
  client.setServer(mqtt_server, mqttPort);
  client.setCallback(callback);
  
  String IoTRele = "IoT/Rele"; 
  IoTRele.toCharArray(RELE, 50);

}
 
void loop() {

  if (!client.connected()) 
    {
      reconnect();
    }
  
  client.loop();

//------------------------------

    if(digitalRead(PIR) == HIGH)
      {
        if(ESTADO_PIR == LOW)
        {
          client.publish("IoT/Pir","ON");
          Serial.println("Movimiento detectado!");
          ESTADO_PIR = HIGH;  
        }         
            
      }

       else 
        {
           if(ESTADO_PIR == HIGH)
            {
              client.publish("IoT/Pir","OFF");
              Serial.println("No hay movimiento!");
              ESTADO_PIR = LOW;   
            }
        }

  

//------------------------------
}