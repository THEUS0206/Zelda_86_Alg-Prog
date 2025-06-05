#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

// Constantes do jogo
#define MAP_ROWS 16
#define MAP_COLS 24
#define BLOCK_SIZE 50
#define STATUS_BAR_HEIGHT 60
#define GAME_AREA_HEIGHT 800
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT (STATUS_BAR_HEIGHT + GAME_AREA_HEIGHT)
#define MAX_MONSTERS 10
#define MAX_EXTRA_LIVES 5
#define MAX_LEVELS 99
#define SWORD_RANGE 3

// Estruturas de dados
typedef enum { NORTH, SOUTH, EAST, WEST, NONE } Direction;

typedef struct {
    int x, y;
    Direction dir;
    int score;
    int lives;
    bool hasSword;
    bool swordActive;
    float swordActiveTime;
} Player;

typedef struct {
    int x, y;
    Direction dir;
    int points;
    bool alive;
    float moveTimer;
} Monster;

typedef struct {
    int x, y;
    bool collected;
} ExtraLife;

typedef struct {
    int x, y;
    bool collected;
} Sword;

typedef struct {
    char nome[20];
    int score;
} TIPO_SCORE;

typedef enum { MENU, PLAYING, PAUSED, GAMEOVER, VICTORY, SCOREBOARD } GameState;

// Variáveis globais permitidas
char gameMap[MAP_ROWS][MAP_COLS];
Texture2D playerTextures[4];  // N, S, E, W
Texture2D monsterTexture;
Texture2D wallTexture;
Texture2D swordTexture;
Texture2D heartTexture;
Texture2D groundTexture;
Font gameFont;

// Protótipos de funções
void LoadMap(int level);
void InitGame(GameState *state, int *currentLevel, Player *player, 
              Monster monsters[], int *numMonsters, 
              ExtraLife extraLives[], int *numExtraLives, 
              Sword *sword);
void UpdateGame(GameState *state, int *currentLevel, Player *player, 
                Monster monsters[], int *numMonsters, 
                ExtraLife extraLives[], int *numExtraLives, 
                Sword *sword);
void DrawGame(Player *player, Monster monsters[], int numMonsters, 
              ExtraLife extraLives[], int numExtraLives, 
              Sword *sword, int currentLevel);
void DrawMenu(GameState *state, int *menuSelection);
void DrawScoreboard(TIPO_SCORE ranking[]);
void DrawPauseMenu(int *pauseSelection);
void DrawGameOverMenu(int *gameOverSelection);
void DrawVictoryScreen(int *victorySelection);
void UpdateRanking(int score, TIPO_SCORE ranking[]);
void DrawStatusBar(Player *player, int currentLevel);
void MovePlayer(Player *player, int key);
void MoveMonsters(Monster monsters[], int numMonsters);
void CheckCollisions(GameState *state, Player *player, 
                     Monster monsters[], int numMonsters, 
                     ExtraLife extraLives[], int numExtraLives, 
                     Sword *sword);
void UseSword(Player *player, Monster monsters[], int numMonsters);
void LoadTextures();
void UnloadTextures();
bool LoadRanking(TIPO_SCORE ranking[]);
void SaveRanking(TIPO_SCORE ranking[]);
void ResetLevel(int level, Player *player, Monster monsters[], int *numMonsters, 
                ExtraLife extraLives[], int *numExtraLives, Sword *sword);

