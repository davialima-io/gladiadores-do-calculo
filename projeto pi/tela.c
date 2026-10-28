#include "tela.h"
#include "jogo.h"

/* Calcula escala e deslocamento para manter a proporcao do jogo
   centralizada na janela atual. */
static void calcular(ALLEGRO_DISPLAY* display,
    float* escala, float* off_x, float* off_y,
    int* largura_janela, int* altura_janela)
{
    int w = al_get_display_width(display);
    int h = al_get_display_height(display);

    float esc_x = (float)w / LARGURA_TELA;
    float esc_y = (float)h / ALTURA_TELA;
    float esc = (esc_x < esc_y) ? esc_x : esc_y;

    *escala = esc;
    *off_x = (w - LARGURA_TELA * esc) / 2.0f;
    *off_y = (h - ALTURA_TELA * esc) / 2.0f;
    *largura_janela = w;
    *altura_janela = h;
}

void tela_alternar_tela_cheia(ALLEGRO_DISPLAY* display)
{
    bool cheia = (al_get_display_flags(display) & ALLEGRO_FULLSCREEN_WINDOW) != 0;
    al_toggle_display_flag(display, ALLEGRO_FULLSCREEN_WINDOW, !cheia);
}

void tela_preparar_frame(ALLEGRO_DISPLAY* display)
{
    float escala, off_x, off_y;
    int w, h;
    calcular(display, &escala, &off_x, &off_y, &w, &h);

    /* Limpa a janela inteira (barras pretas incluidas) */
    al_set_clipping_rectangle(0, 0, w, h);
    al_clear_to_color(al_map_rgb(0, 0, 0));

    /* Escala e centraliza o mundo do jogo */
    ALLEGRO_TRANSFORM t;
    al_identity_transform(&t);
    al_scale_transform(&t, escala, escala);
    al_translate_transform(&t, off_x, off_y);
    al_use_transform(&t);

    /* Nada fora da area do jogo e desenhado (o clipping nao sofre transformacao) */
    al_set_clipping_rectangle(
        (int)off_x, (int)off_y,
        (int)(LARGURA_TELA * escala + 0.5f),
        (int)(ALTURA_TELA * escala + 0.5f)
    );
}

void tela_mouse_para_jogo(ALLEGRO_DISPLAY* display, float* x, float* y)
{
    float escala, off_x, off_y;
    int w, h;
    calcular(display, &escala, &off_x, &off_y, &w, &h);

    if (escala <= 0.0f) return;

    *x = (*x - off_x) / escala;
    *y = (*y - off_y) / escala;
}