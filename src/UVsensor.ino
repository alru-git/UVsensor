/**
 *
 * DESCRIPTION
 *
 * UVsensor 
 * genutzt wird der UVM30A Sensor
 * https://www.mysensors.org/build/uv
 * 
 * Ver 0.1: Spannungsübertragung
 * Ver 0.2 - 0.5: UVIndex mit Textangabe, Spg.Vers. des Sensors über pinMode zum Stromsparen
 * Ver 0.61 : Mit Spannung und Glasfaktor kalibriert (bei UV Index 5-6)
 * Ver 0.7 : Schwellwerte des UV Index an Hand des Datenblattes angepasst
 *            Bsp.: 1.5 - 2.4 wird als UV Index 2 angezeigt
 *            die Grenzwerte für die jeweilige Spg. gehen jetzt also bis Index.4
 *            Bisher wurde bei 1mV über dem Index.0 der nächst höhere Index angezeigt
 *            Die neuen Grenzwerte wurden über die Formel:
 *            U [mv] = 94,62 * [Index.4] + 128.56   über eine lin. Regression ermittelt
 *            Mit den Korrekturfaktoren ist der maximalwert der Spg. 1185mV
 * Ver 0.8: Mit Solarpanel an RAW und alle Spannungen werden an VCC abgegriffen
 * Ver 0.9: SLEEP_TIME auf 5 Min, Code für Spg.-Vers. über PIN3 entfernt, erste produktive Version
 * 
 */


// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24

// MSG1 (Frequenz 2496 MHz)
#define MY_RF24_CHANNEL 96

// Optional: Define Node ID
#define MY_NODE_ID 102
// Node 0: MSG1 oder MSG2
#define MY_PARENT_NODE_ID 0
#define MY_PARENT_NODE_IS_STATIC

#include <MySensors.h>

int VOLTAGE_SENSE_PIN = A0;        // select the input pin for the VOLTAGE sense point

#define CHILD_ID_UVI 0              // UV Index wird hierdurch gesendet

unsigned long SLEEP_TIME = 5*60*1000;   // Sleep time between reads (in milliseconds)
float sensorValue = 0;             // 10bit Wert der Spannung an A0
float sensorV = 0;                 // Kalibrierte Spannung, die übertragen wird

// Übertragung des Spannungslevels
MyMessage msgV(0,V_VOLTAGE);

// Übertragung des UV Index
MyMessage msgUVI(CHILD_ID_UVI, V_UV);

// Berechnungsarray für den UV Index
float uvIndex;
float lastUV = -1;
// es werden nur Werte bis 1100 berücksichtigt (wg. 1.1V interner Ref. Spg.)
uint16_t uvIndexValue [12] = { 100, 261, 356, 450, 545, 640, 734, 829, 923, 1018, 1113, 1186};

// UV Beschreibung
char *uvText[] = {"0 (niedrig)", "1 (niedrig)", "2 (niedrig)", "3 (mäßig, 20 Min)", "4 (mäßig, 20 Min))",
                   "5 (mäßig, 20 Min)", "6 (hoch, 15 Min)", "7 (hoch, 15 Min)", "8 (sehr hoch, 10 Min)",
                   "9 (sehr hoch, 10 Min)", "10 (sehr hoch, 10 Min)", "11 (extrm, 2 Min)", "11+ (extrem, 2 Min)"
                    };
// Text Message für uvIndex initialisieren
MyMessage msgT(0,V_TEXT);

void setup()
{
  // use the 1.1 V internal reference (Ausgabe als 10bit Integer)
  analogReference(INTERNAL);
}

void presentation()
{
	// Send the sketch version information to the gateway and Controller
	sendSketchInfo("UVsensor", "0.9");

  present(0, S_MULTIMETER);
  present(CHILD_ID_UVI, S_UV);
  present(0,S_INFO);
}

void loop()
{
  // Update vom heartbeat
    sendHeartbeat();
  
	// Spannung in [mv] ermitteln als 10bit Wert ermitteln, kalibrieren
  	sensorValue = analogRead(VOLTAGE_SENSE_PIN);
    Serial.println("Sensor Ausgang :");
    Serial.println(sensorValue);
    
    // Spannungsberechnung: Sensor [mV] = Wert / 2^10 * Ref.-Spg.[mV] + 8% Glas - 10mV Messfehler
    // Es werden ca. 10mV linear zuviel angezeigt, verglichen mit Multimeter
    // Das Glas schluckt 8% Licht
    sensorV = (sensorValue / 1023 * 1100) * (1 + 8 / 92) - 10;
  	Serial.println("Sensor Voltage [mV]: ");
  	Serial.println(sensorV);
  // --> Ende mit Spannung

  // UV Index ermitteln
     int i;
        for (i = 0; i < 12; i++) {
            if (sensorV <= uvIndexValue[i]) {
                uvIndex = i;
                break;
            }
        }
      Serial.println("UVI: ");
      Serial.println(uvIndex,0);
      Serial.println("UV Text: ");
      Serial.println(uvText[i]);
  // --> Ende mit UV Index  
  
  // Daten versenden, nur wenn sich der Index verändert hat
      if (uvIndex != lastUV) {
          send(msgV.set(sensorV,0));  
          send(msgUVI.set(uvIndex,0));
          send(msgT.set(uvText[i]));
          lastUV = uvIndex;
      }

  sleep(SLEEP_TIME);
}
