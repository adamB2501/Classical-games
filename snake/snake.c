#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <time.h>
#include <windows.h>

#define WIDTH 80
#define HEIGHT 24

typedef struct{
    COORD* body;
    COORD apple;
    COORD head;
    int snakeSize ;
    int score ;
    char direction;
    int self_flag;
} Gamestate;

void setup_console() {
    SetConsoleOutputCP(65001);  // set to UTF-8
    SetConsoleCP(65001);
    // set font to one that supports unicode
    CONSOLE_FONT_INFOEX font = {0};
    font.cbSize = sizeof(font);
    font.dwFontSize.Y = 16;
    font.FontWeight = FW_NORMAL;
    wcscpy(font.FaceName, L"Consolas");  // Consolas supports unicode
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &font);
}

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

void keyDetection(Gamestate* G) {
    if (kbhit() && (getch() == 224)) {
        switch (getch()) {
            case 72://up
            if (!(G->direction == 'd')) {
                G->direction = 'u';
            }
            break;
            case 80://down
            if (!(G->direction == 'u')) {
                G->direction = 'd';
            }
            break;
            case 75://left
            if (!(G->direction == 'r')) {
                G->direction = 'l';
            }
            break;
            case 77://right
            if (!(G->direction == 'l')) {
                G->direction = 'r';
            }
            break;
        }
    }
}
void interf(char inter[HEIGHT][WIDTH],int mode ,Gamestate* G) {
    if (mode == 0) {//menu
        system("cls");
        printf("################################################################################\n");
        printf("#                                                                              #\n");
        printf("#                                                                              #\n");
        printf("#                                                                              #\n");
        printf("#                 ███████╗███╗   ██╗ █████╗ ██╗  ██╗███████╗                   #\n");
        printf("#                 ██╔════╝████╗  ██║██╔══██╗██║ ██╔╝██╔════╝                   #\n");
        printf("#                 ███████╗██╔██╗ ██║███████║█████╔╝ █████╗                     #\n");
        printf("#                 ╚════██║██║╚██╗██║██╔══██║██╔═██╗ ██╔══╝                     #\n");
        printf("#                 ███████║██║ ╚████║██║  ██║██║  ██╗███████╗                   #\n");
        printf("#                 ╚══════╝╚═╝  ╚═══╝╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝                   #\n");
        printf("#                                                                              #\n");
        printf("#                       ~~ ~ ~ ~ ~ ~ ~ ~ ~ ~~                                  #\n");
        printf("#                                                                              #\n");
        printf("#                       >  PRESS ENTER TO PLAY                                 #\n");
        printf("#                       >  PRESS Q TO QUIT                                     #\n");
        printf("#                                                                              #\n");
        printf("#                       ~~ ~ ~ ~ ~ ~ ~ ~ ~ ~~                                  #\n");
        printf("#                                                                              #\n");
        printf("#                                                                              #\n");
        printf("#                           HIGH SCORE: %d                                      #\n", G->score);
        printf("#                                                                              #\n");
        printf("#                                                                              #\n");
        printf("#                                                                              #\n");
        printf("################################################################################\n");
    }
    else if (mode == 1) {//filling up base interface
        for (int i=0;i<HEIGHT;i++){inter[i][0]='#';}
        for (int i=0;i<HEIGHT;i++){inter[i][WIDTH-1]='#';}
        for (int i=0;i<WIDTH;i++){inter[0][i]='#';}
        for (int i=0;i<WIDTH;i++){inter[HEIGHT-1][i]='#';}
    }

}

void show(char inter[HEIGHT][WIDTH]) {
    for (int i=0; i<HEIGHT;i++) {
        for (int j=0;j<WIDTH;j++) {
            printf("%c",inter[i][j]);
        }
        printf("\n");
    }
}

