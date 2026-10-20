#include "jogo.h"
#include "jogador.h"
#include "tiro.h"
#include "inimigo.h"
#include "menu.h"
#include "fundo.h"
#include "equacao.h"
#include "alternativa.h"

#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h> /* Necessário para o menu_fundo.png */
#include <allegro5/allegro_ttf.h>

int main(void) {

    // Inicializa a biblioteca principal do Allegro
    if (!al_init()) {
        return 1;
    }

    // Inicializa a semente para geração de números aleatórios (usada nas equações)
    srand((unsigned)time(NULL));

    // Inicializa os add-ons
    if (!al_init_primitives_addon()) return 1;
    if (!al_init_image_addon()) return 1; // Necessário para carregar as imagens do menu e do fundo
    if (!al_install_keyboard()) return 1;
    if (!al_install_mouse()) return 1;
    
    al_init_font_addon();
    if (!al_init_ttf_addon()) return 1;

    // CRIAÇÃO DA JANELA (DISPLAY)
    ALLEGRO_DISPLAY* tela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!tela) {
        return 1;
    }

    // INICIALIZAÇÃO DO FUNDO
    if (!fundo_inicializar()) {
        al_destroy_display(tela);
        return 1;
    }

    // CONFIGURAÇÃO DO TIMER
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

    // REGISTRO DAS FONTES DE EVENTOS NA FILA
    al_register_event_source(fila_eventos, al_get_display_event_source(tela));
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_mouse_event_source());

    // CARREGAMENTO DE FONTES
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

    // ESTADOS E OBJETOS DO JOGO
    Jogador jogador;
    Tiro tiros[MAX_TIROS];
    Inimigo inimigo;
    Equacao equacao;
    Pergunta pergunta;

    jogador_inicializar(&jogador);
    tiros_inicializar(tiros);
    inimigo_inicializar(&inimigo);

    equacao_gerar_nova(&equacao);
    pergunta_de_equacao(&pergunta, &equacao, LARGURA_TELA, 80, 60);

    int espera_proxima = 0;

    // ==========================================
    // AQUI ENTRA O NOVO MENU DA BRANCH-MENU
    // ==========================================
    int opcao = menu_executar(tela);

    if (opcao == MENU_SAIR) {
        // Se o jogador escolheu sair, limpa a memória e fecha o jogo
        al_destroy_font(fonte);
        al_destroy_font(fonte_equacao);
        al_destroy_event_queue(fila_eventos);
        al_destroy_timer(timer);
        fundo_finalizar();
        al_destroy_display(tela);
        return 0;
    }

    // Se chegou aqui, o jogador escolheu JOGAR!
    ALLEGRO_KEYBOARD_STATE estado_teclado;
    float mouse_x = LARGURA_TELA / 2;
    float mouse_y = ALTURA_TELA / 2;
    bool executando = true;

    // INICIALIZAÇÃO DO TIMER PRINCIPAL
    al_start_timer(timer);

    // LOOP PRINCIPAL DO JOGO (AQUI FICA A LÓGICA DA MASTER)
    while (executando) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            executando = false;
        }

        if (evento.type == ALLEGRO_EVENT_MOUSE_AXES) {
            mouse_x = evento.mouse.x;
            mouse_y = evento.mouse.y;
        }

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

        if (evento.type == ALLEGRO_EVENT_TIMER) {
            // Atualiza o jogador e entidades
            al_get_keyboard_state(&estado_teclado);
            jogador_atualizar(&jogador, &estado_teclado);
            tiros_atualizar(tiros);
            inimigo_atualizar(&inimigo);

            // COLISÕES: TIROS X INIMIGO
            for (int i = 0; i < MAX_TIROS; i++) {
                if (!tiros[i].ativo) continue;

                if (inimigo_colidiu_com_tiro(&inimigo, tiros[i].x, tiros[i].y)) {
                    tiros[i].ativo = false;
                    inimigo_receber_dano(&inimigo);
                }
            }

            // COLISÕES: TIROS X ALTERNATIVAS DA EQUAÇÃO
            for (int i = 0; i < MAX_TIROS; i++) {
                if (!tiros[i].ativo) continue;

                int r = pergunta_testar_tiro(&pergunta, tiros[i].x, tiros[i].y, 0, 0);
                if (r != -1) {
                    tiros[i].ativo = false;
                    espera_proxima = 120;

                    if (r == 1) {
                        // Resposta correta
                    } else {
                        // Resposta incorreta
                    }
                }
            }

            // GERENCIAMENTO DE TEMPO PARA PRÓXIMA PERGUNTA
            if (pergunta.respondida && --espera_proxima <= 0) {
                equacao_gerar_nova(&equacao);
                pergunta_de_equacao(&pergunta, &equacao, LARGURA_TELA, 80, 60);
            }

            // RENDERIZAÇÃO GRÁFICA
            fundo_atualizar();
            fundo_desenhar();
            
            equacao_desenhar(&equacao, fonte_equacao, LARGURA_TELA, ALTURA_TELA);
            pergunta_desenhar(&pergunta, fonte_equacao, LARGURA_TELA);
            
            jogador_desenhar(&jogador);
            jogador_desenhar_arma(&jogador, mouse_x, mouse_y);
            tiros_desenhar(tiros);
            inimigo_desenhar(&inimigo);

            al_flip_display();
        }
    }

    // FINALIZAÇÃO E LIBERAÇÃO DE RECURSOS
    al_destroy_font(fonte);
    al_destroy_font(fonte_equacao);
    al_destroy_event_queue(fila_eventos);
    al_destroy_timer(timer);
    fundo_finalizar();
    al_destroy_display(tela);

    return 0;
}