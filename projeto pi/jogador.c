// jogador: movimento, pulo e animacao do gladiador
#include "jogador.h"
#include "jogo.h"
#include <math.h>
#include <stdio.h>

#define JOGADOR_LARGURA 100
#define JOGADOR_ALTURA 133
#define JOGADOR_X_INICIAL 445
#define JOGADOR_VELOCIDADE 3
#define JOGADOR_GRAVIDADE 0.5f
#define JOGADOR_FORCA_PULO -13.0f

// tempo de um tick do jogo (60 fps), usado pra contar a animacao
#define PASSO_ANIMACAO (1.0f / 60.0f)
#define PI_FLOAT 3.14159265f

#define SHEET_COLUNAS 6
#define SHEET_LINHAS 4

// linhas do gladiador.png (1536x1024, fundo transparente)
#define LINHA_PARADO 0
#define LINHA_ATAQUE 1
#define LINHA_ANDANDO 2
#define LINHA_PULO 3

// segundos que cada frame fica na tela
#define TEMPO_FRAME_PARADO 0.15f
#define TEMPO_FRAME_ATAQUE 0.08f
#define TEMPO_FRAME_ANDANDO 0.16f
#define TEMPO_FRAME_PULO 0.15f

// altura do personagem no gladiador.png, a escala sai dela
#define ALTURA_REFERENCIA 229.0f

// respiracao usada quando nao acha o gladiador_idle.png
#define IDLE_PERIODO 1.2f
#define IDLE_ESTICAR 0.08f
#define IDLE_ENCOLHER 0.035f

// idle desenhado (gladiador_idle.png), 6 frames numa linha so
#define IDLE_FRAMES 6
#define IDLE_TEMPO_FRAME 0.20f
#define ALTURA_REFERENCIA_IDLE 310.0f
// respiracao leve por cima dos frames, so pra nao ficar travado
#define IDLE_ESTICAR_FRAMES 0.025f
#define IDLE_ENCOLHER_FRAMES 0.010f

static ALLEGRO_BITMAP* idle_sheet = NULL;
static bool idle_ativo = false;
static double idle_inicio = 0.0;
static int linha_anterior = -1;

// as linhas do png nao seguem uma grade fixa, entao medi cada frame na mao
// cx = centro de massa do desenho, pe = linha dos pes
typedef struct {
    int x, y, w, h;
    float cx;
    int pe;
} FrameSprite;

static const FrameSprite FRAMES[SHEET_LINHAS][SHEET_COLUNAS] = {
    {
        { 53, 18, 173, 227, 134.9f, 245 },
        { 297, 16, 176, 229, 377.3f, 245 },
        { 542, 17, 181, 228, 624.9f, 245 },
        { 784, 21, 195, 224, 869.0f, 245 },
        { 1033, 17, 196, 228, 1118.0f, 245 },
        { 1282, 19, 221, 226, 1369.1f, 245 },
    },
    // ataque fora de ordem de proposito: prepara a lanca e depois lanca
    {
        { 785, 282, 158, 214, 864.8f, 496 },
        { 1030, 285, 202, 211, 1114.9f, 496 },
        { 1280, 285, 204, 211, 1365.7f, 496 },
        { 537, 282, 204, 214, 622.2f, 496 },
        { 33, 279, 228, 217, 126.2f, 496 },
        { 282, 280, 227, 216, 374.2f, 496 },
    },
    {
        { 36, 525, 204, 229, 121.2f, 754 },
        { 283, 527, 204, 225, 370.4f, 754 },
        { 524, 527, 218, 222, 615.4f, 754 },
        { 784, 529, 184, 220, 859.2f, 754 },
        { 1018, 533, 226, 216, 1112.1f, 754 },
        { 1275, 533, 210, 216, 1360.5f, 754 },
    },
    {
        { 30, 796, 203, 175, 113.8f, 971 },
        { 263, 802, 221, 168, 355.0f, 970 },
        { 510, 795, 232, 180, 597.5f, 975 },
        { 777, 799, 220, 176, 862.8f, 975 },
        { 1023, 786, 193, 196, 1101.1f, 982 },
        { 1275, 773, 197, 209, 1359.3f, 982 },
    },
};

static const FrameSprite FRAMES_IDLE[IDLE_FRAMES] = {
    { 22, 380, 231, 309, 137.1f, 689 },
    { 278, 383, 225, 306, 394.3f, 689 },
    { 529, 381, 215, 308, 638.9f, 689 },
    { 777, 377, 225, 312, 887.7f, 689 },
    { 1026, 376, 223, 313, 1138.8f, 689 },
    { 1279, 380, 218, 309, 1387.6f, 689 },
};

