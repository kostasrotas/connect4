/*

MIT License

Copyright (c) 2016 Kostas Rotas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <stdio.h>
#include <stdlib.h>

#define X_SIZE 7
#define Y_SIZE 7
#define DEPTH 6

char table [Y_SIZE][X_SIZE];
signed char tablepos[X_SIZE];
char pl[2] = {'X','O'};
char plt[2]; /* Player type : 0 human , 1 computer */
int moves_num;

void print_table(){
 int i,j;
 printf("\n\n\n*** CONNECT 4 *** \n");
 for (j=0;j<Y_SIZE;j++){
  printf("| ");
  for (i=0;i<X_SIZE;i++)
   printf("%c ",table[j][i]);
  printf("|\n");
  }
 for (j=0;j<=X_SIZE;j++)
  printf("--");
 printf ("-\n  ");
 for (j=0;j<X_SIZE;j++)
  printf("%c ",'1'+j);
 printf ("\n");
 }

void init_table(){
 int i,j;
 for (i=0;i<X_SIZE;i++){
  tablepos[i]=Y_SIZE-1;
  for (j=0;j<Y_SIZE;j++)
   table[j][i]=' ';
  }
 }

void play(int row,char c){
 table[tablepos[row]][row]=c;
 tablepos[row]--;
 }

void undo_move(int row){
 tablepos[row]++;
 table[tablepos[row]][row]=' ';
 }

int check_winner(int player,int i,int j){
 int ret_value=0;
 int dx[4] = {1,1,1,0};
 int dy[4] = {-1,0,1,1};
 char cas;
 int k,r;
 for (cas=0;cas<4;cas++){
  for (r=1;r<4;r++){
   if (i-dx[cas]*r>=X_SIZE || i-dx[cas]*r<0)
    break;
   if (j-dy[cas]*r>=Y_SIZE || j-dy[cas]*r<0)
    break;
   if (table[j-r*dy[cas]][i-r*dx[cas]]!=pl[player])
    break;
   }
  if (r==4)
   return 1;
  for (k=4-r;k>=1;k--){
   if (i+dx[cas]*k>=X_SIZE || i+dx[cas]*k<0)
    break;
   if (j+dy[cas]*k>=Y_SIZE || j+dy[cas]*k<0)
    break;
   if (table[j+k*dy[cas]][i+k*dx[cas]]!=pl[player])
    break;
   }
  if (k==0)
   return 1;
  }
 return 0;
 }

int evaluate_pos(int player){  /* 0-14 */
 int i,j,v,found;
 v=0;
 for (i=0;i<X_SIZE;i++){
  found=0;
  if (table[0][i]==' ')
  for (j=tablepos[i];j>=0;j--){
   table[j][i]=pl[player];
   if (check_winner(player,i,j)){
    v++;
    found=1;
    }
   table[j][i]=pl[1-player];
   if (check_winner(1-player,i,j)){
    v--;
    found=1;
    }
   table[j][i]=' ';
   if (found)
    break;
   }
  }
 return v;
 }

int evaluate_fast(int player,int move){ /* evaluate moves 0-7 */
 int v,i,j;
 v=0;
 for (j=tablepos[move]-1;j<=tablepos[move]+1;j++)
 for (i=move-1;i<=move+1;i++)
   if (j<=Y_SIZE-1 && j>=0)
   if (i<=X_SIZE-1 && i>=0)
   if (table[j][i]!=' ')
    v++;
 return v;
 }

int evaluate_move(int player,int move,int depth){
 int i,m,v;
 char one_found=0;
 int values[X_SIZE];
 if (depth== 0)
  return 0; /* no evaluation */

 play(move,pl[player]);
 if (check_winner(player,move,tablepos[move]+1)){ /* O.K. I win with this move */
   undo_move(move);
   return 200;          /* winning value */
   }
 for (i=0;i<X_SIZE;i++){
  if (table[0][i]!=' '){
   values[i]=1000;      /*invalid move value*/
   continue;
   }
  one_found=1;
  values[i]=-evaluate_move(1-player,i,depth-1);
  }
 if (one_found==0){
  undo_move(move); /* no more moves */
  return 0; /* no evaluation */
  }
 m=0;
 for (i=1;i<X_SIZE;i++)
  if (values[i]<values[m])
   m=i;
 v=values[m];
 if (v==200 || v==-200){  /* Result */
  undo_move(move);
  return v;
  }
 if (depth==DEPTH - 2)
  v=evaluate_pos(player)*8+1;
 undo_move(move);
 if (depth==DEPTH)
  v+=evaluate_fast(player,move);
 return v;
 }

int computer_play(int player){
 int m,v,i,d;
 int values[X_SIZE];
 if (moves_num<2){ /* Random if the first move */
  m=rand() % 7; 
  printf("%d",m+1);
  return m;
  }
 for (d=DEPTH;d>1;d-=2){
  for (m=0;m<X_SIZE;m++){
   if (table[0][m]!=' '){
    values[m]=-1000;
    continue;
    }
   values[m]=evaluate_move(player,m,d);
   }
  m=(rand() % 7);
  for (i=0;i<7;i++)
   if (values[i]>values[m])
    m=i;
  v=values[m];
  if (v!=-200)
   break;
  }
 printf("%d",m+1);
 if (v==-200)
  printf(" - I loose.\n");
 else if (v==200)
  printf(" - I won.\n");
 else
  printf("\n");
  /* printf "- [%d] /n"); */
 return m; 
 }

void play_game(){
 char current_player=0; /* 0 or  1*/
 char result=0,c; /* result 0 = nothing, 1 = win , 2 =no winner */
 int move,i;
 init_table();
 print_table();
 moves_num=0;
 while (result==0){
  printf("[%c] - Player %c (1-7):",pl[current_player],'1'+current_player);
  if (plt[current_player]==0){ /* Human player - Get from keyboard */
   do{
    do{
     c=getchar();
     }while(c<'1' || c>'7');
    move=c-'1';
    }while (table[0][move]!=' ');
   }
  else     
   move=computer_play(current_player); /* Computer player */
  play(move,pl[current_player]);
  print_table();
  if (check_winner(current_player,move,tablepos[move]+1)==1)
   result=1;
  else{
   current_player=1-current_player;
   for (i=0;i<X_SIZE;i++)
    if (table[0][i]==' ')
     break;
   if (i==X_SIZE)
    result=2; /* End with no winner */
   }
  moves_num++;
  }
 if (result == 1)
  printf("Player %c wins !!!\n",'1'+current_player);
 else 
  printf("End of game, no winner !!!\n");
 }

void setup_players(){
 int i;
 char c;
 for (i=0;i<2;i++){
  printf("Player %d, Human or Computer (h-c) :",i+1);
  do{
   c=getchar();
   }while (c!='h' && c!='c');
  if (c=='h')
   plt[i]=0;
  else
   plt[i]=1;
  printf("\n");
  }
 }

int main(){
 char fin=0,c;
 while (fin==0){
  setup_players();
  play_game();
  printf("Another game ? (y/n) ");
  do{
   c=getchar();
   }while(c!='y' && c!='n');
  if (c=='n')
   fin=1;
  }
 }
