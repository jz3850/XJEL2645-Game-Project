///////////// includes //////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "mbed.h"
#include "Joystick.h"
#include "N5110.h"
#include "Utils.h"
#include "Music.h"
#include "NoteEngine.h"
#include "Image.h"
///////////// objects //////////////////////////////////////////////////////////////////////////////////////////////////////////
N5110 lcd(PC_7, PA_9, PB_10, PB_5, PB_3, PA_10);
Joystick joystick(PC_1, PC_0);
DigitalIn Joystick(PB_0);
InterruptIn buttonA(BUTTON1);
volatile int g_buttonA_flag = 0; void buttonA_isr(){g_buttonA_flag = 1;}
int pos1,pos2,pos3,pos4,degree;
char score[3];
PwmOut buzzer(PA_15);
NoteEngine note;
Note _note;
///////////// prototypes ///////////////////////////////////////////////////////////////////////////////////////////////////////
void init();
void render(int note_type);
void note_check(int pos,int note_type);
void note_display(int note_type);
void note_path_classification(int i);
void Continue_to_play(int state, int con, int set_i);
void give_score(int degree);
void welcome();
void menu();void pause_menu(int i);
void game_tutorial();
void complex_select();
void Play();void Game_tutorial();void Complex_option();//Display the menu
void Easy();void Medium();void Hard();//Display the complex selection menu
void Continue();void Restart(); void Back_to_main();//Display the pause menu
////////////main function/////////////////////////////////////////////////////////////////////////////////////////////////////////
int main() {
    init();      // initialise devices and objects
    welcome();   // waiting for the user to start  
    menu();
}
////////////other functions///////////////////////////////////////////////////////////////////////////////////////////////////////
void init() {
    lcd.init(LPH7366_1);
    lcd.setContrast(0.5);
    _note.init();
    note.init();
    note.init_degree();
    joystick.init();
    Joystick.mode(PullUp);
}

void render(int note_type) {
    lcd.clear();  
    note.draw(lcd);
    note_display(note_type);
    lcd.refresh(); 
}

void play_note(int note, int degree){
    //play the buzzer
    buzzer.period_us((float) 1000000.0f/ (float) note);         //set the period of the pwm signal (in us)
    buzzer.pulsewidth_us(buzzer.read_period_us()/2);            //set pulse width of the pwm to 1/2 the period
    ThisThread::sleep_for(150ms-25ms*degree);                    //play sound for 150ms-25ms*degree
    buzzer.pulsewidth_us(0);                                    //turn off buzzer
}

void play_music(){  //When certain note is playing, the lcd automatically generates a note on its corresponding path.
    int degree = note.getdegree();
    lcd.clear();  
    note.draw(lcd);
    lcd.clear();      // first draw the initial frame
    thread_sleep_for(100);  // and wait for one frame period - millseconds
    _note.init(); note.init();  //Initiate the parameters (such as the note position, the score), otherwise the game is continuing to be
      //played rather than resarted when the user select restart
    buttonA.fall(&buttonA_isr);
    buttonA.mode(PullUp);  //Initialise the buttonA
    Continue_to_play(0,0,1);
    buzzer.pulsewidth_us(0);            //turn off buzzer
    thread_sleep_for(1000);
    give_score(degree);
}

void Continue_to_play(int state, int con, int set_i){
    int degree = note.getdegree();
    for(int i = set_i; i<336; i++){
        if(con==1){ //Continue to play item is selected
            con = 0;
            note_path_classification(i);
            lcd.printString("3", 40, 3);
            lcd.refresh(); 
            ThisThread::sleep_for(1000ms);
            lcd.printString("2", 40, 3);
            lcd.refresh(); 
            ThisThread::sleep_for(1000ms);
            lcd.printString("1", 40, 3);
            lcd.refresh(); 
            ThisThread::sleep_for(1000ms);
        }
        if (g_buttonA_flag) {
        g_buttonA_flag = 0;  // if it has, clear the flag
        state = !state;
        }
        if (state == 0){  //The blue button is not pressed, the game runs normally
            if(beats[i]==1){
                play_note(music[i],degree);
                note_path_classification(i);
            }
            else{
                for(int j=1; j<=beats[i];j++){
                    play_note(music[i],degree);
                    note_path_classification(i);
                }
            }
        }
        else{
            while(state == 1){
                if (g_buttonA_flag) {
                g_buttonA_flag = 0;  // if it has, clear the flag
                state = !state;
                }
                pause_menu(i);
            }
        }
    }
}

