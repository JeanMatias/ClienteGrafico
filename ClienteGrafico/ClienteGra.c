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

int tipoServidor		=	0;			//Se está ligado a um servidor local ou remoto

//**** Variáveis de configuração do Jogo (começam com o valor por defeito)****
int cobrasAutomaticas	=	NUMAUTOSNAKE;
int colunasConfig		=	COLUNAS;
int linhasConfig		=	LINHAS;
int numJogadores		=	NUMJOGADORES;
int numObjectos			=	NUMOBJETOS;
int tamanhoSnakes		=	TAMANHOSNAKE;
int maxjogadores		=	MAXJOGADORES;
int minlinhas			=	MIN_LINHAS;
int mincolunas			=	MIN_COLUNAS;
int maxlinhas			=	MAX_LINHAS;
int maxcolunas			=	MAX_COLUNAS;
int maxobjectos			=	MAXOBJECTOS;


//**** Variáveis de configuração de Objectos (começam com o tempo de vida por defeito) ****
int alimento			=	PERMANENTE;
int gelo				=	PERMANENTE;
int granada				=	PERMANENTE;
int vodka				=	SEGUNDOSMAPA;
int oleo				=	SEGUNDOSMAPA;
int cola				=	SEGUNDOSMAPA;
int o_vodka				=	SEGUNDOSMAPA;
int o_oleo				=	SEGUNDOSMAPA;
int o_cola				=	SEGUNDOSMAPA;
int surpresa			=	SEGUNDOSMAPA;

//**** Variáveis de configuração do Jogo ****
int numJogadoresLocal	=	0;				//Num Jogadores a jogar nesta maquina
int valorCobra1			=	0;				//valor da cobra do jogador 1 desta maquina
int valorCobra2			=	0;				//valor da cobra do jogador 2 desta maquina
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
void desenhaMapaNaMemoria();

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
DWORD WINAPI esperaActualizacao(LPVOID param);

LRESULT CALLBACK TrataEventos(HWND, UINT, WPARAM, LPARAM);


/* --------------------------------------------------------- */
/*                   HANDLES GLOBAIS                         */
/* --------------------------------------------------------- */

HINSTANCE	hInstGlobal;
HMENU		hMenu;
HWND		janelaglobal;

HBITMAP		hbit;
HWND		hWndRButton1;		// Handles para os radio buttons (Teclas default)
HWND		hWndRbutton2;		//								 (Configurar Teclas)
HANDLE		teste;				//handle para testar se o cliente é unico na máquina

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

