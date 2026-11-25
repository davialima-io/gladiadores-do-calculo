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
enum { GAMEOVER_BOTAO_REJOGAR, GAMEOVER_BOTAO_MENU, NUM_BOTOES_GAMEOVER };

#define BOTAO_X 330
#define BOTAO_Y_INICIAL 250
#define BOTAO_ESPACO_Y 80
#define BOTAO_LARGURA 300
#define BOTAO_ALTURA 60
#define BORDA_ESPESSURA 3
#define FPS_MENU 60
#define BOTAO_NENHUM -1
#define RECORDE_Y 570

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

#define GAMEOVER_PAINEL_ESQUERDA 150
#define GAMEOVER_PAINEL_DIREITA 650
#define GAMEOVER_PAINEL_TOPO 65
#define GAMEOVER_PAINEL_BAIXO 535
#define GAMEOVER_TITULO_Y 100
#define GAMEOVER_MOTIVO_Y 160
#define GAMEOVER_SCORE_Y 210
#define GAMEOVER_RECORDE_Y 240
#define GAMEOVER_BOTAO_X 250
#define GAMEOVER_BOTAO_Y_INICIAL 320
#define GAMEOVER_BOTAO_ESPACO_Y 85
#define GAMEOVER_BOTAO_LARGURA 300
#define GAMEOVER_BOTAO_ALTURA 60
#define GAMEOVER_RODAPE_Y 485

#define COR_BOTAO al_map_rgb(80, 50, 30)
#define COR_SELECIONADO al_map_rgb(200, 60, 60)
#define COR_TEXTO al_map_rgb(255, 255, 255)
#define COR_BORDA al_map_rgb(240, 200, 100)
#define COR_FUNDO_LISO al_map_rgb(40, 20, 15)
#define COR_GAMEOVER al_map_rgb(200, 60, 60)

typedef struct {
    ALLEGRO_DISPLAY* display;
    ALLEGRO_FONT* fonte;
    ALLEGRO_BITMAP* fundo;
    ALLEGRO_EVENT_QUEUE* fila;
    int recorde;
} Menu;

typedef struct {
    const char* tecla;
    const char* acao;
} Comando;

static const char* TEXTOS[NUM_BOTOES] = { "JOGAR", "COMANDOS", "CREDITOS", "SAIR" };
static const char* TEXTOS_GAMEOVER[NUM_BOTOES_GAMEOVER] = { "JOGAR NOVAMENTE", "MENU PRINCIPAL" };
static const Comando COMANDOS[] = {
    { "A / D", "Andar" },
    { "ESPACO", "Pular" },
    { "MOUSE", "Mirar" },
    { "BOTAO ESQUERDO", "Atirar" },
    { "ESC", "Pausar" },
    { "F11", "Tela cheia" }
};
#define TOTAL_COMANDOS ((int)(sizeof(COMANDOS) / sizeof(COMANDOS[0])))

static float botao_y(int i) {
    return BOTAO_Y_INICIAL + i * BOTAO_ESPACO_Y;
}

static float gameover_botao_y(int i) {
    return GAMEOVER_BOTAO_Y_INICIAL + i * GAMEOVER_BOTAO_ESPACO_Y;
}

static int botao_sob_mouse(const Menu* m, float mouse_x, float mouse_y) {
    tela_mouse_para_jogo(m->display, &mouse_x, &mouse_y);
    for (int i = 0; i < NUM_BOTOES; i++) {
        float y = botao_y(i);
        if (mouse_x >= BOTAO_X && mouse_x <= BOTAO_X + BOTAO_LARGURA &&
            mouse_y >= y && mouse_y <= y + BOTAO_ALTURA) return i;
    }
    return BOTAO_NENHUM;
}

static int gameover_botao_sob_mouse(const Menu* m, float mouse_x, float mouse_y) {
    tela_mouse_para_jogo(m->display, &mouse_x, &mouse_y);
    for (int i = 0; i < NUM_BOTOES_GAMEOVER; i++) {
        float y = gameover_botao_y(i);
        if (mouse_x >= GAMEOVER_BOTAO_X && mouse_x <= GAMEOVER_BOTAO_X + GAMEOVER_BOTAO_LARGURA &&
            mouse_y >= y && mouse_y <= y + GAMEOVER_BOTAO_ALTURA) return i;
    }
    return BOTAO_NENHUM;
}

static void desenhar_fundo(ALLEGRO_BITMAP* fundo) {
    if (fundo) {
        al_draw_scaled_bitmap(fundo, 0, 0,
            al_get_bitmap_width(fundo), al_get_bitmap_height(fundo),
            0, 0, LARGURA_TELA, ALTURA_TELA, 0);
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
    al_draw_text(m->fonte, COR_TEXTO, LARGURA_TELA / 2, COMANDOS_RODAPE_Y, ALLEGRO_ALIGN_CENTER, "ESC ou clique para voltar");
    al_flip_display();
}

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
        else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev.mouse.button == 1) {
            return false;
        }
        else if (ev.type == ALLEGRO_EVENT_TIMER) {
            desenhar_comandos(m);
        }
    }
}

