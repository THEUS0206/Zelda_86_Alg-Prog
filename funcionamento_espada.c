void ativar_espada(jogador *jogador, monstro *monstros, int conta_monstro) { //olha se o jogador tem a espada

    if(!jogador->espada) return;


    jogador->espada_ativa = true; // ira ativar a espada

    posicao zona_ataque[3]; // zona de ataque sera 3 blocos a frente

    // ve as posicoes de ataque conforme a direcao


    for(int i = 0; i < 3; i++) {
        zona_ataque[i] = jogador->pos; // vai comecar na posicao do jogador

        if (jogador->direcao == 'N') {
        zona_ataque[i].y -= i+1; // ataca para cima

        }
        else if(jogador->direcao == 'S') {
        zona_ataque[i].y += i+1;
        }
        else if(jogador->direcao == 'L') {
        zona_ataque[i].x += i+1;
        }
        else if(jogador->direcao == 'O') {
        zona_ataque[i].x -= i+1;
       }
    }

    //verifica se cada monstro esta na posicao de ataque

    for (int i = 0; i < conta_monstro; i++) {
        if (monstro[i].vivo) {
            for (int j = 0; j < 3; j++) {
                if(monstro[i]. pos.x == zona_ataque[j].x && monstro[i].pos.y == zona_ataque[j].y) {
                    monstro[i]. vivo = false; // mata o monstro
                    jogador->pontuacao += monstro[i].pontos; //adiciona pontos
                    j = 3; //sai do loop interno

                }
            }
        }
    }
}
