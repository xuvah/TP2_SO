#ifndef TP2VIRTUAL_H
#define TP2VIRTUAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

// --------------- Estrutura da página ---------------

typedef struct no {
    struct no *proximo;
    char operacao;       // Tipo de operação
    int modificado;      // Identificação se foi modificado
    int contagemRef;     // Contagem de referência
    int enderecoFisico;  // Endereço físico
    int ultimoAcesso;    // Marcação de último acesso
    long int pagina;     // Página
} celula;

// ---------------- Variáveis Globais ----------------

#define KByte 1024
#define N 30

char localArquivo[N];    // Inicialização - endereço dos arquivos .log
char arquivo[N];         // Inicialização - arquivo .log escolhido
char subpolitica[N];     // Inicialização - política de substituição escolhida
int tamanhoPagina;       // Inicialização - tamanho da página
int tamanhoMemoria;      // Inicialização - tamanho da memória
int contadorTempo;       // Contagem de tempo para verificar as páginas mais antigas
long int acertos;        // Número de acertos
long int falhas;         // Número de falhas de leitura
long int *memoriaFisica; // Vetor de memória física

// --------------------- Funções ---------------------

// Calcula a página a partir do endereço
int calcularPagina( );

// --------------- Leitura dos arquivos --------------
void lerArquivo( celula *instancia, int deslocamento );

// ------ Conversão dos endereços Hex para Dec -------
long int HexParaDec( char hex[8] );

// ------ Busca por posição vazia na memória --------
int proximaPosicaoVazia( );

// ------------------- Política LRU ------------------
void lru( celula *instancia );

// ------------------- Política NRU ------------------
void nru( celula *instancia, int arrayTempo[] );

// ------------- Política Segunda Chance -------------
void segundaChance( celula *instancia, int arrayTempo[] );

// -------- Insere página na tabela invertida --------
void inserirPagina( celula *instancia, int arrayTempo[], int hash, long int pagina, char rw );

// -------- Remove página na tabela invertida --------
void removerPagina( celula *instancia, celula *minimo, int pos, int subpolitica );

// ----------- Mostra a tabela resumo final ----------
void imprimirResumo( );

#endif