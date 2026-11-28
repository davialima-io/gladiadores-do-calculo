// inimigo: leao que persegue o jogador (correr e atacar)
#include "inimigo.h"
#include "jogo.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define INIMIGO_TAMANHO 100
#define INIMIGO_X_INICIAL 750
#define INIMIGO_Y_INICIAL (ALTURA_TELA - 100 - INIMIGO_TAMANHO)
#define INIMIGO_VELOCIDADE 3.5f
#define INIMIGO_GRAVIDADE 0.5f
#define INIMIGO_FORCA_PULO -10.0f
#define FRAMES_ENTRE_PULOS 100

// ataca quando o jogador esta a ate esse tanto de pixels (centro com centro)
#define ALCANCE_ATAQUE_X 120
#define ALCANCE_ATAQUE_Y INIMIGO_TAMANHO

// abaixo disso o leao nao anda nem vira, senao ficava tremendo embaixo do jogador no pulo
#define ZONA_PARADA_X 8

// sprite sheet do leao (leao_medieval.png)
#define LEAO_SHEET_LARGURA 1536
#define LEAO_SHEET_ALTURA 1024
#define LEAO_ESTADOS 2
#define LEAO_COLUNAS 8
#define ALTURA_REFERENCIA_LEAO 145.0f

// tempo de um tick do jogo (60 fps), usado pra contar a animacao
#define INIMIGO_PASSO_ANIMACAO (1.0f / 60.0f)
#define TEMPO_FRAME_CORRENDO 0.07f
#define TEMPO_FRAME_ATACANDO 0.10f

// cada frame foi medido no png porque a grade nao e uniforme
// cx = centro de massa do desenho, pe = linha dos pes
typedef struct {
    int x, y, w, h;
    float cx;
    int pe;
} FrameLeao;

// linha 0 = correr (8 frames), linha 1 = atacar (6 frames)
static const FrameLeao FRAMES_LEAO[LEAO_ESTADOS][LEAO_COLUNAS] = {
    {
        { 4, 414, 195, 150, 111.8f, 564 },
        { 180, 420, 205, 143, 302.7f, 564 },
        { 391, 420, 183, 141, 489.0f, 564 },
        { 577, 417, 187, 144, 677.0f, 564 },
        { 758, 414, 198, 146, 869.1f, 564 },
        { 946, 416, 206, 146, 1065.5f, 564 },
        { 1152, 416, 192, 147, 1256.1f, 564 },
        { 1334, 419, 197, 143, 1442.9f, 564 },
    },
    {
        { 10, 739, 225, 155, 137.9f, 900 },
        { 252, 760, 218, 137, 372.9f, 900 },
        { 479, 730, 219, 146, 601.5f, 900 },
        { 733, 726, 270, 157, 883.9f, 900 },
        { 1024, 778, 260, 135, 1169.8f, 900 },
        { 1305, 761, 220, 143, 1426.5f, 900 },
    },
};

static const int FRAMES_POR_ESTADO[LEAO_ESTADOS] = { 8, 6 };

// confere o tamanho porque a tabela de frames so vale pra esse png
static ALLEGRO_BITMAP* carregar_sprite_leao(void) {
    ALLEGRO_BITMAP* bmp = al_load_bitmap("leao_medieval.png");
    if (!bmp) {
        printf("AVISO: leao_medieval.png nao encontrado, usando o quadrado\n");
        return NULL;
    }

    int largura = al_get_bitmap_width(bmp);
    int altura = al_get_bitmap_height(bmp);
    if (largura != LEAO_SHEET_LARGURA || altura != LEAO_SHEET_ALTURA) {
        printf("AVISO: leao_medieval.png tem %dx%d e o esperado e %dx%d, usando o quadrado\n", largura, altura, LEAO_SHEET_LARGURA, LEAO_SHEET_ALTURA);
        al_destroy_bitmap(bmp);
        return NULL;
    }
    return bmp;
}

