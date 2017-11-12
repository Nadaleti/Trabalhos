/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 02 - Árvore B
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
#define TAM_DESENVOLVEDORA 	51
#define TAM_DATA 			11
#define TAM_CLASSIFICACAO 	3
#define TAM_PRECO 			8
#define TAM_DESCONTO 		4
#define TAM_CATEGORIA 		51
#define TAM_STRING_INDICE 	(TAM_DESENVOLVEDORA + TAM_NOME)


#define TAM_REGISTRO 		192
#define MAX_REGISTROS 		1000
#define MAX_ORDEM 			150
#define TAM_ARQUIVO 		(MAX_REGISTROS * TAM_REGISTRO + 1)


/* Saídas do usuário */
#define OPCAO_INVALIDA 			 "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE	 "Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO 	 "Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO 			 "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA 		 "ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO 			 "Arquivo vazio!"
#define INICIO_BUSCA			 "********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM 		 "********************************LISTAR********************************\n"
#define INICIO_ALTERACAO		 "********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO			 "********************************ARQUIVO*******************************\n"
#define INICIO_INDICE_PRIMARIO	 "***************************INDICE PRIMÁRIO****************************\n"
#define INICIO_INDICE_SECUNDARIO "***************************INDICE SECUNDÁRIO**************************\n"
#define SUCESSO  				 "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 "FALHA AO REALIZAR OPERACAO!\n"
#define NOS_PERCORRIDOS_IP 		 "Busca por %s. Nos percorridos:\n"
#define NOS_PERCORRIDOS_IS 		 "Busca por %s.\nNos percorridos:\n"


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


