#include "fundo.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>

#include "jogo.h"

/* QUANTIDADE TOTAL DE FRAMES */
#define TOTAL_FRAMES 240

/* VELOCIDADE DA ANIMAÇÃO: 45 frames por segundo */
#define FPS_FUNDO 30.0

/* FRAME ATUAL DA ANIMAÇÃO */
static ALLEGRO_BITMAP* frame_atual = NULL;

/* NÚMERO DO FRAME ATUAL: começa no frame 0 */
static int numero_frame = 0;

/* DIREÇÃO: 1 = avançando, -1 = voltando */
static int direcao_frame = 1;

/* CONTROLE DO TEMPO DA ANIMAÇÃO */
static double ultimo_frame = 0;


/* INICIALIZA O FUNDO */
bool fundo_inicializar()
{
    /* Inicializa o addon responsável por carregar imagens */
    if (!al_init_image_addon())
    {
        return false;
    }

    /* Carrega o primeiro frame */
    frame_atual = al_load_bitmap(
        "fundo_frames/frame_0001.jpg"
    );

    /* Verifica se conseguiu carregar a imagem */
    if (!frame_atual)
    {
        al_shutdown_image_addon();
        return false;
    }

    /* Começa no primeiro frame */
    numero_frame = 0;

    /* Começa avançando */
    direcao_frame = 1;

    /* Guarda o momento inicial da animação */
    ultimo_frame = al_get_time();

    return true;
}


/* ATUALIZA A ANIMAÇÃO */
void fundo_atualizar()
{
    /* Pega o tempo atual */
    double tempo_atual = al_get_time();

    /* Intervalo entre os frames */
    double intervalo = 1.0 / FPS_FUNDO;

    /* Verifica se já passou tempo suficiente */
    if (tempo_atual - ultimo_frame < intervalo)
    {
        return;
    }

    /* Guarda o momento da troca */
    ultimo_frame = tempo_atual;

    /* Avança ou volta um frame */
    numero_frame += direcao_frame;

    /* Chegou ao último frame? Começa a voltar */
    if (numero_frame >= TOTAL_FRAMES - 1)
    {
        numero_frame = TOTAL_FRAMES - 1;
        direcao_frame = -1;
    }
    /* Chegou ao primeiro frame? Começa a avançar */
    else if (numero_frame <= 0)
    {
        numero_frame = 0;
        direcao_frame = 1;
    }

    /* Monta o nome do arquivo */
    char nome_arquivo[100];

    sprintf_s(
        nome_arquivo,
        sizeof(nome_arquivo),
        "fundo_frames/frame_%04d.jpg",
        numero_frame + 1
    );

    /* Carrega o próximo frame */
    ALLEGRO_BITMAP* novo_frame = al_load_bitmap(nome_arquivo);

    /* Só substitui o frame atual se a imagem foi carregada */
    if (novo_frame)
    {
        /* Libera a imagem anterior */
        if (frame_atual)
        {
            al_destroy_bitmap(frame_atual);
        }

        /* Guarda a nova imagem */
        frame_atual = novo_frame;
    }
}


/* DESENHA O FUNDO */
void fundo_desenhar()
{
    /* Se não existe imagem, não desenha */
    if (!frame_atual)
    {
        return;
    }

    /* Desenha a imagem ocupando toda a tela */
    al_draw_scaled_bitmap(
        frame_atual,

        /* Área original da imagem */
        0,
        0,
        al_get_bitmap_width(frame_atual),
        al_get_bitmap_height(frame_atual),

        /* Posição na tela */
        0,
        0,

        /* Tamanho da tela */
        LARGURA_TELA,
        ALTURA_TELA,

        /* Sem espelhamento */
        0
    );
}


/* FINALIZA O FUNDO */
void fundo_finalizar()
{
    /* Libera o frame atual */
    if (frame_atual)
    {
        al_destroy_bitmap(frame_atual);
        frame_atual = NULL;
    }

    /* Reinicia as variáveis da animação */
    numero_frame = 0;
    direcao_frame = 1;
    ultimo_frame = 0;

    /* Desliga o sistema de imagens */
    al_shutdown_image_addon();
}