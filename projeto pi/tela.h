#ifndef TELA_H
#define TELA_H

#include <stdbool.h>
#include <allegro5/allegro.h>

/* Alterna entre janela e tela cheia (modo janela sem bordas). */
void tela_alternar_tela_cheia(ALLEGRO_DISPLAY* display);

/* Chame UMA VEZ por frame, logo antes de desenhar.
   Limpa a tela de preto, aplica escala + centralizacao e
   restringe o desenho a area do jogo (800x600 logicos). */
void tela_preparar_frame(ALLEGRO_DISPLAY* display);

/* Converte coordenadas do mouse (pixels da janela)
   para coordenadas do jogo (0..LARGURA_TELA, 0..ALTURA_TELA). */
void tela_mouse_para_jogo(ALLEGRO_DISPLAY* display, float* x, float* y);

#endif