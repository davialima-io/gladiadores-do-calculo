#pragma once
#ifndef JOGADOR_H
#define JOGADOR_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

typedef struct
{
    float x, y;
    float tamanho;
    float velocidade;

    float velocidade_y;
    float gravidade;
    float forca_pulo;

    int no_chao;
    int espaco_anterior;

} Jogador;

void jogador_inicializar(Jogador* jogador);

void jogador_atualizar(
    Jogador* jogador,
    ALLEGRO_KEYBOARD_STATE* estado_teclado
);

void jogador_desenhar(Jogador* jogador);

void jogador_desenhar_arma(
    Jogador* jogador,
    float mouse_x,
    float mouse_y
);

float jogador_centro_x(Jogador* jogador);
float jogador_centro_y(Jogador* jogador);

#endif