#pragma once

#include <windows.h>
#include <tchar.h>
#include "resource.h"
#include "..\..\SnakeDLL\SnakeDLL\SnakeDLL.h"


/* ======================================================================= */
/*					ZONA DE DECLARAÇÃO DE VARIAVEIS E FUNÇÕES		       */
/* ======================================================================= */


/* -----------------------------------------------------*/
/*               #DEFINES LOCAIS                        */
/* -----------------------------------------------------*/
#define MAX				256
#define TAM				6
#define LIMITE_1POSI	1
#define LIMITE_2POSI	2
#define LIMITE_3POSI	3


/* ----------------------------------------------------- */
/*              VARIÁVEIS GLOBAIS						 */
/* ----------------------------------------------------- */

static int maxX;
static int maxY;

TCHAR *szProgName = TEXT("Snake");
TCHAR executavel[MAX] = TEXT("mspaint.exe");
//TCHAR executavel[MAX] = TEXT("%windir%\system32\mspaint.exe");

PROCESS_INFORMATION pi;
STARTUPINFO si;

int tipoServidor = 0;			//Se está ligado a um servidor local ou remoto

int emJogo = 0;							//Se está a jogar ou não

										//**** Variáveis de configuração do Jogo (começam com o valor por defeito)****
int cobrasAutomaticas = NUMAUTOSNAKE;
int colunasConfig = COLUNAS;
int linhasConfig = LINHAS;
int numJogadores = NUMJOGADORES;
int numObjectos = NUMOBJETOS;
int tamanhoSnakes = TAMANHOSNAKE;
int maxjogadores = MAXJOGADORES;
int minlinhas = MIN_LINHAS;
int mincolunas = MIN_COLUNAS;
int maxlinhas = MAX_LINHAS;
int maxcolunas = MAX_COLUNAS;
int maxobjectos = MAXOBJECTOS;


//**** Variáveis de configuração de Objectos (começam com o tempo de vida por defeito) ****
int alimento = PERMANENTE;
int gelo = PERMANENTE;
int granada = PERMANENTE;
int vodka = SEGUNDOSMAPA;
int oleo = SEGUNDOSMAPA;
int cola = SEGUNDOSMAPA;
int o_vodka = SEGUNDOSMAPA;
int o_oleo = SEGUNDOSMAPA;
int o_cola = SEGUNDOSMAPA;
int surpresa = SEGUNDOSMAPA;

//**** Variáveis de configuração do Jogo ****
int numJogadoresLocal = 0;				//Num Jogadores a jogar nesta maquina
int valorCobra1 = 0;				//valor da cobra do jogador 1 desta maquina
int valorCobra2 = 0;				//valor da cobra do jogador 2 desta maquina
TCHAR username1[SIZE_USERNAME];				//Nome do Jogador 1 desta Maquina
TCHAR username2[SIZE_USERNAME];				//Nome do Jogador 2 desta Maquina
int pId;									//Process Id deste cliente
int tId;									//Thread Id da thread principal deste cliente
int linhas;
int colunas;
int mapa[MAX_LINHAS][MAX_COLUNAS];

//**** Estrutura de teclas ****
typedef struct {
	int esquerda, direita, cima, baixo;
}teclas;

teclas teclasjogador1;
teclas teclasjogador2;

/* --------------------------------------------------------- */
/*				  PROTOTIPOS FUNÇÕES						 */
/* --------------------------------------------------------- */

int chamaCriaJogo(int *valor);
int chamaAssociaJogo(TCHAR username[SIZE_USERNAME], int codigo, int *valor);
void chamaMudaDirecao(int direcao, int jogador);
void desenhaMapaNaMemoria();


/* --------------------------------------------------------- */
/*				  PROTOTIPOS FUNÇÕES DE THREADS				 */
/* --------------------------------------------------------- */

DWORD WINAPI esperaActualizacao(LPVOID param);
DWORD WINAPI ligaServidorLocal(LPVOID param);
DWORD WINAPI criaJogo(LPVOID param);
DWORD WINAPI associaJogo(LPVOID param);
DWORD WINAPI iniciaJogo(LPVOID param);
DWORD WINAPI ligaServidorRemoto(LPVOID param);

/* --------------------------------------------------------- */
/*  PROTOTIPOS FUNÇÕES DE TRATAMENTO DE EVENTOS DE JANELAS	 */
/* --------------------------------------------------------- */

BOOL CALLBACK IndicaIPRemoto(HWND h, UINT m, WPARAM w, LPARAM l);
BOOL CALLBACK ConfiguraJogo(HWND h, UINT m, WPARAM w, LPARAM l);
BOOL CALLBACK ConfiguraObjectos(HWND h, UINT m, WPARAM w, LPARAM l);
BOOL CALLBACK Pede_NomeJogador1(HWND h, UINT m, WPARAM w, LPARAM l);
BOOL CALLBACK Pede_NomeJogador2(HWND h, UINT m, WPARAM w, LPARAM l);
BOOL CALLBACK CarregaBitmaps(HWND hWnd);
BOOL CALLBACK ConfigTeclas1(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ConfigTeclas2(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK TrataEventos(HWND, UINT, WPARAM, LPARAM);//JANELA PRINCIPAL


/* --------------------------------------------------------- */
/*                   HANDLES GLOBAIS                         */
/* --------------------------------------------------------- */

HINSTANCE	hInstGlobal;
HMENU		hMenu;
HWND		janelaglobal;

HBITMAP		hbit;

HANDLE teste;	//handle para testar se o cliente é unico na máquina

				//*** Outros Objectos ***
HDC	device;
HDC memoriajanela;
HDC hparede;
HDC hfundo;

//*** Objectos ***
HDC halimento;
HDC hgelo;
HDC hgranada;
HDC hvodka;
HDC holeo;
HDC hcola;
HDC ho_vodka;
HDC ho_oleo;
HDC ho_cola;
HDC surpresa_vida;
HDC hcomida;
HDC hmouse;
HDC hprenda;

//*** Cobra 1 ***
HDC hcobra1_cab1_esquerda, hcobra1_cab1_direita, hcobra1_cab1_cima, hcobra1_cab1_baixo;
HDC hcobra1_corpo1;
HDC hcobra1_corpo2;
HDC hcobra1_corpo3;
HDC hcobra1_corpo4;

//*** Cobra 2 ***
HDC hcobra2_cab2_esquerda, hcobra2_cab2_direita, hcobra2_cab2_cima, hcobra2_cab2_baixo;
HDC hcobra2_corpo1;
HDC hcobra2_corpo2;
HDC hcobra2_corpo3;
HDC hcobra2_corpo4;

//*** Cobra 3 ***
HDC hcobra3_cab3_esquerda, hcobra3_cab3_direita, hcobra3_cab3_cima, hcobra3_cab3_baixo;
HDC hcobra3_corpo1;
HDC hcobra3_corpo2;
HDC hcobra3_corpo3;
HDC hcobra3_corpo4;
