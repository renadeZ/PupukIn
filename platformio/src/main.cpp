#include <ModbusMaster.h>
#include <Arduino.h>
#include "BluetoothSerial.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "Wire.h"
#include "Fertilizer.h"
#include "StatusBar.h"

unsigned long timeRun;
#define BATT_PIN 34
//Konfigurasi BT Serial
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to enable it.
#endif
BluetoothSerial BlueSerial;
#define BLUETOOH_ID "Pupukin_ITQ2B"
bool isConnected = false;
String receivedData;
//Konfigurasi OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
//Konfigurasi MAX485
ModbusMaster modbus;
#define TX_PIN 17
#define RX_PIN 16
#define DERE_PIN 19
#define NPN_PIN 15
#define DELAY_READING 1000
const uint8_t NIT = 0x1E;
const uint8_t PHO = 0x1F;
const uint8_t POT = 0x20;
float nit_val = 0, pho_val = 0, pot_val = 0;
float rec_nit = 0, rec_pho = 0, rec_pot = 0;
//Variabel Global
unsigned long lastTime1;
uint8_t sensorState, mode, recIteration=0;
uint8_t cropSelection, locSelection, andisolSelection;
Recomendation rek;

void preTrasmit(){digitalWrite(DERE_PIN, HIGH);}
void postTrasmit(){digitalWrite(DERE_PIN, LOW);}

uint16_t readNPK(uint16_t address){
    uint16_t result = modbus.readHoldingRegisters(address,1);
    if(result != modbus.ku8MBSuccess){
        Serial.println("!Sensor Tidak Terbaca");
    }
    return modbus.getResponseBuffer(0);
}

void drawStatusBar(){
    uint16_t battLevel = analogRead(BATT_PIN);
    switch (isConnected){
    case true:
        oled.drawBitmap(98, 13, bluetoothOn, STATUSBAR_BLUETOOTH_WIDTH,STATUSBAR_BLUETOOTH_HEIGHT, WHITE);
        break;
    case false:
        oled.drawBitmap(98, 13, bluetoothOff, STATUSBAR_BLUETOOTH_WIDTH,STATUSBAR_BLUETOOTH_HEIGHT, WHITE);
        break;
    }

    if(battLevel > (uint16_t)3305) oled.drawBitmap(96, 0, batteryFull, STATUSBAR_BATTERY_WIDTH,STATUSBAR_BATTERY_HEIGHT, WHITE);
    else if(battLevel > (uint16_t)3133) oled.drawBitmap(96, 0, batteryThreeQuarter, STATUSBAR_BATTERY_WIDTH,STATUSBAR_BATTERY_HEIGHT, WHITE);
    else if(battLevel > (uint16_t)2960) oled.drawBitmap(96, 0, batteryHalf, STATUSBAR_BATTERY_WIDTH,STATUSBAR_BATTERY_HEIGHT, WHITE);
    else if(battLevel > (uint16_t)2788) oled.drawBitmap(96, 0, batteryQuarter, STATUSBAR_BATTERY_WIDTH,STATUSBAR_BATTERY_HEIGHT, WHITE);
    else oled.drawBitmap(96, 0, batteryEmpty, STATUSBAR_BATTERY_WIDTH,STATUSBAR_BATTERY_HEIGHT, WHITE);
}

void drawProgress(uint8_t progress){
    switch(progress){
        case 0:
            oled.drawBitmap(0, 49, progressBar0, PROGRESS_WIDTH, PROGRESS_HEIGHT, WHITE);
            break;
        case 1:
            oled.drawBitmap(0, 49, progressBar1, PROGRESS_WIDTH, PROGRESS_HEIGHT, WHITE);
            break;
        case 2:
            oled.drawBitmap(0, 49, progressBar2, PROGRESS_WIDTH, PROGRESS_HEIGHT, WHITE);
            break;
        case 3:
            oled.drawBitmap(0, 49, progressBar3, PROGRESS_WIDTH, PROGRESS_HEIGHT, WHITE);
            break;
    }
    oled.display();
}


