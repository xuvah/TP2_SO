#include "tp2virtual.h"

// ----------------------- Main ----------------------

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