int main(void) {
    // Inicialização da janela
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Zelda INF");
    InitAudioDevice();
    SetTargetFPS(60);
    
    // Carregar recursos
    LoadTextures();
    gameFont = LoadFontEx("fonts/arial.ttf", 24, 0, 0);
    
    // Variáveis de estado do jogo
    GameState state = MENU;
    int currentLevel = 1;
    int menuSelection = 0;
    int pauseSelection = 0;
    int gameOverSelection = 0;
    int victorySelection = 0;
    
    // Entidades do jogo
    Player player;
    Monster monsters[MAX_MONSTERS];
    ExtraLife extraLives[MAX_EXTRA_LIVES];
    Sword sword;
    int numMonsters = 0;
    int numExtraLives = 0;
    
    // Ranking
    TIPO_SCORE ranking[5];
    if (!LoadRanking(ranking)) {
        // Inicializar ranking padrão se arquivo não existir
        for (int i = 0; i < 5; i++) {
            sprintf(ranking[i].nome, "Jogador %d", i+1);
            ranking[i].score = 1000 - i * 200;
        }
    }
    
    // Loop principal do jogo
    while (!WindowShouldClose()) {
        switch (state) {
            case MENU:
                // Navegação no menu
                if (IsKeyPressed(KEY_DOWN)) menuSelection = (menuSelection + 1) % 3;
                if (IsKeyPressed(KEY_UP)) menuSelection = (menuSelection + 2) % 3;
                
                if (IsKeyPressed(KEY_ENTER)) {
                    switch (menuSelection) {
                        case 0: // Novo jogo
                            currentLevel = 1;
                            InitGame(&state, &currentLevel, &player, monsters, &numMonsters, 
                                     extraLives, &numExtraLives, &sword);
                            state = PLAYING;
                            break;
                        case 1: // Scoreboard
                            state = SCOREBOARD;
                            break;
                        case 2: // Sair
                            CloseWindow();
                            return 0;
                    }
                }
                break;
                
            case PLAYING:
                UpdateGame(&state, &currentLevel, &player, monsters, &numMonsters, 
                           extraLives, &numExtraLives, &sword);
                break;
                
            case PAUSED:
                // Navegação no menu de pausa
                if (IsKeyPressed(KEY_DOWN)) pauseSelection = (pauseSelection + 1) % 3;
                if (IsKeyPressed(KEY_UP)) pauseSelection = (pauseSelection + 2) % 3;
                
                if (IsKeyPressed(KEY_ENTER)) {
                    switch (pauseSelection) {
                        case 0: // Continuar
                            state = PLAYING;
                            break;
                        case 1: // Voltar ao menu
                            state = MENU;
                            break;
                        case 2: // Sair
                            CloseWindow();
                            return 0;
                    }
                }
                break;
                
            case GAMEOVER:
                // Navegação no menu de game over
                if (IsKeyPressed(KEY_DOWN)) gameOverSelection = (gameOverSelection + 1) % 3;
                if (IsKeyPressed(KEY_UP)) gameOverSelection = (gameOverSelection + 2) % 3;
                
                if (IsKeyPressed(KEY_ENTER)) {
                    switch (gameOverSelection) {
                        case 0: // Carregar jogo (não implementado)
                            // Implementar se for requisito extra
                            break;
                        case 1: // Reiniciar jogo
                            currentLevel = 1;
                            InitGame(&state, &currentLevel, &player, monsters, &numMonsters, 
                                     extraLives, &numExtraLives, &sword);
                            state = PLAYING;
                            break;
                        case 2: // Voltar ao menu
                            state = MENU;
                            break;
                    }
                }
                break;
                
            case VICTORY:
                // Navegação na tela de vitória
                if (IsKeyPressed(KEY_DOWN)) victorySelection = (victorySelection + 1) % 2;
                if (IsKeyPressed(KEY_UP)) victorySelection = (victorySelection + 1) % 2;
                
                if (IsKeyPressed(KEY_ENTER)) {
                    switch (victorySelection) {
                        case 0: // Voltar ao menu
                            state = MENU;
                            break;
                        case 1: // Sair
                            CloseWindow();
                            return 0;
                    }
                }
                break;
                
            case SCOREBOARD:
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
                    state = MENU;
                }
                break;
        }

        // Renderização
        BeginDrawing();
            ClearBackground(RAYWHITE);
            
            switch (state) {
                case MENU:
                    DrawMenu(&state, &menuSelection);
                    break;
                    
                case PLAYING:
                    DrawGame(&player, monsters, numMonsters, extraLives, numExtraLives, &sword, currentLevel);
                    break;
                    
                case PAUSED:
                    DrawGame(&player, monsters, numMonsters, extraLives, numExtraLives, &sword, currentLevel);
                    DrawPauseMenu(&pauseSelection);
                    break;
                    
                case GAMEOVER:
                    DrawGameOverMenu(&gameOverSelection);
                    break;
                    
                case VICTORY:
                    DrawVictoryScreen(&victorySelection);
                    break;
                    
                case SCOREBOARD:
                    DrawScoreboard(ranking);
                    break;
            }
        EndDrawing();
    }

    // Salvar ranking e liberar recursos
    SaveRanking(ranking);
    UnloadTextures();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

