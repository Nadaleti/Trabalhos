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
int inserir (Indice *raiz, Jogo j, char ip, int rrn);
int insere_aux(int rrn, void *chave_ins, void **prom_key, int ip);
int split(void *no, void *chave_ins, void *prom_key, int rrn_no_criado, int ip);

/* FUNÇÕES PARA MANIPULAÇÃO DE ARQUIVO */

//Lê a entrada e salva os dados em uma struct Jogo
void ler_entrada (Jogo **titulo);

//Insere um registro no arquivo de dados
void insere_Arq (Jogo titulo);

//Dado um rrn, recupera um registro do arquivo de dados
Jogo recuperar_registro(int rrn);

/* FUNÇÕES AUXILIARES PARA LISTAGEM */
void listar_IP (int rrn, int nvl);
void listar_IS (int rrn);

/* FUNÇÃO AUXILIAR PARA BUSCA E ALTERAÇÃO */
int buscar_IP (Indice iprimary, char *pk);

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
	criar_iprimary(&iprimary);

	/* Índice secundário de nomes dos Jogos */
	Indice idev;
	criar_idev(&idev);

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
			printf(INICIO_ALTERACAO);
			if (alterar(iprimary))
				printf(SUCESSO);
			else
				printf(FALHA);
			break;
		case 3: /* Buscar um jogo */
			printf(INICIO_BUSCA);
			buscar(iprimary, idev);
			break;
		case 4: /* Listar todos os Jogos */
			printf(INICIO_LISTAGEM);
			listar(iprimary, idev);
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

	strncpy(categorias, j.categoria, strlen(j.categoria)+1);

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

void libera_no(void *node, char ip) {
	//Desaloca a estrutura o nó da estrutura passada como parâmetro
	if (ip == 'p') {
		free(((node_Btree_ip *)node)->chave);
		free(((node_Btree_ip *)node)->desc);
		free(node);
	} else if (ip == 's') {
		free(((node_Btree_is *)node)->chave);
		free(((node_Btree_is *)node)->desc);
		free(node);
	}
}

//Cria iprimary a partir de um arquivo de dados fornecido
void criar_iprimary(Indice *iprimary) {
	iprimary->raiz = -1;

	int i;
	Jogo j;

	for (i = 0; i < nregistros; i++) {
		j = recuperar_registro(i);
		inserir(iprimary, j, 'p', i);
	}
}

//Cria idev a partir de um arquivo de dados fornecido
void criar_idev(Indice *idev) {
	idev->raiz = -1;

	int i;
	Jogo j;

	for (i = 0; i < nregistros; i++) {
		j = recuperar_registro(i);
		inserir(idev, j, 's', i);
	}
}

