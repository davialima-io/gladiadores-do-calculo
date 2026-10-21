#ifndef EQUACAO_H
#define EQUACAO_H

#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#define EQUACAO_TEXTO_MAX 32

typedef struct {
    int num1;
    int num2;
    int resposta_correta;
    char texto[EQUACAO_TEXTO_MAX];
    bool ativa;
} Equacao;

void equacao_gerar_nova(Equacao* eq);
void equacao_desenhar(const Equacao* eq, ALLEGRO_FONT* fonte, int largura_tela, int altura_tela);

#endif