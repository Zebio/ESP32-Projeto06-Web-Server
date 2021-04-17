#include <Arduino.h>
#include <WiFi.h>
#include <fstream>
#include <iostream>




/*---------------------Mapeamento de Hardware----------------------*/
#define led1 32
#define led2 33


/*---------------------Constantes de Projeto ----------------------*/
const char * ssid = "nome_da_rede";
const char * password = "senha_da_rede";

/*---------------------Variáveis GLobais    ----------------------*/
bool led1_status = false;
bool led2_status = false;

String header;


// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

/*---------------------Objetos-------------------------------------*/
WiFiServer server(80);


/*---------------------Declaração das Funções----------------------*/
void serverWifi();



void setup() {
  Serial.begin(115200);           //inicializa Serial em 115200 baud rate
  pinMode(led1, OUTPUT);          //configura saída para relé 1
  digitalWrite(led1,LOW);         //inicializa desligado
  pinMode(led2, OUTPUT);          //configura saída para relé 2
  digitalWrite(led2,LOW);         //inicializa desligado

  Serial.print("Conectando-se a ");      //
  Serial.println(ssid);                  //
  WiFi.begin(ssid, password);     //conecta-se a rede wireless

  while(WiFi.status() != WL_CONNECTED)   //Espera até a conexão bem-sucedida
  {
    delay(500);                          //
    Serial.print(".");                   //imprime os pontos no monitor
  }

  Serial.println("");                    //mostra WiFi conectada
  Serial.println("WiFi conectada");      //
  Serial.println("Endereço de IP: ");    //
  Serial.println(WiFi.localIP());        //mostra o endereço IP no monitor

  server.begin();                        //inicializa o servidor web
}

void loop() {
    while(WiFi.status() != WL_CONNECTED) //verifica se ainda está conectado
  {
    WiFi.begin(ssid, password);          //tenta conectar denovo
    Serial.print(".");                   

    delay(500);                         
  }
  serverWifi();                          //se estiver conectado iniciamos a função Server
}

void serverWifi()
{
  WiFiClient client = server.available(); 
  if (client) {                            
   Serial.println("Novo Cliente Conectado");        
    String currentLine = "";               
    while (client.connected()) {           
      if (client.available()) {            
        char c = client.read();            
        Serial.write(c);                   
        header += c;
        if (c == '\n') {                 
          if (currentLine.length() == 0||currentLine=="\n") {

          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println();

          if(header.indexOf("GET /led1") >= 0)
          {
            led1_status= !led1_status;
            digitalWrite(led1,led1_status);
            Serial.print("Recebeu Get L1: ");
            Serial.println(led1_status);
          }

          if(header.indexOf("GET /led2") >= 0)
          {
            led2_status= !led2_status;
            digitalWrite(led2,led2_status);
          }
            Serial.println("Imprimindo pagina HTML");
            client.println("<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><link rel=\"icon\" href=\"data:,\"></head>");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<title>Projeto05 - Automacao com ESP32</title>");
            client.println("<style> html{color: #ffffff;font-family: Verdana;text-align: center;background-color: #272727fd}.botao_on{color: #ffffff;padding: 15px 25px;font-size: 30px;margin: 2px;font-family: Tahoma;background-color: #313891a6;}.botao_off{color: #ffffff;padding: 15px 25px;font-size: 30px;margin: 2px;font-family: Tahoma;background-color: #7c7c7ca6;}</style>");
            client.println("<body><h2>Controle de Saidas</h2>");
            if (led1_status)
              client.println("<p><a href=\"/led1\"><button class=\"botao_on\" > LED 1   </button></a></p>");
            else 
              client.println("<p><a href=\"/led1\"><button class=\"botao_off\"> LED 1   </button></a></p>");
            if (led2_status)
              client.println("<p><a href=\"/led2\"><button class=\"botao_on\" > LED 2   </button></a></p>");
            else
              client.println("<p><a href=\"/led2\"><button class=\"botao_off\"> LED 2   </button></a></p>");
            
            client.println("</body></html>");
            client.println();
            break;
          }
          else currentLine = ""; //senão, impede string de fizer com espaços em branco
        }
        else if (c != '\r')  
          currentLine += c;      //adiciona caractere como parte da mensage
      }
    }
    header = "";                              //limpa header
    client.stop();                            //finaliza conexão
    Serial.println("Cliente desconectado"); 
    Serial.println("");  
  }
}
