#include <mcp_can.h>
#include <SPI.h>
#include <SoftwareSerial.h>

// ===== UART PRO ESP32 =====
SoftwareSerial espSerial(2, 3); // RX, TX

// ===== MCP2515 =====
const int SPI_CS_PIN = 10;
MCP_CAN CAN(SPI_CS_PIN);

// ===== VARIÁVEIS =====
float rpm = 0;
float extBattery = 0;

float ventoinhaCurrent = 0;
float engineTemp = 0;

float throttlePosition = 0;
float lambdaVal = 0;

float airTemp = 0;
float mapVal = 0;

float bicosD2Current = 0;
float bicosD1Current = 0;

float bombaCurrent = 0;
float shifterCurrent = 0;

float brakeLightCurrent = 0;
float partidaCurrent = 0;

float bobinasCurrent = 0;
float gpsSpeed = 0;

float poTotCurrent = 0;
float poTotCrntAll = 0;

uint16_t bombaInput = 0;
uint16_t giratoriaInput = 0;

// ===== TIMING =====
unsigned long lastSend = 0;
const unsigned long SEND_INTERVAL = 100; // ms

// ===== FUNÇÃO AUXILIAR =====
float readFloat(uint8_t *buf, uint8_t offset) {
  float val;
  memcpy(&val, &buf[offset], sizeof(float));
  return val;
}

void setup() {
  
  Serial.begin(115200);
  espSerial.begin(9600);

  if (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
    Serial.println("CAN iniciado com sucesso");
  } else {
    Serial.println("Erro ao iniciar CAN");
    while (1);
  }

  CAN.setMode(MCP_NORMAL);
  Serial.println("Aguardando CAN...");
}

void loop() {
  bool canUpdated = false;

  // ===== RECEBE CAN =====
  if (CAN.checkReceive() == CAN_MSGAVAIL) {
    long unsigned int rxId;
    unsigned char len;
    unsigned char buf[8];

    CAN.readMsgBuf(&rxId, &len, buf);

    switch (rxId) {

      case 0x64:
        rpm = readFloat(buf, 0);
        extBattery = readFloat(buf, 4);
        canUpdated = true;
        break;

      case 0x65:
        ventoinhaCurrent = readFloat(buf, 0);
        engineTemp = readFloat(buf, 4);
        canUpdated = true;
        break;

      case 0x66:
        throttlePosition = readFloat(buf, 0);
        lambdaVal = readFloat(buf, 4);
        canUpdated = true;
        break;

      case 0x67:
        airTemp = readFloat(buf, 0);
        mapVal = readFloat(buf, 4);
        canUpdated = true;
        break;

      case 0x68:
        bicosD2Current = readFloat(buf, 0);
        bicosD1Current = readFloat(buf, 4);
        canUpdated = true;
        break;

      case 0x69:
        bombaCurrent = readFloat(buf, 0);
        shifterCurrent = readFloat(buf, 4);
        canUpdated = true;
        break;

      case 0x70:
        brakeLightCurrent = readFloat(buf, 0);
        partidaCurrent = readFloat(buf, 4);
        canUpdated = true;
        break;

      case 0x71:
        bobinasCurrent = readFloat(buf, 0);
        gpsSpeed = readFloat(buf, 4);
        canUpdated = true;
        break;

      case 0x72:
        poTotCurrent = readFloat(buf, 0);
        poTotCrntAll = readFloat(buf, 4);
        canUpdated = true;
        break;

      case 0x73:
        bombaInput = buf[0] | (buf[1] << 8);
        giratoriaInput = buf[2] | (buf[3] << 8);
        canUpdated = true;
        break;
    }
  }

  // ===== ENVIA JSON SOMENTE SE CAN ATUALIZOU =====
  if (canUpdated) {
    canUpdated = false;

    unsigned long ts = millis();

    String json = "{";
    json += "\"ts\":" + String(ts) + ",";
    json += "\"rpm\":" + String(rpm, 1) + ",";
    json += "\"extBattery\":" + String(extBattery, 2) + ",";
    json += "\"ventoinhaCurrent\":" + String(ventoinhaCurrent, 2) + ",";
    json += "\"engineTemp\":" + String(engineTemp, 2) + ",";
    json += "\"throttle\":" + String(throttlePosition, 2) + ",";
    json += "\"lambda\":" + String(lambdaVal, 2) + ",";
    json += "\"airTemp\":" + String(airTemp, 2) + ",";
    json += "\"map\":" + String(mapVal, 2) + ",";
    json += "\"bicosD2Current\":" + String(bicosD2Current, 2) + ",";
    json += "\"bicosD1Current\":" + String(bicosD1Current, 2) + ",";
    json += "\"bombaCurrent\":" + String(bombaCurrent, 2) + ",";
    json += "\"shifterCurrent\":" + String(shifterCurrent, 2) + ",";
    json += "\"brakeLightCurrent\":" + String(brakeLightCurrent, 2) + ",";
    json += "\"partidaCurrent\":" + String(partidaCurrent, 2) + ",";
    json += "\"bobinasCurrent\":" + String(bobinasCurrent, 2) + ",";
    json += "\"gpsSpeed\":" + String(gpsSpeed, 2) + ",";
    json += "\"poTotCurrent\":" + String(poTotCurrent, 2) + ",";
    json += "\"poTotCrntAll\":" + String(poTotCrntAll, 2) + ",";
    json += "\"bombaInput\":" + String(bombaInput) + ",";
    json += "\"giratoriaInput\":" + String(giratoriaInput);
    json += "}";

    espSerial.println(json);
    Serial.println(json);
  }
}
