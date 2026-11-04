#include "jogador.h"
#include "jogo.h"
#include <math.h>
#include <stdio.h>

#define JOGADOR_LARGURA 100
#define JOGADOR_ALTURA 133
#define JOGADOR_X_INICIAL 445
#define JOGADOR_VELOCIDADE 5
#define COMPRIMENTO_ARMA 35
#define ESPESSURA_ARMA 8

// Layout do sprite sheet (8 colunas x 4 linhas)
#define SHEET_COLUNAS 8
#define SHEET_LINHAS 4

// Recorte fixo por linha da sprite sheet (união de todos os frames da linha)
static int bbox_linha[SHEET_LINHAS][4]; // x, y, w, h relativos à célula
static float escala_sprite = 1.0f;

// Diz se o pixel é parte do personagem (não é fundo)
static bool pixel_e_personagem(ALLEGRO_COLOR cor, ALLEGRO_COLOR fundo, bool fundo_transparente) {
    float r, g, b, a, fr, fg, fb, fa;
    al_unmap_rgba_f(cor, &r, &g, &b, &a);
    if (a < 0.05f) return false;
    if (fundo_transparente) return true;
    al_unmap_rgba_f(fundo, &fr, &fg, &fb, &fa);
    float d = fabsf(r - fr) + fabsf(g - fg) + fabsf(b - fb);
    return d > 0.25f;
}

static void calcular_bboxes(Jogador* jogador) {
    ALLEGRO_BITMAP* bmp = jogador->sprite_sheet;
    int cell_w = al_get_bitmap_width(bmp) / SHEET_COLUNAS;
    int cell_h = al_get_bitmap_height(bmp) / SHEET_LINHAS;

    al_lock_bitmap(bmp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);

    for (int l = 0; l < SHEET_LINHAS; l++) {
        int minx = cell_w, miny = cell_h, maxx = -1, maxy = -1;

        for (int c = 0; c < SHEET_COLUNAS; c++) {
            // cor de fundo = pixel do canto da célula
            ALLEGRO_COLOR fundo = al_get_pixel(bmp, c * cell_w + 1, l * cell_h + 1);
            float fr, fg, fb, fa;
            al_unmap_rgba_f(fundo, &fr, &fg, &fb, &fa);
            bool fundo_transparente = (fa < 0.05f);

            for (int y = 0; y < cell_h; y++) {
                for (int x = 0; x < cell_w; x++) {
                    ALLEGRO_COLOR cor = al_get_pixel(bmp, c * cell_w + x, l * cell_h + y);
                    if (pixel_e_personagem(cor, fundo, fundo_transparente)) {
                        if (x < minx) minx = x;
                        if (x > maxx) maxx = x;
                        if (y < miny) miny = y;
                        if (y > maxy) maxy = y;
                    }
                }
            }
        }

        if (maxx < 0) { minx = 0; miny = 0; maxx = cell_w - 1; maxy = cell_h - 1; }
        bbox_linha[l][0] = minx;
        bbox_linha[l][1] = miny;
        bbox_linha[l][2] = maxx - minx + 1;
        bbox_linha[l][3] = maxy - miny + 1;

        printf("Linha %d: x=%d y=%d w=%d h=%d (celula %dx%d)\n",
            l, minx, miny, bbox_linha[l][2], bbox_linha[l][3], cell_w, cell_h);
    }

    al_unlock_bitmap(bmp);

    // Escala única: a altura do personagem parado (linha 0) = altura do jogador
    escala_sprite = (float)jogador->altura / (float)bbox_linha[0][3];
}

void jogador_inicializar(Jogador* jogador) {
    jogador->largura = JOGADOR_LARGURA;
    jogador->altura = JOGADOR_ALTURA;
    jogador->x = JOGADOR_X_INICIAL;
    jogador->y = CHAO_Y - jogador->altura;

    jogador->velocidade = JOGADOR_VELOCIDADE;

    jogador->velocidade_y = 0;
    jogador->gravidade = 0.5f;
    jogador->forca_pulo = -14.0f;

    jogador->no_chao = false;
    jogador->espaco_anterior = false;

    jogador->sprite_sheet = al_load_bitmap("gladiador.png");
    if (!jogador->sprite_sheet) {
        printf("ERRO: Nao foi possivel carregar gladiador.png\n");
    }
    else {
        calcular_bboxes(jogador);
    }

    jogador->frame_atual = 0;
    jogador->tempo_animacao = 0;
    jogador->atirando = false;
    jogador->olhando_direita = true;
    jogador->andando = false;
}

