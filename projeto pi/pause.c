#include "pause.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <math.h>

#define TOTAL_OPCOES 3
#define BOTAO_LARG 420.0f
#define BOTAO_ALT 56.0f
#define BOTAO_PASSO 72.0f

// Fonte pixelada: a fonte embutida do Allegro (8x8), ampliada sem suavizar
static ALLEGRO_FONT* fonte_pixel = NULL;
static ALLEGRO_BITMAP* tmp_texto = NULL;

static const char* TEXTOS_OPCOES[TOTAL_OPCOES] = {
    "CONTINUAR",
    "RECOMECAR",
    "VOLTAR AO MENU"
};

static bool estava_ativo = false;
static double t_abertura = 0.0;

// Cor com alpha pre-multiplicado (o blender padrao do Allegro espera isso)
static ALLEGRO_COLOR cor(int r, int g, int b, float a) {
    return al_map_rgba_f(r / 255.0f * a, g / 255.0f * a, b / 255.0f * a, a);
}

// Escreve texto centralizado, ampliado 'esc' vezes (efeito pixel art)
static void texto_pixel(ALLEGRO_COLOR c, float cx, float y, int esc, const char* txt) {
    int w = al_get_text_width(fonte_pixel, txt);
    int h = al_get_font_line_height(fonte_pixel);

    if (!tmp_texto) {
        int flags = al_get_new_bitmap_flags();
        al_set_new_bitmap_flags(flags & ~(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR));
        tmp_texto = al_create_bitmap(512, 16);
        al_set_new_bitmap_flags(flags);
    }

    // desenha o texto pequeno no bitmap temporario...
    ALLEGRO_BITMAP* alvo = al_get_target_bitmap();
    al_set_target_bitmap(tmp_texto);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));
    al_draw_text(fonte_pixel, al_map_rgb(255, 255, 255), 0, 0, 0, txt);
    al_set_target_bitmap(alvo);

    // ...e amplia na tela com a cor desejada
    al_draw_tinted_scaled_bitmap(tmp_texto, c, 0, 0, w, h,
        floorf(cx - w * esc / 2.0f), floorf(y), (float)(w * esc), (float)(h * esc), 0);
}

// Posicao do botao i (usada no desenho e no mouse)
static void posicao_botao(int i, int largura_tela, int altura_tela, float* x, float* y) {
    *x = largura_tela / 2.0f - BOTAO_LARG / 2.0f;
    *y = altura_tela / 2.0f - 10 + i * BOTAO_PASSO;
}

void pause_inicializar(MenuPause* pause) {
    pause->ativo = false;
    pause->opcao = 0;

    if (!fonte_pixel) fonte_pixel = al_create_builtin_font();
}

PauseAcao pause_processar_evento(MenuPause* pause, ALLEGRO_EVENT* evento, int largura_tela, int altura_tela) {

    if (evento->type == ALLEGRO_EVENT_KEY_DOWN) {
        int tecla = evento->keyboard.keycode;

        if (tecla == ALLEGRO_KEY_ESCAPE) {
            if (pause->ativo) {
                pause->ativo = false;
                return PAUSE_CONTINUAR;
            }
            pause->ativo = true;
            pause->opcao = 0;
            return PAUSE_NENHUMA_ACAO;
        }

        if (!pause->ativo)
            return PAUSE_NENHUMA_ACAO;

        if (tecla == ALLEGRO_KEY_UP || tecla == ALLEGRO_KEY_W) {
            pause->opcao = (pause->opcao + TOTAL_OPCOES - 1) % TOTAL_OPCOES;
        }
        else if (tecla == ALLEGRO_KEY_DOWN || tecla == ALLEGRO_KEY_S) {
            pause->opcao = (pause->opcao + 1) % TOTAL_OPCOES;
        }
        else if (tecla == ALLEGRO_KEY_ENTER || tecla == ALLEGRO_KEY_SPACE) {
            pause->ativo = false;
            if (pause->opcao == 0) return PAUSE_CONTINUAR;
            if (pause->opcao == 1) return PAUSE_RECOMECAR;
            if (pause->opcao == 2) return PAUSE_MENU_PRINCIPAL;
        }
    }

    if (pause->ativo &&
        (evento->type == ALLEGRO_EVENT_MOUSE_AXES ||
            evento->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)) {

        float mx = (float)evento->mouse.x;
        float my = (float)evento->mouse.y;

        for (int i = 0; i < TOTAL_OPCOES; i++) {
            float bx, by;
            posicao_botao(i, largura_tela, altura_tela, &bx, &by);

            if (mx >= bx && mx <= bx + BOTAO_LARG && my >= by && my <= by + BOTAO_ALT) {
                pause->opcao = i;

                if (evento->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && evento->mouse.button == 1) {
                    pause->ativo = false;
                    if (i == 0) return PAUSE_CONTINUAR;
                    if (i == 1) return PAUSE_RECOMECAR;
                    if (i == 2) return PAUSE_MENU_PRINCIPAL;
                }
                break;
            }
        }
    }

    return PAUSE_NENHUMA_ACAO;
}

