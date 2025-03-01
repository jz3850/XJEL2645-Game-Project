#ifndef NOTE_H
#define NOTE_H

#include "mbed.h"
#include "N5110.h"
#include "Utils.h"  // for Position


/** Ball Class
@author Dr Craig A. Evans, University of Leeds
@brief Controls the ball in the Pong game 
@date Febraury 2017
*/ 

class Note{
public:
    Note();
    void init();
    void draw1(N5110 &lcd, int hit_check1);  //draw the notes
    void draw2(N5110 &lcd, int hit_check2);
    void draw3(N5110 &lcd, int hit_check3);
    void draw4(N5110 &lcd, int hit_check4);
    void update1();  //update the note positions and size
    void update2();
    void update3();
    void update4();
    void set_pos1(int x, int y);  //if the note is hit or missed, reset its position to make it flow again
    void set_pos2(int x, int y);
    void set_pos3(int x, int y);
    void set_pos4(int x, int y);
    /// accessors and mutators
    int get_y1();  //get the y position of the notes
    int get_y2();
    int get_y3();
    int get_y4();
    
private:
    float speed;
    int size1,size2,size3,size4;
    int i1,i2,i4;
    int y_check(int y_check);  //return the note size according to the input y position 
    int x1,x2,x3,x4;
    int y1,y2,y3,y4;
    int hit_check1_,hit_check2_,hit_check3_,hit_check4_;
};
#endif