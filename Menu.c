#include "raylib.h"

#define LARGURA_TELA 1200   // Largura da janela do jogo
#define ALTURA_TELA 860     // Altura da janela do jogo
#define ALT_STATUS    60    // Altura da barra de status superior
#define CELULA        50    // Tamanho de cada célula do mapa
#define COLUNAS       24    // Número de colunas do mapa em células
#define LINHAS        16    // Número de linhas do mapa em células

// Função que agrupa toda a lógica do jogo principal
void RunGame(void) {
    // --- Carregamento de texturas ---
    Texture2D texChao   = LoadTexture("chao_1200x800.png");
    Texture2D texParede = LoadTexture("parede.png");
    Texture2D texVida   = LoadTexture("vida.png");
    Texture2D texNorte  = LoadTexture("jogador-norte.png");
    Texture2D texSul    = LoadTexture("jogador-sul.png");
    Texture2D texLeste  = LoadTexture("jogador-leste.png");
    Texture2D texOeste  = LoadTexture("jogador-oeste.png");

    // Posição inicial do jogador (células)
    int px = COLUNAS / 2;
    int py = LINHAS / 2;
    Texture2D *skin = &texSul; // Textura inicial do jogador virado para o sul

    // --- Construção do mapa ---
    float escalaParede = (float)CELULA / texParede.width;
    int matriz_mapa[LINHAS][COLUNAS] = {0};
    // Preenche bordas com paredes (valor 1)
    for (int y = 0; y < LINHAS; y++) {
        for (int x = 0; x < COLUNAS; x++) {
            if (y == 0 || y == LINHAS - 1 || x == 0 || x == COLUNAS - 1) matriz_mapa[y][x] = 1;
        }
    }
    // Garante que a célula inicial do jogador esteja livre
    matriz_mapa[py][px] = 0;

    // Loop principal do jogo até o usuário fechar ou voltar ao menu
    while (!WindowShouldClose()) {
        // --- Movimentação ---
        int nx = px, ny = py;
        if (IsKeyPressed(KEY_UP))    { ny--; skin = &texNorte; }
        if (IsKeyPressed(KEY_DOWN))  { ny++; skin = &texSul;   }
        if (IsKeyPressed(KEY_LEFT))  { nx--; skin = &texOeste; }
        if (IsKeyPressed(KEY_RIGHT)) { nx++; skin = &texLeste; }
        if (nx >= 0 && nx < COLUNAS && ny >= 0 && ny < LINHAS && matriz_mapa[ny][nx] == 0) {
            px = nx; py = ny;
        }

        // --- Desenho ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Barra de status
        DrawRectangle(0, 0, LARGURA_TELA, ALT_STATUS, BLACK);
        DrawText("VIDAS:", 10, 10, 20, WHITE);

        // Vidas, nível e escore fixos
        int vidas = 3;
        float escalaVida = 20.0f / texVida.height;
        int larguraVidas = MeasureText("VIDAS:", 20);
        int coracaoX = 10 + larguraVidas + 10;
        for (int i = 0; i < vidas; i++) {
            DrawTextureEx(texVida, (Vector2){coracaoX + i*(texVida.width*escalaVida + 5), 10}, 0.0f, escalaVida, WHITE);
        }
        DrawText(TextFormat("NIVEL: %d", 1), coracaoX + vidas*(texVida.width*escalaVida + 5) + 20, 10, 20, WHITE);
        DrawText(TextFormat("ESCORE: %d", 0), coracaoX + vidas*(texVida.width*escalaVida + 5) + 140, 10, 20, WHITE);

        // Desenha chão e paredes
        DrawTexture(texChao, 0, ALT_STATUS, WHITE);
        for (int y = 0; y < LINHAS; y++) {
            for (int x = 0; x < COLUNAS; x++) {
                if (matriz_mapa[y][x] == 1) {
                    DrawTextureEx(texParede, (Vector2){x*CELULA, ALT_STATUS + y*CELULA}, 0.0f, escalaParede, WHITE);
                }
            }
        }

        // Desenha jogador
        DrawTexturePro(*skin,
                       (Rectangle){0, 0, (float)skin->width, (float)skin->height},
                       (Rectangle){px*CELULA, ALT_STATUS + py*CELULA, CELULA, CELULA},
                       (Vector2){0, 0}, 0.0f, WHITE);

        // Instrução para voltar ao menu
        DrawText("Pressione 'M' ou 'ESC' para voltar ao menu", 10, ALTURA_TELA-30, 20, DARKGRAY);
        EndDrawing();

        // Volta ao menu se apertar M ou ESC
        if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_ESCAPE)) break;
    }

    // Libera texturas
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

    int estadoDoJogo = 0;      // 0: menu, 1: jogo, 2: placar
    int opcaoSelecionada = 0;  // 0: Novo jogo, 1: Placar, 2: Sair

    while (!WindowShouldClose()) {
        Vector2 posicaoDoMouse = GetMousePosition();

        // Navegação no menu por teclado
        if (IsKeyPressed(KEY_DOWN)) opcaoSelecionada = (opcaoSelecionada + 1) % 3;
        if (IsKeyPressed(KEY_UP))   opcaoSelecionada = (opcaoSelecionada + 2) % 3;

        // Seleção por teclado ou mouse
        if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            int escolha = -1;
            if (IsKeyPressed(KEY_ENTER)) {
                escolha = opcaoSelecionada;
            } else {
                Rectangle botoes[3] = {{100,400,200,50}, {100,500,200,50}, {100,600,200,50}};
                for (int i = 0; i < 3; i++) {
                    if (CheckCollisionPointRec(posicaoDoMouse, botoes[i])) escolha = i;
                }
            }
            switch (escolha) {
                case 0: estadoDoJogo = 1; break;
                case 1: estadoDoJogo = 2; break;
                case 2: CloseWindow(); return 0;
                default: break;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        switch (estadoDoJogo) {
            case 0: { // Menu principal
                DrawText("ZELDASSO", LARGURA_TELA/2 - MeasureText("ZELDASSO",40)/2, 100, 60, GREEN);
                Rectangle botoes[3] = {{100,400,200,50}, {100,500,200,50}, {100,600,200,50}};
                const char *labels[3] = {"Novo jogo","Placar","Sair"};
                for (int i = 0; i < 3; i++) {
                    Color cor = CheckCollisionPointRec(posicaoDoMouse, botoes[i]) ? DARKGREEN : GRAY;
                    DrawRectangleRec(botoes[i], cor);
                    DrawText(labels[i], botoes[i].x+10, botoes[i].y+10, 20, DARKGRAY);
                    if (opcaoSelecionada == i) DrawRectangleLines(botoes[i].x-5,botoes[i].y-5,botoes[i].width+10,botoes[i].height+10,RED);
                }
            } break;
            case 1: // Tela de jogo
                RunGame();
                estadoDoJogo = 0; // volta ao menu após sair de RunGame
                break;
            case 2: // Tela de placar
                DrawText("Placar", LARGURA_TELA/2 - MeasureText("Placar",40)/2, 100, 40, DARKGRAY);
                DrawText("Pressione 'M' ou 'ESC' para voltar ao menu", 10, ALTURA_TELA-30, 20, DARKGRAY);
                if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_ESCAPE)) estadoDoJogo = 0;
                break;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
