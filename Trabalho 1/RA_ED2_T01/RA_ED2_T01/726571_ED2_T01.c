/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 01 - Indexação
 *
 * RA: 726571
 * Aluno: Matheus Nadaleti Garcia
 * ========================================================================== */

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 	11
#define TAM_NOME 			51
#define TAM_DESENVOLVEDORA	51
#define TAM_DATA			11
#define TAM_CLASSIFICACAO 	3
#define TAM_PRECO 			8
#define TAM_DESCONTO 		4
#define TAM_CATEGORIA 		51


#define TAM_REGISTRO 		192
#define MAX_REGISTROS 		1000
#define MAX_CATEGORIAS 		10
#define TAM_ARQUIVO 		(MAX_REGISTROS * TAM_REGISTRO + 1)


/* Saídas do usuário */
#define OPCAO_INVALIDA 			"Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE 	"Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO 	"Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO 			"Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA 		"ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO 			"Arquivo vazio!\n"
#define INICIO_BUSCA 		 	"**********************BUSCAR**********************\n"
#define INICIO_LISTAGEM  		"**********************LISTAR**********************\n"
#define INICIO_ALTERACAO 		"**********************ALTERAR*********************\n"
#define INICIO_EXCLUSAO  		"**********************EXCLUIR*********************\n"
#define INICIO_ARQUIVO  		"**********************ARQUIVO*********************\n"
#define SUCESSO  				"OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					"FALHA AO REALIZAR OPERACAO!\n"

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

/* Registros dos Índices*/

/* Struct para índice Primário */
typedef struct primary_index{
  char pk[TAM_PRIMARY_KEY];
  int rrn;
} Ip;

/*	Você deve optar por utilizar índices secundários simples, ou listas invertidas.
 *  Seguem abaixo sugestões para suas EDs*/
//Struct para índice secundário comum:
typedef struct secundary_index{
   char pk[TAM_PRIMARY_KEY];
   char *chave;
 } Is;

//Lista ligada simples para a lista invertida:
typedef struct linked_list{
  char pk[TAM_PRIMARY_KEY];
  struct linked_list *prox;
} ll;

//Struct para a lista invertida:
typedef struct reverse_index{
  char chave[51];
  ll* lista;
} Ir;

/* Variavel global que simula o arquivo de dados */
char ARQUIVO[TAM_ARQUIVO];

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */
 /* Exibe o menu de opções */
void print_menu();

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo();

/* Exibe o jogo, utilize com_desconto = 1 apenas para listagem por preço */
int exibir_registro(int rrn, char com_desconto);

/* Recupera do arquivo o registro com o rrn informado e retorna os dados na
 * struct Jogo */
Jogo recuperar_registro(int rrn);

/*Dada uma certa chave, retorna o rrn dela consultando o ip*/
int recuperar_rrn(Ip* iprimary, const char* pk, int nregistros);

/* (Re)faz o índice primário */
void criar_iprimary(Ip *indice_primario, int nregistros);

/* Salva os dados da entrada para a struct jogo*/
void ler_entrada (Jogo **titulo);

/* Funções para o arquivo de índices primários */
// Cria e insere novo índice primário
int inserir_Pindex (Ip *indice_primario, char *index, int num);

//Remove o índice primário desejado
int remover_Pindex (Ip *indice_primario, char *index);

//Funções de comparação para bsearch
int compare_ind_Bsearch (const void *search, const void *actual);
int compare_ind_Qsort (const void *search, const void *actual);
int compare_Sind_Bsearch (const void *actual, const void *search);
int compare_Sind_Qsort (const void *actual, const void *search);

//Função para comparação de valores
int max (int a, int b);

/* Funções para índices secundários */
//Insere novo IS se esse não existir previamente, senão insere novo IP na lista
//invertida do IS passado
void inserir_ListaInvertida (Ir **indice_secundario, char *Sec_index, char *P_index, int *num);
void inserir_Cat (Ir **categoria, char *cat, char *P_index, int *num);
void inserir_Preco (Ir **precos, char *Sec_index, char *P_index, int *num, char *desconto);

//Remove índice primário da lista invertida do IS passado
int remover_ListaInvertida (Ir *indice_secundario, char *Sec_index, char *P_index);

//Realiza busca do índice secundário desejado
Ir *recuperar_ListaInvertida(Ir *indice_secundario, char *key, int num);

//Exibe os índices primários associados a um título
int exibir_ListaInvertida(Ir *game, Ip *indice_primario, int nregistros);

