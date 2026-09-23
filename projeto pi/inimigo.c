#include "inimigo.h"
#include "jogo.h"


void inimigo_inicializar(
    Inimigo* inimigo
)
{
    // Posição inicial do inimigo
    inimigo->x = 600;
    inimigo->y = 250;

    // Tamanho
    inimigo->tamanho = 50;

    // Velocidade horizontal
    inimigo->velocidade = 2;

    // Começa andando para a direita
    inimigo->direcao = 1;

    // Define a vida inicial
    inimigo->vida_maxima =
        VIDA_MAXIMA_INIMIGO;

    inimigo->vida =
        inimigo->vida_maxima;
}


void inimigo_atualizar(
    Inimigo* inimigo
)
{
    // Move o inimigo horizontalmente
    inimigo->x +=
        inimigo->velocidade *
        inimigo->direcao;


    // Chegou ao lado direito
    if (inimigo->x + inimigo->tamanho >=
        LARGURA_TELA)
    {
        inimigo->x =
            LARGURA_TELA -
            inimigo->tamanho;

        inimigo->direcao = -1;
    }


    // Chegou ao lado esquerdo
    if (inimigo->x <= 0)
    {
        inimigo->x = 0;

        inimigo->direcao = 1;
    }
}


void inimigo_receber_dano(
    Inimigo* inimigo
)
{
    inimigo->vida--;


    /*
        Quando a vida chega a zero,
        o inimigo volta para a posição inicial
        com a vida cheia.
    */

    if (inimigo->vida <= 0)
    {
        inimigo->x = 600;
        inimigo->y = 250;

        inimigo->vida =
            inimigo->vida_maxima;

        inimigo->direcao = 1;
    }
}


bool inimigo_colidiu_com_tiro(
    Inimigo* inimigo,
    float tiro_x,
    float tiro_y
)
{
    /*
        Verifica se o centro do tiro
        está dentro do quadrado do inimigo.
    */

    if (tiro_x >= inimigo->x &&
        tiro_x <= inimigo->x + inimigo->tamanho &&
        tiro_y >= inimigo->y &&
        tiro_y <= inimigo->y + inimigo->tamanho)
    {
        return true;
    }


    return false;
}


void inimigo_desenhar(
    Inimigo* inimigo
)
{
    // Não desenha um inimigo sem vida
    if (inimigo->vida <= 0)
    {
        return;
    }


    /*
        DESENHO DO INIMIGO

        Por enquanto usamos um quadrado vermelho.
        Depois  colocar a imagem do leão.
    */

    al_draw_filled_rectangle(
        inimigo->x,
        inimigo->y,
        inimigo->x + inimigo->tamanho,
        inimigo->y + inimigo->tamanho,
        al_map_rgb(200, 50, 50)
    );


    /*
        BARRA DE VIDA
    */

    float largura_barra = 60;
    float altura_barra = 8;


    float barra_x =
        inimigo->x - 5;

    float barra_y =
        inimigo->y - 15;


    // Fundo da barra
    al_draw_filled_rectangle(
        barra_x,
        barra_y,
        barra_x + largura_barra,
        barra_y + altura_barra,
        al_map_rgb(50, 50, 50)
    );


    // Calcula quanto da barra deve estar preenchido
    float largura_vida =
        largura_barra *
        ((float)inimigo->vida /
            inimigo->vida_maxima);


    // Vida atual
    al_draw_filled_rectangle(
        barra_x,
        barra_y,
        barra_x + largura_vida,
        barra_y + altura_barra,
        al_map_rgb(50, 220, 70)
    );
}