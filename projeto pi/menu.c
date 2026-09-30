/* menu.c - Menu principal de "Gladiadores do Calculo" (Allegro 5, C)
   Controles: SETA CIMA/BAIXO (ou W/S) para escolher, ENTER/ESPACO para confirmar,
              ESC para sair, ou use o MOUSE (passar por cima e clicar). */
#include "menu.h"

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

              /* O menu e desenhado em 960x720 e escalado para caber na janela */
#define LARGURA_LOGICA 960
#define ALTURA_LOGICA  720

#define NUM_BOTOES   3
#define ESCALA_TEXTO 3     /* tamanho das letras dos botoes */

/* ---- Edite aqui os nomes da equipe (sem acentos: a fonte embutida nao tem) ---- */
static const char* LINHAS_CREDITOS[] = {
    "GLADIADORES DO CALCULO",
    "",
    "EQUIPE",
    "NOME DO INTEGRANTE 1",
    "NOME DO INTEGRANTE 2",
    "NOME DO INTEGRANTE 3",
    "",
    "PRESSIONE QUALQUER TECLA"
};
#define NUM_CREDITOS ((int)(sizeof(LINHAS_CREDITOS) / sizeof(LINHAS_CREDITOS[0])))

typedef struct {
    const char* texto;
    float         x, y, w, h;
    ALLEGRO_BITMAP* rotulo;
} Botao;

/* Cores da paleta do jogo */
#define COR_VERMELHO   al_map_rgb(150, 22, 42)
#define COR_OURO       al_map_rgb(214, 150, 50)
#define COR_OURO_CLARO al_map_rgb(248, 208, 100)
#define COR_OURO_ESCURO al_map_rgb(140, 88, 28)
#define COR_MARROM     al_map_rgb(44, 26, 18)
#define COR_CONTORNO   al_map_rgb(24, 14, 16)

/* Desenha um texto (fonte embutida, 8x8) num bitmap branco para podermos escalar/colorir */
static ALLEGRO_BITMAP* criar_rotulo(ALLEGRO_FONT* fonte, const char* txt)
{
    int w = al_get_text_width(fonte, txt);
    int h = al_get_font_line_height(fonte);
    ALLEGRO_BITMAP* rotulo, * anterior;

    if (w < 1) w = 1;
    rotulo = al_create_bitmap(w, h);
    if (!rotulo) return NULL;

    anterior = al_get_target_bitmap();
    al_set_target_bitmap(rotulo);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));
    al_draw_text(fonte, al_map_rgb(255, 255, 255), 0, 0, 0, txt);
    al_set_target_bitmap(anterior);
    return rotulo;
}

/* Desenha o rotulo centralizado em cx, com a escala e cor pedidas */
static void desenhar_rotulo(ALLEGRO_BITMAP* r, float cx, float y, int escala, ALLEGRO_COLOR cor)
{
    float w, h;
    if (!r) return;
    w = (float)al_get_bitmap_width(r);
    h = (float)al_get_bitmap_height(r);
    al_draw_tinted_scaled_bitmap(r, cor, 0, 0, w, h,
        floorf(cx - w * escala / 2.0f), floorf(y),
        w * escala, h * escala, 0);
}

static void desenhar_botao(const Botao* b, bool selecionado, double tempo)
{
    ALLEGRO_COLOR fundo = selecionado ? COR_VERMELHO : COR_MARROM;
    ALLEGRO_COLOR borda = selecionado ? COR_OURO_CLARO : COR_OURO_ESCURO;
    ALLEGRO_COLOR texto = selecionado ? COR_OURO_CLARO : COR_OURO;
    float altura_texto = 8.0f * ESCALA_TEXTO;

    /* sombra */
    al_draw_filled_rectangle(b->x + 4, b->y + 4, b->x + b->w + 4, b->y + b->h + 4, COR_CONTORNO);
    /* corpo + bordas (dupla, estilo pixel art) */
    al_draw_filled_rectangle(b->x, b->y, b->x + b->w, b->y + b->h, fundo);
    al_draw_rectangle(b->x + 2, b->y + 2, b->x + b->w - 2, b->y + b->h - 2, borda, 4);
    al_draw_rectangle(b->x, b->y, b->x + b->w, b->y + b->h, COR_CONTORNO, 2);

    desenhar_rotulo(b->rotulo, b->x + b->w / 2, b->y + (b->h - altura_texto) / 2, ESCALA_TEXTO, texto);

    if (selecionado) {
        /* setinha dourada que "balanca" ao lado do botao selecionado */
        float mov = (float)floor(sin(tempo * 7.0) * 3.0);
        float ax = b->x - 26 + mov, ay = b->y + b->h / 2;
        al_draw_filled_triangle(ax, ay - 12, ax, ay + 12, ax + 18, ay, COR_OURO_CLARO);
        al_draw_triangle(ax, ay - 12, ax, ay + 12, ax + 18, ay, COR_CONTORNO, 2);
    }
}

