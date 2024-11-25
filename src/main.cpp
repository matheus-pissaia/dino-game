#include <pthread.h>
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_MISSEIS 5
#define MISSEIS_NECESSARIOS 3
#define TEMPO_GERAR_DINO 5
#define MAX_DINOS 5
#define MAX_MISSEIS_ATIVOS 10
#define TEMPO_RECARGA 10

typedef struct
{
    int x, y;
    int ativo;
    int direcao;
    int vida;
} Dino;

typedef struct
{
    int x, y;
    int misseis;
    int ativo;
} Helicoptero;

typedef struct
{
    int x, y;
    int ativo;
} Missil;

typedef struct
{
    int quantidade;
    int tempo_recarga;
    pthread_mutex_t mutex;
} Deposito;

// Variáveis globais
Helicoptero helicoptero;
Dino dinos[MAX_DINOS];
Missil misseis_ativos[MAX_MISSEIS_ATIVOS];
Deposito deposito;
int jogo_ativo = 1;
int dificuldade = 1; // 1 - Fácil, 2 - Médio, 3 - Difícil
int m = MISSEIS_NECESSARIOS;
int n = MAX_MISSEIS;
int t = TEMPO_GERAR_DINO;
int num_dinos = 0;
int tempo_restante = 60; // Contador de tempo

// Mutexes para proteger acesso aos dinossauros e mísseis
pthread_mutex_t mutex_dinos = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_misseis = PTHREAD_MUTEX_INITIALIZER;

void init_game()
{
    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    timeout(50);

    // Inicializa o helicóptero
    helicoptero.x = COLS / 2;
    helicoptero.y = 1;
    helicoptero.misseis = n;
    helicoptero.ativo = 1;

    // Inicializa os dinossauros
    for (int i = 0; i < MAX_DINOS; i++)
    {
        dinos[i].ativo = 0;
    }

    // Inicializa os mísseis
    for (int i = 0; i < MAX_MISSEIS_ATIVOS; i++)
    {
        misseis_ativos[i].ativo = 0;
    }

    // Inicializa o depósito
    deposito.quantidade = MAX_MISSEIS;
    deposito.tempo_recarga = TEMPO_RECARGA;
    pthread_mutex_init(&deposito.mutex, NULL);
}

// Função para desenhar o helicóptero
void desenha_helicoptero()
{
    mvprintw(helicoptero.y, helicoptero.x, "   -----|-----");
    mvprintw(helicoptero.y + 1, helicoptero.x, "*>=====[_]L)");
    mvprintw(helicoptero.y + 2, helicoptero.x, "      -'-`-");
}

// Função para desenhar os dinossauros maiores
void desenha_dinos()
{
    for (int i = 0; i < MAX_DINOS; i++)
    {
        if (dinos[i].ativo)
        {
            int x = dinos[i].x;
            int y = dinos[i].y;

            // Dino
            mvprintw(y, x, "           __");
            mvprintw(y + 1, x, "          / _)");
            mvprintw(y + 2, x, "   .-^^^-/ /");
            mvprintw(y + 3, x, "__/       /");
            mvprintw(y + 4, x, "<__.|_|-|_|");

            // Vida Dino
            mvprintw(y + 5, x, "  Vida: %d", dinos[i].vida);
        }
    }
}

// Função para desenhar os mísseis
void desenha_misseis()
{
    for (int i = 0; i < MAX_MISSEIS_ATIVOS; i++)
    {
        if (misseis_ativos[i].ativo)
        {
            int x = misseis_ativos[i].x;
            int y = misseis_ativos[i].y;

            mvprintw(y, x, "|");
            mvprintw(y + 1, x, "v");
        }
    }
}

// Função para desenhar a casa
void desenha_casa()
{
    int x = 2;
    int y = LINES - 10;

    // Desenha a casa
    mvprintw(y, x, "   /----\\");
    mvprintw(y + 1, x, "  /      \\");
    mvprintw(y + 2, x, " /________\\");
    mvprintw(y + 3, x, " |        |");
    mvprintw(y + 4, x, " |Deposito|");
    mvprintw(y + 5, x, " |________|");

    // Exibe a quantidade de mísseis dentro da casa
    mvprintw(y + 3, x + 3, "   %d", deposito.quantidade);

    // Exibe o tempo restante para recarga acima da casa
    mvprintw(y - 1, x, "Recarga em: %2d s", deposito.tempo_recarga);
}

