#include <WiFiS3.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// ------ WiFi & SERVER -----
const char* ssid = "jghp";
const char* password = "papa123456";

WiFiServer server(80);

// ------ ACTUATORS ------
int fan1_GPIO = 13;
bool fan1_status = false;

int fan2_GPIO = 12;
bool fan2_status = false;

int light1_GPIO = 11; 
bool light1_status = false;

int light2_GPIO = 10;
bool light2_status = false;

int pump_GPIO = 9;
bool pump_status = false;

// ------ SENSORS -------
int water_level_sensor_GPIO = A0;
int temperature_humidity_GPIO = A1;

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// ------ AUTO MODE VARIABLES ------
unsigned long previousMillis = 0;
const long interval = 1000;
bool autoMode = true;

void setup() {
  Serial.begin(115200);

  // WIFI & SERVER
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  server.begin();

  // -- ACTUATORS --
  pinMode(fan1_GPIO, OUTPUT);
  pinMode(fan2_GPIO, OUTPUT);
  pinMode(light1_GPIO, OUTPUT);
  pinMode(light2_GPIO, OUTPUT);
  pinMode(pump_GPIO, OUTPUT);

  // Initialize DHT sensor
  dht.begin();
}

void loop() {
  unsigned long currentMillis = millis();

  // Auto mode logic
  if (autoMode) {
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      int currentHour = (currentMillis / 3600000) % 24;

      // Control lights
      if (currentHour >= 6 && currentHour < 18) {
        turnOnLight1();
        turnOnLight2();
      } else {
        turnOffLight1();
        turnOffLight2();
      }

      // Water plants at 7 AM
      if (currentHour == 7 && !pump_status) {
        waterPlants();
      }
    }
  }

  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client Connected.");
    String request = "";

    while (client.connected()) {
      if (client.available()) {
        request = client.readStringUntil('\r');
        Serial.println(request);
        break;
      }
    }

    // Route the request
    routeRequest(request, client);

    client.flush();
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

// ------------------ REQUEST HANDLING --------------------

void routeRequest(String request, WiFiClient& client) {
  // Parse the request line
  int firstSpace = request.indexOf(' ');
  int secondSpace = request.indexOf(' ', firstSpace + 1);
  String method = request.substring(0, firstSpace);
  String path = request.substring(firstSpace + 1, secondSpace);

  Serial.println("Method: " + method);
  Serial.println("Path: " + path);

  // Handle GET requests
  if (method == "GET") {
    if (path == "/") {
      handleRoot(client);
    } else if (path == "/fan1") {
      handleGetFan1(client);
    } else if (path == "/fan2") {
      handleGetFan2(client);
    } else if (path == "/light1") {
      handleGetLight1(client);
    } else if (path == "/light2") {
      handleGetLight2(client);
    } else if (path == "/pump") {
      handleGetPump(client);
    } else {
      handleNotFound(client);
    }
  }
  // Handle POST requests
  else if (method == "POST") {
    // Read the body of the request
    String requestBody = "";
    while (client.available()) {
      requestBody += client.readStringUntil('\r');
    }
    Serial.println("Request Body: " + requestBody);

    if (path == "/fan1") {
      handlePostFan1(client, requestBody);
    } else if (path == "/fan2") {
      handlePostFan2(client, requestBody);
    } else if (path == "/light1") {
      handlePostLight1(client, requestBody);
    } else if (path == "/light2") {
      handlePostLight2(client, requestBody);
    } else if (path == "/pump") {
      handlePostPump(client, requestBody);
    } else {
      handleNotFound(client);
    }
  }
  // Method not allowed
  else {
    sendResponse(client, 405, "application/json", "{\"error\": \"Method Not Allowed\"}");
  }
}

// ------------------ GET HANDLERS --------------------

void handleRoot(WiFiClient& client) {
  // Create JSON object with sensor and actuator data
  StaticJsonDocument<256> jsonDoc;
  jsonDoc["fan1_status"] = fan1_status;
  jsonDoc["fan2_status"] = fan2_status;
  jsonDoc["light1_status"] = light1_status;
  jsonDoc["light2_status"] = light2_status;
  jsonDoc["pump_status"] = pump_status;
  jsonDoc["water_level"] = analogRead(water_level_sensor_GPIO);
  jsonDoc["temperature_humidity"] = analogRead(temperature_humidity_GPIO);
  jsonDoc["humidity"] = dht.readHumidity();
  jsonDoc["temperatureF"] = dht.readTemperature(true);

  String response;
  serializeJson(jsonDoc, response);
  sendJsonResponse(client, 200, response);
}

void handleGetFan1(WiFiClient& client) {
  String content = "{\"id\": \"fan1\", \"name\": \"Fan 1\", \"status\": ";
  content += fan1_status ? "true" : "false";
  content += "}";
  sendJsonResponse(client, 200, content);
}

// Repeat similar GET handlers for fan2, light1, light2, pump

// ------------------ POST HANDLERS --------------------

void handlePostFan1(WiFiClient& client, String requestBody) {
  StaticJsonDocument<200> jsonDoc;
  DeserializationError error = deserializeJson(jsonDoc, requestBody);

  if (error) {
    sendJsonResponse(client, 400, "{\"error\": \"Invalid JSON\"}");
    return;
  }

  String action = jsonDoc["action"];
  if (action == "turn_on") {
    turnOnFan1();
    sendJsonResponse(client, 200, "{\"status\": \"Fan 1 turned on\"}");
  } else if (action == "turn_off") {
    turnOffFan1();
    sendJsonResponse(client, 200, "{\"status\": \"Fan 1 turned off\"}");
  } else {
    sendJsonResponse(client, 400, "{\"error\": \"Invalid action\"}");
  }
}

// Repeat similar POST handlers for fan2, light1, light2, pump

// ------------------ ACTUATOR CONTROL FUNCTIONS --------------------

void turnOnFan1() {
  digitalWrite(fan1_GPIO, HIGH);
  fan1_status = true;
}

void turnOffFan1() {
  digitalWrite(fan1_GPIO, LOW);
  fan1_status = false;
}

// Repeat similar control functions for fan2, light1, light2, pump

void waterPlants() {
  turnOnPump();
  delay(1500);
  turnOffPump();
}

// ------------------ RESPONSE FUNCTIONS --------------------

void sendResponse(WiFiClient& client, int statusCode, String contentType, String content) {
  String response;

  if (statusCode == 200) {
    response = "HTTP/1.1 200 OK\r\n";
  } else if (statusCode == 404) {
    response = "HTTP/1.1 404 Not Found\r\n";
  } else if (statusCode == 400) {
    response = "HTTP/1.1 400 Bad Request\r\n";
  } else if (statusCode == 405) {
    response = "HTTP/1.1 405 Method Not Allowed\r\n";
  } else {
    response = "HTTP/1.1 500 Internal Server Error\r\n";
  }

  response += "Content-Type: " + contentType + "\r\n";
  response += "Connection: close\r\n";
  response += "Content-Length: " + String(content.length()) + "\r\n";
  response += "\r\n";
  response += content;

  client.print(response);
}

void sendJsonResponse(WiFiClient& client, int statusCode, String jsonContent) {
  sendResponse(client, statusCode, "application/json", jsonContent);
}

void handleNotFound(WiFiClient& client) {
  sendJsonResponse(client, 404, "{\"error\": \"Not Found\"}");
}