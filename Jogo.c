#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

// Constantes do jogo
#define LINHAS 16
#define COLUNAS 24
#define CELULA 50
#define BARRA_STATUS 60
#define MAPA_AREA 800
#define LARGURA_TELA 1200
#define ALTURA_TELA ( BARRA_STATUS + MAPA_AREA )
#define MAX_MONSTROS 10
#define MAX_VIDA_EXTRA 5
#define MAX_NIVEIS 99
#define ALCANCE_ESPADA 3
#define MAX_RANKING 5

// Estruturas de dados
typedef enum { NORTE, SUL, LESTE, OESTE } Direcao_t;

typedef struct {
    int x, y;
    Direcao_t direcao;
    int score;
    int vidas;
    bool tem_espada;
    bool ativa_espada;
    float cooldown_espada;
} Jogador_t;

typedef struct {
    int x, y;
    Direcao_t direcao;
    int pontos;
    bool vivo;
    float cooldown_mover;
} Monstro_t;

typedef struct {
    int x, y;
    bool coletado;
} Vida_Extra_t;

typedef struct {
    int x, y;
    bool coletado;
} Espada_t;

typedef struct {
    char nome[20];
    int score;
} Score_t;

typedef enum { MENU, JOGANDO, PAUSA, DERROTA, VITORIA, SCOREBOARD } Estado_jogo_t;

// Variáveis globais permitidas
char mapa[LINHAS][COLUNAS];
Texture2D textura_jogador[4];  // N, S, L, O
Texture2D textura_monstros[4]; // N, S, L, O
Texture2D textura_parede;
Texture2D textura_espada;
Texture2D textura_vida;
Texture2D textura_chao;
Texture2D textura_menu;
Font fonte_jogo;

// Protótipos de funções
void Atualiza_jogo(Estado_jogo_t *Estado, int *nivel_atual, Jogador_t *jogador,
                Monstro_t monstros[], int *numero_monstros,
                Vida_Extra_t vidas_extra[], int *numero_vidas_extra,
                Espada_t *espada);

void Atualiza_ranking(int score, Score_t ranking[]);

bool Carrega_mapa(int level);

void Comeca_jogo(Estado_jogo_t *estado, int *niveis_atual, Jogador_t *jogador,
              Monstro_t monstros[], int *numero_monstros,
              Vida_Extra_t vidas_extra[], int *numero_vidas_extra,
              Espada_t *espada);

void Checar_colisoes(Estado_jogo_t *estado, Jogador_t *jogador,
                     Monstro_t monstros[], int numero_monstros,
                     Vida_Extra_t vidas_extra[], int numero_vidas_extra,
                     Espada_t *espada);

bool Carrega_ranking(Score_t ranking[]);

void Carrega_texturas();

void Descarrega_texturas();

void Desenha_jogo( Jogador_t *jogador, Monstro_t monstros[], int numero_monstros,
              Vida_Extra_t vidas_extra[], int numero_vidas_extra,
              Espada_t *espada, int nivel_atual);

void Desenha_menu(Estado_jogo_t *estado, int *selecao_menu);

void Desenha_scoreboard(Score_t ranking[]);

void Desenha_menu_pausa(int *selecao_pause);

void Desenha_menu_derrota(int *selecao_derrota);

void Desenha_menu_vitoria(int *selecao_vitoria);

void Desenha_barra_status(Jogador_t *jogador, int level_atual);

void Movimento_jogador(Jogador_t *jogador, int tecla);

void Movimento_monstros(Monstro_t monstros[], int numero_monstros);

bool Posicao_valida(int x, int y, char mapa[LINHAS][COLUNAS]);

void Reseta_nivel(int level, Jogador_t *jogador, Monstro_t monstros[], int *numero_monstros,
                Vida_Extra_t vidas_extra[], int *numero_vidas_extra, Espada_t *espada);

void Salva_ranking(Score_t ranking[]);

void Teleportar_entidade(int *px, int *py, char mapa[LINHAS][COLUNAS]);

void Usar_espada(Jogador_t *jogador, Monstro_t monstros[], int numero_monstros);


