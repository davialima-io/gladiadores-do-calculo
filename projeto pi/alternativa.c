#define _CRT_SECURE_NO_WARNINGS

#include "alternativa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <allegro5/allegro_primitives.h>

#define ESPACO_ENTRE_BOTOES 20.0f
#define LARGURA_MAX_BOTAO 140.0f
#define TENTATIVAS_ANTES_DE_AUMENTAR 30

// Copia o texto garantindo o \0 no final, o strncpy sozinho nao garante
static void copiar_texto(char* destino, size_t tamanho, const char* origem) {
    strncpy(destino, origem, tamanho - 1);
    destino[tamanho - 1] = '\0';
}

void pergunta_criar(Pergunta* p, const char* enunciado, const char* textos[], int qtd, int idx_correta, float largura_tela, float y, float altura) {
    if (!p || qtd > MAX_ALTERNATIVAS) return;
    copiar_texto(p->enunciado, sizeof(p->enunciado), enunciado);
    p->qtd = qtd;
    p->respondida = false;

    float largura_botao = (largura_tela - ESPACO_ENTRE_BOTOES * (qtd + 1)) / qtd;
    if (largura_botao > LARGURA_MAX_BOTAO) largura_botao = LARGURA_MAX_BOTAO;

    // centraliza o grupo de botoes na tela
    float largura_total = qtd * largura_botao + (qtd - 1) * ESPACO_ENTRE_BOTOES;
    float x_inicial = (largura_tela - largura_total) / 2.0f;

    for (int i = 0; i < qtd; i++) {
        p->alt[i].x = x_inicial + i * (largura_botao + ESPACO_ENTRE_BOTOES);
        p->alt[i].y = y;
        p->alt[i].w = largura_botao;
        p->alt[i].h = altura;
        copiar_texto(p->alt[i].texto, sizeof(p->alt[i].texto), textos[i]);
        p->alt[i].correta = (i == idx_correta);
        p->alt[i].estado = ALT_NORMAL;
    }
}

// Distancia maxima entre a resposta errada e a certa, por nivel.
// Nivel baixo: erradas longe da certa. Nivel alto: erradas coladas nela.
static int max_erro_por_nivel(int nivel) {
    // minimo 2, com 1 so existem 2 valores possiveis e precisamos de 3 erradas
    static const int tabela[NIVEL_MAX] = { 10, 8, 5, 3, 2, 2, 2, 2, 2, 2 };

    if (nivel < 1) nivel = 1;
    if (nivel > NIVEL_MAX) nivel = NIVEL_MAX;
    return tabela[nivel - 1];
}

// Sorteia uma alternativa errada que nao seja negativa nem repita as que ja existem
static int sortear_alternativa_errada(int certa, int max_erro, const int usados[], int qtd_usados) {
    int tentativas = 0;

    for (;;) {
        int candidata = certa + (rand() % (2 * max_erro + 1)) - max_erro;
        bool repetida = false;

        for (int k = 0; k < qtd_usados; k++) {
            if (usados[k] == candidata) repetida = true;
        }
        if (candidata >= 0 && !repetida) return candidata;

        // com resposta perto de 0 o intervalo pode ser pequeno demais, entao vai abrindo
        tentativas++;
        if (tentativas >= TENTATIVAS_ANTES_DE_AUMENTAR) {
            max_erro++;
            tentativas = 0;
        }
    }
}

