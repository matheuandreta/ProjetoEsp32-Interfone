#include <SoftwareSerial.h>
#include <FPM.h>
#include <PushButton.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <WiFi.h> 
#include <PubSubClient.h>

//Display LCD_I2C
LiquidCrystal_I2C lcd (0x27, 16, 2);

// DEFINIÇÃO DO PINO DO BOTÃO
#define pinBot 4 //Cadastrar ADM
#define pinBot1 15 //Abrir trava com o botao
#define pinBot2 12 //Campainha
#define pinBotDel 14 //Cadastrar Deletar
#define pinLDR 27  //sensor de luminosidade

// DEFINIÇÃO DO PINO DO RELÊ
#define pinTrava 5  // trava rele
#define pinLuz 18  //luz rele


//pino da campainha
#define pinCamp 19  //campainha

SoftwareSerial fserial(16, 17);

FPM finger(&fserial);
FPM_System_Params params;
PushButton botao(pinBot);
PushButton botao1 (pinBot1);
PushButton botao2 (pinBot2);
PushButton botaoDel (pinBotDel);

// DECLARAÇÃO DAS VARIÁVEIS E FUNCOES
int16_t numID = 1;
bool gravar = false;
bool del = false;

int16_t enroll_finger(int16_t fid);
/////////////////////////////////////////////////////////////////////////


const char* SSID = "Andreta";                // SSID / nome da rede WiFi que deseja se conectar
const char* PASSWORD = "mhandreta";   // Senha da rede WiFi que deseja se conectar
WiFiClient wifiClient;                        
 
//MQTT Server
const char* BROKER_MQTT = "mqtt.eclipse.org"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883;                      // Porta do Broker MQTT

#define ID_MQTT  "Casa1"            //Informe um ID unico e seu. Caso sejam usados IDs repetidos a ultima conexão irá sobrepor a anterior. 
#define TOPIC_PUBLISH "Casa20"    //Informe um Tópico único. Caso sejam usados tópicos em duplicidade, o último irá eliminar o anterior.
PubSubClient MQTT(wifiClient);        // Instancia o Cliente MQTT passando o objeto espClient

//Declaração das Funções
void mantemConexoes();  //Garante que as conexoes com WiFi e MQTT Broker se mantenham ativas
void conectaWiFi();     //Faz conexão com WiFi
void conectaMQTT();     //Faz conexão com Broker MQTT
void enviaPacote();     //





//////////////////////////////////////////////////////////////////////////
void setup()
{

   conectaWiFi();
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);   


  //INICIA LCD
  Wire.begin();
  lcd.begin(16, 2);
  lcd.clear();

  digitalWrite(pinTrava, HIGH);
  digitalWrite(pinLuz, HIGH);
  pinMode(pinTrava, OUTPUT);
  pinMode(pinLuz, OUTPUT);
  pinMode(pinCamp, OUTPUT);
  
  Serial.begin(9600);
  Serial.println("ENROLL test");
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Iniciando!!!");
  fserial.begin(57600);

  if (finger.begin()) {
    finger.readParams(&params);
    Serial.println("Found fingerprint sensor!");
    lcd.clear();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Sensor");
    lcd.setCursor(0, 1);
    lcd.print("encontrado!");
    Serial.print("Capacity: "); Serial.println(params.capacity);
    Serial.print("Packet length: "); Serial.println(FPM::packet_lengths[params.packet_len]);

    delay(1000);
    lcd.clear();
    lcd.noBacklight();
  } else {
    Serial.println("Did not find fingerprint sensor :(");
     lcd.clear();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Sensor nao");
     lcd.setCursor(0, 1);
    lcd.print("encontrado!");
    delay(3000);
    lcd.clear();
    lcd.noBacklight();
    ESP.restart();
    while (1) yield();
  }

  /* just to find out if your sensor supports the handshake command */
  if (finger.handshake()) {
    Serial.println("Handshake command is supported.");
  }
  else {
    Serial.println("Handshake command not supported.");
  }
}

