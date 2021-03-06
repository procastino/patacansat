// rf69 demo tx rx.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing client
// with the RH_RF69 class. RH_RF69 class does not provide for addressing or
// reliability, so you should only use RH_RF69  if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf69_server.
// Demonstrates the use of AES encryption, setting the frequency and modem 
// configuration

#include <SPI.h>
#include <RH_RF69.h>

/************ Radio Setup ***************/

// Change to 434.0 or other frequency, must match RX's freq!
#define RF69_FREQ 434.0

//definimos pins para radio e outras funcións
#define RFM69_CS      8
#define RFM69_INT     3
#define RFM69_RST     4
#define LED           13
#define PINBOTON      10
#define PINLED        11

//variabeis para a detección do estado do botón
int contadorBoton = 0;   // contador de pulsacións
int estadoBoton = 0;        
int ultimoEstadoBoton = 0;

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

int16_t packetnum = 0;  // packet counter, we increment per xmission

void setup() 
{
  Serial.begin(115200);
  //while (!Serial) { delay(1); } // wait until serial console is open, remove if not tethered to computer
  pinMode(PINBOTON, INPUT);
  pinMode(PINLED, OUTPUT);     
  pinMode(LED, OUTPUT);     
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

//para que dea tempo a iniciarse o monitor serie antes de empezar a sacar mensaxes
  delay(3000);
 
  Serial.println("Feather RFM69 RX Test!");
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

//**** lemos o botón e actualizamos o contador ****//
//facemos a lectura do botón, unha pulsación cambia de estado
estadoBoton = digitalRead(PINBOTON);

 // comparamos a ver se cambiou o estado do botón
  if (estadoBoton != ultimoEstadoBoton) {
    //se cambiou e foi por premelo
    if (estadoBoton == HIGH) {
      contadorBoton++;
      Serial.println("pulsado");
      }
    // para evitar bounces
    delay(50);
  }
  // o estado último pasa a ser como o estado actual
  ultimoEstadoBoton = estadoBoton;

  //acendemos ou apagamos o led segundo se a pulsación é impar ou par
  if (contadorBoton % 2) {
    digitalWrite(PINLED, HIGH);
  }
  else {
    digitalWrite(PINLED, LOW);
  }

//**** recepción de mensaxes ****//
 if (rf69.available()) {
    // Should be a message for us now   
    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf69.recv(buf, &len)) {
      if (!len) return;
      buf[len] = 0;
      Serial.print("Received [");
      Serial.print(len);
      Serial.print("]: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf69.lastRssi(), DEC);

//**** envío de comandos ****//
//Se o botón ten unha pulsación impar envíamos apertura
        if (contadorBoton % 2) {
        //creamos un array data que contén abrir
        uint8_t data[] = "abrir";
        //envíamos data indicando a súa lonxitude
        rf69.send(data, sizeof(data));
        //agardamos a que se envíe o paquete
        rf69.waitPacketSent();
        //imprimimos mensaxe de confirmación
        Serial.println("Enviada orde de apertura");
        //facemos que o led parpadee
        Blink(LED, 40, 2); //blink LED 2 times, 40ms between blinks
      }
      else {
        //envíamos cerrar, coa mesma estrutura que antes
        uint8_t data[] = "cerrar";
        rf69.send(data, sizeof(data));
        rf69.waitPacketSent();
        Serial.println("Enviada orde de peche");
        Blink(LED, 40, 4); //blink LED 3 times, 40ms between blinks
      }
      //esta mensaxe de confirmación non a imos envíar de momento
//      else if (strstr((char *)buf, "pres")){
//        // Send a reply!
//        uint8_t data[] = "correto";
//        rf69.send(data, sizeof(data));
//        rf69.waitPacketSent();
//        Serial.println("Sent a reply");
//        Blink(LED, 40, 3); //blink LED 3 times, 40ms between blinks
//        }
        
    } 
    //se non recibimos nada
    else {
      Serial.println("Receive failed");
    }
  }
//  else {
//    Serial.println("Non hai RFM69 dispoñible");
//    delay(500);
//  }
}

//función que define o parpadeo
void Blink(byte PIN
, byte DELAY_MS, byte loops) {
  for (byte i=0; i<loops; i++)  {
    digitalWrite(PIN,HIGH);
    delay(DELAY_MS);
    digitalWrite(PIN,LOW);
    delay(DELAY_MS);
  }
}