void LoadMap(int level) {
    char filename[20];
    sprintf(filename, "mapa%02d.txt", level);
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        TraceLog(LOG_ERROR, "Falha ao carregar o mapa: %s", filename);
        return;
    }

    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            int c = fgetc(file);
            if (c == EOF) break;
            gameMap[i][j] = (char)c;
        }
        // Ignorar quebra de linha
        fgetc(file);
    }
    fclose(file);
}

void InitGame(GameState *state, int *currentLevel, Player *player, 
              Monster monsters[], int *numMonsters, 
              ExtraLife extraLives[], int *numExtraLives, 
              Sword *sword) {
    // Carregar mapa
    LoadMap(*currentLevel);
    
    // Inicializar jogador
    player->x = 0;
    player->y = 0;
    player->dir = SOUTH;
    player->score = 0;
    player->lives = 3;
    player->hasSword = false;
    player->swordActive = false;
    player->swordActiveTime = 0.0f;
    
    // Encontrar posições iniciais
    *numMonsters = 0;
    *numExtraLives = 0;
    sword->collected = false;
    
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            switch (gameMap[i][j]) {
                case 'J': // Jogador
                    player->x = j;
                    player->y = i;
                    break;
                    
                case 'M': // Monstro
                    if (*numMonsters < MAX_MONSTERS) {
                        monsters[*numMonsters].x = j;
                        monsters[*numMonsters].y = i;
                        monsters[*numMonsters].dir = rand() % 4;
                        monsters[*numMonsters].points = rand() % 101;
                        monsters[*numMonsters].alive = true;
                        monsters[*numMonsters].moveTimer = 0.0f;
                        (*numMonsters)++;
                    }
                    break;
                    
                case 'E': // Espada
                    sword->x = j;
                    sword->y = i;
                    break;
                    
                case 'V': // Vida extra
                    if (*numExtraLives < MAX_EXTRA_LIVES) {
                        extraLives[*numExtraLives].x = j;
                        extraLives[*numExtraLives].y = i;
                        extraLives[*numExtraLives].collected = false;
                        (*numExtraLives)++;
                    }
                    break;
            }
        }
    }
    
    *state = PLAYING;
}

void UpdateGame(GameState *state, int *currentLevel, Player *player, 
                Monster monsters[], int *numMonsters, 
                ExtraLife extraLives[], int *numExtraLives, 
                Sword *sword) {
    // Pausar o jogo
    if (IsKeyPressed(KEY_TAB)) {
        *state = PAUSED;
        return;
    }
    
    // Movimento do jogador
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) MovePlayer(player, KEY_W);
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) MovePlayer(player, KEY_S);
    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) MovePlayer(player, KEY_A);
    if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) MovePlayer(player, KEY_D);
    
    // Ativar espada
    if (IsKeyPressed(KEY_J) && player->hasSword && !player->swordActive) {
        player->swordActive = true;
        player->swordActiveTime = 0.2f; // 0.2 segundos de duração
    }
    
    // Atualizar temporizador da espada
    if (player->swordActive) {
        player->swordActiveTime -= GetFrameTime();
        if (player->swordActiveTime <= 0) {
            player->swordActive = false;
        } else {
            // Verificar se a espada atingiu monstros
            UseSword(player, monsters, *numMonsters);
        }
    }
    
    // Movimento dos monstros
    MoveMonsters(monsters, *numMonsters);
    
    // Verificar colisões
    CheckCollisions(state, player, monsters, *numMonsters, 
                   extraLives, *numExtraLives, sword);
    
    // Verificar vitória na fase
    bool allMonstersDead = true;
    for (int i = 0; i < *numMonsters; i++) {
        if (monsters[i].alive) {
            allMonstersDead = false;
            break;
        }
    }
    
    if (allMonstersDead) {
        (*currentLevel)++;
        if (*currentLevel > MAX_LEVELS) {
            *state = VICTORY;
        } else {
            ResetLevel(*currentLevel, player, monsters, numMonsters, 
                       extraLives, numExtraLives, sword);
        }
    }
}

