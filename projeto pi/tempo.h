#ifndef TEMPO_H
#define TEMPO_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

typedef struct
{
    double inicio;
    double decorrido;
    double duracao;
} Tempo;

// Inicializa o cronômetro
void tempo_inicializar(Tempo* tempo);

// Atualiza o tempo decorrido
void tempo_atualizar(Tempo* tempo);

// Desenha o cronômetro na tela
void tempo_desenhar(Tempo* tempo, ALLEGRO_FONT* fonte);

// Adiciona segundos ao cronômetro
void tempo_adicionar(Tempo* tempo, double segundos);

#endif