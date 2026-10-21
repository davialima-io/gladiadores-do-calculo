#include "fundo.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>

#include "jogo.h"


/* QUANTIDADE DE FRAMES Nossa animação possui 240 imagens*/
#define TOTAL_FRAMES 60
/* FPS DA ANIMAÇÃO O jogo continua rodando a 60 FPS, mas o fundo troca de imagem 30 vezes por segundo*/
#define FPS_FUNDO 30.0
/* FRAME ATUAL  Guarda a imagem que está sendo desenhada*/
static ALLEGRO_BITMAP* frame_atual = NULL;
/* NUMERO DO FRAME Começa no frame 0 */
static int numero_frame = 0;
/* CONTROLE DO TEMPO = Guarda quando o último frame foi trocado*/
static double ultimo_frame = 0;
/* INICIALIZA O FUNDO*/
bool fundo_inicializar()
{
/* Inicializa o addon responsávelpor carregar imagens */
	if (!al_init_image_addon())
	{
		return false;
	}
	/* Carrega o primeiro frame */
	frame_atual =
		al_load_bitmap(
			"fundo_frames/frame_0001.jpg"
		);


 /* Verifica se conseguiu carregar */ 
	if (!frame_atual)
	{
		al_shutdown_image_addon();

		return false;
	}
/* Começamos no primeiro frame   */
	numero_frame = 0;
/* Guarda o momento inicial da animação */
	ultimo_frame = al_get_time();
	return true;
}
/*
    ATUALIZA A ANIMAÇÃO
*/

void fundo_atualizar()
{/* Pega o tempo atual */
	double tempo_atual =
		al_get_time();


/* Calcula quanto tempo cada frame deve permanecer na tela. 30 FPS:   1 / 30 = aproximadamente0,033 segundos */
	double intervalo =
		1.0 / FPS_FUNDO;
/* Ainda não passou tempo suficiente?  Então mantém o frame atua */
	if (tempo_atual - ultimo_frame < intervalo)
	{
		return;
	}
/* Guarda o momento da troca */
	ultimo_frame = tempo_atual;

/*Vai para o próximo frame */
	numero_frame++;

/*Chegou ao final?Volta para o começo.Isso cria o loo */

	if (numero_frame >= TOTAL_FRAMES)
	{
		numero_frame = 0;
	}
 /* Monta o nome do arquivo */
	char nome_arquivo[100];
	sprintf_s(
		nome_arquivo,
		sizeof(nome_arquivo),
		"fundo_frames/frame_%04d.jpg",
		numero_frame + 1
	);


/* Carrega o próximo frame  */
	ALLEGRO_BITMAP* novo_frame =
	al_load_bitmap(nome_arquivo);
/* Só substitui o frame atual se o novo foi carregado*/
	if (novo_frame)
	{
	/* Libera a imagem anterior */
		if (frame_atual)
		{
			al_destroy_bitmap(
				frame_atual
			);
		}
	/* Guarda a nova imagem */
		frame_atual =
			novo_frame;
	}
}
/* DESENHA O FUNDO */
void fundo_desenhar()
{
/* Se não existe imagem, não desenha  */
	if (!frame_atual)
	{
		return;
	}
/* Desenha a imagem ocupando toda a telaa*/
al_draw_scaled_bitmap(
frame_atual,
 /* Área original da imagem */
		0,
		0,

		al_get_bitmap_width(
			frame_atual
		),

		al_get_bitmap_height(
			frame_atual
		),
/* Posição na tela.*/
		0,
		0,

 /* Tamanho da tela */
		LARGURA_TELA,
		ALTURA_TELA,
	/* Sem espelhamento */
		0
	);
}
/* FINALIZA O FUNDO*/
void fundo_finalizar()
{
/* Libera o frame atual */
	if (frame_atual)
	{
		al_destroy_bitmap(
			frame_atual
		);

		frame_atual = NULL;
	}
/* Desliga o sistema de imagens. */
	al_shutdown_image_addon();
}