void menu(){
    int select_item=1;
    Play();
    while (Joystick.read() == 1){  //Use the joystick to select between the two options
    UserInput input = {joystick.get_direction(),joystick.get_mag()};
        if (input.d == N){
            if (select_item == 3){
                select_item=2;
                Game_tutorial();
            }
            else{
                select_item=1;
                Play();
            }
        }
        else if (input.d == S){
            if (select_item == 1){
                select_item=2;
                Game_tutorial();
            }
            else{
                select_item=3;
                Complex_option();
            }
        }
        else{
            sleep();
        }
    ThisThread::sleep_for(100ms);
    }
    if (select_item == 2){
        game_tutorial();
        return menu();
    }
    else if (select_item == 3){
        complex_select();
        return menu();
    }
    else{  //begin to play
        play_music();
    }
}

void pause_menu(int i){
    int select_item=1;
    Continue();
    while (Joystick.read() == 1){  //Use the joystick to select between the two options
    UserInput input = {joystick.get_direction(),joystick.get_mag()};
        if (input.d == N){
            if (select_item == 3){
                select_item=2;
                Restart();
            }
            else{
                select_item=1;
                Continue();
            }
        }
        else if (input.d == S){
            if (select_item == 1){
                select_item=2;
                Restart();
            }
            else{
                select_item=3;
                Back_to_main();
            }
        }
        else{
            sleep();
        }
    ThisThread::sleep_for(100ms);
    }
    if (select_item == 2){
        Restart();
        return play_music();
    }
    else if (select_item == 3){
        Back_to_main();
        return menu();
    }
    else{
        return Continue_to_play(0, 1, i);
    }
}
//Note display and grading------------------------------------------------------------------------------------------------------------
void note_path_classification(int i){
    int note = music[i];
    pos1 = _note.get_y1();
    pos2 = _note.get_y2();
    pos3 = _note.get_y3();
    pos4 = _note.get_y4();
    if ((note==262||note==523||note==1046||note==311||note==659||note==1318)&&(pos1==48&&pos2==48&&pos3==48&&pos4==48)){  //Note 1 and 3
            pos1=0;
            _note.set_pos1(28,0);
    }
    if ((note==286||note==587||note==1174||note==349||note==698||note==1396)&&(pos1==48&&pos2==48&&pos3==48&&pos4==48)){  //Note 2 and 4
            pos2=0;
            _note.set_pos2(36,0);
    }
    if ((note==440||note==880||note==1760)&&(pos1==48&&pos2==48&&pos3==48&&pos4==48)){  //Note 6
            pos3=0;
            _note.set_pos3(43,0);
    }
    if ((note==392||note==784||note==1567||note==494||note==987||note==1975)&&(pos1==48&&pos2==48&&pos3==48&&pos4==48)){  //Note 5 and 7
            pos4=0;
            _note.set_pos4(50,0);
    }
    //draw the note
    if (pos1<48){
        render(1);
    }
    if (pos2<48){
        render(2);
    }
    if (pos3<48){
        render(3);
    }
    if (pos4<48) {
        render(4);
    }
}

void give_score(int degree){
    int s = note.getscore();
    sprintf(score, "%d", s); 
    lcd.clear(); 
    lcd.printString("Your score is",0,1);  
    lcd.printString(score, 35, 2);
    lcd.printString("Your level is",0,3);
    if (degree == 1){
        if (s>=600){
            lcd.printString("S", 40, 4);
        }
        else if (s>=500){
            lcd.printString("A", 40, 4);
        }
        else if (s>=400){
            lcd.printString("B", 40, 4);
        }
        else{
            lcd.printString("C", 40, 4);
        }
    }
    else{
        if (s>=700){
            lcd.printString("S", 40, 4);
        }
        else if (s>=550){
            lcd.printString("A", 40, 4);
        }
        else if (s>=450){
            lcd.printString("B", 40, 4);
        }
        else{
            lcd.printString("C", 40, 4);
        }
    }
    lcd.refresh();
    ThisThread::sleep_for(500ms);
    while (Joystick.read() == 1){
         ThisThread::sleep_for(500ms);}
    return menu();
}

