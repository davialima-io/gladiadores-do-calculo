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
    // INICIALIZACAO DO ALLEGRO
    if (!al_init()) {
        return 1;
    }

    // Semente para numeros aleatorios (usada nas equacoes)
    srand((unsigned)time(NULL));

    // Primitivas (retangulos, circulos e linhas)
    if (!al_init_primitives_addon()) {
        return 1;
    }

    // Imagens (necessario para o menu_fundo.png)
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

    // CRIACAO DA JANELA
    ALLEGRO_DISPLAY* tela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!tela) {
        return 1;
    }

    // INICIALIZACAO DO FUNDO
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

    // Inicializa o inimigo na posicao correta do chao
    inimigo_inicializar(&inimigo, 750, ALTURA_TELA - 50);

    // Vidas do jogador
    int vidas = VIDAS_INICIAIS;

    // Primeira equacao e suas alternativas
    equacao_gerar_nova(&equacao);

    pergunta_de_equacao(&pergunta, &equacao, LARGURA_TELA, 80, 60);
    int espera_proxima = 0; // frames de espera ate a proxima pergunta

    // CHAMAR MENU: o menu tem seu proprio loop interno.
    // So retorna quando o jogador escolhe JOGAR ou SAIR.
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

    // O timer so comeca a contar agora, depois do menu
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

        // ATUALIZACAO DO JOGO (60 FPS)
        if (evento.type == ALLEGRO_EVENT_TIMER) {

            al_get_keyboard_state(&estado_teclado);

            jogador_atualizar(&jogador, &estado_teclado);

            tiros_atualizar(tiros);

            // Atualiza o inimigo passando o chao correto
            inimigo_atualizar(&inimigo, LARGURA_TELA, ALTURA_TELA);

            // COLISOES: TIROS X INIMIGO
            for (int i = 0; i < MAX_TIROS; i++) {
                if (!tiros[i].ativo) continue;

                if (inimigo_colidiu_com_tiro(&inimigo, tiros[i].x, tiros[i].y)) {
                    tiros[i].ativo = false;
                    inimigo_receber_dano(&inimigo, 750, ALTURA_TELA);
                }
            }

            // COLISOES: TIROS X ALTERNATIVAS DA EQUACAO
            for (int i = 0; i < MAX_TIROS; i++) {
                if (!tiros[i].ativo) continue;

                int r = pergunta_testar_tiro(&pergunta, tiros[i].x, tiros[i].y, 0, 0);
                if (r != -1) {
                    tiros[i].ativo = false;
                    espera_proxima = 120; // 2 segundos a 60 FPS

                    if (r == 1) {
                        // Resposta correta
                    }
                    else {
                        // Resposta incorreta: perde uma vida
                        vidas--;
                    }
                }
            }

            // GAME OVER: acabaram as vidas, reseta a partida e volta pro menu
            if (vidas <= 0) {
                vidas = VIDAS_INICIAIS;

                jogador_inicializar(&jogador);
                tiros_inicializar(tiros);
                inimigo_inicializar(&inimigo, 750, ALTURA_TELA - 50);

                equacao_gerar_nova(&equacao);
                pergunta_de_equacao(&pergunta, &equacao, LARGURA_TELA, 80, 60);
                espera_proxima = 0;

                // Pausa o timer enquanto o menu estiver aberto
                al_stop_timer(timer);

                opcao = menu_executar(tela);

                if (opcao == MENU_SAIR) {
                    executando = false;
                    continue;
                }

                // Descarta eventos acumulados e retoma o jogo
                al_flush_event_queue(fila_eventos);
                al_start_timer(timer);
                continue;
            }

            // Proxima pergunta depois do tempo de espera
            if (pergunta.respondida && --espera_proxima <= 0) {
                equacao_gerar_nova(&equacao);
                pergunta_de_equacao(&pergunta, &equacao, LARGURA_TELA, 80, 60);
            }

            // RENDERIZACAO
            fundo_atualizar();
            fundo_desenhar();
            equacao_desenhar(&equacao, fonte_equacao, LARGURA_TELA, ALTURA_TELA);
            pergunta_desenhar(&pergunta, fonte_equacao, LARGURA_TELA);

            jogador_desenhar(&jogador);
            jogador_desenhar_arma(&jogador, mouse_x, mouse_y);
            tiros_desenhar(tiros);
            inimigo_desenhar(&inimigo);

            // VIDAS (por ultimo, pra ficar por cima de tudo)
            vidas_desenhar(vidas);

            al_flip_display();
        }
    }

    // FINALIZACAO
    al_destroy_font(fonte);
    al_destroy_font(fonte_equacao);
    al_destroy_event_queue(fila_eventos);
    al_destroy_timer(timer);
    fundo_finalizar();
    al_destroy_display(tela);

    return 0;
}