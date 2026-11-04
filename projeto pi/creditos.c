#include "creditos.h"
#include "tela.h"
#include "jogo.h"
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>

#define FPS_CREDITOS 60
#define ESPACO_LINHA 24
#define VELOCIDADE_SCROLL 1.0f
#define MULTIPLICADOR_RAPIDO 4

#define COR_FUNDO al_map_rgb(40, 20, 15)
#define COR_TEXTO al_map_rgb(255, 255, 255)
#define COR_TITULO al_map_rgb(240, 200, 100)

static const char* LINHAS[] = {
    "Gladiadores do Calculo",
    "",
    "DESENVOLVIDO POR",
    "Arthur Santana",
    "Davi Albuquerque",
    "Kamilie Mendonca",
    "",
    "PROFESSOR",
    "Mario Leandro Pires Toledo",
    "",
    "DISCIPLINA",
    "Projeto Integrador - 2o semestre - Senac SP",
    "",
    "FERRAMENTAS",
    "Allegro 5",
    "Visual Studio",
    "",
    "OBRIGADO POR JOGAR"
};
#define TOTAL_LINHAS ((int)(sizeof(LINHAS) / sizeof(LINHAS[0])))

// Titulo = linha toda em maiusculas, mais o nome do jogo na primeira linha.
static bool eh_titulo(int indice) {
    const char* texto = LINHAS[indice];
    if (indice == 0) return true;
    if (texto[0] == '\0') return false;
    for (int i = 0; texto[i] != '\0'; i++) {
        if (islower((unsigned char)texto[i])) return false;
    }
    return true;
}

// Retorna true quando os creditos devem fechar.
static bool tratar_tecla(ALLEGRO_DISPLAY* display, int tecla) {
    if (tecla == ALLEGRO_KEY_F11) {
        tela_alternar_tela_cheia(display);
    }
    else if (tecla == ALLEGRO_KEY_ESCAPE || tecla == ALLEGRO_KEY_ENTER) {
        return true;
    }
    return false;
}

static void desenhar_creditos(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* fonte, ALLEGRO_BITMAP* fundo, float scroll_y) {
    tela_preparar_frame(display);

    if (fundo) {
        al_draw_scaled_bitmap(fundo, 0, 0, al_get_bitmap_width(fundo), al_get_bitmap_height(fundo), 0, 0, LARGURA_TELA, ALTURA_TELA, 0);
    }
    else {
        al_clear_to_color(COR_FUNDO);
    }

    for (int i = 0; i < TOTAL_LINHAS; i++) {
        float y = scroll_y + i * ESPACO_LINHA;
        // linha fora da tela nao precisa ser desenhada
        if (y >= -ESPACO_LINHA && y <= ALTURA_TELA) {
            ALLEGRO_COLOR cor = eh_titulo(i) ? COR_TITULO : COR_TEXTO;
            al_draw_text(fonte, cor, LARGURA_TELA / 2, y, ALLEGRO_ALIGN_CENTER, LINHAS[i]);
        }
    }

    al_flip_display();
}

// Segurar seta pra baixo acelera a rolagem.
static float velocidade_atual(void) {
    ALLEGRO_KEYBOARD_STATE teclado;
    al_get_keyboard_state(&teclado);
    if (al_key_down(&teclado, ALLEGRO_KEY_DOWN) || al_key_down(&teclado, ALLEGRO_KEY_S)) {
        return VELOCIDADE_SCROLL * MULTIPLICADOR_RAPIDO;
    }
    return VELOCIDADE_SCROLL;
}

static void rodar_creditos(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* fonte, ALLEGRO_BITMAP* fundo, ALLEGRO_EVENT_QUEUE* fila) {
    // comeca logo abaixo da tela e sobe ate a ultima linha sumir
    float scroll_y = ALTURA_TELA;
    bool terminou = false;

    while (!terminou) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(fila, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            terminou = true;
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            terminou = tratar_tecla(display, ev.keyboard.keycode);
        }
        else if (ev.type == ALLEGRO_EVENT_TIMER) {
            scroll_y -= velocidade_atual();
            desenhar_creditos(display, fonte, fundo, scroll_y);
            terminou = (scroll_y + TOTAL_LINHAS * ESPACO_LINHA < 0);
        }
    }
}

void creditos_executar(ALLEGRO_DISPLAY* display) {
    ALLEGRO_FONT* fonte = al_create_builtin_font();
    ALLEGRO_BITMAP* fundo = al_load_bitmap("menu_fundo.png");
    ALLEGRO_EVENT_QUEUE* fila = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS_CREDITOS);

    // mesma imagem do menu
    if (fonte && fila && timer) {
        al_register_event_source(fila, al_get_display_event_source(display));
        al_register_event_source(fila, al_get_keyboard_event_source());
        al_register_event_source(fila, al_get_timer_event_source(timer));
        al_start_timer(timer);

        rodar_creditos(display, fonte, fundo, fila);
        al_stop_timer(timer);
    }
    else {
        fprintf(stderr, "creditos: falhou ao criar fonte, fila ou timer\n");
    }

    al_destroy_timer(timer);
    al_destroy_event_queue(fila);
    al_destroy_bitmap(fundo);
    al_destroy_font(fonte);
}