void jogador_inicializar(Jogador* jogador) {
    jogador->largura = JOGADOR_LARGURA;
    jogador->altura = JOGADOR_ALTURA;
    jogador->x = JOGADOR_X_INICIAL;
    jogador->y = CHAO_Y - jogador->altura;

    jogador->velocidade = JOGADOR_VELOCIDADE;
    jogador->velocidade_y = 0;
    jogador->gravidade = JOGADOR_GRAVIDADE;
    jogador->forca_pulo = JOGADOR_FORCA_PULO;

    jogador->no_chao = false;
    jogador->espaco_anterior = false;

    jogador->sprite_sheet = al_load_bitmap("gladiador.png");
    if (!jogador->sprite_sheet) {
        printf("ERRO: Nao foi possivel carregar gladiador.png\n");
    }

    idle_sheet = al_load_bitmap("gladiador_idle.png");
    if (!idle_sheet) {
        printf("AVISO: gladiador_idle.png nao encontrado, usando respiracao simples\n");
    }

    jogador->frame_atual = 0;
    jogador->tempo_animacao = 0;
    jogador->atirando = false;
    jogador->olhando_direita = true;
    jogador->andando = false;
}

// no ar sempre vale a animacao de pulo, mesmo atirando
static int escolher_linha(const Jogador* jogador) {
    if (!jogador->no_chao) return LINHA_PULO;
    if (jogador->atirando) return LINHA_ATAQUE;
    if (jogador->andando) return LINHA_ANDANDO;
    return LINHA_PARADO;
}

static void mover_horizontal(Jogador* jogador, const ALLEGRO_KEYBOARD_STATE* teclado) {
    float movimento_x = 0;

    if (al_key_down(teclado, ALLEGRO_KEY_A)) {
        movimento_x -= 1;
        jogador->andando = true;
    }
    else if (al_key_down(teclado, ALLEGRO_KEY_D)) {
        movimento_x += 1;
        jogador->andando = true;
    }
    else {
        jogador->andando = false;
    }

    // atirando, quem decide pra onde ele olha e o mouse, nao a tecla
    if (movimento_x != 0 && !jogador->atirando) {
        jogador->olhando_direita = (movimento_x > 0);
    }

    jogador->x += movimento_x * jogador->velocidade;
}

// vira o personagem pro lado do mouse, pra ele nao atirar de costas
static void virar_para_mouse(Jogador* jogador, float mouse_x) {
    if (!jogador->atirando) return;
    jogador->olhando_direita = (mouse_x >= jogador->x + jogador->largura / 2.0f);
}

// so pula quando aperta (segurar nao conta) e esta no chao
static void tratar_pulo(Jogador* jogador, const ALLEGRO_KEYBOARD_STATE* teclado) {
    bool espaco = al_key_down(teclado, ALLEGRO_KEY_SPACE);

    if (espaco && !jogador->espaco_anterior && jogador->no_chao) {
        jogador->velocidade_y = jogador->forca_pulo;
        jogador->no_chao = false;
        jogador->frame_atual = 0;
    }
    jogador->espaco_anterior = espaco;
}

static void aplicar_gravidade(Jogador* jogador) {
    jogador->velocidade_y += jogador->gravidade;
    jogador->y += jogador->velocidade_y;

    float chao = CHAO_Y - jogador->altura;
    if (jogador->y >= chao) {
        jogador->y = chao;
        jogador->velocidade_y = 0;
        jogador->no_chao = true;
    }
    else {
        jogador->no_chao = false;
    }
}

static void limitar_na_tela(Jogador* jogador) {
    if (jogador->x < 0) jogador->x = 0;
    if (jogador->x + jogador->largura > LARGURA_TELA) jogador->x = LARGURA_TELA - jogador->largura;
}

// o idle conta pelo relogio, entao guarda quando o personagem parou
static void marcar_inicio_idle(int linha) {
    if (linha != LINHA_PARADO) {
        idle_ativo = false;
    }
    else if (!idle_ativo) {
        idle_ativo = true;
        idle_inicio = al_get_time();
    }
}

static float tempo_por_frame(int linha) {
    switch (linha) {
    case LINHA_ATAQUE: return TEMPO_FRAME_ATAQUE;
    case LINHA_PULO: return TEMPO_FRAME_PULO;
    case LINHA_ANDANDO: return TEMPO_FRAME_ANDANDO;
    default: return TEMPO_FRAME_PARADO;
    }
}

static void avancar_frame(Jogador* jogador, int linha) {
    jogador->tempo_animacao = 0;
    jogador->frame_atual++;
    if (jogador->frame_atual < SHEET_COLUNAS) return;

    if (linha == LINHA_PULO) {
        // segura o ultimo frame ate pousar
        jogador->frame_atual = SHEET_COLUNAS - 1;
    }
    else {
        jogador->frame_atual = 0;
        if (linha == LINHA_ATAQUE) jogador->atirando = false;
    }
}

