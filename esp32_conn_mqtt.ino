#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// Define pin mappings
#define LANE1_PIN  2   // D2
#define LANE2_PIN  4   // D4
#define LANE3_PIN  5   // D5
#define LANE4_PIN  18  // D18

// WiFi credentials
const char* ssid = "Nishad";
const char* password = "23456789";

// AWS IoT Core parameters
const char* mqtt_server = "a1jsymzdtk2uy7-ats.iot.ap-south-1.amazonaws.com";
const int mqtt_port = 8883;

// AWS IoT Certificates
const char* amazon_root_ca =
  "-----BEGIN CERTIFICATE-----\n"
  "MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n"
  "ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n"
  "b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n"
  "MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n"
  "b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n"
  "ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n"
  "9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n"
  "IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n"
  "VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n"
  "93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n"
  "jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n"
  "AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n"
  "A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n"
  "U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n"
  "N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n"
  "o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n"
  "5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n"
  "rqXRfboQnoZsG4q5WTP468SQvvG5\n"
  "-----END CERTIFICATE-----\n";


const char* certificate_pem_crt =
  "-----BEGIN CERTIFICATE-----\n"
  "MIIDWTCCAkGgAwIBAgIUcGo/B+whe0/HIENhI68FnULgfQIwDQYJKoZIhvcNAQEL\n"
  "BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n"
  "SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI1MDIxMTEwMzYx\n"
  "MFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n"
  "ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAK6e3o/JkBhetOfh66xE\n"
  "ptTD3azsQ/B9TacrFhxMTPmyzBI55yA1Qm2HIUYeiv3x2EI6tAFYQl6nJT6sXV9Y\n"
  "ilbx0uaXrPIz2//2NLc5Bira/iPgU1L1otRjpqQTGEDbdzPnQNWlYFXhgxPi/Jqv\n"
  "1QuASM4G6aa/aQLvqMURNVqq3QDSueZf2xXFKvI1VzEurYEq48zNy+46RpdSwpLa\n"
  "TcOwdYylL0o1rg2+ny8LXYDbr2uxstTcI6/Yd6tpgcN2vjQr8ZtVMzMsXYGtfIYY\n"
  "UXr25QX5zPohYfa2BjLF65gJgOKh/Mj97uV0rDVi538EQnYpJ0CPsP5TNM3loRa8\n"
  "TrcCAwEAAaNgMF4wHwYDVR0jBBgwFoAU2qbWpaGqb7+IYmo23R9EJjNhiDMwHQYD\n"
  "VR0OBBYEFEiWqdoesN5TzCwaRuTaJBHoqBfKMAwGA1UdEwEB/wQCMAAwDgYDVR0P\n"
  "AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQCdatDuw1YUKEkosYFxla6Yf8iS\n"
  "ooblIi1Vd/t8aHPkpZJ9G/Q4LPJNgtnP9Hv0C/L+uRsh3BJIwXQZtd3fJL9FpXfQ\n"
  "nSnKGpzjCaoePjWjgpH5zhrtkXdWmHUKrpYqjlKN61aBkPq/2z4KKV8QYGoaREih\n"
  "1QsodhUKqPyvTM9b2z8C1VEDfLEHD5t4m1PAnqK7RHk2LmfxAYajKeErg0Ms28gK\n"
  "t1+Gp4ssD6stsXtPNgGH66f2YcHMXJVDH4u2jhzxCUabb7h01T/EFxTkMlAvXZYv\n"
  "LGIBdWv8uRPGx1xE3hPuY1PMN2uON3rwTVrR8En/PBjPRSQj0NZKqS9gSmoY\n"
  "-----END CERTIFICATE-----\n";