/* ============================================================================ */
/*					Programa base (esqueleto) para aplicações Windows			*/
/*																				*/																	
/*					FUNÇÃO DE INÍCIO DO PROGRAMA: WinMain()						*/
/* ============================================================================ */
				
				// ============================================================================ //
				// WinMain()																	//
				// NOTA: Ver declaração de HACCEL, LoadAccelerators() e TranslateAccelerator()  //
				//		 Estas são as modificações necessárias para tratar as teclas de atalho  //
				//		 para opções do menu													//
				// ============================================================================	//

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {

	HWND hWnd;													// hWnd é o handler da janela, gerado mais abaixo por CreateWindow()
	MSG lpMsg;													// MSG é uma estrutura definida no Windows para as mensagens
	WNDCLASSEX wcApp;											// WNDCLASSEX é uma estrutura cujos membros servem para 
																// definir as características da classe da janela
	BOOL ret;
	HACCEL hAccel;												// Handler da resource accelerators (teclas de atalho

	hInstGlobal = hInst;

	pId = GetCurrentProcessId();
	tId = GetCurrentThreadId();


	/* ---- Definição Pipes ---- */

																//    Definição das características da janela "wcApp" 
																//    (Valores dos elementos da estrutura "wcApp" do tipo WNDCLASSEX)

	wcApp.cbSize = sizeof(WNDCLASSEX);							// Tamanho da estrutura WNDCLASSEX
	wcApp.hInstance = hInst;									// Instância da janela actualmente exibida 
																// ("hInst" é parâmetro de WinMain e vem 
																// inicializada daí)
	wcApp.lpszClassName = szProgName;							// Nome da janela (neste caso = nome do programa)
	wcApp.lpfnWndProc = TrataEventos;							// Endereço da função de processamento da janela 	              
	wcApp.style = CS_HREDRAW | CS_VREDRAW;						// Estilo da janela: Fazer o redraw se for modificada horizontal ou verticalmente
																//wcApp.hIcon = LoadIcon(NULL, IDI_HAND);// "hIcon" = handler do ícon normal
	wcApp.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDC_ICON));
																// Icon Grande		IDI_APPLICATION		"NULL" = Icon definido no Windows
																// "IDI_AP..." Ícone "aplicação"
	wcApp.hIconSm = LoadIcon(hInst, IDI_ICON4);					// "hIconSm" = handler do ícon pequeno      
																// Icon Small	IDI_INFORMATION	"NULL" = Icon definido no Windows
																// "IDI_INF..." Ícon de informação
	wcApp.hCursor = LoadCursor(NULL, IDC_ARROW);				// "hCursor" = handler do cursor (rato) 
																// IDC_ARROW "NULL" = Forma definida no Windows
																// "IDC_ARROW" Aspecto "seta" 
	wcApp.lpszMenuName = (LPCTSTR)(IDR_MENU1);
	//wcApp.lpszMenuName = NULL;								// Classe do menu que a janela pode ter
																// (NULL = não tem menu)
	wcApp.cbClsExtra = 0;										// Livre, para uso particular
	wcApp.cbWndExtra = 0;										// Livre, para uso particular
	wcApp.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// LTGRAY_BRUSH
																// "hbrBackground" = handler para "brush" de pintura do fundo da janela. Devolvido por  // "GetStockObject".Neste caso o fundo será branco
	
	/* --------------------------------------------------------- */
	/*		   Registar a classe "wcApp" no Windows	             */
	/* --------------------------------------------------------- */
	
	if (!RegisterClassEx(&wcApp))
		return(0);

	/* --------------------------------------------------------- */
	/*			Criar a janela                                   */
	/* --------------------------------------------------------- */
	
	hWnd = CreateWindow(
		szProgName,												// Nome da janela (programa) definido acima
		TEXT("Snake Multiplayer"),								// Texto que figura na barra do título
																// Adicinar Scrool Bar se necessário	WS_VSCROLL |			
		WS_OVERLAPPEDWINDOW,									// Estilo da janela (WS_OVERLAPPED= normal)
		50,														//CW_USEDEFAULT,
																// Posição x pixels (default=à direita da última)
		50,														// Posição y pixels (default=abaixo da última)
																//CW_USEDEFAULT,		
		1200, 													// Largura da janela (em pixels)
																//CW_USEDEFAULT,		
		600, 													// Altura da janela (em pixels)
																//CW_USEDEFAULT,		
		(HWND)HWND_DESKTOP,										// handle da janela pai (se se criar uma a partir de
																// outra) ou HWND_DESKTOP se a janela for a primeira, 
																// criada a partir do "desktop"
		(HMENU)hMenu,											// handle do menu da janela (se tiver menu)
		(HINSTANCE)hInst,										// handle da instância do programa actual ("hInst" é 
																// passado num dos parâmetros de WinMain()
		0);														// Não há parâmetros adicionais para a janela

	/* ---------------------------------------------------------------------------- */
	/* Carregar as definições das teclas aceleradoras (atalhos de opções dos Menus) */
	/* LoadAccelerators(instância_programa, ID_da_resource_atalhos)				    */
	/* ---------------------------------------------------------------------------- */

	hAccel = LoadAccelerators(hInst, (LPCWSTR)IDR_ACCELERATOR1);

	/* ---------------------------------------------------------------------------- */
	/*							Mostrar a janela									*/
	/* ---------------------------------------------------------------------------- */

	ShowWindow(hWnd, nCmdShow);									// "hWnd"= handler da janela, devolvido por 
																// "CreateWindow"; "nCmdShow"= modo de exibição (p.e. 
																// normal/modal); é passado como parâmetro de WinMain()
	UpdateWindow(hWnd);											// Refrescar a janela (Windows envia à janela uma 
																// mensagem para pintar, mostrar dados, (refrescar)… 
										// -----------------------------------------------------------------------------
										//				Loop de Mensagens
										// -----------------------------------------------------------------------------
										// O Windows envia mensagens às janelas (programas). Estas mensagens ficam numa fila de
										// espera até que GetMessage(...) possa ler "a mensagem seguinte"	
										// Parâmetros de "getMessage":
										// 1)"&lpMsg"=Endereço de uma estrutura do tipo MSG ("MSG lpMsg" ja foi declarada no  
										//   início de WinMain()):
										//			HWND hwnd		handler da janela a que se destina a mensagem
										//			UINT message		Identificador da mensagem
										//			WPARAM wParam		Parâmetro, p.e. código da tecla premida
										//			LPARAM lParam		Parâmetro, p.e. se ALT também estava premida
										//			DWORD time		Hora a que a mensagem foi enviada pelo Windows
										//			POINT pt		Localização do mouse (x, y) 
										// 2)handle da window para a qual se pretendem receber mensagens (=NULL se se pretendem //   receber as mensagens para todas as janelas pertencentes à thread actual)
										// 3)Código limite inferior das mensagens que se pretendem receber
										// 4)Código limite superior das mensagens que se pretendem receber

										// NOTA: GetMessage() devolve 0 quando for recebida a mensagem de fecho da janela,
										// 	  terminando então o loop de recepção de mensagens, e o programa 

							// ### ALERTA ===>>>>>
							// ### ESTA CONSTANTEMENTE A RECEBER OS EVENTOS ***************************************************************
	
							/* ============================================================================ */
							/* Loop de Mensagens															*/	
							/* Para usar as teclas aceleradoras do menu é necessário chamar a função		*/
							/* TranslateAccelerator(handler_janela, handler_accelerators, pont_mensagem)	*/
							/* Se esta função devolver falso, não foi premida tecla de aceleração			*/
							/* ============================================================================ */

	while ((ret = GetMessage(&lpMsg, NULL, 0, 0)) != 0) {
		if (ret != -1) {
			if (!TranslateAccelerator(hWnd, hAccel, &lpMsg)) {
				TranslateMessage(&lpMsg);							// Pré-processamento da mensagem (p.e. obter código 
																	// ASCII da tecla premida)
				DispatchMessage(&lpMsg);							// Enviar a mensagem traduzida de volta ao Windows, que
																	// aguarda até que a possa reenviar à função de 
																	// tratamento da janela, CALLBACK TrataEventos (abaixo)
			}
		}
	}

	/* ---------------------------------------------------------------------------- */
	/*							Fim do programa										*/
	/* ---------------------------------------------------------------------------- */

	return((int)lpMsg.wParam);										// Retorna sempre o parâmetro wParam da estrutura lpMsg
}
																	// ============================================================================
																	// FUNÇÃO DE PROCESSAMENTO DA JANELA
																	// Esta função pode ter um nome qualquer: Apenas é necesário que na inicialização da     // estrutura "wcApp", feita no início de WinMain(), se identifique essa função. Neste   // caso "wcApp.lpfnWndProc = WndProc"
																	//
																	// WndProc recebe as mensagens enviadas pelo Windows (depois de lidas e pré-processadas // no loop "while" da função WinMain()
																	//
																	// Parâmetros:
																	//		hWnd	O handler da janela, obtido no CreateWindow()
																	//		messg	Ponteiro para a estrutura mensagem (ver estrutura em 5. Loop...
																	//		wParam	O parâmetro wParam da estrutura messg (a mensagem)
																	//		lParam	O parâmetro lParam desta mesma estrutura
																	//
																	// NOTA:Estes parâmetros estão aqui acessíveis o que simplifica o acesso aos seus valores
																	//
																	// A função EndProc é sempre do tipo "switch..." com "cases" que descriminam a mensagem // recebida e a tratar. Estas mensagens são identificadas por constantes (p.e. 
																	// WM_DESTROY, WM_CHAR, WM_KEYDOWN, WM_PAINT...) definidas em windows.h ============================================================================


/* ============================================================================ */
/*								FUNÇÕES CALLBACK   								*/
/* ============================================================================ */

/* ----------------------------------------------------- */
/*				FUNÇÃO MENU PARA IP DO REMOTO			 */
/* ----------------------------------------------------- */