void loop()
{

   mantemConexoes();
   enviaValores();
   MQTT.loop();
  
   botao2.button_loop();

  if ( botao2.pressed() ){
     digitalWrite(pinCamp, HIGH);
       delay(500);
       digitalWrite(pinCamp, LOW);
       delay(50);
       digitalWrite(pinCamp, HIGH);
       delay(500);
       digitalWrite(pinCamp, LOW);
       delay(20);
       digitalWrite(pinCamp, HIGH);
       delay(100);
       digitalWrite(pinCamp, LOW);
       delay(20);
       digitalWrite(pinCamp, HIGH);
       delay(100);
       digitalWrite(pinCamp, LOW);
       delay(20);
       digitalWrite(pinCamp, HIGH);
       delay(100);
       digitalWrite(pinCamp, LOW);
       delay(20);
       digitalWrite(pinCamp, HIGH);
       delay(20);
       digitalWrite(pinCamp, HIGH);
       delay(100);
       digitalWrite(pinCamp, LOW);
       delay(20);
       digitalWrite(pinCamp, HIGH);
       delay(100);
       digitalWrite(pinCamp, LOW);
       delay(20);
       digitalWrite(pinCamp, HIGH);
       delay(100);
       digitalWrite(pinCamp, LOW);
       delay(20);
       digitalWrite(pinCamp, HIGH);
       delay(500);
       digitalWrite(pinCamp, LOW);
       delay(20);
       digitalWrite(pinCamp, HIGH);
       delay(500);
       digitalWrite(pinCamp, LOW);
       delay(20);
       digitalWrite(pinCamp, HIGH);
       delay(100);
       digitalWrite(pinCamp, LOW);
       delay(20);
       digitalWrite(pinCamp, HIGH);
       delay(100);
       digitalWrite(pinCamp, LOW);
       delay(20);
       digitalWrite(pinCamp, HIGH);
       delay(100);
       digitalWrite(pinCamp, LOW);
       delay(20);
       digitalWrite(pinCamp, HIGH);
       delay(100);
       digitalWrite(pinCamp, LOW);
       delay(20);
       digitalWrite(pinCamp, HIGH);
       delay(100);
       digitalWrite(pinCamp, LOW);
       delay(20);
       digitalWrite(pinCamp, HIGH);
       delay(100);
       digitalWrite(pinCamp, LOW);
     
       
       
  }

 botao1.button_loop();

  if ( botao1.pressed() ){
     digitalWrite(pinTrava, LOW);
       delay(500);
       digitalWrite(pinTrava, HIGH);
  }
  
  botao.button_loop();

  if ( botao.pressed() ){
    gravar = true;
  }

  if(gravar){
    enroll_finger(0);
    gravar = false;
  }

  
  
   //Deletar digital 
   botaoDel.button_loop();

  if ( botaoDel.pressed() ){
    del = true;
  }

  if(del){
    enroll_finger(1);
    del = false;
  }

getFingerprintIDez();
  
}



//Achar digital para deletar