const char* private_pem_key =
  "-----BEGIN RSA PRIVATE KEY-----\n"
  "MIIEowIBAAKCAQEArp7ej8mQGF605+HrrESm1MPdrOxD8H1NpysWHExM+bLMEjnn\n"
  "IDVCbYchRh6K/fHYQjq0AVhCXqclPqxdX1iKVvHS5pes8jPb//Y0tzkGKtr+I+BT\n"
  "UvWi1GOmpBMYQNt3M+dA1aVgVeGDE+L8mq/VC4BIzgbppr9pAu+oxRE1WqrdANK5\n"
  "5l/bFcUq8jVXMS6tgSrjzM3L7jpGl1LCktpNw7B1jKUvSjWuDb6fLwtdgNuva7Gy\n"
  "1Nwjr9h3q2mBw3a+NCvxm1UzMyxdga18hhhRevblBfnM+iFh9rYGMsXrmAmA4qH8\n"
  "yP3u5XSsNWLnfwRCdiknQI+w/lM0zeWhFrxOtwIDAQABAoIBAQCG4KWuUJVhjX2U\n"
  "gnekF175htSp5dDQaCjbz7cDYpguYsGn7lutEBZgPXkhDSR05CITff5d74tMSzTE\n"
  "hSSWKfCpGZ66iDuI1S/ANK8hOwWdgou8sGAL+zmrbdFgeHNKXXpkd+InY9bWXxr8\n"
  "+F3/KYOYn50crlA5gS2em/kzRoh8n6fz0RO4qtEVGBteNbaU5Dfj9q5tG1+qzj6I\n"
  "zEdqutZQai0mrlQzbN1e/2mNDkZU/owfgMZCzTSY5XxmlFZY7FsgeLWVP8YYdqPO\n"
  "LWTNDnn+NvTI5jSeN6WFER7yeLgLnqeUX6CUFKGksZPYdgiL8nZ/Pmq1IzYvIyLk\n"
  "fAvGRytZAoGBAOTB1DVruUfKukBsUQa8PjeApGPzNupHgjjvLVc/bCRhEcHWCkKo\n"
  "Uor4m6Jr4b/h4o4TyDG/zBcX5n4wyDadxG+uDiL1sU9Uw+mcCZhBNdqzW6GKHulG\n"
  "YGKB+k+56JtXK4dHAaMQFKNS0KnyXJfT5ii+NPcV/myY9iphibpWOOMjAoGBAMNq\n"
  "kRo74MzQZle7PqrGb9pHt7V+Ha6WgEFygWiPQCLN92b56fgEcQHl4iV3HlTGn0S5\n"
  "JvxVS/hF8pCROhY3pqoySg6ubs7gDEJAY/VzfYMZwC0Q5ox5PvVop0gbKA5fJWBh\n"
  "RoUViaj7hpF2BFQoyJyKGH+/m7lL0rDnH2Fz9TldAoGAL+AxHY0SFehj2p/QxWwY\n"
  "SV1vCkwYrieRJYFJJ/1qEKMQKFU3iIPZ4qPtTqztbk9ELyAMwfy2Jg8mejqTbHrQ\n"
  "Zd8zrjVliojfB8FUcL0VHFR1VPWnSoMkIPm6/XyXJgu0XHeL7CitaREoAqlqn4wF\n"
  "m3lTdHLx8KCCxJPtU0ZAWakCgYA9uP8z5g3PI1PwCfh1QFA8BcME1FzTtD22blBp\n"
  "kjWTyLKIGA/Kgrye+NlBNCqkZEgSENk7gkWIma9P8k8CjXla8J9rm7TsKuA0XNo1\n"
  "wNC4A/4rPddUL0evg+qyX8F7KJpJou//QRpGAYwzja84VdlneEbcssyg7k4SCi5j\n"
  "PZkLEQKBgGzcIcgh9i9rsXn/yTH4nZWrRPCGk+eRBytCdmtR4BRcC+yKGUk6tWv6\n"
  "U8noFP+pq8+ZR7P2BX67q/YLfubrmUuoItq0/XD9lKJSeAa3Gw3HqCfUax/gFFxM\n"
  "e9iqVzWaiBpV67hrYgI8b+bazo5PdW9MCgk8W6GRE4EAZupK1xTj\n"
  "-----END RSA PRIVATE KEY-----\n";


// Global objects
WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup_wifi() {
  Serial.println("\n[WiFi] Connecting to network...");
  WiFi.begin(ssid, password);

  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    attempt++;

    if (attempt > 20) {
      Serial.println("\n[WiFi] Failed to connect. Restarting ESP...");
      ESP.restart();
    }
  }
  Serial.println("\n[WiFi] Connected successfully.");
  Serial.print("[WiFi] IP Address: ");
  Serial.println(WiFi.localIP());
}