void advance(char inter[HEIGHT][WIDTH], Gamestate* G) {
    //saving the tail coords in case i need em
    int xTail = G->body[0].X;
    int yTail = G->body[0].Y;
    //regular advancing
    inter[G->body[0].Y][G->body[0].X]= ' ';
    for (int i =0; i<(*G).snakeSize-1;i++) {
        G->body[i] = G->body[i+1];
        inter[G->body[i].Y][G->body[i].X]= 'o';
    }
    G->body[G->snakeSize-1].X = G->head.X;
    G->body[G->snakeSize-1].Y = G->head.Y;
    inter[G->head.Y][G->head.X] = 'o';
    switch ((*G).direction) {//updating the Coordinates based on current direction
        case 'r':
        G->head.X++;
        inter[G->head.Y][G->head.X] ='>';
        break;
        case 'l':
        G->head.X--;
        inter[G->head.Y][G->head.X] ='<';
        break;
        case 'u':
        G->head.Y--;
        inter[G->head.Y][G->head.X] ='^';
        break;
        case'd':
        G->head.Y++;
        inter[G->head.Y][G->head.X] ='v';
        break;
    }
    if ((G->apple.X == G->head.X) && (G->apple.Y == G->head.Y)) {//checking for apple collision
        G->apple.X = 0;
        G->snakeSize++;
        G->score++;
        G->body = realloc(G->body, G->snakeSize * sizeof(COORD));
        memmove(G->body+1, G->body, (G->snakeSize - 1) * sizeof(COORD));
        G->body[0].X = xTail;
        G->body[0].Y = yTail;
        inter[yTail][xTail] = 'o';
    }
    //checking for self collision
    for (int i = 0; i < G->snakeSize - 1; i++) {
        if ((G->head.X == G->body[i].X) && (G->head.Y == G->body[i].Y)) {
            G->self_flag = 1;
            break;
        }
    }
    if ((G->head.X % (WIDTH - 1) == 0) || (G->head.Y % (HEIGHT - 1) == 0)) {//checking for wall collision
        G->self_flag = 1;
    }
}

void snake(char inter[HEIGHT][WIDTH],int* n,Gamestate* G) {
    if (*n == 0) {//first time running (init the snake)
        int x_first_tail = 12;
        for(int i = 0; i < G->snakeSize; i++) {
            G->body[i].X = x_first_tail;
            G->body[i].Y = 4;
            inter[4][x_first_tail++] = 'o';
        }
        G->head.X = x_first_tail;
        G->head.Y = 4;
        inter[G->head.Y][G->head.X] = '>';
        *n=1;
    }
}

void gen_apple(Gamestate* G,char inter[HEIGHT][WIDTH]) {
    if (G->apple.X == 0) {
        do {
            G->apple.X = 1 + rand() % (WIDTH - 1);
            G->apple.Y = 1 + rand() % (HEIGHT - 1);
        } while (inter[G->apple.Y][G->apple.X] != ' ');
        inter[G->apple.Y][G->apple.X] = '@';
    }
}
/*
void render(Gamestate* G) {
    gotoxy()
}*/

void reset(Gamestate* G, char inter[HEIGHT][WIDTH]) {
    srand(time(NULL));
    memset(inter, ' ', HEIGHT * WIDTH * sizeof(char));
    G->apple.X = 0;
    G->apple.Y = 0;
    free(G->body);
    G->body = malloc(4*sizeof(COORD));
    G->snakeSize = 4;
    G->self_flag = 0;
    G->score = 0;
    G->direction = 'r';
}
void game_start(char inter[HEIGHT][WIDTH], Gamestate* G) {
    reset(G, inter);
    interf(inter, 1, G);
    int n = 0;
    while (1) {
        gotoxy(0, 0);
        show(inter);
        snake(inter, &n, G);
        gen_apple(G, inter);
        keyDetection(G);
        Sleep(100);
        advance(inter, G);
        if (G->self_flag) {
            printf("Game over!\nYour score is: %d\nPress Enter to replay", G->score);
            reset(G, inter);
            interf(inter, 1, G);
            break;
            if (kbhit() && (getch() == 'q')) {
                break;
            }
        }
    }
}
int main() {
    setup_console();
    char inter[HEIGHT][WIDTH];
    Gamestate G;
    G.body = NULL;
    interf(inter, 0, &G);
    while (1) {
        if (kbhit() && (getch()==13)) {
            gotoxy(0,0);
            system("cls");
            Sleep(2000);
            system("cls");
            hideCursor();
            //start the game
            game_start(inter,&G);
        }
        else if (kbhit() && (getch() == 'q')) {
            break;
        }
    }
    
    return 0;
}