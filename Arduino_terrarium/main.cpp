// PLEASE NOTE THAT YOU MUST HAVE THE FOLLOWING LIBRARIES INSTALLED
// 1. WiFiS3
//    - Open the Arduino IDE
//    - Go to Sketch > Include Library > Manage Libraries...
//    - Search for "WiFiS3" and install it
// 2. Arduino.h (This is a built-in library)
// 3. ArduinoJson
//    - Open the Arduino IDE
//    - Go to Sketch > Include Library > Manage Libraries...
//    - Search for "ArduinoJson" and install it
// 4. Adafruit Unified Sensor
//    - Open the Arduino IDE
//    - Go to Sketch > Include Library > Manage Libraries...
//    - Search for "Adafruit Unified Sensor" and install it
// 5. DHT sensor library
//    - Open the Arduino IDE
//    - Go to Sketch > Include Library > Manage Libraries...
//    - Search for "DHT sensor library" and install it

#include <WiFiS3.h>
#include <Arduino.h>
#include <ArduinoJson.h> // <-- Added for JSON handling
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// ------ WiFi & SERVER -----
const char* ssid = "Jghp";
const char* password = "papa123456";

WiFiServer server(80); // <-- Added to create a server that listens on port 80

// ------ ACTUATORS ------
int fan1_GPIO = 13;
bool fan1_status = true;

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

#define DHTPIN 4  // pin number
#define DHTTYPE DHT22 // DHT22 sensor 

// Initialize DHT 
DHT dht(DHTPIN, DHTTYPE);

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
  server.begin(); // <-- Added to start the server

  // -- ACTUATORS --
  pinMode(fan1_GPIO, OUTPUT);
  pinMode(fan2_GPIO, OUTPUT);
  pinMode(light1_GPIO, OUTPUT);
  pinMode(light2_GPIO, OUTPUT);
  pinMode(pump_GPIO, OUTPUT);

  // Initialize DHT sensor
  dht.begin(); // <-- Added to initialize the DHT sensor
}

void loop() {
  WiFiClient client = server.available(); // <-- Added to check if a client has connected
  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read(); // <-- Added to read a byte from the client
        Serial.write(c); // <-- Added to print the byte to the serial monitor
        if (c == '\n') {
          if (currentLine.length() == 0) {
            // Read sensor data
            float humidity = dht.readHumidity();
            float temperatureF = dht.readTemperature(true); // true for Fahrenheit

            // Check if any reading failed
            if (isnan(humidity) || isnan(temperatureF)) {
              Serial.println(F("Failed to read from DHT sensor!")); // F() saves memory
              return;
            }

            // Send HTTP response
            client.println("HTTP/1.1 200 OK"); // <-- Added to send HTTP response
            client.println("Content-Type: application/json"); // <-- Added to set content type
            client.println("Connection: close"); // <-- Added to close connection after response
            client.println();

            // Create JSON object
            StaticJsonDocument<200> jsonDoc; // <-- Added to create JSON object
            jsonDoc["fan1_status"] = fan1_status;
            jsonDoc["fan2_status"] = fan2_status;
            jsonDoc["light1_status"] = light1_status;
            jsonDoc["light2_status"] = light2_status;
            jsonDoc["pump_status"] = pump_status;
            jsonDoc["water_level"] = analogRead(water_level_sensor_GPIO);
            jsonDoc["temperature_humidity"] = analogRead(temperature_humidity_GPIO);
            jsonDoc["humidity"] = humidity; // <-- Added humidity data
            jsonDoc["temperatureF"] = temperatureF; // <-- Added temperature data

            // Serialize JSON and send to client
            serializeJson(jsonDoc, client); // <-- Added to serialize JSON and send to client
            break;
          } else {
            currentLine = ""; // <-- Added to clear the current line
          }
        } else if (c != '\r') {
          currentLine += c; // <-- Added to add character to current line
        }
      }
    }
    client.stop(); // <-- Added to close the connection
    Serial.println("Client Disconnected.");
  }
}

// Helper function to send HTTP response
void sendResponse(WiFiClient& client, int statusCode, String contentType, String content) {
    String response;

    // Constructing the status line with the appropriate message for the status code
    if (statusCode == 200) {
        response = "HTTP/1.1 200 OK\r\n";
    } else if (statusCode == 404) {
        response = "HTTP/1.1 404 Not Found\r\n";
    } else if (statusCode == 400) {
        response = "HTTP/1.1 400 Bad Request\r\n";
    } else {
        response = "HTTP/1.1 500 Internal Server Error\r\n";
    }

    // Add common headers
    response += "Content-Type: " + contentType + "\r\n";
    response += "Connection: close\r\n";
    response += "Content-Length: " + String(content.length()) + "\r\n";  // Content-Length is important for proper parsing
    response += "\r\n";  // Blank line to indicate the end of headers
    
    // Add the actual HTML content
    response += content;

    Serial.println(response);

    // Send the complete response to the client
    client.print(response);
}

// Function to send a JSON response
void sendJsonResponse(WiFiClient& client, int statusCode, String jsonContent) {
    String response;

    // Constructing the status line with the appropriate message for the status code
    if (statusCode == 200) {
        response = "HTTP/1.1 200 OK\r\n";
    } else if (statusCode == 404) {
        response = "HTTP/1.1 404 Not Found\r\n";
    } else if (statusCode == 400) {
        response = "HTTP/1.1 400 Bad Request\r\n";
    } else {
        response = "HTTP/1.1 500 Internal Server Error\r\n";
    }

    // Add common headers
    response += "Content-Type: application/json\r\n";
    response += "Connection: close\r\n";
    response += "Content-Length: " + String(jsonContent.length()) + "\r\n";
    response += "\r\n";  // Blank line to indicate the end of headers

    // Add the JSON content
    response += jsonContent;

    // Send the complete response to the client
    client.print(response);
}