//Rotinas para lista invertida (índices primários referenciados)
//Insere novo índice primário referenciado, em ordem lexicográfica
void insere_novoNo (ll **lista, ll *novo_no);

//Remove o índice desejado da lista de índices primários referenciados
void remove_No (ll **lista, char *P_index);

//Outras funções
//Insere um novo registro no arquivo de dados
void insere_Arq (Jogo titulo, int nregistros);

/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * ======================================================================= */
int main(){
  /* Arquivo */
	int tipo;
	int carregarArquivo = 0, nregistros = 0, ncat = 0, ngame = 0, ndev = 0, nprice = 0;
	Jogo* titulo = (Jogo *) malloc (sizeof(Jogo));

	ll* aux;

	scanf("%d%*c", &carregarArquivo); /* 1 (sim) | 0 (nao) */

	if (carregarArquivo)
		nregistros = carregar_arquivo();

	/* Índice primário */
	Ip *iprimary = (Ip *) malloc (MAX_REGISTROS * sizeof(Ip));
  if (!iprimary) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	//criar_iprimary(iprimary, nregistros);

	/* Alocar e criar índices secundários */
	Ir *igame = (Ir *) malloc (sizeof(Ir));
	Ir *idev = (Ir *) malloc (sizeof(Ir));
	Ir *icat = (Ir *) malloc (sizeof(Ir));
	Is *iprice = (Is *) malloc (sizeof(Ir));

	//Cria índices a partir do arquivo

    /* Execução do programa */
	int opcao = 0, i;
	while(1) {
		scanf("%d%*c", &opcao);
		switch(opcao) {

		// INSERÇÃO
		case 1:
			ler_entrada(&titulo);

			//Insere um novo índice na lista de índices primários
			if (inserir_Pindex(iprimary, titulo->pk, nregistros)) {

				insere_Arq ((*titulo), nregistros);

				/* INSERÇÃO ÍNDICES SECUNDÁRIOS -> INSERÇÃO LISTA INVERTIDA */
				inserir_ListaInvertida (&igame, titulo->nome, titulo->pk, &ngame);
				inserir_ListaInvertida (&idev, titulo->desenvolvedora, titulo->pk, &ndev);

				inserir_Cat (&icat, titulo->categoria, titulo->pk, &ncat);
				//inserir_Preco (&iprice, titulo->preco, titulo->pk, &nprice, titulo->desconto);

				//Imprimir índices primários
				// printf("===== IPRIMARY =====\n");
				// for (i = 0; i < nregistros; i++)
				// 	printf("\n%s\n", iprimary[i].pk);
				//
				// //Imprimir índices secundários e suas listas
				// printf("===== IGAME =====\n");
				// for (i = 0; i < ngame; i++) {
				// 	printf("\n%s\n", igame[i].chave);
				// 	aux = igame[i].lista;
				// 	while (aux != NULL) {
				// 		printf("\t%s\n", aux->pk);
				// 		aux = aux->prox;
				// 	}
				// }
				//
				// aux = NULL;
				//
				// printf("===== IDEV =====\n");
				// for (i = 0; i < ndev; i++) {
				// 	printf("\n%s\n", idev[i].chave);
				// 	aux = idev[i].lista;
				// 	while (aux != NULL) {
				// 		printf("\t%s\n", aux->pk);
				// 		aux = aux->prox;
				// 	}
				// }
				//
				// aux = NULL;
				//
				// printf("===== ICAT =====\n");
				// for (i = 0; i < ncat; i++) {
				// 	printf("\n%s\n", icat[i].chave);
				// 	aux = icat[i].lista;
				// 	while (aux != NULL) {
				// 		printf("\t%s\n", aux->pk);
				// 		aux = aux->prox;
				// 	}
				// }
				//
				// aux = NULL;
				// printf("===== IPRICE =====\n");
				// for (i = 0; i < nprice; i++) {
				// 	printf("\n%s\n", iprice[i].chave);
				// 	aux = iprice[i].lista;
				// 	while (aux != NULL) {
				// 		printf("\t%s\n", aux->pk);
				// 		aux = aux->prox;
				// 	}
				// }
				//
				// aux = NULL;

				nregistros++;
			} else
				printf(ERRO_PK_REPETIDA, titulo->pk);

			break;
		case 2:
			/* alterar */
			printf(INICIO_ALTERACAO);
			/*if(alterar([args]))
				printf(SUCESSO);
			else
				printf(FALHA);*/
			break;
		case 3:
			/* excluir */
			printf(INICIO_EXCLUSAO);
			/*if(remover([args]))
				printf(SUCESSO);
			else
				printf(FALHA);*/
			break;
		case 4:
			/* buscar */
			printf(INICIO_BUSCA );
			scanf("%d\n", &tipo);

			//Busca por índice primário
			if (tipo == 1) {
				//Lê a chave primária que deseja-se buscar
				scanf("%[^\n]", titulo->pk); getchar();
				if (!exibir_registro(recuperar_rrn(iprimary, titulo->pk, nregistros), 0))
					printf(REGISTRO_N_ENCONTRADO);
			}

			//Busca por título do jogo
			else if (tipo == 2) {
				//Lê o título que deseja-se buscar
				scanf("%[^\n]", titulo->nome); getchar();
				if (!exibir_ListaInvertida(recuperar_ListaInvertida(igame, titulo->nome, ngame), iprimary, nregistros))
					printf(REGISTRO_N_ENCONTRADO);
			}

			//Busca por nome de desenvolvedora e categoria
			else if (tipo == 3) {
				//Lê o desenvolvedora que deseja-se buscar
				scanf("%s", titulo->desenvolvedora); getchar();

				//Lê a categoria que deseja-se buscar
				scanf("%s", titulo->categoria); getchar();
			}

			break;
		case 5:
			/* listar */
			printf(INICIO_LISTAGEM);

			break;
		case 6:
			/* liberar espaco */
		break;
		case 7:
			/* imprimir arquivo de dados*/
			printf(INICIO_ARQUIVO);
			printf("%s\n", ARQUIVO);
			break;
		case 8:
    		/* Liberar memoria e finalizar */
			return (0);
			break;
		default:
			printf(OPCAO_INVALIDA);
			break;
		}
	}
	return (0);
}



