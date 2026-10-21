#include "tempo.h"
#include <stdio.h>

#define TEMPO_TOTAL 60.0
#define LARGURA_TELA 800

void tempo_inicializar(Tempo* tempo)
{
    tempo->inicio = al_get_time();
    tempo->decorrido = 0;
    tempo->duracao = TEMPO_TOTAL;
}

void tempo_atualizar(Tempo* tempo)
{
    tempo->decorrido = al_get_time() - tempo->inicio;
}

void tempo_desenhar(
    Tempo* tempo,
    ALLEGRO_FONT* fonte)
{
    double restante =
        tempo->duracao - tempo->decorrido;

    if (restante < 0)
    {
        restante = 0;
    }

    int total_segundos = (int)restante;
    int minutos = total_segundos / 60;
    int segundos = total_segundos % 60;

    char texto[50];

    sprintf_s(
        texto,
        sizeof(texto),
        "Tempo: %02d:%02d",
        minutos,
        segundos
    );

    int largura_texto = al_get_text_width(fonte, texto);

    al_draw_text(
        fonte,
        al_map_rgb(255, 255, 0),
        LARGURA_TELA - largura_texto - 10,
        10,
        0,
        texto
    );
}

void tempo_adicionar(Tempo* tempo, double segundos)
{
    tempo->duracao += segundos;
}