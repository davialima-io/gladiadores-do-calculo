#include "equacao.h"
#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro_primitives.h>

// Posicao e tamanho do quadro (proporcoes da tela)
#define QUADRO_CENTRO_X 0.50f
#define QUADRO_CENTRO_Y 0.334f
#define QUADRO_LARGURA 0.17f
#define QUADRO_ALTURA 0.14f

// Configuracao de cada nivel.
// Os limites sao acumulados e o sorteio vai de 0 a 99:
//   sorteio < limite_soma  -> soma
//   sorteio < limite_sub   -> subtracao
//   sorteio < limite_mult  -> multiplicacao
//   resto                  -> equacao de 1o grau
typedef struct {
    int limite_soma;
    int limite_sub;
    int limite_mult;
    int max_soma_sub;   // maior numero sorteado em soma e subtracao
    int max_tabuada;    // maior fator na multiplicacao
    int tipos_1grau;    // quantos tipos de 1o grau podem sair (1 a 5)
    int max_1grau;      // maior valor de b (e de x nos tipos 0 e 1)
    int max_a;          // maior coeficiente 'a' (minimo 2)
    int max_x;          // maior valor de x nos tipos com coeficiente
} ConfigNivel;

static const ConfigNivel CONFIG[NIVEL_MAX] = {
    //  soma sub mult  maxSS tab tipos max1g  maxA maxX
    { 50, 100, 100,   10,  0,   0,    0,    0,   0 },  // nivel 1: so soma e subtracao
    { 35,  70, 100,   20,  5,   0,    0,    0,   0 },  // nivel 2: + tabuada ate 5
    { 25,  50,  80,   30, 10,   3,   10,    9,  10 },  // nivel 3: + 1o grau simples
    { 15,  30,  65,   50, 10,   5,   15,    9,  10 },  // nivel 4: 1o grau completo
    { 10,  20,  50,   99, 12,   5,   20,    9,  10 },  // nivel 5: numeros maiores
    { 8,  16,  40,  120, 12,   5,   25,   10,  12 },  // nivel 6
    { 5,  10,  30,  150, 15,   5,   30,   12,  12 },  // nivel 7
    { 5,  10,  25,  200, 15,   5,   40,   12,  15 },  // nivel 8
    { 3,   6,  20,  300, 20,   5,   50,   15,  15 },  // nivel 9
    { 2,   4,  15,  500, 20,   5,   60,   15,  20 },  // nivel 10: quase so 1o grau
};

// Numero entre 1 e max
static int sortear(int max) {
    return (rand() % max) + 1;
}

// Coeficiente 'a' entre 2 e max_a
static int sortear_coeficiente(const ConfigNivel* cfg) {
    return (rand() % (cfg->max_a - 1)) + 2;
}

// Equacao de 1o grau com x inteiro. Guarda a e b em num1 e num2.
static void equacao_gerar_primeiro_grau(Equacao* eq, const ConfigNivel* cfg) {
    int m = cfg->max_1grau;
    int tipo = rand() % cfg->tipos_1grau;
    int x, a = 1, b = 0, c;

    switch (tipo) {
    case 0: // x + b = c
        x = sortear(m);
        b = sortear(m);
        c = x + b;
        snprintf(eq->texto, sizeof(eq->texto), "x + %d = %d", b, c);
        break;
    case 1: // x - b = c
        b = sortear(m);
        c = sortear(m);
        x = c + b;
        snprintf(eq->texto, sizeof(eq->texto), "x - %d = %d", b, c);
        break;
    case 2: // a*x = c
        a = sortear_coeficiente(cfg);
        x = sortear(cfg->max_x);
        c = a * x;
        snprintf(eq->texto, sizeof(eq->texto), "%dx = %d", a, c);
        break;
    case 3: // a*x + b = c
        a = sortear_coeficiente(cfg);
        x = sortear(cfg->max_x);
        b = sortear(m);
        c = a * x + b;
        snprintf(eq->texto, sizeof(eq->texto), "%dx + %d = %d", a, b, c);
        break;
    default: // a*x - b = c
        a = sortear_coeficiente(cfg);
        x = sortear(cfg->max_x);
        b = (rand() % (a * x - 1)) + 1; // b menor que a*x, assim c fica positivo
        c = a * x - b;
        snprintf(eq->texto, sizeof(eq->texto), "%dx - %d = %d", a, b, c);
        break;
    }

    eq->num1 = a;
    eq->num2 = b;
    eq->resposta_correta = x;
}

int equacao_nivel_para_acertos(int acertos) {
    int nivel = 1 + acertos / ACERTOS_POR_NIVEL;
    if (nivel > NIVEL_MAX) nivel = NIVEL_MAX;
    return nivel;
}

void equacao_gerar_nova(Equacao* eq, int nivel) {
    if (!eq) return;

    if (nivel < 1) nivel = 1;
    if (nivel > NIVEL_MAX) nivel = NIVEL_MAX;
    const ConfigNivel* cfg = &CONFIG[nivel - 1];

    int sorteio = rand() % 100;

    if (sorteio < cfg->limite_soma) {
        eq->num1 = sortear(cfg->max_soma_sub);
        eq->num2 = sortear(cfg->max_soma_sub);
        eq->resposta_correta = eq->num1 + eq->num2;
        snprintf(eq->texto, sizeof(eq->texto), "%d + %d = ?", eq->num1, eq->num2);
    }
    else if (sorteio < cfg->limite_sub) {
        eq->num1 = sortear(cfg->max_soma_sub);
        eq->num2 = sortear(cfg->max_soma_sub);
        // o maior vai na frente pra resposta nunca ser negativa
        if (eq->num2 > eq->num1) {
            int tmp = eq->num1;
            eq->num1 = eq->num2;
            eq->num2 = tmp;
        }
        eq->resposta_correta = eq->num1 - eq->num2;
        snprintf(eq->texto, sizeof(eq->texto), "%d - %d = ?", eq->num1, eq->num2);
    }
    else if (sorteio < cfg->limite_mult) {
        eq->num1 = sortear(cfg->max_tabuada);
        eq->num2 = sortear(cfg->max_tabuada);
        eq->resposta_correta = eq->num1 * eq->num2;
        snprintf(eq->texto, sizeof(eq->texto), "%d x %d = ?", eq->num1, eq->num2);
    }
    else {
        equacao_gerar_primeiro_grau(eq, cfg);
    }

    eq->ativa = true;
}

void equacao_desenhar(const Equacao* eq, ALLEGRO_FONT* fonte, int largura_tela, int altura_tela) {
    if (!eq || !eq->ativa || !fonte) return;

    float cx = largura_tela * QUADRO_CENTRO_X;
    float cy = altura_tela * QUADRO_CENTRO_Y;
    float meia_l = largura_tela * QUADRO_LARGURA / 2.0f;
    float meia_a = altura_tela * QUADRO_ALTURA / 2.0f;

    // fundo de madeira escura e moldura avermelhada
    al_draw_filled_rectangle(cx - meia_l, cy - meia_a, cx + meia_l, cy + meia_a, al_map_rgb(45, 30, 20));
    al_draw_rectangle(cx - meia_l, cy - meia_a, cx + meia_l, cy + meia_a, al_map_rgb(128, 34, 34), 3.0f);

    float texto_y = cy - al_get_font_line_height(fonte) / 2.0f;
    al_draw_text(fonte, al_map_rgb(221, 144, 54), cx, texto_y, ALLEGRO_ALIGN_CENTER, eq->texto);
}