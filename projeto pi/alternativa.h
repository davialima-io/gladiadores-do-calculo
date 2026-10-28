#pragma once
#ifndef ALTERNATIVAS_H
#define ALTERNATIVAS_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <stdbool.h>
#include "equacao.h"

#define MAX_ALTERNATIVAS 4

typedef enum { ALT_NORMAL, ALT_CERTA, ALT_ERRADA } EstadoAlt;

typedef struct {
    float x, y, w, h;
    char texto[64];
    bool correta;
    EstadoAlt estado;
} Alternativa;

typedef struct {
    char enunciado[128];
    Alternativa alt[MAX_ALTERNATIVAS];
    int qtd;
    bool respondida;
} Pergunta;

void pergunta_criar(Pergunta* p, const char* enunciado,
    const char* textos[], int qtd, int idx_correta,
    float largura_tela, float y, float altura);

void pergunta_desenhar(const Pergunta* p, ALLEGRO_FONT* fonte, float largura_tela, ALLEGRO_BITMAP* escudo);

// Retorna: 1 = acertou, 0 = errou, -1 = o tiro nao atingiu nenhum botao
int pergunta_testar_tiro(Pergunta* p, float tx, float ty, float tw, float th);

// Monta as 4 alternativas a partir da equacao.
// Quanto maior o nivel, mais perto da resposta certa ficam as erradas.
void pergunta_de_equacao(Pergunta* p, const Equacao* eq,
    float largura_tela, float y, float altura, int nivel);

#endif