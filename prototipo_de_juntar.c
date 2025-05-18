#include <stdio.h>
#include "raylib.h"

#define LARGURA 1200
#define ALTURA 800
#define ALTSTATUS 60

int main(void) {
    InitWindow(LARGURA, ALTURA, "ZELDASSO");
    SetTargetFPS(60);

    // Carregar textura do chão
    Texture2D chao = LoadTexture("chao_1200x740.png");

    // Variáveis de controle
    int estadoDoJogo = 0; // 0: menu, 1: jogo

    while (!WindowShouldClose()) {
        Vector2 posicaoDoMouse = GetMousePosition();

        // Lógica do menu
        if (estadoDoJogo == 0) {
            // Definir o botão "Novo jogo"
            Rectangle botaoNovoJogo = { LARGURA / 2 - 100, 200, 200, 50 };

            // Verificar clique no botão
            if (CheckCollisionPointRec(posicaoDoMouse, botaoNovoJogo) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                estadoDoJogo = 1; // Muda para a tela do jogo
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        if (estadoDoJogo == 0) {
            // Desenhar menu
            DrawText("ZELDASSO", LARGURA / 2 - MeasureText("ZELDASSO", 40) / 2, 100, 60, DARKGRAY);
            Rectangle botaoNovoJogo = { LARGURA / 2 - 100, 200, 200, 50 };
            DrawRectangleRec(botaoNovoJogo, LIGHTGRAY);
            DrawText("Novo jogo", botaoNovoJogo.x + 10, botaoNovoJogo.y + 10, 20, DARKGRAY);
        } else if (estadoDoJogo == 1) {
            // Desenhar tela do jogo (seu código original)
            DrawRectangle(0, 0, LARGURA, ALTSTATUS, BLACK); // Barra de status
            DrawTexture(chao, 0, ALTSTATUS, WHITE); // Chão
        }

        EndDrawing();
    }

    UnloadTexture(chao); // Descarregar textura
    CloseWindow();
    return 0;
}
