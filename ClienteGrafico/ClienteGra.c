#include <windows.h>
#include <tchar.h>
#include "resource.h"
#include "..\..\SnakeDLL\SnakeDLL\SnakeDLL.h"


/* -----------------------------------------------------*/
/*         #DEFINES LOCAIS                              */
/* -----------------------------------------------------*/
#define MAX				256
#define TAM				6
#define LIMITE_1POSI	1
#define LIMITE_2POSI	2
#define LIMITE_3POSI	3

static int maxX;
static int maxY;

/* ----------------------------------------------------- */
/*          CAMINHOS GLOBBAIS                            */
/* ----------------------------------------------------- */

TCHAR full_path[MAX] = TEXT("SOFTWARE\\MinhaAplicação\\");
TCHAR full_path_orig[MAX] = TEXT("SOFTWARE\\MinhaAplicação\\");


/* ===================================================== */
/*		ZONA DE DECLARAÇÃO DE VARIAVEIS E FUNÇÕES        */
/* ===================================================== */

/* ----------------------------------------------------- */
/*  VARIÁVEIS GLOBAIS									 */
/* ----------------------------------------------------- */
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
int alimento			=	ALIMENTO;
int gelo				=	GELO;
int granada				=	GRANADA;
int vodka				=	VODKA;
int oleo				=	OLEO;
int cola				=	COLA;
int o_vodka				=	O_VODKA;
int o_oleo				=	O_OLEO;
int o_cola				=	O_COLA;
int surpresa			=	SURPRESA;

//**** Variáveis de configuração do Jogo ****
int numJogadoresLocal	=	0;				//Num Jogadores a jogar nesta maquina
int valorCobra1			=	0;				//valor da cobra do jogador 1 desta maquina
int valorCobra2			=	0;				//valor da cobra do jogador 2 desta maquina
TCHAR username1[SIZE_USERNAME];				//Nome do Jogador 1 desta Maquina
TCHAR username2[SIZE_USERNAME];				//Nome do Jogador 2 desta Maquina
int pId;									//Process Id deste cliente
int linhas;							
int colunas;
int mapa[MAX_LINHAS][MAX_COLUNAS];

//**** Estrutura de teclas ****
typedef struct {
	int esquerda, direita, cima, baixo;
}teclas;

teclas teclasjogador1 = { 1, 2, 3, 4 };
teclas teclasjogador2 = { 5, 6, 7, 8 };

