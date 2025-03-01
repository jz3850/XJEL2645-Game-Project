#ifndef NOTEENGINE_H
#define NOTEENGINE_H

#include "mbed.h"
#include "N5110.h"
#include "Note.h"
#include "Utils.h"

class NoteEngine {
    public:
        NoteEngine();  // pass in the lcd object from the main file
        void init();
        void init_degree();
        void draw(N5110 &lcd);
        void score(int x);
        void set_hitcheck1(int x);  //If the note has been hit during the render of certain note flowing,
        void set_hitcheck2(int x);  //the hitcheck will be set to be 0 from 1, which indicates that
        void set_hitcheck3(int x);  //there is no feedback is given and no score is added again.
        void set_hitcheck4(int x);
        void set_degree(int x);
        int hitcheck1();  //get the hit check
        int hitcheck2();  
        int hitcheck3();
        int hitcheck4();
        int getscore();
        int getdegree();
        

    private:
        int sum, m, n, com, degree;
        char sum_[3], com_[2];
        int hit_check1_,hit_check2_,hit_check3_,hit_check4_;    
};

#endif