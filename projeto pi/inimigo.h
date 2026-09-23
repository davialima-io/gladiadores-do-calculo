
#ifndef INIMIGO_H
#define INIMIGO_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>


// inimigo na arena
typedef struct
{
    float x;
    float y;

    float tamanho;
    float velocidade;

    int direcao;

    int vida;
    int vida_maxima;

} Inimigo;


// Coloca o inimigo na posição inicial
void inimigo_inicializar(
    Inimigo* inimigo
);


// Atualiza o movimento do inimigo
void inimigo_atualizar(
    Inimigo* inimigo
);


// Aplica dano ao inimigo
void inimigo_receber_dano(
    Inimigo* inimigo
);


// Verifica se um tiro acertou o inimigo
bool inimigo_colidiu_com_tiro(
    Inimigo* inimigo,
    float tiro_x,
    float tiro_y
);


// Desenha o inimigo e sua barra de vida
void inimigo_desenhar(
    Inimigo* inimigo
);


#endif