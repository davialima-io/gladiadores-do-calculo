#include "equacao.h"
#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro_primitives.h>

//protótipo só pra não ter erro de compilação pq a função precisa ser declarada antes de ser chamada na
static void equacao_gerar_primeiro_grau(Equacao* eq);

void equacao_gerar_nova(Equacao* eq) {
    if (eq == NULL) return;

    //sorteia uma % pra cair cada tipo de equacao
    int sorteio = rand() % 100;

    printf("sorteio = %d\n", sorteio);

    //40% de soma
    if (sorteio< 40) {
        // Dois números aleatórios entre 1 e 20
        eq->num1 = (rand() % 20) + 1;
        eq->num2 = (rand() % 20) + 1;
        eq->resposta_correta = eq->num1 + eq->num2;

        snprintf(eq->texto, sizeof(eq->texto), "%d + %d = ?", eq->num1, eq->num2);
    }
    //30% de chance de cair subtração
    else if (sorteio < 70) {
        eq->num1 = rand() % 20 + 1;
        eq->num2 = rand() % 20 + 1;
        if (eq->num2 > eq->num1) {
            int tmp = eq->num1;
            eq->num1 = eq->num2;
            eq->num2 = tmp;
        }
        eq->resposta_correta = eq->num1 - eq->num2;
        snprintf(eq->texto, sizeof(eq->texto), "%d - %d = ?", eq->num1, eq->num2);
    }
    else if(sorteio < 90){
        // MULTIPLICAÇÃO (20%)
        eq->num1 = (rand() % 10) + 1;   // só tabuada
        eq->num2 = (rand() % 10) + 1;
        eq->resposta_correta = eq->num1 * eq->num2;
        snprintf(eq->texto, sizeof(eq->texto), "%d x %d = ?", eq->num1, eq->num2);
    }
    //10% de 1° grau
    else {
        equacao_gerar_primeiro_grau(eq);
    }
    
    eq->ativa = true;
}

// Posição e tamanho do quadro (proporções da tela).
#define QUADRO_CENTRO_X  0.50f
#define QUADRO_CENTRO_Y  0.334f
#define QUADRO_LARGURA   0.17f
#define QUADRO_ALTURA    0.14f

void equacao_desenhar(const Equacao* eq, ALLEGRO_FONT* fonte, int largura_tela, int altura_tela) {
    if (eq == NULL || !eq->ativa || fonte == NULL) return;

    float cx = largura_tela * QUADRO_CENTRO_X;
    float cy = altura_tela * QUADRO_CENTRO_Y;
    float meia_l = largura_tela * QUADRO_LARGURA / 2.0f;
    float meia_a = altura_tela * QUADRO_ALTURA / 2.0f;

    // Fundo do quadro (madeira escura)
    al_draw_filled_rectangle(
        cx - meia_l, cy - meia_a,
        cx + meia_l, cy + meia_a,
        al_map_rgb(45, 30, 20)
    );

    // Moldura dourada
    al_draw_rectangle(
        cx - meia_l, cy - meia_a,
        cx + meia_l, cy + meia_a,
        al_map_rgb(128, 34, 34), 3.0f
    );

    // Texto centralizado no quadro
    int altura_fonte = al_get_font_line_height(fonte);
    al_draw_text(fonte, al_map_rgb(221, 144, 54), cx, cy - altura_fonte / 2.0f, ALLEGRO_ALIGN_CENTER, eq->texto);
}


// Gera uma equação de 1º grau com resposta inteira (x)
static void equacao_gerar_primeiro_grau(Equacao* eq) {
    int tipo = rand() % 5;
    int x, a=1, b=0, c;

    switch (tipo) {
    case 0: // x + b = c
        x = (rand() % 20) + 1;
        b = (rand() % 20) + 1;
        c = x + b;
        snprintf(eq->texto, sizeof(eq->texto), "x + %d = %d", b, c);
        break;

    case 1: // x - b = c
        b = (rand() % 15) + 1;
        c = (rand() % 15) + 1;
        x = c + b;
        snprintf(eq->texto, sizeof(eq->texto), "x - %d = %d", b, c);
        break;

    case 2: // a*x = c
        a = (rand() % 8) + 2;          // 2..9
        x = (rand() % 10) + 1;
        c = a * x;
        snprintf(eq->texto, sizeof(eq->texto), "%dx = %d", a, c);
        break;

    case 3: // a*x + b = c
        a = (rand() % 8) + 2;
        x = (rand() % 10) + 1;
        b = (rand() % 15) + 1;
        c = a * x + b;
        snprintf(eq->texto, sizeof(eq->texto), "%dx + %d = %d", a, b, c);
        break;

    default: // a*x - b = c
        a = (rand() % 8) + 2;
        x = (rand() % 10) + 1;
        b = (rand() % (a * x - 1)) + 1; // garante c > 0
        c = a * x - b;
        snprintf(eq->texto, sizeof(eq->texto), "%dx - %d = %d", a, b, c);
        break;
    }

    eq->num1 = a;                 // (opcional, só pra manter algo preenchido)
    eq->num2 = b;
    eq->resposta_correta = x;
}