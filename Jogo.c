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
#define MAX_NOME 30

// Estruturas de dados
typedef enum { NORTE, SUL, LESTE, OESTE } Direcao_t;

typedef struct
{
    int x, y;
    Direcao_t direcao;
    int vidas;
    int score;
    bool tem_espada;
    bool ativa_espada;
    float cooldown_espada;
} Jogador_t;

typedef struct
{
    int x, y;
    Direcao_t direcao;
    int pontos;
    bool vivo;
    float cooldown_mover;
} Monstro_t;

typedef struct
{
    int x, y;
    bool coletado;
} Vida_Extra_t;

typedef struct
{
    int x, y;
    bool coletado;
} Espada_t;

typedef struct
{
    char nome[MAX_NOME];
    int score;
} TIPO_SCORE;

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

void Atualiza_ranking(TIPO_SCORE nome_jogador, TIPO_SCORE ranking[]);

bool Carrega_mapa(int level);

void Comeca_jogo(Estado_jogo_t *estado, int *niveis_atual, Jogador_t *jogador,
                 Monstro_t monstros[], int *numero_monstros,
                 Vida_Extra_t vidas_extra[], int *numero_vidas_extra,
                 Espada_t *espada);

void Checar_colisoes(Estado_jogo_t *estado, Jogador_t *jogador,
                     Monstro_t monstros[], int numero_monstros,
                     Vida_Extra_t vidas_extra[], int numero_vidas_extra,
                     Espada_t *espada);

bool Carrega_ranking(TIPO_SCORE ranking[]);

void Carrega_texturas();

void Descarrega_texturas();

void Desenha_jogo( Jogador_t *jogador, Monstro_t monstros[], int numero_monstros,
                   Vida_Extra_t vidas_extra[], int numero_vidas_extra,
                   Espada_t *espada, int nivel_atual);

void Desenha_menu(int *selecao_menu);

void Desenha_scoreboard(TIPO_SCORE ranking[]);

void Desenha_menu_pausa(int *selecao_pause);

void Desenha_menu_derrota(int *selecao_derrota);

void Desenha_menu_vitoria(int *selecao_vitoria);

void Desenha_barra_status(Jogador_t *jogador, int level_atual);

void Movimento_jogador(Jogador_t *jogador, int tecla);

void Movimento_monstros(Monstro_t monstros[], int numero_monstros);

void Nome_do_jogador(char nome[]);

bool Posicao_valida(int x, int y, char mapa[LINHAS][COLUNAS]);

void Reseta_nivel(int level, Jogador_t *jogador, Monstro_t monstros[], int *numero_monstros,
                  Vida_Extra_t vidas_extra[], int *numero_vidas_extra, Espada_t *espada);

void Salva_ranking(TIPO_SCORE ranking[]);

void Teleportar_entidade(int *px, int *py, char mapa[LINHAS][COLUNAS]);

void Usar_espada(Jogador_t *jogador, Monstro_t monstros[], int numero_monstros);


