
#include "pause.h"

#include <allegro5/allegro_primitives.h>
#include <math.h>

// Quantidade de opções
#define TOTAL_OPCOES 3

// Inicializa o menu de pause
void pause_inicializar(MenuPause* pause)
{
    pause->ativo = false;
    pause->opcao = 0;
}

// Processa os comandos do teclado e mouse
PauseAcao pause_processar_evento(
    MenuPause* pause,
    ALLEGRO_EVENT* evento,
    int largura_tela,
    int altura_tela
)
{
    // Processamento do teclado
    if (evento->type == ALLEGRO_EVENT_KEY_DOWN)
    {
        int tecla = evento->keyboard.keycode;

        // ESC abre ou fecha o pause
        if (tecla == ALLEGRO_KEY_ESCAPE)
        {
            if (pause->ativo)
            {
                pause->ativo = false;
                return PAUSE_CONTINUAR;
            }
            else
            {
                pause->ativo = true;
                pause->opcao = 0;
            }

            return PAUSE_NENHUMA_ACAO;
        }

        if (!pause->ativo)
            return PAUSE_NENHUMA_ACAO;

        // W ou seta para cima
        if (tecla == ALLEGRO_KEY_W || tecla == ALLEGRO_KEY_UP)
        {
            pause->opcao--;

            if (pause->opcao < 0)
                pause->opcao = TOTAL_OPCOES - 1;
        }

        // S ou seta para baixo
        else if (tecla == ALLEGRO_KEY_S || tecla == ALLEGRO_KEY_DOWN)
        {
            pause->opcao++;

            if (pause->opcao >= TOTAL_OPCOES)
                pause->opcao = 0;
        }

        // ENTER confirma a opção
        else if (tecla == ALLEGRO_KEY_ENTER)
        {
            pause->ativo = false;

            if (pause->opcao == 0)
                return PAUSE_CONTINUAR;

            if (pause->opcao == 1)
                return PAUSE_RECOMECAR;

            if (pause->opcao == 2)
                return PAUSE_MENU_PRINCIPAL;
        }
    }

    // Processamento do mouse
    if (pause->ativo &&
        (evento->type == ALLEGRO_EVENT_MOUSE_AXES ||
            evento->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN))
    {
        float x = evento->mouse.x;
        float y = evento->mouse.y;

        float centro_x = largura_tela / 2.0f;
        float inicio_y = altura_tela / 2.0f - 30;

        for (int i = 0; i < TOTAL_OPCOES; i++)
        {
            float opcao_y = inicio_y + i * 50;

            if (x >= centro_x - 130 &&
                x <= centro_x + 130 &&
                y >= opcao_y - 15 &&
                y <= opcao_y + 35)
            {
                pause->opcao = i;

                // Clique esquerdo seleciona
                if (evento->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN &&
                    evento->mouse.button == 1)
                {
                    pause->ativo = false;

                    if (i == 0)
                        return PAUSE_CONTINUAR;

                    if (i == 1)
                        return PAUSE_RECOMECAR;

                    if (i == 2)
                        return PAUSE_MENU_PRINCIPAL;
                }
            }
        }
    }

    return PAUSE_NENHUMA_ACAO;
}

// Desenha o menu de pause com animações
void pause_desenhar(
    MenuPause* pause,
    ALLEGRO_FONT* fonte,
    int largura_tela,
    int altura_tela
)
{
    if (!pause->ativo)
        return;

    // Tempo para controlar as animações
    double tempo = al_get_time();

    // Fundo escuro transparente
    al_draw_filled_rectangle(
        0, 0,
        largura_tela, altura_tela,
        al_map_rgba(0, 0, 0, 180)
    );

    // ANIMAÇÃO DO TÍTULO
    float pulsacao = (sin(tempo * 3.0) + 1.0f) / 2.0f;

    int brilho = 180 + (int)(75 * pulsacao);

    al_draw_text(
        fonte,
        al_map_rgb(brilho, brilho, 255),
        largura_tela / 2,
        altura_tela / 2 - 120,
        ALLEGRO_ALIGN_CENTER,
        "JOGO PAUSADO"
    );

    // Textos das opções
    const char* opcoes[TOTAL_OPCOES] =
    {
        "CONTINUAR",
        "RECOMECAR",
        "VOLTAR AO MENU"
    };

    // Desenha as opções
    for (int i = 0; i < TOTAL_OPCOES; i++)
    {
        float posicao_x = largura_tela / 2.0f;
        float posicao_y = altura_tela / 2.0f - 30 + i * 50;

        ALLEGRO_COLOR cor;

        if (i == pause->opcao)
        {
            // Movimento lateral da opção selecionada
            float movimento = sin(tempo * 5.0) * 5.0f;

            posicao_x += movimento;

            // Animação de brilho
            float animacao_cor =
                (sin(tempo * 5.0) + 1.0f) / 2.0f;

            int vermelho = 255;
            int verde = 180 + (int)(75 * animacao_cor);
            int azul = 30;

            cor = al_map_rgb(vermelho, verde, azul);

            // Indicador animado
            al_draw_text(
                fonte,
                cor,
                posicao_x - 110,
                posicao_y,
                ALLEGRO_ALIGN_CENTER,
                ">"
            );
        }
        else
        {
            cor = al_map_rgb(255, 255, 255);
        }

        // Desenha o texto
        al_draw_text(
            fonte,
            cor,
            posicao_x,
            posicao_y,
            ALLEGRO_ALIGN_CENTER,
            opcoes[i]
        );
    }
}