BOOL CALLBACK IndicaIPRemoto(HWND h, UINT m, WPARAM w, LPARAM l) {
	switch (m) {
	case WM_COMMAND:
		switch (LOWORD(w))
		{
		case IDCANCEL:
			EndDialog(h, 0);
			//return 1;
		case IDOK:
			
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(h, 0);
		return 1;

	case WM_INITDIALOG:
	
		return 1;
	}
	//Não foi tratado o evento
	return 0;
}

/* ----------------------------------------------------- */
/*				FUNÇÃO MENU PARA CONFIGURAR JOGO		 */
/* ----------------------------------------------------- */

BOOL CALLBACK ConfiguraJogo(HWND h, UINT m, WPARAM w, LPARAM l) {
	int aux1, aux2, aux3, aux4, aux5, aux6;
	switch (m) {
	case WM_COMMAND:
		switch (LOWORD(w))
		{
		case IDCANCEL:
			EndDialog(h, 0);
			//return 1;
		case IDOK:
			aux1 = GetDlgItemInt(h, IDC_EDIT1, NULL, TRUE);
			aux2 = GetDlgItemInt(h, IDC_EDIT2, NULL, TRUE);
				if (aux1 > MAX_LINHAS || aux1 < MIN_LINHAS || aux2 > MAX_COLUNAS || aux2 < MIN_COLUNAS) {
					MessageBox(h, TEXT("LINHAS: Máximo 40 E Minimo 10 \n\nCOLUNAS: Máximo 80 E Minimo 10"), TEXT("ATENÇÂO"), MB_OK);
				}
				else {
					colunasConfig = aux1;
					linhasConfig = aux2;
				}
			aux3 = GetDlgItemInt(h, IDC_EDIT3, NULL, TRUE);
				if (aux3 < TAMANHOSNAKE || aux3 > 6)
					MessageBox(h, TEXT("Tamanho Configurável das Cobras: Compreendido entre 3 e 6"), TEXT("ATENÇÂO"), MB_OK);
				else{
					tamanhoSnakes = aux3;			
				}
			aux4 = GetDlgItemInt(h, IDC_EDIT4, NULL, TRUE);
				if (aux4 < NUMAUTOSNAKE || aux4 > 6)
					MessageBox(h, TEXT("Número Cobras Automáticas: Compreendido entre 1 e 6"), TEXT("ATENÇÃO"), MB_OK);
				else {
					cobrasAutomaticas = aux4;
				}
			aux5 = GetDlgItemInt(h, IDC_EDIT5, NULL, TRUE);
				if (aux5 > MAXOBJECTOS)
					MessageBox(h, TEXT("Número Máximo de Objectos: Limite é 30"), TEXT("ATENÇÃO"), MB_OK);
				else {
					numObjectos = aux5;
				}
			aux6 = GetDlgItemInt(h, IDC_EDIT6, NULL, TRUE);
				if (aux6 < NUMJOGADORES || aux6 > MAXJOGADORES)
					MessageBox(h, TEXT("Número de Jogadores: Compreendido entre 1 e 4"), TEXT("ATENÇÃO"), MB_OK);
				else {
					numJogadores = aux6;
				}
				//EndDialog(h, 0);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(h, 0);
		return 1;
	case WM_INITDIALOG:
		SendDlgItemMessage(h, IDC_EDIT1, EM_LIMITTEXT, LIMITE_2POSI, NULL);
			SetDlgItemInt(h, IDC_EDIT1, LINHAS, TRUE);							// Tamanho X do tabuleiro
		SendDlgItemMessage(h, IDC_EDIT2, EM_LIMITTEXT, LIMITE_2POSI, NULL);	
			SetDlgItemInt(h, IDC_EDIT2, COLUNAS, TRUE);							// Tamanho Y do tabuleiro
		SendDlgItemMessage(h, IDC_EDIT3, EM_LIMITTEXT, LIMITE_1POSI, NULL);
			SetDlgItemInt(h, IDC_EDIT3, TAMANHOSNAKE, TRUE);					// Tamanho das cobras
		SendDlgItemMessage(h, IDC_EDIT4, EM_LIMITTEXT, LIMITE_1POSI, NULL);
			SetDlgItemInt(h, IDC_EDIT4, NUMAUTOSNAKE, TRUE);					// Nº Cobras Automáticas
		SendDlgItemMessage(h, IDC_EDIT5, EM_LIMITTEXT, LIMITE_2POSI, NULL);
			SetDlgItemInt(h, IDC_EDIT5, NUMOBJETOS, TRUE);						// Nº de objectos
		SendDlgItemMessage(h, IDC_EDIT6, EM_LIMITTEXT, LIMITE_1POSI, NULL);
			SetDlgItemInt(h, IDC_EDIT6, NUMJOGADORES, TRUE);					// Nº Max Jogadores
		return 1;		
	}
	//Não foi tratado o evento
	return 0;
}

/* ----------------------------------------------------- */
/*				FUNÇÃO MENU PARA CONFIGURAR OBJECTOS	 */
/* ----------------------------------------------------- */

BOOL CALLBACK ConfiguraObjectos(HWND h, UINT m, WPARAM w, LPARAM l) {
	int aux4, aux5, aux6, aux7, aux8, aux9, aux10;
	switch (m) {
	case WM_COMMAND:
		switch (LOWORD(w))
		{
		case IDCANCEL:
			EndDialog(h, 0);
			//return 1;

		case IDOK:
			aux4 = GetDlgItemInt(h, IDC_EDIT16, NULL, TRUE);
				if (aux4 < VODKA || aux4 > 30) {
					MessageBox(h, TEXT("VODKA: Compreendido entre 4 e 30"), TEXT("ATENÇÂO"), MB_OK);
				}
				else {
					vodka = aux4;
				}
			aux5 = GetDlgItemInt(h, IDC_EDIT17, NULL, TRUE);
				if (aux5 < OLEO || aux5 > 30) {
					MessageBox(h, TEXT("OLEO: Compreendido entre 5 e 30"), TEXT("ATENÇÂO"), MB_OK);
				}
				else {
					oleo = aux5;
				}
			aux6 = GetDlgItemInt(h, IDC_EDIT18, NULL, TRUE);
				if (aux6 < COLA || aux6 > 30) {
					MessageBox(h, TEXT("COLA: Compreendido entre 6 e 30"), TEXT("ATENÇÂO"), MB_OK);
				}
				else {
					cola = aux6;
				}
			aux7 = GetDlgItemInt(h, IDC_EDIT19, NULL, TRUE);
				if (aux7 < O_OLEO || aux7 > 30) {
					MessageBox(h, TEXT("O_OLEO: Compreendido entre 8 e 30"), TEXT("ATENÇÂO"), MB_OK);
				}
				else {
					o_oleo = aux7;
				}
			aux8 = GetDlgItemInt(h, IDC_EDIT20, NULL, TRUE);
				if (aux8 < O_COLA || aux8 > 30) {
					MessageBox(h, TEXT("O_COLA: Compreendido entre 9 e 30"), TEXT("ATENÇÂO"), MB_OK);
				}
				else {
					o_cola = aux8;
				}
			aux9 = GetDlgItemInt(h, IDC_EDIT21, NULL, TRUE);
				if (aux9 < O_VODKA || aux9 > 30) {
					MessageBox(h, TEXT("O_VODKA: Compreendido entre 7 e 30"), TEXT("ATENÇÂO"), MB_OK);
				}
				else {
					o_vodka = aux9;
				}
			aux10 = GetDlgItemInt(h, IDC_EDIT1, NULL, TRUE);
				if (aux10 < SURPRESA || aux10 > 30) {
					MessageBox(h, TEXT("SURPRESA: Compreendido entre 10 e 30"), TEXT("ATENÇÂO"), MB_OK);
				}
				else {
					surpresa = aux10;
				}
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(h, 0);
		return 1;

	case WM_INITDIALOG:
		SendDlgItemMessage(h, IDC_EDIT16, EM_LIMITTEXT, LIMITE_2POSI, NULL);
			SetDlgItemInt(h, IDC_EDIT16, vodka, TRUE);						// Tempo vida VODKA
		SendDlgItemMessage(h, IDC_EDIT17, EM_LIMITTEXT, LIMITE_2POSI, NULL);
			SetDlgItemInt(h, IDC_EDIT17, oleo, TRUE);						// Tempo vida OLEO
		SendDlgItemMessage(h, IDC_EDIT18, EM_LIMITTEXT, LIMITE_2POSI, NULL);
			SetDlgItemInt(h, IDC_EDIT18, cola, TRUE);						// Tempo vida COLA
		SendDlgItemMessage(h, IDC_EDIT19, EM_LIMITTEXT, LIMITE_2POSI, NULL);
			SetDlgItemInt(h, IDC_EDIT19, o_oleo, TRUE);						// Tempo vida O_OLEO
		SendDlgItemMessage(h, IDC_EDIT20, EM_LIMITTEXT, LIMITE_2POSI, NULL);
			SetDlgItemInt(h, IDC_EDIT20, o_cola, TRUE);						// Tempo vida O_COLA
		SendDlgItemMessage(h, IDC_EDIT21, EM_LIMITTEXT, LIMITE_2POSI, NULL);
			SetDlgItemInt(h, IDC_EDIT21, o_vodka, TRUE);					// Tempo vida O_VODKA
		SendDlgItemMessage(h, IDC_EDIT1, EM_LIMITTEXT, LIMITE_2POSI, NULL);
			SetDlgItemInt(h, IDC_EDIT1, surpresa, TRUE);					// Tempo vida SURPRESA
	}
	//Não foi tratado o evento
	return 0;
}

/* ----------------------------------------------------- */
/*		FUNÇÃO CALLBACK PARA PEDIR NOME JOGADOR 1		 */
/* ----------------------------------------------------- */

BOOL CALLBACK Pede_NomeJogador1(HWND h, UINT m, WPARAM w, LPARAM l) {
	switch (m) {
	case WM_COMMAND:
		switch (LOWORD(w)) {
		case IDCANCEL:
			EndDialog(h, 0);
			return 1;
		case IDOK:
			GetDlgItemText(h, IDC_EDIT23, username1, sizeof(IDC_EDIT23));
			EndDialog(h, 0);
			return 1;
		}
		return 1;
	case WM_CLOSE:
		
		EndDialog(h, 1);
		return TRUE;
	case WN_CANCEL:
		EndDialog(h, 0);
		return 1;
	case WM_INITDIALOG:
		SendDlgItemMessage(h, IDC_EDIT23, EM_LIMITTEXT, SIZE_USERNAME, NULL);
		return 1;
	}
	return 0;
}

/* ----------------------------------------------------- */
/*		FUNÇÃO CALLBACK PARA PEDIR NOME JOGADOR 2		 */
/* ----------------------------------------------------- */

BOOL CALLBACK Pede_NomeJogador2(HWND h, UINT m, WPARAM w, LPARAM l) {
	switch (m) {
	case WM_COMMAND:
		switch (LOWORD(w)) {
		case IDCANCEL:
			EndDialog(h, 0);
			return 1;
		case IDOK:
			GetDlgItemText(h, IDC_EDIT24, username2, sizeof(IDC_EDIT24));
			EndDialog(h, 0);
			return 1;
		}
		return 1;
	case WM_CLOSE:
		EndDialog(h, 1);
		return TRUE;
	case WN_CANCEL:
		EndDialog(h, 0);
		return 1;
	case WM_INITDIALOG:
		SendDlgItemMessage(h, IDC_EDIT24, EM_LIMITTEXT, SIZE_USERNAME, NULL);
		return 1;
	}
	return 0;
}

/* ----------------------------------------------------- */
/*	FUNÇÃO CALLBACK BASE DA APLICAÇÃO / TRATA TECLADO    */
/* ----------------------------------------------------- */

LRESULT CALLBACK TrataEventos(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	hMenu = GetMenu(hWnd);							// Para obter o Handle do Menu
	PAINTSTRUCT pintar;
	RECT area;
	int resposta, valor;
	int retorno_utilizadores = 0;
	DWORD auxtid;

	switch (messg) {
		case WM_CREATE:		
					//teste = CreateMutex(NULL, TRUE, NOME_MUTEX_TESTE);
					//if (GetLastError() == ERROR_ALREADY_EXISTS) {
					//	MessageBox(hWnd, TEXT("Já foi criado o jogo !"), TEXT("ERRO"), MB_YESNO == IDYES);
					////apresenta messagebox e sai do programa
					//}
					CarregaBitmaps(hWnd);  // Carregar BitMaps para memoria
				break;

		case WM_CLOSE:				
					if (MessageBox(hWnd, TEXT("Quer mesmo sair?"), TEXT("Snake Multiplayer."), MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
					{
					//fechaMemoriaPartilhada();
					fechaMemoriaPartilhadaGeral();
					fechaMemoriaPartilhadaResposta();
					PostQuitMessage(0);
					}
				break;

		case WM_COMMAND:			
			switch (wParam)
			{					
			case ID_SERVIDOR_REMOTO:DialogBox(hInstGlobal, IDD_DIALOG3, hWnd, IndicaIPRemoto);
									EnableMenuItem(hMenu, ID_JOGO_CRIAR, MF_ENABLED);
									EnableMenuItem(hMenu, ID_JOGO_ASSOCIAR, MF_ENABLED);
									EnableMenuItem(hMenu, ID_JOGO_JOGAR, MF_ENABLED);
									EnableMenuItem(hMenu, ID_CONF_TECLAS_1, MF_ENABLED);
									EnableMenuItem(hMenu, ID_CONF_TECLAS_2, MF_ENABLED);
									break;

			case ID_SERVIDOR_LOCAL:tipoServidor = LOCAL;
									preparaMemoriaPartilhada();
									pede_RegistarClienteLocal(pId,tId);
									Sleep(1000);
									preparaMemoriaPartilhadaResposta(pId,tId);
									WaitForSingleObject(hEventoResposta, INFINITE);
									ResetEvent(hEventoResposta);
									if (vistaResposta->resposta == SUCESSO) {
										MessageBox(hWnd, TEXT("Ligado a servidor Local"), TEXT("SUCESSO"), MB_OK);													
										EnableMenuItem(hMenu, ID_JOGO_CRIAR, MF_ENABLED);
										EnableMenuItem(hMenu, ID_JOGO_ASSOCIAR, MF_ENABLED);
										EnableMenuItem(hMenu, ID_JOGO_JOGAR, MF_ENABLED);
										EnableMenuItem(hMenu, ID_CONF_TECLAS_1, MF_ENABLED);
										EnableMenuItem(hMenu, ID_CONF_TECLAS_2, MF_ENABLED);
										}
										else if (vistaResposta->resposta == INSUCESSO) {
											MessageBox(hWnd, TEXT("Não foi possivel ligar ao servidor, contacte o utilizador da maquina"), TEXT("INSUCESSO"), MB_OK);
										}
									break;

				case ID_JOGO_CRIAR:	DialogBox(hInstGlobal, IDD_DIALOG5, hWnd, Pede_NomeJogador1);
								{
									resposta = chamaCriaJogo(&valor);
									if (resposta == SUCESSO) {
										MessageBox(hWnd, TEXT("Jogo Criado"), TEXT("SUCESSO"), BS_AUTOCHECKBOX | MB_OK);
										valorCobra1 = valor;
										EnableMenuItem(hMenu, ID_JOGO_ASSOCIAR, MF_ENABLED);
										EnableMenuItem(hMenu, ID_JOGO_JOGAR, MF_ENABLED);
										EnableMenuItem(hMenu, ID_CONF_TECLAS_1, MF_ENABLED);
										EnableMenuItem(hMenu, ID_CONF_TECLAS_2, MF_ENABLED);

											// ****** CRIAR hWandle Radio Buttons *****
										hWndRButton1 = CreateWindowEx(0,"BUTTON","Button 1",
											WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON,
											10,10,80,20,hWnd,(HMENU)IDC_RADIO1,hInstGlobal,NULL);

										hWndRbutton2 = CreateWindowEx(0, "BUTTON", "Button 2",
											WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON,
											10, 10, 80, 20, hWnd, (HMENU)IDC_RADIO2, hInstGlobal, NULL);

											// ***********
										
											case IDC_RADIO1:
											{
												switch (HIWORD(wParam)) {
												case BN_CLICKED:
													if (SendDlgItemMessage(hWnd, IDC_RADIO1, BM_GETCHECK, 0, 0) == 0)
													{
														SendDlgItemMessage(hWnd, IDC_RADIO1, BM_SETCHECK, 1, 0);
														SendDlgItemMessage(hWnd, IDC_RADIO2, BM_SETCHECK, 0, 0);
													}
												case BST_CHECKED:
													MessageBox(hWnd, TEXT("Vai jogar com a teclas por defeito."), TEXT("Selecção teclas do jogo."), MB_OK);
													break;
												}	
											}
											case IDC_RADIO2:
											{
												switch (HIWORD(wParam))	{
												case BN_CLICKED:
													if (SendDlgItemMessage(hWnd, IDC_RADIO2, BM_GETCHECK, 0, 0) == 0)
													{
														SendDlgItemMessage(hWnd, IDC_RADIO2, BM_SETCHECK, 1, 0);
														SendDlgItemMessage(hWnd, IDC_RADIO1, BM_SETCHECK, 0, 0);
													}
												case BST_CHECKED:
													DialogBox(hInstGlobal, IDD_TECLAS1, hWnd, ConfigTeclas1);
												break;																						
												}
											}						
											break;								

											
										//*********
									}
									else if (resposta == INSUCESSO) {
										MessageBox(hWnd, TEXT("Não é possivel criar jogo neste momento"), TEXT("INSUCESSO"), MB_OK);
									}
									break;
								}

			case ID_JOGO_ASSOCIAR:if (numJogadoresLocal == 0) {
									DialogBox(hInstGlobal, IDD_DIALOG5, hWnd, Pede_NomeJogador1);
									resposta = 	chamaAssociaJogo(username1, ASSOCIAR_JOGADOR1,&valor);
									if (resposta == SUCESSO) {
										MessageBox(hWnd, TEXT("Jogador 1 Associado"), TEXT("SUCESSO"), MB_OK);
										valorCobra1 = valor;
										numJogadoresLocal++;
									}
									else if (resposta == INSUCESSO) {
										if (valor == AGORANAO) {
										MessageBox(hWnd, TEXT("Não é possivel associar ao jogo neste momento"), TEXT("INSUCESSO"), MB_OK);
									}
									else if (valor == JOGOCHEIO) {
										MessageBox(hWnd, TEXT("Não existem mais vagas no jogo"), TEXT("INSUCESSO"), MB_OK);
											}
										}
									}
									else if (numJogadoresLocal == 1) {
										DialogBox(hInstGlobal, IDD_DIALOG6, hWnd, Pede_NomeJogador2);
											resposta = chamaAssociaJogo(username2, ASSOCIAR_JOGADOR2, &valor);
											if (resposta == SUCESSO) {
												MessageBox(hWnd, TEXT("Jogador 2 Associado"), TEXT("SUCESSO"), MB_OK);
												valorCobra2 = valor;
												numJogadoresLocal++;
											}
											else if (resposta == INSUCESSO) {
												if (valor == AGORANAO) {
												MessageBox(hWnd, TEXT("Não é possivel associar ao jogo neste momento"), TEXT("INSUCESSO"), MB_OK);
											}
											else if (valor == JOGOCHEIO) {
												MessageBox(hWnd, TEXT("Não existem mais vagas no jogo"), TEXT("INSUCESSO"), MB_OK);
												}
											}
										}
									break;

			case ID_JOGO_JOGAR:	resposta = chamaIniciaJogo(&valor);
								if (resposta == SUCESSO) {
									MessageBox(hWnd, TEXT("Jogo Iniciado"), TEXT("SUCESSO"), MB_OK);//lançar as threads de actualização do mapa
											CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)esperaActualizacao, (LPVOID)NULL, 0, &auxtid);
										}
										else if (resposta == INSUCESSO) {
										if (valor == CRIADORERRADO) {
											MessageBox(hWnd, TEXT("Apenas o cliente que criou o jogo pode iniciar o mesmo!"), TEXT("INSUCESSO"), MB_OK);
										}
										if (valor == AGORANAO) {
											MessageBox(hWnd, TEXT("Deve iniciar o jogo só depois de criar o mesmo!"), TEXT("INSUCESSO"), MB_OK);
											}
										}						
									break;	

			case ID_CONFIGURAR_TABULEIRO: DialogBox(hInstGlobal, IDD_DIALOG1, hWnd, ConfiguraJogo);
													
									break;

			case ID_CONFIGURAR_OBJECTOS: DialogBox(hInstGlobal, IDD_DIALOG4, hWnd, ConfiguraObjectos);
													
									break;

			case ID_EDITARJPGS:	if (CreateProcess(NULL, executavel, NULL, NULL, 0, 0, NULL, NULL, &si, &pi)) {														
									return 1;
								}
								else
									return 0;
									break;

			case ID_SAIR:if (MessageBox(hWnd, TEXT("Quer mesmo sair?"), TEXT("Snake Multiplayer."), MB_YESNO | MB_ICONEXCLAMATION) == IDYES)												
							PostQuitMessage(0);
									break;

			case ID_AUTORIA:MessageBoxA(hWnd, "\n\tSO2\n\n\nTrabalho Prático 2016/2017\n\nPaulo Alves - Aluno nº 21170449\n\t&\nJean Matias - Aluno nº 21200943 \n", "Snake Multiplayer", MB_OK);											
									break;

			case ID_AJUDA_AJUDA:MessageBoxA(hWnd, "\n\tSO2\n\n\n 1º Deve escolher se joga localmente\n ou liga a computador remoto. \n", "Snake Multiplayer", MB_OK);												
									break;

			case ID_CONF_TECLAS_1:DialogBox(hInstGlobal, IDD_TECLAS1, hWnd, ConfigTeclas1);												
									break;

			case ID_CONF_TECLAS_2:DialogBox(hInstGlobal, IDD_TECLAS2, hWnd, ConfigTeclas2);												
									break;
					}
		
			case WM_ERASEBKGND:

				break;

			case WM_PAINT:
				device = BeginPaint(hWnd, &pintar);
				BitBlt(device, 0, 0, maxX, maxY, memoriajanela, 0, 0, SRCCOPY);
				EndPaint(hWnd, &pintar);								
				break;

			case WM_KEYUP:					
				break;						
			default:				
		return(DefWindowProc(hWnd, messg, wParam, lParam));
	}
	return(0);
}

/* ----------------------------------------------------- */
/*	    FUNÇÕES DE CHAMADA DE FUNÇÔES DE PEDIDOS         */
/* ----------------------------------------------------- */

int chamaCriaJogo(int *valor) {
	ConfigInicial aux;

	aux.A = cobrasAutomaticas;
	aux.C = colunasConfig;
	aux.L = linhasConfig;
	aux.N = numJogadores;
	aux.O = numObjectos;
	aux.T = tamanhoSnakes;

	switch (tipoServidor)
	{
	case LOCAL:pede_CriaJogo(aux, pId,tId, username1);
			//ler Resposta e só depois validar
		WaitForSingleObject(hEventoResposta, INFINITE);
		ResetEvent(hEventoResposta);
		*valor = vistaResposta->valor;
		return vistaResposta->resposta;
		break;
	case REMOTO://comunicar por pipes
		break;
	default:
		break;
	}
}

int chamaAssociaJogo(TCHAR username[SIZE_USERNAME], int codigo, int *valor) {
	
	switch (tipoServidor)
	{
	case LOCAL:pede_AssociaJogo(pId,tId, username, codigo);
			//ler Resposta e só depois validar
			WaitForSingleObject(hEventoResposta, INFINITE);
			ResetEvent(hEventoResposta);
			*valor = vistaResposta->valor;
			return vistaResposta->resposta;
		break;
	case REMOTO://comunicar por pipes
		break;
	default:
		break;
	}
}

int chamaIniciaJogo(int *valor) {

	switch (tipoServidor)
	{
	case LOCAL:pede_IniciaJogo(pId,tId);
		//ler Resposta e só depois validar
		WaitForSingleObject(hEventoResposta, INFINITE);
		ResetEvent(hEventoResposta);
		*valor = vistaResposta->valor;
		return vistaResposta->resposta;
		break;
	case REMOTO://comunicar por pipes
		break;
	default:
		break;
	}
}

/* ----------------------------------------------------- */
/*	          FUNÇÃO CONFIGURAÇÃO TECLAS                 */
/* ----------------------------------------------------- */

/*==============================================================================
Função Configuar Teclas
==============================================================================*/
BOOL CALLBACK ConfigTeclas1(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	static int i;
	HANDLE hPipe;
	int n;
	char str[TAM];

	SetFocus(hWnd);
	switch (messg) {
	case WM_INITDIALOG:
		i = 0;
		SetDlgItemText(hWnd, IDC_TEXTO, TEXT("Pressione a tecla para se mover para a esquerda..."));
		return 1;
	case WM_KEYDOWN:
		switch (i) {
		case 0://apanhar tecla para a esquerda
			SetDlgItemText(hWnd, IDC_TEXTO, TEXT("Pressione a tecla para se mover para a direita..."));
			i++;
			teclasjogador1.esquerda = wParam;
			return 1;
		case 1://apanhar tecla para a direita
			SetDlgItemText(hWnd, IDC_TEXTO, TEXT("Pressione a tecla para se mover para cima..."));
			i++;
			teclasjogador1.direita = wParam;
			return 1;
		case 2://apanhar tecla para cima
			SetDlgItemText(hWnd, IDC_TEXTO, TEXT("Pressione a tecla para se mover para baixo..."));
			i++;
			teclasjogador1.cima = wParam;
			return 1;
		case 3://apanhar tecla para baixo
			teclasjogador1.baixo = wParam;
			EndDialog(hWnd, 1);
			return 1;
		}
		return 0;
	}
	return 0;
}

BOOL CALLBACK ConfigTeclas2(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	static int i;
	HANDLE hPipe;
	int n;
	char str[TAM];

	SetFocus(hWnd);
	switch (messg) {
	case WM_INITDIALOG:
		i = 0;
		SetDlgItemText(hWnd, IDC_TEXTO, TEXT("Pressione a tecla para se mover para a esquerda..."));
		return 1;
	case WM_KEYDOWN:
		switch (i) {
		case 0://apanhar tecla para a esquerda
			SetDlgItemText(hWnd, IDC_TEXTO, TEXT("Pressione a tecla para se mover para a direita..."));
			i++;
			teclasjogador2.esquerda = wParam;
			return 1;
		case 1://apanhar tecla para a direita
			SetDlgItemText(hWnd, IDC_TEXTO, TEXT("Pressione a tecla para se mover para cima..."));
			i++;
			teclasjogador2.direita = wParam;
			return 1;
		case 2://apanhar tecla para cima
			SetDlgItemText(hWnd, IDC_TEXTO, TEXT("Pressione a tecla para se mover para baixo..."));
			i++;
			teclasjogador2.cima = wParam;
			return 1;
		case 3://apanhar tecla para baixo
			teclasjogador2.baixo = wParam;
			EndDialog(hWnd, 1);
			return 1;
		}
		return 0;
	}
	return 0;
}



DWORD WINAPI esperaActualizacao(LPVOID param) {
	
	WaitForSingleObject(hEventoMapa, INFINITE);
	getLimitesMapa(&linhas, &colunas);
	getMapa(mapa);
	while (1) {   //enquanto o jogo estiver a continuar
		WaitForSingleObject(hEventoMapa, INFINITE);
		getMapa(mapa);
		desenhaMapaNaMemoria();
		InvalidateRect(janelaglobal, NULL, 0);
	}
}

void desenhaMapaNaMemoria() {
	int unidades, dezenas, centenas, aux;
	for (int x = 0; x < colunas; x++) {
		for (int y = 0; y < linhas; y++) {
			switch (mapa[y][x]) {
			case PAREDE:BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hparede, 0, 0, SRCCOPY);
				break;
			case ESPACOVAZIO:BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hfundo, 0, 0, SRCCOPY);
				break;
			case ALIMENTO:BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hmouse, 0, 0, SRCCOPY);
				break;
			case GELO:BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hgelo, 0, 0, SRCCOPY);
				break;
			case GRANADA:BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hgranada, 0, 0, SRCCOPY);
				break;
			case VODKA:BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hvodka, 0, 0, SRCCOPY);
				break;
			case OLEO:BitBlt(memoriajanela, x * 20, y * 20, 20, 20, holeo, 0, 0, SRCCOPY);
				break;
			case COLA:BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcola, 0, 0, SRCCOPY);
				break;
			case O_VODKA:BitBlt(memoriajanela, x * 20, y * 20, 20, 20, ho_vodka, 0, 0, SRCCOPY);
				break;
			case O_OLEO:BitBlt(memoriajanela, x * 20, y * 20, 20, 20, ho_oleo, 0, 0, SRCCOPY);
				break;
			case O_COLA:BitBlt(memoriajanela, x * 20, y * 20, 20, 20, ho_cola, 0, 0, SRCCOPY);
				break;
			case SURPRESA:BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hprenda, 0, 0, SRCCOPY);
				break;
			default://está uma cobra no mapa nesta posicao
				unidades = mapa[y][x] % 10;
				aux = mapa[y][x] / 10;
				dezenas = aux % 10;
				centenas = aux / 10;
				if (centenas == valorCobra1) {	//a cobra do jogador 1 está na posição ***
					switch (unidades)
					{//se tiver direção é cabeça senão é corpo da cobra
					case CIMA:BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcobra1_cab1_cima, 0, 0, SRCCOPY);
						break;
					case BAIXO:BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcobra1_cab1_baixo, 0, 0, SRCCOPY);
						break;
					case ESQUERDA:BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcobra1_cab1_esquerda, 0, 0, SRCCOPY);
						break;
					case DIREITA:BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcobra1_cab1_direita, 0, 0, SRCCOPY);
						break;
					case 0://não tem direção quer dizer que é corpo vamos ver o estado da cobra
						switch (dezenas) {
						case BEBADO:
							BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcobra1_corpo4, 0, 0, SRCCOPY);
							break;
						case LEBRE:
							BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcobra1_corpo3, 0, 0, SRCCOPY);
							break;
						case TARTARUGA:
							BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcobra1_corpo2, 0, 0, SRCCOPY);
							break;
						default://cor normal da cobra 1
							BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcobra1_corpo1, 0, 0, SRCCOPY);
							break;
						}
						break;
					}					
				}
				else if (centenas == valorCobra2) {	//a cobra do jogador 2 está na posição ***
					switch (dezenas) {
					case BEBADO:switch (unidades)
					{//se tiver direção é cabeça senão é corpo da cobra
					case CIMA:
						BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcobra2_cab2_cima, 0, 0, SRCCOPY);
						break;
					case BAIXO:
						BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcobra2_cab2_baixo, 0, 0, SRCCOPY);
						break;
					case ESQUERDA:
						BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcobra2_cab2_esquerda, 0, 0, SRCCOPY);
						break;
					case DIREITA:
						BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcobra2_cab2_direita, 0, 0, SRCCOPY);
						break;
					default:
						break;
					}
								break;
					case LEBRE:switch (unidades)
					{//se tiver direção é cabeça senão é corpo da cobra
					case CIMA:
						break;
					case BAIXO:
						break;
					case ESQUERDA:
						break;
					case DIREITA:
						break;
					default:
						break;
					}
							   break;
					case TARTARUGA:switch (unidades)
					{//se tiver direção é cabeça senão é corpo da cobra
					case CIMA:
						break;
					case BAIXO:
						break;
					case ESQUERDA:
						break;
					case DIREITA:
						break;
					default:
						break;
					}
								   break;
					default://cor normal da cobra 2
						switch (unidades)
						{//se tiver direção é cabeça senão é corpo da cobra
						case CIMA:BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcobra2_cab2_cima, 0, 0, SRCCOPY);
							break;
						case BAIXO:BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcobra2_cab2_baixo, 0, 0, SRCCOPY);
							break;
						case ESQUERDA:BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcobra2_cab2_esquerda, 0, 0, SRCCOPY);
							break;
						case DIREITA:BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcobra2_cab2_direita, 0, 0, SRCCOPY);
							break;
						default:
							BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcobra2_corpo2, 0, 0, SRCCOPY);
							break;
						}
						break;
					}
				}
				else {//está outra cobra na posição
					switch (unidades)
					{//se tiver direção é cabeça senão é corpo da cobra
					case CIMA:
						break;
					case BAIXO:
						break;
					case ESQUERDA:
						break;
					case DIREITA:
						break;
					default://corpo da cobra 3
						break;
					}
				}

				break;
			}
		}
	}
}

