#include "jogador.h"
#include "jogo.h"
#include <math.h>

#define JOGADOR_TAMANHO 50
#define JOGADOR_X_INICIAL 445
#define JOGADOR_VELOCIDADE 5
#define COMPRIMENTO_ARMA 35
#define ESPESSURA_ARMA 8

void jogador_inicializar(Jogador* jogador) {
    jogador->x = JOGADOR_X_INICIAL;
    jogador->y = CHAO_Y - 100;
    jogador->tamanho = JOGADOR_TAMANHO;
    jogador->velocidade = JOGADOR_VELOCIDADE;

    jogador->velocidade_y = 0;
    jogador->gravidade = 0.5f;
    jogador->forca_pulo = -12.0f;

    jogador->no_chao = false;
    jogador->espaco_anterior = false;
}

void jogador_atualizar(Jogador* jogador, const ALLEGRO_KEYBOARD_STATE* estado_teclado) {
    float movimento_x = 0;
    if (al_key_down(estado_teclado, ALLEGRO_KEY_A)) movimento_x -= 1;
    if (al_key_down(estado_teclado, ALLEGRO_KEY_D)) movimento_x += 1;
    jogador->x += movimento_x * jogador->velocidade;

    // segurar espaco nao fica pulando, so conta o aperto novo
    bool espaco = al_key_down(estado_teclado, ALLEGRO_KEY_SPACE);
    if (espaco && !jogador->espaco_anterior && jogador->no_chao) {
        jogador->velocidade_y = jogador->forca_pulo;
        jogador->no_chao = false;
    }
    jogador->espaco_anterior = espaco;

    jogador->velocidade_y += jogador->gravidade;
    jogador->y += jogador->velocidade_y;

    float chao = CHAO_Y - jogador->tamanho;
    if (jogador->y >= chao) {
        jogador->y = chao;
        jogador->velocidade_y = 0;
        jogador->no_chao = true;
    }
    else {
        jogador->no_chao = false;
    }

    // nao deixa sair da tela pelos lados
    if (jogador->x < 0) jogador->x = 0;
    if (jogador->x + jogador->tamanho > LARGURA_TELA) jogador->x = LARGURA_TELA - jogador->tamanho;
}

void jogador_desenhar(const Jogador* jogador) {
    al_draw_filled_rectangle(jogador->x, jogador->y, jogador->x + jogador->tamanho, jogador->y + jogador->tamanho, al_map_rgb(255, 255, 255));
}

void jogador_desenhar_arma(const Jogador* jogador, float mouse_x, float mouse_y) {
    float centro_x = jogador_centro_x(jogador);
    float centro_y = jogador_centro_y(jogador);
    float direcao_x = mouse_x - centro_x;
    float direcao_y = mouse_y - centro_y;
    float distancia = sqrtf(direcao_x * direcao_x + direcao_y * direcao_y);

    // mouse em cima do centro, nao tem pra onde apontar
    if (distancia < 0.001f) return;

    float ponta_x = centro_x + direcao_x / distancia * COMPRIMENTO_ARMA;
    float ponta_y = centro_y + direcao_y / distancia * COMPRIMENTO_ARMA;
    al_draw_line(centro_x, centro_y, ponta_x, ponta_y, al_map_rgb(150, 150, 150), ESPESSURA_ARMA);
}

float jogador_centro_x(const Jogador* jogador) {
    return jogador->x + jogador->tamanho / 2;
}

float jogador_centro_y(const Jogador* jogador) {
    return jogador->y + jogador->tamanho / 2;
}

// Os dois quadrados se tocam quando se sobrepoem nos dois eixos ao mesmo tempo
bool jogador_colidiu_com_inimigo(const Jogador* jogador, const Inimigo* inimigo) {
    return jogador->x < inimigo->x + inimigo->tamanho && jogador->x + jogador->tamanho > inimigo->x && jogador->y < inimigo->y + inimigo->tamanho && jogador->y + jogador->tamanho > inimigo->y;
}