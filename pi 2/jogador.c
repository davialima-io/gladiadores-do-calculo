#include "jogador.h"

#include <math.h>


void jogador_inicializar(
    Jogador* jogador
)
{
    // Posição inicial do jogador
    jogador->x = 375;
    jogador->y = 275;

    // Tamanho do quadrado que representa o jogador
    jogador->tamanho = 50;

    // Velocidade de movimentação
    jogador->velocidade = 5;
}


void jogador_atualizar(
    Jogador* jogador,
    ALLEGRO_KEYBOARD_STATE* estado_teclado
)
{
    // Direção do movimento.
    // Começamos em zero porque o jogador pode ficar parado.
    float movimento_x = 0;
    float movimento_y = 0;


    // W movimenta para cima
    if (al_key_down(
        estado_teclado,
        ALLEGRO_KEY_W))
    {
        movimento_y -= 1;
    }


    // S movimenta para baixo
    if (al_key_down(
        estado_teclado,
        ALLEGRO_KEY_S))
    {
        movimento_y += 1;
    }


    // A movimenta para a esquerda
    if (al_key_down(
        estado_teclado,
        ALLEGRO_KEY_A))
    {
        movimento_x -= 1;
    }


    // D movimenta para a direita
    if (al_key_down(
        estado_teclado,
        ALLEGRO_KEY_D))
    {
        movimento_x += 1;
    }


    /*
        Normalizamos o movimento diagonal.

        Sem isso, o jogador seria mais rápido
        quando W+A, W+D, S+A ou S+D fossem usados
        ao mesmo tempo.
    */

    if (movimento_x != 0 ||
        movimento_y != 0)
    {
        float distancia = sqrt(
            movimento_x * movimento_x +
            movimento_y * movimento_y
        );

        movimento_x /= distancia;
        movimento_y /= distancia;


        jogador->x +=
            movimento_x *
            jogador->velocidade;

        jogador->y +=
            movimento_y *
            jogador->velocidade;
    }


    /*
        Impede que o jogador saia da arena.
    */

    if (jogador->x < 0)
    {
        jogador->x = 0;
    }

    if (jogador->y < 0)
    {
        jogador->y = 0;
    }

    if (jogador->x + jogador->tamanho >
        800)
    {
        jogador->x =
            800 - jogador->tamanho;
    }

    if (jogador->y + jogador->tamanho >
        600)
    {
        jogador->y =
            600 - jogador->tamanho;
    }
}


void jogador_desenhar(
    Jogador* jogador
)
{
    // Por enquanto o jogador é representado
    // por um quadrado branco.
    // Mais tarde podemos substituir por uma imagem.
    al_draw_filled_rectangle(
        jogador->x,
        jogador->y,
        jogador->x + jogador->tamanho,
        jogador->y + jogador->tamanho,
        al_map_rgb(255, 255, 255)
    );
}


void jogador_desenhar_arma(
    Jogador* jogador,
    float mouse_x,
    float mouse_y
)
{
    float centro_x =
        jogador_centro_x(jogador);

    float centro_y =
        jogador_centro_y(jogador);


    // Calcula a direção entre o jogador e o mouse
    float direcao_x =
        mouse_x - centro_x;

    float direcao_y =
        mouse_y - centro_y;


    // Calcula a distância até o mouse
    float distancia = sqrt(
        direcao_x * direcao_x +
        direcao_y * direcao_y
    );


    // Evita divisão por zero
    if (distancia == 0)
    {
        return;
    }


    // Normaliza a direção
    direcao_x /= distancia;
    direcao_y /= distancia;


    /*
        A arma começa no centro do jogador
        e se estende 35 pixels na direção do mouse.
    */

    float arma_x =
        centro_x +
        direcao_x * 35;

    float arma_y =
        centro_y +
        direcao_y * 35;


    // Desenha a arma
    al_draw_line(
        centro_x,
        centro_y,
        arma_x,
        arma_y,
        al_map_rgb(150, 150, 150),
        8
    );
}


float jogador_centro_x(
    Jogador* jogador
)
{
    return jogador->x +
        jogador->tamanho / 2;
}


float jogador_centro_y(
    Jogador* jogador
)
{
    return jogador->y +
        jogador->tamanho / 2;
}