// Função para atualizar a tela
void atualiza_tela()
{
    clear();
    desenha_helicoptero();
    desenha_dinos();
    desenha_misseis();
    desenha_casa();

    // Informações do jogo
    mvprintw(0, 0, "Mísseis no helicóptero: %d", helicoptero.misseis);
    mvprintw(1, 0, "Dinossauros vivos: %d", num_dinos);
    mvprintw(2, 0, "Tempo restante: %d s", tempo_restante);

    refresh();
}

// Thread do helicóptero
void *thread_helicoptero(void *)
{
    int ch;
    while (jogo_ativo)
    {
        ch = getch();
        // Movimentação
        if (ch == KEY_LEFT && helicoptero.x > 0)
        {
            helicoptero.x--;
        }
        else if (ch == KEY_RIGHT && helicoptero.x < COLS - 15)
        {
            helicoptero.x++;
        }
        else if (ch == KEY_UP && helicoptero.y > 0)
        {
            helicoptero.y--;
        }
        else if (ch == KEY_DOWN && helicoptero.y < LINES - 3)
        {
            helicoptero.y++;
        }
        else if (ch == ' ')
        {
            // Disparo de míssil
            if (helicoptero.misseis > 0)
            {
                pthread_mutex_lock(&mutex_misseis);
                for (int i = 0; i < MAX_MISSEIS_ATIVOS; i++)
                {
                    if (!misseis_ativos[i].ativo)
                    {
                        misseis_ativos[i].ativo = 1;
                        misseis_ativos[i].x = helicoptero.x + 6;
                        misseis_ativos[i].y = helicoptero.y + 3;
                        break;
                    }
                }
                pthread_mutex_unlock(&mutex_misseis);
                helicoptero.misseis--;
            }
        }

        // Verifica se o helicóptero está sobre a casa para recarregar
        if (helicoptero.misseis == 0)
        {
            int casa_x = 2;
            int casa_y = LINES - 10;
            if (helicoptero.x + 13 >= casa_x && helicoptero.x <= casa_x + 10 &&
                helicoptero.y + 2 >= casa_y && helicoptero.y <= casa_y + 5)
            {
                pthread_mutex_lock(&deposito.mutex);
                if (deposito.quantidade > 0)
                {
                    helicoptero.misseis = deposito.quantidade;
                    deposito.quantidade = 0;
                }
                pthread_mutex_unlock(&deposito.mutex);
            }
        }

        // Atualiza a tela
        atualiza_tela();
        usleep(50000);
    }
    pthread_exit(NULL);
}

// Thread para gerar novos dinossauros
void *thread_dino_generator(void *)
{
    while (jogo_ativo)
    {
        sleep(t);
        pthread_mutex_lock(&mutex_dinos);
        for (int i = 0; i < MAX_DINOS; i++)
        {
            if (!dinos[i].ativo)
            {
                dinos[i].ativo = 1;
                dinos[i].x = rand() % (COLS - 20) + 1;
                dinos[i].y = LINES - 20;
                dinos[i].direcao = (rand() % 2) ? 1 : -1; // Direção inicial
                dinos[i].vida = m;                        // Vida inicial do dinossauro
                num_dinos++;
                break;
            }
        }
        if (num_dinos >= MAX_DINOS)
        {
            jogo_ativo = 0;
        }
        pthread_mutex_unlock(&mutex_dinos);
    }
    pthread_exit(NULL);
}

// Thread para movimentar os dinossauros
void *thread_dinos(void *)
{
    while (jogo_ativo)
    {
        pthread_mutex_lock(&mutex_dinos);
        for (int i = 0; i < MAX_DINOS; i++)
        {
            if (dinos[i].ativo)
            {
                // Movimenta o dinossauro
                dinos[i].x += dinos[i].direcao;
                // Verifica colisão com as bordas
                if (dinos[i].x <= 0 || dinos[i].x >= COLS - 15)
                {
                    dinos[i].direcao *= -1; // Inverte a direção
                    dinos[i].x += dinos[i].direcao;
                }
                // Verifica colisão com o helicóptero
                if ((dinos[i].x <= helicoptero.x + 13 && dinos[i].x + 12 >= helicoptero.x) &&
                    (dinos[i].y <= helicoptero.y + 2 && dinos[i].y + 5 >= helicoptero.y))
                {
                    jogo_ativo = 0;
                }
            }
        }
        pthread_mutex_unlock(&mutex_dinos);
        usleep(200000); // Controla a velocidade dos dinossauros
    }
    pthread_exit(NULL);
}

