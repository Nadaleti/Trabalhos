/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 03A - Hashing com reespalhamento linear
 *
 * RA: 726571
 * Aluno: Matheus Nadaleti Garcia
 * ========================================================================== */

 /* Bibliotecas */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 11
#define TAM_NOME 51
#define TAM_DESENVOLVEDORA 51
#define TAM_DATA 11
#define TAM_CLASSIFICACAO 3
#define TAM_PRECO 8
#define TAM_DESCONTO 4
#define TAM_CATEGORIA 51

#define TAM_REGISTRO 192
#define MAX_REGISTROS 1000
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)


#define POS_OCUPADA 	"[%d] Ocupado: %s\n"
#define POS_LIVRE 		"[%d] Livre\n"
#define POS_REMOVIDA 	"[%d] Removido\n"

/* Estado das posições da tabela hash */
#define LIVRE       0
#define OCUPADO     1
#define REMOVIDO    2

/* Saídas do usuário */
#define OPCAO_INVALIDA "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE "Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO "Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA "ERRO: Ja existe um registro com a chave primaria: %s.\n\n"
#define ARQUIVO_VAZIO "Arquivo vazio!"
#define INICIO_BUSCA 							 "********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM						  "********************************LISTAR********************************\n"
#define INICIO_ALTERACAO 						"********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO					    "********************************ARQUIVO*******************************\n"
#define INICIO_EXCLUSAO 			 			"**********************EXCLUIR*********************\n"

#define SUCESSO  				 "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 "FALHA AO REALIZAR OPERACAO!\n"
#define ERRO_TABELA_CHEIA 		"ERRO: Tabela Hash esta cheia!\n\n"
#define REGISTRO_INSERIDO 		"Registro %s inserido com sucesso. Numero de colisoes: %d.\n\n"


/* Registro do jogo */
typedef struct {
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
	char desenvolvedora[TAM_DESENVOLVEDORA];
	char data[TAM_DATA];	/* DD/MM/AAAA */
	char classificacao[TAM_CLASSIFICACAO];
	char preco[TAM_PRECO];
	char desconto[TAM_DESCONTO];
	char categoria[TAM_CATEGORIA];
} Jogo;

/* Registro da Tabela Hash
 * Contém o estado da posição, a chave primária e o RRN do respectivo registro */
