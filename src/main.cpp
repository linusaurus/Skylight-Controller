#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

enum State_enum {STOP, FORWARD, ROTATE_RIGHT, ROTATE_LEFT};
enum Sensors_enum {NONE, SENSOR_RIGHT, SENSOR_LEFT, BOTH};
 
void state_machine_run(uint8_t sensors);
void motors_stop();
void motors_forward();
void motors_right();
void motors_left();
uint8_t read_IR();
 
uint8_t state = STOP;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network.
// gateway and subnet are optional:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 10, 177);
IPAddress myDns(192, 168, 10, 1);
IPAddress gateway(192, 168, 1, 199);
IPAddress subnet(255, 255, 255, 0);


IPAddress server(192, 168, 10, 22);
EthernetClient ethclient;
PubSubClient client(ethclient);

void reconnect() {

  // Loop until we're reconnected

  while (!client.connected()) {

    Serial.print("Attempting MQTT connection...");

    // Attempt to connect

    if (client.connect("arduinoClient")) {

      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic","hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);

    }

  }

}

void state_machine_run(uint8_t sensors) 
{
  switch(state)
  {
    case STOP:
      if(sensors == NONE){
        motors_forward();
        state = FORWARD;
      }
      else if(sensors == SENSOR_RIGHT){
        motors_left();
        state = ROTATE_LEFT;
      }
      else{
        motors_right();
        state = ROTATE_RIGHT;
      }
      break;
       
    case FORWARD:
      if(sensors != NONE){
        motors_stop();
        state = STOP;
      }
      break;
 
    case ROTATE_RIGHT:
      if(sensors == NONE || sensors == SENSOR_RIGHT){
        motors_stop();
        state = STOP;
      }
      break;
 
    case ROTATE_LEFT:
      if(sensors != SENSOR_RIGHT)
      {
        motors_stop();
        state = STOP; 
      }
      break;
  }
}

void motors_stop()
{
  //code for stopping motors
}
 
void motors_forward()
{
  //code for driving forward  
}
 
void motors_right()
{
  //code for turning right
}
 
void motors_left()
{
  //code for turning left
}
 
uint8_t read_IR()
{
  return 1;
}

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  for (unsigned int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {
 
  Serial.begin(9600);
  client.setServer(server, 1883);
  client.setCallback(callback);
  Ethernet.begin(mac, ip);

  // Allow the hardware to sort itself out

  delay(1500);

}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  state_machine_run(read_IR());
}