//check whether they are hit and give eliminate feedback
void note_display(int note_type){
    UserInput input = {joystick.get_direction(),joystick.get_mag()};
    pos1 = _note.get_y1();
    pos2 = _note.get_y2();
    pos3 = _note.get_y3();
    pos4 = _note.get_y4();
    if (pos1==0){
        note.set_hitcheck1(1);
    }
    if (pos2==0){
        note.set_hitcheck2(1);
    }
    if (pos3==0){
        note.set_hitcheck3(1);
    }
    if (pos4==0){
        note.set_hitcheck4(1);
    }
    if (note_type==1){
        if (input.d == W){    //If the coming note is hit, it elmininates
            if(note.hitcheck1()==1){
            note_check(pos1,1);
            _note.draw1(lcd,0);
            goto out1;    //The hitcheck function and the goto are used to help break the if loop and 
            }             //avoid entering again once the note has been hit in the same render process
            out1:
            note.set_hitcheck1(0);
        }
        else{
            if (pos1==40) {    //if the note is not hit, it also eliminates, but with a "miss"
                lcd.printChar('M',10,5);
                note.score(0);
                _note.draw1(lcd,0);
            }
            _note.draw1(lcd,1);    //Otherwise, the note continues to flow
            _note.update1();
        }
    }
    else if (note_type==2){
        if (input.d == N){
            if(note.hitcheck2()==1){
            note_check(pos2,2);
            _note.draw2(lcd,0);
            goto out2;
            }
            out2:
            note.set_hitcheck2(0);
        }
        else{
            if (pos2==40) {
                lcd.printChar('M',31,5);
                note.score(0);
                _note.draw2(lcd,0);
            }
            _note.draw2(lcd,1);
            _note.update2();
        }
    }
    else if (note_type==3){
        if (input.d == S){
            if(note.hitcheck3()==1){
            note_check(pos3,3);
            _note.draw3(lcd,0);
            goto out3;
            }
            out3:
            note.set_hitcheck3(0);
        }
        else{
            if (pos3==40) {
                lcd.printChar('M',48,5);
                note.score(0);
                _note.draw3(lcd,0);
            }
            _note.draw3(lcd,1);
            _note.update3();
        }
    }
    else{
        if (input.d == E){
            if(note.hitcheck4()==1){
            note_check(pos4,4);
            _note.draw4(lcd,0);
            goto out4;
            }
            out4:
            note.set_hitcheck4(0);
        }
        else{
            if (pos4==40) {
                lcd.printChar('M',68,5);
                note.score(0);
                _note.draw4(lcd,0);
            }
            _note.draw4(lcd,1);
            _note.update4();
        }
    }
}

//check the position of the notes when they are hit and give goal feedback
void note_check(int pos, int note_type){
    if (note_type==1){
        if (pos<=29 || pos>=40){
            lcd.printChar('M',10,5);
            note.score(0);
        }
        else if (pos>=34 && pos<=36){
            lcd.printChar('P',10,5);
            note.score(10);
        }
        else{
            lcd.printChar('G',10,5);
            note.score(5);
        }
    }
    else if (note_type==2){
        if (pos<=29 || pos>=40){
            lcd.printChar('M',31,5);
            note.score(0);
        }
        else if (pos>=34 && pos<=36){
            lcd.printChar('P',31,5);
            note.score(10);
        }
        else{
            lcd.printChar('G',31,5);
            note.score(5);
        }
    }
    else if (note_type==3){
        if (pos<=29 || pos>=40){
            lcd.printChar('M',52,5);
            note.score(0);
        }
        else if (pos>=34 && pos<=36){
            lcd.printChar('P',52,5);
            note.score(10);
        }
        else{
            lcd.printChar('G',52,5);
            note.score(5);
        }
    }
    else{
        if (pos<=29 || pos>=40){
            lcd.printChar('M',73,5);
            note.score(0);
        }
        else if (pos>=34 && pos<=36){
            lcd.printChar('P',73,5);
            note.score(10);
        }
        else{
            lcd.printChar('G',73,5);
            note.score(5);
        }
    }
}

