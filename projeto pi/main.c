#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "jogo.h"
#include "jogador.h"
#include "tiro.h"
#include "inimigo.h"
#include "menu.h"
#include "fundo.h"
#include "equacao.h"
#include "tempo.h"
#include "alternativa.h"
#include "tela.h"

#define FPS 60
#define ALTERNATIVAS_Y 60
#define ALTERNATIVAS_ALTURA 140   // escudo fica quadrado com 140
#define FRAMES_ENTRE_PERGUNTAS 120
#define FRAMES_AVISO_NIVEL 120
#define PENALIDADE_ERRO 7.0
#define BONUS_TEMPO_INIMIGO 5.0
#define INTERVALO_DANO_INIMIGO 1.0

// Tudo que existe so durante uma partida e volta ao zero quando ela recomeca
typedef struct {
    Jogador jogador;
    Tiro tiros[MAX_TIROS];
    Inimigo inimigo;
    Equacao equacao;
    Pergunta pergunta;
    Tempo tempo;
    int vidas;
    double ultimo_dano_inimigo;
    int acertos;
    int nivel;
    int aviso_nivel;      // frames que faltam pro aviso de nivel sumir
    int espera_proxima;   // frames ate a proxima pergunta aparecer
} Partida;

// Coisas do Allegro que vivem o programa inteiro
typedef struct {
    ALLEGRO_DISPLAY* tela;
    ALLEGRO_TIMER* timer;
    ALLEGRO_EVENT_QUEUE* fila;
    ALLEGRO_FONT* fonte;
    ALLEGRO_FONT* fonte_equacao;
    ALLEGRO_BITMAP* escudo;
    bool fundo_ok;
} Recursos;

// Segundos ganhos por acerto: nivel 1 = 7s, nivel 2 = 6s, do 3 em diante = 5s
static double bonus_tempo_por_nivel(int nivel) {
    double bonus = 8.0 - nivel;
    if (bonus < 5.0) bonus = 5.0;
    return bonus;
}

static void finalizar_recursos(Recursos* r) {
    al_destroy_bitmap(r->escudo);
    al_destroy_font(r->fonte);
    al_destroy_font(r->fonte_equacao);
    al_destroy_event_queue(r->fila);
    al_destroy_timer(r->timer);
    if (r->fundo_ok) fundo_finalizar();
    al_destroy_display(r->tela);
}

static bool iniciar_recursos(Recursos* r) {
    memset(r, 0, sizeof(*r));

    if (!al_init() || !al_init_primitives_addon() || !al_init_image_addon()) return false;
    if (!al_install_keyboard() || !al_install_mouse()) return false;
    if (!al_init_font_addon() || !al_init_ttf_addon()) return false;

    srand((unsigned)time(NULL));

    r->tela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    r->fundo_ok = r->tela && fundo_inicializar();
    r->timer = al_create_timer(1.0 / FPS);
    r->fila = al_create_event_queue();
    r->fonte = al_create_builtin_font();
    r->fonte_equacao = al_load_ttf_font("C:/Windows/Fonts/arial.ttf", 20, 0);
    r->escudo = al_load_bitmap("escudo_alternativa.png");

    // sem escudo o jogo roda mesmo assim, so desenha retangulos
    if (!r->escudo) printf("Aviso: escudo_alternativa.png nao encontrado!\n");

    if (!r->fundo_ok || !r->timer || !r->fila || !r->fonte || !r->fonte_equacao) {
        finalizar_recursos(r);
        return false;
    }

    al_register_event_source(r->fila, al_get_display_event_source(r->tela));
    al_register_event_source(r->fila, al_get_timer_event_source(r->timer));
    al_register_event_source(r->fila, al_get_keyboard_event_source());
    al_register_event_source(r->fila, al_get_mouse_event_source());
    return true;
}

static void nova_pergunta(Partida* p) {
    equacao_gerar_nova(&p->equacao, p->nivel);
    pergunta_de_equacao(&p->pergunta, &p->equacao, LARGURA_TELA, ALTERNATIVAS_Y, ALTERNATIVAS_ALTURA, p->nivel);
}

static void iniciar_partida(Partida* p) {
    jogador_inicializar(&p->jogador);
    tiros_inicializar(p->tiros);
    inimigo_inicializar(&p->inimigo);
    tempo_inicializar(&p->tempo);

    p->vidas = VIDAS_INICIAIS;
    p->ultimo_dano_inimigo = al_get_time() - INTERVALO_DANO_INIMIGO;
    p->acertos = 0;
    p->nivel = 1;
    p->aviso_nivel = 0;
    p->espera_proxima = 0;
    nova_pergunta(p);
}

// Inimigo encostando no jogador tira uma vida, no maximo uma por segundo
static void tratar_dano_do_inimigo(Partida* p) {
    double agora = al_get_time();
    if (agora - p->ultimo_dano_inimigo < INTERVALO_DANO_INIMIGO) return;
    if (!jogador_colidiu_com_inimigo(&p->jogador, &p->inimigo)) return;
    p->vidas--;
    p->ultimo_dano_inimigo = agora;
}

static void recompensar_morte_do_inimigo(Partida* p) {
    tempo_adicionar(&p->tempo, BONUS_TEMPO_INIMIGO);
    // so existem coracoes desenhados ate VIDAS_INICIAIS, entao nao passa disso
    if (p->vidas < VIDAS_INICIAIS) p->vidas++;
}

static void tratar_tiros_no_inimigo(Partida* p) {
    for (int i = 0; i < MAX_TIROS; i++) {
        if (!p->tiros[i].ativo) continue;
        if (inimigo_colidiu_com_tiro(&p->inimigo, p->tiros[i].x, p->tiros[i].y)) {
            p->tiros[i].ativo = false;
            if (inimigo_receber_dano(&p->inimigo)) recompensar_morte_do_inimigo(p);
        }
    }
}

