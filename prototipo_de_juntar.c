#include "raylib.h"

#define LARGURA_TELA 1200
#define ALTURA_TELA 860
#define ALT_STATUS    60
#define CELULA        50
#define COLUNAS       24
#define LINHAS        16

// Função que agrupa a lógica do jogo (original de boneco_andando.c)
void RunGame(void) {
    // Carrega texturas
    Texture2D texChao   = LoadTexture("chao_1200x800.png");
    Texture2D texParede = LoadTexture("parede.png");
    Texture2D texVida   = LoadTexture("vida.png");
    Texture2D texNorte  = LoadTexture("jogador-norte.png");
    Texture2D texSul    = LoadTexture("jogador-sul.png");
    Texture2D texLeste  = LoadTexture("jogador-leste.png");
    Texture2D texOeste  = LoadTexture("jogador-oeste.png");

    // Posição inicial do jogador (em células)
    int px = COLUNAS / 2;
    int py = LINHAS / 2;
    Texture2D *skin = &texSul;

    // Escalas e geração do mapa
    float escalaParede = (float)CELULA / texParede.width;
    int matriz_mapa[LINHAS][COLUNAS] = {0};
    for (int y = 0; y < LINHAS; y++) {
        for (int x = 0; x < COLUNAS; x++) {
            if (y == 0 || y == LINHAS - 1 || x == 0 || x == COLUNAS - 1) matriz_mapa[y][x] = 1;
        }
    }
    matriz_mapa[py][px] = 0; // garante posição livre

    // Loop principal do jogo
    while (!WindowShouldClose()) {
        // Movimento
        int nx = px, ny = py;
        if (IsKeyPressed(KEY_UP))    { ny--; skin = &texNorte; }
        if (IsKeyPressed(KEY_DOWN))  { ny++; skin = &texSul;   }
        if (IsKeyPressed(KEY_LEFT))  { nx--; skin = &texOeste; }
        if (IsKeyPressed(KEY_RIGHT)) { nx++; skin = &texLeste; }
        if (nx >= 0 && nx < COLUNAS && ny >= 0 && ny < LINHAS && matriz_mapa[ny][nx] == 0) {
            px = nx; py = ny;
        }

        // Desenho
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Barra de status
        DrawRectangle(0, 0, LARGURA_TELA, ALT_STATUS, BLACK);
        DrawText("VIDAS:", 10, 10, 20, WHITE);

        // Desenha corações, nível e escore (exemplos fixos)
        int vidas = 3;
        float escalaVida = 20.0f / texVida.height;
        int larguraVidas = MeasureText("VIDAS:", 20);
        int coracaoX = 10 + larguraVidas + 10;
        for (int i = 0; i < vidas; i++) DrawTextureEx(texVida, (Vector2){coracaoX + i * (texVida.width * escalaVida + 5), 10}, 0.0f, escalaVida, WHITE);
        DrawText(TextFormat("NIVEL: %d", 1), coracaoX + vidas * (texVida.width * escalaVida + 5) + 20, 10, 20, WHITE);
        DrawText(TextFormat("ESCORE: %d", 0), coracaoX + vidas * (texVida.width * escalaVida + 5) + 140, 10, 20, WHITE);

        // Desenha chão e paredes
        DrawTexture(texChao, 0, ALT_STATUS, WHITE);
        for (int y = 0; y < LINHAS; y++) {
            for (int x = 0; x < COLUNAS; x++) {
                if (matriz_mapa[y][x] == 1) {
                    DrawTextureEx(texParede, (Vector2){x * CELULA, ALT_STATUS + y * CELULA}, 0.0f, escalaParede, WHITE);
                }
            }
        }

        // Desenha jogador
        DrawTexturePro(*skin,
                       (Rectangle){0, 0, (float)skin->width, (float)skin->height},
                       (Rectangle){(float)(px * CELULA), (float)(ALT_STATUS + py * CELULA), (float)CELULA, (float)CELULA},
                       (Vector2){0, 0}, 0.0f, WHITE);

        // Retornar ao menu
        DrawText("Pressione 'M' para voltar ao menu", 10, ALTURA_TELA - 30, 20, DARKGRAY);
        EndDrawing();

        if (IsKeyPressed(KEY_M)) break;
    }

    // Libera recursos do jogo
    UnloadTexture(texChao);
    UnloadTexture(texParede);
    UnloadTexture(texVida);
    UnloadTexture(texNorte);
    UnloadTexture(texSul);
    UnloadTexture(texLeste);
    UnloadTexture(texOeste);
}

int main(void) {
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Zelda INF");
    SetTargetFPS(60);

    int opcaoSelecionada = 0;

    while (!WindowShouldClose()) {
        Vector2 posicaoDoMouse = GetMousePosition();
        if (IsKeyPressed(KEY_DOWN)) opcaoSelecionada = (opcaoSelecionada + 1) % 3;
        if (IsKeyPressed(KEY_UP))   opcaoSelecionada = (opcaoSelecionada - 1 + 3) % 3;

        if (IsKeyPressed(KEY_ENTER) || (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) {
            // Verifica clique no mouse apenas para botões
            Rectangle botaoNovo  = {100, 400, 200, 50};
            Rectangle botaoPlacar= {100, 500, 200, 50};
            Rectangle botaoSair  = {100, 600, 200, 50};
            if (opcaoSelecionada == 0 || CheckCollisionPointRec(posicaoDoMouse, botaoNovo)) {
                RunGame();
            } else if (opcaoSelecionada == 1 || CheckCollisionPointRec(posicaoDoMouse, botaoPlacar)) {
                // lógica de placar (a implementar)
            } else if (opcaoSelecionada == 2 || CheckCollisionPointRec(posicaoDoMouse, botaoSair)) {
                break;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("ZELDASSO", LARGURA_TELA/2 - MeasureText("ZELDASSO", 40)/2, 100, 60, GREEN);

        Rectangle botoes[3] = {{100,400,200,50},{100,500,200,50},{100,600,200,50}};
        const char *labels[3] = {"Novo jogo","Placar","Sair"};
        for (int i = 0; i < 3; i++) {
            Color c = CheckCollisionPointRec(posicaoDoMouse, botoes[i]) ? DARKGREEN : GRAY;
            DrawRectangleRec(botoes[i], c);
            DrawText(labels[i], botoes[i].x+10, botoes[i].y+10, 20, DARKGRAY);
            if (opcaoSelecionada == i) {
                DrawRectangleLines(botoes[i].x-5,botoes[i].y-5,botoes[i].width+10,botoes[i].height+10,RED);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