int main(void) {
    // Inicialização da janela
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Zelda INF");
    InitAudioDevice();
    SetTargetFPS(60);

    // Carregar recursos
    Carrega_texturas();
    fonte_jogo = LoadFont("alagard.png");

    // Variáveis de estado do jogo
    Estado_jogo_t estado = MENU;
    int level_atual = 1;
    int selecao_menu = 0;
    int selecao_pause = 0;
    int selecao_derrota = 0;
    int selecao_vitoria = 0;

    // Entidades do jogo
    Jogador_t jogador;
    Monstro_t monstros[MAX_MONSTROS];
    Vida_Extra_t vidas_extra[MAX_VIDA_EXTRA];
    Espada_t espada;
    int numero_monstros = 0;
    int numero_vidas_extra = 0;

    // Ranking
    Score_t ranking[5];
    if (!Carrega_ranking(ranking)) {
        // Inicializar ranking padrão se arquivo não existir
        for (int i = 0; i < 5; i++) {
            sprintf(ranking[i].nome, "Jogador %d", i+1);
            ranking[i].score = 1000 - i * 200;
        }
    }

    // Loop principal do jogo
    while (!WindowShouldClose()) {
        switch (estado) {
            case MENU:
                // Navegação no menu
                if (IsKeyPressed(KEY_DOWN)) selecao_menu = (selecao_menu + 1) % 3;
                if (IsKeyPressed(KEY_UP)) selecao_menu = (selecao_menu + 2) % 3;

                if (IsKeyPressed(KEY_ENTER)) {
                    switch (selecao_menu) {
                        case 0: // Novo jogo
                            level_atual = 1;
                            Comeca_jogo(&estado, &level_atual, &jogador, monstros, &numero_monstros,
                                     vidas_extra, &numero_vidas_extra, &espada);
                            estado = JOGANDO;
                            break;
                        case 1: // Scoreboard
                            estado = SCOREBOARD;
                            break;
                        case 2: // Sair
                            CloseWindow();
                            return 0;
                    }
                }
                break;

            case JOGANDO:
                Atualiza_jogo(&estado, &level_atual, &jogador, monstros, &numero_monstros,
                           vidas_extra, &numero_vidas_extra, &espada);
                break;

            case PAUSA:
                // Navegação no menu de pausa
                if (IsKeyPressed(KEY_DOWN)) selecao_pause = (selecao_pause + 1) % 3;
                if (IsKeyPressed(KEY_UP)) selecao_pause = (selecao_pause + 2) % 3;

                if (IsKeyPressed(KEY_ENTER)) {
                    switch (selecao_pause) {
                        case 0: // Continuar
                            estado = JOGANDO;
                            break;
                        case 1: // Voltar ao menu
                            estado = MENU;
                            break;
                        case 2: // Sair
                            CloseWindow();
                            return 0;
                    }
                }
                break;

            case DERROTA:
                // Navegação no menu de game over
                if (IsKeyPressed(KEY_DOWN)) selecao_derrota = (selecao_derrota + 1) % 3;
                if (IsKeyPressed(KEY_UP)) selecao_derrota = (selecao_derrota + 2) % 3;

                if (IsKeyPressed(KEY_ENTER)) {
                    switch (selecao_derrota) {
                        case 0: // Carregar jogo (não implementado)
                            // Implementar se for requisito extra
                            break;
                        case 1: // Reiniciar jogo
                            level_atual = 1;
                            Comeca_jogo(&estado, &level_atual, &jogador, monstros, &numero_monstros,
                                     vidas_extra, &numero_vidas_extra, &espada);
                            estado = JOGANDO;
                            break;
                        case 2: // Voltar ao menu
                            estado = MENU;
                            break;
                    }
                }
                break;

            case VITORIA:
                // Navegação na tela de vitória
                if (IsKeyPressed(KEY_DOWN)) selecao_vitoria = (selecao_vitoria + 1) % 2;
                if (IsKeyPressed(KEY_UP)) selecao_vitoria = (selecao_vitoria + 1) % 2;

                if (IsKeyPressed(KEY_ENTER)) {
                    switch (selecao_vitoria) {
                        case 0: // Voltar ao menu
                            estado = MENU;
                            break;
                        case 1: // Sair
                            CloseWindow();
                            return 0;
                    }
                }
                break;

            case SCOREBOARD:
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
                    estado = MENU;
                }
                break;
        }

        // Renderização
        BeginDrawing();
            ClearBackground(RAYWHITE);

            switch (estado) {
                case MENU:
                    Desenha_menu(&estado, &selecao_menu);
                    break;

                case JOGANDO:
                    Desenha_jogo(&jogador, monstros, numero_monstros, vidas_extra, numero_vidas_extra, &espada, level_atual);
                    break;

                case PAUSA:
                    Desenha_jogo(&jogador, monstros, numero_monstros, vidas_extra, numero_vidas_extra, &espada, level_atual);
                    Desenha_menu_pausa(&selecao_pause);
                    break;

                case DERROTA:
                    Desenha_menu_derrota(&selecao_derrota);
                    break;

                case VITORIA:
                    Desenha_menu_vitoria(&selecao_vitoria);
                    break;

                case SCOREBOARD:
                    Desenha_scoreboard(ranking);
                    break;
            }
        EndDrawing();
    }

    // Salvar ranking e liberar recursos
    Salva_ranking(ranking);
    Descarrega_texturas();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

