#include "jogador.h"
#include <math.h>
#define LARGURA_TELA 800
#define ALTURA_TELA 535

void jogador_inicializar(Jogador* jogador)
{
    jogador->x = 445;
    jogador->y = ALTURA_TELA - 100;

    jogador->tamanho = 50;
    jogador->velocidade = 5;

    jogador->velocidade_y = 0;
    jogador->gravidade = 0.5f;
    jogador->forca_pulo = -12.0f;

    jogador->no_chao = 0;
    jogador->espaco_anterior = 0;
}

void jogador_atualizar( Jogador* jogador, ALLEGRO_KEYBOARD_STATE* estado_teclado){

    float movimento_x = 0;

    // Movimento horizontal
    if (al_key_down(estado_teclado, ALLEGRO_KEY_A)){
        movimento_x -= 1;
    }
    if (al_key_down(estado_teclado, ALLEGRO_KEY_D))
        movimento_x += 1;
        jogador->x += movimento_x * jogador->velocidade;

    // Pulo
    int espaco_pressionado = al_key_down(estado_teclado, ALLEGRO_KEY_SPACE);
       
    if (espaco_pressionado && !jogador->espaco_anterior && jogador->no_chao){
        jogador->velocidade_y = jogador->forca_pulo;
        jogador->no_chao = 0;
    }   jogador->espaco_anterior = espaco_pressionado;
    
    // Aplica gravidade
    jogador->velocidade_y += jogador->gravidade;
    // Atualiza possicao
    jogador->y += jogador->velocidade_y;
    // Chão da arena
    float chao = ALTURA_TELA - jogador->tamanho;
    if (jogador->y >= chao) {
        jogador->y = chao;
        jogador->velocidade_y = 0;
        jogador->no_chao = 1;
    }else{
        jogador->no_chao = 0;
    }
    // Limites horizontais
    if (jogador->x < 0) jogador->x = 0;

    if (jogador->x + jogador->tamanho > LARGURA_TELA){
        jogador->x = LARGURA_TELA - jogador->tamanho;
    }
}
void jogador_desenhar(Jogador* jogador)
{
    al_draw_filled_rectangle( jogador->x, jogador->y, jogador->x + jogador->tamanho, jogador->y + jogador->tamanho,  al_map_rgb(255, 255, 255));

}

void jogador_desenhar_arma( Jogador* jogador,float mouse_x, float mouse_y){

    float centro_x = jogador_centro_x(jogador);
    float centro_y = jogador_centro_y(jogador);

    float direcao_x = mouse_x - centro_x;
    float direcao_y = mouse_y - centro_y;

    float distancia = sqrtf(direcao_x * direcao_x + direcao_y * direcao_y);
    
    if (distancia < 0.001f) return;
    
    direcao_x /= distancia;
    direcao_y /= distancia;

    float arma_x = centro_x + direcao_x * 35;
    float arma_y = centro_y + direcao_y * 35;

    al_draw_line(
        centro_x, centro_y,
        arma_x, arma_y,
        al_map_rgb(150, 150, 150),
        8
    );
}

float jogador_centro_x(Jogador* jogador)
{
    return jogador->x + jogador->tamanho / 2;
}

float jogador_centro_y(Jogador* jogador)
{
    return jogador->y + jogador->tamanho / 2;
}