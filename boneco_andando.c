#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define LARGURA       1200
#define ALTURA        800
#define ALT_STATUS    60
#define CELULA        50
#define G_COLS        (LARGURA / CELULA)
#define G_LINS        ((ALTURA - ALT_STATUS) / CELULA)

int main(void) {
    // Inicialização da janela
    InitWindow(LARGURA, ALTURA, "ZELDASSO");
    SetTargetFPS(60);

    // Carrega texturas
    Texture2D texChao   = LoadTexture("chao_1200x740.png");
    Texture2D texParede = LoadTexture("parede.png");
    Texture2D texNorte  = LoadTexture("jogador-norte.png");
    Texture2D texSul    = LoadTexture("jogador-sul.png");
    Texture2D texLeste  = LoadTexture("jogador-leste.png");
    Texture2D texOeste  = LoadTexture("jogador-oeste.png");

    // Posição inicial (em células)
    int px = G_COLS / 2;
    int py = G_LINS / 2;
    // Orientação atual do sprite
    Texture2D *skin = &texSul;

    // Escalas para paredes e jogador
    float escalaParede = (float)CELULA / texParede.width;

    // Gera matriz de paredes (1 = parede, 0 = chão)
    srand(time(NULL));
    int grid[G_LINS][G_COLS];
    for (int y = 0; y < G_LINS; y++) {
        for (int x = 0; x < G_COLS; x++) {
            // Parede em aprox. 15% das células
            grid[y][x] = (rand() % 100 < 15) ? 1 : 0;
        }
    }
    // Garante que posição inicial esteja livre
    grid[py][px] = 0;

    // Loop principal do jogo
    while (!WindowShouldClose()) {
        // Calcula próxima posição conforme tecla
        int nx = px;
        int ny = py;
        if (IsKeyPressed(KEY_UP))    { ny--; skin = &texNorte; }
        if (IsKeyPressed(KEY_DOWN))  { ny++; skin = &texSul; }
        if (IsKeyPressed(KEY_LEFT))  { nx--; skin = &texOeste; }
        if (IsKeyPressed(KEY_RIGHT)) { nx++; skin = &texLeste; }

        // Atualiza posição se dentro dos limites e não houver parede
        if (nx >= 0 && nx < G_COLS && ny >= 0 && ny < G_LINS && grid[ny][nx] == 0) {
            px = nx;
            py = ny;
        }

        // Escala do jogador (pode variar se texturas tiverem tamanhos diferentes)
        float escalaJogador = (float)CELULA / skin->width;

        // Desenha tudo
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Barra de status
        DrawRectangle(0, 0, LARGURA, ALT_STATUS, BLACK);

        // Chão
        DrawTexture(texChao, 0, ALT_STATUS, WHITE);

        // Paredes
        for (int y = 0; y < G_LINS; y++) {
            for (int x = 0; x < G_COLS; x++) {
                if (grid[y][x] == 1) {
                    DrawTextureEx(texParede,
                                  (Vector2){ x * CELULA, ALT_STATUS + y * CELULA },
                                  0.0f, escalaParede, WHITE);
                }
            }
        }

        // Jogador
        DrawTextureEx(*skin,
                      (Vector2){ px * CELULA, ALT_STATUS + py * CELULA },
                      0.0f, escalaJogador, WHITE);

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
