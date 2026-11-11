#include "menu.h"
#include "tela.h"
#include "jogo.h"
#include "creditos.h"
#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

enum { BOTAO_JOGAR, BOTAO_COMANDOS, BOTAO_CREDITOS, BOTAO_SAIR, NUM_BOTOES };

#define BOTAO_X 330
#define BOTAO_Y_INICIAL 250
#define BOTAO_ESPACO_Y 80
#define BOTAO_LARGURA 300
#define BOTAO_ALTURA 60
#define BORDA_ESPESSURA 3
#define FPS_MENU 60

// Tela de comandos
#define PAINEL_ESQUERDA 150
#define PAINEL_DIREITA 650
#define PAINEL_TOPO 80
#define PAINEL_BAIXO 500
#define COMANDOS_TITULO_Y 110
#define COMANDOS_LINHA_Y_INICIAL 190
#define COMANDOS_ESPACO_Y 40
#define COMANDOS_COLUNA_TECLA 390
#define COMANDOS_COLUNA_ACAO 410
#define COMANDOS_RODAPE_Y 460

#define COR_BOTAO al_map_rgb(80, 50, 30)
#define COR_SELECIONADO al_map_rgb(200, 60, 60)
#define COR_TEXTO al_map_rgb(255, 255, 255)
#define COR_BORDA al_map_rgb(240, 200, 100)
#define COR_FUNDO_LISO al_map_rgb(40, 20, 15)

typedef struct {
    ALLEGRO_DISPLAY* display;
    ALLEGRO_FONT* fonte;
    ALLEGRO_BITMAP* fundo;
    ALLEGRO_EVENT_QUEUE* fila;
} Menu;

typedef struct {
    const char* tecla;
    const char* acao;
} Comando;

static const char* TEXTOS[NUM_BOTOES] = { "JOGAR", "COMANDOS", "CREDITOS", "SAIR" };

static const Comando COMANDOS[] = {
    { "A / D", "Andar" },
    { "ESPACO", "Pular" },
    { "MOUSE", "Mirar" },
    { "BOTAO ESQUERDO", "Atirar" },
    { "ESC", "Pausar" },
    { "F11", "Tela cheia" }
};
#define TOTAL_COMANDOS ((int)(sizeof(COMANDOS) / sizeof(COMANDOS[0])))

// Imagem do menu esticada na tela, ou fundo liso se ela nao carregou
static void desenhar_fundo(ALLEGRO_BITMAP* fundo) {
    if (fundo) {
        al_draw_scaled_bitmap(fundo, 0, 0, al_get_bitmap_width(fundo), al_get_bitmap_height(fundo), 0, 0, LARGURA_TELA, ALTURA_TELA, 0);
    }
    else {
        al_clear_to_color(COR_FUNDO_LISO);
    }
}

static void desenhar_comandos(const Menu* m) {
    tela_preparar_frame(m->display);
    desenhar_fundo(m->fundo);

    al_draw_filled_rectangle(PAINEL_ESQUERDA, PAINEL_TOPO, PAINEL_DIREITA, PAINEL_BAIXO, COR_FUNDO_LISO);
    al_draw_rectangle(PAINEL_ESQUERDA, PAINEL_TOPO, PAINEL_DIREITA, PAINEL_BAIXO, COR_BORDA, BORDA_ESPESSURA);
    al_draw_text(m->fonte, COR_BORDA, LARGURA_TELA / 2, COMANDOS_TITULO_Y, ALLEGRO_ALIGN_CENTER, "COMANDOS");

    for (int i = 0; i < TOTAL_COMANDOS; i++) {
        float y = COMANDOS_LINHA_Y_INICIAL + i * COMANDOS_ESPACO_Y;
        al_draw_text(m->fonte, COR_BORDA, COMANDOS_COLUNA_TECLA, y, ALLEGRO_ALIGN_RIGHT, COMANDOS[i].tecla);
        al_draw_text(m->fonte, COR_TEXTO, COMANDOS_COLUNA_ACAO, y, ALLEGRO_ALIGN_LEFT, COMANDOS[i].acao);
    }

    al_draw_text(m->fonte, COR_TEXTO, LARGURA_TELA / 2, COMANDOS_RODAPE_Y, ALLEGRO_ALIGN_CENTER, "ESC para voltar");
    al_flip_display();
}

