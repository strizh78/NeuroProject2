//
//  frames&words.h
//  NeuroProject
//
//  Created by Хайкова Светлана and Мансуров Рустам on 30.05.2018.
//  Copyright © 2018 Khaykova Svetlana and Mansurov Rustam. All rights reserved.
//

#ifndef frames_words_h
#define frames_words_h

const int FRAME_LENGTH = 10;
const double FRAME_OVERLAP = 0.5;

class Frame
{
public:
    ld entropy;
    ui start, end;
    
public:
    Frame(ui st, ui end) : start(st), end(end) {}
    
    ui size() { return end - start; }
};

const usi WORD_MIN_SIZE = (150 / FRAME_LENGTH) / (1 - FRAME_OVERLAP);
const usi WORDS_MIN_DISTANCE = WORD_MIN_SIZE * 0.50;

class Word
{
public:
    ui start, end;
    vector <ld> MFCC;
    
    Word(ui st, ui end) : start(st), end(end) {}
    ui size() { return end - start; }
    bool operator < (Word a) { return start < a.start; }
};

#endif /* frames_words_h */
