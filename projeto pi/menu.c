/* menu.c - Menu principal simples de "Gladiadores do Calculo"
   Controles: SETA CIMA/BAIXO para escolher, ENTER para confirmar, ESC para sair. */

#include "menu.h"

#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#define NUM_BOTOES 3

   /* Cores */
#define COR_BOTAO        al_map_rgb(80, 50, 30)
#define COR_SELECIONADO  al_map_rgb(200, 60, 60)
#define COR_TEXTO        al_map_rgb(255, 255, 255)
#define COR_BORDA        al_map_rgb(240, 200, 100)

int menu_executar(ALLEGRO_DISPLAY* display)
{
    /* Os 3 textos dos botoes */
    const char* textos[NUM_BOTOES] = { "JOGAR", "CREDITOS", "SAIR" };

    /* Posicao e tamanho de cada botao (x, y, largura, altura) */
    float bx[NUM_BOTOES] = { 330, 330, 330 };
    float by[NUM_BOTOES] = { 250, 330, 410 };
    float bw = 300;
    float bh = 60;

    /* Fonte embutida do Allegro (8x8) */
    ALLEGRO_FONT* fonte = al_create_builtin_font();
    if (!fonte) return MENU_SAIR;

    /* Imagem de fundo */
    ALLEGRO_BITMAP* fundo = al_load_bitmap("menu_fundo.png");
    if (!fundo)
        fprintf(stderr, "menu: nao achei menu_fundo.png\n");

    /* Fila de eventos e timer */
    ALLEGRO_EVENT_QUEUE* fila = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    al_register_event_source(fila, al_get_display_event_source(display));
    al_register_event_source(fila, al_get_keyboard_event_source());
    al_register_event_source(fila, al_get_timer_event_source(timer));
    al_start_timer(timer);

    int selecionado = 0;
    int resultado = MENU_SAIR;
    bool terminou = false;

    /* ---- LOOP PRINCIPAL DO MENU ---- */
    while (!terminou)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(fila, &ev);

        /* Fechar a janela no X */
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            resultado = MENU_SAIR;
            terminou = true;
        }

        /* Teclado */
        if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            int tecla = ev.keyboard.keycode;

            if (tecla == ALLEGRO_KEY_UP || tecla == ALLEGRO_KEY_W) {
                selecionado = selecionado - 1;
                if (selecionado < 0) selecionado = NUM_BOTOES - 1;
            }
            else if (tecla == ALLEGRO_KEY_DOWN || tecla == ALLEGRO_KEY_S) {
                selecionado = selecionado + 1;
                if (selecionado >= NUM_BOTOES) selecionado = 0;
            }
            else if (tecla == ALLEGRO_KEY_ENTER || tecla == ALLEGRO_KEY_SPACE) {
                if (selecionado == 0) { resultado = MENU_JOGAR; terminou = true; }
                else if (selecionado == 1) { /* creditos - por enquanto nao faz nada */ }
                else { resultado = MENU_SAIR; terminou = true; }
            }
            else if (tecla == ALLEGRO_KEY_ESCAPE) {
                resultado = MENU_SAIR;
                terminou = true;
            }
        }

        /* Desenho (a cada tick do timer) */
        if (ev.type == ALLEGRO_EVENT_TIMER)
        {
            /* Descobre o tamanho atual da janela */
            int largura_janela = al_get_display_width(display);
            int altura_janela = al_get_display_height(display);

            /* 1) Desenha o fundo ESTICADO para cobrir a janela toda */
            if (fundo)
            {
                int img_w = al_get_bitmap_width(fundo);
                int img_h = al_get_bitmap_height(fundo);

                al_draw_scaled_bitmap(
                    fundo,
                    0, 0, img_w, img_h,                 /* pedaco da imagem (inteira) */
                    0, 0, largura_janela, altura_janela,/* destino (janela toda)     */
                    0
                );
            }
            else
            {
                al_clear_to_color(al_map_rgb(40, 20, 15));
            }

            /* 2) Desenha cada botao */
            for (int i = 0; i < NUM_BOTOES; i++)
            {
                ALLEGRO_COLOR cor = (i == selecionado) ? COR_SELECIONADO : COR_BOTAO;

                /* Corpo do botao */
                al_draw_filled_rectangle(bx[i], by[i], bx[i] + bw, by[i] + bh, cor);

                /* Borda */
                al_draw_rectangle(bx[i], by[i], bx[i] + bw, by[i] + bh, COR_BORDA, 3);

                /* Texto centralizado dentro do botao */
                al_draw_text(fonte, COR_TEXTO,
                    bx[i] + bw / 2,
                    by[i] + bh / 2 - 4,
                    ALLEGRO_ALIGN_CENTER, textos[i]);
            }

            /* 3) Instrucoes na parte de baixo */
            al_draw_text(fonte, COR_TEXTO, largura_janela / 2, altura_janela - 30,
                ALLEGRO_ALIGN_CENTER,
                "SETAS PARA ESCOLHER - ENTER PARA CONFIRMAR");

            /* 4) Mostra na tela */
            al_flip_display();
        }
    }

    /* ---- LIMPEZA ---- */
    if (fundo) al_destroy_bitmap(fundo);
    al_destroy_timer(timer);
    al_destroy_event_queue(fila);
    al_destroy_font(fonte);

    return resultado;
}