//Menu Display--------------------------------------------------------------------------------------------------------------------------
void welcome() { // splash screen
    lcd.printString(" Rhythm Master",0,0);
    lcd.drawSprite(0,8,24,84,(int *)cover);
    lcd.printString("Press Joystick",0,4);
    lcd.printString(" to get start",0,5);
    lcd.refresh();
        while (Joystick.read() == 1){
         ThisThread::sleep_for(500ms);}
}

void Play(){
    lcd.clear();
    lcd.printString("     Play     ",0,1);
    lcd.printString(" Game tutorial",0,3);
    lcd.printString("  Difficulty  ",0,5);
    lcd.drawSprite(24,8,7,4,(int *)arrow);
    lcd.refresh();
    ThisThread::sleep_for(500ms);
}

void Game_tutorial(){
    lcd.clear();
    lcd.printString("     Play     ",0,1);
    lcd.printString(" Game tutorial",0,3);
    lcd.printString("  Difficulty  ",0,5);
    lcd.drawSprite(0,24,7,4,(int *)arrow);
    lcd.refresh();
    ThisThread::sleep_for(500ms);
}

void Complex_option(){
    lcd.clear();
    lcd.printString("     Play     ",0,1);
    lcd.printString(" Game tutorial",0,3);
    lcd.printString("  Difficulty  ",0,5);
    lcd.drawSprite(6,40,7,4,(int *)arrow);
    lcd.refresh();
    ThisThread::sleep_for(500ms);
}

void Continue(){
    lcd.clear();
    lcd.printString("   Continue   ",0,1);
    lcd.printString("   Restart    ",0,3);
    lcd.printString(" Back to main ",0,5);
    lcd.drawSprite(12,8,7,4,(int *)arrow);
    lcd.refresh();
    ThisThread::sleep_for(500ms);
}

void Restart(){
    lcd.clear();
    lcd.printString("   Continue   ",0,1);
    lcd.printString("   Restart    ",0,3);
    lcd.printString(" Back to main ",0,5);
    lcd.drawSprite(12,24,7,4,(int *)arrow);
    lcd.refresh();
    ThisThread::sleep_for(500ms);
}

void Back_to_main(){
    lcd.clear();
    lcd.printString("   Continue   ",0,1);
    lcd.printString("   Restart    ",0,3);
    lcd.printString(" Back to main ",0,5);
    lcd.drawSprite(0,40,7,4,(int *)arrow);
    lcd.refresh();
    ThisThread::sleep_for(500ms);
}

void Easy(){
    lcd.clear();  
    lcd.printString("     Easy    ",0,1); 
    lcd.printString("    Medium   ",0,3);
    lcd.printString("     Hard    ",0,5);
    lcd.drawSprite(24,8,7,4,(int *)arrow);
    lcd.refresh();
    ThisThread::sleep_for(500ms);
}

void Medium(){
    lcd.clear();  
    lcd.printString("     Easy    ",0,1); 
    lcd.printString("    Medium   ",0,3);
    lcd.printString("     Hard    ",0,5);
    lcd.drawSprite(18,24,7,4,(int *)arrow);
    lcd.refresh();
    ThisThread::sleep_for(500ms);
}

void Hard(){
    lcd.clear();  
    lcd.printString("     Easy    ",0,1);  
    lcd.printString("    Medium   ",0,3);
    lcd.printString("     Hard    ",0,5);
    lcd.drawSprite(24,40,7,4,(int *)arrow);
    lcd.refresh();
    ThisThread::sleep_for(500ms);
}

void complex_select(){
    int select_item = 1;
    Easy();
    while (Joystick.read() == 1){  //Use the joystick to select between the two options
    UserInput input = {joystick.get_direction(),joystick.get_mag()};
        if (input.d == N){
            if (select_item == 3){
                select_item=2;
                Medium();
            }
            else{
                select_item=1;
                Easy();
            }
        }
        else if (input.d == S){
            if (select_item == 1){
                select_item=2;
                Medium();
            }
            else {
                select_item=3;
                Hard();
            }
        }
        else{
            sleep();
        }
    ThisThread::sleep_for(100ms);
    }
    //The comlex degree is selected, set the degree
    note.set_degree(select_item);
}

