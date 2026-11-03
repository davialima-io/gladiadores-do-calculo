#ifndef TIRO_H
#define TIRO_H

#include <stdbool.h>

#define MAX_TIROS 50

// Projetil disparado pelo jogador
typedef struct {
    float x;
    float y;
    float velocidade;
    float direcao_x;   // direcao ja normalizada (tamanho 1)
    float direcao_y;
    bool ativo;
} Tiro;

void tiros_inicializar(Tiro tiros[]);

// Cria um tiro saindo do jogador em direcao ao mouse. Se nao tiver vaga no vetor, ignora o clique.
void tiro_disparar(Tiro tiros[], float jogador_x, float jogador_y, float mouse_x, float mouse_y);

void tiros_atualizar(Tiro tiros[]);
void tiros_desenhar(Tiro tiros[]);

#endif