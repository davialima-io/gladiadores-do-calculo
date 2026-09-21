#include "tiro.h"
#include "jogo.h"

#include <allegro5/allegro_primitives.h>
#include <math.h>


void tiros_inicializar(
    Tiro tiros[]
)
{
    for (int i = 0; i < MAX_TIROS; i++)
    {
        tiros[i].x = 0;
        tiros[i].y = 0;

        tiros[i].velocidade = 10;

        tiros[i].direcao_x = 0;
        tiros[i].direcao_y = 0;

        tiros[i].ativo = false;
    }
}


void tiro_disparar(
    Tiro tiros[],
    float jogador_x,
    float jogador_y,
    float mouse_x,
    float mouse_y
)
{
    /*
        Procuramos o primeiro espaço livre
        no vetor de tiros.
    */

    for (int i = 0; i < MAX_TIROS; i++)
    {
        if (tiros[i].ativo)
        {
            continue;
        }


        // Calcula a direção do tiro
        float direcao_x =
            mouse_x - jogador_x;

        float direcao_y =
            mouse_y - jogador_y;


        // Calcula a distância até o mouse
        float distancia = sqrt(
            direcao_x * direcao_x +
            direcao_y * direcao_y
        );


        // Não dispara se o mouse estiver exatamente
        // sobre o centro do jogador.
        if (distancia == 0)
        {
            return;
        }


        // Normaliza a direção
        direcao_x /= distancia;
        direcao_y /= distancia;


        /*
            O tiro nasce 30 pixels à frente
            do jogador para não começar dentro dele.
        */

        tiros[i].x =
            jogador_x +
            direcao_x * 30;

        tiros[i].y =
            jogador_y +
            direcao_y * 30;


        tiros[i].velocidade = 10;

        tiros[i].direcao_x =
            direcao_x;

        tiros[i].direcao_y =
            direcao_y;

        tiros[i].ativo = true;


        // Criamos apenas um tiro por clique
        return;
    }
}


void tiros_atualizar(
    Tiro tiros[]
)
{
    for (int i = 0; i < MAX_TIROS; i++)
    {
        if (!tiros[i].ativo)
        {
            continue;
        }


        // Move o tiro na direção em que foi disparado
        tiros[i].x +=
            tiros[i].direcao_x *
            tiros[i].velocidade;

        tiros[i].y +=
            tiros[i].direcao_y *
            tiros[i].velocidade;


        /*
            Quando o tiro sai da arena,
            ele deixa de ser ativo.
        */

        if (tiros[i].x < 0 ||
            tiros[i].x > LARGURA_TELA ||
            tiros[i].y < 0 ||
            tiros[i].y > ALTURA_TELA)
        {
            tiros[i].ativo = false;
        }
    }
}


void tiros_desenhar(
    Tiro tiros[]
)
{
    for (int i = 0; i < MAX_TIROS; i++)
    {
        if (!tiros[i].ativo)
        {
            continue;
        }


        // Por enquanto o tiro é representado
        // por um pequeno círculo amarelo.
        al_draw_filled_circle(
            tiros[i].x,
            tiros[i].y,
            5,
            al_map_rgb(255, 220, 0)
        );
    }
}