/*Estrutura da chave de um nó do Índice Primário*/
typedef struct Chaveip
{
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Chave_ip;

/*Estrutura da chave de um  do Índice Secundário*/
typedef struct Chaveis
{
	char string[TAM_STRING_INDICE];
	char pk[TAM_PRIMARY_KEY];
} Chave_is;

/* Estrutura das Árvores-B */
typedef struct nodeip
{
	int num_chaves;		/* numero de chaves armazenadas*/
	Chave_ip *chave;	/* vetor das chaves e rrns [m-1]*/
	int *desc;			/* ponteiros para os descendentes, *desc[m]*/
	char folha;			/* flag folha da arvore*/
} node_Btree_ip;

typedef struct nodeis
{
	int num_chaves;		/* numero de chaves armazenadas*/
	Chave_is *chave;	/* vetor das chaves e rrns [m-1]*/
	int *desc;			/* ponteiros para os descendentes, *desc[m]*/
	char folha;			/* flag folha da arvore*/
} node_Btree_is;

typedef struct {
	int raiz;
} Indice;

/* Variáveis globais */

char ARQUIVO[MAX_REGISTROS * TAM_REGISTRO];
char ARQUIVO_IP[2000*sizeof(Chave_ip) + 40];
char ARQUIVO_IS[2000*sizeof(Chave_is) + 40];
/*Ordem das árvores*/
int ordem_ip;
int ordem_is;
int nregistros;
int nregistrosip; /*Número de nós presentes no ARQUIVO_IP*/
int nregistrosis; /*Número de nós presentes no ARQUIVO_IS*/
/*Quantidade de bytes que ocupa cada nó da árvore nos arquivos de índice:*/
int tamanho_registro_ip;
int tamanho_registro_is;
/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

 /********************FUNÇÕES DO MENU*********************/
 void cadastrar(Indice* iprimary, Indice* idev);

 int alterar(Indice iprimary);

 void buscar(Indice iprimary,Indice idev);

 void listar(Indice iprimary,Indice idev);

 /*******************************************************/

 /* Recebe do usuário uma string simulando o arquivo completo e retorna o número
  * de registros. */
 int carregar_arquivo();

/* (Re)faz o Cria iprimary*/
void criar_iprimary(Indice *iprimary);

/* (Re)faz o índice de jogos  */
void criar_idev(Indice *idev) ;

/*Escreve um nó da árvore no arquivo de índice,
* O terceiro parametro serve para informar qual indice se trata */
void write_btree(void *salvar, int rrn, char ip);

/*Lê um nó do arquivo de índice e retorna na estrutura*/
void *read_btree(int rrn, char ip);

/* Aloca um nó de árvore para ser utilizado em memória primária, o primeiro parametro serve para informar que árvore se trata
* É conveniente que essa função também inicialize os campos necessários com valores nulos*/
void *criar_no(char ip);

/*Libera todos os campos dinâmicos do nó, inclusive ele mesmo*/
void libera_no(void *node, char ip);

/*
*	Caro aluno,
*	caso não queira trabalhar com void*, é permitido dividir as funções que utilizam
* em duas, sendo uma para cada índice...
* Um exemplo, a write_btree e read_btree ficariam como:
*
*	void write_btree_ip(node_Btree_ip *salvar, int rrn),  node_Btree_ip *read_btree_ip(int rrn),
*	void write_btree_is(node_Btree_is *salvar, int rrn) e node_Btree_is *read_btree_is(int rrn).
*/



/* Atualiza os dois índices com o novo registro inserido */
void inserir_registro_indices(Indice *iprimary, Indice *idev, Jogo j);

/* Exibe o jogo */
int exibir_registro(int rrn);

/* FUNÇÕES PARA INSERÇÃO */
int inserir (Indice *raiz, Jogo j, char ip);
int insere_aux(int rrn, void *chave_ins, void **prom_key, int ip);
int split(void *no, void *chave_ins, void *prom_key, int rrn_no_criado, int ip);

/* FUNÇÕES PARA MANIPULAÇÃO DE ARQUIVO */

/* Lê a entrada e salva os dados em uma struct Jogo */
void ler_entrada (Jogo **titulo);

int main()
{

	int carregarArquivo = 0; nregistros = 0, nregistrosip = 0, nregistrosis = 0;
	scanf("%d\n", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	if (carregarArquivo)
		nregistros = carregar_arquivo();

	scanf("%d %d%*c", &ordem_ip, &ordem_is);

	tamanho_registro_ip = 3 + ((ordem_ip-1) * 14) + 1 + (ordem_ip * 3);
	tamanho_registro_is = 3 + ((ordem_is-1) * 111) + 1 + (ordem_is * 3);

	/* Índice primário */
	Indice iprimary;
	iprimary.raiz = -1;
	//criar_iprimary(&iprimary);

	/* Índice secundário de nomes dos Jogos */
	Indice idev;
	idev.raiz = -1;
	//criar_idev(&idev);

	/* Execução do programa */
	int opcao = 0; char *p;
	while(1)
	{
		scanf("%d%*c", &opcao);
		switch(opcao) {
		case 1: /* Cadastrar um novo jogo */
			cadastrar(&iprimary, &idev);
			break;
		case 2: /* Alterar o desconto de um Jogo */
			// printf(INICIO_ALTERACAO);
			// if (alterar(iprimary))
			// 	printf(SUCESSO);
			// else
			// 	printf(FALHA);
			break;
		case 3: /* Buscar um jogo */
			printf(INICIO_BUSCA);
			//buscar(iprimary, idev);
			break;
		case 4: /* Listar todos os Jogos */
			printf(INICIO_LISTAGEM);
			//listar(iprimary, idev);
			break;
		case 5: /* Imprimir o arquivo de dados */
			printf(INICIO_ARQUIVO);
			printf("%s\n", (*ARQUIVO!='\0') ? ARQUIVO : ARQUIVO_VAZIO);
			break;
		case 6: /* Imprime o Arquivo de Índice Primário*/
			printf(INICIO_INDICE_PRIMARIO);
			if(!*ARQUIVO_IP)
				puts(ARQUIVO_VAZIO);
			else
				for(p = ARQUIVO_IP; *p!='\0'; p+=tamanho_registro_ip)
				{
					fwrite( p , 1 ,tamanho_registro_ip,stdout);
					puts("");
				}
			break;
		case 7: /* Imprime o Arquivo de Índice Secundário*/
			printf(INICIO_INDICE_SECUNDARIO);
			if(!*ARQUIVO_IS)
				puts(ARQUIVO_VAZIO);
			else
				for(p = ARQUIVO_IS; *p!='\0'; p+=tamanho_registro_is)
				{
					fwrite( p , 1 ,tamanho_registro_is,stdout);
					puts("");

				}
			break;
		case 8: /*Libera toda memória alocada dinâmicamente (se ainda houver) e encerra*/
			return 0;
		default: /* Exibe mensagem de erro */
			printf(OPCAO_INVALIDA);
			break;
		}
	}
	return -1;
}

/* ==========================================================================
 * ================================= FUNÇÕES ================================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo()
{
	scanf("%[^\n]\n", ARQUIVO);
	return strlen(ARQUIVO) / TAM_REGISTRO;
}


/* Exibe o jogo */
/*int exibir_registro(int rrn)
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

	strncpy(categorias, j.categoria, strlen(j.categoria));

  	for (cat = strtok (categorias, "|"); cat != NULL; cat = strtok (NULL, "|"))
    	printf("%s ", cat);
	printf("\n");

	return 1;
}

/* ========================================================= */
void *criar_no(char ip) {
	int i = 0;

	//Aloca a estrutura o nó da estrutura passada como parâmetro
	if (ip == 'p') {
		node_Btree_ip *no = (node_Btree_ip *) malloc (sizeof(node_Btree_ip));
		no->num_chaves = 0;
		no->chave = (Chave_ip *) malloc ((ordem_ip-1) * sizeof(Chave_ip));
		no->desc = (int *) malloc (ordem_ip * sizeof(int));
		for (i = 0; i < ordem_ip; i++) no->desc[i] = -1;

		return no;
	} else if (ip == 's') {
		node_Btree_is *no = (node_Btree_is *) malloc (sizeof(node_Btree_is));
		no->num_chaves = 0;
		no->chave = (Chave_is *) malloc ((ordem_is-1) * sizeof(Chave_is));
		no->desc = (int *) malloc (ordem_is * sizeof(int));
		for (i; i < ordem_is; i++) no->desc[i] = -1;

		return no;
	}
}

/* FUNÇÕES PARA MANIPULAÇÃO DE ARQUIVO */
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

//Função para escrita de nó no arquivo de índices
void write_btree(void *salvar, int rrn, char ip) {
	char *no_salvar, *aux;
	int i, qntd_sharps, tam_no;

	//Faz o cast para o tipo desejado e aloca o tamanho da string de acordo com o índice
	if (ip == 'p') {
		tam_no = tamanho_registro_ip;

		node_Btree_ip *saveIp = (node_Btree_ip *) salvar;
		no_salvar = (char *) malloc (sizeof(tamanho_registro_ip) + 1);

		//Faz a transformação do nó em string
		sprintf (no_salvar, "%03d", saveIp->num_chaves); //Número de nós

		//Chaves concatenadas com seu RRN
		aux = &no_salvar[3];
		for (i = 0; i < saveIp->num_chaves; i++) {
			sprintf(aux, "%s%04d", saveIp->chave[i].pk, saveIp->chave[i].rrn);
			aux = aux + TAM_PRIMARY_KEY + 3;
		}

		//Preenchimento com # para chaves não existentes
		qntd_sharps = (ordem_ip - 1 - saveIp->num_chaves) * 14;
		i = (saveIp->num_chaves * 14) + 3;
		for (; qntd_sharps; i++, qntd_sharps--) no_salvar[i] = '#';

		//Flag de folha
		no_salvar[tamanho_registro_ip - (ordem_ip * 3) - 1] = saveIp->folha;

		//Descendentes
		aux = &no_salvar[tamanho_registro_ip - (ordem_ip * 3)];
		for (i = 0; saveIp->desc[i] != -1 && i < ordem_ip; i++) {
			sprintf(aux, "%03d", saveIp->desc[i]);
			aux = aux + 3;
		}

		//Descendentes inexistentes
		while (i < ordem_ip) {
			strcat(no_salvar, "***");
			i++;
		}

		//strcat(ARQUIVO_IP, no_salvar);
		aux = &ARQUIVO_IP[rrn * tamanho_registro_ip];

	} else if (ip == 's') {
		tam_no = tamanho_registro_is;

		node_Btree_is *saveIs = (node_Btree_is *) salvar;
		no_salvar = (char *) malloc (sizeof(tamanho_registro_is) + 1);

		//Faz a transformação do nó em string
		sprintf (no_salvar, "%03d", saveIs->num_chaves); //Número de nós

		//Pk + com nome desenvolvedora concatenado com nome do jogo
		char string_aux[112];
		int tam;
		aux = &no_salvar[3];
		for (i = 0; i < saveIs->num_chaves; i++) {
			tam = 0;

			strcpy(string_aux, saveIs->chave[i].pk);
			strcat(string_aux, saveIs->chave[i].string);

			//Coloca # nas posições restantes da string auxiliar
			tam = strlen(string_aux);
			while (tam < 112) {
				string_aux[tam] = '#'; 
				tam++;
			}

			//Concatena com a ser salva no arquivo
			string_aux[111] = '\0';
			strcat(no_salvar, string_aux);
			string_aux[0] = '\0';
		}

		//Preenchimento com # para chaves não existentes
		qntd_sharps = (ordem_is - 1 - saveIs->num_chaves) * 111;
		i = (saveIs->num_chaves * 111) + 3;
		for (; qntd_sharps; i++, qntd_sharps--) no_salvar[i] = '#';

		//Flag de folha
		no_salvar[tamanho_registro_is - (ordem_is * 3) - 1] = saveIs->folha;

		//Descendentes
		aux = &no_salvar[tamanho_registro_is - (ordem_is * 3)];
		for (i = 0; saveIs->desc[i] != -1 && i < ordem_is; i++) {
			sprintf(aux, "%03d", saveIs->desc[i]);
			aux = aux + 3;
		}

		//Descendentes inexistentes
		while (i < ordem_is) {
			strcat(no_salvar, "***");
			i++;
		}

		// strcat(ARQUIVO_IS, no_salvar);
		aux = &ARQUIVO_IS[rrn * tamanho_registro_is];
	}

	strncpy(aux, no_salvar, tam_no);
}

//Função para leitura de nó do arquivo de índices
void *read_btree(int rrn, char ip){
	char *arq_aux; //String auxiliar que é uma cópia do recuperado do arquivo
	char num[4];
	void *no = criar_no(ip);
	int i;

	//Recupera um registro do arquivo de dados de acordo com o tipo de estrutura desejado
	if (ip == 'p') {
		node_Btree_ip *no_lido = (node_Btree_ip *) no;

		arq_aux = &ARQUIVO_IP[rrn * tamanho_registro_ip];
		
		//Recupera o número de nós do registro
		strncpy(num, arq_aux, 3); num[4] = '\0';
		sscanf(num, "%d", &no_lido->num_chaves);

		//Recupera as chaves
		arq_aux += 3;
		for (i = 0; i < no_lido->num_chaves; i++) {
			strncpy(no_lido->chave[i].pk, arq_aux, 10); no_lido->chave[i].pk[TAM_PRIMARY_KEY-1] = '\0';
			arq_aux += 10;
			sscanf(arq_aux, "%d", &no_lido->chave[i].rrn);
			arq_aux += 4;
		}

		arq_aux += (ordem_ip - 1 - no_lido->num_chaves) * 14;

		//Recupera a informação sobre o nó ser folha ou não
		no_lido->folha = *(arq_aux++);

		//Recupera os descendentes
		for (i = 0; i < no_lido->num_chaves+1; i++) {
			strncpy(num, arq_aux, 3); num[4] = '\0';
			sscanf(num, "%d", &no_lido->desc[i]);
			arq_aux += 3;
		}

	} else if (ip == 's') {
		node_Btree_is *no_lido = (node_Btree_is *) no;

		arq_aux = &ARQUIVO_IS[rrn * tamanho_registro_is];

		//Recupera o número de nós do registro
		strncpy(num, arq_aux, 3); num[4] = '\0';
		sscanf(num, "%d", &no_lido->num_chaves);

		//Recupera as chaves
		arq_aux = &arq_aux[3];
		int cont;
		for (i = 0; i < no_lido->num_chaves; i++) {
			strncpy(no_lido->chave[i].pk, arq_aux, 10); no_lido->chave[i].pk[TAM_PRIMARY_KEY-1] = '\0';
			arq_aux += 10;

			//Copia a string (nome $ desenvolvedora)
			cont = 0;
			while (cont < TAM_STRING_INDICE-1 && arq_aux[cont] != '#' && arq_aux[cont] != '\0') {
				no_lido->chave[i].string[cont] = arq_aux[cont];
				cont++;
			}

			no_lido->chave[i].string[cont] = '\0';

			arq_aux += TAM_STRING_INDICE - 1;
		}

		arq_aux += (ordem_is - 1 - no_lido->num_chaves) * 111;

		//Recupera a informação sobre o nó ser folha ou não
		no_lido->folha = *(arq_aux++);

		//Recupera os descendentes
		for (i = 0; i < no_lido->num_chaves+1; i++) {
			strncpy(num, arq_aux, 3); num[4] = '\0';
			sscanf(num, "%d", &no_lido->desc[i]);
			arq_aux += 3;
		}
	}

	return no;
}

/* FUNÇÕES PARA INSERÇÃO */

/* Dado um jogo, insere-o nos arquivos de índices */
void cadastrar(Indice* iprimary, Indice* idev) {
	Jogo *titulo = (Jogo *) malloc (sizeof(Jogo));
	ler_entrada(&titulo);

	inserir_registro_indices(iprimary, idev, *titulo);
}

//Função para inserção do novo jogo nos índices
void inserir_registro_indices(Indice *iprimary, Indice *idev, Jogo j) {
	// void *no;
	// void *prom_key;
	// int no_criado;

	// /* INSERÇÃO EM IPRIMARY */

	// //Árvore vazia
	// if (iprimary->raiz == -1) {
	// 	no = criar_no('p');
	// 	node_Btree_ip *novo_no = (node_Btree_ip *)no;

	// 	novo_no->folha = 'F';
	// 	novo_no->num_chaves = 1;
	// 	strcpy(novo_no->chave[0].pk, j.pk);
	// 	novo_no->chave[0].rrn = nregistros;

	// 	iprimary->raiz = 0;

	// 	//Escreve o registro no arquivo de índices primários e no arquivo de dados
	// 	/* ESCREVE NO ARQUIVO DE DADOS */
	// 	write_btree(no, nregistrosip, 'p');

	// 	//Desaloca os dados que já foram utilizados
	// 	free(no);

	// 	nregistros++;
	// 	nregistrosip++;
	// }

	// //Árvore não vazia
	// else {
	// 	//Compõem a chave a ser inserida
	// 	prom_key = malloc (sizeof(Chave_ip));
	// 	Chave_ip *chave_ins = (Chave_ip *) malloc (sizeof(Chave_ip));
	// 	strcpy(chave_ins->pk, j.pk); chave_ins->rrn = nregistros;

	// 	//Busca o nó folha onde a chave deve ser inserida, realizando operações de divisão de nós(split) se necessário
	// 	no_criado = insere_aux(iprimary->raiz, chave_ins, &prom_key, 'p');

	// 	//Verifica se a propagação de split chegou até a raiz
	// 	//Chave inserida já existe na árvore
	// 	if (no_criado == -2){
	// 		printf(ERRO_PK_REPETIDA, j.pk);
	// 		return;
	// 	} else if (prom_key != NULL) {
	// 		no = criar_no('p');
	// 		node_Btree_ip *novo_no = (node_Btree_ip *)no;

	// 		novo_no->folha = 'N';
	// 		novo_no->num_chaves = 1;

	// 		novo_no->chave[0] = *((Chave_ip *)prom_key);

	// 		novo_no->desc[0] = iprimary->raiz;
	// 		novo_no->desc[1] = no_criado;

	// 		iprimary->raiz = nregistrosip;

	// 		//Escreve o registro no arquivo de índices primários e no arquivo de dados
	// 		/* ESCREVE NO ARQUIVO DE DADOS */
	// 		write_btree(no, nregistrosip, 'p');

	// 		//Desaloca os dados que já foram utilizados
	// 		free(no);
	// 		free(prom_key);

	// 		nregistros++;
	// 		nregistrosip++;
	// 	}
	// }

	// no = NULL;
	// prom_key = NULL;
	// no_criado = -1;

	// /* INSERÇÃO EM IDEV */
	// //Árvore vazia
	// if (idev->raiz == -1) {
	// 	no = criar_no('s');
	// 	node_Btree_is *novo_no = (node_Btree_is *)no;

	// 	novo_no->folha = 'F';
	// 	novo_no->num_chaves = 1;
	// 	strcpy(novo_no->chave[0].pk, j.pk);
	// 	strcpy(novo_no->chave[0].string, j.desenvolvedora);
	// 	strcat(novo_no->chave[0].string, "$");
	// 	strcat(novo_no->chave[0].string, j.nome);

	// 	idev->raiz = 0;
	// 	//Escreve o registro no arquivo de índices primários
	// 	write_btree(no, nregistrosis, 's');

	// 	//Desaloca as variáveis já utilizadas
	// 	free(no);

	// 	nregistrosis++;
	// }

	// //Árvore não vazia
	// else {
	// 	//Compõem a chave a ser inserida
	// 	prom_key = malloc(sizeof(Chave_is));
	// 	Chave_is *chave_ins = (Chave_is *) malloc (sizeof(Chave_is));
	// 	strcpy (chave_ins->pk, j.pk);
	// 	strcat (chave_ins->string, j.desenvolvedora);
	// 	strcat (chave_ins->string, "$");
	// 	strcat (chave_ins->string, j.nome);

	// 	//Busca o nó folha onde a chave deve ser inserida, realizando operações de divisão de nós(split) se necessário
	// 	no_criado = insere_aux(idev->raiz, chave_ins, prom_key, 's');

	// 	//Verifica se a propagação de split chegou até a raiz
	// 	if (prom_key != NULL) {

	// 		no = criar_no('s');
	// 		node_Btree_is *novo_no = (node_Btree_is *)no;

	// 		novo_no->folha = 'N';
	// 		novo_no->num_chaves = 1;

	// 		novo_no->chave[0] = *((Chave_is *)prom_key);

	// 		novo_no->desc[0] = idev->raiz;
	// 		novo_no->desc[1] = no_criado;

	// 		idev->raiz = nregistrosis;
	// 		//Escreve o registro no arquivo de índices primários
	// 		write_btree(no, nregistrosis, 's');

	// 		//Desaloca as variáveis já utilizadas
	// 		free(no);
	// 		free(prom_key);

	// 		nregistrosis++;
	// 	}
	// }

	if (inserir(iprimary, j, 'p') == -2) printf(ERRO_PK_REPETIDA, j.pk);
	else inserir(idev, j, 's');
}

//Função auxiliar para inserção
int inserir (Indice *ind, Jogo j, char ip) {
	void *no;
	void *prom_key;
	int no_criado, *nregs;

	//Define qual indicador de quantidade de registros será utilizado
	if (ip == 'p') nregs = &nregistrosip;
	else nregs = &nregistrosis;

	//Árvore vazia
	if (ind->raiz == -1) {
		no = criar_no(ip);

		//Insere dados sobre o novo nó de acordo com o tipo de estrutura
		if (ip == 'p') {
			node_Btree_ip *novo_no = (node_Btree_ip *)no;

			novo_no->folha = 'F';
			novo_no->num_chaves = 1;
			strcpy(novo_no->chave[0].pk, j.pk);
			novo_no->chave[0].rrn = nregistros;
			nregistros++;
		} else {
			node_Btree_is *novo_no = (node_Btree_is *)no;

			novo_no->folha = 'F';
			novo_no->num_chaves = 1;
			strcpy(novo_no->chave[0].pk, j.pk);
			strcpy(novo_no->chave[0].string, j.desenvolvedora);
			strcat(novo_no->chave[0].string, "$");
			strcat(novo_no->chave[0].string, j.nome);
		}

		ind->raiz = 0;

		//Escreve o registro no arquivo de índices primários e no arquivo de dados
		/* ESCREVE NO ARQUIVO DE DADOS */
		write_btree(no, (*nregs), ip);

		//Desaloca os dados que já foram utilizados
		free(no);

		(*nregs)++;
	}

	//Árvore não vazia
	else {
		//Aloca e compoem a chave a ser inserida de acordo com o tipo de estrutura usado
		void *chave_ins;
		if (ip == 'p'){
			chave_ins = malloc (sizeof(Chave_ip));
			prom_key = malloc (sizeof(Chave_ip));
			strcpy(((Chave_ip *)chave_ins)->pk, j.pk); ((Chave_ip *)chave_ins)->rrn = nregistros;
		}
		else{
			chave_ins = malloc (sizeof(Chave_is));
			prom_key = malloc (sizeof(Chave_is));
			strcpy(((Chave_is *)chave_ins)->pk, j.pk);
			strcpy(((Chave_is *)chave_ins)->string, j.desenvolvedora);
			strcat(((Chave_is *)chave_ins)->string, "$");
			strcat(((Chave_is *)chave_ins)->string, j.nome);
		}

		//Busca o nó folha onde a chave deve ser inserida, realizando operações de divisão de nós(split) se necessário
		no_criado = insere_aux(ind->raiz, chave_ins, &prom_key, ip);

		//Verifica se a propagação de split chegou até a raiz
		//Chave inserida já existe na árvore
		if (no_criado == -2) return -2;
		else if (prom_key != NULL) {
			no = criar_no(ip);

			if (ip == 'p') {
				node_Btree_ip *novo_no = (node_Btree_ip *)no;
				
				novo_no->folha = 'N';
				novo_no->num_chaves = 1;

				novo_no->chave[0] = *((Chave_ip *)prom_key);

				novo_no->desc[0] = ind->raiz;
				novo_no->desc[1] = no_criado;

				nregistros++;
			} else {
				node_Btree_is *novo_no = (node_Btree_is *)no;
				
				novo_no->folha = 'N';
				novo_no->num_chaves = 1;

				novo_no->chave[0] = *((Chave_is *)prom_key);

				novo_no->desc[0] = ind->raiz;
				novo_no->desc[1] = no_criado;
			}

			ind->raiz = (*nregs);

			//Escreve o registro no arquivo de índices primários e no arquivo de dados
			/* ESCREVE NO ARQUIVO DE DADOS */
			write_btree(no, (*nregs), ip);

			//Desaloca os dados que já foram utilizados
			free(no);
			free(prom_key);

			(*nregs)++;
		}
	}

	return 1;
}

//Função auxiliar que busca a primeira folha para inserção e cuida da propagação dos splits
int insere_aux(int rrn, void *chave_ins, void **prom_key, int ip) {
	//Recupera o nó raiz do arquivo de índices passado como parâmetro
	void *no = read_btree(rrn, ip);
	int i;

	if (ip == 'p') {

		//Verifica se é nó folha = CASO BASE DA RECURSÃO
		node_Btree_ip *no_rec = (node_Btree_ip *)no;
		if (no_rec->folha == 'F'){

			//Verifica se haverá divisão do nó (split)
			if (no_rec->num_chaves < ordem_ip-1) {
				Chave_ip *key = (Chave_ip *)chave_ins;

				//Busca o local onde inserir a chave
				i = no_rec->num_chaves-1;

				while (i >= 0 && strcmp(key->pk, no_rec->chave[i].pk) < 0) {
					no_rec->chave[i+1] = no_rec->chave[i];
					i--;
				}

				//Verifica se a pk já existia na árvore
				if(!strcmp(key->pk, no_rec->chave[i].pk)) return -2;

				//Realiza o posicionamento da chave
				no_rec->chave[i+1] = *(key);
				no_rec->num_chaves++;
				(*prom_key) = NULL;

				//Escreve no arquivo de índices
				write_btree(no, rrn, ip);

				nregistros++;

				return -1;
			} else {
				int ret = split(no, chave_ins, *prom_key, -1, ip);
				if (ret >= 0) write_btree(no, rrn, ip);
				return ret;
			}
		}

		//Caso não seja nó folha
		else {
			//Busca pelo rrn do filho a ser passado na recursão
			i = no_rec->num_chaves-1;

			Chave_ip *key = (Chave_ip *)chave_ins;
			while (i >= 0 && strcmp(key->pk, no_rec->chave[i].pk) < 0) i--;

			//Para o caso de a chave já estiver inserida neste nó
			if (strcmp(key->pk, no_rec->chave[i].pk) == 0) return -2;

			i++;
			int no_criado = insere_aux(no_rec->desc[i], chave_ins, prom_key, ip);

			//Para o caso de a chave já estiver inserida em um nó mais baixo na hierarquia
			if (no_criado == -2) return -2;

			//Verifica se houve propagação de split até o nível atual
			else if ((*prom_key) != NULL) {

				//Verifica se haverá divisão do nó (split)
				if (no_rec->num_chaves < ordem_ip-1) {

					//Busca o local onde inserir a chave
					i = no_rec->num_chaves-1;

					while (i >= 0 && strcmp(key->pk, no_rec->chave[i].pk) < 0) {
						no_rec->chave[i+1] = no_rec->chave[i];
						no_rec->desc[i+2] = no_rec->desc[i+1];
						i--;
					}

					//Realiza o posicionamento da chave
					no_rec->chave[i+1] = *((Chave_ip *)(*prom_key));
					no_rec->desc[i+2] = no_criado;
					no_rec->num_chaves++;
					(*prom_key) = NULL;

					//Escreve o nó no arquivo de índices
					write_btree(no_rec, rrn, ip);

					nregistros++;

					return -1;
				} else {
					int ret = split(no, *prom_key, *prom_key, no_criado, ip);
					if (ret >= 0) write_btree(no, rrn, ip);
					return ret;
				}
			} else {
				(*prom_key) = NULL;
				return -1;
			}
		}
	}
}

int split(void *no, void *chave_ins, void *prom_key, int rrn_no_criado, int ip) {
	/* SPLIT DE ÍNDICE PRIMÁRIO */
	if (ip == 'p') {
		node_Btree_ip *node = (node_Btree_ip *)no;
		Chave_ip *key = (Chave_ip *)chave_ins;

		int i, aux, flag_ins = 0;
		Chave_ip chaves[ordem_ip]; //Vetor auxiliar para realização do split
		int descendentes[ordem_ip+1]; //Vetor auxiliar para realização do split
		
		//Insere a chave passada no vetor auxiliar, assim como seu descendente
		for (i = ordem_ip-1; i > 0;) {
			aux = strcmp(key->pk, node->chave[i-1].pk);

			if (!aux) return -2; //Para o caso de a chave já estar inserida na árvore
			else if (!flag_ins && aux > 0) {
				flag_ins = 1;
				chaves[i] = *key;
				descendentes[i+1] = rrn_no_criado;
			} else if (!flag_ins) {
				chaves[i] = node->chave[i-1];
				descendentes[i+1] = node->desc[i];
				i--;
			} else {
				chaves[i-1] = node->chave[i-1];
				descendentes[i] = node->desc[i];
				i--;
			}
		}
		if (!flag_ins) {
			flag_ins = 1;
			chaves[i] = *key;
			descendentes[i+1] = rrn_no_criado;
		}
		descendentes[0] = node->desc[0];

		//Encontra a chave a ser promovida
		int pos_prom = ordem_ip/2;
		(*((Chave_ip *)prom_key)) = chaves[pos_prom];

		//Cria novo nó e atribui a ele as chaves e os respectivos descendentes
		node_Btree_ip *novo_no = (node_Btree_ip *) criar_no(ip);
		for (i = pos_prom+1; i < ordem_ip; i++) {
			novo_no->chave[i - (pos_prom+1)] = chaves[i];
			novo_no->desc[i - (pos_prom+1)] = descendentes[i];
		}
		novo_no->desc[i-(pos_prom+1)] = descendentes[ordem_ip];

		//Atribui os valores de nó folha e num_chaves
		novo_no->folha = node->folha;
		novo_no->num_chaves = ordem_ip-1 - pos_prom;

		//Escreve o novo nó no arquivo de índices primários
		write_btree(novo_no, nregistrosip, ip);
		nregistrosip++;

		//Atualiza o nó que sofreu o split
		node->num_chaves -= novo_no->num_chaves;
		for (i = 0; i < pos_prom; i++) {
			node->chave[i] = chaves[i];
			node->desc[i] = descendentes[i];
		}
		node->desc[i] = descendentes[i];

		for (i = pos_prom+1; i < ordem_ip; i++) node->desc[i] = -1; //Coloca descendentes passasdos para o nó criado como nulos

		return (nregistrosip-1);
	}
}
