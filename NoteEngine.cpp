#include "NoteEngine.h"

NoteEngine::NoteEngine(){}

void NoteEngine::init(){
    sum = 0;
    sum_[3] = 0;
    m = 0;
    n = 0;
    com = 0;
    com_[2] = 0;
    hit_check1_=1; hit_check2_=1; hit_check3_=1; hit_check4_=1;    //hit check=1 indicates that the note is flowing
}

void NoteEngine::init_degree(){
    degree = 1;
}

void NoteEngine::draw(N5110 &lcd) {
    // draw the interface
    lcd.drawLine(0,0,83,0,1);  // top
    lcd.drawLine(83,0,83,47,1);  // right
    lcd.drawLine(0,47,83,47,1); // bottom
    lcd.drawLine(0,0,0,47,1);  // left
    //draw the four note path
    lcd.drawLine(27,0,0,47,1);
    lcd.drawLine(34,0,20,47,1);
    lcd.drawLine(41,0,41,47,1);
    lcd.drawLine(48,0,62,47,1);
    lcd.drawLine(55,0,83,47,1);
    //draw the callibration area
    lcd.drawLine(9,31,73,31,1);
    lcd.drawLine(5,39,78,39,1);
    //draw the score
    sprintf(sum_, "%3d", sum); 
    lcd.printString(sum_, 65, 1);
    lcd.refresh();
    //draw the combo count
    sprintf(com_, "%d", com); 
    lcd.printString(com_, 5, 1);
    lcd.refresh();
}

void NoteEngine::set_hitcheck1(int x){
    hit_check1_ = x;
}

void NoteEngine::set_hitcheck2(int x){
    hit_check2_ = x;
}

void NoteEngine::set_hitcheck3(int x){
    hit_check3_ = x;
}

void NoteEngine::set_hitcheck4(int x){
    hit_check4_ = x;
}

void NoteEngine::set_degree(int x){
    degree = x;
}

int NoteEngine::hitcheck1(){
    return hit_check1_;
}

int NoteEngine::hitcheck2(){
    return hit_check2_;
}

int NoteEngine::hitcheck3(){
    return hit_check3_;
}

int NoteEngine::hitcheck4(){
    return hit_check4_;
}

int NoteEngine::getscore(){
    return sum;
}

int NoteEngine::getdegree(){
    return degree;
}

void NoteEngine::score(int x){
    if (x == 0){  //miss, the combo is break, no score added and the bonus point is reset to be 0
        n = 0;
        m = 0;
        com = 0;
    }
    else{  //press, the combo is added, the score is added, the bonus point is added but it cannot be greater than 3
        com ++;
        if (m < 3){
            m++;
        }
        n = x + m;
    }
    sum += n;
}