int search_database(void) {
    int16_t p = -1;

    /* first get the finger image */
    Serial.println("Waiting for valid finger");
    while (p != FPM_OK) {
        p = finger.getImage();
        switch (p) {
            case FPM_OK:
                Serial.println("Image taken");
                break;
            case FPM_NOFINGER:
                Serial.println(".");
                delay(50);
                lcd.clear();
                delay(50);
                lcd.setCursor(0, 0);
                lcd.print("........");
                delay(50);
                lcd.print("................");
                break;
            case FPM_PACKETRECIEVEERR:
                Serial.println("Communication error");
                break;
            case FPM_IMAGEFAIL:
                Serial.println("Imaging error");
                break;
            case FPM_TIMEOUT:
                Serial.println("Timeout!");
                break;
            case FPM_READ_ERROR:
                Serial.println("Got wrong PID or length!");
                break;
            default:
                Serial.println("Unknown error");
                break;
        }
        yield();
    }

    /* convert it */
    p = finger.image2Tz();
    switch (p) {
        case FPM_OK:
            Serial.println("Image converted");
            break;
        case FPM_IMAGEMESS:
            Serial.println("Image too messy");
            return p;
        case FPM_PACKETRECIEVEERR:
            Serial.println("Communication error");
            return p;
        case FPM_FEATUREFAIL:
            Serial.println("Could not find fingerprint features");
            return p;
        case FPM_INVALIDIMAGE:
            Serial.println("Could not find fingerprint features");
            return p;
        case FPM_TIMEOUT:
            Serial.println("Timeout!");
            return p;
        case FPM_READ_ERROR:
            Serial.println("Got wrong PID or length!");
            return p;
        default:
            Serial.println("Unknown error");
            return p;
    }

    /* search the database for the converted print */
    uint16_t fid, score;
    p = finger.searchDatabase(&fid, &score);
    
    /* now wait to remove the finger, though not necessary; 
       this was moved here after the search because of the R503 sensor, 
       which seems to wipe its buffers after each scan */
    Serial.println("Remove finger");
    while (finger.getImage() != FPM_NOFINGER) {
        delay(500);
    }
    Serial.println();
    
    if (p == FPM_OK) {
        Serial.println("Found a print match!");
    } else if (p == FPM_PACKETRECIEVEERR) {
        Serial.println("Communication error");
        return p;
    } else if (p == FPM_NOTFOUND) {
        Serial.println("Did not find a match");
        return p;
    } else if (p == FPM_TIMEOUT) {
        Serial.println("Timeout!");
        return p;
    } else if (p == FPM_READ_ERROR) {
        Serial.println("Got wrong PID or length!");
        return p;
    } else {
        Serial.println("Unknown error");
        return p;
    }

    // found a match!
    Serial.print("Found ID #"); Serial.print(fid);
    Serial.print(" with confidence of "); Serial.println(score);
     lcd.clear();
     lcd.backlight();
     lcd.setCursor(0, 0);
     lcd.print("ID encontrado #");lcd.print(fid);
     lcd.setCursor(0, 1);
     lcd.print("Deletando...");
     delay(1000);

     // Deleta a digital
    p = finger.deleteModel(fid);

    if (p == FPM_OK) {
       Serial.println("Deletado!");
       lcd.clear();
       lcd.backlight();
       lcd.setCursor(0, 0);
       lcd.print("ID Deletado! #");lcd.print(fid);
       delay(2000);
       lcd.clear();
       lcd.noBacklight();
    } else if (p == FPM_PACKETRECIEVEERR) {
        Serial.println("Communication error");
        return p;
    } else if (p == FPM_BADLOCATION) {
        Serial.println("Could not delete in that location");
        return p;
    } else if (p == FPM_FLASHERR) {
        Serial.println("Error writing to flash");
        return p;
    } else if (p == FPM_TIMEOUT) {
        Serial.println("Timeout!");
        return p;
    } else if (p == FPM_READ_ERROR) {
        Serial.println("Got wrong PID or length!");
        return p;
    } else {
        Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
        return p;
    }
}






bool get_free_id(int16_t * fid) {
  int16_t p = -1;
  for (int page = 0; page < (params.capacity / FPM_TEMPLATES_PER_PAGE) + 1; page++) {
    p = finger.getFreeIndex(page, fid);
    switch (p) {
      case FPM_OK:
        if (*fid != FPM_NOFREEINDEX) {
          Serial.print("Free slot at ID ");
          Serial.println(*fid);
          return true;
        }
        break;
      case FPM_PACKETRECIEVEERR:
        Serial.println("Communication error!");
        return false;
      case FPM_TIMEOUT:
        Serial.println("Timeout!");
        return false;
      case FPM_READ_ERROR:
        Serial.println("Got wrong PID or length!");
        return false;
      default:
        Serial.println("Unknown error!");
        return false;
    }
    yield();
  }

  Serial.println("No free slots!");
  return false;
}