void MovePlayer(Player *player, int key) {
    int newX = player->x;
    int newY = player->y;
    Direction newDir = player->dir;

    switch (key) {
        case KEY_W:
        case KEY_UP:
            newY--;
            newDir = NORTH;
            break;
        case KEY_S:
        case KEY_DOWN:
            newY++;
            newDir = SOUTH;
            break;
        case KEY_A:
        case KEY_LEFT:
            newX--;
            newDir = WEST;
            break;
        case KEY_D:
        case KEY_RIGHT:
            newX++;
            newDir = EAST;
            break;
        default:
            return;
    }

    // Verificar limites do mapa e colisão com paredes
    if (newX >= 0 && newX < MAP_COLS && newY >= 0 && newY < MAP_ROWS) {
        if (gameMap[newY][newX] != 'P') {
            player->x = newX;
            player->y = newY;
            player->dir = newDir;
        }
    }
}

void MoveMonsters(Monster monsters[], int numMonsters) {
    for (int i = 0; i < numMonsters; i++) {
        if (!monsters[i].alive) continue;
        
        // Atualizar temporizador de movimento
        monsters[i].moveTimer -= GetFrameTime();
        if (monsters[i].moveTimer > 0) continue;
        
        // Resetar temporizador (0.5-1.5 segundos)
        monsters[i].moveTimer = 0.5f + (float)rand() / (float)RAND_MAX;
        
        // Escolher direção aleatória
        int direction = rand() % 4;
        int newX = monsters[i].x;
        int newY = monsters[i].y;
        
        switch (direction) {
            case 0: newY--; break; // Norte
            case 1: newY++; break; // Sul
            case 2: newX++; break; // Leste
            case 3: newX--; break; // Oeste
        }
        
        // Verificar limites do mapa e colisão com paredes
        if (newX >= 0 && newX < MAP_COLS && newY >= 0 && newY < MAP_ROWS) {
            if (gameMap[newY][newX] != 'P') {
                monsters[i].x = newX;
                monsters[i].y = newY;
                monsters[i].dir = direction;
            }
        }
    }
}

void CheckCollisions(GameState *state, Player *player, 
                     Monster monsters[], int numMonsters, 
                     ExtraLife extraLives[], int numExtraLives, 
                     Sword *sword) {
    // Verificar colisão com monstros
    for (int i = 0; i < numMonsters; i++) {
        if (monsters[i].alive && monsters[i].x == player->x && monsters[i].y == player->y) {
            player->lives--;
            if (player->lives <= 0) {
                *state = GAMEOVER;
            }
            // Reposicionar monstro após contato
            monsters[i].x = rand() % MAP_COLS;
            monsters[i].y = rand() % MAP_ROWS;
        }
    }
    
    // Verificar colisão com espada
    if (!sword->collected && !player->hasSword && 
        sword->x == player->x && sword->y == player->y) {
        player->hasSword = true;
        sword->collected = true;
    }
    
    // Verificar colisão com vidas extras
    for (int i = 0; i < numExtraLives; i++) {
        if (!extraLives[i].collected && 
            extraLives[i].x == player->x && extraLives[i].y == player->y) {
            player->lives++;
            extraLives[i].collected = true;
        }
    }
}

