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
//Struct para índice secundário comum; usado apenas para preço com desconto
typedef struct secundary_index{
   char pk[TAM_PRIMARY_KEY];
   char chave[TAM_PRECO];
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
int compare_Sind_Bsearch (const void *actual, const void *search);
int compare_IS_Bsearch (const void *actual, const void *search);

//Função para comparação de valores
int max (int a, int b);

//Funções de conversão (para funções que manipulam o preco)
double str2double (char *string);
int str2int (char *string);

/* Funções para índices secundários */
//Dado um arquivo de entrada, lê todos os registros e insere no arquivo de índices secundários
void criar_IndicesSecundarios (Ir **igame, Ir **idev, Ir **icat, Is *iprice, int *ngame, int *ndev, int *ncat, int *nprice, int nregistros);

/* Funções para índices secunários com lista invertida */
//Insere novo IS se esse não existir previamente, senão insere novo IP na lista
//invertida do IS passado
void inserir_ListaInvertida (Ir **indice_secundario, char *Sec_index, char *P_index, int *num);
void inserir_Cat (Ir **categoria, char *cat, char *P_index, int *num);

//Inserção no vetor de índices secundários simples
void inserir_IS (Is *indice_secundario, char *price, char *desc, char *pk, int *num);

//Realiza busca do índice secundário desejado
Ir *recuperar_ListaInvertida(Ir *indice_secundario, char *key, int num);

//Exibe os índices primários associados a um título
int exibir_ListaInvertida(Ir *game, Ip *indice_primario, int nregistros);

/* Funções para listagem */
int listar_ListaInvertida (Ir *indice_secundario, Ip *indice_primario, int nregistros, int nSecIndex);
int listar_precos (Is *iprice, Ip *indice_primario, int nregistros, int nprice);

/* Funções para busca */
int buscaDevCat(Ip *indice_primario, Ir *icat, Ir *idev, char *cat, char *dev, int ncat, int ndev, int nregistros);

/* Rotinas para lista invertida (índices primários referenciados) */
//Insere novo índice primário referenciado, em ordem lexicográfica
void insere_novoNo (ll **lista, ll *novo_no);

/* Funções de manipulação do arquivo de dados */
//Insere um novo registro no arquivo de dados
void insere_Arq (Jogo titulo, int nregistros);

//Função de ordenação
void selectionSort (int ip, int ir, int is, Ip *iprimary, Ir *listaInvertida, Is *iprice, int num);

//Função de alteração
int alterar(Ip *iprimary, Is **iprice, int nregistros, int nprice);

/* Função para remoção */
//Lê um índice primário e realiza todo o processo de remoção da opção 3
int remover_registro (Ip **iprimary, Ir **igame, Ir **idev, Ir **icat, Is **iprice, int nregistros, int ngame, int ndev, int ncat, int *nprice);

//Remove o índice desejado da lista de índices primários referenciados
void remove_No (ll **lista, char *P_index);

/* Função para limpar o arquivo */
void limpa_arquivos (Ip **iprimary, Ir **igame, Ir **idev, Ir **icat, int *nregistros, int *ngame, int *ndev, int *ncat);
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
	criar_iprimary(iprimary, nregistros);

	/* Alocar e criar índices secundários */
	Ir *igame = (Ir *) malloc (sizeof(Ir));
	Ir *idev = (Ir *) malloc (sizeof(Ir));
	Ir *icat = (Ir *) malloc (sizeof(Ir));
	Is *iprice = (Is *) malloc (MAX_REGISTROS * sizeof(Is));

	//Cria índices secundários a partir do arquivo
	criar_IndicesSecundarios (&igame, &idev, &icat, iprice, &ngame, &ndev, &ncat, &nprice, nregistros);

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

				/* INSERÇÃO ÍNDICES SECUNDÁRIOS -> INSERÇÃO DE LISTA INVERTIDA */
				inserir_ListaInvertida (&igame, titulo->nome, titulo->pk, &ngame);
				inserir_ListaInvertida (&idev, titulo->desenvolvedora, titulo->pk, &ndev);
				inserir_Cat (&icat, titulo->categoria, titulo->pk, &ncat);

				/* INSERÇÃO EM ÍNDICES SECUNDÁRIOS -> INSERÇÃO DE ÍNDICE SIMPLES */
				inserir_IS (iprice, titulo->preco, titulo->desconto, titulo->pk, &nprice);

				nregistros++;
			} else
				printf(ERRO_PK_REPETIDA, titulo->pk);

			break;
		case 2:
			/* alterar */
			printf(INICIO_ALTERACAO);
			if(alterar(iprimary, &iprice, nregistros, nprice))
				printf(SUCESSO);
			else
				printf(FALHA);
			break;
		case 3:
			/* excluir */
			printf(INICIO_EXCLUSAO);

			if(remover_registro (&iprimary, &igame, &idev, &icat, &iprice, nregistros, ngame, ndev, ncat, &nprice))
				printf(SUCESSO);
			else
				printf(FALHA);
			break;
		case 4:
			/* buscar */
			printf(INICIO_BUSCA );
			scanf("%d", &tipo); getchar();

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
				//Realiza a busca
				if (!buscaDevCat(iprimary, icat, idev, titulo->categoria, titulo->desenvolvedora, ncat, ndev, nregistros))
					printf(REGISTRO_N_ENCONTRADO);
			}

			break;
		case 5:
			/* listar */
			printf(INICIO_LISTAGEM);
			scanf("%d", &tipo); getchar();

			//Listagem por código
			if (tipo == 1) {
				if (!nregistros) printf(REGISTRO_N_ENCONTRADO);
				else
					for (i = 0; i < nregistros; i++) {
						exibir_registro(iprimary[i].rrn, 0);
						if (i + 1 < nregistros) printf("\n");
					}
			}

			//Listagem por categoria
			else if (tipo == 2) {
				if (!listar_ListaInvertida (icat, iprimary, nregistros, ncat))
					printf(REGISTRO_N_ENCONTRADO);
			}

			//Listagem por desenvolvedora
			else if (tipo == 3) {
				if (!listar_ListaInvertida (idev, iprimary, nregistros, ndev))
					printf(REGISTRO_N_ENCONTRADO);
			}

			//Listagem por preço
			else if (tipo == 4) {
				if (!listar_precos (iprice, iprimary, nregistros, nprice))
					printf(REGISTRO_N_ENCONTRADO);
			}
			break;
		case 6:
			limpa_arquivos (&iprimary, &igame, &idev, &icat, &nregistros, &ngame, &ndev, &ncat);
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