static bool dentro(const Botao* b, float mx, float my)
{
    return mx >= b->x && mx <= b->x + b->w && my >= b->y && my <= b->y + b->h;
}

int menu_executar(ALLEGRO_DISPLAY* display)
{
    static const char* textos[NUM_BOTOES] = { "JOGAR", "CREDITOS", "SAIR" };
    Botao botoes[NUM_BOTOES];
    ALLEGRO_BITMAP* rotulos_creditos[16] = { NULL };
    ALLEGRO_BITMAP* fundo = NULL, * rotulo_dica = NULL;
    ALLEGRO_FONT* fonte = NULL;
    ALLEGRO_EVENT_QUEUE* fila = NULL;
    ALLEGRO_TIMER* timer = NULL;
    int novas_flags;
    int i;
    int selecionado = 0;
    int resultado = MENU_SAIR;
    bool terminou = false, redesenhar = true, mostrando_creditos = false;
    double inicio;

    /* inicializacoes (pode chamar varias vezes sem problema) */
    al_init_image_addon();
    al_init_primitives_addon();
    al_init_font_addon();
    al_install_keyboard();
    al_install_mouse();
    al_set_target_backbuffer(display);

    fonte = al_create_builtin_font();
    if (!fonte) { fprintf(stderr, "menu: nao consegui criar a fonte\n"); return MENU_SAIR; }

    /* fundo com filtro suave (fica bonito se a janela for maior que 960x720) */
    novas_flags = al_get_new_bitmap_flags();
    al_add_new_bitmap_flag(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
    fundo = al_load_bitmap("menu_fundo.png");
    al_set_new_bitmap_flags(novas_flags);
    if (!fundo) fprintf(stderr, "menu: nao achei menu_fundo.png (coloque na pasta do projeto)\n");

    /* botoes */
    for (i = 0; i < NUM_BOTOES; i++) {
        botoes[i].texto = textos[i];
        botoes[i].x = 590; botoes[i].y = 450.0f + 70.0f * i;
        botoes[i].w = 290; botoes[i].h = 54;
        botoes[i].rotulo = criar_rotulo(fonte, textos[i]);
    }
    rotulo_dica = criar_rotulo(fonte, "SETAS OU MOUSE   ENTER PARA CONFIRMAR");
    for (i = 0; i < NUM_CREDITOS && i < 16; i++)
        rotulos_creditos[i] = criar_rotulo(fonte, LINHAS_CREDITOS[i]);

    fila = al_create_event_queue();
    timer = al_create_timer(1.0 / 60.0);
    al_register_event_source(fila, al_get_display_event_source(display));
    al_register_event_source(fila, al_get_keyboard_event_source());
    al_register_event_source(fila, al_get_mouse_event_source());
    al_register_event_source(fila, al_get_timer_event_source(timer));
    al_start_timer(timer);
    inicio = al_get_time();

    while (!terminou) {
        ALLEGRO_EVENT ev;
        bool confirmar = false;
        int dw = al_get_display_width(display), dh = al_get_display_height(display);
        float esc = fminf(dw / (float)LARGURA_LOGICA, dh / (float)ALTURA_LOGICA);
        float ox = (dw - LARGURA_LOGICA * esc) / 2.0f;
        float oy = (dh - ALTURA_LOGICA * esc) / 2.0f;

        al_wait_for_event(fila, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            resultado = MENU_SAIR; terminou = true;
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            int k = ev.keyboard.keycode;
            if (mostrando_creditos) {
                mostrando_creditos = false;
            }
            else if (k == ALLEGRO_KEY_UP || k == ALLEGRO_KEY_W) {
                selecionado = (selecionado + NUM_BOTOES - 1) % NUM_BOTOES;
            }
            else if (k == ALLEGRO_KEY_DOWN || k == ALLEGRO_KEY_S) {
                selecionado = (selecionado + 1) % NUM_BOTOES;
            }
            else if (k == ALLEGRO_KEY_ENTER || k == ALLEGRO_KEY_PAD_ENTER || k == ALLEGRO_KEY_SPACE) {
                confirmar = true;
            }
            else if (k == ALLEGRO_KEY_ESCAPE) {
                resultado = MENU_SAIR; terminou = true;
            }
        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES || ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            float mx = (ev.mouse.x - ox) / esc;
            float my = (ev.mouse.y - oy) / esc;
            if (mostrando_creditos) {
                if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) mostrando_creditos = false;
            }
            else {
                for (i = 0; i < NUM_BOTOES; i++) {
                    if (dentro(&botoes[i], mx, my)) {
                        selecionado = i;
                        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev.mouse.button == 1)
                            confirmar = true;
                    }
                }
            }
        }
        else if (ev.type == ALLEGRO_EVENT_TIMER) {
            redesenhar = true;
        }

        if (confirmar) {
            if (selecionado == MENU_JOGAR) { resultado = MENU_JOGAR; terminou = true; }
            else if (selecionado == MENU_CREDITOS) { mostrando_creditos = true; }
            else { resultado = MENU_SAIR;  terminou = true; }
        }

        if (redesenhar && al_is_event_queue_empty(fila)) {
            ALLEGRO_TRANSFORM t;
            double tempo = al_get_time() - inicio;
            float fade = 1.0f - (float)(tempo / 0.6);   /* entrada suave */
            redesenhar = false;

            al_identity_transform(&t);
            al_use_transform(&t);
            al_clear_to_color(al_map_rgb(0, 0, 0));

            al_identity_transform(&t);
            al_scale_transform(&t, esc, esc);
            al_translate_transform(&t, ox, oy);
            al_use_transform(&t);

            if (fundo)
                al_draw_scaled_bitmap(fundo, 0, 0, (float)al_get_bitmap_width(fundo), (float)al_get_bitmap_height(fundo),
                    0, 0, LARGURA_LOGICA, ALTURA_LOGICA, 0);
            else
                al_draw_filled_rectangle(0, 0, LARGURA_LOGICA, ALTURA_LOGICA, al_map_rgb(120, 70, 30));

            for (i = 0; i < NUM_BOTOES; i++)
                desenhar_botao(&botoes[i], i == selecionado, tempo);

            desenhar_rotulo(rotulo_dica, LARGURA_LOGICA / 2.0f, 686, 2, al_map_rgb(214, 196, 160));

            if (mostrando_creditos) {
                float y0 = 190;
                al_draw_filled_rectangle(0, 0, LARGURA_LOGICA, ALTURA_LOGICA, al_map_rgba_f(0, 0, 0, 0.7f));
                al_draw_filled_rectangle(170, y0, 790, 590, COR_MARROM);
                al_draw_rectangle(176, y0 + 6, 784, 584, COR_OURO_ESCURO, 4);
                al_draw_rectangle(170, y0, 790, 590, COR_OURO_CLARO, 3);
                for (i = 0; i < NUM_CREDITOS && i < 16; i++) {
                    bool titulo = (i == 0);
                    bool destaque = (i == 2);
                    desenhar_rotulo(rotulos_creditos[i], LARGURA_LOGICA / 2.0f, y0 + 34 + i * 44,
                        titulo ? 3 : 2, (titulo || destaque) ? COR_OURO_CLARO : al_map_rgb(230, 215, 190));
                }
            }

            if (fade > 0)
                al_draw_filled_rectangle(0, 0, LARGURA_LOGICA, ALTURA_LOGICA, al_map_rgba_f(0, 0, 0, fade));

            al_flip_display();
        }
    }

    /* limpeza + volta a transformacao normal para o resto do jogo */
    {
        ALLEGRO_TRANSFORM t;
        al_identity_transform(&t);
        al_use_transform(&t);
    }
    for (i = 0; i < NUM_BOTOES; i++) if (botoes[i].rotulo) al_destroy_bitmap(botoes[i].rotulo);
    for (i = 0; i < 16; i++) if (rotulos_creditos[i]) al_destroy_bitmap(rotulos_creditos[i]);
    if (rotulo_dica) al_destroy_bitmap(rotulo_dica);
    if (fundo) al_destroy_bitmap(fundo);
    al_destroy_font(fonte);
    al_destroy_timer(timer);
    al_destroy_event_queue(fila);

    return resultado;
}