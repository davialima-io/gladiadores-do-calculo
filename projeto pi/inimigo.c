#include "inimigo.h"
#include "jogo.h"
#include <stdlib.h>

void inimigo_inicializar(Inimigo* inimigo, float x, float y)
{
    inimigo->tamanho = 50;
    inimigo->x = x;

    // Força a altura exata em cima da plataforma de pedra (igual ao jogador)
    inimigo->y = (ALTURA_TELA - 100) - inimigo->tamanho;

    inimigo->velocidade = 3.5f;
    inimigo->direcao = -1; // Começa a andar para a esquerda

    inimigo->velocidade_y = 0;
    inimigo->gravidade = 0.5f;
    inimigo->forca_pulo = -10.0f;
    inimigo->no_chao = 1;

    inimigo->vida_maxima = VIDA_MAXIMA_INIMIGO;
    inimigo->vida = inimigo->vida_maxima;
    inimigo->tempo_pulo = 0;
}

void inimigo_atualizar(Inimigo* inimigo, float largura_tela, float altura_chao)
{
    // 1. Movimento horizontal (eixo X)
    inimigo->x += inimigo->velocidade * inimigo->direcao;

    if (inimigo->x + inimigo->tamanho >= largura_tela) {
        inimigo->x = largura_tela - inimigo->tamanho;
        inimigo->direcao = -1;
    }
    if (inimigo->x <= 0) {
        inimigo->x = 0;
        inimigo->direcao = 1;
    }

    // 2. Lógica de pulo aleatório
    inimigo->tempo_pulo++;
    if (inimigo->tempo_pulo >= 100) {
        inimigo->tempo_pulo = 0;

        if (inimigo->no_chao && (rand() % 2 == 1)) {
            inimigo->velocidade_y = inimigo->forca_pulo;
            inimigo->no_chao = 0;
        }
    }

    // 3. Aplica gravidade e movimento vertical (eixo Y)
    inimigo->velocidade_y += inimigo->gravidade;
    inimigo->y += inimigo->velocidade_y;

    // 4. Colisão com o chão alinhada perfeitamente com a plataforma
    float chao_jogador = (ALTURA_TELA - 65) - inimigo->tamanho;

    if (inimigo->y >= chao_jogador) {
        inimigo->y = chao_jogador;
        inimigo->velocidade_y = 0;
        inimigo->no_chao = 1;
    }
    else {
        inimigo->no_chao = 0;
    }
}

void inimigo_receber_dano(Inimigo* inimigo, float x_inicial, float altura_chao)
{
    inimigo->vida--;

    if (inimigo->vida <= 0)
    {
        inimigo->x = 750;
        // Respawna na altura correta em cima da plataforma
        inimigo->y = (ALTURA_TELA - 100) - inimigo->tamanho;
        inimigo->vida = inimigo->vida_maxima;
        inimigo->velocidade_y = 0;
        inimigo->no_chao = 1;
        inimigo->direcao = -1;
    }
}

bool inimigo_colidiu_com_tiro(Inimigo* inimigo, float tiro_x, float tiro_y)
{
    if (tiro_x >= inimigo->x &&
        tiro_x <= inimigo->x + inimigo->tamanho &&
        tiro_y >= inimigo->y &&
        tiro_y <= inimigo->y + inimigo->tamanho)
    {
        return true;
    }
    return false;
}

void inimigo_desenhar(Inimigo* inimigo)
{
    if (inimigo->vida <= 0) return;

    // Desenha o inimigo
    al_draw_filled_rectangle(
        inimigo->x,
        inimigo->y,
        inimigo->x + inimigo->tamanho,
        inimigo->y + inimigo->tamanho,
        al_map_rgb(200, 50, 50)
    );

    // Barra de vida
    float largura_barra = 60;
    float altura_barra = 8;
    float barra_x = inimigo->x - 5;
    float barra_y = inimigo->y - 15;

    al_draw_filled_rectangle(barra_x, barra_y, barra_x + largura_barra, barra_y + altura_barra, al_map_rgb(50, 50, 50));

    float largura_vida = largura_barra * ((float)inimigo->vida / inimigo->vida_maxima);
    al_draw_filled_rectangle(barra_x, barra_y, barra_x + largura_vida, barra_y + altura_barra, al_map_rgb(50, 220, 70));
}