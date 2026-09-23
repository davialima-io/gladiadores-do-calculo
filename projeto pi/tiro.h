#pragma once
#ifndef TIRO_H
#define TIRO_H

#include <stdbool.h>


// Representa um projétil disparado pelo jogador
typedef struct
{
    float x;
    float y;

    float velocidade;

    float direcao_x;
    float direcao_y;

    bool ativo;

} Tiro;


// Inicializa todos os tiros
void tiros_inicializar(
    Tiro tiros[]
);


// Cria um novo tiro na direção do mouse
void tiro_disparar(
    Tiro tiros[],
    float jogador_x,
    float jogador_y,
    float mouse_x,
    float mouse_y
);


// Atualiza a posição dos tiros
void tiros_atualizar(
    Tiro tiros[]
);


// Desenha os tiros ativos
void tiros_desenhar(
    Tiro tiros[]
);


#endif