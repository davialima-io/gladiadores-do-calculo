#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include <time.h>
#include <stdlib.h>
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

int main(void)
{
    // INICIALIZACAO DO ALLEGRO
    if (!al_init()) {
        return 1;
    }

    // Semente para numeros aleatorios
    srand((unsigned)time(NULL));

    // Primitivas
    if (!al_init_primitives_addon()) {
        return 1;
    }

    // Imagens
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

    // Fontes TrueType
    if (!al_init_ttf_addon()) {
        return 1;
    }

    // CRIACAO DA JANELA
    ALLEGRO_DISPLAY* tela =
        al_create_display(LARGURA_TELA, ALTURA_TELA);

    if (!tela) {
        return 1;
    }

    // INICIALIZACAO DO FUNDO
    if (!fundo_inicializar()) {
        al_destroy_display(tela);
        return 1;
    }

    // TIMER (60 FPS)
    ALLEGRO_TIMER* timer =
        al_create_timer(1.0 / 60.0);

    if (!timer) {
        fundo_finalizar();
        al_destroy_display(tela);
        return 1;
    }

    // FILA DE EVENTOS
    ALLEGRO_EVENT_QUEUE* fila_eventos =
        al_create_event_queue();

    if (!fila_eventos) {
        al_destroy_timer(timer);
        fundo_finalizar();
        al_destroy_display(tela);
        return 1;
    }

    // REGISTRO DOS EVENTOS
    al_register_event_source(
        fila_eventos,
        al_get_display_event_source(tela)
    );

    al_register_event_source(
        fila_eventos,
        al_get_timer_event_source(timer)
    );

    al_register_event_source(
        fila_eventos,
        al_get_keyboard_event_source()
    );

    al_register_event_source(
        fila_eventos,
        al_get_mouse_event_source()
    );

    // FONTES
    ALLEGRO_FONT* fonte = al_create_builtin_font();

    if (!fonte) {
        al_destroy_event_queue(fila_eventos);
        al_destroy_timer(timer);
        fundo_finalizar();
        al_destroy_display(tela);
        return 1;
    }

    ALLEGRO_FONT* fonte_equacao =
        al_load_ttf_font(
            "C:/Windows/Fonts/arial.ttf",
            20,
            0
        );

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

    // OBJETO DO CRONOMETRO
    Tempo tempo;

    jogador_inicializar(&jogador);
    tiros_inicializar(tiros);

    // Inicializa o inimigo
    inimigo_inicializar(
        &inimigo,
        750,
        ALTURA_TELA - 50
    );

    // Primeira equacao e alternativas
    equacao_gerar_nova(&equacao);

    pergunta_de_equacao(
        &pergunta,
        &equacao,
        LARGURA_TELA,
        80,
        60
    );

    int espera_proxima = 0;

    // MENU
   // ESTADO DE ENTRADA
    ALLEGRO_KEYBOARD_STATE estado_teclado;

    float mouse_x = LARGURA_TELA / 2;
    float mouse_y = ALTURA_TELA / 2;

    bool continuar_programa = true;

    // LOOP EXTERNO: MENU E PARTIDAS
    while (continuar_programa)
    {
        // MOSTRA O MENU
        int opcao = menu_executar(tela);

        if (opcao == MENU_SAIR)
        {
            break;
        }

        // REINICIA OS OBJETOS PARA UMA NOVA PARTIDA
        jogador_inicializar(&jogador);
        tiros_inicializar(tiros);

        inimigo_inicializar(
            &inimigo,
            750,
            ALTURA_TELA - 50
        );

        equacao_gerar_nova(&equacao);

        pergunta_de_equacao(
            &pergunta,
            &equacao,
            LARGURA_TELA,
            80,
            60
        );

        espera_proxima = 0;

        // REINICIA O CRONOMETRO
        tempo_inicializar(&tempo);

        // LIMPA EVENTOS ANTIGOS E INICIA O TIMER
        al_flush_event_queue(fila_eventos);
        al_start_timer(timer);

        bool jogando = true;

        // LOOP DA PARTIDA
        while (jogando)
        {
            ALLEGRO_EVENT evento;

            al_wait_for_event(
                fila_eventos,
                &evento
            );

            // FECHAR A JANELA
            if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            {
                continuar_programa = false;
                jogando = false;
                break;
            }

            // MOVIMENTO DO MOUSE
            if (evento.type == ALLEGRO_EVENT_MOUSE_AXES)
            {
                mouse_x = evento.mouse.x;
                mouse_y = evento.mouse.y;
            }

            // CLIQUE DO MOUSE
            if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
            {
                mouse_x = evento.mouse.x;
                mouse_y = evento.mouse.y;

                if (evento.mouse.button == 1)
                {
                    tiro_disparar(
                        tiros,
                        jogador_centro_x(&jogador),
                        jogador_centro_y(&jogador),
                        mouse_x,
                        mouse_y
                    );
                }
            }

            // ATUALIZACAO DO JOGO
            if (evento.type == ALLEGRO_EVENT_TIMER)
            {
                tempo_atualizar(&tempo);

                // TEMPO ESGOTADO: VOLTA AO MENU
                if (tempo.decorrido >= tempo.duracao)
                {
                    jogando = false;
                    break;
                }

                al_get_keyboard_state(&estado_teclado);

                jogador_atualizar(
                    &jogador,
                    &estado_teclado
                );

                tiros_atualizar(tiros);

                inimigo_atualizar(
                    &inimigo,
                    LARGURA_TELA,
                    ALTURA_TELA
                );

                // COLISOES: TIROS X INIMIGO
                for (int i = 0; i < MAX_TIROS; i++)
                {
                    if (!tiros[i].ativo)
                    {
                        continue;
                    }

                    if (inimigo_colidiu_com_tiro(
                        &inimigo,
                        tiros[i].x,
                        tiros[i].y
                    ))
                    {
                        tiros[i].ativo = false;

                        inimigo_receber_dano(
                            &inimigo,
                            750,
                            ALTURA_TELA
                        );
                    }
                }

                // COLISOES: TIROS X ALTERNATIVAS
                for (int i = 0; i < MAX_TIROS; i++)
                {
                    if (!tiros[i].ativo)
                    {
                        continue;
                    }

                    int r = pergunta_testar_tiro(
                        &pergunta,
                        tiros[i].x,
                        tiros[i].y,
                        0,
                        0
                    );

                    if (r != -1)
                    {
                        tiros[i].ativo = false;
                        espera_proxima = 120;

                        if (r == 1)
                        {
                            // ACERTO: ADICIONA 7 SEGUNDOS
                            tempo_adicionar(&tempo, 7.0);
                        }
                        else
                        {
                            // ERRO: RETIRA 7 SEGUNDOS
                            tempo_adicionar(&tempo, -7.0);
                        }
                    }
                }

                // PROXIMA PERGUNTA
                if (pergunta.respondida && espera_proxima > 0)
                {
                    espera_proxima--;

                    if (espera_proxima == 0)
                    {
                        equacao_gerar_nova(&equacao);

                        pergunta_de_equacao(
                            &pergunta,
                            &equacao,
                            LARGURA_TELA,
                            80,
                            60
                        );
                    }
                }

                // RENDERIZACAO
                fundo_atualizar();
                fundo_desenhar();

                equacao_desenhar(
                    &equacao,
                    fonte_equacao,
                    LARGURA_TELA,
                    ALTURA_TELA
                );

                pergunta_desenhar(
                    &pergunta,
                    fonte_equacao,
                    LARGURA_TELA
                );

                jogador_desenhar(&jogador);

                jogador_desenhar_arma(
                    &jogador,
                    mouse_x,
                    mouse_y
                );

                tiros_desenhar(tiros);
                inimigo_desenhar(&inimigo);

                tempo_desenhar(&tempo, fonte);

                al_flip_display();
            }
        }

        // PARA O TIMER DA PARTIDA
        al_stop_timer(timer);

        // DESCARTA EVENTOS PENDENTES DA PARTIDA
        al_flush_event_queue(fila_eventos);

        // O LOOP EXTERNO MOSTRA O MENU NOVAMENTE
    }

    // FINALIZACAO: SO ACONTECE AO ESCOLHER SAIR OU FECHAR A JANELA
    al_destroy_font(fonte);
    al_destroy_font(fonte_equacao);
    al_destroy_event_queue(fila_eventos);
    al_destroy_timer(timer);
    fundo_finalizar();
    al_destroy_display(tela);

    return 0;
}