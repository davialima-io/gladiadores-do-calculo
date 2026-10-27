#ifndef MENU_H
#define MENU_H

#include <allegro5/allegro.h>

typedef enum {
    MENU_JOGAR = 0,
    MENU_CREDITOS,   //tratado dentro do proprio menu 
    MENU_SAIR
} MenuOpcao;

/* Mostra o menu principal e fica nele ate o jogador escolher.
   Arquivo usado: menu_fundo.png (na pasta do projeto). */
int menu_executar(ALLEGRO_DISPLAY* display);

#endif