int compare_Sind_Bsearch (const void *actual, const void *search) {
	return strcmp(((Ir *)actual)->chave, (char *)search);
}

int compare_IS_Bsearch (const void *actual, const void *search) {
	int ret = strcmp(((Is *)actual)->chave, ((Is *)search)->chave);

	if (!ret) {
		return strcmp(((Is *)actual)->pk, ((Is *)search)->pk);
	}

	return ret;
}

//Comparação de maior valor
int max (int a, int b) {
	if (a > b) return a;
	return b;
}

/* FUNÇÕES DE MANIPULAÇÃO DE ARQUIVO*/
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

	char copy[193];
	char *arq = ARQUIVO;
	int i = rrn * 192;

	//Realiza a cópia do registro encontrado no arquivo de dados
	strncpy(copy, &arq[i], 192); copy[193] = '\0';

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

/* FUNÇÃO DE ORDENAÇÃO */
void selectionSort (int ip, int ir, int is, Ip *iprimary, Ir *listaInvertida, Is *iprice, int num) {
	int i, j;

	//Verifica qual opção de ordenação é desejada
	if (ip) {
		Ip aux;

		//Selection Sort
		for (i = 0; i < num; i++)
			for (j = i+1; j < num; j++)
				if (strcmp (iprimary[i].pk, iprimary[j].pk) > 0) {
					aux = iprimary[i];
					iprimary[i] = iprimary[j];
					iprimary[j] = aux;
				}
	} else if (ir) {
		Ir aux;

		//Selection Sort
		for (i = 0; i < num; i++)
			for (j = i+1; j < num; j++)
				if (strcmp (listaInvertida[i].chave, listaInvertida[j].chave) > 0) {
					aux = listaInvertida[i];
					listaInvertida[i] = listaInvertida[j];
					listaInvertida[j] = aux;
				}
	} else if (is) {
		Is aux;
		int ret;

		//Selection Sort
		for (i = 0; i < num; i++)
			for (j = i+1; j < num; j++) {
				ret = strcmp (iprice[i].chave, iprice[j].chave);

				//Trocas: se o preço analisado for maior que o outro comparado ou se os preços forem
				//iguais e a pk for maior no elemento analisado
				if ((ret > 0) || (!ret && (strcmp (iprice[i].pk, iprice[j].pk) > 0))) {
					aux = iprice[i];
					iprice[i] = iprice[j];
					iprice[j] = aux;
				}
			}
	}
}

