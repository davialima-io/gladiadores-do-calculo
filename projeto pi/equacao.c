#include "equacao.h"
#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro_primitives.h>

void equacao_gerar_nova(Equacao* eq) {
    if (eq == NULL) return;

    // Dois números aleatórios entre 1 e 20
    // (srand deve ser chamado uma vez no main)
    eq->num1 = (rand() % 20) + 1;
    eq->num2 = (rand() % 20) + 1;
    eq->resposta_correta = eq->num1 + eq->num2;

    snprintf(eq->texto, sizeof(eq->texto), "%d + %d = ?", eq->num1, eq->num2);
    eq->ativa = true;
}

// Posição e tamanho do quadro (proporções da tela).
// Ajuste esses valores até cobrir a bandeira.
#define QUADRO_CENTRO_X  0.50f
#define QUADRO_CENTRO_Y  0.39f
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
        al_map_rgb(200, 160, 60), 3.0f
    );

    // Texto centralizado no quadro
    int altura_fonte = al_get_font_line_height(fonte);
    al_draw_text(
        fonte,
        al_map_rgb(255, 255, 0),
        cx,
        cy - altura_fonte / 2.0f,
        ALLEGRO_ALIGN_CENTER,
        eq->texto
    );
}