void pause_desenhar(MenuPause* pause, ALLEGRO_FONT* fonte_padrao, int largura_tela, int altura_tela) {
    (void)fonte_padrao;

    if (!pause->ativo) {
        estava_ativo = false;
        return;
    }

    // fade-in rapido ao abrir
    double agora = al_get_time();
    if (!estava_ativo) {
        estava_ativo = true;
        t_abertura = agora;
    }
    float fade = (float)((agora - t_abertura) / 0.2);
    if (fade > 1.0f) fade = 1.0f;

    float cx = largura_tela / 2.0f;
    float cy = altura_tela / 2.0f;

    // Escurece o proprio jogo (o cenario continua aparecendo atras)
    al_draw_filled_rectangle(0, 0, (float)largura_tela, (float)altura_tela, cor(10, 6, 4, 0.65f * fade));

    // Titulo com sombra
    texto_pixel(cor(0, 0, 0, 0.8f * fade), cx + 4, cy - 150 + 4, 5, "JOGO PAUSADO");
    texto_pixel(cor(240, 230, 200, fade), cx, cy - 150, 5, "JOGO PAUSADO");

    // Botoes na paleta do escudo: dourado normal, vermelho quando selecionado
    for (int i = 0; i < TOTAL_OPCOES; i++) {
        float bx, by;
        posicao_botao(i, largura_tela, altura_tela, &bx, &by);
        bool sel = (i == pause->opcao);
        float x2 = bx + BOTAO_LARG, y2 = by + BOTAO_ALT;

        if (sel) {
            al_draw_filled_rectangle(bx, by, x2, y2, cor(190, 30, 35, fade));            // vermelho
            al_draw_filled_rectangle(bx, by, x2, by + 4, cor(235, 85, 70, fade));        // luz
            al_draw_filled_rectangle(bx, y2 - 4, x2, y2, cor(120, 15, 20, fade));        // sombra
        }
        else {
            al_draw_filled_rectangle(bx, by, x2, y2, cor(212, 165, 45, fade));           // dourado
            al_draw_filled_rectangle(bx, by, x2, by + 4, cor(248, 212, 100, fade));      // luz
            al_draw_filled_rectangle(bx, y2 - 4, x2, y2, cor(150, 105, 20, fade));       // sombra
        }
        al_draw_rectangle(bx, by, x2, y2, cor(35, 28, 40, fade), 2);                     // contorno escuro

        float ty = by + (BOTAO_ALT - 24) / 2.0f;
        if (sel)
            texto_pixel(cor(255, 235, 170, fade), cx, ty, 3, TEXTOS_OPCOES[i]);
        else
            texto_pixel(cor(45, 30, 20, fade), cx, ty, 3, TEXTOS_OPCOES[i]);
    }

    // Dica discreta no rodape
    texto_pixel(cor(200, 190, 165, 0.6f * fade), cx, altura_tela - 40, 2, "W/S: ESCOLHER   ENTER: OK   ESC: VOLTAR");
}