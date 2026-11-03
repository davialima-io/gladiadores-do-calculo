#include "inimigo.h"
#include "jogo.h"
#include <stdlib.h>

#define INIMIGO_TAMANHO 50
#define INIMIGO_X_INICIAL 750
#define INIMIGO_Y_INICIAL (ALTURA_TELA - 100 - INIMIGO_TAMANHO)
#define FRAMES_ENTRE_PULOS 100

// Coloca o inimigo no canto direito com a vida cheia (usado no inicio e quando ele morre)
static void voltar_ao_ponto_inicial(Inimigo* inimigo) {
    inimigo->x = INIMIGO_X_INICIAL;
    inimigo->y = INIMIGO_Y_INICIAL;
    inimigo->velocidade_y = 0;
    inimigo->no_chao = true;
    inimigo->direcao = -1;
    inimigo->vida = inimigo->vida_maxima;
}

void inimigo_inicializar(Inimigo* inimigo) {
    inimigo->tamanho = INIMIGO_TAMANHO;
    inimigo->velocidade = 3.5f;
    inimigo->gravidade = 0.5f;
    inimigo->forca_pulo = -10.0f;
    inimigo->tempo_pulo = 0;
    inimigo->vida_maxima = VIDA_MAXIMA_INIMIGO;
    voltar_ao_ponto_inicial(inimigo);
}

void inimigo_atualizar(Inimigo* inimigo) {
    // anda pro lado e inverte a direcao quando bate na parede
    inimigo->x += inimigo->velocidade * inimigo->direcao;
    if (inimigo->x + inimigo->tamanho >= LARGURA_TELA) {
        inimigo->x = LARGURA_TELA - inimigo->tamanho;
        inimigo->direcao = -1;
    }
    if (inimigo->x <= 0) {
        inimigo->x = 0;
        inimigo->direcao = 1;
    }

    // de tempos em tempos sorteia se vai pular (50% de chance)
    inimigo->tempo_pulo++;
    if (inimigo->tempo_pulo >= FRAMES_ENTRE_PULOS) {
        inimigo->tempo_pulo = 0;
        if (inimigo->no_chao && rand() % 2 == 0) {
            inimigo->velocidade_y = inimigo->forca_pulo;
            inimigo->no_chao = false;
        }
    }

    inimigo->velocidade_y += inimigo->gravidade;
    inimigo->y += inimigo->velocidade_y;

    float chao = CHAO_Y - inimigo->tamanho;
    if (inimigo->y >= chao) {
        inimigo->y = chao;
        inimigo->velocidade_y = 0;
        inimigo->no_chao = true;
    }
    else {
        inimigo->no_chao = false;
    }
}

bool inimigo_receber_dano(Inimigo* inimigo) {
    inimigo->vida--;
    if (inimigo->vida > 0) return false;

    voltar_ao_ponto_inicial(inimigo);
    return true;
}

bool inimigo_colidiu_com_tiro(const Inimigo* inimigo, float tiro_x, float tiro_y) {
    return tiro_x >= inimigo->x && tiro_x <= inimigo->x + inimigo->tamanho && tiro_y >= inimigo->y && tiro_y <= inimigo->y + inimigo->tamanho;
}

void inimigo_desenhar(const Inimigo* inimigo) {
    al_draw_filled_rectangle(inimigo->x, inimigo->y, inimigo->x + inimigo->tamanho, inimigo->y + inimigo->tamanho, al_map_rgb(200, 50, 50));

    // barra de vida em cima da cabeca, a verde encolhe conforme ele apanha
    float largura_barra = 60;
    float altura_barra = 8;
    float barra_x = inimigo->x - 5;
    float barra_y = inimigo->y - 15;
    float largura_vida = largura_barra * ((float)inimigo->vida / inimigo->vida_maxima);

    al_draw_filled_rectangle(barra_x, barra_y, barra_x + largura_barra, barra_y + altura_barra, al_map_rgb(50, 50, 50));
    al_draw_filled_rectangle(barra_x, barra_y, barra_x + largura_vida, barra_y + altura_barra, al_map_rgb(50, 220, 70));
}