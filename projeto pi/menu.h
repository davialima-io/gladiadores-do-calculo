#pragma once
#ifndef MENU_H
#define MENU_H

#include <stdbool.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>


// Desenha a tela inicial do jogo
void menu_desenhar(
    ALLEGRO_FONT* fonte
);


// Verifica se o jogador clicou no botão START
bool menu_clicou_start(
    float mouse_x,
    float mouse_y
);


#endif