#include "Fertilizer.h"

float DEPTH = 15, DENSITY = 1.3;

/*
Memberikan hasil rekomendasi pupuk dari pembacaan NPK
Crop : 0 Cabai Merah, 1 Bawang Merah, 2 Bawang Putih, 3 Kubis, 4 Kentang
Fertilizer : 0 Urea, 1 SP-36, 2 KCl
*/
Recomendation recommendation(float urea, float sp36, float kcl, uint8_t crop, uint8_t loc, uint8_t isAndisol) {
    Recomendation hasil;
    uint16_t minUrea, minSP36, minKCl;
    uint16_t maxUrea, maxSP36, maxKCl;

    Serial.printf("%f, %f, %f, %u, %u, %u, %.2f, %.2f\n",urea, sp36, kcl, crop, loc, isAndisol, DEPTH, DENSITY);
    // Dataset Load
    minUrea = FERTILIZER[loc][isAndisol][0][crop][0];
    maxUrea = FERTILIZER[loc][isAndisol][1][crop][0];
    minSP36 = FERTILIZER[loc][isAndisol][0][crop][1];
    maxSP36 = FERTILIZER[loc][isAndisol][1][crop][1];
    minKCl = FERTILIZER[loc][isAndisol][0][crop][2];
    maxKCl = FERTILIZER[loc][isAndisol][1][crop][2];

    Serial.printf("Dataset Urea Min: %u, Max %u\n", minUrea, maxUrea);
    Serial.printf("Dataset SP36 Min: %u, Max %u\n", minUrea, maxUrea);
    Serial.printf("Dataset KCl Min: %u, Max %u\n", minUrea, maxUrea);
    //Rekomendasi Pupuk
    if(urea < minUrea) hasil.urea = (uint16_t)((float(maxUrea)+float(minUrea))/2) - urea;
    else hasil.urea = 0;
    if(sp36 < minSP36) hasil.sp36 = (uint16_t)((float(maxSP36)+float(minSP36))/2) - sp36;
    else hasil.sp36 = 0;
    if(kcl < minKCl) hasil.kcl = (uint16_t)((float(maxKCl)+float(minKCl))/2) - kcl;
    else hasil.kcl = 0;
    Serial.printf("Rekomendasi Pupuk %u, %u, %u\n", hasil.urea, hasil.sp36, hasil.kcl);
    //Status
    if(urea < minUrea || sp36 < minSP36 || kcl < minKCl) hasil.status = 0;
    else if(urea >= minUrea && urea <= maxUrea && sp36 >= minSP36 && sp36 <= maxSP36 && kcl >= minKCl && kcl <= maxKCl) hasil.status = 1;
    else if(urea > maxUrea || sp36 > maxSP36 || kcl > maxKCl) hasil.status = 2;
    Serial.printf("Status : %u\n", hasil.status);
    //Optimal NPK
    hasil.n =   String(int(doseToPpm(fertilizerToDose(1, float(FERTILIZER[loc][isAndisol][0][crop][0]))))) + "-" + 
                String(int(doseToPpm(fertilizerToDose(1, float(FERTILIZER[loc][isAndisol][1][crop][0])))));
    hasil.p =   String(int(doseToPpm(fertilizerToDose(2, float(FERTILIZER[loc][isAndisol][0][crop][1]))))) + "-" + 
                String(int(doseToPpm(fertilizerToDose(2, float(FERTILIZER[loc][isAndisol][1][crop][1])))));
    hasil.k =   String(int(doseToPpm(fertilizerToDose(3, float(FERTILIZER[loc][isAndisol][0][crop][2]))))) + "-" + 
                String(int(doseToPpm(fertilizerToDose(3, float(FERTILIZER[loc][isAndisol][1][crop][2])))));

    Serial.printf("Optimal : %s, %s, %s\n", hasil.n, hasil.p, hasil.k);
    return hasil;
}

/*
Perubahan PPM (mg/kg) ke Dosis (kg N/ha)
*/
float ppmToDose(float value) {
    return (value * DENSITY * DEPTH )/ 10;
}

/*
Perubahan Dosis (kg N/ha) ke PPM (mg/kg)
*/
float doseToPpm(float value) {
    return float(value)*10 / (DENSITY * DEPTH);
}

/*
Perubahan Dosis (kg N/ha) ke Pupuk
*/
float doseToFertilizer(uint8_t type, float value) {
    switch (type)
    {
    case 1: //N ke Urea
        return float(value)/46*100;
        break;
    case 2: //P ke SP36
        return float(value)/15.696*100;
        break;
    case 3: //K ke KCl
        return float(value)/49.8*100;
    default:
        break;
    }
    return 0;
}

/*
Perubahan Pupuk ke Dosis (kg N/ha)
*/
float fertilizerToDose(uint8_t type, float value) {
    switch (type)
    {
    case 1: //Urea ke N
        return float(value)*46/100;
        break;
    case 2: //SP36 ke P
        return float(value)*15.696/100;
        break;
    case 3: //KCL ke K
        return float(value)*53.4/100;
    default:
        break;
    }
    return 0;
}