void Atualiza_jogo(Estado_jogo_t *estado, int *level_atual, Jogador_t *jogador,
                Monstro_t monstros[], int *numero_monstros,
                Vida_Extra_t vidas_extra[], int *numero_vidas_extra,
                Espada_t *espada) {
    // Pausar o jogo
    if (IsKeyPressed(KEY_TAB)) {
        *estado = PAUSA;
        return;
    }

    // Movimento do jogador
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) Movimento_jogador(jogador, KEY_W);
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) Movimento_jogador(jogador, KEY_S);
    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) Movimento_jogador(jogador, KEY_A);
    if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) Movimento_jogador(jogador, KEY_D);

    // Ativar espada
    if (IsKeyPressed(KEY_J) && jogador->tem_espada && !jogador->ativa_espada) {
        jogador->ativa_espada = true;
        jogador->cooldown_espada = 0.2f; // 0.2 segundos de duração
    }

    // Atualizar temporizador da espada
    if (jogador->ativa_espada) {
        jogador->cooldown_espada -= GetFrameTime();
        if (jogador->cooldown_espada <= 0) {
            jogador->ativa_espada = false;
        } else {
            // Verificar se a espada atingiu monstros
            Usar_espada(jogador, monstros, *numero_monstros);
        }
    }

    // Movimento dos monstros
    Movimento_monstros(monstros, *numero_monstros);

    // Verificar colisões
    Checar_colisoes(estado, jogador, monstros, *numero_monstros,
                   vidas_extra, *numero_vidas_extra, espada);

    // Verificar vitória na fase
    bool monstros_morreram = (*numero_monstros > 0);
    for (int i = 0; i < *numero_monstros; i++) {
        if (monstros[i].vivo) {
            monstros_morreram = false;
        }
    }

    if (monstros_morreram) {
        (*level_atual)++;
        if (*level_atual > MAX_NIVEIS) {
            *estado = VITORIA;
        } else {
            Reseta_nivel(*level_atual, jogador, monstros, numero_monstros,
                       vidas_extra, numero_vidas_extra, espada);
        }
    }
}

void Atualiza_ranking(int score, Score_t ranking[]) {
    char nome[20];
    printf("Digite seu nome para o ranking: ");
    fgets(nome, 20, stdin);


    // Determina a posição de inserção
    int posição = MAX_RANKING;
    for (int i = 0; i < MAX_RANKING; i++) {
        if (score > ranking[i].score) {
            posição = i;
            break;
        }
    }

    // Se for pontuação suficiente para entrar no ranking
    if (posição < MAX_RANKING) {
        // Desloca os valores inferiores
        for (int j = MAX_RANKING - 1; j > posição; j--) {
            ranking[j] = ranking[j - 1];
        }
        // Insere novo recorde
        strcpy(ranking[posição].nome, nome);
        ranking[posição].score = score;
    }
}

