//
//  Word.h
//  NeuroProject
//
//  Created by Хайкова Светлана on 25.04.2018.
//  Copyright © 2018 Khaykova Svetlana. All rights reserved.
//

#ifndef Word_h
#define Word_h

const usi WORD_MIN_SIZE = (200 / FRAME_LENGTH) / (1 - FRAME_OVERLAP);
const usi WORDS_MIN_DISTANCE = WORD_MIN_SIZE * 0.50;

class Word
{
public:
    ui ID;
    ui start, end;
    ld entropy;
    
    vector <ll> MFCC;
    
    Word(ui ID, ui st, ui end) : ID(ID), start(st), end(end) {}
    
};

#endif /* Word_h */
