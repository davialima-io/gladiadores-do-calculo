/* menu.c - Menu principal: apenas a imagem de fundo.
   Qualquer tecla -> MENU_JOGAR. ESC ou fechar a janela -> MENU_SAIR. */

#include "menu.h"

#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

int menu_executar(ALLEGRO_DISPLAY* display)
{
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

    int resultado = MENU_JOGAR;
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

        /* Qualquer tecla -> sai do menu e vai para o jogo.
           ESC -> encerra o programa. */
        if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                resultado = MENU_SAIR;
            }
            else {
                resultado = MENU_JOGAR;
            }
            terminou = true;
        }

        /* Desenho (a cada tick do timer) */
        if (ev.type == ALLEGRO_EVENT_TIMER)
        {
            int largura_janela = al_get_display_width(display);
            int altura_janela = al_get_display_height(display);

            if (fundo)
            {
                int img_w = al_get_bitmap_width(fundo);
                int img_h = al_get_bitmap_height(fundo);

                al_draw_scaled_bitmap(
                    fundo,
                    0, 0, img_w, img_h,
                    0, 0, largura_janela, altura_janela,
                    0
                );
            }
            else
            {
                al_clear_to_color(al_map_rgb(40, 20, 15));
            }

            al_flip_display();
        }
    }

    /* ---- LIMPEZA ---- */
    if (fundo) al_destroy_bitmap(fundo);
    al_destroy_timer(timer);
    al_destroy_event_queue(fila);

    return resultado;
}