bool Carrega_ranking(Score_t ranking[]) {
    FILE *file = fopen("ranking.bin", "rb");
    if (!file) {return false;};

    fread(ranking, sizeof(Score_t), 5, file);
    fclose(file);
    return true;
}

bool Carrega_mapa(int level) {
    char mapas[LINHAS];
    char temporario[COLUNAS + 2];
    // gera “mapa01.txt”, “mapa02.txt”...
    sprintf(mapas, "mapa%d.txt", level);

    FILE *f = fopen(mapas, "r");
     if (!f) {
        // não encontrou o arquivo - vitoria
        return false;
    }
      // +1 para '\n' e +1 para '\0'
    for (int i = 0; i < LINHAS; i++) {
        if (fgets(temporario, sizeof temporario, f)) {
        // copia exatamente COLUNAS caracteres (ignora '\n' se houver)
        for (int j = 0; j < COLUNAS; j++) {
            mapa[i][j] = temporario[j];
        }
    }}

    fclose(f);
    return true;

}

void Carrega_texturas() {
    // Carregar texturas do jogador para cada direção
    textura_jogador[NORTE] = LoadTexture("jogador-norte.png");
    textura_jogador[SUL]   = LoadTexture("jogador-sul.png");
    textura_jogador[LESTE] = LoadTexture("jogador-leste.png");
    textura_jogador[OESTE] = LoadTexture("jogador-oeste.png");

   // Carregar texturas dos monstros para cada direção
    textura_monstros[NORTE] = LoadTexture("monstro-norte.png");
    textura_monstros[SUL]   = LoadTexture("monstro-sul.png");
    textura_monstros[LESTE] = LoadTexture("monstro-leste.png");
    textura_monstros[OESTE] = LoadTexture("monstro-oeste.png");

    // Carregar texturas de outros elementos
    textura_menu   = LoadTexture("menu.png");
    textura_parede = LoadTexture("parede.png");
    textura_espada = LoadTexture("espada.png");
    textura_vida   = LoadTexture("vida.png");
    textura_chao   = LoadTexture("chao.png");
}

void Comeca_jogo(Estado_jogo_t *estado, int *level_atual, Jogador_t *jogador,
              Monstro_t monstros[], int *numero_monstros,
              Vida_Extra_t vidas_extra[], int *numero_vidas_extra,
              Espada_t *espada) {
    // Carregar mapa

   if (!Carrega_mapa(*level_atual)) {
        *estado = VITORIA;
        return;
    }

    // Inicializar jogador
    jogador->x = 1;
    jogador->y = 1;
    jogador->direcao = SUL;
    jogador->score = 0;
    jogador->vidas = 3;
    jogador->tem_espada = false;
    jogador->ativa_espada = false;
    jogador->cooldown_espada = 0.0f;

    // Encontrar posições iniciais
    *numero_monstros = 0;
    *numero_vidas_extra = 0;
    espada->coletado = false;

    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            switch (mapa[i][j]) {
                case 'J': // Jogador
                    jogador->x = j;
                    jogador->y = i;
                    break;

                case 'M': // Monstro
                    if (*numero_monstros < MAX_MONSTROS) {
                        monstros[*numero_monstros].x = j;
                        monstros[*numero_monstros].y = i;
                        monstros[*numero_monstros].direcao = rand() % 4;
                        monstros[*numero_monstros].pontos = rand() % 101;
                        monstros[*numero_monstros].vivo = true;
                        monstros[*numero_monstros].cooldown_mover = 0.0f;
                        (*numero_monstros)++;
                    }
                    break;

                case 'E': // Espada
                    espada->x = j;
                    espada->y = i;
                    break;

                case 'V': // Vida extra
                    if (*numero_vidas_extra < MAX_VIDA_EXTRA) {
                        vidas_extra[*numero_vidas_extra].x = j;
                        vidas_extra[*numero_vidas_extra].y = i;
                        vidas_extra[*numero_vidas_extra].coletado = false;
                        (*numero_vidas_extra)++;
                    }
                    break;
            }
        }
    }

    *estado = JOGANDO;
}




