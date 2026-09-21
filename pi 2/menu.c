#include "menu.h"
#include "jogo.h"


void menu_desenhar(
    ALLEGRO_FONT* fonte
)
{
    /*
        FUNDO DO MENU
    */

    al_clear_to_color(
        al_map_rgb(20, 15, 15)
    );


    /*
        TITULO
    */

    al_draw_text(
        fonte,
        al_map_rgb(220, 180, 70),
        LARGURA_TELA / 2,
        150,
        ALLEGRO_ALIGN_CENTER,
        "GLADIADORES DO CALCULO"
    );


    /*
        SUBTITULO
    */

    al_draw_text(
        fonte,
        al_map_rgb(220, 220, 220),
        LARGURA_TELA / 2,
        220,
        ALLEGRO_ALIGN_CENTER,
        "ENTRE NA ARENA"
    );


    /*
        BOTAO START
    */

    float botao_x1 = 300;
    float botao_y1 = 300;

    float botao_x2 = 500;
    float botao_y2 = 370;


    // Fundo do botão
    al_draw_filled_rectangle(
        botao_x1,
        botao_y1,
        botao_x2,
        botao_y2,
        al_map_rgb(150, 40, 40)
    );


    // Borda do botão
    al_draw_rectangle(
        botao_x1,
        botao_y1,
        botao_x2,
        botao_y2,
        al_map_rgb(230, 180, 70),
        3
    );


    // Texto do botão
    al_draw_text(
        fonte,
        al_map_rgb(255, 255, 255),
        LARGURA_TELA / 2,
        320,
        ALLEGRO_ALIGN_CENTER,
        "START"
    );


    /*
        INSTRUCAO
    */

    al_draw_text(
        fonte,
        al_map_rgb(180, 180, 180),
        LARGURA_TELA / 2,
        430,
        ALLEGRO_ALIGN_CENTER,
        "CLIQUE OU APERTE SPACE"
    );


    // Mostra o menu na tela
    al_flip_display();
}


bool menu_clicou_start(
    float mouse_x,
    float mouse_y
)
{
    /*
        Verifica se o clique aconteceu
        dentro dos limites do botão.
    */

    if (mouse_x >= 300 &&
        mouse_x <= 500 &&
        mouse_y >= 300 &&
        mouse_y <= 370)
    {
        return true;
    }


    return false;
}