Jogo de Helicóptero e Dinossauros
Este é um jogo em C que utiliza as bibliotecas ncurses e pthread para criar uma experiência interativa onde o jogador controla um helicóptero que combate dinossauros em um cenário com elementos dinâmicos e gerenciamento de recursos.

![WhatsApp Image 2024-11-24 at 22 35 57](https://github.com/user-attachments/assets/d4c5bdd0-e60c-4be9-bd29-548e1afe687e)


Características do Jogo:

Helicóptero:
  Controlado pelo jogador.
  Movimenta-se no cenário usando as setas do teclado.
  Dispara mísseis para derrotar dinossauros.
  Pode recarregar mísseis ao retornar à base.

Dinossauros:
  Gerados dinamicamente durante o jogo.
  Movem-se no cenário e possuem uma barra de vida.
  Devem ser derrotados antes que atinjam o helicóptero.
  
Base (Depósito):
  Armazena mísseis para recarga do helicóptero.
  Reabastece após um tempo de recarga.

Cronômetro:
  O jogo possui um limite de tempo de 60 segundos.
  O jogador vence se derrotar os dinossauros antes do tempo acabar.
  
Como Jogar
Inicie o jogo:
  Ao executar o programa, selecione o nível de dificuldade:

1: Fácil.
2: Médio.
3: Difícil.

Comandos do jogo:
  Seta para a esquerda (←): Move o helicóptero para a esquerda.
  Seta para a direita (→): Move o helicóptero para a direita.
  Seta para cima (↑): Move o helicóptero para cima.
  Seta para baixo (↓): Move o helicóptero para baixo.
  Barra de espaço (SPACE): Dispara um míssil.
  
Objetivo:
Derrote todos os dinossauros antes que o tempo termine ou antes que o helicóptero seja atingido.

Requisitos:
  Sistema Linux.
  Compilador gcc ou g++.
  Biblioteca ncurses instalada:
    sudo apt-get install libncurses5-dev libncursesw5-dev

Compilação
Clone o repositório ou copie o código-fonte para uma pasta local.
Compile o projeto: Utilize o Makefile para compilar o jogo.

Compilação padrão execute:
  make
  
Execute o jogo: Após a compilação, o jogo estará disponível em bin/run.
Execute-o com o comando:

./bin/run

Estrutura do Código:
  main.c (ou outro nome de entrada): Contém a lógica principal do jogo.
  Makefile: Script para compilação e gerenciamento do projeto.
  
Componentes Principais

Helicóptero:
  Estrutura responsável pelo controle do jogador.
  Armazena informações como posição, número de mísseis e estado ativo.

Dinossauros:
  Gerados dinamicamente em posições aleatórias.
  Possuem vida e direção de movimento.

Mísseis:
  Controlados pelo helicóptero.
  Movem-se no cenário e verificam colisões com os dinossauros.

Depósito:
  Gerencia o armazenamento e recarga de mísseis para o helicóptero.

Threads:
Cada componente do jogo é controlado por uma thread separada:
  Helicóptero.
  Geração de dinossauros.
  Movimento dos dinossauros.
  Movimento dos mísseis.
  Gerenciamento do depósito.
  Cronômetro.
  Verificação de fim de jogo.