void UseSword(Player *player, Monster monsters[], int numMonsters) {
    int startX = player->x;
    int startY = player->y;
    int endX = startX;
    int endY = startY;
    
    // Determinar direção da espada
    switch (player->dir) {
        case NORTH: endY -= SWORD_RANGE; break;
        case SOUTH: endY += SWORD_RANGE; break;
        case EAST: endX += SWORD_RANGE; break;
        case WEST: endX -= SWORD_RANGE; break;
        default: break;
    }
    
    // Garantir que as coordenadas estão dentro dos limites
    int minX = (startX < endX) ? startX : endX;
    int maxX = (startX > endX) ? startX : endX;
    int minY = (startY < endY) ? startY : endY;
    int maxY = (startY > endY) ? startY : endY;
    
    // Verificar monstros na área de ataque
    for (int i = 0; i < numMonsters; i++) {
        if (monsters[i].alive && 
            monsters[i].x >= minX && monsters[i].x <= maxX &&
            monsters[i].y >= minY && monsters[i].y <= maxY) {
            
            // Verificar se está na linha de ataque
            bool inLine = false;
            switch (player->dir) {
                case NORTH:
                case SOUTH:
                    inLine = (monsters[i].x == startX);
                    break;
                case EAST:
                case WEST:
                    inLine = (monsters[i].y == startY);
                    break;
                default:
                    break;
            }
            
            if (inLine) {
                player->score += monsters[i].points;
                monsters[i].alive = false;
            }
        }
    }
}

void DrawGame(Player *player, Monster monsters[], int numMonsters, 
              ExtraLife extraLives[], int numExtraLives, 
              Sword *sword, int currentLevel) {
    // Desenhar fundo
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            // Desenhar chão
            DrawTexture(groundTexture, j * BLOCK_SIZE, i * BLOCK_SIZE + STATUS_BAR_HEIGHT, WHITE);
            
            // Desenhar paredes
            if (gameMap[i][j] == 'P') {
                DrawTexture(wallTexture, j * BLOCK_SIZE, i * BLOCK_SIZE + STATUS_BAR_HEIGHT, WHITE);
            }
        }
    }
    
    // Desenhar espada
    if (!sword->collected) {
        DrawTexture(swordTexture, sword->x * BLOCK_SIZE, sword->y * BLOCK_SIZE + STATUS_BAR_HEIGHT, WHITE);
    }
    
    // Desenhar vidas extras
    for (int i = 0; i < numExtraLives; i++) {
        if (!extraLives[i].collected) {
            DrawTexture(heartTexture, extraLives[i].x * BLOCK_SIZE, 
                        extraLives[i].y * BLOCK_SIZE + STATUS_BAR_HEIGHT, WHITE);
        }
    }
    
    // Desenhar monstros
    for (int i = 0; i < numMonsters; i++) {
        if (monsters[i].alive) {
            DrawTexture(monsterTexture, monsters[i].x * BLOCK_SIZE, 
                        monsters[i].y * BLOCK_SIZE + STATUS_BAR_HEIGHT, WHITE);
        }
    }
    
    // Desenhar jogador
    DrawTexture(playerTextures[player->dir], player->x * BLOCK_SIZE, 
                player->y * BLOCK_SIZE + STATUS_BAR_HEIGHT, WHITE);
    
    // Desenhar área de ataque da espada
    if (player->swordActive) {
        int startX = player->x;
        int startY = player->y;
        int endX = startX;
        int endY = startY;
        
        switch (player->dir) {
            case NORTH: endY -= SWORD_RANGE; break;
            case SOUTH: endY += SWORD_RANGE; break;
            case EAST: endX += SWORD_RANGE; break;
            case WEST: endX -= SWORD_RANGE; break;
            default: break;
        }
        
        // Garantir que as coordenadas estão dentro dos limites
        if (endY < 0) endY = 0;
        if (endY >= MAP_ROWS) endY = MAP_ROWS - 1;
        if (endX < 0) endX = 0;
        if (endX >= MAP_COLS) endX = MAP_COLS - 1;
        
        // Desenhar área de ataque
        for (int y = (startY < endY) ? startY : endY; y <= ((startY > endY) ? startY : endY); y++) {
            for (int x = (startX < endX) ? startX : endX; x <= ((startX > endX) ? startX : endX); x++) {
                if ((x == startX && y >= startY && y <= endY) || 
                    (y == startY && x >= startX && x <= endX)) {
                    DrawRectangle(x * BLOCK_SIZE, y * BLOCK_SIZE + STATUS_BAR_HEIGHT, 
                                 BLOCK_SIZE, BLOCK_SIZE, 
                                 (Color){255, 0, 0, 128}); // Vermelho semi-transparente
                }
            }
        }
    }
    
    // Desenhar barra de status
    DrawStatusBar(player, currentLevel);
}

