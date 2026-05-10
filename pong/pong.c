#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <string.h>
#include <conio.h>

//constants
#define WIDTH 80
#define HEIGHT 24
#define SPACE_FROM_EDGES 5

typedef struct Ball {
    COORD coord;
    int dx;
    int dy;
}Ball;
typedef struct Player {
    COORD top;
    COORD bottom;
    int score;
}Player;

void hideCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 1;      // Use 1, NOT 0
    info.bVisible = FALSE; 
    SetConsoleCursorInfo(consoleHandle, &info);
}


void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void gen_ui(char ui[HEIGHT][WIDTH], Player* p1, Player* p2) {
    //handling the basic interface 
    int i = 0;
    memset(ui, ' ', HEIGHT * WIDTH * sizeof(char));
    for (i = 1; i < HEIGHT-1; i++) {ui[i][0] = ui[i][WIDTH-1] = '|';}
    for (i = 1; i < WIDTH-1; i++) {ui[0][i] = ui[HEIGHT-1][i] = '-';}
    ui[0][0] = ui[0][WIDTH-1] = ui[HEIGHT-1][0] = ui[HEIGHT-1][WIDTH-1] = '+';
    //handling the paddles 
    for (i = 9;i<=12;i++) {
        ui[i][SPACE_FROM_EDGES] = '|';
        ui[i][WIDTH - SPACE_FROM_EDGES - 1] = '|';
    }
    p1->top.X = p1->bottom.X = SPACE_FROM_EDGES;
    p2->top.X = p2->bottom.X = WIDTH - SPACE_FROM_EDGES - 1;
    p1->top.Y = p2->top.Y = 9;
    p1->bottom.Y = p2->bottom.Y = 12;
}
void reset(char ui[HEIGHT][WIDTH], Player* p1, Player* p2, Ball* ball) {
    //resetting the ball, paddles and field
    ball->coord.X = (WIDTH - 1) / 2;
    ball->coord.Y = (HEIGHT - 1) / 2;
    ball->dx = 1;
    ball->dy = 1;
    ui[(HEIGHT - 1) / 2][(WIDTH - 1) / 2] = 'o';
    gen_ui(ui, p1, p2);
}

void render(char ui[HEIGHT][WIDTH], Player p1, Player p2) {
    gotoxy(0,0);
    for (int i = 0; i < HEIGHT; i++){
        for (int j = 0; j < WIDTH; j++) {
            printf("%c", ui[i][j]);
        }
        printf("\n");
    }
    printf("score : %d, %d\n", p1.score, p2.score);
}
void update_ui(char ui[HEIGHT][WIDTH], Player* p, int mode) {
    switch (mode) {
        case 1://moving up
        ui[p->top.Y][p->top.X] = '|';
        ui[p->bottom.Y + 1][p->bottom.X] = ' ';
        break;
        case 2://moving down
        ui[p->bottom.Y][p->bottom.X] = '|';
        ui[p->top.Y - 1][p->top.X] = ' ';
        break;
    }
}
void key_detection(char ui[HEIGHT][WIDTH], Player* p1, Player* p2, int c) {
    int c1;
    switch (c) {
        case 'z'://up player1
        if (p1->top.Y - 1 > 1) {p1->top.Y--;p1->bottom.Y--;update_ui(ui, p1, 1);}
        break;
        case 's'://down player1        
        if (p1->bottom.Y + 1 < HEIGHT - 1) {p1->bottom.Y++;p1->top.Y++;update_ui(ui, p1, 2);}
        break;
        case 224:
        c1 = getch();
        if(c1 == 72){//up player2
            if (p2->top.Y - 1 > 0) {p2->top.Y--;p2->bottom.Y--;update_ui(ui, p2, 1);}
        }
        else if(c1 == 80){//down player2
            if (p2->bottom.Y + 1 < HEIGHT - 1) {p2->bottom.Y++;p2->top.Y++;update_ui(ui, p2, 2);}
        }
        break;
    }
}
void move_ball(char ui[HEIGHT][WIDTH], Player* p1, Player* p2, Ball* ball) {
    for (int i = 0; i < abs(ball->dx); i++) {
        if (ball->dx < 0) {
            if (ball->coord.X - 1 == 0) {reset(ui, p1, p2, ball);p2->score++;;return;}//goal for player 2
            if (ui[ball->coord.Y][ball->coord.X - 1] == ' '){
                ui[ball->coord.Y][ball->coord.X] = ' ';
                ball->coord.X--;
                ui[ball->coord.Y][ball->coord.X] = 'o';
            }
            else {ball->dx = -ball->dx;}
        }
        else {
            if (ball->coord.X + 1 == WIDTH - 1) {reset(ui, p1, p2, ball);p1->score++;return;}//goal for player 1
            if (ui[ball->coord.Y][ball->coord.X + 1] == ' ') {
                ui[ball->coord.Y][ball->coord.X] = ' ';
                ball->coord.X++;
                ui[ball->coord.Y][ball->coord.X] = 'o';
            }
            else {ball->dx = -ball->dx;}
        }
    }
    for (int i = 0; i < abs(ball->dy); i++) {
        if (ball->dy < 0) {
            if (ui[ball->coord.Y - 1][ball->coord.X] == ' ') {
                ui[ball->coord.Y][ball->coord.X] = ' ';
                ball->coord.Y--;
                ui[ball->coord.Y][ball->coord.X] = 'o';
            }
            else {ball->dy = -ball->dy; return;}
        }
        else {
            if (ui[ball->coord.Y+1][ball->coord.X] == ' '){
                ui[ball->coord.Y][ball->coord.X] = ' ';
                ball->coord.Y++;
                ui[ball->coord.Y][ball->coord.X] = 'o';
            }
            else {ball->dy = - ball->dy; return;}
        }
    }

}
int main () {
    char ui[HEIGHT][WIDTH];
    Player p1, p2;
    p1.score = 0;
    p2.score = 0;
    Ball ball;
    hideCursor();
    reset(ui, &p1, &p2, &ball);
    gotoxy(0,0);
    render(ui, p1, p2);
    while (1) {
        if (kbhit()) {
            int c = getch();
            if(c == 'q') {
                break;
            }
            else {key_detection(ui, &p1, &p2, c);}
        }
        move_ball(ui, &p1, &p2, &ball);
        Sleep(50);
        gotoxy(0,0);
        render(ui, p1, p2);
    }
    return 0;
}