int main(void)
{
    // Inicialização da janela
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Zelda INF");
    SetTargetFPS(60);

    // Carregar recursos
    Carrega_texturas();
    fonte_jogo = LoadFont("conteudo/fonte/alagard.png");

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
    int numero_monstros = 0, numero_vidas_extra = 0;

    // Ranking
    TIPO_SCORE ranking[MAX_RANKING];
    TIPO_SCORE entrada_score;

    if (!Carrega_ranking(ranking))
    {
        // Inicializar ranking padrão se arquivo não existir
        for (int i = 0; i < MAX_RANKING; i++)
        {
            snprintf(ranking[i].nome, MAX_NOME, "Jogador %d", i+1);
            ranking[i].score = 1000 - i * 200;
        }
    }
    bool ranking_atualizado = false;

    // Loop principal do jogo
    while (!WindowShouldClose())
    {
        switch (estado)
        {
        case MENU:
            // Navegação no menu
            if (IsKeyPressed(KEY_DOWN))
            {
                selecao_menu = (selecao_menu + 1) % 3;
            }
            if (IsKeyPressed(KEY_UP))
            {
                selecao_menu = (selecao_menu + 2) % 3;
            }

            if (IsKeyPressed(KEY_ENTER))
            {
                switch (selecao_menu)
                {
                case 0: // Novo jogo
                    level_atual = 1;
                    Comeca_jogo(&estado, &level_atual, &jogador, monstros, &numero_monstros, vidas_extra, &numero_vidas_extra, &espada);
                    estado = JOGANDO;
                    ranking_atualizado = false;
                    break;

                case 1: // scoreborad
                    estado = SCOREBOARD;
                    break;

                case 2: // sair
                    CloseWindow();
                    return 0;
                }
            }
            break;

        case JOGANDO:
            Atualiza_jogo(&estado, &level_atual, &jogador, monstros, &numero_monstros, vidas_extra, &numero_vidas_extra, &espada);
            break;

        case PAUSA:
            // Navegação no menu de pausa
            if (IsKeyPressed(KEY_DOWN))
            {
                selecao_pause = (selecao_pause + 1) % 3;
            }
            if (IsKeyPressed(KEY_UP))
            {
                selecao_pause = (selecao_pause + 2) % 3;
            }

            if (IsKeyPressed(KEY_ENTER))
            {
                switch (selecao_pause)
                {
                case 0: // Continuar
                    estado = JOGANDO;
                    break;

                case 1:// Voltar ao Menu
                    estado = MENU;
                    break;

                case 2: // Sair
                    CloseWindow();
                    return 0;
                }
            }
            break;

        case DERROTA:
            if (!ranking_atualizado)
            {
                Nome_do_jogador(entrada_score.nome);
                entrada_score.score = jogador.score;
                Atualiza_ranking(entrada_score, ranking);
                Salva_ranking(ranking);
                ranking_atualizado = true;
            }
            // Navegação no menu de game over
            if (IsKeyPressed(KEY_DOWN))
            {
                selecao_derrota = (selecao_derrota + 1) % 2;
            }
            if (IsKeyPressed(KEY_UP))
            {
                selecao_derrota = (selecao_derrota + 1) % 2;
            }

            if (IsKeyPressed(KEY_ENTER))
            {
                switch (selecao_derrota)
                {
                case 0:
                    level_atual = 1;
                    Comeca_jogo(&estado, &level_atual, &jogador, monstros, &numero_monstros, vidas_extra, &numero_vidas_extra, &espada);
                    estado = JOGANDO;
                    ranking_atualizado = false;
                    break;

                case 1: // Reiniciar jogo
                    estado = MENU;
                    break;
                }
            }
            break;

        case VITORIA:
            if (!ranking_atualizado)
            {
                Nome_do_jogador(entrada_score.nome);
                entrada_score.score = jogador.score;
                Atualiza_ranking(entrada_score, ranking);
                Salva_ranking(ranking);
                ranking_atualizado = true;
            }
            // Navegação na tela de vitória
            if (IsKeyPressed(KEY_DOWN))
            {
                selecao_vitoria = (selecao_vitoria + 1) % 2;
            }
            if (IsKeyPressed(KEY_UP))
            {
                selecao_vitoria = (selecao_vitoria + 1) % 2;
            }

            if (IsKeyPressed(KEY_ENTER))
            {
                switch (selecao_vitoria)
                {
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
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE))
            {
                estado = MENU;
            }
            break;
        }

        // Renderização
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (estado)
        {
        case MENU:
            Desenha_menu(&selecao_menu);
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
    CloseWindow();
    return 0;
}

void Atualiza_jogo(Estado_jogo_t *estado, int *level_atual, Jogador_t *jogador, Monstro_t monstros[], int *numero_monstros, Vida_Extra_t vidas_extra[], int *numero_vidas_extra, Espada_t *espada)
{
    // Pausar o jogo
    if (IsKeyPressed(KEY_TAB))
    {
        *estado = PAUSA;
        return;
    }

    // Movimento do jogador
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))
    {
        Movimento_jogador(jogador, KEY_W);
    }
    else if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))
    {
        Movimento_jogador(jogador, KEY_S);
    }
    else if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT))
    {
        Movimento_jogador(jogador, KEY_A);
    }
    else if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT))
    {
        Movimento_jogador(jogador, KEY_D);
    }

    // Ativar espada
    if (IsKeyPressed(KEY_J) && jogador->tem_espada && !jogador->ativa_espada)
    {
        jogador->ativa_espada = true;
        jogador->cooldown_espada = 0.2f; // 0.2 segundos de duração
    }

    // Atualizar temporizador da espada
    if (jogador->ativa_espada)
    {
        jogador->cooldown_espada -= GetFrameTime();
        if (jogador->cooldown_espada <= 0)
        {
            jogador->ativa_espada = false;
        }
        else
        {
            // Verificar se a espada atingiu monstros
            Usar_espada(jogador, monstros, *numero_monstros);
        }
    }

    // Movimento dos monstros
    Movimento_monstros(monstros, *numero_monstros);

    // Verificar colisões
    Checar_colisoes(estado, jogador, monstros, *numero_monstros, vidas_extra, *numero_vidas_extra, espada);

    // Verificar vitória na fase
    bool monstros_morreram = true;
    for (int i = 0; i < *numero_monstros; i++)
    {
        if (monstros[i].vivo)
        {
            monstros_morreram = false;
        }
    }

    if (monstros_morreram)
    {
        (*level_atual)++;
        if (*level_atual > MAX_NIVEIS)
        {
            *estado = VITORIA;
        }
        else
        {
            Reseta_nivel(*level_atual, jogador, monstros, numero_monstros, vidas_extra, numero_vidas_extra, espada);
        }
    }
}

