#include <mcp_can.h>
#include <SPI.h>
#include <SoftwareSerial.h>

// ===== UART pro ESP32 =====
SoftwareSerial espSerial(2, 3); // RX, TX

// ===== MCP2515 =====
const int SPI_CS_PIN = 10;
MCP_CAN CAN(SPI_CS_PIN);

// Variáveis temporárias (opcional, para debug)
double battery = 0;
double temp = 0;
double rpm = 0;

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);

  // Inicializa MCP2515
  if (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
    Serial.println("CAN iniciado com sucesso!");
  } else {
    Serial.println("Falha ao iniciar CAN");
    while (1);
  }

  CAN.setMode(MCP_NORMAL); 
  Serial.println("Aguardando mensagens do PDM...");
}

void loop() {
  if (CAN.checkReceive() == CAN_MSGAVAIL) {
    long unsigned int rxId;
    unsigned char len = 0;
    unsigned char buf[8];

    CAN.readMsgBuf(&rxId, &len, buf);

      double value;
      memcpy(&value, buf, sizeof(double)); 

      // Mapeamento de IDs
      switch (rxId) {
      case 0x64: {
        uint32_t rpm_val = (uint32_t)buf[0] | ((uint32_t)buf[1] << 8) | ((uint32_t)buf[2] << 16) | ((uint32_t)buf[3] << 24);

        float battery_val;
        memcpy(&battery_val, &buf[4], sizeof(float));

        Serial.println("RPM: " + String(rpm_val));  
        Serial.println("Battery: " + String(battery_val, 2));
        delay(1000);
        break;
      }

        case 0x65: // Temperatura
          temp = value;
          espSerial.println("temp:" + String(temp, 2));
          Serial.println("Temp: " + String(temp, 2));
          break;

        case 0x66: // RPM
          rpm = value;
          espSerial.println("rpm:" + String(rpm, 0));
          Serial.println("RPM: " + String(rpm, 0));
          break;

        default:
          break;
      }
    }
}
