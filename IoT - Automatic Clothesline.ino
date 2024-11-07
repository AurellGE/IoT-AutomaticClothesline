#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6_cH1bhMi"
#define BLYNK_TEMPLATE_NAME "IoT Project"

#include <Stepper.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

const int stepsPerRevolution = 4000;  // Jumlah langkah per revolusi motor

// Pin untuk driver motor ULN2003
#define IN1 19
#define IN2 18
#define IN3 5
#define IN4 17

int int_cahaya = 34;
int air = 35;

// Inisialisasi library stepper
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);

// Variabel Blynk
char auth[] = "TuMqiD8SOrioFYFT1NLsVdaKFBp4KaQS";  // Masukkan token Blynk Anda
char ssid[] = "Lycia";         // Masukkan nama WiFi Anda
char pass[] = "lmgswsspltgcf";     // Masukkan password WiFi Anda

// Batas tetap kalibrasi sensor
const int bawah = 0;
const int atas = 1024;

int isStepperTriggered = 0;  // Variabel untuk memeriksa apakah stepper sudah berjalan

void setup() {
    pinMode(int_cahaya, INPUT);
    pinMode(air, INPUT);
    myStepper.setSpeed(10);        // Atur kecepatan stepper dalam RPM
    Serial.begin(115200);         // Inisialisasi komunikasi serial

    // Koneksi ke Blynk
    Blynk.begin(auth, ssid, pass);
}

void loop() {
    Blynk.run();  // Jalankan fungsi Blynk

    int data_air = analogRead(air);
    int range = map(data_air, bawah, atas, 0, 3);
    int cahaya = analogRead(int_cahaya);

    Serial.print("Range: ");
    Serial.print(range);
    Serial.print(" | Cahaya: ");
    Serial.print(cahaya);
    Serial.print(" | Status: ");
    Serial.println(isStepperTriggered);
    
    // Kirim data sensor ke Blynk
    Blynk.virtualWrite(V0, range);     // Mengirim nilai air ke Virtual Pin V0
    Blynk.virtualWrite(V1, cahaya);    // Mengirim nilai cahaya ke Virtual Pin V1
    Blynk.virtualWrite(V2, isStepperTriggered); 

    // Kondisi untuk memicu motor
    if ((cahaya <= 2000) && (range == 11) && isStepperTriggered == 0) {
        // Stepper hanya aktif satu kali ketika kondisi terpenuhi
        Serial.println("TRIGGER");
        Serial.println("clockwise");
        
        int forwardCount = 4;
        for (int i = 0; i < forwardCount; i++) {
          myStepper.step(stepsPerRevolution); // Move stepper forward
        }

        delay(1000);
        isStepperTriggered = 1;  // Tandai bahwa stepper sudah aktif
    } 
    // Kondisi untuk mengembalikan motor ke posisi semula
    else if ((cahaya > 2000 || range < 6) && isStepperTriggered == 1) {
        Serial.println("TRIGGER");
        Serial.println("counterclockwise");
        
        int retractCount = 4;
        for (int i = 0; i < retractCount; i++) {
          myStepper.step(-stepsPerRevolution); // Move stepper backward
        }

        delay(1000);
        isStepperTriggered = 0;  // Reset kondisi setelah stepper kembali
    }
}