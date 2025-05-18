#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define LARGURA       1200
#define ALTURA        860
#define ALT_STATUS    60
#define CELULA        50
#define COLUNAS        24
#define LINHAS        16

int main(void) {
    // Inicializa��o da janela
    InitWindow(LARGURA, ALTURA, "ZELDASSO");
    SetTargetFPS(60);

    // Carrega texturas
    Texture2D texChao   = LoadTexture("chao_1200x800.png");
    Texture2D texParede = LoadTexture("parede.png");
    Texture2D texNorte  = LoadTexture("jogador-norte.png");
    Texture2D texSul    = LoadTexture("jogador-sul.png");
    Texture2D texLeste  = LoadTexture("jogador-leste.png");
    Texture2D texOeste  = LoadTexture("jogador-oeste.png");

    // Posi��o inicial (em c�lulas)
    int px = COLUNAS / 2;
    int py = LINHAS / 2;
    // Orienta��o atual do sprite
    Texture2D *skin = &texSul;

    // Escalas para paredes e jogador
    float escalaParede = (float)CELULA / texParede.width;

    // Gera matriz de paredes (1 = parede, 0 = ch�o)
    srand(time(NULL));
    int grid[LINHAS][COLUNAS];
    for (int y = 0; y < LINHAS; y++) {
        for (int x = 0; x < COLUNAS; x++) {
            // Parede em aprox. 15% das c�lulas
            grid[y][x] = (rand() % 100 < 15) ? 1 : 0;
        }
    }
    // Garante que posi��o inicial esteja livre
    grid[py][px] = 0;

    // Loop principal do jogo
    while (!WindowShouldClose()) {
        // Calcula pr�xima posi��o conforme tecla
        int nx = px;
        int ny = py;
        if (IsKeyPressed(KEY_UP))    { ny--; skin = &texNorte; }
        if (IsKeyPressed(KEY_DOWN))  { ny++; skin = &texSul; }
        if (IsKeyPressed(KEY_LEFT))  { nx--; skin = &texOeste; }
        if (IsKeyPressed(KEY_RIGHT)) { nx++; skin = &texLeste; }

        // Atualiza posi��o se dentro dos limites e n�o houver parede
        if (nx >= 0 && nx < COLUNAS && ny >= 0 && ny < LINHAS && grid[ny][nx] == 0) {
            px = nx;
            py = ny;
        }

        // Escala do jogador (separada em X e Y)
       // float escalaJogadorX = (float)CELULA / skin->width;
       // float escalaJogadorY = (float)CELULA / skin->height;

        // Desenha tudo
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Barra de status
        DrawRectangle(0, 0, LARGURA, ALT_STATUS, BLACK);

        // Ch�o
        DrawTexture(texChao, 0, ALT_STATUS, WHITE);

        // Paredes
        for (int y = 0; y < LINHAS; y++) {
            for (int x = 0; x < COLUNAS; x++) {
                if (grid[y][x] == 1) {
                    DrawTextureEx(texParede,
                                  (Vector2){ x * CELULA, ALT_STATUS + y * CELULA },
                                  0.0f, escalaParede, WHITE);
                }
            }
        }

        // Jogador - usando DrawTexturePro para escala personalizada
        DrawTexturePro(
            *skin,
            (Rectangle){ 0, 0, (float)skin->width, (float)skin->height },          // parte da textura
            (Rectangle){ (float)(px * CELULA), (float)(ALT_STATUS + py * CELULA),  // posi��o na tela
                         (float)CELULA, (float)CELULA },                            // tamanho no destino
            (Vector2){ 0, 0 },   // origem para rota��o
            0.0f,                // rota��o
            WHITE
        );

        EndDrawing();
    }

    // Libera recursos e fecha
    UnloadTexture(texChao);
    UnloadTexture(texParede);
    UnloadTexture(texNorte);
    UnloadTexture(texSul);
    UnloadTexture(texLeste);
    UnloadTexture(texOeste);
    CloseWindow();
    return 0;
}
