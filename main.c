#define KByte 1024
#define N 30
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

// -------------------------- Variáveis -------------------------

typedef struct no {
    struct no *proximo;
    char operacao;       // Tipo de operação
    int modificado;      // Identificação se foi modificado
    int contagemRef;     // Contagem de referência
    int enderecoFisico;  // Endereço físico
    int ultimoAcesso;    // Marcação de último acesso
    long int pagina;     // Página
} celula;

char localArquivo[N];    // Inicialização - endereço dos arquivos .log
char arquivo[N];         // Inicialização - arquivo .log escolhido
char subpolitica[N];     // Inicialização - política de substituição escolhida
int tamanhoPagina;       // Inicialização - tamanho da página
int tamanhoMemoria;      // Inicialização - tamanho da memória
int contadorTempo;       // Contagem de tempo para verificar as páginas mais antigas
long int acertos;        // Número de acertos
long int falhas;         // Número de falhas de leitura
long int *memoriaFisica; // Vetor de memória física

int acessos = 0;         // Número de acessos
int paginasLidas = 0;    // Número de páginas lidas
int paginasEscritas = 0; // Número de páginas escritas
int i = 0;               // Iterador padrão

// -------------------------- Funções -------------------------

long int HexParaDec(char hex[8]) { // Conversão do endereçamento hexadecimal para decimal
    int valor, tamanho;
    long long decimal = 0;

    tamanho = strlen(hex);
    tamanho--;
    for (i = 0; hex[i] != '\0'; i++) {
        if (hex[i] >= '0' && hex[i] <= '9') {
            valor = hex[i] - 48;
        } else if (hex[i] >= 'a' && hex[i] <= 'f') {
            valor = hex[i] - 97 + 10;
        } else if (hex[i] >= 'A' && hex[i] <= 'F') {
            valor = hex[i] - 65 + 10;
        }
        decimal += valor * pow(16, tamanho);
        tamanho--;
    }
    return decimal;
}

int calcularPagina() { // Cálculo do deslocamento de página pelo endereço
    int pst = tamanhoPagina;
    int i = 0;
    while (pst > 1) {
        pst = pst >> 1;
        i++;
    }
    return i;
}

void lerArquivo(celula *instancia, int deslocamento) { // Leitura do arquivo de entrada, seleciona políticas de substituição, etc.
    acertos = 0;
    falhas = 0;
    contadorTempo = 0;
    paginasLidas = 0;
    paginasEscritas = 0;

    char enderecoHex[8];
    char rw;
    int hash;
    int arrayTempo[tamanhoMemoria];
    long int decimal, pagina;

    FILE *arquivo = fopen(localArquivo, "r");

    while (!feof(arquivo)) {
        fscanf(arquivo, "%s %c", enderecoHex, &rw);
        decimal = HexParaDec(enderecoHex);
        pagina = decimal >> deslocamento;

        // Se memória cheia -> "Escolher" política de substituição
        if ((proximaPosicaoVazia()) == -1) {

            // LRU
            if (strcmp(subpolitica, "lru") == 0) {
                lru(instancia);

            // NRU
            } else if (strcmp(subpolitica, "nru") == 0) {
                nru(instancia, arrayTempo);

            // Segunda Chance
            } else if (strcmp(subpolitica, "segundachance") == 0) {
                segundaChance(instancia, arrayTempo);

            // Senão erro na digitação
            } else {
                printf("Política de Substituição não reconhecida.\n");
                return;
            }
        }

        hash = pagina % tamanhoMemoria;
        if (rw == 'W') {
            paginasEscritas++;
        } else if (rw == 'R') {
            paginasLidas++;
        }
        inserirPagina(instancia, arrayTempo, hash, pagina, rw);
        contadorTempo++;
    }
    fclose(arquivo);
}

int proximaPosicaoVazia() { // Calcula qual a próxima posição vazia para substituição/inserção
    for (int pos = 0; pos < tamanhoMemoria; pos++) {
        if (memoriaFisica[pos] == -1) {
            return pos;
        }
    }
    return -1;
}

void lru(celula *instancia) { // Política de substituição LRU - Least Recently Used

    int menosUsado = __INT_MAX__;
    celula *temp, *menor = instancia;

    // Busca pelo elemento menos utilizado na tabela
    for (i = 0; i < tamanhoMemoria; i++) {
        temp = instancia[i].proximo;
        while (temp != NULL) {
            if (temp->contagemRef < menosUsado) {
                menor = temp;
                menosUsado = temp->contagemRef;
            }
            temp = temp->proximo;
        }
    }
    removerPagina(instancia, menor, 0, 1);
}