void setup(){
    //Setup PIN dan Blueetooth
    pinMode(DERE_PIN, OUTPUT);
    pinMode(NPN_PIN, OUTPUT);
    pinMode(BATT_PIN, INPUT);
    Serial.begin(115200);
    BlueSerial.begin(BLUETOOH_ID);
    Serial.printf("Bluetooth %s siap untuk pairing\n", BLUETOOH_ID);
    digitalWrite(DERE_PIN, LOW);
    //Setup OLED
    if(!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
        Serial.println(F("Alokasi SSD1306 gagal"));
        for(;;);
    }
    delay(1000);
    oled.clearDisplay();
    
    oled.setTextSize(2);
    oled.setTextColor(WHITE);
    oled.setCursor(5, 0);
    oled.println("=PUPUKIN=");
    oled.println("Starting..");
    oled.display();
    //Setup MAX485, RS485-to-TTL
    Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
    modbus.begin(1, Serial2);
    modbus.preTransmission(preTrasmit);
    modbus.postTransmission(postTrasmit);
    //Clear Buffer
    Serial.println("Startup");
    digitalWrite(NPN_PIN, LOW);
    readNPK(NIT);readNPK(PHO);readNPK(POT);
    Serial.println("Buffer Cleared");
    delay(100);
    digitalWrite(NPN_PIN, HIGH);
    //Ready
    Serial.println("Starting Device");
    delay(500);
    lastTime1 = millis();
    mode = 1;
}