// Coloca o inimigo no canto direito com a vida cheia (usado no inicio e quando ele morre)
static void voltar_ao_ponto_inicial(Inimigo* inimigo) {
    inimigo->x = INIMIGO_X_INICIAL;
    inimigo->y = INIMIGO_Y_INICIAL;
    inimigo->velocidade_y = 0;
    inimigo->no_chao = true;
    inimigo->direcao = -1;
    inimigo->vida = inimigo->vida_maxima;

    inimigo->estado = INIMIGO_CORRENDO;
    inimigo->frame_atual = 0;
    inimigo->tempo_animacao = 0;
}

void inimigo_inicializar(Inimigo* inimigo) {
    inimigo->tamanho = INIMIGO_TAMANHO;
    inimigo->velocidade = INIMIGO_VELOCIDADE;
    inimigo->gravidade = INIMIGO_GRAVIDADE;
    inimigo->forca_pulo = INIMIGO_FORCA_PULO;
    inimigo->tempo_pulo = 0;
    inimigo->vida_maxima = VIDA_MAXIMA_INIMIGO;
    inimigo->sprite_sheet = carregar_sprite_leao();
    voltar_ao_ponto_inicial(inimigo);
}

// o leao sempre olha pro jogador, mesmo parado no ataque
static void virar_para_alvo(Inimigo* inimigo, float dx) {
    if (fabsf(dx) <= ZONA_PARADA_X) return;
    inimigo->direcao = (dx > 0) ? 1 : -1;
}

// o ataque so vale com o jogador perto e mais ou menos na mesma altura, senao morderia o ar quando ele pula
static void atualizar_estado(Inimigo* inimigo, float dx, float dy) {
    bool no_alcance = fabsf(dx) <= ALCANCE_ATAQUE_X && fabsf(dy) <= ALCANCE_ATAQUE_Y;
    EstadoInimigo novo = no_alcance ? INIMIGO_ATACANDO : INIMIGO_CORRENDO;

    // trocou de animacao, comeca do frame 0
    if (novo != inimigo->estado) {
        inimigo->estado = novo;
        inimigo->frame_atual = 0;
        inimigo->tempo_animacao = 0;
    }
}

// no ataque ele fica plantado, so corre quando o jogador esta fora do alcance
static void correr_atras_do_alvo(Inimigo* inimigo, float dx) {
    if (inimigo->estado == INIMIGO_ATACANDO) return;
    if (fabsf(dx) <= ZONA_PARADA_X) return;

    inimigo->x += inimigo->velocidade * inimigo->direcao;
    if (inimigo->x < 0) inimigo->x = 0;
    if (inimigo->x + inimigo->tamanho > LARGURA_TELA) inimigo->x = LARGURA_TELA - inimigo->tamanho;
}

// de tempos em tempos sorteia se vai pular (50% de chance)
static void sortear_pulo(Inimigo* inimigo) {
    inimigo->tempo_pulo++;
    if (inimigo->tempo_pulo < FRAMES_ENTRE_PULOS) return;

    inimigo->tempo_pulo = 0;
    if (inimigo->no_chao && rand() % 2 == 0) {
        inimigo->velocidade_y = inimigo->forca_pulo;
        inimigo->no_chao = false;
    }
}

static void aplicar_gravidade(Inimigo* inimigo) {
    inimigo->velocidade_y += inimigo->gravidade;
    inimigo->y += inimigo->velocidade_y;

    float chao = CHAO_Y - inimigo->tamanho;
    if (inimigo->y >= chao) {
        inimigo->y = chao;
        inimigo->velocidade_y = 0;
        inimigo->no_chao = true;
    }
    else {
        inimigo->no_chao = false;
    }
}

static float tempo_por_frame(EstadoInimigo estado) {
    return (estado == INIMIGO_ATACANDO) ? TEMPO_FRAME_ATACANDO : TEMPO_FRAME_CORRENDO;
}

