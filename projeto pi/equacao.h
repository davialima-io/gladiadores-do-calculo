#ifndef EQUACAO_H
#define EQUACAO_H

#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#define EQUACAO_TEXTO_MAX 32

// Progressao de dificuldade
#define NIVEL_MAX 10         // ultimo nivel
#define ACERTOS_POR_NIVEL 3  // acertos necessarios para subir de nivel

typedef struct {
    int num1;
    int num2;
    int resposta_correta;
    char texto[EQUACAO_TEXTO_MAX];
    bool ativa;
} Equacao;

// Converte total de acertos em nivel (1..NIVEL_MAX)
int equacao_nivel_para_acertos(int acertos);

// Gera uma equacao de acordo com o nivel (1..NIVEL_MAX)
void equacao_gerar_nova(Equacao* eq, int nivel);

void equacao_desenhar(const Equacao* eq, ALLEGRO_FONT* fonte, int largura_tela, int altura_tela);

#endif