/* ----------------------------------------------------- */
/*	          FUNÇÃO CARREGAR BITMAPS                    */
/* ----------------------------------------------------- */

BOOL CALLBACK CarregaBitmaps(HWND hWnd) {

	/****** INICIO FUNÇÕES PARA CARREGAR BITMAPS - WM_CREATE ******/

	maxX = GetSystemMetrics(SM_CXSCREEN);
	maxY = GetSystemMetrics(SM_CYSCREEN);
	device = GetDC(hWnd);
	
	//Criar Janela Virtual
	memoriajanela = CreateCompatibleDC(device);
	hbit = CreateCompatibleBitmap(device, maxX, maxY);
	SelectObject(memoriajanela, hbit);
	SelectObject(memoriajanela, GetStockObject(WHITE_BRUSH));
	PatBlt(memoriajanela, 0, 0, maxX, maxY, PATCOPY);
	DeleteObject(hbit);

	// Criar janela virtual para fundo
	hfundo = CreateCompatibleDC(device);
	hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_FUNDO));
	SelectObject(hfundo, hbit);
	DeleteObject(hbit);

	// Criar janela virtual para parede
	hparede = CreateCompatibleDC(device);
	hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_PAREDE));
	SelectObject(hparede, hbit);
	DeleteObject(hbit);

	/****** INICIO FUNÇÕES PARA CARREGAR BITMAPS - COBRA 1 ******/
					// Criar janela virtual para cobra 1 cabeça cima
					hcobra1_cab1_cima = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_CABCOBRA1_CIMA));
					SelectObject(hcobra1_cab1_cima, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para cobra 1 cabeça baixo
					hcobra1_cab1_baixo = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_CABCOBRA1_BAIXO));
					SelectObject(hcobra1_cab1_baixo, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para cobra 1 cabeça esquerda
					hcobra1_cab1_esquerda = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_CABCOBRA1_ESQUERDA));
					SelectObject(hcobra1_cab1_esquerda, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para cobra 1 cabeça direita
					hcobra1_cab1_direita = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_CABCOBRA1_DIREITA));
					SelectObject(hcobra1_cab1_direita, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para cobra 1 corpo
					hcobra1_corpo1 = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_CORPOCOBRA1));
					SelectObject(hcobra1_corpo1, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para cobra 1 corpo ==> LENTA
					hcobra1_corpo2 = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_CORPOCOBRA1_2));
					SelectObject(hcobra1_corpo2, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para cobra 1 corpo ==> RAPIDA
					hcobra1_corpo3 = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_CORPOCOBRA1_3));
					SelectObject(hcobra1_corpo3, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para cobra 1 corpo ==> BEBADA
					hcobra1_corpo4 = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_CORPOCOBRA1_4));
					SelectObject(hcobra1_corpo4, hbit);
					DeleteObject(hbit);

	/****** INICIO FUNÇÕES PARA CARREGAR BITMAPS - COBRA 2 ******/
					// Criar janela virtual para cobra 2 cabeça cima
					hcobra2_cab2_cima = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_CABCOBRA2_CIMA));
					SelectObject(hcobra2_cab2_cima, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para cobra 2 cabeça baixo
					hcobra2_cab2_baixo = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_CABCOBRA2_BAIXO));
					SelectObject(hcobra2_cab2_baixo, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para cobra 2 cabeça esquerda
					hcobra2_cab2_esquerda = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_CABCOBRA2_ESQUERDA));
					SelectObject(hcobra2_cab2_esquerda, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para cobra 2 cabeça direita
					hcobra2_cab2_direita = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_CABCOBRA2_DIREITA));
					SelectObject(hcobra2_cab2_direita, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para cobra 2 corpo
					hcobra2_corpo1 = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_CORPOCOBRA2));
					SelectObject(hcobra2_corpo1, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para cobra 2 corpo ==> LENTA
					hcobra2_corpo2 = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_CORPOCOBRA2_2));
					SelectObject(hcobra2_corpo2, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para cobra 2 corpo ==> RAPIDA
					hcobra2_corpo3 = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_CORPOCOBRA2_3));
					SelectObject(hcobra2_corpo3, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para cobra 2 corpo ==> BEBADA
					hcobra2_corpo4 = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_CORPOCOBRA2_4));
					SelectObject(hcobra2_corpo4, hbit);
					DeleteObject(hbit);

	/****** INICIO FUNÇÕES PARA CARREGAR BITMAPS - COBRA 3 ******/
					// Criar janela virtual para cobra 3 cabeça cima
					hcobra3_cab3_cima = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_CABCOBRA3_CIMA));
					SelectObject(hcobra3_cab3_cima, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para cobra 3 cabeça baixo
					hcobra3_cab3_baixo = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_CABCOBRA3_BAIXO));
					SelectObject(hcobra3_cab3_baixo, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para cobra 3 cabeça esquerda
					hcobra3_cab3_esquerda = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_CABCOBRA3_ESQUERDA));
					SelectObject(hcobra3_cab3_esquerda, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para cobra 3 cabeça direita
					hcobra3_cab3_direita = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_CABCOBRA3_DIREITA));
					SelectObject(hcobra3_cab3_direita, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para cobra 3 corpo
					hcobra3_corpo1 = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_CORPOCOBRA3));
					SelectObject(hcobra3_corpo1, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para cobra 3 corpo ==> LENTA
					hcobra3_corpo2 = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_CORPOCOBRA3_2));
					SelectObject(hcobra3_corpo2, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para cobra 3 corpo ==> RAPIDA
					hcobra3_corpo3 = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_CORPOCOBRA3_3));
					SelectObject(hcobra3_corpo3, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para cobra 3 corpo ==> BEBADA
					hcobra3_corpo4 = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_CORPOCOBRA3_4));
					SelectObject(hcobra3_corpo4, hbit);
					DeleteObject(hbit);


	/****** INICIO FUNÇÕES PARA CARREGAR BITMAPS - OBJETOS ******/
					// Criar janela virtual para Comida Rato
					hcomida = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_ALIMENTO));
					SelectObject(hcomida, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para gelo
					hgelo = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_GELO));
					SelectObject(hgelo, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para granada
					hgranada = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_GRANADA));
					SelectObject(hgranada, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para Vodka
					hvodka = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_VODKA));
					SelectObject(hvodka, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para o_vodka
					ho_vodka = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_O_VODKA));
					SelectObject(ho_vodka, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para oleo
					holeo = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_OLEO));
					SelectObject(holeo, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para o_oleo
					ho_oleo = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_O_OLEO));
					SelectObject(ho_oleo, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para Cola
					hcola = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_COLA));
					SelectObject(hcola, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para O_Cola
					ho_cola = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_O_COLA));
					SelectObject(ho_cola, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para Surpresa Vida
					surpresa_vida = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_VIDA));
					SelectObject(surpresa_vida, hbit);
					DeleteObject(hbit);

					// Criar janela virtual para Surpresa Prenda
					hprenda = CreateCompatibleDC(device);
					hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_PRENDA));
					SelectObject(hprenda, hbit);
					DeleteObject(hbit);

	/* Igual janela */
	janelaglobal = hWnd;

	//hparede		=	LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_CORPOCOBRA1));

	/****** FIM FUNÇÕES PARA CARREGAR BITMAPS - WM_CREATE ******/

	return 1;
}
