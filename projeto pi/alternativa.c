#define _CRT_SECURE_NO_WARNINGS

#include "alternativa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <allegro5/allegro_primitives.h>

// Cria a pergunta com os textos passados
void pergunta_criar(Pergunta* p, const char* enunciado,
    const char* textos[], int qtd, int idx_correta,
    float largura_tela, float y, float altura)
{
    if (!p) return;
    strncpy(p->enunciado, enunciado, sizeof(p->enunciado) - 1);
    p->qtd = qtd;
    p->respondida = false;

    float espaco = 20.0f;

    float largura_botao = (largura_tela - (espaco * (qtd + 1))) / qtd;
    if (largura_botao > 220.0f) largura_botao = 220.0f;

    for (int i = 0; i < qtd; i++) {
        p->alt[i].x = espaco + (i * (largura_botao + espaco));
        p->alt[i].x = (largura_tela - (qtd * largura_botao + (qtd - 1) * espaco)) / 2.0f + i * (largura_botao + espaco);

        p->alt[i].y = y;
        p->alt[i].w = largura_botao;
        p->alt[i].h = altura;

        strncpy(p->alt[i].texto, textos[i], sizeof(p->alt[i].texto) - 1);
        p->alt[i].correta = (i == idx_correta);
        p->alt[i].estado = ALT_NORMAL;
    }
}

// Monta as 4 alternativas a partir da equação
void pergunta_de_equacao(Pergunta* p, const Equacao* eq,
    float largura_tela, float y, float altura)
{
    if (!p || !eq) return;

    char textos[4][64];
    int idx_correta = rand() % 4;
    int resposta_certa = eq->resposta_correta;

    for (int i = 0; i < 4; i++) {
        if (i == idx_correta) {
            snprintf(textos[i], sizeof(textos[i]), "%d", resposta_certa);
        }
        else {
            int erro = (rand() % 11) - 5;
            if (erro == 0) erro = 1;
            int resposta_errada = resposta_certa + erro;
            if (resposta_errada < 0) resposta_errada = resposta_certa + 1;
            snprintf(textos[i], sizeof(textos[i]), "%d", resposta_errada);
        }
    }

    const char* ptrs[4] = { textos[0], textos[1], textos[2], textos[3] };
    pergunta_criar(p, eq->texto, ptrs, 4, idx_correta, largura_tela, y, altura);
}

// Testa colisão do tiro com as alternativas
int pergunta_testar_tiro(Pergunta* p, float tx, float ty, float tw, float th)
{
    if (!p || p->respondida) return -1;

    for (int i = 0; i < p->qtd; i++) {
        if (tx >= p->alt[i].x && tx <= p->alt[i].x + p->alt[i].w &&
            ty >= p->alt[i].y && ty <= p->alt[i].y + p->alt[i].h)
        {
            p->respondida = true;
            if (p->alt[i].correta) {
                p->alt[i].estado = ALT_CERTA;
                return 1; // Acertou
            }
            else {
                p->alt[i].estado = ALT_ERRADA;
                // Marca a certa de verde também, para o jogador ver qual era a correta
                for (int j = 0; j < p->qtd; j++) {
                    if (p->alt[j].correta) p->alt[j].estado = ALT_CERTA;
                }
                return 0; // Errou
            }
        }
    }
    return -1;
}

// Desenha as alternativas com o texto colorido
void pergunta_desenhar(const Pergunta* p, ALLEGRO_FONT* fonte, float largura_tela, ALLEGRO_BITMAP* escudo) {
    if (p == NULL || fonte == NULL) return;

    float img_w = 0, img_h = 0;
    if (escudo != NULL) {
        img_w = al_get_bitmap_width(escudo);
        img_h = al_get_bitmap_height(escudo);
    }

    for (int i = 0; i < p->qtd; i++) {
        float cx = p->alt[i].x;
        float cy = p->alt[i].y;
        float w = p->alt[i].w;
        float h = p->alt[i].h;

        // 1. Desenha o Escudo (mantendo o branco original)
        if (escudo != NULL) {
            al_draw_scaled_bitmap(escudo,
                0, 0, img_w, img_h,
                cx, cy, w, h,
                0
            );
        }
        else {
            // Fallback (caso a imagem não carregue)
            al_draw_filled_rectangle(cx, cy, cx + w, cy + h, al_map_rgb(45, 45, 150));
        }

        // 2. Define a cor do texto baseado no estado
        ALLEGRO_COLOR cor_texto = al_map_rgb(255, 255, 255); // Branco padrão

        if (p->respondida) {
            if (p->alt[i].estado == ALT_CERTA) {
                cor_texto = al_map_rgb(50, 255, 50); // Verde neon
            }
            else if (p->alt[i].estado == ALT_ERRADA) {
                cor_texto = al_map_rgb(255, 50, 50); // Vermelho vivo
            }
        }

        // 3. Desenha o Texto por cima do Escudo
        int altura_fonte = al_get_font_line_height(fonte);
        float texto_x = cx + (w / 2.0f);
        float texto_y = cy + (h / 2.0f) - (altura_fonte / 2.0f);

        // Sombra preta para dar contraste
        al_draw_text(fonte, al_map_rgb(0, 0, 0), texto_x + 2, texto_y + 2, ALLEGRO_ALIGN_CENTER, p->alt[i].texto);

        // Número colorido (Verde, Vermelho ou Branco)
        al_draw_text(fonte, cor_texto, texto_x, texto_y, ALLEGRO_ALIGN_CENTER, p->alt[i].texto);
    }
}