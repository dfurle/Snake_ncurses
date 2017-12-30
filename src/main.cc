#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
using namespace std;
int Debug = 1;
//------------------------------------------------
class Field{
private:
public:
  int xfm, yfm;
  vector <vector <int> > field;
  Field(int x, int y) {
    xfm=x;
    yfm=y;
    field.resize(yfm+1, vector<int>(xfm+1));
    
    for (int iy=0;iy<yfm;iy++) {
      for (int ix=0;ix<xfm;ix++) {
	field[iy][ix]=0;
      }
    }
    
    
  }
  ~Field(){ }
  
  int draw_border (){ 
    //---  DRAW BORDER
    for (int kk=0;kk<=yfm;kk++) { mvaddch(kk, xfm, ACS_VLINE); field[kk][xfm]=3;} 
    for (int kk=0;kk<=xfm;kk++) { mvaddch(yfm, kk, ACS_HLINE); field[yfm][kk]=3;}  
    mvaddch(yfm, xfm, ACS_LRCORNER);
    return 0;
  }
};
//------------------------------------------------
class Food { 
private:
  Field *fld;
public:
  
  Food (int n=2, Field *fl=NULL){ 
    fld=fl; 
    for (int q=0;q<n;q++){
      long int xran = rand();   xran = xran*fld->xfm/RAND_MAX;   //scale to field
      long int yran = rand();   yran = yran*fld->yfm/RAND_MAX;
      fld->field[yran][xran]=2;
      mvaddch(yran, xran, ACS_DIAMOND);
      refresh();
    }
  }
  ~Food(){}
  int new_food() {
    while(1){
      long int xran = rand();   xran = xran*fld->xfm/RAND_MAX;
      long int yran = rand();   yran = yran*fld->yfm/RAND_MAX;
      if(fld->field[yran][xran]==0){
	fld->field[yran][xran]=2;
	mvaddch(yran, xran, ACS_DIAMOND);
	break;
      }
    }
    return 0;
  } 
};
//------------------------------------------------
class Snake {
private:
  Field *fld;
public: 
  
  vector <int> y;
  vector <int> x;
  int xpos, ypos, dirx, diry, length;
  
  Snake(int x0=20, int y0=20, Field *fl=NULL){
    fld=fl;
    xpos = x0;
    ypos = y0;
    dirx = 1;
    diry = 0;
    length = 1;
    fld->field[ypos][xpos] = 1;   //--  mark snake position on the field
    x.push_back(x0);
    y.push_back(y0);
  }
  ~Snake(){
  }
  int move_next(char key){
    if (key == 2 && diry != -1){diry = 1; dirx = 0;}
    if (key == 3 && diry != 1){diry = -1; dirx = 0;}
    if (key == 4 && dirx != 1){dirx = -1; diry = 0;}
    if (key == 5 && dirx != -1){dirx = 1; diry = 0;}
    xpos = xpos + dirx;
    ypos = ypos + diry;
    if (fld->field[ypos][xpos]==3){return 3;}   //stop if hit edge
    if (fld->field[ypos][xpos]==1){return 2;}   //stop if hit him self
    if (xpos < 0){return 5;}               //if hits top
    if (ypos < 0){return 4;}               //if hit left
    return 0;
  }
  
  int on_food() {
    if (fld->field[ypos][xpos]==2) {  //if on food
      fld->field[ypos][xpos]=0;       //makes current head field 0
      return 1;
    }
    return 0;
  }      
  int grow_snake(int food){
    //-----------    
    length= x.size();
    mvaddch(ypos, xpos, '#');           //draws current head pos to "#"           
    fld->field[ypos][xpos]=1;                //makes current head field a number of 1
    //if (Debug>1) printf("pre snake size %d\n   ", fd.food);
    if (food==0){
      long int poorand = rand(); poorand = poorand*100/RAND_MAX;
      int threshhold = 1;
      if (poorand <= threshhold){
	mvaddch(y[length-1], x[length-1], '*');  //makes last block in snake blank
	fld->field[y[length-1]][x[length-1]]=3;  //makes last field in snake a number of 0
      }else {
	mvaddch(y[length-1], x[length-1], ' ');  //makes last block in snake blank
	fld->field[y[length-1]][x[length-1]]=0;  //makes last field in snake a number of 0
      }
      x.pop_back();
      y.pop_back();
    }
    std::vector<int>::iterator it;
    it = x.begin();  it = x.insert ( it , xpos );
    it = y.begin();  it = y.insert ( it , ypos );
    
    //if (Debug>2) printf("aga %d %d\n", x(length), y(length));
    //if (Debug>1) printf("post snake size %d\n", length );
    return 0;
  }
};
//------------------------------------------------
//====================================================
//               
//====================================================
int main() {
  
  //--------------  ncurses initialization  ---------------
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  timeout(150);  //--- speed of snake 
  int xmax;
  int ymax;
  getmaxyx(stdscr, ymax, xmax);  // get screen size 
  WINDOW * win = newwin(25, 25, 5,5);  //--- window on the screen 
  wborder(win, 0, 0, 0, 0, 0, 0, 0, 0);
  whline(win, ACS_HLINE, 14);
  //-------------- end ncurses init ------------------------
  
  
  Field fld(60, 30); 
  fld.draw_border();
  Food fd(3,&fld);  
  Snake sn(35, 20, &fld);
  
  
  char key;
  while (1){
    key = getch();
    int ret = sn.move_next(key);
    //if (Debug>0) printf("after move next ret=%d x=%d y=%d\n",ret,sn.xpos,sn.ypos);
    if (ret>0) { sleep(2); break; }
    int food = sn.on_food();
    sn.grow_snake(food); 
    
    if (food==1){
      fd.new_food();
    }
    
  }
  
  
  endwin();
  
  return 0;
}