// -------- GET HANDLERS ------------

// Function to handle root path ("/")
void handleRoot(WiFiClient& client) {
  String content = "<h1>Welcome to Arduino Uno R4 WiFi Server</h1>";
  content += "<p>This is the root page.</p>";
  sendResponse(client, 200, "text/html", content);
}

// Function to handle "/about" path
void handleAbout(WiFiClient& client) {
  String content = "<h1>About Page</h1>";
  content += "<p>Arduino Uno R4 WiFi.</p>";
  sendResponse(client, 200, "text/html", content);
}

// Function to handle not found (404)
void handleNotFound(WiFiClient& client) {
  String content = "<h1>404 Not Found</h1>";
  content += "<p>The requested resource was not found on this server.</p>";
  sendResponse(client, 404, "text/html", content);
}

void handleGetFan1(WiFiClient& client) {
  String content = "";
  if(fan1_status) {
    content += "{\"id\": \"fan1\", \"name\": \"Fan1\", \"status\": \"true\"}";
  } else {
    content += "{\"id\": \"fan1\", \"name\": \"Fan1\", \"status\": \"false\"}";
  }

  Serial.println(content);
  sendJsonResponse(client, 200, content);
}

void handleGetFan2(WiFiClient& client) {
  String content = "";
  if(fan2_status) {
    content += "{\"id\": \"fan2\", \"name\": \"Fan2\", \"status\": \"true\"}";
  } else {
    content += "{\"id\": \"fan2\", \"name\": \"Fan2\", \"status\": \"false\"}";
  }
  sendJsonResponse(client, 200, content);
}

void handleGetLight1(WiFiClient& client) {
  String content = "";
  if(light1_status) {
    content += "{\"id\": \"light1\", \"name\": \"Light 1\", \"status\": \"true\"}";
  } else {
    content += "{\"id\": \"light1\", \"name\": \"Light 1\", \"status\": \"false\"}";
  }
  sendJsonResponse(client, 200, content);
}

void handleGetLight2(WiFiClient& client) {
  String content = "";
  if(light2_status) {
    content += "{\"id\": \"light2\", \"name\": \"Light 2\", \"status\": \"true\"}";
  } else {
    content += "{\"id\": \"light2\", \"name\": \"Light 2\", \"status\": \"false\"}";
  }
  sendJsonResponse(client, 200, content);
}

void handleGetPump(WiFiClient& client) {
  String content = "";
  if(pump_status) {
    content += "{\"id\": \"pump\", \"name\": \"Pump\", \"status\": \"true\"}";
  } else {
    content += "{\"id\": \"pump\", \"name\": \"Pump\", \"status\": \"false\"}";
  }
  sendJsonResponse(client, 200, content);
}

// ------------------ POST HANDLERS --------------------

void handlePostFan1(WiFiClient& client) {
  String content = "";

}

// Simple router function
void routeRequest(String request, WiFiClient& client) {
  // Extract the first line of the request (the request line)
  int endOfFirstLine = request.indexOf('\r');
  String requestLine = request.substring(0, endOfFirstLine);
  
  // Check which endpoint the request is targeting
  if (requestLine.startsWith("GET / ")) {
    handleRoot(client);
  } else if (requestLine.startsWith("GET /fan1 ")) {
    handleGetFan1(client);
  } else if (requestLine.startsWith("GET /fan2 ")) {
    handleGetFan2(client);
  } else if (requestLine.startsWith("GET /light1 ")) {
    handleGetLight1(client);
  } else if (requestLine.startsWith("GET /light2 ")) {
    handleGetLight2(client);
  } else if (requestLine.startsWith("GET /pump ")) {
    handleGetPump(client);
  } else {
    handleNotFound(client);
  }

  if(requestLine.startsWith("POST /fan1 ")) {

  }
}

void turnOnFan1() {
  digitalWrite(fan1_GPIO, HIGH);
  fan1_status = true;
}

void turnOffFan1() {
  digitalWrite(fan1_GPIO, LOW);
  fan1_status = false;
}

void turnOnFan2() {
  digitalWrite(fan2_GPIO, HIGH);
  fan2_status = true;
}

void turnOffFan2() {
  digitalWrite(fan2_GPIO, LOW);
  fan2_status = false;
}

void turnOnLight1() {
  digitalWrite(light1_GPIO, HIGH);
  light1_status = true;
}

void turnOffLight1() {
  digitalWrite(light1_GPIO, LOW);
  light1_status = false;
}

void turnOnLight2() {
  digitalWrite(light2_GPIO, HIGH);
  light2_status = true;
}

void turnOffLight2() {
  digitalWrite(light2_GPIO, LOW);
  light2_status = false;
}

void turnOnPump() {
  digitalWrite(pump_GPIO, HIGH);
  pump_status = true;
}

void turnOffPump() {
  digitalWrite(pump_GPIO, LOW);
  pump_status = false;
}

void waterPlants() {
  turnOnPump();
  delay(1500);
  turnOffPump();
}