#include "Note.h"
Note::Note(){}
//Initialisation
void Note::init() {
    speed = 1;
    size1 = 2; size2 = 2; size3 = 2; size4 = 2;
    i1=0; i2=0; i4=0;  //reference variables to help judge the displacement of x position in a single render
    x1 = 28; x2 = 36; x3 = 43; x4 = 50;  //x positions
    y1=48; y2=48; y3=48; y4=48;    //The four notes are initialised to be 48 that are out of the screen, 
    //when the the input note in the main function reset the y position to be 0, the corresponding note begins to slow
    hit_check1_=1; hit_check2_=1; hit_check3_=1; hit_check4_=1;    //hit check=1 indicates that the note is flowing
}
//Draw the four note paths
void Note::draw1(N5110 &lcd, int hit_check1) {
    hit_check1_ = hit_check1_ * hit_check1;  //Once the note is hit, hit_check = 0, 
    if(hit_check1_==1){  //the note will no longer display until hit_check is reset to be 1
    lcd.drawRect(x1,y1,size1*2.5,size1,FILL_BLACK);
    }
}
void Note::draw2(N5110 &lcd, int hit_check2) {
    hit_check2_ = hit_check2_ * hit_check2;
    if(hit_check2_==1){
    lcd.drawRect(x2,y2,size2*2.5,size2,FILL_BLACK);
    }
}
void Note::draw3(N5110 &lcd, int hit_check3) {
    hit_check3_ = hit_check3_ * hit_check3;
    if(hit_check3_==1){
    lcd.drawRect(x3,y3,size3*2.5,size3,FILL_BLACK);
    }
}
void Note::draw4(N5110 &lcd, int hit_check4) {
    hit_check4_ = hit_check4_ * hit_check4;
    if(hit_check4_==1){
    lcd.drawRect(x4,y4,size4*2.5,size4,FILL_BLACK);
    }
}
//Update the four note paths
void Note::update1(){
    size1 = Note::y_check(y1);
    if(hit_check1_==1){
        i1++;
        if(i1%7==0){  //x1 moves left for 15 pixels in 7 refreshs
            x1 -= speed*3;
        }
        else{
            x1 -= speed*2;    
        }
        if (y1<48){  //y1 drops for 28 pixels in 7 refreshs
            y1 += speed*4;
        }
        else{  //the note is missed
            y1 = 48;
            hit_check1_=1;    
            size1 = 2;    //Once the note reaches the buttom, reset the hit check to be 1, otherwise this path will never 
        //generate notes again; reset the size to be 2, otherwise the note will be very large when it appear on this path next time
        }
    }
    else{  //the note is hit, and the parameters should also be reset
        y1=48;
        hit_check1_=1;
        size1 = 2; 
    }
}
void Note::update2(){
    size2 = Note::y_check(y2);
    if(hit_check2_==1){
        i2++;
        if(i2%2==1){
            x2 -= speed*2;
        }
        if (y2<48){
            y2 += speed*4;
        }
        else{
            y2 = 48;
            hit_check2_=1;
            size2 = 2; 
        }
    }
    else{
        y2 = 48;
        hit_check2_=1;
        size2 = 2; 
    }
}
void Note::update3(){
    size3 = Note::y_check(y3);
    if(hit_check3_==1){
        if (y3<48){
            y3 += speed*4;
        }
        else{
            y3 = 48;
            hit_check3_=1;
            size3 = 2; 
        }
    }
    else{
        y3 = 48;
        hit_check3_=1;
        size3 = 2; 
    }
}
void Note::update4(){
    size4 = Note::y_check(y4);
    if(hit_check4_==1){
        i4++;
        if(i4%2==1){
            x4 += speed*2;
        }
        if (y4<48){
            y4 += speed*4;
        }
        else{
            y4 = 48;
            hit_check4_=1;
            size4 = 2; 
        }
    }
    else{
         y4 = 48;
         hit_check4_=1;
         size4 = 2; 
    }
}
//Get position feedback from the main.cpp
void Note::set_pos1(int x, int y){
    x1=x;
    y1=y;
}
void Note::set_pos2(int x, int y){
    x2=x;
    y2=y;
}
void Note::set_pos3(int x, int y){
    x3=x;
    y3=y;
}
void Note::set_pos4(int x, int y){
    x4=x;
    y4=y;
}
//Check the y position to adjust the size
int Note::y_check(int y_check){
    if (y_check<8){
        return 2;
    }
    else if (y_check<16){
        return 3;
    }
    else if (y_check<24){
        return 4;
    }
    else if (y_check<32){
        return 5;
    }
    else{
        return 6;
    }
}
//Return functions
int Note::get_y1() { return y1; }
int Note::get_y2() { return y2; }
int Note::get_y3() { return y3; }
int Note::get_y4() { return y4; }