/* ÍNDICES PRIMÁRIOS */
//Cria os índices primários a partir dos registros lidos do arquivo
void criar_iprimary(Ip *indice_primario, int nregistros) {
	int i;
	Jogo j;

	//Lê um registro, cria o índice primário e insere no arquivo de índices primários
	for (i = 0; i < nregistros; i++) {
		j = recuperar_registro(i);

		//Verificação de índice repetido
		if (!inserir_Pindex(indice_primario, j.pk, i))
			printf(ERRO_PK_REPETIDA, j.pk);
	}
}

//Dado uma struct de indice primário, insere e ordena no vetor
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

		//Reordena o vetor de índices usando selection sort
		selectionSort (1, 0, 0, indice_primario, NULL, NULL, (num+1));

		return 1;
	}
}

/* ÍNDICES SECUNDÁRIOS */

//Insere índice secundário quando recebe um arquivo de Dados
void criar_IndicesSecundarios (Ir **igame, Ir **idev, Ir **icat, Is *iprice, int *ngame, int *ndev, int *ncat, int *nprice, int nregistros) {
	int i;
	Jogo j;

	//Lê um registro e insere no arquivo de índices secundários
	for (i = 0; i < nregistros; i++) {
		j = recuperar_registro(i);

		//Inserção de título
		inserir_ListaInvertida (igame, j.nome, j.pk, ngame);

		//Inserção de desenvolvedora
		inserir_ListaInvertida (idev, j.desenvolvedora, j.pk, ndev);

		//Inserção de categoria
		inserir_Cat (icat, j.categoria, j.pk, ncat);

		//Inserção de preço com desconto
		inserir_IS (iprice, j.preco, j.desconto, j.pk, nprice);
	}
}

/* ÍNDICES SECUNDÁRIOS - LISTA INVERTIDA */
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
		selectionSort (0, 1, 0, NULL, (*indice_secundario), NULL, *num);
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

		//Verifica se vai inserir no começo da lista
		if (!ant) {
			(*lista) = novo_no;
			novo_no->prox = aux;
		}
		//Caso contrário, insere no meio da lista ou fim da lista
		else {
			novo_no->prox = aux;
			ant->prox = novo_no;
		}
	}
}