int16_t enroll_finger(int16_t fid) {
  int16_t p = -1;
  Serial.println("Waiting for valid finger to enroll");
   lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("insira uma");
  lcd.setCursor(0, 1);
  lcd.print("digital valida!");
  delay(1000);
  while (p != FPM_OK) {
    p = finger.getImage();
    switch (p) {
      case FPM_OK:
        Serial.println("Image taken");
         lcd.clear();
         lcd.backlight();
         lcd.setCursor(0, 0);
         lcd.print("Imagem");
         lcd.setCursor(0, 1);
         lcd.print("capturada!");
         delay(500);
        break;
      case FPM_NOFINGER:
        Serial.println(".");
        delay(50);
        lcd.clear();
        delay(50);
        lcd.setCursor(0, 0);
        lcd.print("........");
        delay(50);
        lcd.print("................");
        break;
      case FPM_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FPM_IMAGEFAIL:
        Serial.println("Imaging error");
         lcd.print("Falha!");
        break;
      case FPM_TIMEOUT:
        Serial.println("Timeout!");
        break;
      case FPM_READ_ERROR:
        Serial.println("Got wrong PID or length!");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
    yield();
  }
  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FPM_OK:
      Serial.println("Image converted");
        lcd.clear();
       lcd.backlight();
       lcd.setCursor(0, 0);
       lcd.print("Digital");
       lcd.setCursor(0, 1);
       lcd.print("Convertida!");
       delay(500);
      break;
    case FPM_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FPM_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FPM_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FPM_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    case FPM_TIMEOUT:
      Serial.println("Timeout!");
      return p;
    case FPM_READ_ERROR:
      Serial.println("Got wrong PID or length!");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.println("Remove finger");
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("Remova o dedo!");
  delay(1000);
  p = 0;
  while (p != FPM_NOFINGER) {
    p = finger.getImage();
    yield();
  }
  Serial.print("ID "); Serial.println(fid);

  p = -1;
  Serial.println("Place same finger again");
  lcd.clear();
   lcd.backlight();
   lcd.setCursor(0, 0);
   lcd.print("coloque o mesmo");
   lcd.setCursor(0, 1);
   lcd.print("dedo novamente!");
   delay(1000);
  while (p != FPM_OK) {
    p = finger.getImage();
    switch (p) {
      case FPM_OK:
        Serial.println("Image taken");
        lcd.clear();
         lcd.setCursor(0, 0);
         lcd.print("Digital");
         lcd.setCursor(0, 1);
         lcd.print("Capturada!");
         delay(500);
        break;
      case FPM_NOFINGER:
        Serial.print(".");
        delay(50);
        lcd.clear();
        delay(50);
        lcd.setCursor(0, 0);
        lcd.print("........");
        delay(50);
        lcd.print("................");
        break;
      case FPM_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FPM_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      case FPM_TIMEOUT:
        Serial.println("Timeout!");
        break;
      case FPM_READ_ERROR:
        Serial.println("Got wrong PID or length!");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
    yield();
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FPM_OK:
      Serial.println("Image converted");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Digital");
        lcd.setCursor(0, 1);
        lcd.print("Convertida!");
        delay(500);
        lcd.clear();
        lcd.noBacklight();
      break;
    case FPM_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FPM_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FPM_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FPM_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    case FPM_TIMEOUT:
      Serial.println("Timeout!");
      return false;
    case FPM_READ_ERROR:
      Serial.println("Got wrong PID or length!");
      return false;
    default:
      Serial.println("Unknown error");
      return p;
  }


  // OK converted!
  Serial.print("Criando modelo para #");  Serial.println(fid);
  
  p = finger.createModel();
  if (p == FPM_OK) {
     Serial.println("Prints matched!");
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("Digital");
     lcd.setCursor(0, 1);
     lcd.print("Correta!");
     delay(500);
  } else if (p == FPM_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FPM_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else if (p == FPM_TIMEOUT) {
    Serial.println("Timeout!");
    return p;
  } else if (p == FPM_READ_ERROR) {
    Serial.println("Got wrong PID or length!");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID "); Serial.println(fid);
  p = finger.storeModel(fid);
  if (p == FPM_OK) {
    Serial.println("Stored!");
    lcd.clear();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Digital gravada!");
    delay(1000);
    lcd.clear();
    lcd.noBacklight();
    return 0;
  } else if (p == FPM_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FPM_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FPM_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else if (p == FPM_TIMEOUT) {
    Serial.println("Timeout!");
    return p;
  } else if (p == FPM_READ_ERROR) {
    Serial.println("Got wrong PID or length!");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }
}





int getFingerprintIDez() {
  int16_t p = finger.getImage();
  if (p != FPM_OK)  return -1;

  p = finger.image2Tz();
  if (p != FPM_OK)  return -1;

 uint16_t fid, score;
    p = finger.searchDatabase(&fid, &score);
    if (p == FPM_OK) {
        Serial.println("Found a print match!");
         lcd.clear();
          lcd.backlight();
          lcd.setCursor(0, 0);
          lcd.print("Digital");
          lcd.setCursor(0, 1);
          lcd.print("Encontrada!!");
          delay(500);
    } else if (p == FPM_PACKETRECIEVEERR) {
        Serial.println("Communication error");
        return p;
    } else if (p == FPM_NOTFOUND) {
        Serial.println("Did not find a match");
         lcd.clear();
          lcd.backlight();
          lcd.setCursor(0, 0);
          lcd.print("Digital nao");
          lcd.setCursor(0, 1);
          lcd.print("Encontrada!!");
          delay(1000);
          lcd.clear();
          lcd.noBacklight();
        return p;
    } else if (p == FPM_TIMEOUT) {
        Serial.println("Timeout!");
        return p;
    } else if (p == FPM_READ_ERROR) {
        Serial.println("Got wrong PID or length!");
        return p;
    } else {
        Serial.println("Unknown error");
        return p;
    }
  
  //Encontrou uma digital!

   if (fid == 1) {
     Serial.print("Modo DELETE!");
      lcd.clear();
       lcd.backlight();
       lcd.setCursor(0, 0);
       lcd.print("Modo");
       lcd.setCursor(0, 1);
       lcd.print("DELETE!");
       delay(1000);
     
     
     search_database();
     return 0; 
   }
  if (fid == 0) {
     Serial.print("Modo Administrador!");
      lcd.clear();
       lcd.backlight();
       lcd.setCursor(0, 0);
       lcd.print("Modo");
       lcd.setCursor(0, 1);
       lcd.print("Administrador!");
       delay(1000);
     
     numID++;
     enroll_finger(numID);
     return 0; 
  
  } else {
       //vai ler o sensor de luminosidade
       int valorLDR = analogRead(pinLDR);
       Serial.println(valorLDR);
       //mostrar qual digital encontrou e abrir o portao
       Serial.print("ID encontrado #"); Serial.print(fid); 
       Serial.print(" com confiança de "); Serial.println(score);
       lcd.clear();
       lcd.backlight();
       lcd.setCursor(0, 0);
       lcd.print("ID encontrado #");lcd.print(fid);
       lcd.setCursor(0, 1);
       lcd.print("Abrindo...");
       digitalWrite(pinTrava, LOW);
       delay(3000);
       digitalWrite(pinTrava, HIGH);
       delay(1000);
       lcd.noBacklight();
       lcd.clear();
      /* //condicao de luminosidade para ligar ou nao a luz
       if(valorLDR > 200) {
           digitalWrite(pinLuz, LOW);
           delay(2000);
           digitalWrite(pinLuz, HIGH);
       }*/
       return fid;
  } 

}

////////////////////////////////////////////////////////////////////////////////


void mantemConexoes() {
    if (!MQTT.connected()) {
       conectaMQTT(); 
    }
    
    conectaWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}

void conectaWiFi() {

  if (WiFi.status() == WL_CONNECTED) {
     return;
  }
        
  Serial.print("Conectando-se na rede: ");
  Serial.print(SSID);
  Serial.println("  Aguarde!");

  WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI  
  while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Conectado com sucesso, na rede: ");
  Serial.print(SSID);  
  Serial.print("  IP obtido: ");
  Serial.println(WiFi.localIP()); 
}

void conectaMQTT() { 
    while (!MQTT.connected()) {
        Serial.print("Conectando ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) {
            Serial.println("Conectado ao Broker com sucesso!");
        } 
        else {
            Serial.println("Noo foi possivel se conectar ao broker.");
            Serial.println("Nova tentatica de conexao em 10s");
            delay(10000);
        }
    }
}

void enviaValores() {
static bool estadoBotao1 = HIGH;
static bool estadoBotao1Ant = HIGH;
static unsigned long debounceBotao1;

  estadoBotao1 = digitalRead(pinBot1);
  if (  (millis() - debounceBotao1) > 30 ) {  //Elimina efeito Bouncing
     if (!estadoBotao1 && estadoBotao1Ant) {

        //Botao Apertado     
        MQTT.publish(TOPIC_PUBLISH, "1");
        Serial.println("Botao1 APERTADO. Payload enviado.");
        
        debounceBotao1 = millis();
     } else if (estadoBotao1 && !estadoBotao1Ant) {

        //Botao Solto
        MQTT.publish(TOPIC_PUBLISH, "0");
        Serial.println("Botao1 SOLTO. Payload enviado.");
        
        debounceBotao1 = millis();
     }
     
  }
  estadoBotao1Ant = estadoBotao1;
}
