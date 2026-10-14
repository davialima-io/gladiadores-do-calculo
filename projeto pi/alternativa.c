#include "alternativa.h"
#include <allegro5/allegro_primitives.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void pergunta_criar(Pergunta* p, const char* enunciado,
    const char* textos[], int qtd, int idx_correta,
    float largura_tela, float y, float altura)
{
    if (qtd > MAX_ALTERNATIVAS) qtd = MAX_ALTERNATIVAS;

    strncpy_s(p->enunciado, sizeof(p->enunciado), enunciado, _TRUNCATE);
    p->qtd = qtd;
    p->respondida = false;

    float margem = 20.0f;
    float w = (largura_tela - margem * (qtd + 1)) / qtd;

    for (int i = 0; i < qtd; i++) {
        Alternativa* a = &p->alt[i];
        a->x = margem + i * (w + margem);
        a->y = y;
        a->w = w;
        a->h = altura;
        a->correta = (i == idx_correta);
        a->estado = ALT_NORMAL;
        strncpy_s(a->texto, sizeof(a->texto), textos[i], _TRUNCATE);
    }
}

void pergunta_desenhar(const Pergunta* p, ALLEGRO_FONT* fonte, float largura_tela)
{
    al_draw_text(fonte, al_map_rgb(255, 255, 255), largura_tela / 2, 20,
        ALLEGRO_ALIGN_CENTRE, p->enunciado);

    for (int i = 0; i < p->qtd; i++) {
        const Alternativa* a = &p->alt[i];

        ALLEGRO_COLOR cor = al_map_rgb(60, 60, 120);          // normal
        if (a->estado == ALT_CERTA)  cor = al_map_rgb(40, 160, 60);
        if (a->estado == ALT_ERRADA) cor = al_map_rgb(190, 40, 40);

        al_draw_filled_rectangle(a->x, a->y, a->x + a->w, a->y + a->h, cor);
        al_draw_rectangle(a->x, a->y, a->x + a->w, a->y + a->h,
            al_map_rgb(255, 255, 255), 2);
        al_draw_text(fonte, al_map_rgb(255, 255, 255),
            a->x + a->w / 2, a->y + a->h / 2 - al_get_font_line_height(fonte) / 2,
            ALLEGRO_ALIGN_CENTRE, a->texto);
    }
}

int pergunta_testar_tiro(Pergunta* p, float tx, float ty, float tw, float th)
{
    if (p->respondida) return -1;

    for (int i = 0; i < p->qtd; i++) {
        Alternativa* a = &p->alt[i];

        bool colidiu = tx < a->x + a->w && tx + tw > a->x &&
            ty < a->y + a->h && ty + th > a->y;
        if (!colidiu) continue;

        p->respondida = true;
        if (a->correta) {
            a->estado = ALT_CERTA;
            return 1;
        }
        a->estado = ALT_ERRADA;
        // mostra qual era a certa
        for (int j = 0; j < p->qtd; j++)
            if (p->alt[j].correta) p->alt[j].estado = ALT_CERTA;
        return 0;
    }
    return -1;
}

void pergunta_de_equacao(Pergunta* p, const Equacao* eq,
    float largura_tela, float y, float altura)
{
    int valores[MAX_ALTERNATIVAS];
    valores[0] = eq->resposta_correta;
    int n = 1;

    // gera 3 respostas erradas, diferentes entre si e da certa
    while (n < MAX_ALTERNATIVAS) {
        int off = (rand() % 10) + 1;          // 1..10
        if (rand() % 2) off = -off;
        int v = eq->resposta_correta + off;
        if (v < 0) continue;

        bool repetido = false;
        for (int i = 0; i < n; i++)
            if (valores[i] == v) repetido = true;
        if (repetido) continue;

        valores[n++] = v;
    }

    // embaralha (Fisher-Yates); a certa começa no índice 0
    int idx_correta = 0;
    for (int i = MAX_ALTERNATIVAS - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = valores[i]; valores[i] = valores[j]; valores[j] = tmp;
    }
    for (int i = 0; i < MAX_ALTERNATIVAS; i++)
        if (valores[i] == eq->resposta_correta) idx_correta = i;

    char bufs[MAX_ALTERNATIVAS][16];
    const char* textos[MAX_ALTERNATIVAS];
    for (int i = 0; i < MAX_ALTERNATIVAS; i++) {
        snprintf(bufs[i], sizeof(bufs[i]), "%d", valores[i]);
        textos[i] = bufs[i];
    }

    // enunciado vazio: a equação já é desenhada pelo equacao_desenhar
    pergunta_criar(p, "", textos, MAX_ALTERNATIVAS, idx_correta,
        largura_tela, y, altura);
}