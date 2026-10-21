#ifndef INIMIGO_H
#define INIMIGO_H
#define VIDA_MAXIMA_INIMIGO 3

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <stdbool.h>

#define VIDA_MAXIMA_INIMIGO 10

typedef struct
{
    float x;
    float y;

    float tamanho;
    float velocidade;
    int direcao; // 1 para direita, -1 para esquerda

    // Variáveis de pulo e gravidade
    float velocidade_y;
    float gravidade;
    float forca_pulo;
    int no_chao;

    int vida;
    int vida_maxima;

    int tempo_pulo; // Controla quando ele decide pular aleatoriamente

} Inimigo;

void inimigo_inicializar(Inimigo* inimigo, float x, float y);
void inimigo_atualizar(Inimigo* inimigo, float largura_tela, float altura_chao);
void inimigo_receber_dano(Inimigo* inimigo, float x_inicial, float altura_chao);
bool inimigo_colidiu_com_tiro(Inimigo* inimigo, float tiro_x, float tiro_y);
void inimigo_desenhar(Inimigo* inimigo);

#endif