void Atualiza_ranking(TIPO_SCORE novo, TIPO_SCORE ranking[])
{
    int pos = MAX_RANKING;
    int i = 0;
    // Encontra a primeira posição onde novo.pontos seja maior
    while (i < MAX_RANKING && novo.score <= ranking[i].score)
    {
        i++;
    }
    if (i < MAX_RANKING)
    {
        pos = i;
    }
    // Se entrou no top, move demais e insere
    if (pos < MAX_RANKING)
    {
        for (int j = MAX_RANKING - 1; j > pos; j--)
        {
            ranking[j] = ranking[j - 1];
        }
        ranking[pos] = novo;
    }
}

bool Carrega_ranking(TIPO_SCORE ranking[])
{
    FILE *file = fopen("conteudo/bin/ranking.bin", "rb");
    if (!file)
    {
        return false;
    }

    fread(ranking, sizeof(TIPO_SCORE), MAX_RANKING, file);
    fclose(file);
    return true;
}

bool Carrega_mapa(int level)
{
    char mapas[64];
    char temporario[COLUNAS + 2];
    // gera “mapa01.txt”, “mapa02.txt”...
    snprintf(mapas, sizeof(mapas), "conteudo/mapas/mapa%02d.txt", level);

    FILE *f = fopen(mapas, "r");
    if (!f)
    {
        // não encontrou o arquivo - vitoria
        return false;
    }
    // +1 para '\n' e +1 para '\0'
    for (int i = 0; i < LINHAS; i++)
    {
        if (fgets(temporario, sizeof temporario, f))
        {
            // copia exatamente COLUNAS caracteres (ignora '\n' se houver)
            for (int j = 0; j < COLUNAS && temporario[j] != '\n' && temporario[j] != '\r'; j++)
            {
                mapa[i][j] = temporario[j];
            }
        }
    }

    fclose(f);
    return true;

}