// Callback function to handle incoming MQTT messages
void message_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("[MQTT] Message received on topic: ");
  Serial.println(topic);

  // Convert payload to string
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  // Clean the message by removing unwanted characters
  message.replace("{", "");
  message.replace("}", "");
  message.replace("'", "");
  message.replace("\"", "");
  message.trim();  // Remove any leading/trailing spaces

  Serial.print("[MQTT] Cleaned Message: ");
  Serial.println(message);

  // Check for lane commands and set the pins accordingly
  if (message == "lane1 high") {
    digitalWrite(LANE1_PIN, HIGH);
    Serial.println("[ESP] LANE 1 (D2) is HIGH");
  } else if (message == "lane1 low") {
    digitalWrite(LANE1_PIN, LOW);
    Serial.println("[ESP] LANE 1 (D2) is LOW");
  } else if (message == "lane2 high") {
    digitalWrite(LANE2_PIN, HIGH);
    Serial.println("[ESP] LANE 2 (D4) is HIGH");
  } else if (message == "lane2 low") {
    digitalWrite(LANE2_PIN, LOW);
    Serial.println("[ESP] LANE 2 (D4) is LOW");
  } else if (message == "lane3 high") {
    digitalWrite(LANE3_PIN, HIGH);
    Serial.println("[ESP] LANE 3 (D5) is HIGH");
  } else if (message == "lane3 low") {
    digitalWrite(LANE3_PIN, LOW);
    Serial.println("[ESP] LANE 3 (D5) is LOW");
  } else if (message == "lane4 high") {
    digitalWrite(LANE4_PIN, HIGH);
    Serial.println("[ESP] LANE 4 (D18) is HIGH");
  } else if (message == "lane4 low") {
    digitalWrite(LANE4_PIN, LOW);
    Serial.println("[ESP] LANE 4 (D18) is LOW");
  }
}


void setup_mqtt() {
  Serial.println("\n[MQTT] Configuring MQTT connection...");

  espClient.setCACert(amazon_root_ca);
  espClient.setCertificate(certificate_pem_crt);
  espClient.setPrivateKey(private_pem_key);

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(message_callback);  // Set callback function

  Serial.println("[MQTT] Certificates set successfully.");
}

void reconnect_mqtt() {
  if (client.connected()) {
    return;
  }

  Serial.println("\n[MQTT] Connecting to MQTT broker...");
  int attempt = 0;

  while (!client.connected()) {
    Serial.print("[MQTT] Attempt ");
    Serial.print(attempt + 1);
    Serial.println(" to connect...");

    if (client.connect("ESP32Client")) {
      Serial.println("[MQTT] Successfully connected to AWS IoT.");

      // Subscribe to topics
      bool gpsUp = client.subscribe("gps/up");
      bool gpsPing = client.subscribe("gps/ping");  // Subscribe to gps/ping

      if (!gpsUp) {
        Serial.println("[MQTT] Failed to subscribe to gps/up.");
      }

      if (!gpsPing) {
        Serial.println("[MQTT] Failed to subscribe to gps/ping.");
      }
    } else {
      int state = client.state();
      Serial.print("[MQTT] Connection failed. Error code: ");
      Serial.println(state);

      Serial.println("[MQTT] Retrying in 5 seconds...");
      delay(5000);
    }

    attempt++;
    if (attempt > 5) {
      Serial.println("[MQTT] Failed after 5 attempts. Restarting ESP...");
      ESP.restart();
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n[System] Initializing...");

// Configure lane control pins as OUTPUT
  pinMode(LANE1_PIN, OUTPUT);
  pinMode(LANE2_PIN, OUTPUT);
  pinMode(LANE3_PIN, OUTPUT);
  pinMode(LANE4_PIN, OUTPUT);

  // Ensure all pins start LOW
  digitalWrite(LANE1_PIN, LOW);
  digitalWrite(LANE2_PIN, LOW);
  digitalWrite(LANE3_PIN, LOW);
  digitalWrite(LANE4_PIN, LOW);
  

  setup_wifi();
  setup_mqtt();
  reconnect_mqtt();
}

void loop() {
  client.loop();

  // Send a "ping" every 30 seconds to keep the connection alive
  static unsigned long lastPingTime = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - lastPingTime >= 30000) {  // 30-second interval
    Serial.println("[MQTT] Sending ping...");
    lastPingTime = currentMillis;
    client.publish("gps/ping", "{ \"message\":\"Recon mqt\" }");  // Dummy message
  }
  delay(500);
}