void Checar_colisoes(Estado_jogo_t *estado, Jogador_t *jogador,
                     Monstro_t monstros[], int numero_monstros,
                     Vida_Extra_t vidas_extra[], int numero_vidas_extra,
                     Espada_t *espada) {

    // Verificar colisão com monstros
    for (int i = 0; i < numero_monstros; i++) {
        if (monstros[i].vivo && monstros[i].x == jogador->x && monstros[i].y == jogador->y) {
            jogador->vidas--;
            if (jogador->vidas <= 0) {
                *estado = DERROTA;
            }else{
            // Reposicionar monstro após contato
            Teleportar_entidade(&monstros[i].x, &monstros[i].y, mapa);}
        }
    }

    // Verificar colisão com espada
    if (!espada->coletado && !jogador->tem_espada &&
        espada->x == jogador->x && espada->y == jogador->y) {
        jogador->tem_espada = true;
        espada->coletado = true;
    }

    // Verificar colisão com vidas extras
    for (int i = 0; i < numero_vidas_extra; i++) {
        if (!vidas_extra[i].coletado &&
            vidas_extra[i].x == jogador->x && vidas_extra[i].y == jogador->y) {
            jogador->vidas++;
            vidas_extra[i].coletado = true;
        }
    }
}

void Desenha_jogo(Jogador_t *jogador, Monstro_t monstros[], int numero_monstros,
              Vida_Extra_t vidas_extra[], int numero_vidas_extra,
              Espada_t *espada, int level_atual) {

    float escala_chao     = (float)CELULA / textura_chao.width;
    float escala_parede   = (float)CELULA / textura_parede.width;
    float escala_monstro  = (float)CELULA / textura_monstros[0].width;
    float escala_espada   = (float)CELULA / textura_espada.width;
    float escala_vida     = (float)CELULA / textura_vida.width;

     // Desenhar fundo (chão e paredes)
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            // Desenhar chão
            DrawTextureEx(textura_chao,
                          (Vector2){ j * CELULA, i * CELULA + BARRA_STATUS },
                          0.0f, escala_chao, WHITE);

            // Desenhar parede, se existir
            if (mapa[i][j] == 'P') {
                DrawTextureEx(textura_parede,
                              (Vector2){ j * CELULA, i * CELULA + BARRA_STATUS },
                              0.0f, escala_parede, WHITE);
            }
        }
    }

    // Desenhar itens: espada e vidas extra espalhadas no mapa
    for (int i = 0; i < numero_vidas_extra; i++) {
       if (!vidas_extra[i].coletado){
                DrawTextureEx(textura_vida,
                (Vector2){ vidas_extra[i].x * CELULA, vidas_extra[i].y * CELULA + BARRA_STATUS },
                0.0f, escala_vida, WHITE);
       }
    }

    if (!espada->coletado) {
            DrawTextureEx(textura_espada,
            (Vector2){ espada->x * CELULA, espada->y * CELULA + BARRA_STATUS },
            0.0f, escala_espada, WHITE);
    }

    // Desenhar monstros
    for (int i = 0; i < numero_monstros; i++) {
        if (monstros[i].vivo) {
            DrawTextureEx(textura_monstros[monstros[i].direcao],
                          (Vector2){ monstros[i].x * CELULA, monstros[i].y * CELULA + BARRA_STATUS },
                          0.0f, escala_monstro, WHITE);
        }
    }

    // Desenhar jogador com DrawTexturePro para garantir mesma largura e altura
    Texture2D tex_jogador = textura_jogador[jogador->direcao];
    Rectangle fonte = { 0.0f, 0.0f, (float)tex_jogador.width, (float)tex_jogador.height };
    Rectangle destino = { jogador->x * CELULA,
                          jogador->y * CELULA + BARRA_STATUS,
                          (float)CELULA, (float)CELULA };
    Vector2 origem = { 0.0f, 0.0f };
    DrawTexturePro(tex_jogador, fonte, destino, origem, 0.0f, WHITE);


    // Desenhar área de ataque da espada
    if (jogador->ativa_espada) {
    int x0 = jogador->x;
    int y0 = jogador->y;
    int x1 = x0;
    int y1 = y0;

    // 1) Define ponto final conforme a direção
    switch (jogador->direcao) {
        case NORTE: y1 -= ALCANCE_ESPADA; break;
        case SUL:   y1 += ALCANCE_ESPADA; break;
        case LESTE: x1 += ALCANCE_ESPADA; break;
        case OESTE: x1 -= ALCANCE_ESPADA; break;
    }

    // 2) Limita dentro do mapa
    if (x1 < 0) x1 = 0;
    if (x1 >= COLUNAS) x1 = COLUNAS - 1;
    if (y1 < 0) y1 = 0;
    if (y1 >= LINHAS) y1 = LINHAS - 1;

    // 3) Calcula intervalos
    int min_x = (x0 < x1) ? x0 : x1;
    int max_x = (x0 > x1) ? x0 : x1;
    int min_y = (y0 < y1) ? y0 : y1;
    int max_y = (y0 > y1) ? y0 : y1;

    // 4) Desenha a “linha” de ataque: ou na coluna do jogador (vertical)
    //    ou na linha do jogador (horizontal)
    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            if (x == x0 || y == y0) {
                DrawRectangle(
                  x * CELULA,
                  y * CELULA + BARRA_STATUS,
                  CELULA, CELULA,
                  (Color){255, 0, 0, 128}
                );
            }
        }
    }
    }

    // Desenhar barra de status
    Desenha_barra_status(jogador, level_atual);
}

