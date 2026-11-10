#include "jogo.h"
#include <math.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#define CORACAO_LARGURA 9
#define CORACAO_ALTURA 8
#define ESPACO_ENTRE_CORACOES 8

// Borda vermelha que pisca quando sobra so uma vida
#define ALERTA_ESPESSURA 40
#define ALERTA_ALPHA_MIN 0.08
#define ALERTA_ALPHA_MAX 0.35
#define ALERTA_PULSOS_POR_SEGUNDO 1.5
#define PI 3.14159265358979

// Tamanho de cada pixel do sprite na tela (5 = coracao de 45x40)
#define ESCALA_PIXEL 5

// Sprite do coracao em pixel art, simetrico.
// # contorno, o preenchimento, b brilho, . transparente
static const char* sprite_coracao[CORACAO_ALTURA] = {
    ".##...##.",
    "#bbo.#oo#",
    "#boo#ooo#",
    "#ooooooo#",
    ".#ooooo#.",
    "..#ooo#..",
    "...#o#...",
    "....#...."
};

// x e y sao o canto superior esquerdo. Se cheio for false desenha o coracao apagado (vida perdida).
static void desenhar_coracao(float x, float y, bool cheio) {
    ALLEGRO_COLOR contorno = cheio ? al_map_rgb(90, 10, 20) : al_map_rgb(30, 30, 30);
    ALLEGRO_COLOR preenchimento = cheio ? al_map_rgb(220, 30, 50) : al_map_rgb(70, 70, 70);
    ALLEGRO_COLOR brilho = cheio ? al_map_rgb(255, 150, 160) : al_map_rgb(100, 100, 100);

    for (int linha = 0; linha < CORACAO_ALTURA; linha++) {
        for (int coluna = 0; coluna < CORACAO_LARGURA; coluna++) {
            char pixel = sprite_coracao[linha][coluna];
            ALLEGRO_COLOR cor;

            if (pixel == '#') cor = contorno;
            else if (pixel == 'o') cor = preenchimento;
            else if (pixel == 'b') cor = brilho;
            else continue;

            float px = x + coluna * ESCALA_PIXEL;
            float py = y + linha * ESCALA_PIXEL;
            al_draw_filled_rectangle(px, py, px + ESCALA_PIXEL, py + ESCALA_PIXEL, cor);
        }
    }
}

void vidas_desenhar(int vidas_atuais) {
    const float espacamento = CORACAO_LARGURA * ESCALA_PIXEL + ESPACO_ENTRE_CORACOES;
    const float inicio_x = 10;
    const float inicio_y = 10;

    // sempre desenha o total de vidas, os que ja foram perdidos saem apagados
    for (int i = 0; i < VIDAS_INICIAIS; i++) {
        desenhar_coracao(inicio_x + i * espacamento, inicio_y, i < vidas_atuais);
    }
}

void alerta_vida_desenhar(int vidas_atuais) {
    if (vidas_atuais != 1) return;

    // o seno vai de -1 a 1, aqui vira 0 a 1
    double onda = 0.5 + 0.5 * sin(2.0 * PI * ALERTA_PULSOS_POR_SEGUNDO * al_get_time());
    float alpha_base = (float)(ALERTA_ALPHA_MIN + (ALERTA_ALPHA_MAX - ALERTA_ALPHA_MIN) * onda);

    // cada camada e mais fraca que a de fora, assim a borda some suave pra dentro
    for (int i = 0; i < ALERTA_ESPESSURA; i++) {
        float alpha = alpha_base * (1.0f - (float)i / ALERTA_ESPESSURA);
        // o Allegro 5.2 usa alpha premultiplicado, por isso o vermelho tambem leva o alpha
        ALLEGRO_COLOR cor = al_map_rgba_f(alpha, 0.0f, 0.0f, alpha);
        al_draw_rectangle(i + 0.5f, i + 0.5f, LARGURA_TELA - i - 0.5f, ALTURA_TELA - i - 0.5f, cor, 1.0f);
    }
}