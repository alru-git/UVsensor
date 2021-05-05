/**
 * UVsensor 
 * genutzt wird der UVM30A Sensor
 * https://www.mysensors.org/build/uv
 * 
 * Ver 2.0: Umstellung auf MSG2, verspricht besseren Empfang
 *          Üertragen wird nur noch der 10bit Rohwert aus dem Sensor 
 *          Die Sensor Kalibrierung und UV-Index Berechnung wird in Fhem gemacht
 *          Die "analogReference(INTERNAL)" = 1,1V wurde mit Multimeter bestätigt!!
 * Ver 2.1: VOLTAGE_SENSE_PIN = A5 (weil hier schon eine Ltg. angelötet war)
 *          SLEEP_TIME = 60000 Es wird jede Minute gesendet
 *
 */


// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_RF24

// MSG1 (Frequenz 2496 MHz)
// #define MY_RF24_CHANNEL 96
// MSG2 (Frequenz 2506 MHz)
#define MY_RF24_CHANNEL 106

// Optional: Define Node ID
#define MY_NODE_ID 122
// Node 0: MSG1 oder MSG2
#define MY_PARENT_NODE_ID 0
#define MY_PARENT_NODE_IS_STATIC

#include <MySensors.h>

int VOLTAGE_SENSE_PIN = A5;           // select the input pin for the VOLTAGE sense point

#define CHILD_ID_DATA 0               // ID für den Sensor

unsigned long SLEEP_TIME = 60000;    // Sleep time between reads (60000 milliseconds)

int sensorData = 0;                   // 10bit Rohdaten von UV Sensor

// Übertragung der Rohdaten in einer Custom Variable
  MyMessage msgDATA(CHILD_ID_DATA, V_VAR1);

void setup()
{
  // use the 1.1 V internal reference (Ausgabe als 10bit Integer)
    analogReference(INTERNAL);
}

void presentation()
{
	// Send the sketch version information to the gateway and Controller
	  sendSketchInfo("UVsensor", "2.1");
    present(CHILD_ID_DATA, S_CUSTOM);
}

void loop()
{
  // Update vom heartbeat
    sendHeartbeat();

  // Rohdaten als 10bit Wert ermitteln
    sensorData = analogRead(VOLTAGE_SENSE_PIN);
    Serial.println("Sensor Ausgang :");
    Serial.println(sensorData);
  
  // Rohdaten versenden
     send(msgDATA.set(sensorData,0));

  sleep(SLEEP_TIME);
}
