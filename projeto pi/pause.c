#include "pause.h"
#include <allegro5/allegro_primitives.h>

#define TOTAL_OPCOES 3

// Mesmo visual dos botoes do menu principal
#define PAUSE_BOTAO_LARGURA 300
#define PAUSE_BOTAO_ALTURA 60
#define PAUSE_BOTAO_ESPACO_Y 80
#define PAUSE_BORDA_ESPESSURA 3
#define PAUSE_PAINEL_LARGURA 380
#define PAUSE_PAINEL_MARGEM 30
#define PAUSE_TITULO_DISTANCIA 70

#define PAUSE_COR_FUNDO al_map_rgba(0, 0, 0, 180)
#define PAUSE_COR_PAINEL al_map_rgb(40, 20, 15)
#define PAUSE_COR_BOTAO al_map_rgb(80, 50, 30)
#define PAUSE_COR_SELECIONADO al_map_rgb(200, 60, 60)
#define PAUSE_COR_TEXTO al_map_rgb(255, 255, 255)
#define PAUSE_COR_BORDA al_map_rgb(240, 200, 100)

static const char* TEXTOS_PAUSE[TOTAL_OPCOES] = { "CONTINUAR", "RECOMECAR", "VOLTAR AO MENU" };

static float botao_x(int largura_tela) {
    return (largura_tela - PAUSE_BOTAO_LARGURA) / 2.0f;
}

// Y do botao de indice i (o primeiro fica um pouco acima do meio da tela)
static float botao_y(int altura_tela, int i) {
    return altura_tela / 2.0f - 80 + i * PAUSE_BOTAO_ESPACO_Y;
}

static PauseAcao acao_da_opcao(int opcao) {
    if (opcao == 0) return PAUSE_CONTINUAR;
    if (opcao == 1) return PAUSE_RECOMECAR;
    return PAUSE_MENU_PRINCIPAL;
}

static bool mouse_sobre_botao(float x, float y, int largura_tela, int altura_tela, int i) {
    float bx = botao_x(largura_tela);
    float by = botao_y(altura_tela, i);
    return x >= bx && x <= bx + PAUSE_BOTAO_LARGURA && y >= by && y <= by + PAUSE_BOTAO_ALTURA;
}

void pause_inicializar(MenuPause* pause) {
    pause->ativo = false;
    pause->opcao = 0;
}

// Trata o teclado: ESC abre/fecha, W/S = navegar, ENTER confirma
static PauseAcao tratar_teclado(MenuPause* pause, int tecla) {
    if (tecla == ALLEGRO_KEY_ESCAPE) {
        if (pause->ativo) {
            pause->ativo = false;
            return PAUSE_CONTINUAR;
        }
        pause->ativo = true;
        pause->opcao = 0;
        return PAUSE_NENHUMA_ACAO;
    }

    if (!pause->ativo) return PAUSE_NENHUMA_ACAO;

    if (tecla == ALLEGRO_KEY_W || tecla == ALLEGRO_KEY_UP) {
        pause->opcao = (pause->opcao + TOTAL_OPCOES - 1) % TOTAL_OPCOES;
    }
    else if (tecla == ALLEGRO_KEY_S || tecla == ALLEGRO_KEY_DOWN) {
        pause->opcao = (pause->opcao + 1) % TOTAL_OPCOES;
    }
    else if (tecla == ALLEGRO_KEY_ENTER) {
        pause->ativo = false;
        return acao_da_opcao(pause->opcao);
    }
    return PAUSE_NENHUMA_ACAO;
}

// passar por cima seleciona, clique esquerdo confirma
static PauseAcao tratar_mouse(MenuPause* pause, const ALLEGRO_EVENT* evento, int largura_tela, int altura_tela) {
    for (int i = 0; i < TOTAL_OPCOES; i++) {
        if (!mouse_sobre_botao(evento->mouse.x, evento->mouse.y, largura_tela, altura_tela, i)) continue;

        pause->opcao = i;
        if (evento->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && evento->mouse.button == 1) {
            pause->ativo = false;
            return acao_da_opcao(i);
        }
        break;
    }
    return PAUSE_NENHUMA_ACAO;
}

PauseAcao pause_processar_evento(MenuPause* pause, ALLEGRO_EVENT* evento, int largura_tela, int altura_tela) {
    if (evento->type == ALLEGRO_EVENT_KEY_DOWN) {
        return tratar_teclado(pause, evento->keyboard.keycode);
    }

    if (pause->ativo && (evento->type == ALLEGRO_EVENT_MOUSE_AXES || evento->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)) {
        return tratar_mouse(pause, evento, largura_tela, altura_tela);
    }

    return PAUSE_NENHUMA_ACAO;
}

static void desenhar_painel(ALLEGRO_FONT* fonte, int largura_tela, int altura_tela) {
    float titulo_y = botao_y(altura_tela, 0) - PAUSE_TITULO_DISTANCIA;
    float topo = titulo_y - PAUSE_PAINEL_MARGEM;
    float baixo = botao_y(altura_tela, TOTAL_OPCOES - 1) + PAUSE_BOTAO_ALTURA + PAUSE_PAINEL_MARGEM;
    float esquerda = (largura_tela - PAUSE_PAINEL_LARGURA) / 2.0f;
    float direita = esquerda + PAUSE_PAINEL_LARGURA;

    al_draw_filled_rectangle(esquerda, topo, direita, baixo, PAUSE_COR_PAINEL);
    al_draw_rectangle(esquerda, topo, direita, baixo, PAUSE_COR_BORDA, PAUSE_BORDA_ESPESSURA);
    al_draw_text(fonte, PAUSE_COR_BORDA, largura_tela / 2.0f, titulo_y, ALLEGRO_ALIGN_CENTER, "JOGO PAUSADO");
}

static void desenhar_botao(ALLEGRO_FONT* fonte, int largura_tela, int altura_tela, int i, bool selecionado) {
    float x = botao_x(largura_tela);
    float y = botao_y(altura_tela, i);
    ALLEGRO_COLOR cor = selecionado ? PAUSE_COR_SELECIONADO : PAUSE_COR_BOTAO;

    al_draw_filled_rectangle(x, y, x + PAUSE_BOTAO_LARGURA, y + PAUSE_BOTAO_ALTURA, cor);
    al_draw_rectangle(x, y, x + PAUSE_BOTAO_LARGURA, y + PAUSE_BOTAO_ALTURA, PAUSE_COR_BORDA, PAUSE_BORDA_ESPESSURA);

    float texto_y = y + PAUSE_BOTAO_ALTURA / 2.0f - al_get_font_line_height(fonte) / 2.0f;
    al_draw_text(fonte, PAUSE_COR_TEXTO, x + PAUSE_BOTAO_LARGURA / 2.0f, texto_y, ALLEGRO_ALIGN_CENTER, TEXTOS_PAUSE[i]);
}

void pause_desenhar(MenuPause* pause, ALLEGRO_FONT* fonte, int largura_tela, int altura_tela) {
    if (!pause->ativo) return;

    // escurece o jogo sem esconder ele por completo
    al_draw_filled_rectangle(0, 0, largura_tela, altura_tela, PAUSE_COR_FUNDO);

    desenhar_painel(fonte, largura_tela, altura_tela);
    for (int i = 0; i < TOTAL_OPCOES; i++) {
        desenhar_botao(fonte, largura_tela, altura_tela, i, i == pause->opcao);
    }
}