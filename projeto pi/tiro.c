#include "tiro.h"
#include "jogo.h"
#include <allegro5/allegro_primitives.h>
#include <math.h>

#define VELOCIDADE_TIRO 10
#define RAIO_TIRO 5
// O tiro nasce um pouco na frente do jogador pra nao comecar dentro dele
#define DISTANCIA_INICIAL 30

void tiros_inicializar(Tiro tiros[]) {
    for (int i = 0; i < MAX_TIROS; i++) {
        tiros[i].ativo = false;
    }
}

void tiro_disparar(Tiro tiros[], float jogador_x, float jogador_y, float mouse_x, float mouse_y) {
    float dx = mouse_x - jogador_x;
    float dy = mouse_y - jogador_y;
    float distancia = sqrtf(dx * dx + dy * dy);

    // mouse bem em cima do jogador, nao tem direcao pra atirar
    if (distancia == 0) return;

    float direcao_x = dx / distancia;
    float direcao_y = dy / distancia;

    for (int i = 0; i < MAX_TIROS; i++) {
        if (tiros[i].ativo) continue;

        tiros[i].x = jogador_x + direcao_x * DISTANCIA_INICIAL;
        tiros[i].y = jogador_y + direcao_y * DISTANCIA_INICIAL;
        tiros[i].velocidade = VELOCIDADE_TIRO;
        tiros[i].direcao_x = direcao_x;
        tiros[i].direcao_y = direcao_y;
        tiros[i].ativo = true;
        return;
    }
}

void tiros_atualizar(Tiro tiros[]) {
    for (int i = 0; i < MAX_TIROS; i++) {
        Tiro* t = &tiros[i];
        if (!t->ativo) continue;

        t->x += t->direcao_x * t->velocidade;
        t->y += t->direcao_y * t->velocidade;

        // saiu da tela, libera a vaga no vetor
        if (t->x < 0 || t->x > LARGURA_TELA || t->y < 0 || t->y > ALTURA_TELA) t->ativo = false;
    }
}

void tiros_desenhar(Tiro tiros[]) {
    for (int i = 0; i < MAX_TIROS; i++) {
        if (!tiros[i].ativo) continue;
        al_draw_filled_circle(tiros[i].x, tiros[i].y, RAIO_TIRO, al_map_rgb(255, 220, 0));
    }
}