//Incluímos todas as librerías
//Comunicación do módulo de radio
#include <SPI.h>
#include <RH_RF69.h>
//Sensor de temperatura e presión
#include <Wire.h>
#include <SFE_BMP180.h>

//Creamos obxecto bmp180 
SFE_BMP180 bmp180;
double Po=1013.25;
double altitudeBase = 200.00;
double altitudeSubida = altitudeBase + 200;
double altitudeBaixada = altitudeBase + 10;

/************ Radio Setup ***************/

// establecemos frecuencia de comunicación a 434 MHZ
// pódese poñer outra?
#define RF69_FREQ 434.0
//establecemos pins para as funcións da radio e led
#define RFM69_CS      8
#define RFM69_INT     3
#define RFM69_RST     4
#define LED           13
#define PINLED        10
#define PINBUZZER     9

//flag para saber se xa pasamos pola ascensión para a baliza sonora
boolean xaSubiches = false;

// cousas da librería de radio
RH_RF69 rf69(RFM69_CS, RFM69_INT);

void setup() 
{
  Serial.begin(115200);
  //sen este delay non aparecen as mensaxes de inicio, supoño que porque se executan antes de que abra o monitor serie (ou algo así)
  delay(3000);
  //definimos como saída o pin do led
  pinMode(PINLED, OUTPUT);
  
/**** codigo de inicio do BMP ****/
if (bmp180.begin())
    Serial.println("BMP180 iniciado correctamente");
  else
  {
    Serial.println("Error al iniciar el BMP180");
    while(1); // bucle infinito /// é a mellor solución?
  }
  double status;

  status = bmp180.startTemperature();//Inicio de lectura de temperatura: tempo de medida en milisegundos
  Serial.println(status);
  status= bmp180.startPressure(3); //Inicio de lectura de temperatura: devolve o tempo, en milisegundos, que tarda en medir tres veces a presión.
  Serial.println(status); //fin codigo bmp

  /**** Código da comunicación de radio, inicio e mensaxes de funcionamento****/
  
  pinMode(LED, OUTPUT);   
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  Serial.println("Feather RFM69 TX Test!");
  Serial.println();

  // manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);
  
  if (!rf69.init()) {
    Serial.println("RFM69 radio init failed");
    while (1);
  }
  Serial.println("RFM69 radio init OK!");
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
  // No encryption
  if (!rf69.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed");
  }

  // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
  // ishighpowermodule flag set like this:
  rf69.setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW

  // The encryption key has to be the same as the one in the server
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  rf69.setEncryptionKey(key);
  
  pinMode(LED, OUTPUT);

  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");
}



void loop() {
/*** código de lectura do BMP ***/
  char status; //a variable estatus é dinámica, estase redefinindo continuamente
  double T,P,A;
  status = bmp180.startTemperature();//Inicio de lectura de temperatura
  if(status !=0)
    {
      delay(status); 
      status= bmp180.getTemperature(T);
      if (status !=0);
      {
        status=bmp180.startPressure(3);
        if (status !=0)
        {
          delay(status);
          status=bmp180.getPressure(P,T);
          if (status !=0)
          {
            Serial.print("Temperatura: ");
            Serial.print(T,2);
            Serial.print(" oC ; ");
            Serial.print("Presion: ");
            Serial.print(P,2);
            Serial.print(" mbar");     
            A=bmp180.altitude(P,Po);
            Serial.print("; Altitud: ");
            Serial.print(A);
            Serial.println(" metros");  
           
          }
        }
      }
    }

/***código da emisión de paquetes***/
//comprobamos se o satélite xa subiu da altura definida
if (A > altitudeSubida){
  xaSubiches = true;
}
//se xa subiu e está por debaixo da altitude de baixada
if (A < altitudeBaixada && xaSubiches) {
  //facemos soar o buzzer tres veces
  for (int i = 0; i < 3; i++){
    digitalWrite(PINBUZZER, HIGH);
    delay(250);
    digitalWrite(PINBUZZER, LOW);
    delay(80);
  }
  
}
//en caso contrario, esperamos un segundo entre medicións
else {
  delay(1000);  
}
  // creamos o array de 20 caracteres radiopacket, ven a ser o mesmo que un string
  char radiopacket[20];
  //formateamos un string que se chama radiopacket, e que substitúe os datos con % polas variables
  sprintf(radiopacket,"temp. %d, pres. %d",(int)T,(int)P); 
  //mensaxe de emisión 
  Serial.print("Sending "); Serial.println(radiopacket);

  //Enviamos a mensaxe cunha instrución da librería. Non nos importa moito, só que envía "radiopacket"
  rf69.send((uint8_t *)radiopacket, strlen(radiopacket));
  rf69.waitPacketSent();

/*** código da recepción de paquetes desde terra ***/
  uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  // esperamos por unha mensaxe de volta de terra
  if (rf69.waitAvailableTimeout(500))  { 
    // Miramos a ver se se recibiu unha mensaxe con contido   
    if (rf69.recv(buf, &len)) {
      buf[len] = '\0';
      Serial.print("Got a reply: ");
      Serial.println((char*)buf);
      //con esta instrucción buscamos se hai algunha ocurrencia de "abrir" no char buf
      if (strstr((char *)buf, "abrir")) {
        Serial.println("abrindo porta");
        digitalWrite(PINLED, HIGH);       
      }
      //se non a hai, será que recibimos cerrar e cerramos
      else {
        Serial.println("cerrando porta");
        digitalWrite(PINLED, LOW);
        }
      Blink(LED, 50, 3); //blink LED 3 times, 50ms between blinks
    } 
    //se non se recibiu nada
    else {
      Serial.println("Receive failed");
    }
  } 
  //se non había ningunha resposta
  else {
    Serial.println("No reply, is another RFM69 listening?");
  }
}

void Blink(byte PIN, byte DELAY_MS, byte loops) {
  for (byte i=0; i<loops; i++)  {
    digitalWrite(PIN,HIGH);
    delay(DELAY_MS);
    digitalWrite(PIN,LOW);
    delay(DELAY_MS);
  }
}