void jogador_atualizar(Jogador* jogador, const ALLEGRO_KEYBOARD_STATE* estado_teclado, float mouse_x, float mouse_y) {
    float movimento_x = 0;

    if (al_key_down(estado_teclado, ALLEGRO_KEY_A)) {
        movimento_x -= 1;
        jogador->olhando_direita = false;
        jogador->andando = true;
    }
    else if (al_key_down(estado_teclado, ALLEGRO_KEY_D)) {
        movimento_x += 1;
        jogador->olhando_direita = true;
        jogador->andando = true;
    }
    else {
        jogador->andando = false;
    }

    jogador->x += movimento_x * jogador->velocidade;

    bool espaco = al_key_down(estado_teclado, ALLEGRO_KEY_SPACE);
    if (espaco && !jogador->espaco_anterior && jogador->no_chao) {
        jogador->velocidade_y = jogador->forca_pulo;
        jogador->no_chao = false;
        jogador->frame_atual = 0;
    }
    jogador->espaco_anterior = espaco;

    jogador->velocidade_y += jogador->gravidade;
    jogador->y += jogador->velocidade_y;

    float chao = CHAO_Y - jogador->altura;
    if (jogador->y >= chao) {
        jogador->y = chao;
        jogador->velocidade_y = 0;
        jogador->no_chao = true;
    }
    else {
        jogador->no_chao = false;
    }

    if (jogador->x < 0) jogador->x = 0;
    if (jogador->x + jogador->largura > LARGURA_TELA) jogador->x = LARGURA_TELA - jogador->largura;

    // --- LÓGICA DE ANIMAÇÃO ---
    jogador->tempo_animacao += 1.0f / 60.0f;

    int total_frames = 1;
    float velocidade_anim = 0.1f;

    if (jogador->atirando) {
        total_frames = 4;
        velocidade_anim = 0.08f;
    }
    else if (!jogador->no_chao) {
        total_frames = 7;
        velocidade_anim = 0.12f;
    }
    else if (jogador->andando) {
        total_frames = 8;
        velocidade_anim = 0.10f;
    }
    else {
        total_frames = 6;
        velocidade_anim = 0.60f;
    }

    if (jogador->tempo_animacao >= velocidade_anim) {
        jogador->tempo_animacao = 0;
        jogador->frame_atual++;

        if (jogador->frame_atual >= total_frames) {
            jogador->frame_atual = 0;
            jogador->atirando = false;
        }
    }
}

void jogador_desenhar(const Jogador* jogador) {
    if (!jogador->sprite_sheet) return;

    int coluna = jogador->frame_atual;
    int linha = 0;

    if (jogador->atirando) {
        if (!jogador->no_chao) linha = 3;
        else if (jogador->andando) linha = 3;
        else linha = 1;
    }
    else {
        if (!jogador->no_chao) linha = 3;
        else if (jogador->andando) linha = 2;
        else linha = 0;
    }

    if (linha == 0 && coluna > 5) coluna = 0;
    if (coluna > 7) coluna = 0;

    int cell_w = al_get_bitmap_width(jogador->sprite_sheet) / SHEET_COLUNAS;
    int cell_h = al_get_bitmap_height(jogador->sprite_sheet) / SHEET_LINHAS;

    // Recorte fixo da linha (igual para todos os frames: sem tremer)
    float sx = coluna * cell_w + bbox_linha[linha][0];
    float sy = linha * cell_h + bbox_linha[linha][1];
    float sw = bbox_linha[linha][2];
    float sh = bbox_linha[linha][3];

    // Mesma escala para todas as animações, mantendo a proporção
    float dw = sw * escala_sprite;
    float dh = sh * escala_sprite;

    // Centralizado na caixa do jogador, com os pés no chão
    float dx = jogador->x + (jogador->largura - dw) / 2.0f;
    float dy = jogador->y + jogador->altura - dh;

    int flags = jogador->olhando_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;

    al_draw_scaled_bitmap(jogador->sprite_sheet, sx, sy, sw, sh, dx, dy, dw, dh, flags);
}

void jogador_desenhar_arma(const Jogador* jogador, float mouse_x, float mouse_y) {
    // Vazio
}

float jogador_centro_x(const Jogador* jogador) {
    return jogador->x + jogador->largura / 2;
}

float jogador_centro_y(const Jogador* jogador) {
    return jogador->y + jogador->altura / 2;
}

bool jogador_colidiu_com_inimigo(const Jogador* jogador, const Inimigo* inimigo) {
    return jogador->x < inimigo->x + inimigo->tamanho &&
        jogador->x + jogador->largura > inimigo->x &&
        jogador->y < inimigo->y + inimigo->tamanho &&
        jogador->y + jogador->altura > inimigo->y;
}

void jogador_finalizar(Jogador* jogador) {
    if (jogador->sprite_sheet) {
        al_destroy_bitmap(jogador->sprite_sheet);
    }
}