void Desenha_barra_status(Jogador_t *jogador, int level_atual) {
    float escala_vida     = (float)CELULA / textura_vida.width;
    // Fundo da barra de status
    DrawRectangle(0, 0, LARGURA_TELA, BARRA_STATUS, DARKGRAY);

    // Textos
    DrawTextEx(fonte_jogo, TextFormat("Pontos: %d", jogador->score),
               (Vector2){20, 20}, 20, 2, YELLOW);

    DrawTextEx(fonte_jogo, TextFormat("Nivel: %d", level_atual),
               (Vector2){LARGURA_TELA/2 - 50, 20}, 20, 2, WHITE);

    // Vidas (representadas por corações)
    for (int i = 0; i < jogador->vidas; i++) {
        DrawTextureEx(textura_vida,
                      (Vector2){ LARGURA_TELA - 100 - i * (int)(CELULA * escala_vida), 10 },
                      0.0f, escala_vida, WHITE);
    }

    // Indicador de espada
    if (jogador->tem_espada) {
        DrawTextEx(fonte_jogo, "ESPADA", (Vector2){LARGURA_TELA - 200, 20}, 20, 2, BLUE);
    }
}

// Implementações das funções de menu (simplificadas)
void Desenha_menu(Estado_jogo_t *estado, int *selecao_menu) {
    (void)estado;
    Rectangle origem = { 0.0f, 0.0f, (float)textura_menu.width, (float)textura_menu.height };
    Rectangle destino = { 0.0f, 0.0f, 1200, 860 }; // tamanho desejado
    Vector2 origem_desenho = { 0.0f, 0.0f };

    DrawTexturePro(textura_menu, origem, destino, origem_desenho, 0.0f, WHITE);

    // Fundo translúcido por cima para contraste
    DrawRectangle(0, 0, 1200, 860, (Color){0, 0, 0, 150});

    // Título
    DrawTextEx(fonte_jogo, "ZELDA INF", (Vector2){LARGURA_TELA/2 - 100, 100}, 48, 2, GOLD);

    // Opções
    const char *opcoes[] = {"Novo Jogo", "Scoreboard", "Sair"};
    for (int i = 0; i < 3; i++) {
        Color cor = (i == *selecao_menu) ? YELLOW : WHITE;
        DrawTextEx(fonte_jogo, opcoes[i], (Vector2){LARGURA_TELA/2 - 80, 200 + i * 60}, 30, 2, cor);
    }
}

