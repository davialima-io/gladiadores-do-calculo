#ifndef MENU_H
#define MENU_H

#include <allegro5/allegro.h>

typedef enum {
    MENU_JOGAR = 0,
    MENU_CREDITOS,
    MENU_SAIR
} MenuOpcao;

typedef enum {
    GAMEOVER_TEMPO = 0,
    GAMEOVER_VIDAS
} GameOverMotivo;

typedef enum {
    GAMEOVER_JOGAR_NOVAMENTE = 0,
    GAMEOVER_MENU_PRINCIPAL,
    GAMEOVER_SAIR
} GameOverOpcao;

// Mostra o menu principal (com o recorde embaixo) e fica nele ate o jogador escolher.
// Arquivo usado: menu_fundo.png (na pasta do projeto).
int menu_executar(ALLEGRO_DISPLAY* display, int recorde);

// Mostra a tela de fim de partida e retorna a opcao escolhida.
// Mantem o mesmo fundo, cores, botoes e controles do menu principal.
GameOverOpcao menu_gameover_executar(
    ALLEGRO_DISPLAY* display,
    GameOverMotivo motivo,
    int score,
    int recorde
);

#endif
