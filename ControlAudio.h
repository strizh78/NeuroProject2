//
//  ControlAudio.h
//  NeuroProject
//
//  Created by Хайкова Светлана and Мансуров Рустам on 25.04.2018.
//  Copyright © 2018 Khaykova Svetlana and Mansurov Rustam. All rights reserved.
//

#ifndef ControlAudio_h
#define ControlAudio_h

#include "WAV.h"
#include "frames&words.h"

class ControlAudio
{
public:
    WavFile wavData; // наш файл со всеми данными
    
    vector <Frame> frames; // фреймы : начало и конец куска данных фрейма
    
    vector <Word> words;
    
    ui bytesInFrame, bytesInSample, samplesInFrame;
    
public:
    ControlAudio(const string fileName);
    
    void splitIntoFrames();
    void splitIntoWords();
    
    void saveWordAsAudio(const std::string& file, Word& word);
};

#include "MathCalc.h"

ControlAudio::ControlAudio (const string fileName)
{
    wavData.readFromFile(fileName);
    
    bytesInFrame = ceil(wavData.getHeader().bytesPerSec * FRAME_LENGTH / 1000.0); // байт в секунду * длину фрейма в мс
    bytesInSample = ceil(wavData.getHeader().bitsPerSample / 8.0); // бит в семпле / 8 = байт в семпле
    samplesInFrame = bytesInFrame / bytesInSample; // байт в фрейме / байт в семпле
    
    splitIntoFrames();
    splitIntoWords();
}

void ControlAudio::splitIntoFrames()
{
    int samplesNonOverlap = samplesInFrame * (1 - FRAME_OVERLAP); // семплов в 1 фрейме не учитывая пересечение
    int frameCnt = (wavData.getHeader().dataSize / bytesInFrame);
    
    int beg = 0, end = 0;
    int size = wavData.getNumberOfSamples();
    frn (ID, frameCnt)
    {
        beg = ID * samplesNonOverlap;
        end = beg + samplesInFrame;
        if (end <= size) // если номер последнего семпла меньше количества семплов - добавляем фрейм
        {
            frames.pb(Frame(beg, end));
            frames[frames.size() - 1].entropy = entropyCalc(wavData.getNormalizedData(),
                                                            beg, end); // и считаем энтропию
        }
        else break;
    }
}

void ControlAudio::splitIntoWords()
{
    int beg = -1;

//    frs (frame, frames)
//    {
//        if (abs(frame->entropy) > 0.1)
//            cout << frame->entropy << '\n';
//        else cout << 0 << '\n';
//    }
    
    int frameCurId = 0;
    frs (frame, frames)
    {
        if (abs(frame->entropy) > 0.1) // если фрейм - не тишина
        {
                if (beg == -1)
                    beg = frameCurId;
        }
        else
            if (beg >= 0) // надо закончить слово:
            {
                if (words.size() == 0) // если еще не было слов:
                {
                    if( frameCurId - 1 - beg >= WORD_MIN_SIZE)
                        words.pb(Word(beg, frameCurId - 1));
                    beg = -1;
                    continue;
                }
                Word lastW = words[words.size() - 1];
                // 1. проверяем расстояние до предыдущего слова (указана в frames&words.h):
                if (beg - lastW.end >= WORDS_MIN_DISTANCE)
                {
                    // 2. прoверяем что длина слова больше минимальной (указана в frames&words.h):
                    if (frameCurId - 1 - beg >= WORD_MIN_SIZE)
                        words.pb(Word(beg, frameCurId - 1));
                }
                // иначе добавляем к предыдущему (или вообще за слово не считаем?)
                else
                    lastW.end = frameCurId - 1;
                beg = -1;
            }
        frameCurId++;
    }
    
    MFCCcalc(*this);
    
    int i = 0;
    frs (word, words)
    {
        i++;
        string filename = curDirectory + "word" + to_string(word->start) + ".wav";
        saveWordAsAudio(filename , *word);
    }
}

void ControlAudio::saveWordAsAudio(const string& file, Word& word)
{
    
    // number of data bytes in the resulting wave file
    ui samplesPerNonOverlap = (ui)(samplesInFrame * (1 - FRAME_OVERLAP));
    ui waveSize = word.size() * samplesPerNonOverlap * sizeof(rawData);
    
    // prepare a new header and write it to file stream
    WavHeader headerNew;
    strncpy(headerNew.chunkID, wavData.getHeader().chunkID, 4);
    headerNew.chunkSize = waveSize + sizeof(WavHeader);
    strncpy(headerNew.formatID, wavData.getHeader().formatID, 4);
    strncpy(headerNew.fmtChunkID, wavData.getHeader().fmtChunkID, 4);
    headerNew.fmtSize = wavData.getHeader().fmtSize;
    headerNew.audioFormat = wavData.getHeader().audioFormat;
    headerNew.numChannels = 1;
    headerNew.sampleRate = wavData.getHeader().sampleRate;
    headerNew.bytesPerSec = wavData.getHeader().sampleRate * sizeof(rawData);
    headerNew.blockAllign = sizeof(rawData);
    headerNew.bitsPerSample = sizeof(rawData) * 8;
    strncpy(headerNew.dataChunkID, wavData.getHeader().dataChunkID, 4);
    headerNew.dataSize = waveSize;
    
    fstream fl;
    fl.open(file, std::ios::out | std::ios::binary);
    fl.write((char*)&headerNew, sizeof(WavHeader));
    
    rawData* data = new rawData[waveSize / sizeof(rawData)];
    int j = 0;
    int start = frames[word.start].start;
    int finish = start + (samplesPerNonOverlap * word.size());
    frab (currentFrame, start, finish)
    {
        data[j++] = ((wavData.data[currentFrame]));
    }
    
    fl.write((char*)data, waveSize);
    fl.close();
    delete [] data;
}

#endif /* ControlAudio_h */

