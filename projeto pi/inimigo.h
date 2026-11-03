#ifndef INIMIGO_H
#define INIMIGO_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <stdbool.h>

#define VIDA_MAXIMA_INIMIGO 17

typedef struct {
    float x;
    float y;
    float tamanho;
    float velocidade;
    int direcao;   // 1 = direita, -1 = esquerda

    float velocidade_y;
    float gravidade;
    float forca_pulo;
    bool no_chao;
    int tempo_pulo;   // frames desde o ultimo sorteio de pulo

    int vida;
    int vida_maxima;
} Inimigo;

void inimigo_inicializar(Inimigo* inimigo);
void inimigo_atualizar(Inimigo* inimigo);

// Tira uma vida. Quando zera, o inimigo volta pro ponto inicial com a vida cheia e retorna true.
bool inimigo_receber_dano(Inimigo* inimigo);

bool inimigo_colidiu_com_tiro(const Inimigo* inimigo, float tiro_x, float tiro_y);
void inimigo_desenhar(const Inimigo* inimigo);

#endif