#include "pause.h"

#include <allegro5/allegro_primitives.h>
#include <math.h>

// Quantidade de opcoes do menu
#define TOTAL_OPCOES 3

// Inicializa o menu de pause
void pause_inicializar(MenuPause* pause) {
    pause->ativo = false;
    pause->opcao = 0;
}

// Processa os eventos do teclado e do mouse durante o pause
PauseAcao pause_processar_evento(MenuPause* pause, ALLEGRO_EVENT* evento, int largura_tela, int altura_tela) {

    // Processamento do teclado
    if (evento->type == ALLEGRO_EVENT_KEY_DOWN) {
        int tecla = evento->keyboard.keycode;

        // Abre ou fecha o pause com ESC
        if (tecla == ALLEGRO_KEY_ESCAPE) {
            if (pause->ativo) {
                pause->ativo = false;
                return PAUSE_CONTINUAR;
            }

            pause->ativo = true;
            pause->opcao = 0;
            return PAUSE_NENHUMA_ACAO;
        }

        // Ignora os comandos de navegacao se o jogo nao estiver pausado
        if (!pause->ativo)
            return PAUSE_NENHUMA_ACAO;

        // Navega para a opcao anterior
        if (tecla == ALLEGRO_KEY_W || tecla == ALLEGRO_KEY_UP) {
            pause->opcao--;

            // Volta para a ultima opcao ao ultrapassar o inicio
            if (pause->opcao < 0)
                pause->opcao = TOTAL_OPCOES - 1;
        }

        // Navega para a proxima opcao
        else if (tecla == ALLEGRO_KEY_S || tecla == ALLEGRO_KEY_DOWN) {
            pause->opcao++;

            // Volta para a primeira opcao ao ultrapassar o final
            if (pause->opcao >= TOTAL_OPCOES)
                pause->opcao = 0;
        }

        // Confirma a opcao selecionada com ENTER
        else if (tecla == ALLEGRO_KEY_ENTER) {
            pause->ativo = false;

            if (pause->opcao == 0)
                return PAUSE_CONTINUAR;

            if (pause->opcao == 1)
                return PAUSE_RECOMECAR;

            if (pause->opcao == 2)
                return PAUSE_MENU_PRINCIPAL;
        }
    }

    // Processamento do mouse somente enquanto o pause estiver ativo
    if (pause->ativo && (evento->type == ALLEGRO_EVENT_MOUSE_AXES || evento->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)) {

        float x = evento->mouse.x;
        float y = evento->mouse.y;

        float centro_x = largura_tela / 2.0f;
        float inicio_y = altura_tela / 2.0f - 30;

        // Verifica se o mouse esta sobre alguma opcao
        for (int i = 0; i < TOTAL_OPCOES; i++) {
            float opcao_y = inicio_y + i * 50;

            if (x >= centro_x - 130 && x <= centro_x + 130 &&
                y >= opcao_y - 15 && y <= opcao_y + 35) {

                // Atualiza a opcao destacada
                pause->opcao = i;

                // Executa a opcao somente com o clique esquerdo
                if (evento->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && evento->mouse.button == 1) {
             
                    pause->ativo = false;

                    if (i == 0)
                        return PAUSE_CONTINUAR;

                    if (i == 1)
                        return PAUSE_RECOMECAR;

                    if (i == 2)
                        return PAUSE_MENU_PRINCIPAL;
                }

                break;
            }
        }
    }

    return PAUSE_NENHUMA_ACAO;
}

// Desenha o menu de pause e suas animacoes
void pause_desenhar(MenuPause* pause, ALLEGRO_FONT* fonte, int largura_tela, int altura_tela) {

    // Nao desenha o menu quando o jogo nao estiver pausado
    if (!pause->ativo)
        return;

    // Tempo utilizado para controlar as animacoes
    double tempo = al_get_time();

    // Desenha o fundo escuro sem apagar completamente o jogo
    al_draw_filled_rectangle(0, 0, largura_tela, altura_tela, al_map_rgba(0, 0, 0, 180));
   
    // Calcula a pulsacao do titulo para criar o efeito de brilho
    float pulsacao = (sin(tempo * 3.0) + 1.0f) / 2.0f;
    int brilho = 180 + (int)(75 * pulsacao);

    // Desenha o titulo do menu
    al_draw_text(fonte,al_map_rgb(brilho, brilho, 255), largura_tela / 2, altura_tela / 2 - 120, ALLEGRO_ALIGN_CENTER, "JOGO PAUSADO");
    // Opcoes disponiveis no menu
    const char* opcoes[TOTAL_OPCOES] = {
        "CONTINUAR",
        "RECOMECAR",
        "VOLTAR AO MENU"
    };

    // Desenha e destaca cada opcao
    for (int i = 0; i < TOTAL_OPCOES; i++) {
        float posicao_x = largura_tela / 2.0f;
        float posicao_y = altura_tela / 2.0f - 30 + i * 50;

        ALLEGRO_COLOR cor;

        // Verifica se esta opcao esta selecionada
        if (i == pause->opcao) {

            // Aplica um pequeno movimento lateral
            float movimento = sin(tempo * 5.0) * 5.0f;
            posicao_x += movimento;

            // Calcula a intensidade do brilho da opcao
            float animacao_cor = (sin(tempo * 5.0) + 1.0f) / 2.0f;

            int vermelho = 255;
            int verde = 180 + (int)(75 * animacao_cor);
            int azul = 30;

            cor = al_map_rgb(vermelho, verde, azul);

            // Desenha o indicador da opcao selecionada
            al_draw_text(fonte, cor, posicao_x - 110 , posicao_y, ALLEGRO_ALIGN_CENTER,">");
        }
        else {
            // Mantem as demais opcoes na cor branca
            cor = al_map_rgb(255, 255, 255);
        }

        // Desenha o texto da opcao
        al_draw_text(fonte, cor, posicao_x, posicao_y, ALLEGRO_ALIGN_CENTER, opcoes[i]); {


        }
    }
}