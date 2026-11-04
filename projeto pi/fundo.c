#include "fundo.h"
#include "jogo.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>

#define TOTAL_FRAMES 100
#define FPS_FUNDO 30.0

static ALLEGRO_BITMAP* frame_atual = NULL;
static int numero_frame = 0;
static int direcao_frame = 1;   // 1 = avancando, -1 = voltando
static double ultimo_frame = 0;

// O numero vai de 0 a TOTAL_FRAMES - 1, mas os arquivos comecam em frame_0001
static ALLEGRO_BITMAP* carregar_frame(int numero) {
    char nome_arquivo[100];
    snprintf(nome_arquivo, sizeof(nome_arquivo), "fundo_frames/frame_%04d.jpg", numero + 1);
    return al_load_bitmap(nome_arquivo);
}

// O addon de imagens ja e iniciado no main
bool fundo_inicializar(void) {
    frame_atual = carregar_frame(0);
    if (!frame_atual) return false;

    numero_frame = 0;
    direcao_frame = 1;
    ultimo_frame = al_get_time();
    return true;
}

void fundo_atualizar(void) {
    double agora = al_get_time();
    if (agora - ultimo_frame < 1.0 / FPS_FUNDO) return;
    ultimo_frame = agora;

    numero_frame += direcao_frame;

    // vai e volta pra animacao nao dar um salto quando chega no fim
    if (numero_frame >= TOTAL_FRAMES - 1) {
        numero_frame = TOTAL_FRAMES - 1;
        direcao_frame = -1;
    }
    else if (numero_frame <= 0) {
        numero_frame = 0;
        direcao_frame = 1;
    }

    // se o arquivo falhar, fica o frame antigo na tela em vez de ficar sem fundo
    ALLEGRO_BITMAP* novo_frame = carregar_frame(numero_frame);
    if (!novo_frame) return;

    al_destroy_bitmap(frame_atual);
    frame_atual = novo_frame;
}

void fundo_desenhar(void) {
    if (!frame_atual) return;
    al_draw_scaled_bitmap(frame_atual, 0, 0, al_get_bitmap_width(frame_atual), al_get_bitmap_height(frame_atual), 0, 0, LARGURA_TELA, ALTURA_TELA, 0);
}

void fundo_finalizar(void) {
    al_destroy_bitmap(frame_atual);
    frame_atual = NULL;
    numero_frame = 0;
    direcao_frame = 1;
    ultimo_frame = 0;
}