void Desenha_menu_pausa(int *selecao_pause) {
    // Sobrepor semi-transparente
    DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, (Color){0, 0, 0, 180});

    // Título
    DrawTextEx(fonte_jogo, "JOGO PAUSADO", (Vector2){LARGURA_TELA/2 - 120, 150}, 36, 2, WHITE);

    // Opções
    const char *opcoes[] = {"Continuar", "Voltar ao Menu", "Sair"};
    for (int i = 0; i < 3; i++) {
        Color cor = (i == *selecao_pause) ? YELLOW : LIGHTGRAY;
        DrawTextEx(fonte_jogo, opcoes[i], (Vector2){LARGURA_TELA/2 - 80, 250 + i * 60}, 30, 2, cor);
    }
}


void Descarrega_texturas() {
    for (int i = 0; i < 4; i++) {
    UnloadTexture(textura_jogador[i]);
    }
    for (int i = 0; i < 4; i++) {
        UnloadTexture(textura_monstros[i]);
    }
    UnloadTexture(textura_parede);
    UnloadTexture(textura_espada);
    UnloadTexture(textura_vida);
    UnloadTexture(textura_chao);
    UnloadFont(fonte_jogo);
}

// Implementações restantes de menus (simplificadas)
void Desenha_scoreboard(Score_t ranking[]) {
    DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, (Color){30, 30, 50, 255});
    DrawTextEx(fonte_jogo, "TOP 5 PONTUACOES", (Vector2){LARGURA_TELA/2 - 150, 50}, 36, 2, GOLD);

    for (int i = 0; i < 5; i++) {
        DrawTextEx(fonte_jogo, TextFormat("%d. %s: %d", i+1, ranking[i].nome, ranking[i].score),
                  (Vector2){LARGURA_TELA/2 - 150, 150 + i * 60}, 30, 2, WHITE);
    }

    DrawTextEx(fonte_jogo, "Pressione ENTER para voltar",
              (Vector2){LARGURA_TELA/2 - 180, ALTURA_TELA - 50}, 20, 2, LIGHTGRAY);
}

void Desenha_menu_derrota(int *selecao_derrota) {
    DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, (Color){50, 0, 0, 200});
    DrawTextEx(fonte_jogo, "GAME OVER", (Vector2){LARGURA_TELA/2 - 120, 150}, 48, 2, RED);

    const char *opcoes[] = {"Carregar Jogo", "Reiniciar Jogo", "Voltar ao Menu"};
    for (int i = 0; i < 3; i++) {
        Color cor = (i == *selecao_derrota) ? YELLOW : LIGHTGRAY;
        DrawTextEx(fonte_jogo, opcoes[i], (Vector2){LARGURA_TELA/2 - 100, 250 + i * 60}, 30, 2, cor);
    }
}

void Desenha_menu_vitoria(int *selecao_vitoria) {
    DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, (Color){0, 50, 0, 200});
    DrawTextEx(fonte_jogo, "VITORIA!", (Vector2){LARGURA_TELA/2 - 100, 150}, 48, 2, GREEN);
    DrawTextEx(fonte_jogo, "Parabens! Voce completou todos os niveis!",
              (Vector2){LARGURA_TELA/2 - 250, 220}, 24, 2, LIGHTGRAY);

    const char *opcoes[] = {"Voltar ao Menu", "Sair"};
    for (int i = 0; i < 2; i++) {
        Color cor = (i == *selecao_vitoria) ? YELLOW : LIGHTGRAY;
        DrawTextEx(fonte_jogo, opcoes[i], (Vector2){LARGURA_TELA/2 - 80, 300 + i * 60}, 30, 2, cor);
    }
}

void Movimento_jogador(Jogador_t *jogador, int tecla) {
    int novo_x = jogador->x;
    int novo_y = jogador->y;
    Direcao_t nova_direcao = jogador->direcao;

    switch (tecla) {
        case KEY_W:
        case KEY_UP:
            novo_y--;
            nova_direcao = NORTE;
            break;
        case KEY_S:
        case KEY_DOWN:
            novo_y++;
            nova_direcao = SUL;
            break;
        case KEY_A:
        case KEY_LEFT:
            novo_x--;
            nova_direcao = OESTE;
            break;
        case KEY_D:
        case KEY_RIGHT:
            novo_x++;
            nova_direcao = LESTE;
            break;
        default:
            return;
    }

            // Verificar limites do mapa e colisão com paredes
    if (novo_x >= 0 && novo_x < COLUNAS && novo_y >= 0 && novo_y < LINHAS) {
        if (mapa[novo_y][novo_x] != 'P') {
            jogador->x = novo_x;
            jogador->y = novo_y;
            jogador->direcao = nova_direcao;
        }
    }
}