void game_tutorial(){
    lcd.clear(); 
    lcd.printString("There are 4",0,0);
    lcd.printString("note paths",0,2);
    lcd.refresh();
    ThisThread::sleep_for(500ms);
    while (Joystick.read() == 1){
         ThisThread::sleep_for(100ms);}

    lcd.clear(); 
    lcd.printString("Each path",0,0);
    lcd.printString("corresponds to",0,2);
    lcd.printString("a direction",0,4);
    lcd.refresh();
    ThisThread::sleep_for(500ms);
    while (Joystick.read() == 1){
         ThisThread::sleep_for(100ms);}

    lcd.clear(); 
    lcd.printString("When the",0,0);
    lcd.printString("notes enter",0,2);
    lcd.printString("test area",0,4);
    lcd.refresh();
    ThisThread::sleep_for(500ms);
    while (Joystick.read() == 1){
         ThisThread::sleep_for(100ms);}

    lcd.clear();  
    lcd.printString("Control the",0,0);
    lcd.printString("joystick to",0,2);
    lcd.printString("hit the notes",0,4);
    lcd.refresh();
    ThisThread::sleep_for(500ms);
    while (Joystick.read() == 1){
         ThisThread::sleep_for(100ms);}
        
    lcd.clear(); 
    lcd.drawSprite(13,32,8,8,(int *)left);
    lcd.drawSprite(29,32,8,8,(int *)up);
    lcd.drawSprite(46,31,8,8,(int *)down);
    lcd.drawSprite(62,32,8,8,(int *)right);
    lcd.drawRect(30,20,10,4,FILL_BLACK);
    lcd.printString("Note",43,2);
    lcd.printChar('W',10,5);
    lcd.printChar('N',31,5);
    lcd.printChar('S',48,5);
    lcd.printChar('E',68,5);
    note.draw(lcd);
    lcd.refresh();
    ThisThread::sleep_for(500ms);
    while (Joystick.read() == 1){
         ThisThread::sleep_for(100ms);}

    lcd.clear();  
    lcd.printString("Hit feedback:",0,0);
    lcd.printString("M: Miss",0,2);
    lcd.printString("G: Good",0,3);
    lcd.printString("P: Perfect",0,4);
    lcd.refresh();
    ThisThread::sleep_for(500ms);
    while (Joystick.read() == 1){
         ThisThread::sleep_for(100ms);}

    lcd.clear();  
    lcd.printString("The top left",0,1);
    lcd.printString("is combo count",0,2);
    lcd.printString("The top right",0,3);
    lcd.printString("is total score",0,4);
    lcd.refresh();
    ThisThread::sleep_for(500ms);
    while (Joystick.read() == 1){
         ThisThread::sleep_for(100ms);}

    lcd.clear();  
    lcd.printString("When the game ",0,1);
    lcd.printString("is over,you'll",0,2);
    lcd.printString(" get a score  ",0,3);
    lcd.printString(" and a level  ",0,4);
    lcd.refresh();
    ThisThread::sleep_for(500ms);
    while (Joystick.read() == 1){
         ThisThread::sleep_for(100ms);}
    
    lcd.clear();  
    lcd.printString("Score rating: ",0,1);
    lcd.printString("S: Excellent  ",0,2);
    lcd.printString("A: Good       ",0,3);
    lcd.printString("B: Medium     ",0,4);
    lcd.printString("C: Bad        ",0,5);
    lcd.refresh();
    ThisThread::sleep_for(500ms);
    while (Joystick.read() == 1){
         ThisThread::sleep_for(100ms);}

    lcd.clear();  
    lcd.printString("The higher the",0,1);
    lcd.printString("combo count,",0,2);
    lcd.printString("The higher the",0,3);
    lcd.printString("total score",0,4);
    lcd.refresh();
    ThisThread::sleep_for(500ms);
    while (Joystick.read() == 1){
         ThisThread::sleep_for(100ms);}

    lcd.clear();
    lcd.printString("Thats the rule",0,0);
    lcd.printString("Now enjoy your",0,2);
    lcd.printString("music journey!",0,4);
    lcd.refresh();
    ThisThread::sleep_for(500ms);
    while (Joystick.read() == 1){
         ThisThread::sleep_for(100ms);}
}