static bool ativar_botao(const Menu* m, int botao, int* resultado) {
    switch (botao) {
    case BOTAO_JOGAR:
        *resultado = MENU_JOGAR;
        return true;
    case BOTAO_SAIR:
        *resultado = MENU_SAIR;
        return true;
    case BOTAO_COMANDOS:
        if (rodar_comandos(m)) {
            *resultado = MENU_SAIR;
            return true;
        }
        break;
    case BOTAO_CREDITOS:
        creditos_executar(m->display);
        al_flush_event_queue(m->fila);
        break;
    }
    return false;
}

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
        return ativar_botao(m, *selecionado, resultado);
    }
    return false;
}

static bool tratar_mouse(const Menu* m, const ALLEGRO_EVENT* ev, int* selecionado, int* resultado) {
    int botao = botao_sob_mouse(m, (float)ev->mouse.x, (float)ev->mouse.y);
    if (botao == BOTAO_NENHUM) return false;
    *selecionado = botao;
    if (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev->mouse.button == 1)
        return ativar_botao(m, botao, resultado);
    return false;
}

static void desenhar_menu(const Menu* m, int selecionado) {
    tela_preparar_frame(m->display);
    desenhar_fundo(m->fundo);
    for (int i = 0; i < NUM_BOTOES; i++) {
        float y = botao_y(i);
        ALLEGRO_COLOR cor = (i == selecionado) ? COR_SELECIONADO : COR_BOTAO;
        al_draw_filled_rectangle(BOTAO_X, y, BOTAO_X + BOTAO_LARGURA, y + BOTAO_ALTURA, cor);
        al_draw_rectangle(BOTAO_X, y, BOTAO_X + BOTAO_LARGURA, y + BOTAO_ALTURA, COR_BORDA, BORDA_ESPESSURA);
        float texto_y = y + BOTAO_ALTURA / 2 - al_get_font_line_height(m->fonte) / 2;
        al_draw_text(m->fonte, COR_TEXTO, BOTAO_X + BOTAO_LARGURA / 2, texto_y, ALLEGRO_ALIGN_CENTER, TEXTOS[i]);
    }
    al_draw_textf(m->fonte, COR_BORDA, LARGURA_TELA / 2, RECORDE_Y, ALLEGRO_ALIGN_CENTER, "RECORDE: %d", m->recorde);
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
        else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES || ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            terminou = tratar_mouse(m, &ev, &selecionado, &resultado);
        }
        else if (ev.type == ALLEGRO_EVENT_TIMER) {
            desenhar_menu(m, selecionado);
        }
    }
    return resultado;
}

static void desenhar_gameover(const Menu* m, GameOverMotivo motivo, int score, int selecionado) {
    const char* texto_motivo = (motivo == GAMEOVER_TEMPO)
        ? "O TEMPO ACABOU"
        : "VOCE PERDEU TODAS AS VIDAS";

    tela_preparar_frame(m->display);
    desenhar_fundo(m->fundo);
    al_draw_filled_rectangle(GAMEOVER_PAINEL_ESQUERDA, GAMEOVER_PAINEL_TOPO,
        GAMEOVER_PAINEL_DIREITA, GAMEOVER_PAINEL_BAIXO, COR_FUNDO_LISO);
    al_draw_rectangle(GAMEOVER_PAINEL_ESQUERDA, GAMEOVER_PAINEL_TOPO,
        GAMEOVER_PAINEL_DIREITA, GAMEOVER_PAINEL_BAIXO, COR_BORDA, BORDA_ESPESSURA);

    al_draw_text(m->fonte, COR_GAMEOVER, LARGURA_TELA / 2, GAMEOVER_TITULO_Y,
        ALLEGRO_ALIGN_CENTER, "GAME OVER");
    al_draw_text(m->fonte, COR_TEXTO, LARGURA_TELA / 2, GAMEOVER_MOTIVO_Y,
        ALLEGRO_ALIGN_CENTER, texto_motivo);
    al_draw_textf(m->fonte, COR_BORDA, LARGURA_TELA / 2, GAMEOVER_SCORE_Y,
        ALLEGRO_ALIGN_CENTER, "SCORE: %d", score);
    al_draw_textf(m->fonte, COR_BORDA, LARGURA_TELA / 2, GAMEOVER_RECORDE_Y,
        ALLEGRO_ALIGN_CENTER, "RECORDE: %d", m->recorde);

    for (int i = 0; i < NUM_BOTOES_GAMEOVER; i++) {
        float y = gameover_botao_y(i);
        ALLEGRO_COLOR cor = (i == selecionado) ? COR_SELECIONADO : COR_BOTAO;
        al_draw_filled_rectangle(GAMEOVER_BOTAO_X, y,
            GAMEOVER_BOTAO_X + GAMEOVER_BOTAO_LARGURA, y + GAMEOVER_BOTAO_ALTURA, cor);
        al_draw_rectangle(GAMEOVER_BOTAO_X, y,
            GAMEOVER_BOTAO_X + GAMEOVER_BOTAO_LARGURA, y + GAMEOVER_BOTAO_ALTURA,
            COR_BORDA, BORDA_ESPESSURA);
        float texto_y = y + GAMEOVER_BOTAO_ALTURA / 2 - al_get_font_line_height(m->fonte) / 2;
        al_draw_text(m->fonte, COR_TEXTO,
            GAMEOVER_BOTAO_X + GAMEOVER_BOTAO_LARGURA / 2, texto_y,
            ALLEGRO_ALIGN_CENTER, TEXTOS_GAMEOVER[i]);
    }

    al_draw_text(m->fonte, COR_TEXTO, LARGURA_TELA / 2, GAMEOVER_RODAPE_Y,
        ALLEGRO_ALIGN_CENTER, "Não deixe de aprender! Seu professor de cálculo agradece.");
    al_flip_display();
}