void Carrega_texturas()
{
    // Carregar texturas do jogador para cada direção
    textura_jogador[NORTE] = LoadTexture("conteudo/texturas/jogador-norte.png");
    textura_jogador[SUL]   = LoadTexture("conteudo/texturas/jogador-sul.png");
    textura_jogador[LESTE] = LoadTexture("conteudo/texturas/jogador-leste.png");
    textura_jogador[OESTE] = LoadTexture("conteudo/texturas/jogador-oeste.png");

    // Carregar texturas dos monstros para cada direção
    textura_monstros[NORTE] = LoadTexture("conteudo/texturas/monstro-norte.png");
    textura_monstros[SUL]   = LoadTexture("conteudo/texturas/monstro-sul.png");
    textura_monstros[LESTE] = LoadTexture("conteudo/texturas/monstro-leste.png");
    textura_monstros[OESTE] = LoadTexture("conteudo/texturas/monstro-oeste.png");

    // Carregar texturas de outros elementos
    textura_menu   = LoadTexture("conteudo/texturas/menu.png");
    textura_parede = LoadTexture("conteudo/texturas/parede.png");
    textura_espada = LoadTexture("conteudo/texturas/espada.png");
    textura_vida   = LoadTexture("conteudo/texturas/vida.png");
    textura_chao   = LoadTexture("conteudo/texturas/chao.png");
}