/* ----------------------------------------------------- */
/*  PROTOTIPOS FUNÇÕES									 */
/* ----------------------------------------------------- */
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
BOOL CALLBACK ConfigTeclas1(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ConfigTeclas2(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK CarregaBitmaps(HWND hWnd);
DWORD WINAPI esperaActualizacao(LPVOID param);

// ========= FIM ZONA DECLARAÇÃO DE VARIAVEIS E FUNÇÕES ========= //

/* ===================================================== */
/* Programa base (esqueleto) para aplicações Windows     */
/* ===================================================== */
				// Cria uma janela de nome "Janela Principal" e pinta fundo de branco
				// Modelo para programas Windows:
				//  Composto por 2 funções: 
				//	WinMain()     = Ponto de entrada dos programas windows
				//			1) Define, cria e mostra a janela
				//			2) Loop de recepção de mensagens provenientes do Windows
				//     TrataEventos()= Processamentos da janela (pode ter outro nome)
				//			1) É chamada pelo Windows (callback) 
				//			2) Executa código em função da mensagem recebida

LRESULT CALLBACK TrataEventos(HWND, UINT, WPARAM, LPARAM);

				// Nome da classe da janela (para programas de uma só janela, normalmente este nome é 
				// igual ao do próprio programa) "szprogName" é usado mais abaixo na definição das 
				// propriedades do objecto janela

TCHAR *szProgName = TEXT("Snake");
TCHAR executavel[MAX] = TEXT("mspaint.exe");
//TCHAR executavel[MAX] = TEXT("%windir%\system32\mspaint.exe");

PROCESS_INFORMATION pi;
STARTUPINFO si;

/* --------------------------------------------------------- */
/*                   HANDLES GLOBAIS                         */
/* --------------------------------------------------------- */

HINSTANCE	hInstGlobal;
HMENU		hMenu;
HWND		janelaglobal;

HBITMAP		hbit;

//*** Outros Objectos ***
HDC	device;
HDC memoriajanela;
HDC hparede;

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
HDC hcobra1_corpo;
HDC hcobra1_corpo1;
HDC hcobra1_corpo2;
HDC hcobra1_corpo3;

//*** Cobra 2 ***
HDC hcobra2_cab2_esquerda, hcobra2_cab2_direita, hcobra2_cab2_cima, hcobra2_cab2_baixo;
HDC hcobra2_corpo;
HDC hcobra2_corpo1;
HDC hcobra2_corpo2;
HDC hcobra2_corpo3;

// ============================================================================
// FUNÇÃO DE INÍCIO DO PROGRAMA: WinMain()
// ============================================================================
				// Em Windows, o programa começa sempre a sua execução na função WinMain()que desempenha // o papel da função main() do C em modo consola WINAPI indica o "tipo da função" (WINAPI // para todas as declaradas nos headers do Windows e CALLBACK para as funções de        // processamento da janela)
				// Parâmetros:
				//   hInst: Gerado pelo Windows, é o handle (número) da instância deste programa 
				//   hPrevInst: Gerado pelo Windows, é sempre NULL para o NT (era usado no Windows 3.1)
				//   lpCmdLine: Gerado pelo Windows, é um ponteiro para uma string terminada por 0
				//              destinada a conter parâmetros para o programa 
				//   nCmdShow:  Parâmetro que especifica o modo de exibição da janela (usado em  
				//        	   ShowWindow()
				// ============================================================================
				// WinMain()
				// NOTA: Ver declaração de HACCEL, LoadAccelerators() e TranslateAccelerator()
				//		 Estas são as modificações necessárias para tratar as teclas de atalho
				//		 para opções do menu
				// ============================================================================

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {

	HWND hWnd;				// hWnd é o handler da janela, gerado mais abaixo por CreateWindow()
	MSG lpMsg;				// MSG é uma estrutura definida no Windows para as mensagens
	WNDCLASSEX wcApp;		// WNDCLASSEX é uma estrutura cujos membros servem para 
							// definir as características da classe da janela
	BOOL ret;
	HACCEL hAccel;			// Handler da resource accelerators (teclas de atalho

	hInstGlobal = hInst;

	pId = GetCurrentProcessId();


	/* ---- Definição Pipes ---- */

						// ============================================================================
						// 1. Definição das características da janela "wcApp" 
						//    (Valores dos elementos da estrutura "wcApp" do tipo WNDCLASSEX)
						// ============================================================================

	wcApp.cbSize = sizeof(WNDCLASSEX);				// Tamanho da estrutura WNDCLASSEX
	wcApp.hInstance = hInst;						// Instância da janela actualmente exibida 
													// ("hInst" é parâmetro de WinMain e vem 
													// inicializada daí)
	wcApp.lpszClassName = szProgName;				// Nome da janela (neste caso = nome do programa)
	wcApp.lpfnWndProc = TrataEventos;				// Endereço da função de processamento da janela 	// ("TrataEventos" foi declarada no início e                 // encontra-se mais abaixo)
	wcApp.style = CS_HREDRAW | CS_VREDRAW;			// Estilo da janela: Fazer o redraw se for      // modificada horizontal ou verticalmente
													// CS_VREDRAW
													//wcApp.hIcon = LoadIcon(NULL, IDI_HAND);// "hIcon" = handler do ícon normal
	wcApp.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDC_ICON));
													// Icon Grande		// IDI_APPLICATION		  //"NULL" = Icon definido no Windows
													// "IDI_AP..." Ícone "aplicação"
	wcApp.hIconSm = LoadIcon(hInst, IDI_ICON4);		// "hIconSm" = handler do ícon pequeno   //   
													// Icon Small		// IDI_INFORMATION			 //"NULL" = Icon definido no Windows
													// "IDI_INF..." Ícon de informação
	wcApp.hCursor = LoadCursor(NULL, IDC_ARROW);		// "hCursor" = handler do cursor (rato) 
													// IDC_ARROW				// "NULL" = Forma definida no Windows
													// "IDC_ARROW" Aspecto "seta" 
	wcApp.lpszMenuName = (LPCTSTR)(IDR_MENU1);
	//wcApp.lpszMenuName = NULL;					// Classe do menu que a janela pode ter
													// (NULL = não tem menu)
	wcApp.cbClsExtra = 0;							// Livre, para uso particular
	wcApp.cbWndExtra = 0;							// Livre, para uso particular
	wcApp.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
													// LTGRAY_BRUSH
													// "hbrBackground" = handler para "brush" de pintura do fundo da janela. Devolvido por  // "GetStockObject".Neste caso o fundo será branco
	
	/* ============================================================================*/
	/*					2. Registar a classe "wcApp" no Windows	                   */
	/* ============================================================================*/
	
	if (!RegisterClassEx(&wcApp))
		return(0);

	/* ============================================================================*/
	/*					3. Criar a janela                                          */
	/* ============================================================================*/
	
	hWnd = CreateWindow(
		szProgName,									// Nome da janela (programa) definido acima
		TEXT("Snake Multiplayer"),					// Texto que figura na barra do título
		// WS_VSCROLL |								// Adicinar Scrool Bar se necessário		
		WS_OVERLAPPEDWINDOW,						// Estilo da janela (WS_OVERLAPPED= normal)
		50,								//CW_USEDEFAULT,
													// Posição x pixels (default=à direita da última)
		50,								// Posição y pixels (default=abaixo da última)
		//CW_USEDEFAULT,		
		1200, //800,										// Largura da janela (em pixels)
		//CW_USEDEFAULT,		
		600, //600,										// Altura da janela (em pixels)
		//CW_USEDEFAULT,		
		(HWND)HWND_DESKTOP,							// handle da janela pai (se se criar uma a partir de
													// outra) ou HWND_DESKTOP se a janela for a primeira, 
													// criada a partir do "desktop"
		(HMENU)hMenu,								// handle do menu da janela (se tiver menu)
		(HINSTANCE)hInst,							// handle da instância do programa actual ("hInst" é 
													// passado num dos parâmetros de WinMain()
		0);											// Não há parâmetros adicionais para a janela

	/* ============================================================================ */
	/* Carregar as definições das teclas aceleradoras (atalhos de opções dos Menus) */
	/* LoadAccelerators(instância_programa, ID_da_resource_atalhos)				    */
	/* ============================================================================ */

	hAccel = LoadAccelerators(hInst, (LPCWSTR)IDR_ACCELERATOR1);

	/* ============================================================================ */
	/*				4. Mostrar a janela												*/
	/* ============================================================================ */

	ShowWindow(hWnd, nCmdShow);			// "hWnd"= handler da janela, devolvido por 
										// "CreateWindow"; "nCmdShow"= modo de exibição (p.e. 
										// normal/modal); é passado como parâmetro de WinMain()
			UpdateWindow(hWnd);			// Refrescar a janela (Windows envia à janela uma 
										// mensagem para pintar, mostrar dados, (refrescar)… 
										// ============================================================================
										// 5. Loop de Mensagens
										// ============================================================================
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
				TranslateMessage(&lpMsg);	// Pré-processamento da mensagem (p.e. obter código 
											// ASCII da tecla premida)
				DispatchMessage(&lpMsg);	// Enviar a mensagem traduzida de volta ao Windows, que
											// aguarda até que a possa reenviar à função de 
											// tratamento da janela, CALLBACK TrataEventos (abaixo)
			}
		}
	}

	/* ============================================================================ */
	/*						6. Fim do programa										*/
	/* ============================================================================ */

	return((int)lpMsg.wParam);				// Retorna sempre o parâmetro wParam da estrutura lpMsg
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
/*					FUNÇÕES CALLBACK											*/
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

