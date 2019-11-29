#include <Wire.h>                //Biblioteca para I2C
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

//Código adptado de Fenando K Tecnlogia: https://www.fernandok.com/2018/08/esp32-bluetooth-low-energy.html
 
BLECharacteristic *characteristicTX; //através desse objeto iremos enviar dados para o client
 
bool deviceConnected = false; //controle de dispositivo conectado
 
const int LED = 2; //LED interno do ESP32 (esse pino pode variar de placa para placa)
 
#define SERVICE_UUID   "6ab11f25-3c21-4008-b27e-35d4a30fc879"
#define CHARACTERISTIC_UUID_RX  "4ac8a682-9736-4e5d-932b-e9b31405049c"
#define CHARACTERISTIC_UUID_TX  "0972EF8C-7613-4075-AD52-756F33D4DA91"

//callback para receber os eventos de conexão de dispositivos
class ServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
 deviceConnected = true;
    };
 
    void onDisconnect(BLEServer* pServer) {
 deviceConnected = false;
    }
};

//callback para eventos das características
class CharacteristicCallbacks: public BLECharacteristicCallbacks {
     void onWrite(BLECharacteristic *characteristic) {
          //retorna ponteiro para o registrador contendo o valor atual da caracteristica
          std::string rxValue = characteristic->getValue(); 
          //verifica se existe dados (tamanho maior que zero)
          if (rxValue.length() > 0) {
           for (int i = 0; i < rxValue.length(); i++) {
             Serial.print(rxValue[i]);
               }
               Serial.println("Entrou Aqui");
               //L1 liga o LED | L0 desliga o LED
               if (rxValue.find("L1") != -1) { 
             digitalWrite(LED, HIGH);
               }
               else if (rxValue.find("L0") != -1) {
             digitalWrite(LED, LOW);
               }
          }
     }//onWrite
};


void setup() {
    Serial.begin(115200);
 
    pinMode(LED, OUTPUT);
 
    // Create the BLE Device
    BLEDevice::init("ESP32-GUIA"); // nome do dispositivo bluetooth
 
    // Create the BLE Server
    BLEServer *server = BLEDevice::createServer(); //cria um BLE server 
 
    server->setCallbacks(new ServerCallbacks()); //seta o callback do server
 
    // Create the BLE Service
    BLEService *service = server->createService(SERVICE_UUID);
 
    // Create a BLE Characteristic para envio de dados
    characteristicTX = service->createCharacteristic(
                       CHARACTERISTIC_UUID_TX,
                       BLECharacteristic::PROPERTY_NOTIFY
                     );
    characteristicTX->addDescriptor(new BLE2902());

       // Create a BLE Characteristic para recebimento de dados
    BLECharacteristic *characteristic = service->createCharacteristic(
                                                      CHARACTERISTIC_UUID_RX,
                                                      BLECharacteristic::PROPERTY_WRITE
                                                    );
 
    characteristic->setCallbacks(new CharacteristicCallbacks());
 
    // Start the service
    service->start();
 
    // Start advertising (descoberta do ESP32)
    server->getAdvertising()->start();
 
    //Inicializa sensor de temperatura infravermelho 
    //sensor.begin(); 
 
    //Seleciona temperatura em Celsius 
    //sensor.setUnit(TEMP_C);//podemos ainda utilizar TEMP_F para Fahrenheit ou TEMP_K para Kelvin 
    Serial.println("Aguardando algum dispositivo conectar...");
}

void loop() {

      //se existe algum dispositivo conectado
    if (deviceConnected) {
            Serial.println("Conectou... ");
            //characteristicTX->setValue(100); //seta o valor que a caracteristica notificará (enviar) 
            //characteristicTX->notify(); // Envia o valor para o smartphone
    }
    delay(1000);
}
