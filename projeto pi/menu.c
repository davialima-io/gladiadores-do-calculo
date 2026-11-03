#include "menu.h"
#include "tela.h"
#include "jogo.h"
#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

enum { BOTAO_JOGAR, BOTAO_CREDITOS, BOTAO_SAIR, NUM_BOTOES };

#define BOTAO_X 330
#define BOTAO_Y_INICIAL 250
#define BOTAO_ESPACO_Y 80
#define BOTAO_LARGURA 300
#define BOTAO_ALTURA 60
#define BORDA_ESPESSURA 3
#define FPS_MENU 60

#define COR_BOTAO al_map_rgb(80, 50, 30)
#define COR_SELECIONADO al_map_rgb(200, 60, 60)
#define COR_TEXTO al_map_rgb(255, 255, 255)
#define COR_BORDA al_map_rgb(240, 200, 100)

static const char* TEXTOS[NUM_BOTOES] = { "JOGAR", "CREDITOS", "SAIR" };

// Retorna true quando o menu deve fechar. O resultado so vale nesse caso.
static bool tratar_tecla(ALLEGRO_DISPLAY* display, int tecla, int* selecionado, int* resultado) {
    if (tecla == ALLEGRO_KEY_UP || tecla == ALLEGRO_KEY_W) {
        *selecionado = (*selecionado + NUM_BOTOES - 1) % NUM_BOTOES;
    }
    else if (tecla == ALLEGRO_KEY_DOWN || tecla == ALLEGRO_KEY_S) {
        *selecionado = (*selecionado + 1) % NUM_BOTOES;
    }
    else if (tecla == ALLEGRO_KEY_F11) {
        tela_alternar_tela_cheia(display);
    }
    else if (tecla == ALLEGRO_KEY_ESCAPE) {
        *resultado = MENU_SAIR;
        return true;
    }
    else if (tecla == ALLEGRO_KEY_ENTER || tecla == ALLEGRO_KEY_SPACE) {
        if (*selecionado == BOTAO_JOGAR) {
            *resultado = MENU_JOGAR;
            return true;
        }
        if (*selecionado == BOTAO_SAIR) {
            *resultado = MENU_SAIR;
            return true;
        }
        // creditos ainda nao tem tela, entao fica no menu
    }
    return false;
}

static void desenhar_menu(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* fonte, ALLEGRO_BITMAP* fundo, int selecionado) {
    tela_preparar_frame(display);

    if (fundo) {
        al_draw_scaled_bitmap(fundo, 0, 0, al_get_bitmap_width(fundo), al_get_bitmap_height(fundo), 0, 0, LARGURA_TELA, ALTURA_TELA, 0);
    }
    else {
        al_clear_to_color(al_map_rgb(40, 20, 15));
    }

    for (int i = 0; i < NUM_BOTOES; i++) {
        float x = BOTAO_X;
        float y = BOTAO_Y_INICIAL + i * BOTAO_ESPACO_Y;
        ALLEGRO_COLOR cor = (i == selecionado) ? COR_SELECIONADO : COR_BOTAO;

        al_draw_filled_rectangle(x, y, x + BOTAO_LARGURA, y + BOTAO_ALTURA, cor);
        al_draw_rectangle(x, y, x + BOTAO_LARGURA, y + BOTAO_ALTURA, COR_BORDA, BORDA_ESPESSURA);

        float texto_y = y + BOTAO_ALTURA / 2 - al_get_font_line_height(fonte) / 2;
        al_draw_text(fonte, COR_TEXTO, x + BOTAO_LARGURA / 2, texto_y, ALLEGRO_ALIGN_CENTER, TEXTOS[i]);
    }

    al_flip_display();
}

static int rodar_menu(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* fonte, ALLEGRO_BITMAP* fundo, ALLEGRO_EVENT_QUEUE* fila) {
    int selecionado = BOTAO_JOGAR;
    int resultado = MENU_SAIR;
    bool terminou = false;

    while (!terminou) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(fila, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            resultado = MENU_SAIR;
            terminou = true;
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            terminou = tratar_tecla(display, ev.keyboard.keycode, &selecionado, &resultado);
        }
        else if (ev.type == ALLEGRO_EVENT_TIMER) {
            desenhar_menu(display, fonte, fundo, selecionado);
        }
    }
    return resultado;
}

int menu_executar(ALLEGRO_DISPLAY* display) {
    int resultado = MENU_SAIR;

    ALLEGRO_FONT* fonte = al_create_builtin_font();
    ALLEGRO_BITMAP* fundo = al_load_bitmap("menu_fundo.png");
    ALLEGRO_EVENT_QUEUE* fila = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS_MENU);

    // sem imagem o menu funciona com fundo liso
    if (!fundo) fprintf(stderr, "menu: nao achei menu_fundo.png\n");

    if (fonte && fila && timer) {
        al_register_event_source(fila, al_get_display_event_source(display));
        al_register_event_source(fila, al_get_keyboard_event_source());
        al_register_event_source(fila, al_get_timer_event_source(timer));
        al_start_timer(timer);

        resultado = rodar_menu(display, fonte, fundo, fila);
        al_stop_timer(timer);
    }

    // so tem uma saida da funcao, entao nada fica sem liberar
    al_destroy_timer(timer);
    al_destroy_event_queue(fila);
    al_destroy_bitmap(fundo);
    al_destroy_font(fonte);
    return resultado;
}