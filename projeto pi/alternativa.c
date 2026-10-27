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
    if (largura_botao > 140.0f) largura_botao = 140.0f;

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

// Distancia maxima entre a resposta errada e a certa, por nivel.
// Nivel baixo: erradas bem longe (facil). Nivel alto: erradas coladas na certa.
static int max_erro_por_nivel(int nivel)
{
    // Minimo 2: com +-1 so existem 2 valores possiveis e precisamos de 3 erradas
    static const int tabela[NIVEL_MAX] = { 10, 8, 5, 3, 2, 2, 2, 2, 2, 2 };

    if (nivel < 1) nivel = 1;
    if (nivel > NIVEL_MAX) nivel = NIVEL_MAX;
    return tabela[nivel - 1];
}

// Monta as 4 alternativas a partir da equacao
void pergunta_de_equacao(Pergunta* p, const Equacao* eq,
    float largura_tela, float y, float altura, int nivel)
{
    if (!p || !eq) return;

    int resposta_certa = eq->resposta_correta;
    int idx_correta = rand() % 4;
    int max_erro = max_erro_por_nivel(nivel);

    int valores[4];
    int usados[4];       // valores ja sorteados (para nao repetir)
    int qtd_usados = 0;

    usados[qtd_usados++] = resposta_certa;
    valores[idx_correta] = resposta_certa;

    for (int i = 0; i < 4; i++) {
        if (i == idx_correta) continue;

        int candidata;
        bool invalida;
        int tentativas = 0;

        do {
            int erro = (rand() % (2 * max_erro + 1)) - max_erro;
            candidata = resposta_certa + erro;

            // invalida se for negativa ou igual a outra alternativa (inclui a certa)
            invalida = (candidata < 0);
            for (int k = 0; k < qtd_usados; k++) {
                if (usados[k] == candidata) invalida = true;
            }

            // Se o intervalo for pequeno demais para achar valores diferentes
            // (ex.: resposta certa = 0), aumenta o intervalo aos poucos.
            tentativas++;
            if (tentativas >= 30) {
                max_erro++;
                tentativas = 0;
            }
        } while (invalida);

        usados[qtd_usados++] = candidata;
        valores[i] = candidata;
    }

    char textos[4][64];
    for (int i = 0; i < 4; i++) {
        snprintf(textos[i], sizeof(textos[i]), "%d", valores[i]);
    }

    const char* ptrs[4] = { textos[0], textos[1], textos[2], textos[3] };
    pergunta_criar(p, eq->texto, ptrs, 4, idx_correta, largura_tela, y, altura);
}

// Testa colisao do tiro com as alternativas
int pergunta_testar_tiro(Pergunta* p, float tx, float ty, float tw, float th)
{
    if (!p || p->respondida) return -1;

    for (int i = 0; i < p->qtd; i++) {

        float margem_x = p->alt[i].w * 0.25f;
        float margem_y = p->alt[i].h * 0.20f;

        float hit_x = p->alt[i].x + margem_x;
        float hit_y = p->alt[i].y + margem_y;
        float hit_w = p->alt[i].w - (margem_x * 2);
        float hit_h = p->alt[i].h - (margem_y * 2);

        // Testa se o tiro bateu no escudo
        if (tx >= hit_x && tx <= hit_x + hit_w &&
            ty >= hit_y && ty <= hit_y + hit_h) {

            p->respondida = true;
            if (p->alt[i].correta) {
                p->alt[i].estado = ALT_CERTA;
                return 1; // Acertou
            }
            else {
                p->alt[i].estado = ALT_ERRADA;
                // Marca a certa de verde tambem, para o jogador ver qual era a correta
                for (int j = 0; j < p->qtd; j++) {
                    if (p->alt[j].correta) p->alt[j].estado = ALT_CERTA;
                }
                return 0; // Errou
            }
        }
    }
    return -1;
}

//Desenha as alternativas com o texto colorido
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

        //Desenha o Escudo
        if (escudo != NULL) {
            float escala_w = w / img_w;
            float escala_h = h / img_h;
            float escala = (escala_w < escala_h) ? escala_w : escala_h;

            float draw_w = img_w * escala;
            float draw_h = img_h * escala;

            float draw_x = cx + (w - draw_w) / 2.0f;
            float draw_y = cy + (h - draw_h) / 2.0f;

            al_draw_scaled_bitmap(escudo,
                0, 0, img_w, img_h,
                draw_x, draw_y, draw_w, draw_h,
                0
            );
        }
        else {
            al_draw_filled_rectangle(cx, cy, cx + w, cy + h, al_map_rgb(45, 45, 150));
        }

        //Define a cor do texto
        ALLEGRO_COLOR cor_texto = al_map_rgb(255, 255, 255);

        if (p->respondida) {
            if (p->alt[i].estado == ALT_CERTA) {
                cor_texto = al_map_rgb(50, 255, 50); // Verde
            }
            else if (p->alt[i].estado == ALT_ERRADA) {
                cor_texto = al_map_rgb(255, 50, 50); // Vermelho
            }
        }

        //Desenha o Texto
        int altura_fonte = al_get_font_line_height(fonte);
        float texto_x = cx + (w / 2.0f);
        float texto_y = cy + (h / 2.0f) - (altura_fonte / 2.0f) + 10.0f;

        al_draw_text(fonte, al_map_rgb(0, 0, 0), texto_x + 2, texto_y + 2, ALLEGRO_ALIGN_CENTER, p->alt[i].texto);
        al_draw_text(fonte, cor_texto, texto_x, texto_y, ALLEGRO_ALIGN_CENTER, p->alt[i].texto);
    }
}