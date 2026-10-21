#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include <time.h>

#include "jogo.h"
#include "jogador.h"
#include "tiro.h"
#include "inimigo.h"
#include "menu.h"
#include "fundo.h"
#include "equacao.h"
#include "alternativa.h"

int main(void)
{
    // INICIALIZAÇÃO DO ALLEGRO
    if (!al_init()) {
        return 1;
    }

    // Semente para números aleatórios (usada nas equações)
    srand((unsigned)time(NULL));

    // Primitivas (retângulos, círculos e linhas)
    if (!al_init_primitives_addon()) {
        return 1;
    }

    // Imagens (necessário para o menu_fundo.png)
    if (!al_init_image_addon()) {
        return 1;
    }

    // Teclado
    if (!al_install_keyboard()) {
        return 1;
    }

    // Mouse
    if (!al_install_mouse()) {
        return 1;
    }

    // Fontes
    if (!al_init_font_addon()) {
        return 1;
    }

    // Fontes TrueType (TTF)
    if (!al_init_ttf_addon()) {
        return 1;
    }

    // CRIAÇÃO DA JANELA
    ALLEGRO_DISPLAY* tela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!tela) {
        return 1;
    }

    // INICIALIZAÇÃO DO FUNDO
    if (!fundo_inicializar()) {
        al_destroy_display(tela);
        return 1;
    }

    // TIMER (60 FPS)
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    if (!timer) {
        fundo_finalizar();
        al_destroy_display(tela);
        return 1;
    }

    // FILA DE EVENTOS
    ALLEGRO_EVENT_QUEUE* fila_eventos = al_create_event_queue();
    if (!fila_eventos) {
        al_destroy_timer(timer);
        fundo_finalizar();
        al_destroy_display(tela);
        return 1;
    }

    // REGISTRO DAS FONTES DE EVENTOS
    al_register_event_source(fila_eventos, al_get_display_event_source(tela));
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_mouse_event_source());

    // FONTES
    ALLEGRO_FONT* fonte = al_create_builtin_font();
    if (!fonte) {
        al_destroy_event_queue(fila_eventos);
        al_destroy_timer(timer);
        fundo_finalizar();
        al_destroy_display(tela);
        return 1;
    }

    ALLEGRO_FONT* fonte_equacao = al_load_ttf_font("C:/Windows/Fonts/arial.ttf", 20, 0);
    if (!fonte_equacao) {
        al_destroy_font(fonte);
        al_destroy_event_queue(fila_eventos);
        al_destroy_timer(timer);
        fundo_finalizar();
        al_destroy_display(tela);
        return 1;
    }

    // OBJETOS DO JOGO
    Jogador jogador;
    Tiro tiros[MAX_TIROS];
    Inimigo inimigo;
    Equacao equacao;
    Pergunta pergunta;

    jogador_inicializar(&jogador);
    tiros_inicializar(tiros);
    inimigo_inicializar(&inimigo);

    // Primeira equação e suas alternativas
    equacao_gerar_nova(&equacao);
    pergunta_de_equacao(&pergunta, &equacao, LARGURA_TELA, 80, 60);

    int espera_proxima = 0; // frames de espera até a próxima pergunta

    // CHAMAR MENU
    // O menu tem seu próprio loop interno.
    // Só retorna quando o jogador escolhe JOGAR ou SAIR.
    int opcao = menu_executar(tela);

    if (opcao == MENU_SAIR) {
        al_destroy_font(fonte);
        al_destroy_font(fonte_equacao);
        al_destroy_event_queue(fila_eventos);
        al_destroy_timer(timer);
        fundo_finalizar();
        al_destroy_display(tela);
        return 0;
    }

    // ESTADO DE ENTRADA
    ALLEGRO_KEYBOARD_STATE estado_teclado;
    float mouse_x = LARGURA_TELA / 2;
    float mouse_y = ALTURA_TELA / 2;

    bool executando = true;

    // O timer só começa a contar agora, depois do menu
    al_start_timer(timer);

    // LOOP PRINCIPAL
    while (executando) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        // Fechar janela
        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            executando = false;
        }

        // Movimento do mouse
        if (evento.type == ALLEGRO_EVENT_MOUSE_AXES) {
            mouse_x = evento.mouse.x;
            mouse_y = evento.mouse.y;
        }

        // Clique do mouse (dispara tiro)
        if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            mouse_x = evento.mouse.x;
            mouse_y = evento.mouse.y;

            if (evento.mouse.button == 1) {
                tiro_disparar(
                    tiros,
                    jogador_centro_x(&jogador),
                    jogador_centro_y(&jogador),
                    mouse_x,
                    mouse_y
                );
            }
        }

        // ATUALIZAÇÃO DO JOGO (60 FPS)
        if (evento.type == ALLEGRO_EVENT_TIMER) {

            al_get_keyboard_state(&estado_teclado);
            jogador_atualizar(&jogador, &estado_teclado);

            tiros_atualizar(tiros);
            inimigo_atualizar(&inimigo);

            // COLISÕES: TIROS X INIMIGO
            for (int i = 0; i < MAX_TIROS; i++) {
                if (!tiros[i].ativo) {
                    continue;
                }

                if (inimigo_colidiu_com_tiro(&inimigo, tiros[i].x, tiros[i].y)) {
                    tiros[i].ativo = false;
                    inimigo_receber_dano(&inimigo);
                }
            }

            // COLISÕES: TIROS X ALTERNATIVAS DA EQUAÇÃO
            for (int i = 0; i <