static void atualizar_animacao(Jogador* jogador) {
    int linha = escolher_linha(jogador);

    // trocou de animacao, comeca do frame 0
    if (linha != linha_anterior) {
        jogador->frame_atual = 0;
        jogador->tempo_animacao = 0;
        linha_anterior = linha;
    }

    marcar_inicio_idle(linha);

    jogador->tempo_animacao += PASSO_ANIMACAO;
    if (jogador->tempo_animacao >= tempo_por_frame(linha)) avancar_frame(jogador, linha);
}

void jogador_atualizar(Jogador* jogador, const ALLEGRO_KEYBOARD_STATE* estado_teclado, float mouse_x, float mouse_y) {
    mover_horizontal(jogador, estado_teclado);
    virar_para_mouse(jogador, mouse_x);
    tratar_pulo(jogador, estado_teclado);
    aplicar_gravidade(jogador);
    limitar_na_tela(jogador);
    atualizar_animacao(jogador);
}

// desenha um frame com os pes na base da caixa e o corpo no centro dela
static void desenhar_frame(const Jogador* jogador, ALLEGRO_BITMAP* sheet, const FrameSprite* f, float altura_ref, float esc_x, float esc_y) {
    // a escala vem de cada sheet pra altura na tela ser igual em todas
    float escala = jogador->altura / altura_ref;
    float dw = f->w * escala * esc_x;
    float dh = f->h * escala * esc_y;

    float centro_x = jogador->x + jogador->largura / 2.0f;
    float pes_y = jogador->y + jogador->altura;
    float dy = pes_y - (f->pe - f->y) * escala * esc_y;
    float dx;
    int flags;

    if (jogador->olhando_direita) {
        dx = centro_x - (f->cx - f->x) * escala * esc_x;
        flags = 0;
    }
    else {
        dx = centro_x - (f->w - (f->cx - f->x)) * escala * esc_x;
        flags = ALLEGRO_FLIP_HORIZONTAL;
    }

    al_draw_scaled_bitmap(sheet, f->x, f->y, f->w, f->h, dx, dy, dw, dh, flags);
}

static float tempo_parado(void) {
    if (!idle_ativo) return 0.0f;
    return (float)(al_get_time() - idle_inicio);
}

static void desenhar_parado(const Jogador* jogador) {
    float fase = tempo_parado();
    ALLEGRO_BITMAP* sheet = jogador->sprite_sheet;
    const FrameSprite* f = &FRAMES[LINHA_PARADO][0];
    float altura_ref = ALTURA_REFERENCIA;
    float estica = IDLE_ESTICAR;
    float encolhe = IDLE_ENCOLHER;

    if (idle_sheet) {
        int coluna = (int)(fase / IDLE_TEMPO_FRAME) % IDLE_FRAMES;
        sheet = idle_sheet;
        f = &FRAMES_IDLE[coluna];
        altura_ref = ALTURA_REFERENCIA_IDLE;
        estica = IDLE_ESTICAR_FRAMES;
        encolhe = IDLE_ENCOLHER_FRAMES;
    }

    // comeca em 0 pra nao dar salto quando o personagem para
    float t = (1.0f - cosf(fase * 2.0f * PI_FLOAT / IDLE_PERIODO)) * 0.5f;
    desenhar_frame(jogador, sheet, f, altura_ref, 1.0f - encolhe * t, 1.0f + estica * t);
}

void jogador_desenhar(const Jogador* jogador) {
    if (!jogador->sprite_sheet) return;

    int linha = escolher_linha(jogador);
    if (linha == LINHA_PARADO) {
        desenhar_parado(jogador);
        return;
    }

    int coluna = jogador->frame_atual;
    if (coluna < 0 || coluna >= SHEET_COLUNAS) coluna = 0;
    desenhar_frame(jogador, jogador->sprite_sheet, &FRAMES[linha][coluna], ALTURA_REFERENCIA, 1.0f, 1.0f);
}

void jogador_desenhar_arma(const Jogador* jogador, float mouse_x, float mouse_y) {
}

float jogador_centro_x(const Jogador* jogador) {
    return jogador->x + jogador->largura / 2;
}

float jogador_centro_y(const Jogador* jogador) {
    return jogador->y + jogador->altura / 2;
}

bool jogador_colidiu_com_inimigo(const Jogador* jogador, const Inimigo* inimigo) {
    return jogador->x < inimigo->x + inimigo->tamanho &&
        jogador->x + jogador->largura > inimigo->x &&
        jogador->y < inimigo->y + inimigo->tamanho &&
        jogador->y + jogador->altura > inimigo->y;
}

void jogador_finalizar(Jogador* jogador) {
    if (jogador->sprite_sheet) {
        al_destroy_bitmap(jogador->sprite_sheet);
    }
    if (idle_sheet) {
        al_destroy_bitmap(idle_sheet);
        idle_sheet = NULL;
    }
}