void Movimento_monstros(Monstro_t monstros[], int numero_monstros) {
    for (int i = 0; i < numero_monstros; i++) {
        if (monstros[i].vivo){

        // Atualizar temporizador de movimento
        monstros[i].cooldown_mover -= GetFrameTime();
        if (monstros[i].cooldown_mover <= 0) {

        // Resetar temporizador (0.5-1.5 segundos)
        monstros[i].cooldown_mover = 0.5f + (float)rand() / (float)RAND_MAX;

        // Escolher direção aleatória
        int direcao = rand() % 4;
        int novo_x = monstros[i].x;
        int novo_y = monstros[i].y;

        switch (direcao) {
            case 0: novo_y--; break; // Norte
            case 1: novo_y++; break; // Sul
            case 2: novo_x++; break; // Leste
            case 3: novo_x--; break; // Oeste
        }

        // Verificar limites do mapa e colisão com paredes
        if (novo_x >= 0 && novo_x < COLUNAS && novo_y >= 0 && novo_y < LINHAS) {
            if (mapa[novo_y][novo_x] != 'P') {
                monstros[i].x = novo_x;
                monstros[i].y = novo_y;
                monstros[i].direcao = direcao;
            }}
        }
    }
}}

bool Posicao_valida(int x, int y, char mapa[LINHAS][COLUNAS]) {
    return mapa[y][x] != 'P';  // ou o caracter que você usa pra parede
}

void Reseta_nivel(int level, Jogador_t *jogador, Monstro_t monstros[], int *numero_monstros, Vida_Extra_t vidas_extra[], int *numero_vidas_extra, Espada_t *espada) {

    // Manter pontuação e vidas do jogador
    int score = jogador->score;
    int vidas = jogador->vidas;


    // Recarregar o nível
    Comeca_jogo(&(Estado_jogo_t){0}, &level, jogador, monstros, numero_monstros, vidas_extra, numero_vidas_extra, espada);

    // Restaurar estado do jogador
    jogador->score = score;
    jogador->vidas = vidas;
    jogador->tem_espada = false;
}


void Salva_ranking(Score_t ranking[]) {
    FILE *file = fopen("ranking.bin", "wb");
    if (!file) {return;};

    fwrite(ranking, sizeof(Score_t), 5, file);
    fclose(file);
}

void Teleportar_entidade(int *px, int *py, char mapa[LINHAS][COLUNAS]) {
    int nx, ny;
    do {
        nx = rand() % COLUNAS;
        ny = rand() % LINHAS;
    } while (!Posicao_valida(nx, ny, mapa));
    *px = nx;
    *py = ny;
}

void Usar_espada(Jogador_t *jogador, Monstro_t monstros[], int numero_monstros) {
    int dx = 0, dy = 0;

    // Determinar vetor de direção
    switch (jogador->direcao) {
        case NORTE: dy = -1; break;
        case SUL:   dy = +1; break;
        case LESTE: dx = +1; break;
        case OESTE: dx = -1; break;
        default: return; // direção inválida
    }

    for (int i = 0; i < numero_monstros; i++) {
        Monstro_t *m = &monstros[i];
        if (m->vivo){

        int dist_x = abs(m->x - jogador->x);
        int dist_y = abs(m->y - jogador->y);

        bool alinhado_e_dentro_do_alcance = false;

        if (dx != 0) { // Ataque na horizontal
            alinhado_e_dentro_do_alcance = (m->y == jogador->y && dist_x <= ALCANCE_ESPADA);
        } else if (dy != 0) { // Ataque na vertical
            alinhado_e_dentro_do_alcance = (m->x == jogador->x && dist_y <= ALCANCE_ESPADA);
        }

        if (alinhado_e_dentro_do_alcance) {
            jogador->score += m->pontos;
            m->vivo = false;
        }}
    }
}
