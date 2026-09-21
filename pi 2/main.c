#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

#include "jogo.h"
#include "jogador.h"
#include "tiro.h"
#include "inimigo.h"
#include "menu.h"


int main(void)
{
    /*
    
        INICIALIZACAO DO ALLEGRO
      
    */

    if (!al_init())
    {
        return 1;
    }


    // Permite desenhar retângulos, círculos e linhas

    if (!al_init_primitives_addon())
    {
        return 1;
    }


    // Instala teclado
    if (!al_install_keyboard())
    {
        return 1;
    }


    // Instala mouse
    if (!al_install_mouse())
    {
        return 1;
    }


    // Inicializa o sistema de fontes
    al_init_font_addon();


    /*
      
        CRIACAO DA JANELA
  
    */

    ALLEGRO_DISPLAY* tela =
        al_create_display(
            LARGURA_TELA,
            ALTURA_TELA
        );

    if (!tela)
    {
        return 1;
    }


    /*
 
        TIMER
  

        O jogo será atualizado 60 vezes por segundo.
    */

    ALLEGRO_TIMER* timer =
        al_create_timer(1.0 / 60.0);

    if (!timer)
    {
        al_destroy_display(tela);

        return 1;
    }


    /*
       
        FILA DE EVENTOS
      
    */

    ALLEGRO_EVENT_QUEUE* fila_eventos =
        al_create_event_queue();

    if (!fila_eventos)
    {
        al_destroy_timer(timer);
        al_destroy_display(tela);

        return 1;
    }


    /*
       
        REGISTRO DAS FONTES DE EVENTOS
        
    */

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


    /*
       
        FONTE
       
    */

    ALLEGRO_FONT* fonte =
        al_create_builtin_font();

    if (!fonte)
    {
        al_destroy_event_queue(fila_eventos);
        al_destroy_timer(timer);
        al_destroy_display(tela);

        return 1;
    }


    /*
        
        OBJETOS DO JOGO
        
    */

    Jogador jogador;

    Tiro tiros[MAX_TIROS];

    Inimigo inimigo;


    // Inicializa cada objeto
    jogador_inicializar(&jogador);

    tiros_inicializar(tiros);

    inimigo_inicializar(&inimigo);


    /*
      
        ESTADO DO TECLADO
       
    */

    ALLEGRO_KEYBOARD_STATE estado_teclado;


    /*
       
        POSICAO DO MOUSE
        
    */

    float mouse_x =
        LARGURA_TELA / 2;

    float mouse_y =
        ALTURA_TELA / 2;


    /*
        
        ESTADO DO JOGO
        

        true  = menu
        false = partida
    */

    bool no_menu = true;

    bool executando = true;


    /*
        
        INICIA O TIMER
       
    */

    al_start_timer(timer);


    /*
        LOOP PRINCIPAL
       
    */

    while (executando)
    {
        ALLEGRO_EVENT evento;


        // Espera até acontecer algum evento
        al_wait_for_event(
            fila_eventos,
            &evento
        );


        /*
            
            FECHAR JANELA
            
        */

        if (evento.type ==
            ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            executando = false;
        }


        /*
            
            MOVIMENTO DO MOUSE
           
        */

        if (evento.type ==
            ALLEGRO_EVENT_MOUSE_AXES)
        {
            mouse_x = evento.mouse.x;
            mouse_y = evento.mouse.y;
        }


        /*
            
            CLIQUE DO MOUSE
            
        */

        if (evento.type ==
            ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
        {
            mouse_x = evento.mouse.x;
            mouse_y = evento.mouse.y;


            /*
                Se estamos no menu,
                o clique pode iniciar o jogo.
            */

            if (no_menu)
            {
                if (evento.mouse.button == 1)
                {
                    if (menu_clicou_start(
                        mouse_x,
                        mouse_y))
                    {
                        no_menu = false;
                    }
                }
            }


            /*
                Se estamos jogando,
                o clique dispara um tiro.
            */

            else
            {
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
        }


        /*
           
            TECLADO
            
        */

        if (evento.type ==
            ALLEGRO_EVENT_KEY_DOWN)
        {
            /*
                SPACE inicia o jogo
                quando estamos no menu.
            */

            if (no_menu)
            {
                if (evento.keyboard.keycode ==
                    ALLEGRO_KEY_SPACE)
                {
                    no_menu = false;
                }
            }
        }


        /*
           
            ATUALIZACAO DO JOGO
            
        */

        if (evento.type ==
            ALLEGRO_EVENT_TIMER)
        {
            /*
                Se estamos no menu,
                não atualizamos os objetos.
            */

            if (no_menu)
            {
                menu_desenhar(fonte);

                continue;
            }


            /*
               
                JOGADOR
               
            */

            al_get_keyboard_state(
                &estado_teclado
            );

            jogador_atualizar(
                &jogador,
                &estado_teclado
            );


            /*
                
                TIROS
               
            */

            tiros_atualizar(tiros);


            /*
              
                INIMIGO
           
            */

            inimigo_atualizar(
                &inimigo
            );


            /*
              
                COLISAO ENTRE TIROS E INIMIGO
            
            */

            for (int i = 0;
                i < MAX_TIROS;
                i++)
            {
                // Ignora espaços vazios do vetor
                if (!tiros[i].ativo)
                {
                    continue;
                }


                // Verifica se o tiro acertou o inimigo
                if (inimigo_colidiu_com_tiro(
                    &inimigo,
                    tiros[i].x,
                    tiros[i].y))
                {
                    // O tiro desaparece
                    tiros[i].ativo = false;


                    // O inimigo perde vida
                    inimigo_receber_dano(
                        &inimigo
                    );
                }
            }


            /*
               
                DESENHO DA ARENA
             
            */

            al_clear_to_color(
                al_map_rgb(30, 30, 30)
            );


            /*
                
                JOGADOR
               
            */

            jogador_desenhar(
                &jogador
            );


            /*
               
                ARMA
                
            */

            jogador_desenhar_arma(
                &jogador,
                mouse_x,
                mouse_y
            );


            /*
              
                TIROS
                
            */

            tiros_desenhar(
                tiros
            );


            /*
                
                INIMIGO
                
            */

            inimigo_desenhar(
                &inimigo
            );


            /*
                Mostra o frame completo na tela.
            */

            al_flip_display();
        }
    }


    /*
        
        FINALIZACAO
      
    */

    al_destroy_font(fonte);

    al_destroy_event_queue(
        fila_eventos
    );

    al_destroy_timer(timer);

    al_destroy_display(tela);


    return 0;
}