/* ÍNDICE SECUNDÁRIO SIMPLES */
void inserir_IS (Is *indice_secundario, char *price, char *desc, char *pk, int *num) {
	int i = *num;

	//Lê o preço, aplica do desconto e salva na string
	int desconto;
	double novoPreco;
	novoPreco = str2double(price);
	desconto = str2int(desc);
	novoPreco = novoPreco * (1 - ((float)desconto/100));

	//Insere o novo elemento na primeira posição livre do vetor de índices secundários
	sprintf(indice_secundario[i].chave, "%07.2lf", novoPreco); indice_secundario[i].chave[TAM_PRECO-1] = '\0';
	strcpy (indice_secundario[i].pk, pk); indice_secundario[i].pk[TAM_PRIMARY_KEY-1] = '\0';

	(*num) = (*num) + 1;

	//Reordena o vetor usando selection sort
	selectionSort (0, 0, 1, NULL, NULL, indice_secundario, *num);
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

int buscaDevCat(Ip *indice_primario, Ir *icat, Ir *idev, char *cat, char *dev, int ncat, int ndev, int nregistros) {
	//Recupera as listas invertidas da categoria e desenvolvedora em questão
	Ir *categ = recuperar_ListaInvertida(icat, cat, ncat);
	Ir *desenv = recuperar_ListaInvertida(idev, dev, ndev);
	ll *lista_cat = categ->lista, *lista_dev = desenv->lista;

	int match = 0, ret = 0;
	//Compara os elementos das listas buscando por pks iguais
	if (lista_cat && lista_dev) {
		while (lista_cat->prox || lista_dev->prox) {
			ret = strcmp(lista_cat->pk, lista_dev->pk);
			//Compara os elementos: se for igual, imprime o registro e movimenta os dois ponteiros de lista
			//Se for diferente, movimenta o ponteiro da lista cujo elemento é o menor entre os dois comparados
			if (!ret) {
				match = 1;
				exibir_registro(recuperar_rrn(indice_primario, lista_cat->pk, nregistros), 0);

				//Se não for NULL, passa para o próximo elemento da lista
				if (lista_cat->prox) lista_cat = lista_cat->prox;
				if (lista_dev->prox) lista_dev = lista_dev->prox;

			} else if (lista_cat->prox && ret < 0)
				lista_cat = lista_cat->prox;
			else if (lista_dev->prox && ret > 0)
				lista_dev = lista_dev->prox;

			//Caso uma das listas já tenha finalizado e o menor elemento ainda seja pertencente a
			//essa lista, o laço é interrompido pois não há mais necessidade de comparação
			else if ((lista_cat->prox && ret > 0) || (lista_dev->prox && ret < 0)) break;
		}
	}

	return match;
}

/* FUNÇÕES PARA LISTAGEM */
//Busca e exibe os índices primários associados a um título de jogo
int exibir_ListaInvertida(Ir *game, Ip *indice_primario, int nregistros) {
	//Retorna 0 se o jogo não for encontrado
	if (!game || !game->lista) return 0;

	ll *aux = game->lista;

	//Percorre toda a lista invertida exibindo os jogos
	while (aux != NULL) {
		exibir_registro(recuperar_rrn(indice_primario, aux->pk, nregistros), 0);

		aux = aux->prox;
		if (aux != NULL) printf("\n");
	}

	return 1;
}

//Listagem de índices em lista invertida
int listar_ListaInvertida (Ir *indice_secundario, Ip *indice_primario, int nregistros, int nSecIndex) {
	int i, contInexistentes = 0;

	//Imprime os jogos em ordem alfabética de índice secundário e, com relação às listas,
	//imprime em ordem alfabética de código
	for (i = 0; i < nSecIndex; i++) {
		//Contabiliza os índices secundários que não possuem lista de jogos
		if (!exibir_ListaInvertida(&indice_secundario[i], indice_primario, nregistros))
			contInexistentes++;
		else if (i + 1 < nSecIndex) printf("\n");
	}

	//Retorna 0 se todos os índices não possuírem lista e um número maior que 0 caso contrário
	return (nSecIndex - contInexistentes);
}

int listar_precos (Is *iprice, Ip *indice_primario, int nregistros, int nprice) {
	//Caso não tenha elementos no vetor
	if (!nprice) return 0;

	//Busca por chave primária e exibe o índice
	int i;
	for (i = 0; i < nprice; i++) {
		exibir_registro(recuperar_rrn(indice_primario, iprice[i].pk, nregistros), 1);
		if (i + 1 < nprice) printf("\n");
	}

	return 1;
}

/* FUNÇÕES DE ALTERAÇÃO DE DESCONTO */
int alterar(Ip *iprimary, Is **iprice, int nregistros, int nprice) {
	char pk[TAM_PRIMARY_KEY], descontoStr[5];

	scanf("%[^\n]", pk); getchar();

	//Realiza a busca do índice primário
	Ip *Pindex = bsearch(pk, iprimary, nregistros, sizeof(Ip), compare_ind_Bsearch);
	if (Pindex && Pindex->rrn >= 0) {

		//Recebe o desconto desejado e verifica o valor está dentro dos limites estabelecidos
		scanf("%[^\n]", descontoStr); getchar();
		int tam = strlen (descontoStr);
		int desconto = str2int(descontoStr);
		while (tam < 3 || tam > 3 || (desconto < 0 || desconto > 100)) {
			printf(CAMPO_INVALIDO);
			scanf("%[^\n]", descontoStr); getchar();
			tam = strlen (descontoStr);
			desconto = str2int(descontoStr);
		}

		double preco, comDesconto;
		int desc_atual;

		//Lê o registro, calcula o preço com desconto atual e busca no índice de preços
		Jogo j = recuperar_registro(Pindex->rrn);
		preco = str2double(j.preco);
		desc_atual = str2int(j.desconto);
		comDesconto = preco * (1 - ((float)desc_atual/100));
		Is search;
		sprintf (search.chave, "%07.2lf", comDesconto); search.chave[TAM_PRECO-1] = '\0';
		strcpy (search.pk, j.pk); search.pk[TAM_PRIMARY_KEY-1] = '\0';

		Is *aux = bsearch(&search, (*iprice), nprice, sizeof(Is), compare_IS_Bsearch);

		//Altera o valor e reordena o vetor
		comDesconto = preco * (1 - ((float)desconto/100));
		sprintf (aux->chave, "%07.2lf", comDesconto); aux->chave[TAM_PRECO-1] = '\0';
		selectionSort (0, 0, 1, NULL, NULL, (*iprice), nprice);

		//Altera o valor de desconto no arquivo de dados
		char *arq = ARQUIVO;
		int cont_arroba = 0, i = Pindex->rrn * 192;

		while (cont_arroba != 5) if (arq[i++] == '@') cont_arroba++;
		sprintf(&arq[i], "%03d", desconto); arq[i+3] = '@';

		return 1;
	}

	printf(REGISTRO_N_ENCONTRADO);
	return 0;
}

/* FUNÇÕES PARA REMOÇÃO */
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

//Remove índice primário da lista invertida do IS passado
int remover_registro (Ip **iprimary, Ir **igame, Ir **idev, Ir **icat, Is **iprice, int nregistros, int ngame, int ndev, int ncat, int *nprice) {
	char pk[TAM_PRIMARY_KEY];
	scanf("%s\n", pk); pk[TAM_PRIMARY_KEY-1] = '\0';

	//Busca pelo elemento na lista de índices primários
	Ip *Pindex = bsearch(pk, (*iprimary), nregistros, sizeof(Ip), compare_ind_Bsearch);

	//Verifica se o índice existe ou se já foi removido antes
	if (Pindex && Pindex->rrn >= 0) {
		//Recupera o registro para então realizar as remoções/ marcações de remoção
		Jogo j = recuperar_registro(Pindex->rrn);

		//Remoção no arquivo de dados
		ARQUIVO[Pindex->rrn * 192] = '|'; ARQUIVO[(Pindex->rrn * 192 + 1)] = '*';

		//Remoção em índice primário
		int ret = Pindex->rrn;
		Pindex->rrn = -1;

		//Remoção em listas invertidas -> Titulo do jogo e desenvolvedora
		Ir *aux = recuperar_ListaInvertida((*igame), j.nome, ngame);
		remove_No(&(aux->lista), pk);
		aux = recuperar_ListaInvertida((*idev), j.desenvolvedora, ndev);
		remove_No(&(aux->lista), pk);

		//Remoção em listas invertidas -> Categorias
		char *token;
		for (token = strtok(j.categoria, "|"); token; token = strtok(NULL, "|")) {
			aux = recuperar_ListaInvertida((*icat), token, ncat);
			remove_No(&(aux->lista), pk);
		}

		//Remoção em índices secundários simples -> Preços com desconto
		Is search;
		strcpy(search.pk, pk);

		//Lê o preço e o desconto, aplica o desconto no preço e busca no índice de precos
		double precoFinal;
		precoFinal = str2double(j.preco) * (1 - ((float)str2int(j.desconto)/100));
		sprintf(search.chave, "%07.2lf", precoFinal);

		Is *found = bsearch(&search, (*iprice), (*nprice), sizeof(Is), compare_IS_Bsearch);

		//Torna o valor do registro encontrado o maior do vetor
		precoFinal = str2double((*iprice)[(*nprice)-1].chave) + 1;
		sprintf(found->chave, "%07.2lf", precoFinal);

		//Reordena o vetor, deixa o último registro vazio e decrementa nprice
		selectionSort(0, 0, 1, NULL, NULL, (*iprice), (*nprice));
		(*iprice)[(*nprice)-1].chave[0] = '\0'; (*iprice)[(*nprice)-1].pk[0] = '\0';
		(*nprice) = (*nprice) - 1;

		return ret;
	}

	printf(REGISTRO_N_ENCONTRADO);
	return 0;
}

//Função de conversão de string pra double
double str2double (char *string) {
	double preco;
	sscanf(string, "%lf", &preco);
	return preco;
}

int str2int (char *string) {
	int desconto;
	sscanf(string, "%d", &desconto);
	return desconto;
}

/* FUNÇÃO PARA LIBERAÇÃO DE ESPAÇO */
void limpa_arquivos (Ip **iprimary, Ir **igame, Ir **idev, Ir **icat, int *nregistros, int *ngame, int *ndev, int *ncat) {
	//Remove do arquivo
	char *arq = ARQUIVO;
	int i = 0, mult = 1;

	while (arq[i] != '\0') {
		if (arq[i] == '|' && arq[i+1] == '*') {

			//Copia o registro seguinte para a posição do registro atual
			for (; arq[i] != '\0'; i++) {
				//Verifica a existência de outros registros excluídos
				if (arq[i + (mult * 192)] == '|' && arq[i + (mult * 192) + 1] == '*') mult++;
				arq[i] = arq[i + (mult * 192)];
			}

			break;
		}

		i++;
	}

	//Recria os índices primários
	free (*iprimary);
	(*iprimary) = (Ip *) malloc (MAX_REGISTROS * sizeof(Ip));
	(*nregistros) = strlen(ARQUIVO)/192;
	criar_iprimary((*iprimary), (*nregistros));

	/* CRIAR FUNÇÃO PARA LIMPAR ARQUIVO DE ÍNDICES SECUNDÁRIOS */
	/* NOTA: Não esquecer de arrumar a precisão de valores decimais */
}