/* Exibe o jogo, utilize com_desconto = 1 para listagem por preço */
int exibir_registro(int rrn, char com_desconto) {

	if(rrn < 0)
		return (0);

	float preco;
	short int desconto;
	Jogo j = recuperar_registro(rrn);
	char *cat, categorias[TAM_CATEGORIA];

	printf("%s\n", j.pk);
	printf("%s\n", j.nome);
	printf("%s\n", j.desenvolvedora);
	printf("%s\n", j.data);

	if(com_desconto){
		sscanf(j.preco, "%f", &preco);
		sscanf(j.desconto, "%hd", &desconto);
		printf("%07.2f\n", preco * (100-desconto) /(float)100 );
	}
	else{
		printf("%s\n", j.preco);
		printf("%s\n", j.desconto);
	}

	strncpy(categorias, j.categoria, max(strlen(j.categoria), TAM_CATEGORIA - 1));
  	for (cat = strtok (categorias, "|"); cat != NULL; cat = strtok (NULL, "|"))
    	printf("%s ", cat);

	printf("\n");
	return (1);
}

/* Imprime o menu para o usuário */
void print_menu(){
		printf("\n 1. Cadastro");
		printf("\n 2. Alteração");
		printf("\n 3. Remoção");
		printf("\n 4. Busca");
		printf("\n 5. Listagem");
		printf("\n 6. Liberar espaço");
		printf("\n 7. Finalizar");
		printf("\n\nEntre com uma das opções acima: ");
}

/* Carrega o arquivo de dados */
int carregar_arquivo() {
	scanf("%[^\n]\n", ARQUIVO);
	return (strlen(ARQUIVO) / TAM_REGISTRO);
}

//Comparação para Busca
int compare_ind_Bsearch (const void *actual, const void *search) {
	return strcmp(((Ip *)actual)->pk, (char *)search);
}

int compare_ind_Qsort (const void *actual, const void *search) {
	return strcmp(((Ip *)actual)->pk, ((Ip *)search)->pk);
}

int compare_Sind_Bsearch (const void *actual, const void *search) {
	return strcmp(((Ir *)actual)->chave, (char *)search);
}

int compare_Sind_Qsort (const void *actual, const void *search) {
	return strcmp(((Ir *)actual)->chave, ((Ir *)search)->chave);
}

//Comparação de maior valor
int max (int a, int b) {
	if (a > b) return a;
	return b;
}

