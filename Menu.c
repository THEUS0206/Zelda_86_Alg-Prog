#include "raylib.h"

#define LARGURA_TELA 1200
#define ALTURA_TELA 800

int main() {
    // Inicializa a janela do jogo
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Zelda INF");
    SetTargetFPS(60); // Define a taxa de quadros por segundo

    // Vari�veis de controle do jogo
    int estadoDoJogo = 0; // 0: menu, 1: jogo, 2: placar
    int opcaoSelecionada = 0; // 0: Novo jogo, 1: Placar, 2: Sair

    // Inicia o loop principal do jogo
    while (!WindowShouldClose()) {
        // Obt�m a posi��o atual do mouse
        Vector2 posicaoDoMouse = GetMousePosition();

        // Verifica se alguma tecla foi pressionada para navega��o no menu
        if (IsKeyPressed(KEY_DOWN)) {
            opcaoSelecionada = (opcaoSelecionada + 1) % 3; // Move a sele��o para baixo
        }
        if (IsKeyPressed(KEY_UP)) {
            opcaoSelecionada = (opcaoSelecionada - 1 + 3) % 3; // Move a sele��o para cima
        }
        if (IsKeyPressed(KEY_ENTER)) {
            // Executa a a��o correspondente � op��o selecionada
            if (opcaoSelecionada == 0) {
                estadoDoJogo = 1; // Inicia um novo jogo
            } else if (opcaoSelecionada == 1) {
                estadoDoJogo = 2; // Mostra o placar
            } else if (opcaoSelecionada == 2) {
                CloseWindow(); // Fecha o jogo
            }
        }

        // Inicia o desenho na tela
        BeginDrawing();
        ClearBackground(BLACK); // Limpa a tela com fundo branco

        // Desenha os elementos de acordo com o estado do jogo
        switch (estadoDoJogo) {
            case 0: // Tela do menu principal
            {
                // Desenha o t�tulo do jogo
                DrawText("ZELDASSO", LARGURA_TELA / 2 - MeasureText("ZELDASSO", 40) / 2 , 100, 60, GREEN);

                // Define as �reas dos bot�es no menu
                Rectangle botaoNovoJogo = { 100, 400, 200, 50 };
                Rectangle botaoPlacar = { 100, 500, 200, 50 };
                Rectangle botaoSair = { 100, 600, 200, 50 };

                // Altera a cor dos bot�es com base na posi��o do mouse (hover)
                Color corBotaoNovoJogo = CheckCollisionPointRec(posicaoDoMouse, botaoNovoJogo) ? DARKGREEN : GRAY;
                Color corBotaoPlacar = CheckCollisionPointRec(posicaoDoMouse, botaoPlacar) ? DARKGREEN : GRAY;
                Color corBotaoSair = CheckCollisionPointRec(posicaoDoMouse, botaoSair) ? DARKGREEN : GRAY;

                // Desenha os bot�es na tela
                DrawRectangleRec(botaoNovoJogo, corBotaoNovoJogo);
                DrawText("Novo jogo", botaoNovoJogo.x + 10, botaoNovoJogo.y + 10, 20, DARKGRAY);

                DrawRectangleRec(botaoPlacar, corBotaoPlacar);
                DrawText("Placar", botaoPlacar.x + 10, botaoPlacar.y + 10, 20, DARKGRAY);

                DrawRectangleRec(botaoSair, corBotaoSair);
                DrawText("Sair", botaoSair.x + 10, botaoSair.y + 10, 20, DARKGRAY);

                // Destaca a op��o selecionada pelo teclado com uma borda vermelha
                if (opcaoSelecionada == 0) {
                    DrawRectangleLines(botaoNovoJogo.x - 5, botaoNovoJogo.y - 5, botaoNovoJogo.width + 10, botaoNovoJogo.height + 10, RED);
                } else if (opcaoSelecionada == 1) {
                    DrawRectangleLines(botaoPlacar.x - 5, botaoPlacar.y - 5, botaoPlacar.width + 10, botaoPlacar.height + 10, RED);
                } else if (opcaoSelecionada == 2) {
                    DrawRectangleLines(botaoSair.x - 5, botaoSair.y - 5, botaoSair.width + 10, botaoSair.height + 10, RED);
                }

                // Verifica cliques do mouse nos bot�es
                if (CheckCollisionPointRec(posicaoDoMouse, botaoNovoJogo) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    estadoDoJogo = 1; // Inicia um novo jogo
                }
                if (CheckCollisionPointRec(posicaoDoMouse, botaoPlacar) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    estadoDoJogo = 2; // Mostra o placar
                }
                if (CheckCollisionPointRec(posicaoDoMouse, botaoSair) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    CloseWindow(); // Fecha o jogo
                }
            }
            break;
            case 1: // Tela do jogo
                DrawText("Jogo em andamento", LARGURA_TELA / 2 - MeasureText("Jogo em andamento", 20) / 2, ALTURA_TELA / 2, 20, DARKGRAY);
                // Adicione aqui a l�gica principal do jogo
                DrawText("Pressione 'M' para voltar ao menu", 10, ALTURA_TELA - 30, 20, DARKGRAY);
                break;
            case 2: // Tela do placar
                DrawText("Placar", LARGURA_TELA / 2 - MeasureText("Placar", 40) / 2, 100, 40, DARKGRAY);
                // Adicione aqui a l�gica para exibir as pontua��es
                DrawText("Pressione 'M' para voltar ao menu", 10, ALTURA_TELA - 30, 20, DARKGRAY);
                break;
        }

        // Permite voltar ao menu principal pressionando 'M'
        if (estadoDoJogo != 0 && IsKeyPressed(KEY_M)) {
            estadoDoJogo = 0;
        }

        // Finaliza o desenho na tela
        EndDrawing();
    }

    // Fecha a janela do jogo
    CloseWindow();
    return 0;
}
