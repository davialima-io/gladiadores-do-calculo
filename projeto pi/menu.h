#ifndef MENU_H
#define MENU_H

#include <allegro5/allegro.h>

typedef enum {
    MENU_JOGAR = 0,
    MENU_CREDITOS,   /* tratado dentro do proprio menu */
    MENU_SAIR
} MenuOpcao;

/* Mostra o menu principal e fica nele ate o jogador escolher.
   Retorna MENU_JOGAR ou MENU_SAIR.
   Precisa de: al_init() e um display ja criado.
   Arquivo usado: menu_fundo.png (na pasta do projeto). */
int menu_executar(ALLEGRO_DISPLAY* display);

#endif /* MENU_H */