void DrawStatusBar(Player *player, int currentLevel) {
    // Fundo da barra de status
    DrawRectangle(0, 0, SCREEN_WIDTH, STATUS_BAR_HEIGHT, DARKGRAY);
    
    // Textos
    DrawTextEx(gameFont, TextFormat("Pontos: %d", player->score), 
               (Vector2){20, 20}, 20, 2, YELLOW);
    
    DrawTextEx(gameFont, TextFormat("Nivel: %d", currentLevel), 
               (Vector2){SCREEN_WIDTH/2 - 50, 20}, 20, 2, WHITE);
    
    // Vidas (representadas por corações)
    for (int i = 0; i < player->lives; i++) {
        DrawTexture(heartTexture, SCREEN_WIDTH - 100 - i * 40, 10, WHITE);
    }
    
    // Indicador de espada
    if (player->hasSword) {
        DrawTextEx(gameFont, "ESPADA", (Vector2){SCREEN_WIDTH - 200, 20}, 20, 2, BLUE);
    }
}

// Implementações das funções de menu (simplificadas)
void DrawMenu(GameState *state, int *menuSelection) {
    // Desenhar fundo do menu
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 200});
    
    // Título
    DrawTextEx(gameFont, "ZELDA INF", (Vector2){SCREEN_WIDTH/2 - 100, 100}, 48, 2, GOLD);
    
    // Opções
    const char *options[] = {"Novo Jogo", "Scoreboard", "Sair"};
    for (int i = 0; i < 3; i++) {
        Color color = (i == *menuSelection) ? YELLOW : WHITE;
        DrawTextEx(gameFont, options[i], (Vector2){SCREEN_WIDTH/2 - 80, 200 + i * 60}, 30, 2, color);
    }
}

void DrawPauseMenu(int *pauseSelection) {
    // Sobrepor semi-transparente
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 180});
    
    // Título
    DrawTextEx(gameFont, "JOGO PAUSADO", (Vector2){SCREEN_WIDTH/2 - 120, 150}, 36, 2, WHITE);
    
    // Opções
    const char *options[] = {"Continuar", "Voltar ao Menu", "Sair"};
    for (int i = 0; i < 3; i++) {
        Color color = (i == *pauseSelection) ? YELLOW : LIGHTGRAY;
        DrawTextEx(gameFont, options[i], (Vector2){SCREEN_WIDTH/2 - 80, 250 + i * 60}, 30, 2, color);
    }
}

// Funções auxiliares para carregamento de recursos
void LoadTextures() {
    // Texturas de exemplo (substituir por arquivos reais)
    Image img = GenImageColor(BLOCK_SIZE, BLOCK_SIZE, BLUE);
    playerTextures[NORTH] = LoadTextureFromImage(img);
    
    img = GenImageColor(BLOCK_SIZE, BLOCK_SIZE, GREEN);
    playerTextures[SOUTH] = LoadTextureFromImage(img);
    
    img = GenImageColor(BLOCK_SIZE, BLOCK_SIZE, RED);
    playerTextures[EAST] = LoadTextureFromImage(img);
    
    img = GenImageColor(BLOCK_SIZE, BLOCK_SIZE, ORANGE);
    playerTextures[WEST] = LoadTextureFromImage(img);
    
    img = GenImageColor(BLOCK_SIZE, BLOCK_SIZE, PURPLE);
    monsterTexture = LoadTextureFromImage(img);
    
    img = GenImageColor(BLOCK_SIZE, BLOCK_SIZE, GRAY);
    wallTexture = LoadTextureFromImage(img);
    
    img = GenImageColor(BLOCK_SIZE, BLOCK_SIZE, YELLOW);
    swordTexture = LoadTextureFromImage(img);
    
    img = GenImageColor(BLOCK_SIZE, BLOCK_SIZE, PINK);
    heartTexture = LoadTextureFromImage(img);
    
    img = GenImageColor(BLOCK_SIZE, BLOCK_SIZE, (Color){50, 150, 50, 255});
    groundTexture = LoadTextureFromImage(img);
    
    UnloadImage(img);
}

