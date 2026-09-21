#ifndef JOGADOR_H
#define JOGADOR_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>


// Representa o jogador dentro da arena
typedef struct
{
    float x;
    float y;

    float tamanho;
    float velocidade;

} Jogador;


// Coloca o jogador na posição inicial
void jogador_inicializar(
    Jogador* jogador
);


// Atualiza a movimentação do jogador
void jogador_atualizar(
    Jogador* jogador,
    ALLEGRO_KEYBOARD_STATE* estado_teclado
);


// Desenha o jogador
void jogador_desenhar(
    Jogador* jogador
);


// Desenha a arma apontada para o mouse
void jogador_desenhar_arma(
    Jogador* jogador,
    float mouse_x,
    float mouse_y
);


// Retorna a coordenada X do centro do jogador
float jogador_centro_x(
    Jogador* jogador
);


// Retorna a coordenada Y do centro do jogador
float jogador_centro_y(
    Jogador* jogador
);


#endif