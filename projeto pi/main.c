#include "jogo.h"
#include "jogador.h"
#include "tiro.h"
#include "inimigo.h"
#include "menu.h"
#include "fundo.h"
#include "equacao.h"
#include <time.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include "alternativa.h"

int main(void) {

    // Inicializa a biblioteca principal do Allegro
    if (!al_init()) {
        return 1;
    }

    // Inicializa a semente para geração de números aleatórios (usada nas equações)
    srand((unsigned)time(NULL));

    // Inicializa o add-on de primitivas (permite desenhar retângulos, círculos e linhas)
    if (!al_init_primitives_addon()) {
        return 1;
    }

    // Instala o suporte ao teclado
    if (!al_install_keyboard()) {
        return 1;
    }

    // Instala o suporte ao mouse
    if (!al_install_mouse()) {
        return 1;
    }

    // Inicializa o sistema de fontes do Allegro
    al_init_font_addon();

    // Inicializa o add-on de fontes TrueType (TTF)
    if (!al_init_ttf_addon()) {
        return 1;
    }


    // CRIAÇÃO DA JANELA (DISPLAY)

    ALLEGRO_DISPLAY* tela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!tela) {
        return 1;
    }


    // INICIALIZAÇÃO DO FUNDO

    // Carrega os recursos visuais da arena/fundo
    if (!fundo_inicializar()) {
        al_destroy_display(tela);
        return 1;
    }


    // CONFIGURAÇÃO DO TIMER

    // Cria o timer principal do jogo configurado para rodar a 60 quadros por segundo (FPS)
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

    // Fonte padrão do sistema (fallback)
    ALLEGRO_FONT* fonte = al_create_builtin_font();
    if (!fonte) {
        al_destroy_event_queue(fila_eventos);
        al_destroy_timer(timer);
        fundo_finalizar();
        al_destroy_display(tela);
        return 1;
    }

    // Fonte personalizada (Arial) para exibição de equações e textos do jogo
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

    // Inicializa as entidades principais
    jogador_inicializar(&jogador);
    tiros_inicializar(tiros);
    inimigo_inicializar(&inimigo);

    // Gera a primeira equação e suas respectivas alternativas
    equacao_gerar_nova(&equacao);
    pergunta_de_equacao(&pergunta, &equacao, LARGURA_TELA, 80, 60);

    int espera_proxima = 0; // Contador de frames de espera até gerar a próxima pergunta

    // Variáveis de controle de estado de entrada
    ALLEGRO_KEYBOARD_STATE estado_teclado;
    float mouse_x = LARGURA_TELA / 2;
    float mouse_y = ALTURA_TELA / 2;

    // Flags de controle do fluxo do jogo
    bool no_menu = true;     // true = exibe o menu principal; false = em partida
    bool executando = true;  // controla o loop principal da aplicação


    // INICIALIZAÇÃO DO TIMER PRINCIPAL

    al_start_timer(timer);


    // LOOP PRINCIPAL DO JOGO

    while (executando) {
        ALLEGRO_EVENT evento;

        // Aguarda a ocorrência do próximo evento na fila
        al_wait_for_event(fila_eventos, &evento);


        // TRATAMENTO DE EVENTOS: FECHAR JANELA
        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            executando = false;
        }


        // TRATAMENTO DE EVENTOS: MOVIMENTO DO MOUSE
        if (evento.type == ALLEGRO_EVENT_MOUSE_AXES) {
            mouse_x = evento.mouse.x;
            mouse_y = evento.mouse.y;
        }


        // TRATAMENTO DE EVENTOS: CLIQUE DO MOUSE
        if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            mouse_x = evento.mouse.x;
            mouse_y = evento.mouse.y;

            // Se estiver no menu, verifica clique no botão de iniciar
            if (no_menu) {
                if (evento.mouse.button == 1) {
                    if (menu_clicou_start(mouse_x, mouse_y)) {
                        no_menu = false; // Inicia o jogo
                    }
                }
            }
            // Se estiver jogando, o clique esquerdo dispara um tiro
            else {
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
        }


        // TRATAMENTO DE EVENTOS: TECLADO
        if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            // Tecla ESPAÇO no menu também inicia o jogo
            if (no_menu) {
                if (evento.keyboard.keycode == ALLEGRO_KEY_SPACE) {
                    no_menu = false;
                }
            }
        }


        // ATUALIZAÇÃO DO JOGO (TICK DE 60 FPS)
        if (evento.type == ALLEGRO_EVENT_TIMER) {

            // Se estiver no menu, apenas renderiza o menu e pula a lógica da partida
            if (no_menu) {
                menu_desenhar(fonte);
                continue;
            }

            // Atualiza o estado do jogador capturando as teclas pressionadas
            al_get_keyboard_state(&estado_teclado);
            jogador_atualizar(&jogador, &estado_teclado);

            // Atualiza a posição dos projéteis ativos
            tiros_atualizar(tiros);

            // Atualiza o comportamento/movimento do inimigo
            inimigo_atualizar(&inimigo);


            // COLISÕES: TIROS X INIMIGO
            for (int i = 0; i < MAX_TIROS; i++) {
                if (!tiros[i].ativo) {
                    continue; // Ignora slots de tiros inativos
                }

                // Verifica colisão do tiro atual com o inimigo
                if (inimigo_colidiu_com_tiro(&inimigo, tiros[i].x, tiros[i].y)) {
                    tiros[i].ativo = false;         // O tiro some ao atingir o alvo
                    inimigo_receber_dano(&inimigo); // O inimigo sofre dano
                }
            }


            // COLISÕES: TIROS X ALTERNATIVAS DA EQUAÇÃO
            for (int i = 0; i < MAX_TIROS; i++) {
                if (!tiros[i].ativo) continue;

                int r = pergunta_testar_tiro(&pergunta, tiros[i].x, tiros[i].y, 0, 0);
                if (r != -1) {
                    tiros[i].ativo = false;
                    espera_proxima = 120; // Pausa de 2 segundos (120 frames a 60 FPS) até a próxima pergunta

                    if (r == 1) {
                        // Resposta correta: pontos += 10; 
                    }
                    else {
                        // Resposta incorreta: vidas--; 
                    }
                }
            }


            // GERENCIAMENTO DE TEMPO PARA PRÓXIMA PERGUNTA
            // Após a resposta, aguarda o término da contagem para gerar nova equação
            if (pergunta.respondida && --espera_proxima <= 0) {
                equacao_gerar_nova(&equacao);
                pergunta_de_equacao(&pergunta, &equacao, LARGURA_TELA, 80, 60);
            }


            // RENDERIZAÇÃO GRÁFICA (DESENHO NA TELA)

            // Atualiza o frame do fundo animado
            fundo_atualizar();

            // Desenha o fundo primeiro (camada mais ao fundo)
            fundo_desenhar();

            // Desenha a equação matemática centralizada na parte superior
            equacao_desenhar(&equacao, fonte_equacao, LARGURA_TELA, ALTURA_TELA);

            // Desenha as alternativas de resposta da equação
            pergunta_desenhar(&pergunta, fonte_equacao, LARGURA_TELA);

            // Desenha o personagem jogador
            jogador_desenhar(&jogador);

            // Desenha a arma/mira do jogador apontada para a posição do mouse
            jogador_desenhar_arma(&jogador, mouse_x, mouse_y);

            // Desenha todos os tiros ativos na tela
            tiros_desenhar(tiros);

            // Desenha o inimigo
            inimigo_desenhar(&inimigo);

            // Exibe o buffer completo na tela (Double Buffering)
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