// ****************  FIM DAS FUNÇÕES CALLBACK GERAIS *************************************

/* ----------------------------------------------------- */
/*	FUNÇÃO CALLBACK BASE DA APLICAÇÃO / TRATA TECLADO    */
/* ----------------------------------------------------- */

LRESULT CALLBACK TrataEventos(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	hMenu = GetMenu(hWnd);							// Para obter o Handle do Menu
	PAINTSTRUCT pintar;
	RECT area;
	int resposta, valor;
	int retorno_utilizadores = 0;
	DWORD tid;

	switch (messg) {
										// Carregar BitMaps para memoria
		case WM_CREATE:					
									CarregaBitmaps(hWnd);  

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

		case WM_COMMAND:			// Para configuração dos parametros de TABULEIRO
									// Sem PAI fica não bloqueante, pode ficar NULL ou Handle do Pai para bloquear
			switch (wParam)
					{
					case ID_SERVIDOR_REMOTO:	DialogBox(hInstGlobal, IDD_DIALOG3, hWnd, IndicaIPRemoto);
												EnableMenuItem(hMenu, ID_JOGO_ASSOCIAR, MF_ENABLED);
												EnableMenuItem(hMenu, ID_JOGO_CRIAR, MF_ENABLED);
												EnableMenuItem(hMenu, ID_JOGO_JOGAR, MF_ENABLED);
												EnableMenuItem(hMenu, ID_CONF_TECLAS_1, MF_ENABLED);
												EnableMenuItem(hMenu, ID_CONF_TECLAS_2, MF_ENABLED);
												
									break;
					case ID_SERVIDOR_LOCAL:		tipoServidor = LOCAL;
												preparaMemoriaPartilhada();
												pede_RegistarClienteLocal(pId);
												Sleep(1000);
												preparaMemoriaPartilhadaResposta(pId);
												WaitForSingleObject(hEventoResposta, INFINITE);
												ResetEvent(hEventoResposta);
												
												if (vistaResposta->resposta == SUCESSO) {
													MessageBox(hWnd, TEXT("Ligado a servidor Local"), TEXT("SUCESSO"), MB_OK);
													EnableMenuItem(hMenu, ID_JOGO_ASSOCIAR, MF_ENABLED);
													EnableMenuItem(hMenu, ID_JOGO_CRIAR, MF_ENABLED);
													EnableMenuItem(hMenu, ID_JOGO_JOGAR, MF_ENABLED);
													EnableMenuItem(hMenu, ID_CONF_TECLAS_1, MF_ENABLED);
													EnableMenuItem(hMenu, ID_CONF_TECLAS_2, MF_ENABLED);
													
												}
												else if (vistaResposta->resposta == INSUCESSO) {
													MessageBox(hWnd, TEXT("Não foi possivel ligar ao servidor, contacte o utilizador da maquina"), TEXT("INSUCESSO"), MB_OK);
													
												}
												
									break;
					case ID_JOGO_CRIAR:			DialogBox(hInstGlobal, IDD_DIALOG5, hWnd, Pede_NomeJogador1);
												resposta = chamaCriaJogo(&valor);
												retorno_utilizadores = validaUser(username1, retorno_utilizadores);
												if (resposta == SUCESSO) {
													MessageBox(hWnd, TEXT("Jogo Criado"), TEXT("SUCESSO"), MB_OK);
													valorCobra1 = valor;
													
												}
												else if (resposta == INSUCESSO) {
													MessageBox(hWnd, TEXT("Não é possivel criar jogo neste momento"), TEXT("INSUCESSO"), MB_OK);
													
												}
									break;
					case ID_JOGO_ASSOCIAR:		if (numJogadoresLocal == 0) {
													DialogBox(hInstGlobal, IDD_DIALOG5, hWnd, Pede_NomeJogador1);
													resposta = 	chamaAssociaJogo(username1, ASSOCIAR_JOGADOR1,&valor);
													retorno_utilizadores = validaUser(username1, retorno_utilizadores);
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
													retorno_utilizadores = validaUser(username2, retorno_utilizadores);
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
					case ID_JOGO_JOGAR:			resposta = chamaIniciaJogo(&valor);
												if (resposta == SUCESSO) {
													MessageBox(hWnd, TEXT("Jogo Iniciado"), TEXT("SUCESSO"), MB_OK);//lançar as threads de actualização do mapa
													CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)esperaActualizacao, (LPVOID)NULL, 0, &tid);
													
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
					case ID_CONFIGURAR_TABULEIRO:	DialogBox(hInstGlobal, IDD_DIALOG1, hWnd, ConfiguraJogo);
													
									break;
					case ID_CONFIGURAR_OBJECTOS:	DialogBox(hInstGlobal, IDD_DIALOG4, hWnd, ConfiguraObjectos);
													
									break;

					case ID_EDITARJPGS:			if (CreateProcess(NULL, executavel, NULL, NULL, 0, 0, NULL, NULL, &si, &pi)) {
														
														return 1;
													}
												   else
														return 0;
									break;

					case ID_SAIR40011:			if (MessageBox(hWnd, TEXT("Quer mesmo sair?"), TEXT("Snake Multiplayer."), MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
												
												PostQuitMessage(0);
									break;

					case ID_INFORMA40018:		MessageBoxA(hWnd, "\n\tSO2\n\n\nTrabalho Prático 2016/2017\n\nPaulo Alves - Aluno nº 21170449\n\t&\nJean Matias - Aluno nº 21200943 \n", "Snake Multiplayer", MB_OK);
											
									break;

					case ID_AJUDA_AJUDA:		MessageBoxA(hWnd, "\n\tSO2\n\n\n 1º Deve escolher se joga localmente\n ou liga a computador remoto. \n", "Snake Multiplayer", MB_OK);
												
									break;

					case ID_CONF_TECLAS_1:		DialogBox(hInstGlobal, IDD_TECLAS_1, hWnd, ConfigTeclas1);
												
									break;

					case ID_CONF_TECLAS_2:		DialogBox(hInstGlobal, IDD_TECLAS_2, hWnd, ConfigTeclas2);
												
									break;
					}
		
		case WM_ERASEBKGND:
							
									break;

		case WM_PAINT:

				device = BeginPaint(hWnd, &pintar);
				BitBlt(device, 0, 0, maxX, maxY, memoriajanela, 0, 0, SRCCOPY);
				EndPaint(hWnd, &pintar);

				
						//hdc = BeginPaint(hWnd, &pintar);

				//esperaActualizacao(lParam);    *******************************************
					/*							
						//  Parede
						for (int i = 0; i < colunasConfig; i++)
							for (int j = 0; j < linhasConfig; j++)
								//TransparentBlt(hdc, i * 40, j * 40, 45, 40, hparede, 0, 0, 187, 125, RGB(255, 255, 255));
								BitBlt(hdc, i * 20, j * 20, maxX, maxY, hparede, 0, 0, SRCCOPY);

						// Cobra 1 cabeça cima
							//TransparentBlt(hdc, 40, 40, 70, 70, hcobra1_cab1_cima, 0, 0, 187, 125, RGB(255, 255, 255));
						BitBlt(hdc, 80, 40, 80, maxY, hcobra1_cab1_cima, 0, 0, SRCCOPY);

						// Cobra 1 cabeça baixo
							//TransparentBlt(hdc, 100, 150, 50, 50, hcobra1_cab1_baixo, 0, 0, 180, 125, RGB(255, 255, 255));
						BitBlt(hdc, 100, 150, maxX, 80, hcobra1_cab1_baixo, 0, 0, SRCCOPY);

						// Cobra 1 cabeça esquerda
							//TransparentBlt(hdc, 140, 50, 100, 100, hcobra1_cab1_esquerda, 0, 0, 280, 125, RGB(255, 255, 255));
						BitBlt(hdc, 180, 140, 80, maxY, hcobra1_cab1_esquerda, 0, 0, SRCCOPY);

						// Cobra 1 cabeça direita
						BitBlt(hdc, 487, 140, maxX, 80, hcobra1_cab1_direita, 0, 0, SRCCOPY);
						// Cobra 1 corpo
						BitBlt(hdc, 467, 140, 35, 35, hcobra1_corpo1, 0, 0, SRCCOPY);
						BitBlt(hdc, 447, 140, 35, 35, hcobra1_corpo1, 0, 0, SRCCOPY);
						BitBlt(hdc, 427, 140, 35, 35, hcobra1_corpo1, 0, 0, SRCCOPY);

						// Cobra 2 Cabeça direita
						BitBlt(hdc, 487, 180, maxX, 80, hcobra1_cab1_direita, 0, 0, SRCCOPY);
						// Cobra 2 corpo
						BitBlt(hdc, 467, 180, 35, 35, hcobra1_corpo2, 0, 0, SRCCOPY);
						BitBlt(hdc, 447, 180, 35, 35, hcobra1_corpo2, 0, 0, SRCCOPY);
						BitBlt(hdc, 427, 180, 35, 35, hcobra1_corpo2, 0, 0, SRCCOPY);

						// Cobra 3 Cabeça direita
						BitBlt(hdc, 487, 220, maxX, 80, hcobra1_cab1_direita, 0, 0, SRCCOPY);
						// Cobra 2 corpo
						BitBlt(hdc, 467, 220, 35, 35, hcobra1_corpo3, 0, 0, SRCCOPY);
						BitBlt(hdc, 447, 220, 35, 35, hcobra1_corpo3, 0, 0, SRCCOPY);
						BitBlt(hdc, 427, 220, 35, 35, hcobra1_corpo3, 0, 0, SRCCOPY);


						// Comida rato
						BitBlt(hdc, 487, 440, 20, 20, hcomida, 0, 0, SRCCOPY);

						//ReleaseDC(hWnd, hdc);
						//InvalidateRect(hWnd, NULL, 1);

						EndPaint(hWnd, &pintar);*/

				break;
		case WM_KEYUP:

						
				break;
						// TRATANDO O WM_CLOSE não é necessário tartar o WM_DESTROY  <<============  NOTA
						//case WM_DESTROY:	// Destruir a janela e terminar o programa 
						//					// "PostQuitMessage(Exit Status)"	
						//		PostQuitMessage(0);
						//	break;
		default:
						// Neste exemplo, para qualquer outra mensagem (p.e. "minimizar","maximizar","restaurar") // não é efectuado nenhum processamento, apenas se segue o "default" do Windows			
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
	case LOCAL:pede_CriaJogo(aux, pId, username1);
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
	case LOCAL:pede_AssociaJogo(pId, username, codigo);
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
	case LOCAL:pede_IniciaJogo(pId);
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

BOOL CALLBACK ConfigTeclas1(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	static int i;
	HANDLE hPipe;
	int n;
	char str[TAM];
	SetFocus(hWnd);

	switch (messg) {
		case WM_COMMAND:
				switch (LOWORD(wParam)) {
				case IDCANCEL:
					EndDialog(hWnd, 0);
					//return 1;

				case IDOK:							// esquerda, direita, cima, baixo
					GetDlgItemText(hWnd, IDC_EDIT1, teclasjogador1.esquerda, sizeof(IDC_EDIT1));
					GetDlgItemText(hWnd, IDC_EDIT2, teclasjogador1.direita, sizeof(IDC_EDIT2));
					GetDlgItemText(hWnd, IDC_EDIT3, teclasjogador1.cima, sizeof(IDC_EDIT3));
					GetDlgItemText(hWnd, IDC_EDIT4, teclasjogador1.baixo, sizeof(IDC_EDIT4));
					EndDialog(hWnd, 0);
					break;
				}
				break;

		case WM_CLOSE:
				EndDialog(hWnd, 0);
				return 1;

		case WM_INITDIALOG:
				//SetDlgItemText(hWnd, NULL, (LPCSTR)"Pressione a tecla para se mover para a esquerda...");
					SendDlgItemMessage(hWnd, IDC_EDIT1, EM_LIMITTEXT, LIMITE_1POSI, NULL);
						SetDlgItemInt(hWnd, IDC_EDIT1, teclasjogador1.esquerda, TRUE);
					SendDlgItemMessage(hWnd, IDC_EDIT2, EM_LIMITTEXT, LIMITE_1POSI, NULL);
						SetDlgItemInt(hWnd, IDC_EDIT2, teclasjogador1.direita, TRUE);
					SendDlgItemMessage(hWnd, IDC_EDIT3, EM_LIMITTEXT, LIMITE_1POSI, NULL);
						SetDlgItemInt(hWnd, IDC_EDIT3, teclasjogador1.cima, TRUE);
					SendDlgItemMessage(hWnd, IDC_EDIT4, EM_LIMITTEXT, LIMITE_1POSI, NULL);
						SetDlgItemInt(hWnd, IDC_EDIT4, teclasjogador1.baixo, TRUE);
				return 1;

		case WM_KEYDOWN:
				//	switch (i) {
				//	case 0://apanhar tecla para a esquerda
				//		SetDlgItemText(hWnd, IDC_TEXTO, (LPCSTR)"Pressione a tecla para se mover para a direita...");
				//		i++;
				//		t.esquerda = wParam;
				//		return 1;
				//	case 1://apanhar tecla para a direita
				//		SetDlgItemText(hWnd, IDC_TEXTO, (LPCSTR)"Pressione a tecla para se mover para cima...");
				//		i++;
				//		t.direita = wParam;
				//		return 1;
				//	case 2://apanhar tecla para cima
				//		SetDlgItemText(hWnd, IDC_TEXTO, (LPCSTR)"Pressione a tecla para se mover para baixo...");
				//		i++;
				//		t.cima = wParam;
				//		return 1;
				//	case 3://apanhar tecla para baixo
				//		t.baixo = wParam;
				//		//enviar teclas ao servidor
				//		hPipe = LigarAoPipe(PIPE_NAME, "");
				//		sprintf(str, "keyboard");
				//		if (!WriteFile(hPipe, &str, TAM, (LPDWORD)&n, NULL)) {
				//			exit(1);
				//		}
				//		if (!WriteFile(hPipe, j.nome, TAM, (LPDWORD)&n, NULL)) {
				//			exit(1);
				//		}
				//		if (!WriteFile(hPipe, &t, sizeof(struct teclas), (LPDWORD)&n, NULL)) {
				//			exit(1);
				//		}
				//		CloseHandle(hPipe);
				//		EndDialog(hWnd, 1);
				//		return 1;
				//	}
				return 0;
			}
	return 0;
}

BOOL CALLBACK ConfigTeclas2(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	


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
	for (int i = 0; i < linhas; i++) {
		for (int j = 0; j < colunas; j++) {
			switch (mapa[i][j]) {
			case PAREDE:BitBlt(memoriajanela, i * 20, j * 20, 20, 20, hparede, 0, 0, SRCCOPY);
				break;
			case ESPACOVAZIO:
				break;
			case ALIMENTO:BitBlt(memoriajanela, i * 20, j * 20, 20, 20, hmouse, 0, 0, SRCCOPY);
				break;
			case GELO:BitBlt(memoriajanela, i * 20, j * 20, 20, 20, hgelo, 0, 0, SRCCOPY);
				break;
			case GRANADA:BitBlt(memoriajanela, i * 20, j * 20, 20, 20, hgranada, 0, 0, SRCCOPY);
				break;
			case VODKA:BitBlt(memoriajanela, i * 20, j * 20, 20, 20, hvodka, 0, 0, SRCCOPY);
				break;
			case OLEO:BitBlt(memoriajanela, i * 20, j * 20, 20, 20, holeo, 0, 0, SRCCOPY);
				break;
			case COLA:BitBlt(memoriajanela, i * 20, j * 20, 20, 20, hcola, 0, 0, SRCCOPY);
				break;
			case O_VODKA://BitBlt(memoriajanela, i * 20, j * 20, 20, 20, ho, 0, 0, SRCCOPY);
				break;
			case O_OLEO://BitBlt(memoriajanela, i * 20, j * 20, 20, 20, hparede, 0, 0, SRCCOPY);
				break;
			case O_COLA://BitBlt(memoriajanela, i * 20, j * 20, 20, 20, hparede, 0, 0, SRCCOPY);
				break;
			case SURPRESA:BitBlt(memoriajanela, i * 20, j * 20, 20, 20, hprenda, 0, 0, SRCCOPY);
				break;
			default://está uma cobra no mapa nesta posicao
				unidades = mapa[i][j] % 10;
				aux = mapa[i][j] / 10;
				dezenas = aux % 10;
				centenas = aux / 10;
				if (centenas == valorCobra1) {//a cobra do jogador 1 está na posição
					switch (unidades)
					{//se tiver direção é cabeça senão é corpo da cobra
					case CIMA:BitBlt(memoriajanela, i * 20, j * 20, 20, 20, hcobra1_cab1_cima, 0, 0, SRCCOPY);
						break;
					case BAIXO:BitBlt(memoriajanela, i * 20, j * 20, 20, 20, hcobra1_cab1_baixo, 0, 0, SRCCOPY);
						break;
					case ESQUERDA:BitBlt(memoriajanela, i * 20, j * 20, 20, 20, hcobra1_cab1_esquerda, 0, 0, SRCCOPY);
						break;
					case DIREITA:BitBlt(memoriajanela, i * 20, j * 20, 20, 20, hcobra1_cab1_direita, 0, 0, SRCCOPY);
						break;
					case 0://não tem direção quer dizer que é corpo vamos ver o estado da cobra
						switch (dezenas) {
						case BEBADO:
							break;
						case LEBRE:
							break;
						case TARTARUGA:
							break;
						default://cor normal da cobra 1
							BitBlt(memoriajanela, i * 20, j * 20, 20, 20, hcobra1_corpo, 0, 0, SRCCOPY);
							break;
						}
						break;
					}					
				}
				else if (centenas == valorCobra2) {//a cobra do jogador 2 está na posição
					switch (dezenas) {
					case BEBADO:switch (unidades)
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

	// Criar janela virtual para parede
	hparede = CreateCompatibleDC(device);
	hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_PAREDE));
	SelectObject(hparede, hbit);
	DeleteObject(hbit);

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

	// Criar janela virtual para cobra 1 corpo Rapido
	hcobra1_corpo2 = CreateCompatibleDC(device);
	hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_CORPOCOBRA1_2));
	SelectObject(hcobra1_corpo2, hbit);
	DeleteObject(hbit);

	// Criar janela virtual para cobra 1 corpo Bebada
	hcobra1_corpo3 = CreateCompatibleDC(device);
	hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_CORPOCOBRA1_3));
	SelectObject(hcobra1_corpo3, hbit);
	DeleteObject(hbit);

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
	hbit = LoadBitmap(hInstGlobal, MAKEINTRESOURCE(IDB_SURPRESAVIDA));
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


/*******  FUNÇÃO PARA VALIDAR USER  *******/

int validaUser(TCHAR utilizador[SIZE_USERNAME], int usrsAtivos) {
	HKEY chave;
	DWORD oque;
	int quantos = 0;
	// HKEY_LOCAL_MACHINE
	for (int i = 0; i < MAXJOGADORES; i++) {
		_tcscpy_s(full_path, _tcslen(full_path) + 1, full_path_orig);
		_tcscat(full_path, utilizador);
		if (RegCreateKeyEx(HKEY_CURRENT_USER, &full_path, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &chave, &oque) != ERROR_SUCCESS) {
			_tprintf(TEXT("Erro ao criar/abrir chave (%d)\n"), GetLastError());
			return -1;
		}
		else {
			if (oque == REG_CREATED_NEW_KEY) {
				_tprintf(TEXT("\tChave criada\n FOI CRIADO NOVO USER"));

			}
			if (oque == REG_OPENED_EXISTING_KEY) {
				_tprintf(TEXT("\tChave aberta\n FOI VALIDADO O USER"));
				++quantos;
			}
		}
		RegSetValueEx(chave, TEXT("Nome"), 0, REG_SZ, utilizador, sizeof(utilizador));
		//RegSetValueEx(chave, TEXT("Score"), 0, REG_DWORD, &utilizador[i].score, sizeof(utilizador[i].score));
	}
	RegCloseKey(chave);
	return quantos;
}