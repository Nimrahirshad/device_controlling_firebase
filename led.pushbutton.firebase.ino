#include <WiFi.h>
#include <FirebaseESP32.h>
#include <Bounce2.h>

// WiFi credentials
const char* ssid = "Director General";
const char* password = "dgklcmis";

// Firebase credentials
#define FIREBASE_HOST "led-control-f3b7b-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "zIz3jO0eT8nD1czzj78ThrFbIeErwAfAO7Im4uOO"

// Firebase path
#define LED_PATH "/data"

// Initialize Firebase
FirebaseData firebaseData;

// Constants
const int ledPin = 25;
const int buttonPin1 = 27;
const int buttonPin2 = 26;

// Variables
int ledState = LOW;
Bounce button1 = Bounce();
Bounce button2 = Bounce();

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  // Initialize pins
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);

 // Initialize button debouncers
  button1.attach(buttonPin1);
  button1.interval(02); // Adjust debounce time as needed
  button2.attach(buttonPin2);
  button2.interval(02); // Adjust debounce time as needed
}

void loop() {
  // Handle Firebase LED control
  if (Firebase.getString(firebaseData, LED_PATH)) {
    String ledStatus = firebaseData.stringData();
    if (ledStatus == "on") {
      ledState = HIGH;
      digitalWrite(ledPin, ledState);
      Serial.println("LED turned on");
    } else if (ledStatus == "off") {
      ledState = LOW;
      digitalWrite(ledPin, ledState);
      Serial.println("LED turned off");
    }
  }

  // Read button states with debouncing
  button1.update();
  button2.update();

  if (button1.fell()) {
    // Button 1 pressed
    ledState = HIGH;
    Firebase.setString(firebaseData, LED_PATH, "on");
  }

  if (button2.fell()) {
    // Button 2 pressed
    ledState = LOW;
    Firebase.setString(firebaseData, LED_PATH, "off");
  }

  // Update LED based on ledState
  digitalWrite(ledPin, ledState);

  // Delay before checking again
  delay(500); // Adjust the delay as needed for responsiveness
}
