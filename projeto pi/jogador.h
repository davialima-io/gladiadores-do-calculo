#ifndef JOGADOR_H
#define JOGADOR_H

#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include "inimigo.h"

typedef struct {
    float x, y;
    float largura;   // Separado para manter a proporção do sprite
    float altura;
    float velocidade;

    float velocidade_y;
    float gravidade;
    float forca_pulo;

    bool no_chao;
    bool espaco_anterior;   // pra pular so uma vez por aperto

    // --- VARIÁVEIS DE ANIMAÇÃO ---
    ALLEGRO_BITMAP* sprite_sheet;
    int frame_atual;
    float tempo_animacao;
    bool atirando;
    bool olhando_direita;
    bool andando;
} Jogador;

void jogador_inicializar(Jogador* jogador);
// O main.c novo passa o mouse no atualizar para virar o personagem
void jogador_atualizar(Jogador* jogador, const ALLEGRO_KEYBOARD_STATE* estado_teclado, float mouse_x, float mouse_y);
void jogador_desenhar(const Jogador* jogador);
void jogador_desenhar_arma(const Jogador* jogador, float mouse_x, float mouse_y);

float jogador_centro_x(const Jogador* jogador);
float jogador_centro_y(const Jogador* jogador);
bool jogador_colidiu_com_inimigo(const Jogador* jogador, const Inimigo* inimigo);
void jogador_finalizar(Jogador* jogador);

#endif