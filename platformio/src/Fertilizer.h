#ifndef FERTILIZER_H
#define FERTILIZER_H

#include <Arduino.h>

//[Location][Andisol][MIN/MAX][Crop][NPK]
const uint16_t FERTILIZER[2][2][2][5][3] =
{
    //Kab Bandung
    {   
        //Non Andisol
        {{{135,149,135},{135,167,90},{167,252,90},{135,90,54},{203,473,203}},
        {{165,182,165},{165,204,110},{204,308,110},{165,110,66},{248,578,248}}},
        //Andisol
        {{{117,135,135},{149,203,180},{167,252,90},{135,167,68},{180,405,167}},
        {{143,165,165},{182,248,220},{204,308,110},{165,204,83},{220,495,204}}}
    },
    //Kab Bandung Barat
    {   
        //Non Andisol
        {{{135,149,167},{149,203,135},{167,252,135},{135,90,90},{203,473,252}},
        {{165,182,204},{182,248,165},{204,308,165},{165,110,110},{248,578,308}}},
        //Andisol
        {{{117,135,149},{149,203,203},{167,252,135},{135,167,90},{180,405,225}},
        {{143,165,182},{182,248,248},{204,308,165},{165,204,110},{220,495,275}}}
    }
};

extern float DEPTH, DENSITY;

typedef struct{
    uint16_t urea, sp36, kcl, status;
    String n,p,k;
}Recomendation;

Recomendation recommendation(float urea, float sp36, float kcl, uint8_t crop, uint8_t loc, uint8_t isAndisol);
float ppmToDose(float value);
float doseToPpm(float value);
float doseToFertilizer(uint8_t type, float value);
float fertilizerToDose(uint8_t type, float value);



#endif