static GameOverOpcao rodar_gameover(const Menu* m, GameOverMotivo motivo, int score) {
    int selecionado = GAMEOVER_BOTAO_REJOGAR;
    while (true) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(m->fila, &ev);
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) return GAMEOVER_SAIR;
        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            int tecla = ev.keyboard.keycode;
            if (tecla == ALLEGRO_KEY_UP || tecla == ALLEGRO_KEY_W)
                selecionado = (selecionado + NUM_BOTOES_GAMEOVER - 1) % NUM_BOTOES_GAMEOVER;
            else if (tecla == ALLEGRO_KEY_DOWN || tecla == ALLEGRO_KEY_S)
                selecionado = (selecionado + 1) % NUM_BOTOES_GAMEOVER;
            else if (tecla == ALLEGRO_KEY_F11)
                tela_alternar_tela_cheia(m->display);
            else if (tecla == ALLEGRO_KEY_ESCAPE)
                return GAMEOVER_MENU_PRINCIPAL;
            else if (tecla == ALLEGRO_KEY_ENTER || tecla == ALLEGRO_KEY_SPACE)
                return (selecionado == GAMEOVER_BOTAO_REJOGAR)
                ? GAMEOVER_JOGAR_NOVAMENTE : GAMEOVER_MENU_PRINCIPAL;
        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES || ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            int botao = gameover_botao_sob_mouse(m, (float)ev.mouse.x, (float)ev.mouse.y);
            if (botao != BOTAO_NENHUM) {
                selecionado = botao;
                if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev.mouse.button == 1)
                    return (selecionado == GAMEOVER_BOTAO_REJOGAR)
                    ? GAMEOVER_JOGAR_NOVAMENTE : GAMEOVER_MENU_PRINCIPAL;
            }
        }
        else if (ev.type == ALLEGRO_EVENT_TIMER) {
            desenhar_gameover(m, motivo, score, selecionado);
        }
    }
}

static bool menu_inicializar(Menu* menu, ALLEGRO_DISPLAY* display, int recorde, ALLEGRO_TIMER** timer) {
    menu->display = display;
    menu->fonte = al_create_builtin_font();
    menu->fundo = al_load_bitmap("menu_fundo.png");
    menu->fila = al_create_event_queue();
    menu->recorde = recorde;
    *timer = al_create_timer(1.0 / FPS_MENU);

    if (!menu->fundo) fprintf(stderr, "menu: nao achei menu_fundo.png\n");
    if (!menu->fonte || !menu->fila || !*timer) return false;

    al_register_event_source(menu->fila, al_get_display_event_source(display));
    al_register_event_source(menu->fila, al_get_keyboard_event_source());
    al_register_event_source(menu->fila, al_get_mouse_event_source());
    al_register_event_source(menu->fila, al_get_timer_event_source(*timer));
    return true;
}

static void menu_finalizar(Menu* menu, ALLEGRO_TIMER* timer) {
    if (timer) al_destroy_timer(timer);
    if (menu->fila) al_destroy_event_queue(menu->fila);
    if (menu->fundo) al_destroy_bitmap(menu->fundo);
    if (menu->fonte) al_destroy_font(menu->fonte);
}

int menu_executar(ALLEGRO_DISPLAY* display, int recorde) {
    int resultado = MENU_SAIR;
    Menu menu = { 0 };
    ALLEGRO_TIMER* timer = NULL;

    if (menu_inicializar(&menu, display, recorde, &timer)) {
        al_start_timer(timer);
        resultado = rodar_menu(&menu);
        al_stop_timer(timer);
    }
    menu_finalizar(&menu, timer);
    return resultado;
}

GameOverOpcao menu_gameover_executar(ALLEGRO_DISPLAY* display, GameOverMotivo motivo,
    int score, int recorde) {
    GameOverOpcao resultado = GAMEOVER_SAIR;
    Menu menu = { 0 };
    ALLEGRO_TIMER* timer = NULL;

    if (menu_inicializar(&menu, display, recorde, &timer)) {
        al_start_timer(timer);
        resultado = rodar_gameover(&menu, motivo, score);
        al_stop_timer(timer);
    }
    menu_finalizar(&menu, timer);
    return resultado;
}