typedef struct {
	int estado;
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Chave;

/* Estrutura da Tabela Hash */
typedef struct {
    int tam;
    Chave *v;
  } Hashtable;

  /* Variáveis globais */
char ARQUIVO[TAM_ARQUIVO];
int nregistros;




/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */


/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo();

/* Exibe o jogo */
int exibir_registro(int rrn);

/*Função de Hash*/
short hash(const char* chave, int tam);


/*Auxiliar para a função de hash*/
short f(char x);

/*Retorna o primeiro número primo >= a*/
int  prox_primo(int a);

/*Funções do Menu*/
void carregar_tabela(Hashtable* tabela);
void cadastrar(Hashtable* tabela);
int  alterar(Hashtable tabela);
void buscar(Hashtable tabela);
int  remover(Hashtable* tabela);
void liberar_tabela(Hashtable* tabela);


/* PROTOTIPOS */
//Insere um registro no arquivo de dados
void inserir_Arq (Jogo titulo);

//Lê a entrada do usuário e salva em um registro de jogo
Jogo ler_entrada ();

//Recupera um registro do arquivo de dados
Jogo recuperar_registro(int rrn);

//Insere uma nova pk na tabela hash
int inserir_Tab(Hashtable *tabela, char *pk, int rrn);

void criar_tabela(Hashtable *tabela, int tam);
void carregar_tabela(Hashtable* tabela);

//Impressão da tabela
void imprimir_tabela(Hashtable tabela);


/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main() 
{    
	/* Arquivo */
	int carregarArquivo = 0;
	scanf("%d\n", &carregarArquivo); // 1 (sim) | 0 (nao)
	if (carregarArquivo) 
		carregar_arquivo();

	/* Tabela Hash */
	int tam;
	scanf("%d", &tam);
	tam = prox_primo(tam);

	Hashtable tabela;
	criar_tabela(&tabela, tam);
	if (carregarArquivo) 
		carregar_tabela(&tabela);

	/* Execução do programa */
	int opcao = 0;
	while(opcao != 6) {
			scanf("%d%*c", &opcao);
			switch(opcao) 
			{
				case 1:
					cadastrar(&tabela);
					break;
				case 2:
					printf(INICIO_ALTERACAO);
					if(alterar(tabela))
						printf(SUCESSO);
					else
						printf(FALHA);
					break;
				case 3:
					printf(INICIO_BUSCA);
					buscar(tabela);
					break;
				case 4:
					printf(INICIO_EXCLUSAO);
					if(	remover(&tabela))
						printf(SUCESSO);
					else
						printf(FALHA);
					break;
				case 5:
					printf(INICIO_LISTAGEM);
					imprimir_tabela(tabela);
					break;
				case 6:
					liberar_tabela(&tabela);
					break;
				case 10:
					printf(INICIO_ARQUIVO);
					printf("%s\n", (*ARQUIVO!='\0') ? ARQUIVO : ARQUIVO_VAZIO);
					break;
				default:
					printf(OPCAO_INVALIDA);
					break;
			}
	}
	return 0;
}

/* <<< IMPLEMENTE AQUI AS FUNCOES >>> */

/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo() {
	scanf("%[^\n]\n", ARQUIVO);
}

/*Auxiliar para a função de hash*/
short f(char x)
{
	return (x < 59) ? x - 48 : x - 54; 
}

/* Exibe o jogo */
int exibir_registro(int rrn)
{
	if(rrn<0)
		return 0;
	float preco;
	int desconto;
	Jogo j = recuperar_registro(rrn);
  char *cat, categorias[TAM_CATEGORIA];
	printf("%s\n", j.pk);
	printf("%s\n", j.nome);
	printf("%s\n", j.desenvolvedora);
	printf("%s\n", j.data);
	printf("%s\n", j.classificacao);
	sscanf(j.desconto,"%d",&desconto);
	sscanf(j.preco,"%f",&preco);
	preco = preco *  (100-desconto);
	preco = ((int) preco)/ (float) 100 ;
	printf("%07.2f\n",  preco);
	strncpy(categorias, j.categoria, strlen(j.categoria) + 1);
  for (cat = strtok (categorias, "|"); cat != NULL; cat = strtok (NULL, "|"))
    printf("%s ", cat);
	printf("\n");
	return 1;
}

/* ======================================================== */

//Função de Hash
short hash(const char* chave, int tam) {
	int i, sum = 0;

	for (i = 0; i < 8; i++)
		sum += (i+1) * f(chave[i]);

	return (sum % tam);
}

//Dado um inteiro a, busca o primo mais próximo
int prox_primo(int a) {
	if (a % 2 == 0 && a != 2) a++;

	int i, max_div;
	while (1) {
		i = 3;

		while (i < a && (a % i != 0))
			i += 2;

		if (i >= a) return a;
		else a += 2;
	}
}

//Aloca um vetor de chaves com tamanho 'tam'
void criar_tabela(Hashtable *tabela, int tam) {
	tabela->tam = tam;
	tabela->v = (Chave *) malloc (sizeof(Chave) * tam);
}

//Dado um arquivo, insere os registros do arquivo na tabela hash
void carregar_tabela(Hashtable* tabela) {
	int i;
	Jogo j;

	nregistros = strlen(ARQUIVO)/TAM_REGISTRO;

	for (i = 0; i < nregistros; i++) {
		j = recuperar_registro(i);
		inserir_Tab(tabela, j.pk, i);
	}
}

//Dado um rrn, recupera um registro do arquivo de dados
Jogo recuperar_registro(int rrn) {
	Jogo j;

	char copy[193];
	char *arq = ARQUIVO;

	//Realiza a cópia do registro encontrado no arquivo de dados
	strncpy(copy, &arq[rrn * 192], 192); copy[192] = '\0';

	strcpy (j.pk, strtok(copy, "@"));
	strcpy (j.nome, strtok(NULL, "@"));
	strcpy (j.desenvolvedora, strtok(NULL, "@"));
	strcpy (j.data, strtok(NULL, "@"));
	strcpy (j.classificacao, strtok(NULL, "@"));
	strcpy (j.preco, strtok(NULL, "@"));
	strcpy (j.desconto, strtok(NULL, "@"));
	strcpy (j.categoria, strtok(NULL, "@"));

	return j;
}

/* CADASTRAR */
void cadastrar(Hashtable* tabela) {
	Jogo j = ler_entrada();	

	int num = inserir_Tab(tabela, j.pk, nregistros);
	if (num != -1) {
		printf(REGISTRO_INSERIDO, j.pk, num);

		inserir_Arq(j);
		nregistros++;
	}
}

//Lê a entrada do usuário e salva em um registro de jogo
Jogo ler_entrada () {
	Jogo titulo;

	//Recebe todos os dados do jogo
	scanf ("%[^\n]", titulo.nome); getchar();
	titulo.nome[strlen(titulo.nome)] = '\0';
	scanf ("%[^\n]", titulo.desenvolvedora); getchar();
	titulo.desenvolvedora[strlen(titulo.desenvolvedora)] = '\0';
	scanf ("%[^\n]", titulo.data); getchar();
	titulo.data[TAM_DATA-1] = '\0';
	scanf ("%[^\n]", titulo.classificacao); getchar();
	titulo.preco[TAM_CLASSIFICACAO-1] = '\0';
	scanf ("%[^\n]", titulo.preco); getchar();
	titulo.preco[TAM_PRECO-1] = '\0';
	scanf ("%[^\n]", titulo.desconto); getchar();
	titulo.desconto[TAM_DESCONTO-1] = '\0';
	scanf ("%[^\n]", titulo.categoria); getchar();
	titulo.categoria[strlen(titulo.categoria)] = '\0';

	//Cria o índice primário
	sprintf (titulo.pk, "%c%c%c%c%c%c%c%c%s", titulo.nome[0], titulo.nome[1],
	titulo.desenvolvedora[0], titulo.desenvolvedora[1], titulo.data[0],
	titulo.data[1], titulo.data[3], titulo.data[4], titulo.classificacao);

	return titulo;
}

//Insere um registro no arquivo de dados
void inserir_Arq (Jogo titulo) {
	char *archive = ARQUIVO;

	//Encontra a quantidade de caracteres '#' devem ser inseridas
	int qntd_sharps = 192 - (40 + strlen(titulo.nome) + strlen(titulo.desenvolvedora) + strlen(titulo.categoria));

	//Escreve o registro no arquivo de dados no primeiro espaço em branco
	sprintf((archive + (192 * nregistros)), "%s@%s@%s@%s@%s@%s@%s@%s@", titulo.pk, titulo.nome, titulo.desenvolvedora,
	titulo.data, titulo.classificacao, titulo.preco, titulo.desconto, titulo.categoria);

	//Coloca os "#" no arquivo
	int i;
	int position = (192 * (nregistros+1)) - qntd_sharps;
	for (i = 0; i < qntd_sharps; i++)
		*(archive + (position + i)) = '#';
}

//Insere uma nova pk na tabela hash
int inserir_Tab(Hashtable *tabela, char *pk, int rrn) {
	int i = hash(pk, tabela->tam), cont = 0;

	//Procura pela primeira posição livre (ou marcada como 'removido')
	while (tabela->v[i % tabela->tam].estado == 1 && cont != tabela->tam) {

		//Verifica se a chave já foi inserida na tabela
		if (!strcmp(tabela->v[i % tabela->tam].pk, pk)) {
			printf(ERRO_PK_REPETIDA, pk);
			return -1;
		}

		i++;
		cont++;
	}

	//Insere na tabela
	if (cont < tabela->tam) {
		tabela->v[i % tabela->tam].estado = 1;
		strcpy(tabela->v[i % tabela->tam].pk, pk);
		tabela->v[i % tabela->tam].rrn = rrn;
	} else if (cont == tabela->tam) { 
		printf(ERRO_TABELA_CHEIA);
		return -1;
	}

	return cont;
}

/* IMPRESSÃO DA TABELA HASH */
void imprimir_tabela(Hashtable tabela) {
	int i;

	for (i = 0; i < tabela.tam; i++) {
		//Ocupado
		if (tabela.v[i].estado == 1) printf("[%d] Ocupado: %s\n", i, tabela.v[i].pk);

		//Livre
		else if (tabela.v[i].estado == 0) printf("[%d] Livre\n", i);

		//Removido
		else if (tabela.v[i].estado == -1) printf("[%d] Removido\n", i);
	}
}

/* REMOÇÃO */
int remover(Hashtable* tabela) {
	char pk[TAM_PRIMARY_KEY];

	scanf("%[^\n]", pk); getchar();

	//Busca pela chave inserida e a remove se for encontrada
	int cont = 0, i = hash(pk, tabela->tam);
	while ((cont != tabela->tam) && (tabela->v[i % tabela->tam].estado == 1)) {
		if (!strcmp(tabela->v[i % tabela->tam].pk, pk)) {
			tabela->v[i % tabela->tam].estado = -1;

			//Marca o registro como removido no arquivo de dados
			char *arq = &ARQUIVO[192 * tabela->v[i % tabela->tam].rrn];
			arq[0] = '*'; arq[1] = '|';

			return 1;
		}

		i++;
		cont++;
	}

	printf(REGISTRO_N_ENCONTRADO);
	return 0;
}

/* BUSCAR */
void buscar(Hashtable tabela) {
	char pk[TAM_PRIMARY_KEY];

	scanf("%[^\n]", pk); getchar();

	//Busca pela chave inserida 
	int cont = 0, i = hash(pk, tabela.tam);
	while ((cont != tabela.tam) && (tabela.v[i % tabela.tam].estado == 1)) {

		//Exibe-a caso a encontre
		if (!strcmp(tabela.v[i % tabela.tam].pk, pk)) {
			exibir_registro(tabela.v[i % tabela.tam].rrn);
			return;
		}

		i++;
		cont++;
	}

	printf(REGISTRO_N_ENCONTRADO);
}

/* ALTERAÇÃO */
int  alterar(Hashtable tabela) {
	char pk[TAM_PRIMARY_KEY];
	int rrn = -1;

	scanf("%[^\n]", pk); getchar();

	//Busca pela chave inserida 
	int cont = 0, i = hash(pk, tabela.tam);
	while ((cont != tabela.tam) && (tabela.v[i % tabela.tam].estado == 1)) {

		//Para o laço caso a chave seja encontrada
		if (!strcmp(tabela.v[i % tabela.tam].pk, pk)) {rrn = tabela.v[i % tabela.tam].rrn; break;}

		i++;
		cont++;
	}

	//Verifica se a chave foi encontrada
	if (rrn != -1) {
		int desc, aux;
		char desconto[1000], *arq = &ARQUIVO[192 * rrn];

		//Lê o valor de desconto sugerido até que ele seja válido
		scanf("%[^\n]", desconto); getchar();
		aux = strlen(desconto);
		sscanf(desconto, "%d", &desc);
		while (aux > 3 || aux < 3 || !isdigit(desconto[0]) || !isdigit(desconto[1]) || !isdigit(desconto[2]) || desc < 0 || desc > 100) {
			printf(CAMPO_INVALIDO);
			scanf("%[^\n]", desconto); getchar();
			aux = strlen(desconto);
			sscanf(desconto, "%d", &desc);
		}

		//Encontra a posição a ser alterada
		aux = 0; i = 0;//Contador de @
		while (aux != 6) {
			if (arq[i] == '@') aux++;
			i++;
		}

		sprintf(&arq[i], "%s", desconto); arq[i+3] = '@';

		return 1;
	}

	printf(REGISTRO_N_ENCONTRADO);
	return 0;
}

/* DESALOCAR TABELA */
void liberar_tabela(Hashtable* tabela) {
	free(tabela->v);
}