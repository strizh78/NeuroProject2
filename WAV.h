//
//  WAV.h
//  NeuroProject
//
//  Created by Хайкова Светлана and Мансуров Рустам on 24.04.2018.
//  Copyright © 2018 Khaykova Svetlana and Mansurov Rustam. All rights reserved.
//

#ifndef WAV_h
#define WAV_h

typedef int16_t rawData;

struct WavHeader
{
    // RIFF chunk:
    char chunkID[4]; // "RIFF"
    ui chunkSize; // fileSize - 8
    char formatID[4]; // "WAVE"
    
    // fmt sub-chunk:
    char fmtChunkID[4]; // "fmt "
    ui fmtSize;
    usi audioFormat; // == 1 - non-compress
    usi numChannels; // mono = 1, stereo = 2, ...
    ui sampleRate; // sampling frequency
    ui bytesPerSec;
    usi blockAllign; // bits in sample by all channels
    usi bitsPerSample; // 8, 16, ... bits (accuracy of audio)
    
    // Отдел данных:
    char dataChunkID[4]; // "data"
    ui dataSize; // raw data size in bits
};

class WavFile
{
public:
    WavHeader header;
    vector<rawData> data;
    vector <ld> normalizedData;
    
    ui numberOfSamples;
    
public:
    void readFromFile(const string fileName);
    void readData(fstream& f);
    WavHeader getHeader() { return header; }
    
    ui getNumberOfSamples() { return numberOfSamples; }
    
    vector <rawData> getRawData() { return data; }
    vector <ld> getNormalizedData() { return normalizedData; }
};

///////////

void WavFile::readFromFile(const string fileName)
{
    fstream f;
    f.open(fileName.c_str(), ios::in | ios::binary);
    
    f.read((char*)(&header), sizeof(WavHeader));
    
    readData(f);
    
    f.close();
}

void WavFile::readData(fstream& f)
{
    numberOfSamples = 0;
    
    ui bytesPerSample = ceil(((ld)header.bitsPerSample / 8.0)); // количество байт в 1 семпле
    ui numberOfSamplesXChannels = header.dataSize / (header.numChannels * bytesPerSample);
    
    data.resize(numberOfSamplesXChannels);
    
    rawData maxValue = -10, minValue = 10;
    
    frn (i, numberOfSamplesXChannels)
    {
        if (f.eof()) break;
        if (header.numChannels == 1)
        {
            f.read((char*)(&data[i]), sizeof(rawData));
            numberOfSamples++;
        }
        else
        {
            rawData val1, val2;
            f.read((char*)(&val1), sizeof(rawData));
            f.read((char*)(&val2), sizeof(rawData));
            numberOfSamples++;
            data[i] = ((val1) + (val2)) / 2;
        }
        
        maxValue = max(maxValue, data[i]);
        minValue = min(minValue, data[i]);
    }
    
    // Нормализуем:
    normalizedData.resize(data.size());
    ld maxAbs = max(abs(minValue), abs(maxValue));
    frn (i, data.size())
        normalizedData[i] = (ld)data[i] / maxAbs;
    
}
#endif /* WAV_h */