void Comeca_jogo(Estado_jogo_t *estado, int *level_atual, Jogador_t *jogador, Monstro_t monstros[], int *numero_monstros, Vida_Extra_t vidas_extra[], int *numero_vidas_extra, Espada_t *espada)
{
    // Carregar mapa
    if (!Carrega_mapa(*level_atual))
    {
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

    for (int i = 0; i < LINHAS; i++)
    {
        for (int j = 0; j < COLUNAS; j++)
        {
            switch (mapa[i][j])
            {
            case 'J': // Jogador
                jogador->x = j;
                jogador->y = i;
                break;

            case 'M': // Monstro
                if (*numero_monstros < MAX_MONSTROS)
                {
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
                if (*numero_vidas_extra < MAX_VIDA_EXTRA)
                {
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




void Checar_colisoes(Estado_jogo_t *estado, Jogador_t *jogador, Monstro_t monstros[], int numero_monstros, Vida_Extra_t vidas_extra[], int numero_vidas_extra, Espada_t *espada)
{
    // Verificar colisão com monstros
    for (int i = 0; i < numero_monstros; i++)
    {
        if (monstros[i].vivo && monstros[i].x == jogador->x && monstros[i].y == jogador->y)
        {
            jogador->vidas--;
            if (jogador->vidas <= 0)
            {
                *estado = DERROTA;
            }
            else
            {
                // Reposicionar monstro após contato
                Teleportar_entidade(&monstros[i].x, &monstros[i].y, mapa);
            }
        }
    }

    // Verificar colisão com espada
    if (!espada->coletado && !jogador->tem_espada && espada->x == jogador->x && espada->y == jogador->y)
    {
        jogador->tem_espada = true;
        espada->coletado = true;
    }

    // Verificar colisão com vidas extras
    for (int i = 0; i < numero_vidas_extra; i++)
    {
        if (!vidas_extra[i].coletado && vidas_extra[i].x == jogador->x && vidas_extra[i].y == jogador->y && jogador->vidas < MAX_VIDA_EXTRA)
        {
            jogador->vidas++;
            vidas_extra[i].coletado = true;
        }
    }
}

void Desenha_jogo(Jogador_t *jogador, Monstro_t monstros[], int numero_monstros, Vida_Extra_t vidas_extra[], int numero_vidas_extra, Espada_t *espada, int level_atual)
{
    float escala_chao     = (float)CELULA / textura_chao.width;
    float escala_parede   = (float)CELULA / textura_parede.width;
    float escala_monstro  = (float)CELULA / textura_monstros[0].width;
    float escala_espada   = (float)CELULA / textura_espada.width;
    float escala_vida     = (float)CELULA / textura_vida.width;

    // Desenhar fundo (chão e paredes)
    for (int i = 0; i < LINHAS; i++)
    {
        for (int j = 0; j < COLUNAS; j++)
        {
            // Desenhar chão
            DrawTextureEx(textura_chao, (Vector2)
            {
                j * CELULA, i * CELULA + BARRA_STATUS
            }, 0.0f, escala_chao, WHITE);
            // Desenhar parede, se existir
            if (mapa[i][j] == 'P')
            {
                DrawTextureEx(textura_parede, (Vector2)
                {
                    j * CELULA, i * CELULA + BARRA_STATUS
                }, 0.0f, escala_parede, WHITE);
            }
        }
    }

    // Desenhar itens: espada e vidas extra espalhadas no mapa
    for (int i = 0; i < numero_vidas_extra; i++)
    {
        if (!vidas_extra[i].coletado)
        {
            DrawTextureEx(textura_vida, (Vector2)
            {
                vidas_extra[i].x * CELULA, vidas_extra[i].y * CELULA + BARRA_STATUS
            }, 0.0f, escala_vida, WHITE);
        }
    }

    if (!espada->coletado)
    {
        DrawTextureEx(textura_espada, (Vector2)
        {
            espada->x * CELULA, espada->y * CELULA + BARRA_STATUS
        }, 0.0f, escala_espada, WHITE);
    }

    // Desenhar monstros
    for (int i = 0; i < numero_monstros; i++)
    {
        if (monstros[i].vivo)
        {
            DrawTextureEx(textura_monstros[monstros[i].direcao], (Vector2)
            {
                monstros[i].x * CELULA, monstros[i].y * CELULA + BARRA_STATUS
            }, 0.0f, escala_monstro, WHITE);
        }
    }

    //Desenhar jogador com DrawTexturePro para garantir mesma largura e altura
    Texture2D tex_jogador = textura_jogador[jogador->direcao];
    Rectangle fonte     = { 0.0f, 0.0f, (float)tex_jogador.width, (float)tex_jogador.height };
    Rectangle destino   = { jogador->x * CELULA, jogador->y * CELULA + BARRA_STATUS, (float)CELULA, (float)CELULA };
    Vector2 origem      = { 0.0f, 0.0f };
    DrawTexturePro(tex_jogador, fonte, destino, origem, 0.0f, WHITE);

    // Desenhar área de ataque da espada
    if (jogador->ativa_espada)
    {
        int dx = 0, dy = 0;
        switch (jogador->direcao)
        {
        case NORTE:
            dy = -1;
            break;
        case SUL:
            dy = +1;
            break;
        case LESTE:
            dx = +1;
            break;
        case OESTE:
            dx = -1;
            break;
        }
        for (int i = 1;
                i <= ALCANCE_ESPADA
                && jogador->x + dx*i >= 0
                && jogador->x + dx*i < COLUNAS
                && jogador->y + dy*i >= 0
                && jogador->y + dy*i < LINHAS;
                i++)
        {
            int nx = jogador->x + dx * i;
            int ny = jogador->y + dy * i;
            DrawRectangle(
                nx * CELULA,
                ny * CELULA + BARRA_STATUS,
                CELULA, CELULA,
                (Color)
            {
                255, 0, 0, 128
            });
        }
    }
    // Desenhar barra de status
    Desenha_barra_status(jogador, level_atual);
}

void Desenha_barra_status(Jogador_t *jogador, int level_atual)
{
    float escala_vida = (float)CELULA / textura_vida.width;
    // Fundo da barra de status
    DrawRectangle(0, 0, LARGURA_TELA, BARRA_STATUS, BLACK);

    // Textos
    DrawTextEx(fonte_jogo, TextFormat("Pontos: %d", jogador->score), (Vector2)
    {
        20, 20
    }, 20, 2, YELLOW);

    DrawTextEx(fonte_jogo, TextFormat("Nivel: %d", level_atual), (Vector2)
    {
        LARGURA_TELA/2 - 50, 20
    }, 20, 2, WHITE);

    // Vidas (representadas por corações)
    for (int i = 0; i < jogador->vidas; i++)
    {
        DrawTextureEx(textura_vida, (Vector2)
        {
            LARGURA_TELA - 100 - i * 20, 10
        }, 0.0f, escala_vida, WHITE);
    }

    // Indicador de espada
    if (jogador->tem_espada)
    {
        DrawTextEx(fonte_jogo, "ESPADA", (Vector2)
        {
            LARGURA_TELA - 300, 20
        }, 20, 2, GREEN);
    }
}

// Implementações das funções de menu (simplificadas)
void Desenha_menu(int *selecao_menu)
{
    Rectangle origem = { 0.0f, 0.0f, (float)textura_menu.width, (float)textura_menu.height };
    Rectangle destino = { 0.0f, 0.0f, 1200, 860 }; // tamanho desejado
    Vector2 origem_desenho = { 0.0f, 0.0f };

    DrawTexturePro(textura_menu, origem, destino, origem_desenho, 0.0f, WHITE);

    // Fundo translúcido por cima para contraste
    DrawRectangle(0, 0, 1200, 860, (Color)
    {
        0, 0, 0, 150
    });

    // Título
    DrawTextEx(fonte_jogo, "ZELDA INF", (Vector2)
    {
        LARGURA_TELA/2 - 100, 100
    }, 50, 2, GOLD);

    // Opções
    const char *opcoes[] = {"Novo Jogo", "Scoreboard", "Sair"};
    for (int i = 0; i < 3; i++)
    {
        Color cor = (i == *selecao_menu) ? YELLOW : WHITE;
        DrawTextEx(fonte_jogo, opcoes[i], (Vector2)
        {
            LARGURA_TELA/2 - 80, 200 + i * 60
        }, 30, 2, cor);
    }
}

void Desenha_menu_pausa(int *selecao_pause)
{
    // Sobrepor semi-transparente
    DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, (Color)
    {
        0, 0, 0, 180
    });

    // Título
    DrawTextEx(fonte_jogo, "JOGO PAUSADO", (Vector2)
    {
        LARGURA_TELA/2 - 120, 150
    }, 40, 2, WHITE);

    // Opções
    const char *opcoes[] = {"Continuar", "Voltar ao Menu", "Sair"};
    for (int i = 0; i < 3; i++)
    {
        Color cor = (i == *selecao_pause) ? YELLOW : LIGHTGRAY;
        DrawTextEx(fonte_jogo, opcoes[i], (Vector2)
        {
            LARGURA_TELA/2 - 80, 250 + i * 60
        }, 30, 2, cor);
    }
}


void Descarrega_texturas()
{
    for (int i = 0; i < 4; i++)
    {
        UnloadTexture(textura_jogador[i]);
    }
    for (int i = 0; i < 4; i++)
    {
        UnloadTexture(textura_monstros[i]);
    }
    UnloadTexture(textura_parede);
    UnloadTexture(textura_espada);
    UnloadTexture(textura_vida);
    UnloadTexture(textura_chao);
    UnloadFont(fonte_jogo);
}

// Implementações restantes de menus (simplificadas)
void Desenha_scoreboard(TIPO_SCORE ranking[])
{
    DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, (Color)
    {
        30, 30, 50, 255
    });
    DrawTextEx(fonte_jogo, "TOP 5 PONTUACOES", (Vector2)
    {
        LARGURA_TELA/2 - 150, 50
    }, 36, 2, GOLD);

    for (int i = 0; i < 5; i++)
    {
        DrawTextEx(fonte_jogo, TextFormat("%d. %s: %d", i+1, ranking[i].nome, ranking[i].score), (Vector2)
        {
            LARGURA_TELA/2 - 150, 150 + i * 60
        }, 30, 2, WHITE);
    }

    DrawTextEx(fonte_jogo, "Pressione ENTER para voltar", (Vector2)
    {
        LARGURA_TELA/2 - 180, ALTURA_TELA - 50
    }, 20, 2, LIGHTGRAY);
}

void Desenha_menu_derrota(int *selecao_derrota)
{
    DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, (Color)
    {
        50, 0, 0, 200
    });
    DrawTextEx(fonte_jogo, "GAME OVER", (Vector2)
    {
        LARGURA_TELA/2 - 120, 150
    }, 48, 2, RED);

    const char *opcoes[] = {"Reiniciar Jogo", "Voltar ao Menu"};
    for (int i = 0; i < 2; i++)
    {
        Color cor = (i == *selecao_derrota) ? YELLOW : LIGHTGRAY;
        DrawTextEx(fonte_jogo, opcoes[i], (Vector2)
        {
            LARGURA_TELA/2 - 100, 250 + i * 60
        }, 30, 2, cor);
    }
}

void Desenha_menu_vitoria(int *selecao_vitoria)
{
    DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, (Color)
    {
        0, 50, 0, 200
    });
    DrawTextEx(fonte_jogo, "VITORIA!", (Vector2)
    {
        LARGURA_TELA/2 - 100, 150
    }, 48, 2, GREEN);
    DrawTextEx(fonte_jogo, "Parabens! Voce completou todos os niveis!", (Vector2)
    {
        LARGURA_TELA/2 - 250, 220
    }, 24, 2, LIGHTGRAY);

    const char *opcoes[] = {"Voltar ao Menu", "Sair"};
    for (int i = 0; i < 2; i++)
    {
        Color cor = (i == *selecao_vitoria) ? YELLOW : LIGHTGRAY;
        DrawTextEx(fonte_jogo, opcoes[i], (Vector2)
        {
            LARGURA_TELA/2 - 80, 300 + i * 60
        }, 30, 2, cor);
    }
}

void Movimento_jogador(Jogador_t *jogador, int tecla)
{
    int novo_x = jogador->x;
    int novo_y = jogador->y;
    Direcao_t nova_direcao = jogador->direcao;

    switch (tecla)
    {
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
    if (novo_x >= 0 && novo_x < COLUNAS && novo_y >= 0 && novo_y < LINHAS)
    {
        if (mapa[novo_y][novo_x] != 'P')
        {
            jogador->x = novo_x;
            jogador->y = novo_y;
            jogador->direcao = nova_direcao;
        }
    }
}


void Movimento_monstros(Monstro_t monstros[], int numero_monstros)
{
    for (int i = 0; i < numero_monstros; i++)
    {
        if (monstros[i].vivo)
        {

            // Atualizar temporizador de movimento
            monstros[i].cooldown_mover -= GetFrameTime();
            if (monstros[i].cooldown_mover <= 0)
            {

                // Resetar temporizador (0.5-1.5 segundos)
                monstros[i].cooldown_mover = 0.5f + (float)rand() / (float)RAND_MAX;

                // Escolher direção aleatória
                int direcao = rand() % 4;
                int novo_x = monstros[i].x;
                int novo_y = monstros[i].y;

                switch (direcao)
                {
                case 0:
                    novo_y--;
                    break; // Norte

                case 1:
                    novo_y++;
                    break; // Sul

                case 2:
                    novo_x++;
                    break; // Leste

                case 3:
                    novo_x--;
                    break; // Oeste
                }

                // Verificar limites do mapa e colisão com paredes
                if (novo_x >= 0 && novo_x < COLUNAS && novo_y >= 0 && novo_y < LINHAS)
                {
                    if (mapa[novo_y][novo_x] != 'P')
                    {
                        monstros[i].x = novo_x;
                        monstros[i].y = novo_y;
                        monstros[i].direcao = direcao;
                    }
                }
            }
        }
    }
}

void Nome_do_jogador(char nome[])
{
    int tam = 0;
    bool concluido = false;
    nome[0] = '\0';

    while (!concluido && !WindowShouldClose())
    {
        // 1) Captura caracteres imprimíveis
        int c = GetCharPressed();
        if (c > 0 && tam < MAX_NOME-1)
        {
            nome[tam++] = (char)c;
            nome[tam] = '\0';
        }

        // 2) Backspace (remove um caractere)
        if ((IsKeyPressed(KEY_BACKSPACE) || IsKeyDown(KEY_BACKSPACE)) && tam > 0)
        {
            tam--;
            nome[tam] = '\0';
        }

        // 3) ENTER — finaliza somente se já tiver ao menos um caractere
        if (IsKeyPressed(KEY_ENTER) && tam > 0)
        {
            concluido = true;
        }

        // 4) Desenha a tela de input
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Digite seu nome:",
                 GetScreenWidth()/2 - 100, GetScreenHeight()/2 - 50, 20, BLACK);
        DrawRectangle(GetScreenWidth()/2 - 100, GetScreenHeight()/2 - 20,
                      200, 30, LIGHTGRAY);
        DrawText(nome,
                 GetScreenWidth()/2 - 95, GetScreenHeight()/2 - 15, 20, BLACK);
        EndDrawing();
    }
}

bool Posicao_valida(int x, int y, char mapa[LINHAS][COLUNAS])
{
    return mapa[y][x] != 'P';  // ou o caracter que você usa pra parede
}

void Reseta_nivel(int level, Jogador_t *jogador, Monstro_t monstros[], int *numero_monstros, Vida_Extra_t vidas_extra[], int *numero_vidas_extra, Espada_t *espada)
{

    // Manter pontuação e vidas do jogador
    int score = jogador->score;
    int vidas = jogador->vidas;


    // Recarregar o nível
    Comeca_jogo(&(Estado_jogo_t)
    {
        0
    }, &level, jogador, monstros, numero_monstros, vidas_extra, numero_vidas_extra, espada);

    // Restaurar estado do jogador
    jogador->score = score;
    jogador->vidas = vidas;
    jogador->tem_espada = false;
}


void Salva_ranking(TIPO_SCORE ranking[])
{
    FILE *file = fopen("conteudo/bin/ranking.bin", "wb");
    if (!file)
    {
        return;
    };

    fwrite(ranking, sizeof(TIPO_SCORE), MAX_RANKING, file);
    fclose(file);
}

void Teleportar_entidade(int *px, int *py, char mapa[LINHAS][COLUNAS])
{
    int nx, ny;
    do
    {
        nx = rand() % COLUNAS;
        ny = rand() % LINHAS;
    }
    while (!Posicao_valida(nx, ny, mapa));
    *px = nx;
    *py = ny;
}

void Usar_espada(Jogador_t *jogador, Monstro_t monstros[], int numero_monstros)
{
    int dx = 0, dy = 0;
    switch (jogador->direcao)
    {
    case NORTE:
        dy = -1;
        break;
    case SUL:
        dy = +1;
        break;
    case LESTE:
        dx = +1;
        break;
    case OESTE:
        dx = -1;
        break;
    }

    for (int i = 1; i <= ALCANCE_ESPADA && jogador->x + dx*i >= 0 && jogador->x + dx*i < COLUNAS && jogador->y + dy*i >= 0 && jogador->y + dy*i < LINHAS; i++)
    {
        int nx = jogador->x + dx * i;
        int ny = jogador->y + dy * i;

        for (int j = 0; j < numero_monstros; j++)
        {
            if (monstros[j].vivo && monstros[j].x == nx && monstros[j].y == ny)
            {
                monstros[j].vivo = false;
                jogador->score += monstros[j].pontos;
                return;  // sai ao acertar o monstro
            }
        }
    }
}