void pergunta_de_equacao(Pergunta* p, const Equacao* eq, float largura_tela, float y, float altura, int nivel) {
    if (!p || !eq) return;

    int certa = eq->resposta_correta;
    int idx_correta = rand() % 4;
    int max_erro = max_erro_por_nivel(nivel);

    int valores[4];
    int usados[4];
    int qtd_usados = 0;

    valores[idx_correta] = certa;
    usados[qtd_usados++] = certa;

    for (int i = 0; i < 4; i++) {
        if (i == idx_correta) continue;
        valores[i] = sortear_alternativa_errada(certa, max_erro, usados, qtd_usados);
        usados[qtd_usados++] = valores[i];
    }

    char textos[4][64];
    const char* ponteiros[4];
    for (int i = 0; i < 4; i++) {
        snprintf(textos[i], sizeof(textos[i]), "%d", valores[i]);
        ponteiros[i] = textos[i];
    }

    pergunta_criar(p, eq->texto, ponteiros, 4, idx_correta, largura_tela, y, altura);
}

// A area que conta como acerto e menor que o botao, porque o escudo tem borda vazia
static bool tiro_acertou_botao(const Alternativa* a, float tx, float ty) {
    float margem_x = a->w * 0.25f;
    float margem_y = a->h * 0.20f;
    float esquerda = a->x + margem_x;
    float topo = a->y + margem_y;
    float direita = a->x + a->w - margem_x;
    float baixo = a->y + a->h - margem_y;

    return tx >= esquerda && tx <= direita && ty >= topo && ty <= baixo;
}

int pergunta_testar_tiro(Pergunta* p, float tx, float ty) {
    if (!p || p->respondida) return -1;

    for (int i = 0; i < p->qtd; i++) {
        if (!tiro_acertou_botao(&p->alt[i], tx, ty)) continue;

        p->respondida = true;
        if (p->alt[i].correta) {
            p->alt[i].estado = ALT_CERTA;
            return 1;
        }

        p->alt[i].estado = ALT_ERRADA;
        // mostra qual era a certa pro jogador aprender
        for (int j = 0; j < p->qtd; j++) {
            if (p->alt[j].correta) p->alt[j].estado = ALT_CERTA;
        }
        return 0;
    }
    return -1;
}

static void desenhar_escudo(const Alternativa* a, ALLEGRO_BITMAP* escudo) {
    if (!escudo) {
        al_draw_filled_rectangle(a->x, a->y, a->x + a->w, a->y + a->h, al_map_rgb(45, 45, 150));
        return;
    }

    float img_w = al_get_bitmap_width(escudo);
    float img_h = al_get_bitmap_height(escudo);

    // encolhe a imagem ate caber no botao sem deformar
    float escala = a->w / img_w;
    if (a->h / img_h < escala) escala = a->h / img_h;

    float w = img_w * escala;
    float h = img_h * escala;
    float x = a->x + (a->w - w) / 2.0f;
    float y = a->y + (a->h - h) / 2.0f;
    al_draw_scaled_bitmap(escudo, 0, 0, img_w, img_h, x, y, w, h, 0);
}

static ALLEGRO_COLOR cor_do_texto(const Pergunta* p, const Alternativa* a) {
    if (p->respondida && a->estado == ALT_CERTA) return al_map_rgb(50, 255, 50);
    if (p->respondida && a->estado == ALT_ERRADA) return al_map_rgb(255, 50, 50);
    return al_map_rgb(255, 255, 255);
}

void pergunta_desenhar(const Pergunta* p, ALLEGRO_FONT* fonte, ALLEGRO_BITMAP* escudo) {
    if (!p || !fonte) return;

    for (int i = 0; i < p->qtd; i++) {
        const Alternativa* a = &p->alt[i];
        desenhar_escudo(a, escudo);

        // o +10 empurra o numero pro meio do escudo, que tem a ponta embaixo
        float texto_x = a->x + a->w / 2.0f;
        float texto_y = a->y + a->h / 2.0f - al_get_font_line_height(fonte) / 2.0f + 10.0f;
        al_draw_text(fonte, al_map_rgb(0, 0, 0), texto_x + 2, texto_y + 2, ALLEGRO_ALIGN_CENTER, a->texto);
        al_draw_text(fonte, cor_do_texto(p, a), texto_x, texto_y, ALLEGRO_ALIGN_CENTER, a->texto);
    }
}