static void registrar_acerto(Partida* p) {
    tempo_adicionar(&p->tempo, bonus_tempo_por_nivel(p->nivel));
    p->acertos++;
    int novo_nivel = equacao_nivel_para_acertos(p->acertos);
    if (novo_nivel > p->nivel) {
        p->nivel = novo_nivel;
        p->aviso_nivel = FRAMES_AVISO_NIVEL;
    }
}

static void tratar_tiros_nas_alternativas(Partida* p) {
    for (int i = 0; i < MAX_TIROS; i++) {
        if (!p->tiros[i].ativo) continue;
        int resultado = pergunta_testar_tiro(&p->pergunta, p->tiros[i].x, p->tiros[i].y);
        if (resultado == -1) continue;

        p->tiros[i].ativo = false;
        p->espera_proxima = FRAMES_ENTRE_PERGUNTAS;
        if (resultado == 1) {
            registrar_acerto(p);
        }
        else {
            tempo_adicionar(&p->tempo, -PENALIDADE_ERRO);
            p->vidas--;
        }
    }
}

// So passa pra proxima pergunta depois da contagem, pro jogador ver qual era a certa
static void avancar_pergunta(Partida* p) {
    if (!p->pergunta.respondida || p->espera_proxima <= 0) return;
    p->espera_proxima--;
    if (p->espera_proxima == 0) nova_pergunta(p);
}

// Retorna false quando a partida acabou (tempo ou vidas)
static bool atualizar_partida(Partida* p) {
    ALLEGRO_KEYBOARD_STATE teclado;

    tempo_atualizar(&p->tempo);
    if (p->tempo.decorrido >= p->tempo.duracao) return false;

    al_get_keyboard_state(&teclado);
    jogador_atualizar(&p->jogador, &teclado);
    tiros_atualizar(p->tiros);
    inimigo_atualizar(&p->inimigo);

    tratar_dano_do_inimigo(p);
    tratar_tiros_no_inimigo(p);
    tratar_tiros_nas_alternativas(p);
    if (p->vidas <= 0) return false;

    avancar_pergunta(p);
    if (p->aviso_nivel > 0) p->aviso_nivel--;
    return true;
}

static void desenhar_aviso_nivel(const Partida* p, ALLEGRO_FONT* fonte) {
    // a sombra preta deslocada deixa o texto legivel em cima de qualquer fundo
    al_draw_textf(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA / 2 + 2, 302, ALLEGRO_ALIGN_CENTER, "NIVEL %d!", p->nivel);
    al_draw_textf(fonte, al_map_rgb(255, 215, 0), LARGURA_TELA / 2, 300, ALLEGRO_ALIGN_CENTER, "NIVEL %d!", p->nivel);
}

static void desenhar_partida(Partida* p, Recursos* r, float mouse_x, float mouse_y) {
    tela_preparar_frame(r->tela);
    fundo_atualizar();
    fundo_desenhar();

    equacao_desenhar(&p->equacao, r->fonte_equacao, LARGURA_TELA, ALTURA_TELA);
    pergunta_desenhar(&p->pergunta, r->fonte_equacao, r->escudo);
    jogador_desenhar(&p->jogador);
    jogador_desenhar_arma(&p->jogador, mouse_x, mouse_y);
    tiros_desenhar(p->tiros);
    inimigo_desenhar(&p->inimigo);

    tempo_desenhar(&p->tempo, r->fonte);
    al_draw_textf(r->fonte, al_map_rgb(255, 255, 0), LARGURA_TELA - 10, 24, ALLEGRO_ALIGN_RIGHT, "Nivel: %d", p->nivel);
    if (p->aviso_nivel > 0) desenhar_aviso_nivel(p, r->fonte_equacao);

    // vidas por ultimo pra ficar por cima de tudo
    vidas_desenhar(p->vidas);
    al_flip_display();
}

static void executar_partida(Recursos* r, Partida* p, bool* continuar_programa) {
    float mouse_x = LARGURA_TELA / 2;
    float mouse_y = ALTURA_TELA / 2;

    al_flush_event_queue(r->fila);
    al_start_timer(r->timer);

    while (true) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(r->fila, &evento);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            *continuar_programa = false;
            break;
        }

        if (evento.type == ALLEGRO_EVENT_MOUSE_AXES || evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            mouse_x = evento.mouse.x;
            mouse_y = evento.mouse.y;
            tela_mouse_para_jogo(r->tela, &mouse_x, &mouse_y);
        }

        if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && evento.mouse.button == 1) {
            tiro_disparar(p->tiros, jogador_centro_x(&p->jogador), jogador_centro_y(&p->jogador), mouse_x, mouse_y);
        }

        if (evento.type == ALLEGRO_EVENT_KEY_DOWN && evento.keyboard.keycode == ALLEGRO_KEY_F11) {
            tela_alternar_tela_cheia(r->tela);
        }

        if (evento.type == ALLEGRO_EVENT_TIMER) {
            if (!atualizar_partida(p)) break;
            desenhar_partida(p, r, mouse_x, mouse_y);
        }
    }

    al_stop_timer(r->timer);
    al_flush_event_queue(r->fila);
}

int main(void) {
    Recursos recursos;
    Partida partida;
    bool continuar_programa = true;

    if (!iniciar_recursos(&recursos)) return 1;

    while (continuar_programa) {
        if (menu_executar(recursos.tela) == MENU_SAIR) break;
        iniciar_partida(&partida);
        executar_partida(&recursos, &partida, &continuar_programa);
    }

    finalizar_recursos(&recursos);
    return 0;
}