void loop(){
    if(BlueSerial.connected()){
        if(!isConnected){
            Serial.println("Blueetooth Terkoneksi!");
            isConnected = true;
            mode = 1;
            sensorState = 0;
            lastTime1 = millis();
        }
        //Terima Data
        if(BlueSerial.available()) {
            receivedData = BlueSerial.readString();
            Serial.printf("Data Diterima: %s\n", receivedData);
            //Perubahan Mode
            if(receivedData == "m1") {
                mode = 1;
                sensorState = 0;
                lastTime1 = millis();
			}
			else if(receivedData.startsWith("m2")) {
				cropSelection = (uint8_t)(receivedData.charAt(3)-'0');
                locSelection = (uint8_t)(receivedData.charAt(5)-'0');
                andisolSelection = (uint8_t)(receivedData.charAt(7)-'0');
                Serial.printf("Crop : %u\n", cropSelection);
                Serial.printf("Loc : %u\n", locSelection);
                Serial.printf("Andisol : %u\n", andisolSelection);
                if(andisolSelection == 1) DENSITY = 0.8;
                else if(andisolSelection == 0) DENSITY = 1.3;
                mode = 2;
                sensorState = 0;
                lastTime1 = millis();

                oled.clearDisplay(); oled.setCursor(0,0);
                oled.println("=Rekomen=");
                oled.println("Data :");
                oled.printf("%d / 10", recIteration);
                oled.display();
			}
        }
    }
    else{
        if(isConnected){
            Serial.println("Bluetooth Terputus");
            mode = 1;
            isConnected = false;
        }
    }
    //Mode
    //Baca Sensor
    if(mode == 1){
        if(millis()-lastTime1 > DELAY_READING && sensorState ==0) {
            nit_val = readNPK(NIT);
            sensorState = 1;
            lastTime1 = millis();
            drawProgress(1);
        }
        else if(millis()-lastTime1 > DELAY_READING && sensorState ==1) {
            pho_val = readNPK(PHO);
            sensorState = 2;
            lastTime1 = millis();
            drawProgress(2);
        }
        else if(millis()-lastTime1 > DELAY_READING && sensorState ==2) {
            pot_val = readNPK(POT);
            sensorState = 3;
            lastTime1 = millis();
            drawProgress(3);
        }
        else if(sensorState == 3){
            //Serial Monitor
            Serial.printf("N: %f\n", nit_val);
            Serial.printf("P: %f\n", pho_val);
            Serial.printf("K: %f\n", pot_val);
            //Oled Display
            oled.clearDisplay(); oled.setCursor(0,0);
            oled.printf("N: %.0f \nP: %.0f\nK: %.0f", nit_val, pho_val, pot_val);
            drawStatusBar();
            oled.display();
            drawProgress(0);
            //Kirim Bluetooth
            if(isConnected) BlueSerial.printf("%.0f,%.0f,%.0f",nit_val, pho_val, pot_val);
            //Update Statement
            sensorState = 0;
            lastTime1 = millis();
        }
    }
    //Rekomendasi
    else if(mode == 2){
        
        //Mengambil sampel
		if(recIteration < 10){
            if(millis()-lastTime1 > DELAY_READING && sensorState ==0) {
                rec_nit += readNPK(NIT);
				sensorState = 1;
                lastTime1 = millis();
                drawProgress(1);
			}
			else if(millis()-lastTime1 > DELAY_READING && sensorState ==1) {
				rec_pho += readNPK(PHO);
                lastTime1 = millis();
				sensorState = 2;
                drawProgress(2);
			}
			else if(millis()-lastTime1 > DELAY_READING && sensorState ==2) {
                rec_pot += readNPK(POT);
				sensorState = 3;
                lastTime1 = millis();
                drawProgress(3);
			}
			else if(sensorState == 3){
                //Update statement
                Serial.println(recIteration);
				sensorState  = 0;
				lastTime1 = millis();
				recIteration++;
                oled.clearDisplay(); oled.setCursor(0,0);
                oled.println("=Rekomen=");
                oled.println("Data :");
                oled.printf("%d / 10", recIteration);
                oled.display();
                drawProgress(0);
			}
		}
		else {
            timeRun = millis();
			//Rata rata sampel
            Serial.println("Nilai NPK:");
			rec_nit = rec_nit/10;
			rec_pho = rec_pho/10;
			rec_pot = rec_pot/10;
            Serial.printf("%.1f %.1f %.1f\n", rec_nit, rec_pho, rec_pot);

			//Konversi ke pupuk
            ////PPM ke Dosis
            Serial.println("Konversi NPK ke Kadar Unsur:");
            rec_nit = ppmToDose(rec_nit);
            rec_pho = ppmToDose(rec_pho);
            rec_pot = ppmToDose(rec_pot);
            Serial.printf("%.1f %.1f %.1f\n", rec_nit, rec_pho, rec_pot);
            ////Dosis ke Pupuk
            Serial.println("Konversi Kadar Unsur ke Dosis Pupuk:");
            rec_nit = doseToFertilizer(1, rec_nit);
            rec_pho = doseToFertilizer(2, rec_pho);
            rec_pot = doseToFertilizer(3, rec_pot);
            Serial.printf("%.1f %.1f %.1f\n", rec_nit, rec_pho, rec_pot);
			//Rekomendasi berdasarkan tanaman
            Serial.println("Rekomendasi");
			rek = recommendation(rec_nit, rec_pho, rec_pot, cropSelection, locSelection, andisolSelection);
			
            Serial.println("rek selesai");
            Serial.println(rek.n);
            Serial.println(rek.p);
            Serial.println(rek.k);
			//Kirim Data
            String dataToSend = "done,"+String(rek.urea)+","+String(rek.sp36)+","+String(rek.kcl)+","+rek.n+","+rek.p+","+rek.k+","+String(rek.status);
            Serial.println(dataToSend);
            if(isConnected){
                Serial.println("Kirim");
                BlueSerial.print(dataToSend);
            }
            //Statement
            Serial.printf("Selesai\nWaktu pemrosesan rekomendasi : %u ms\n", millis()-timeRun);
            Serial.println("Kelar");
            recIteration = 0;
            mode = 1;
            sensorState = 0;
            lastTime1 = millis();
		}
    }
}