/* OUTRAS FUNÇÕES */
void ler_entrada (Jogo **titulo) {
	//Recebe todos os dados do jogo
	scanf ("%[^\n]", (*titulo)->nome); getchar();
	(*titulo)->nome[strlen((*titulo)->nome)] = '\0';
	scanf ("%[^\n]", (*titulo)->desenvolvedora); getchar();
	(*titulo)->desenvolvedora[strlen((*titulo)->desenvolvedora)] = '\0';
	scanf ("%[^\n]", (*titulo)->data); getchar();
	(*titulo)->data[TAM_DATA-1] = '\0';
	scanf ("%[^\n]", (*titulo)->classificacao); getchar();
	(*titulo)->preco[TAM_CLASSIFICACAO-1] = '\0';
	scanf ("%[^\n]", (*titulo)->preco); getchar();
	(*titulo)->preco[TAM_PRECO-1] = '\0';
	scanf ("%[^\n]", (*titulo)->desconto); getchar();
	(*titulo)->desconto[TAM_DESCONTO-1] = '\0';
	scanf ("%[^\n]", (*titulo)->categoria); getchar();
	(*titulo)->categoria[strlen((*titulo)->categoria)] = '\0';

	//Cria o índice primário
	sprintf ((*titulo)->pk, "%c%c%c%c%c%c%c%c%s", (*titulo)->nome[0], (*titulo)->nome[1],
	(*titulo)->desenvolvedora[0], (*titulo)->desenvolvedora[1], (*titulo)->data[0],
	(*titulo)->data[1], (*titulo)->data[3], (*titulo)->data[4], (*titulo)->classificacao);
}

//Insere um novo registro no arquivo de dados
void insere_Arq (Jogo titulo, int nregistros) {
	char *archive = ARQUIVO;

	//Encontra a quantidade de caracteres '#' devem ser inseridas
	int qntd_sharps = 192 - (29 + strlen(titulo.nome) + strlen(titulo.desenvolvedora) + strlen(titulo.categoria));

	//Escreve o registro no arquivo de dados no primeiro espaço em branco
	sprintf((archive + (192 * nregistros)), "%s@%s@%s@%s@%s@%s@%s@", titulo.nome, titulo.desenvolvedora,
	titulo.data,	titulo.classificacao, titulo.preco, titulo.desconto, titulo.categoria);

	//Coloca os "#" no arquivo
	int i;
	int position = (192 * (nregistros+1)) - qntd_sharps;
	for (i = 0; i < qntd_sharps; i++)
		sprintf ((archive + (position + i)), "#");
}

Jogo recuperar_registro(int rrn) {
	Jogo j;

	char copy[192];
	char *arq = ARQUIVO;
	int i = rrn * 192;

	//Realiza a cópia do registro encontrado no arquivo de dados
	while (arq[i] != '#')
		copy[i - (rrn * 192)] = arq[i++];

	strcpy (j.nome, strtok(copy, "@"));
	strcpy (j.desenvolvedora, strtok(NULL, "@"));
	strcpy (j.data, strtok(NULL, "@"));
	strcpy (j.classificacao, strtok(NULL, "@"));
	strcpy (j.preco, strtok(NULL, "@"));
	strcpy (j.desconto, strtok(NULL, "@"));
	strcpy (j.categoria, strtok(NULL, "@"));

	sprintf (j.pk, "%c%c%c%c%c%c%c%c%s", j.nome[0], j.nome[1], j.desenvolvedora[0],
	j.desenvolvedora[1], j.data[0],	j.data[1], j.data[3], j.data[4], j.classificacao);

	return j;
}

/* ÍNDICES PRIMÁRIOS */
int inserir_Pindex (Ip *indice_primario, char *index, int num) {
	//Realiza a busca binária e retorna o ponteiro para a posição do índice, se existir
	if (num && bsearch (index, indice_primario, num, sizeof(Ip), compare_ind_Bsearch)) {
		//Elemento já existe na lista
		return 0;
	} else {
		/* Inserção na lista estática de índices primários */

		//Acrescenta uma nova posição no vetor e insere o novo índice
		strcpy (indice_primario[num].pk, index);
		indice_primario[num].rrn = num;

		//Reordena o vetor de índices usando qsort
		qsort (indice_primario, (num+1), sizeof(Ip), compare_ind_Qsort);

		return 1;
	}
}

/* ÍNDICES SECUNDÁRIOS */
void inserir_Cat (Ir **categoria, char *cat, char *P_index, int *num) {
	//Copia a string
	char *copy = (char *) malloc ((strlen(cat)+1) * sizeof(char));
	strcpy(copy, cat);
	char *token = strtok(copy, "|");

	//Separa a string de categorias em vários tokens e cria o arquivo de índices de categorias
	while (token) {
		inserir_ListaInvertida(categoria, token, P_index, num);
		token = strtok(NULL, "|");
	}

	free(copy);
}