// as duas animacoes ficam em loop, o leao nunca fica parado
static void atualizar_animacao(Inimigo* inimigo) {
    inimigo->tempo_animacao += INIMIGO_PASSO_ANIMACAO;
    if (inimigo->tempo_animacao < tempo_por_frame(inimigo->estado)) return;

    inimigo->tempo_animacao = 0;
    inimigo->frame_atual = (inimigo->frame_atual + 1) % FRAMES_POR_ESTADO[inimigo->estado];
}

void inimigo_atualizar(Inimigo* inimigo, float alvo_x, float alvo_y) {
    float dx = alvo_x - (inimigo->x + inimigo->tamanho / 2);
    float dy = alvo_y - (inimigo->y + inimigo->tamanho / 2);

    virar_para_alvo(inimigo, dx);
    atualizar_estado(inimigo, dx, dy);
    correr_atras_do_alvo(inimigo, dx);
    sortear_pulo(inimigo);
    aplicar_gravidade(inimigo);
    atualizar_animacao(inimigo);
}

bool inimigo_atacando(const Inimigo* inimigo) {
    return inimigo->estado == INIMIGO_ATACANDO;
}

bool inimigo_receber_dano(Inimigo* inimigo) {
    inimigo->vida--;
    if (inimigo->vida > 0) return false;

    voltar_ao_ponto_inicial(inimigo);
    return true;
}

bool inimigo_colidiu_com_tiro(const Inimigo* inimigo, float tiro_x, float tiro_y) {
    return tiro_x >= inimigo->x && tiro_x <= inimigo->x + inimigo->tamanho && tiro_y >= inimigo->y && tiro_y <= inimigo->y + inimigo->tamanho;
}

// o sprite olha pra direita, entao vira quando o leao vai pra esquerda
static void desenhar_sprite(const Inimigo* inimigo) {
    const FrameLeao* f = &FRAMES_LEAO[inimigo->estado][inimigo->frame_atual];

    float escala = inimigo->tamanho / ALTURA_REFERENCIA_LEAO;
    float dw = f->w * escala;
    float dh = f->h * escala;

    // centro do corpo no centro da caixa e pes na base dela
    float centro_x = inimigo->x + inimigo->tamanho / 2.0f;
    float pes_y = inimigo->y + inimigo->tamanho;
    float dy = pes_y - (f->pe - f->y) * escala;
    float dx;
    int flags;

    if (inimigo->direcao > 0) {
        dx = centro_x - (f->cx - f->x) * escala;
        flags = 0;
    }
    else {
        dx = centro_x - (f->w - (f->cx - f->x)) * escala;
        flags = ALLEGRO_FLIP_HORIZONTAL;
    }

    al_draw_scaled_bitmap(inimigo->sprite_sheet, f->x, f->y, f->w, f->h, dx, dy, dw, dh, flags);
}

void inimigo_desenhar(const Inimigo* inimigo) {
    if (inimigo->sprite_sheet) {
        desenhar_sprite(inimigo);
    }
    else {
        al_draw_filled_rectangle(inimigo->x, inimigo->y, inimigo->x + inimigo->tamanho, inimigo->y + inimigo->tamanho, al_map_rgb(200, 50, 50));
    }

    // barra de vida em cima da cabeca, a verde encolhe conforme ele apanha
    float largura_barra = 60;
    float altura_barra = 8;
    float barra_x = inimigo->x - 5;
    float barra_y = inimigo->y - 15;
    float largura_vida = largura_barra * ((float)inimigo->vida / inimigo->vida_maxima);

    al_draw_filled_rectangle(barra_x, barra_y, barra_x + largura_barra, barra_y + altura_barra, al_map_rgb(50, 50, 50));
    al_draw_filled_rectangle(barra_x, barra_y, barra_x + largura_vida, barra_y + altura_barra, al_map_rgb(50, 220, 70));
}

void inimigo_finalizar(Inimigo* inimigo) {
    if (inimigo->sprite_sheet) {
        al_destroy_bitmap(inimigo->sprite_sheet);
        inimigo->sprite_sheet = NULL;
    }
}