/* FUNÇÕES PARA MANIPULAÇÃO DE ARQUIVO */
//Lê as entradas fornecidas pelo usuário e insere numa estrutura de jogo
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
void insere_Arq (Jogo titulo) {
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

//Função para escrita de nó no arquivo de índices
void write_btree(void *salvar, int rrn, char ip) {
	char *no_salvar, *aux;
	int i, qntd_sharps, tam_no;

	//Faz o cast para o tipo desejado e aloca o tamanho da string de acordo com o índice
	if (ip == 'p') {
		tam_no = tamanho_registro_ip;

		node_Btree_ip *saveIp = (node_Btree_ip *) salvar;
		no_salvar = (char *) malloc (tamanho_registro_ip + 1);

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
		no_salvar = (char *) malloc (tamanho_registro_is + 1);

		//Faz a transformação do nó em string
		sprintf (no_salvar, "%03d", saveIs->num_chaves); //Número de nós

		//Pk + com nome desenvolvedora concatenado com nome do jogo
		char string_aux[112]; string_aux[0] = '\0';
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
	if (inserir(iprimary, j, 'p', nregistros)) {
		insere_Arq(j); //Insere o registro j no arquivo de dados
		inserir(idev, j, 's', nregistros);
		nregistros++;
	} else {
		printf(ERRO_PK_REPETIDA, j.pk);
	}
}

//Função auxiliar para inserção
int inserir (Indice *ind, Jogo j, char ip, int rrn) {
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
			novo_no->chave[0].rrn = rrn;
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
		write_btree(no, (*nregs), ip);

		//Desaloca os dados que já foram utilizados
		libera_no(no, ip);

		(*nregs)++;
	}

	//Árvore não vazia
	else {
		//Aloca e compoem a chave a ser inserida de acordo com o tipo de estrutura usado
		void *chave_ins;
		if (ip == 'p'){
			Chave_ip chave, chave_prom;
			chave_ins = &chave;
			prom_key = &chave_prom;
			strcpy(((Chave_ip *)chave_ins)->pk, j.pk); ((Chave_ip *)chave_ins)->rrn = rrn;
		}
		else{
			Chave_is chave, chave_prom;
			chave_ins = &chave;
			prom_key = &chave_prom;
			strcpy(((Chave_is *)chave_ins)->pk, j.pk);
			strcpy(((Chave_is *)chave_ins)->string, j.desenvolvedora);
			strcat(((Chave_is *)chave_ins)->string, "$");
			strcat(((Chave_is *)chave_ins)->string, j.nome);
		}

		//Busca o nó folha onde a chave deve ser inserida, realizando operações de divisão de nós(split) se necessário
		no_criado = insere_aux(ind->raiz, chave_ins, &prom_key, ip);

		//Verifica se a propagação de split chegou até a raiz
		//Chave inserida já existe na árvore
		if (no_criado == -2) return 0;

		else if (prom_key != NULL) {
			no = criar_no(ip);

			if (ip == 'p') {
				node_Btree_ip *novo_no = (node_Btree_ip *)no;

				novo_no->folha = 'N';
				novo_no->num_chaves = 1;

				novo_no->chave[0] = *((Chave_ip *)prom_key);

				novo_no->desc[0] = ind->raiz;
				novo_no->desc[1] = no_criado;
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
			write_btree(no, (*nregs), ip);

			//Desaloca os dados que já foram utilizados
			libera_no(no, ip);

			(*nregs)++;
		}
	}

	return 1;
}

//Função auxiliar que busca a primeira folha para inserção e cuida da propagação dos splits
int insere_aux(int rrn, void *chave_ins, void **prom_key, int ip) {
	//Recupera o nó raiz do arquivo de índices passado como parâmetro
	void *no = read_btree(rrn, ip);

	//Variáveis auxiliares
	int i, num_chaves, ordem;
	char flag_folha;
	node_Btree_ip *no_rec_ip = (node_Btree_ip *)no;
	node_Btree_is *no_rec_is = (node_Btree_is *)no;

	//Atribui valores para as variáveis auxiliares de acordo com a estrutura utilizada
	if (ip == 'p') {
		ordem = ordem_ip;
		num_chaves = no_rec_ip->num_chaves;
		flag_folha = no_rec_ip->folha;
	} else if (ip == 's') {
		ordem = ordem_is;
		num_chaves = no_rec_is->num_chaves;
		flag_folha = no_rec_is->folha;
	}

	//Verifica se é nó folha = CASO BASE DA RECURSÃO
	if (flag_folha == 'F'){

		//Verifica se haverá divisão do nó (split)
		if (num_chaves < ordem-1) {

			//Busca o local onde inserir a chave
			i = num_chaves-1;

			if (ip == 'p') {
				while (i >= 0 && strcmp(((Chave_ip *)chave_ins)->pk, no_rec_ip->chave[i].pk) < 0) {
					no_rec_ip->chave[i+1] = no_rec_ip->chave[i];
					i--;
				}

				//Verifica se a pk já existia na árvore
				if(i >= 0 && !strcmp(((Chave_ip *)chave_ins)->pk, no_rec_ip->chave[i].pk)) return -2;

				//Realiza o posicionamento da chave
				no_rec_ip->chave[i+1] = *((Chave_ip *)chave_ins);
				no_rec_ip->num_chaves++;
			} else if (ip == 's') {
				while (i >= 0 && strcmp(((Chave_is *)chave_ins)->string, no_rec_is->chave[i].string) < 0) {
					no_rec_is->chave[i+1] = no_rec_is->chave[i];
					i--;
				}

				//Realiza o posicionamento da chave
				no_rec_is->chave[i+1] = *((Chave_is *)chave_ins);
				no_rec_is->num_chaves++;
			}

			(*prom_key) = NULL;

			//Escreve no arquivo de índices
			write_btree(no, rrn, ip);

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
		i = num_chaves-1;
		int desc;

		if (ip == 'p') {
			while (i >= 0 && strcmp(((Chave_ip *)chave_ins)->pk, no_rec_ip->chave[i].pk) < 0) i--;

			//Para o caso de a chave já estiver inserida neste nó
			if (strcmp(((Chave_ip *)chave_ins)->pk, no_rec_ip->chave[i].pk) == 0) return -2;

			desc = no_rec_ip->desc[i+1];
		} else if (ip == 's') {
			while (i >= 0 && strcmp(((Chave_is *)chave_ins)->string, no_rec_is->chave[i].string) < 0) i--;

			desc = no_rec_is->desc[i+1];
		}

		int no_criado = insere_aux(desc, chave_ins, prom_key, ip);

		//Para o caso de a chave já estiver inserida em um nó mais baixo na hierarquia
		if (no_criado == -2) return -2;

		//Verifica se houve propagação de split até o nível atual
		else if ((*prom_key) != NULL) {

			//Verifica se haverá divisão do nó (split)
			if (num_chaves < ordem-1) {

				//Busca o local onde inserir a chave
				i = num_chaves-1;

				if (ip == 'p') {
					while (i >= 0 && strcmp(((Chave_ip *)(*prom_key))->pk, no_rec_ip->chave[i].pk) < 0) {
						no_rec_ip->chave[i+1] = no_rec_ip->chave[i];
						no_rec_ip->desc[i+2] = no_rec_ip->desc[i+1];
						i--;
					}

					//Realiza o posicionamento da chave
					no_rec_ip->chave[i+1] = *((Chave_ip *)(*prom_key));
					no_rec_ip->desc[i+2] = no_criado;
					no_rec_ip->num_chaves++;
				} else if (ip == 's') {
					while (i >= 0 && strcmp(((Chave_is *)(*prom_key))->string, no_rec_is->chave[i].string) < 0) {
						no_rec_is->chave[i+1] = no_rec_is->chave[i];
						no_rec_is->desc[i+2] = no_rec_is->desc[i+1];
						i--;
					}

					//Realiza o posicionamento da chave
					no_rec_is->chave[i+1] = *((Chave_is *)(*prom_key));
					no_rec_is->desc[i+2] = no_criado;
					no_rec_is->num_chaves++;
				}

				(*prom_key) = NULL;

				//Escreve o nó no arquivo de índices
				write_btree(no, rrn, ip);

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

int split(void *no, void *chave_ins, void *prom_key, int rrn_no_criado, int ip) {
	int i, aux, flag_ins = 0;

	/* SPLIT DE ÍNDICE PRIMÁRIO */
	if (ip == 'p') {
		node_Btree_ip *node = (node_Btree_ip *)no;
		Chave_ip *key = (Chave_ip *)chave_ins;

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

		//Para o caso de a chave inserida ser a primeira do vetor
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

	/* SPLIT DE ÍNDICE SECUNDÁRIO */
	else if (ip == 's') {
		node_Btree_is *node = (node_Btree_is *)no;
		Chave_is *key = (Chave_is *)chave_ins;

		Chave_is chaves[ordem_is]; //Vetor auxiliar para realização do split
		int descendentes[ordem_is+1]; //Vetor auxiliar para realização do split

		//Insere a chave passada no vetor auxiliar, assim como seu descendente
		for (i = ordem_is-1; i > 0;) {
			aux = strcmp(key->string, node->chave[i-1].string);

			if (!flag_ins && aux >= 0) {
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

		//Para o caso de a chave inserida ser a primeira do vetor
		if (!flag_ins) {
			flag_ins = 1;
			chaves[i] = *key;
			descendentes[i+1] = rrn_no_criado;
		}
		descendentes[0] = node->desc[0];

		//Encontra a chave a ser promovida
		int pos_prom = ordem_is/2;
		(*((Chave_is *)prom_key)) = chaves[pos_prom];

		//Cria novo nó e atribui a ele as chaves e os respectivos descendentes
		node_Btree_is *novo_no = (node_Btree_is *) criar_no(ip);
		for (i = pos_prom+1; i < ordem_is; i++) {
			novo_no->chave[i - (pos_prom+1)] = chaves[i];
			novo_no->desc[i - (pos_prom+1)] = descendentes[i];
		}
		novo_no->desc[i-(pos_prom+1)] = descendentes[ordem_is];

		//Atribui os valores de nó folha e num_chaves
		novo_no->folha = node->folha;
		novo_no->num_chaves = ordem_is-1 - pos_prom;

		//Escreve o novo nó no arquivo de índices secundários
		write_btree(novo_no, nregistrosis, ip);
		nregistrosis++;

		//Atualiza o nó que sofreu o split
		node->num_chaves -= novo_no->num_chaves;
		for (i = 0; i < pos_prom; i++) {
			node->chave[i] = chaves[i];
			node->desc[i] = descendentes[i];
		}
		node->desc[i] = descendentes[i];

		for (i = pos_prom+1; i < ordem_is; i++) node->desc[i] = -1; //Coloca descendentes passasdos para o nó criado como nulos

		return (nregistrosis-1);
	}
}

/* ALTERAÇÃO -> MUDAR PARA O CONTEXTO ATUAL */
int alterar(Indice iprimary) {
 	char pk[TAM_PRIMARY_KEY], descontoStr[5];

	scanf("%[^\n]", pk); getchar();

	//Realiza a busca na árvore de índices primários
	int rrn = buscar_IP(iprimary, pk);

	if (rrn >= 0) {

		//Recebe o desconto desejado e verifica o valor está dentro dos limites estabelecidos
		scanf("%[^\n]", descontoStr); getchar();
		int tam = strlen (descontoStr);
		int desconto;
		sscanf(descontoStr, "%d", &desconto);
		while (tam < 3 || tam > 3 || isalpha(descontoStr[0]) || isalpha(descontoStr[1]) || isalpha(descontoStr[2]) || (desconto < 0 || desconto > 100)) {
			printf(CAMPO_INVALIDO);
			scanf("%[^\n]", descontoStr); getchar();
			tam = strlen (descontoStr);
			sscanf(descontoStr, "%d", &desconto);
		}

		//Altera o valor de desconto no arquivo de dados
		char *arq = ARQUIVO;
		int cont_arroba = 0, i = rrn * 192;

		while (cont_arroba != 6) if (arq[i++] == '@') cont_arroba++;
		sprintf(&arq[i], "%03d", desconto); arq[i+3] = '@';

		return 1;
	}

	printf(REGISTRO_N_ENCONTRADO);
	return 0;
 }

/* BUSCA */
void buscar(Indice iprimary,Indice idev) {
	//Recebe o tipo de busca a ser feito
	int tipo, i, found = 1, rrn_no = -1;
	char aux[TAM_STRING_INDICE];

	scanf("%d", &tipo); getchar();
	scanf("%[^\n]\n", aux);

	/* BUSCA POR CHAVE PRIMÁRIA */
	if (tipo == 1) {
		printf(NOS_PERCORRIDOS_IP, aux);

		if (iprimary.raiz == -1) {
			printf("\n");
			printf(REGISTRO_N_ENCONTRADO);
			return;
		}
		node_Btree_ip no_rec_ip = *(node_Btree_ip *)read_btree(iprimary.raiz, 'p');

		//Busca a chave na árvore
		while (found && found != -1) {

			//Sinaliza caso seja nó folha
			if (no_rec_ip.folha == 'F') found = -1;

			//Busca no nó atual e, se não encontrar, vai para o próximo nó
			for (i = 0; i < no_rec_ip.num_chaves; i++) {

				printf("%s", no_rec_ip.chave[i].pk);

				//Não foi encontrada a chave nem o rrn do próximo nó
				if (rrn_no == -1)
					//Verifica se a chave atual é igual à pk buscada
					if (!strcmp(aux, no_rec_ip.chave[i].pk)) {
						found = 0;
						rrn_no = i;
					}
					//Se o nó não for folha, verifica se a chave atual é maior do que a pk buscada.
					//Caso isso ocorra, o próximo nó a ser percorrido é encontrado
					else if
						(found != -1 && strcmp(aux, no_rec_ip.chave[i].pk) <= 0) rrn_no = i;

				if (i != no_rec_ip.num_chaves-1) printf(", ");
			}
			printf("\n");

			//Recupera próximo nó se não é folha e se a chave não foi encontrada
			if (found && found != -1) {
				if (rrn_no == -1) rrn_no = i; //Caso a chave buscada seja maior do que todas as chaves do nó
				no_rec_ip = *(node_Btree_ip *)read_btree(no_rec_ip.desc[rrn_no], 'p');
				rrn_no = -1;
			}
		}
		printf("\n");

		//Verifica se o nó foi encontrado ou não
		if (found == -1 && rrn_no == -1) printf(REGISTRO_N_ENCONTRADO);
		else exibir_registro(no_rec_ip.chave[rrn_no].rrn);
	}

	/* BUSCA POR CHAVE SECUNDÁRIA */
	else if (tipo == 2) {
		char aux_nome[TAM_NOME];

		//Recebe o nome do jogo
		scanf("%[^\n]", aux_nome);
		strcat(aux, "$");
		strcat(aux, aux_nome);

		printf(NOS_PERCORRIDOS_IS, aux);

		//Verifica se o arquivo é vazio
		if (idev.raiz == -1) {
			printf("\n");
			printf(REGISTRO_N_ENCONTRADO);
			return;
		}

		node_Btree_is no_rec_is = *(node_Btree_is *)read_btree(idev.raiz, 's');

		//Busca a chave na árvore
		while (found && found != -1) {

			//Sinaliza caso seja nó folha
			if (no_rec_is.folha == 'F') found = -1;

			//Busca no nó atual e, se não encontrar, vai para o próximo nó
			for (i = 0; i < no_rec_is.num_chaves; i++) {

				printf("%s", no_rec_is.chave[i].string);

				//Não foi encontrada a chave nem o rrn do próximo nó
				if (rrn_no == -1)
					//Verifica se a chave atual é igual à pk buscada
					if (!strcmp(aux, no_rec_is.chave[i].string)) {
						found = 0;
						rrn_no = i;
					}
					//Se o nó não for folha, verifica se a chave atual é maior do que a string buscada.
					//Caso isso ocorra, o próximo nó a ser percorrido é encontrado
					else if
						(found != -1 && strcmp(aux, no_rec_is.chave[i].string) < 0) rrn_no = i;

				if (i != no_rec_is.num_chaves-1) printf(", ");
			}
			printf("\n");

			//Recupera próximo nó se não é folha e se a chave não foi encontrada
			if (found && found != -1) {
				if (rrn_no == -1) rrn_no = i; //Caso a chave buscada seja maior do que todas as chaves do nó
				no_rec_is = *(node_Btree_is *)read_btree(no_rec_is.desc[rrn_no], 's');
				rrn_no = -1;
			}
		}
		printf("\n");

		//Verifica se o nó foi encontrado ou não
		if (found == -1 && rrn_no == -1) printf(REGISTRO_N_ENCONTRADO);
		else exibir_registro(buscar_IP(iprimary, no_rec_is.chave[rrn_no].pk));
	}
}

//Busca na árvore de índices primários
int buscar_IP (Indice iprimary, char *pk) {
	int i, found = 1, rrn_no = -1;

	node_Btree_ip no_rec_ip = *(node_Btree_ip *)read_btree(iprimary.raiz, 'p');

	//Busca a chave na árvore
	while (found && found != -1) {

		//Verifica se é folha
		if (no_rec_ip.folha == 'F') found = -1;

		//Busca no nó atual e, se não encontrar, vai para o próximo nó
		for (i = 0; i < no_rec_ip.num_chaves; i++) {
			if (rrn_no == -1)
				if (!strcmp(pk, no_rec_ip.chave[i].pk)) {
					found = 0;
					rrn_no = i;
				} else if (found != -1 && strcmp(pk, no_rec_ip.chave[i].pk) <= 0) rrn_no = i;
		}

		//Recupera próximo nó se não é folha e se a chave não foi encontrada
		if (found && found != -1) {
			if (rrn_no == -1) rrn_no = i;
			no_rec_ip = *(node_Btree_ip *)read_btree(no_rec_ip.desc[rrn_no], 'p');
			rrn_no = -1;
		}
	}

	//Verifica se o nó foi encontrado ou não
	return no_rec_ip.chave[rrn_no].rrn;
}

/* LISTAGEM */
 void listar(Indice iprimary,Indice idev) {
 	int tipo;

 	scanf("%d", &tipo); getchar();

 	if (tipo == 1) {
 		if (iprimary.raiz == -1) printf(REGISTRO_N_ENCONTRADO);
 		else listar_IP(iprimary.raiz, 1);
 	}
 	else if (tipo == 2) {
 		if (idev.raiz == -1) printf(REGISTRO_N_ENCONTRADO);
 		else listar_IS (idev.raiz);
 	}
 }

//Listagem de índice primário
void listar_IP (int rrn, int nvl) {
	//Recupera o nó com o rrn passado
	node_Btree_ip no_rec_ip = *(node_Btree_ip *)read_btree(rrn, 'p');

	//Imprime o nó atual
	printf("%d - ", nvl);

	int i;
	for (i = 0; i < no_rec_ip.num_chaves; i++) {
		printf("%s", no_rec_ip.chave[i].pk);
		if (i != no_rec_ip.num_chaves-1) printf(", ");
	}
	printf("\n");

	//Realiza chamadas recursivas para cada nó filho
	for (i = 0; no_rec_ip.folha != 'F' && i <= no_rec_ip.num_chaves; i++)
		listar_IP(no_rec_ip.desc[i], nvl+1);
}

//Listagem por índice secundário
void listar_IS (int rrn) {
	//Recupera o nó com o rrn passado
	node_Btree_is no_rec_is = *(node_Btree_is *)read_btree(rrn, 's');

	char *aux = (char *) malloc (TAM_STRING_INDICE);

	int i, qntd_tracos = 0;
	for (i = 0; i < no_rec_is.num_chaves; i++) {
		//Chamada recursiva para o filho a esquerda da chave[i]
		if (no_rec_is.folha != 'F') listar_IS(no_rec_is.desc[i]);

		/* IMPRESSÃO DA DEV E NOME DO JOGO DA CHAVE[i] */
		strcpy(aux, no_rec_is.chave[i].string);
		qntd_tracos = printf("%s", strtok(aux, "$"));

		//Imprime os -
		qntd_tracos = 50 - qntd_tracos;
		while (qntd_tracos--) printf("-");

		qntd_tracos = printf(" %s", strtok(NULL, "$"));

		//Imprime os -
		qntd_tracos = 50 - qntd_tracos + 1;
		while (qntd_tracos--) printf("-");
		printf("\n");
	}
	if (no_rec_is.folha != 'F') listar_IS(no_rec_is.desc[i]);
}
