#include "tela.h"
#include "jogo.h"

typedef struct {
    float escala;
    float off_x;
    float off_y;
    int largura;
    int altura;
} Janela;

// Calcula a escala e o deslocamento pra manter a proporcao do jogo centralizada na janela
static Janela calcular_janela(ALLEGRO_DISPLAY* display) {
    Janela j;
    j.largura = al_get_display_width(display);
    j.altura = al_get_display_height(display);

    float escala_x = (float)j.largura / LARGURA_TELA;
    float escala_y = (float)j.altura / ALTURA_TELA;
    j.escala = (escala_x < escala_y) ? escala_x : escala_y;

    j.off_x = (j.largura - LARGURA_TELA * j.escala) / 2.0f;
    j.off_y = (j.altura - ALTURA_TELA * j.escala) / 2.0f;
    return j;
}

void tela_alternar_tela_cheia(ALLEGRO_DISPLAY* display) {
    bool cheia = (al_get_display_flags(display) & ALLEGRO_FULLSCREEN_WINDOW) != 0;
    al_toggle_display_flag(display, ALLEGRO_FULLSCREEN_WINDOW, !cheia);
}

void tela_preparar_frame(ALLEGRO_DISPLAY* display) {
    Janela j = calcular_janela(display);

    // limpa a janela inteira, inclusive as barras pretas dos lados
    al_set_clipping_rectangle(0, 0, j.largura, j.altura);
    al_clear_to_color(al_map_rgb(0, 0, 0));

    ALLEGRO_TRANSFORM t;
    al_identity_transform(&t);
    al_scale_transform(&t, j.escala, j.escala);
    al_translate_transform(&t, j.off_x, j.off_y);
    al_use_transform(&t);

    // o clipping nao sofre a transformacao, entao precisa ser calculado na mao
    al_set_clipping_rectangle((int)j.off_x, (int)j.off_y, (int)(LARGURA_TELA * j.escala + 0.5f), (int)(ALTURA_TELA * j.escala + 0.5f));
}

void tela_mouse_para_jogo(ALLEGRO_DISPLAY* display, float* x, float* y) {
    Janela j = calcular_janela(display);
    if (j.escala <= 0.0f) return;

    *x = (*x - j.off_x) / j.escala;
    *y = (*y - j.off_y) / j.escala;
}