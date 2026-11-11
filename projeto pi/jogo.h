#ifndef JOGO_H
#define JOGO_H

// Tamanho da janela do jogo
#define LARGURA_TELA 800
#define ALTURA_TELA 600

// Y da plataforma onde o jogador e o inimigo pisam
#define CHAO_Y (ALTURA_TELA - 65)

// Vidas do jogador no comeco da partida
#define VIDAS_INICIAIS 5

// Desenha os coracoes de vida
void vidas_desenhar(int vidas_atuais);

// Faz as bordas da tela pulsarem em vermelho quando resta 1 vida. Nao faz nada nos outros casos.
void alerta_vida_desenhar(int vidas_atuais);

#endif