void nru(celula *instancia, int arrayTempo[]) { // Política de substituição NRU - Not Recently Used
    int tempoAux;
    int contador = 0;
    int aleatorio;
    celula *tmp = (celula *)malloc(tamanhoMemoria * sizeof(celula));

    if (contadorTempo >= (tamanhoMemoria - 1)) {
        tempoAux = contadorTempo - (tamanhoMemoria - 1);
    } else {
        tempoAux = contadorTempo - 1;
    }

    // Vetor de não utilizados recentemente
    int arrayAux[tamanhoMemoria];
    for (i = 0; i < tamanhoMemoria; i++) {
        if ((arrayTempo[i] / tempoAux) == 0) {
            arrayAux[contador] = i;
            contador++;
        }
    }
    srand(time(NULL));
    aleatorio = rand() % contador;

    // Remove uma posição aleatória
    removerPagina(instancia, tmp, arrayAux[aleatorio], 0);
    memoriaFisica[arrayAux[aleatorio]] = -1;
    arrayTempo[arrayAux[aleatorio]] = -1;
}

void segundaChance(celula *instancia, int arrayTempo[]) { // Política de substituição Segunda Chance - FIFO circular
    int tempoAux;
    int i = 0, auxI = 0;        // Auxiliares no deslocamento do tempo
    long int j = 0, auxJ = 0;   // Auxiliares no deslocamento da memória
    int contador = 0;
    celula *tmp = (celula *)malloc(tamanhoMemoria * sizeof(celula));

    if (contadorTempo >= (tamanhoMemoria - 1)) {
        tempoAux = contadorTempo - (tamanhoMemoria - 1);
    } else {
        tempoAux = contadorTempo - 1;
    }
    while (contador != tamanhoMemoria) {
        if ((arrayTempo[0]) / tempoAux == 1) {
            arrayTempo[0] = 0;
            // Deslocamento vetor tempo
            auxI = arrayTempo[0];
            for (i = 0; i < tamanhoMemoria - 1; i++) {
                arrayTempo[i] = arrayTempo[i + 1];
            }
            arrayTempo[tamanhoMemoria - 1] = auxI;
            // Deslocamento vetor memória
            auxJ = memoriaFisica[0];
            for (j = 0; j < tamanhoMemoria - 1; j++) {
                memoriaFisica[j] = memoriaFisica[j + 1];
            }
            memoriaFisica[tamanhoMemoria - 1] = auxJ;
            contador++;
        } else {
            removerPagina(instancia, tmp, 0, 0);
            arrayTempo[0] = -1;
            memoriaFisica[0] = -1;
            break;
        }
    }

    if (contador == tamanhoMemoria) {
        removerPagina(instancia, tmp, 0, 1);
        arrayTempo[0] = -1;
        memoriaFisica[0] = -1;
    }
}

void inserirPagina(celula *instancia, int arrayTempo[], int hash, long int pagina, char rw) { // Função para inserção de página na tabela invertida

    // Inserção de páginas na tabela

    int vPonto = 0;
    int enderecoFisico = 0;
    celula *anterior;
    celula *temp = (celula *)malloc(sizeof(celula));
    celula *aux = (celula *)malloc(sizeof(celula));

    if (instancia[hash].proximo == NULL) {
        falhas++;
        enderecoFisico = proximaPosicaoVazia();
        memoriaFisica[enderecoFisico] = pagina;
        arrayTempo[enderecoFisico] = contadorTempo;
        temp->enderecoFisico = enderecoFisico;
        temp->pagina = pagina;
        temp->operacao = rw;
        temp->contagemRef = acessos;
        temp->proximo = NULL;
        instancia[hash].proximo = temp;
    }
    else {
        temp = instancia[hash].proximo;
        while (temp != NULL) {
            if (temp->pagina == pagina) {
                vPonto = 1;
                int addr = 0;
                for (int i = 0; i < tamanhoMemoria; i++) {
                    if (memoriaFisica[i] == pagina) {
                        addr = i;
                        break;
                    }
                }
                arrayTempo[addr] = contadorTempo;
                temp->contagemRef = acessos;
                acertos++;
                break;
            }
            else {
                anterior = temp;
                temp = temp->proximo;
            }
        }

        if (vPonto == 0) {
    falhas++;
    enderecoFisico = proximaPosicaoVazia();
    memoriaFisica[enderecoFisico] = pagina;
    arrayTempo[enderecoFisico] = contadorTempo;
    aux->pagina = pagina;
    aux->enderecoFisico = enderecoFisico;
    aux->operacao = rw;
    aux->contagemRef = acessos;
    aux->proximo = NULL;
    anterior->proximo = aux;	
}
    }
acessos++;
}