// Retorna true se o jogador fechou a janela enquanto olhava os comandos.
static bool rodar_comandos(const Menu* m) {
    while (true) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(m->fila, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) return true;

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            int tecla = ev.keyboard.keycode;
            if (tecla == ALLEGRO_KEY_F11) tela_alternar_tela_cheia(m->display);
            else if (tecla == ALLEGRO_KEY_ESCAPE || tecla == ALLEGRO_KEY_ENTER) return false;
        }
        else if (ev.type == ALLEGRO_EVENT_TIMER) {
            desenhar_comandos(m);
        }
    }
}

// Retorna true quando o menu deve fechar. O resultado so vale nesse caso.
static bool tratar_tecla(const Menu* m, int tecla, int* selecionado, int* resultado) {
    if (tecla == ALLEGRO_KEY_UP || tecla == ALLEGRO_KEY_W) {
        *selecionado = (*selecionado + NUM_BOTOES - 1) % NUM_BOTOES;
    }
    else if (tecla == ALLEGRO_KEY_DOWN || tecla == ALLEGRO_KEY_S) {
        *selecionado = (*selecionado + 1) % NUM_BOTOES;
    }
    else if (tecla == ALLEGRO_KEY_F11) {
        tela_alternar_tela_cheia(m->display);
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
        if (*selecionado == BOTAO_COMANDOS) {
            if (rodar_comandos(m)) {
                *resultado = MENU_SAIR;
                return true;
            }
        }
        if (*selecionado == BOTAO_CREDITOS) {
            creditos_executar(m->display);
            // o ESC que fechou os creditos tambem chegou nessa fila e fecharia o menu
            al_flush_event_queue(m->fila);
        }
    }
    return false;
}

static void desenhar_menu(const Menu* m, int selecionado) {
    tela_preparar_frame(m->display);
    desenhar_fundo(m->fundo);

    for (int i = 0; i < NUM_BOTOES; i++) {
        float x = BOTAO_X;
        float y = BOTAO_Y_INICIAL + i * BOTAO_ESPACO_Y;
        ALLEGRO_COLOR cor = (i == selecionado) ? COR_SELECIONADO : COR_BOTAO;

        al_draw_filled_rectangle(x, y, x + BOTAO_LARGURA, y + BOTAO_ALTURA, cor);
        al_draw_rectangle(x, y, x + BOTAO_LARGURA, y + BOTAO_ALTURA, COR_BORDA, BORDA_ESPESSURA);

        float texto_y = y + BOTAO_ALTURA / 2 - al_get_font_line_height(m->fonte) / 2;
        al_draw_text(m->fonte, COR_TEXTO, x + BOTAO_LARGURA / 2, texto_y, ALLEGRO_ALIGN_CENTER, TEXTOS[i]);
    }

    al_flip_display();
}

static int rodar_menu(const Menu* m) {
    int selecionado = BOTAO_JOGAR;
    int resultado = MENU_SAIR;
    bool terminou = false;

    while (!terminou) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(m->fila, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            resultado = MENU_SAIR;
            terminou = true;
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            terminou = tratar_tecla(m, ev.keyboard.keycode, &selecionado, &resultado);
        }
        else if (ev.type == ALLEGRO_EVENT_TIMER) {
            desenhar_menu(m, selecionado);
        }
    }
    return resultado;
}

int menu_executar(ALLEGRO_DISPLAY* display) {
    int resultado = MENU_SAIR;

    Menu menu;
    menu.display = display;
    menu.fonte = al_create_builtin_font();
    menu.fundo = al_load_bitmap("menu_fundo.png");
    menu.fila = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS_MENU);

    // sem imagem o menu funciona com fundo liso
    if (!menu.fundo) fprintf(stderr, "menu: nao achei menu_fundo.png\n");

    if (menu.fonte && menu.fila && timer) {
        al_register_event_source(menu.fila, al_get_display_event_source(display));
        al_register_event_source(menu.fila, al_get_keyboard_event_source());
        al_register_event_source(menu.fila, al_get_timer_event_source(timer));
        al_start_timer(timer);

        resultado = rodar_menu(&menu);
        al_stop_timer(timer);
    }

    // so tem uma saida da funcao, entao nada fica sem liberar
    al_destroy_timer(timer);
    al_destroy_event_queue(menu.fila);
    al_destroy_bitmap(menu.fundo);
    al_destroy_font(menu.fonte);
    return resultado;
}