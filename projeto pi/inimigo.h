#ifndef INIMIGO_H
#define INIMIGO_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <stdbool.h>

#define VIDA_MAXIMA_INIMIGO 17

// o valor tambem e a linha da tabela de frames do leao
typedef enum {
    INIMIGO_CORRENDO = 0,
    INIMIGO_ATACANDO = 1
} EstadoInimigo;

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

    EstadoInimigo estado;
    bool alvo_no_alcance;   // jogador ao alcance da mordida agora
    int frame_atual;
    float tempo_animacao;
    ALLEGRO_BITMAP* sprite_sheet;
} Inimigo;

void inimigo_inicializar(Inimigo* inimigo);

// alvo_x e alvo_y e o centro do jogador, o leao persegue ele
void inimigo_atualizar(Inimigo* inimigo, float alvo_x, float alvo_y);

// true quando o leao esta atacando e o jogador esta ao alcance da mordida
bool inimigo_atacando(const Inimigo* inimigo);

// Tira uma vida. Quando zera, o inimigo volta pro ponto inicial com a vida cheia e retorna true.
bool inimigo_receber_dano(Inimigo* inimigo);

bool inimigo_colidiu_com_tiro(const Inimigo* inimigo, float tiro_x, float tiro_y);
void inimigo_desenhar(const Inimigo* inimigo);
void inimigo_finalizar(Inimigo* inimigo);

#endif