void UnloadTextures() {
    for (int i = 0; i < 4; i++) {
        UnloadTexture(playerTextures[i]);
    }
    UnloadTexture(monsterTexture);
    UnloadTexture(wallTexture);
    UnloadTexture(swordTexture);
    UnloadTexture(heartTexture);
    UnloadTexture(groundTexture);
    UnloadFont(gameFont);
}

bool LoadRanking(TIPO_SCORE ranking[]) {
    FILE *file = fopen("ranking.bin", "rb");
    if (!file) return false;
    
    fread(ranking, sizeof(TIPO_SCORE), 5, file);
    fclose(file);
    return true;
}

void SaveRanking(TIPO_SCORE ranking[]) {
    FILE *file = fopen("ranking.bin", "wb");
    if (!file) return;
    
    fwrite(ranking, sizeof(TIPO_SCORE), 5, file);
    fclose(file);
}

void ResetLevel(int level, Player *player, Monster monsters[], int *numMonsters, 
               ExtraLife extraLives[], int *numExtraLives, Sword *sword) {
    // Manter pontuação e vidas do jogador
    int score = player->score;
    int lives = player->lives;
    bool hasSword = player->hasSword;
    
    // Recarregar o nível
    InitGame(&(GameState){0}, &level, player, monsters, numMonsters, 
             extraLives, numExtraLives, sword);
    
    // Restaurar estado do jogador
    player->score = score;
    player->lives = lives;
    player->hasSword = hasSword;
}

// Implementações restantes de menus (simplificadas)
void DrawScoreboard(TIPO_SCORE ranking[]) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){30, 30, 50, 255});
    DrawTextEx(gameFont, "TOP 5 PONTUACOES", (Vector2){SCREEN_WIDTH/2 - 150, 50}, 36, 2, GOLD);
    
    for (int i = 0; i < 5; i++) {
        DrawTextEx(gameFont, TextFormat("%d. %s: %d", i+1, ranking[i].nome, ranking[i].score), 
                  (Vector2){SCREEN_WIDTH/2 - 150, 150 + i * 60}, 30, 2, WHITE);
    }
    
    DrawTextEx(gameFont, "Pressione ENTER para voltar", 
              (Vector2){SCREEN_WIDTH/2 - 180, SCREEN_HEIGHT - 50}, 20, 2, LIGHTGRAY);
}

void DrawGameOverMenu(int *gameOverSelection) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){50, 0, 0, 200});
    DrawTextEx(gameFont, "GAME OVER", (Vector2){SCREEN_WIDTH/2 - 120, 150}, 48, 2, RED);
    
    const char *options[] = {"Carregar Jogo", "Reiniciar Jogo", "Voltar ao Menu"};
    for (int i = 0; i < 3; i++) {
        Color color = (i == *gameOverSelection) ? YELLOW : LIGHTGRAY;
        DrawTextEx(gameFont, options[i], (Vector2){SCREEN_WIDTH/2 - 100, 250 + i * 60}, 30, 2, color);
    }
}

void DrawVictoryScreen(int *victorySelection) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 50, 0, 200});
    DrawTextEx(gameFont, "VITORIA!", (Vector2){SCREEN_WIDTH/2 - 100, 150}, 48, 2, GREEN);
    DrawTextEx(gameFont, "Parabens! Voce completou todos os niveis!", 
              (Vector2){SCREEN_WIDTH/2 - 250, 220}, 24, 2, LIGHTGRAY);
    
    const char *options[] = {"Voltar ao Menu", "Sair"};
    for (int i = 0; i < 2; i++) {
        Color color = (i == *victorySelection) ? YELLOW : LIGHTGRAY;
        DrawTextEx(gameFont, options[i], (Vector2){SCREEN_WIDTH/2 - 80, 300 + i * 60}, 30, 2, color);
    }
}