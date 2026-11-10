#pragma once

#ifndef PAUSE_H
#define PAUSE_H

#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

// Opções do menu de pause
typedef enum
{
    PAUSE_NENHUMA_ACAO,
    PAUSE_CONTINUAR,
    PAUSE_RECOMECAR,
    PAUSE_MENU_PRINCIPAL

} PauseAcao;

// Estrutura do menu
typedef struct
{
    bool ativo;
    int opcao;

} MenuPause;

// Funções
void pause_inicializar(MenuPause* pause);
PauseAcao pause_processar_evento(
    MenuPause* pause,
    ALLEGRO_EVENT* evento,
    int largura_tela,
    int altura_tela
);

void pause_desenhar(
    MenuPause* pause,
    ALLEGRO_FONT* fonte,
    int largura_tela,
    int altura_tela
);

#endif