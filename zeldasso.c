#include <stdio.h>
#include "raylib.h"

#define LARGURA 1200
#define ALTURA 800
#define ALTSTATUS 60

int main(void){
    Texture2D chao;

    InitWindow(LARGURA, ALTURA, "ZELDASSO");
    SetTargetFPS(60);

    chao = LoadTexture("chao_1200x740.png");

    while(!WindowShouldClose()){

            DrawRectangle(0,0, LARGURA, ALTSTATUS, BLACK);

        BeginDrawing();
        ClearBackground(WHITE);
        DrawTexture(chao, 0, 60, WHITE);

        EndDrawing();

    }


return 0;
}