// Thread para movimentar os mísseis
void *thread_misseis(void *)
{
    while (jogo_ativo)
    {
        pthread_mutex_lock(&mutex_misseis);
        pthread_mutex_lock(&mutex_dinos);
        for (int i = 0; i < MAX_MISSEIS_ATIVOS; i++)
        {
            if (misseis_ativos[i].ativo)
            {
                misseis_ativos[i].y += 1; // Move o míssil para baixo
                if (misseis_ativos[i].y >= LINES - 1)
                {
                    // Míssel saiu da tela
                    misseis_ativos[i].ativo = 0;
                }
                else
                {
                    // Verifica colisão com dinossauros
                    for (int j = 0; j < MAX_DINOS; j++)
                    {
                        if (dinos[j].ativo &&
                            misseis_ativos[i].x >= dinos[j].x &&
                            misseis_ativos[i].x <= dinos[j].x + 12 &&
                            misseis_ativos[i].y >= dinos[j].y &&
                            misseis_ativos[i].y <= dinos[j].y + 5)
                        {
                            // Acertou o dinossauro
                            misseis_ativos[i].ativo = 0;
                            dinos[j].vida--;
                            if (dinos[j].vida <= 0)
                            {
                                dinos[j].ativo = 0;
                                num_dinos--;
                            }
                            break;
                        }
                    }
                }
            }
        }
        pthread_mutex_unlock(&mutex_dinos);
        pthread_mutex_unlock(&mutex_misseis);
        usleep(100000); // Controla a velocidade dos mísseis
    }
    pthread_exit(NULL);
}

// Thread para gerenciar a recarga do depósito
void *thread_deposito(void *)
{
    while (jogo_ativo)
    {
        sleep(1);
        pthread_mutex_lock(&deposito.mutex);
        if (deposito.quantidade == 0)
        {
            deposito.tempo_recarga--;
            if (deposito.tempo_recarga <= 0)
            {
                deposito.quantidade = MAX_MISSEIS;
                deposito.tempo_recarga = TEMPO_RECARGA;
            }
        }
        pthread_mutex_unlock(&deposito.mutex);
    }
    pthread_exit(NULL);
}

// Thread para o cronômetro
void *thread_cronometro(void *)
{
    while (jogo_ativo && tempo_restante > 0)
    {
        sleep(1);
        tempo_restante--;
    }
    // Se o tempo acabar, verificar condição de vitória
    if (tempo_restante == 0 && num_dinos < MAX_DINOS)
    {
        jogo_ativo = 0;
    }
    pthread_exit(NULL);
}

// Thread para verificar fim de jogo
void *thread_game_over(void *)
{
    while (jogo_ativo)
    {
        if (!helicoptero.ativo || num_dinos >= MAX_DINOS)
        {
            jogo_ativo = 0;
        }
        usleep(100000);
    }
    pthread_exit(NULL);
}

// Função principal
int main()
{
    srand(time(NULL));
    // Seleciona a dificuldade
    int escolha;
    printf("Selecione o nível de dificuldade:\n");
    printf("1 - Fácil\n");
    printf("2 - Médio\n");
    printf("3 - Difícil\n");
    scanf("%d", &escolha);

    switch (escolha)
    {
    case 1:
        m = 1;
        n = 5;
        break;
    case 2:
        m = 2;
        n = 5;
        break;
    case 3:
        m = 3;
        n = 5;
        break;
    default:
        printf("Opção inválida! Iniciando no nível Fácil.\n");
        m = 1;
        n = 5;
    }

    // Garante que t receba o valor de TEMPO_GERAR_DINO
    t = TEMPO_GERAR_DINO;

    init_game();

    pthread_t th_helicoptero, th_dino_gen, th_dinos, th_misseis, th_deposito, th_cronometro, th_game_over;

    // Cria as threads
    pthread_create(&th_helicoptero, NULL, thread_helicoptero, NULL);
    pthread_create(&th_dino_gen, NULL, thread_dino_generator, NULL);
    pthread_create(&th_dinos, NULL, thread_dinos, NULL);
    pthread_create(&th_misseis, NULL, thread_misseis, NULL);
    pthread_create(&th_deposito, NULL, thread_deposito, NULL);
    pthread_create(&th_cronometro, NULL, thread_cronometro, NULL);
    pthread_create(&th_game_over, NULL, thread_game_over, NULL);

    // Aguarda o fim do jogo
    pthread_join(th_game_over, NULL);

    // Finaliza o ncurses
    endwin();

    // Verifica a condição de vitória
    if (tempo_restante == 0 && num_dinos < MAX_DINOS)
    {
        printf("Parabéns! Você venceu o jogo!\n");
    }
    else
    {
        printf("Game Over!\n");
    }

    return 0;
}