void removerPagina(celula *instancia, celula *menor, int pos, int subpol) { // Função para remover página na tabela invertida
    // Remoção de páginas na tabela
    celula *paginaAux, *anterior;
    int vPonto, indice;

    // Percorre a tabela em busca da página
    for (i = 0; i < tamanhoMemoria; i++) {
        paginaAux = instancia[i].proximo;
        anterior = instancia[i].proximo;
        while (paginaAux != NULL) {
            // Se NRU ou Segunda Chance -> troca a mais antiga
            if ((paginaAux->pagina == memoriaFisica[pos]) && (subpol == 0)) {
                vPonto = 1;
                indice = i;	
                break;
            } else if ((paginaAux->pagina == menor->pagina) && (subpol == 1)) {
                vPonto = 1;
                indice = i;
                break;
            }
            anterior = paginaAux;
            paginaAux = paginaAux->proximo;
        }

        if (vPonto == 1) {
            break;
        }
    }

    // Se for único elemento ---> remover
    if (anterior->proximo == paginaAux->proximo) {
        if (subpol == 1) {
            memoriaFisica[(instancia[indice].proximo)->enderecoFisico] = -1;
        }
        instancia[indice].proximo = paginaAux->proximo;
    } else {
        if (anterior->pagina == paginaAux->pagina) {
            instancia[indice].proximo = paginaAux->proximo;
        } else {
            anterior->proximo = paginaAux->proximo;
        }
        // Se LRU ---> colocar posição vazia
        if (subpol == 1) {
            memoriaFisica[(instancia[indice].proximo)->enderecoFisico] = -1;
        }
    }
}

void imprimirResumo() { // Mostra os resultados
    printf("Política de Substituição: %s\n", subpolitica);
    printf("Arquivo de Entrada: %s\n", localArquivo);
    printf("Tamanho das Páginas: %d KB\n", tamanhoPagina / KByte);
    printf("Tamanho da Memória: %d KB\n", (tamanhoMemoria * tamanhoPagina) / KByte);
    printf("Páginas lidas: %d\n", paginasLidas);
    printf("Páginas escritas: %d\n", paginasEscritas);
    printf("Páginas de acerto: %ld\n", acertos);
    printf("Páginas de falha: %ld\n", falhas);
}

// ----------------------- Main ----------------------

char localArquivo[N];    // Inicialização - endereço dos arquivos .log
char arquivo[N];         // Inicialização - arquivo .log escolhido
char subpolitica[N];     // Inicialização - política de substituição escolhida
int tamanhoPagina;       // Inicialização - tamanho da página
int tamanhoMemoria;      // Inicialização - tamanho da memória
int contadorTempo;       // Contagem de tempo para verificar as páginas mais antigas
long int acertos;        // Número de acertos
long int falhas;         // Número de falhas de leitura
long int *memoriaFisica; // Vetor de memória física

int main(int argc, char *argv[]){
	
    // -------------------- Entradas --------------------

    strcpy(subpolitica, argv[1]);
    strcpy(arquivo, argv[2]);
    strcpy(localArquivo, "arquivos/");
    strcat(localArquivo, arquivo);
    tamanhoPagina = atoi(argv[3]);
    tamanhoMemoria = atoi(argv[4]);

    // ------------------ Conversão ---------------------
	
    tamanhoPagina = tamanhoPagina * KByte;
    tamanhoMemoria = (tamanhoMemoria * KByte) / tamanhoPagina;

    // ------------- Inicialização padrão ---------------

    celula *instancia = (celula *)malloc(tamanhoMemoria * sizeof(celula));
    for (int i = 0; i < tamanhoMemoria; i++) {
        instancia[i].proximo = NULL;
        instancia[i].operacao = ' ';
        instancia[i].contagemRef = 0;
        instancia[i].ultimoAcesso = -1;
        instancia[i].pagina = -1;
    }
	
    // -------------- Chamadas de funções ---------------

    int deslocamento = calcularPagina();
    memoriaFisica = calloc(tamanhoMemoria, sizeof(long int));
    for (int i = 0; i < tamanhoMemoria; i++) {
        memoriaFisica[i] = -1;
    }

    printf("Calculando...\n");
    printf("2 + 2 = ...\n");
    lerArquivo(instancia, deslocamento);
    imprimirResumo();
    return 0;
}