void inserir_Preco (Ir **precos, char *Sec_index, char *P_index, int *num, char *desconto) {
	float preco;
	int desc;

	sscanf(Sec_index, "%f", &preco);
	sscanf(desconto, "%d", &desc);
	//Calcula o valor do jogo com o desconto aplicado
	preco = preco * (1 - ((float)desc/100));
	char comDesconto[TAM_PRECO];

	sprintf(comDesconto, "%07.2f", preco);
	inserir_ListaInvertida (precos, comDesconto, P_index, num);
}

void inserir_ListaInvertida (Ir **indice_secundario, char *Sec_index, char *P_index, int *num) {
	Ir *found = bsearch(Sec_index, (*indice_secundario), (*num), sizeof(Ir), compare_Sind_Bsearch);
	int flag = 0;

	//Para o caso de ser um novo índice secundário
	if (found == NULL) {
		(*indice_secundario) = (Ir *) realloc ((*indice_secundario), (*num + 1) * sizeof(Ir));
		strcpy ((*indice_secundario)[*num].chave, Sec_index);
		(*indice_secundario)[*num].lista = NULL;
		found = &(*indice_secundario)[*num];

		(*num) = (*num) + 1;
		flag = 1;
	}

	//Inserção na lista de índices primários referenciados
	ll *novo_no = (ll *) malloc (sizeof(ll));
	strcpy(novo_no->pk, P_index);
	novo_no->prox = NULL;
	insere_novoNo(&found->lista, novo_no);

	//Reordena o vetor
	if (flag)
		qsort((*indice_secundario), (*num), sizeof(Ir), compare_Sind_Qsort);
}

/* ÍNDICES PRIMÁRIOS REFERENCIADOS */
//Insere novo nó na lista de índices primários
void insere_novoNo (ll **lista, ll *novo_no) {
	//Verifica se a lista está vazia
	if (!(*lista)) {
		(*lista) = novo_no;
		novo_no->prox = NULL;
	} else {

		//Insere o elemento na lista segundo ordem alfabética
		ll *ant = NULL;
		ll *aux = (*lista);

		while (aux && strcmp(aux->pk, novo_no->pk) < 0) {
			ant = aux;
			aux = aux->prox;
		}

		//Chegou ao fim da fila
		if (aux == NULL) {
			novo_no->prox = NULL;
			ant->prox = novo_no;
		}
		//Caso contrário, verifica se vai inserir no começo da lista
		else if (!ant) {
			(*lista) = novo_no;
			novo_no->prox = aux;
		}
		//Caso contrário, insere no meio da lista
		else {
			novo_no->prox = aux;
			ant->prox = novo_no;
		}
	}
}

//Remove um nó da lista de índices primários
void remove_No (ll **lista, char *P_index) {
	//Caso a lista seja vazia
	if (!(*lista)) return;

	//Busca pelo nó desejado
	ll *ant = (*lista);

	//A chave procurada sempre existirá. Por isso, deve-se verificar apenas se
	//esta se encontra na primeira posição
	if (!strcmp(ant->pk, P_index)) {
		(*lista) = ant->prox;
		free(ant);
	} else {
		while (ant->prox && strcmp(ant->prox->pk, P_index))
			ant = ant->prox;

		ll *rem = ant->prox;

		ant = rem->prox;
		free (rem);
	}
}

/* BUSCAS */
//Dada uma chave primária, encontra o rrn correspondente a ela
int recuperar_rrn(Ip* iprimary, const char* pk, int nregistros) {
	Ip *search = bsearch(pk, iprimary, nregistros, sizeof(Ip), compare_ind_Bsearch);

	if (search != NULL)	return search->rrn;
	else return -1;
}

//Dada uma chave secundária, retorna um ponteiro para o registro de chave secundária
//correspondente. Se não for encontrado, retorna NULL
Ir *recuperar_ListaInvertida(Ir *indice_secundario, char *key, int num) {
	Ir *ret = bsearch (key, indice_secundario, num, sizeof(Ir), compare_Sind_Bsearch);

	return ret;
}

//Busca e exibe os índices primários associados a um título de jogo
int exibir_ListaInvertida(Ir *game, Ip *indice_primario, int nregistros) {
	//Retorna 0 se o jogo não for encontrado
	if (!game) return 0;

	ll *aux = game->lista;
	Ip *pk;

	//Percorre toda a lista invertida exibindo os jogos
	while (aux) {
		exibir_registro(recuperar_rrn(indice_primario, aux->pk, nregistros), 0);

		aux = aux->prox;
		if (aux) printf("\n");
	}

	return 1;
}
