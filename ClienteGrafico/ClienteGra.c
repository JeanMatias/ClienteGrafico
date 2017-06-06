#include <windows.h>
#include <tchar.h>
#include "resource.h"
#include "..\..\SnakeDLL\SnakeDLL\SnakeDLL.h"


/* -----------------------------------------------------*/
/*         #DEFINES LOCAIS                              */
/* -----------------------------------------------------*/
#define MAX				256
#define LIMITE_1POSI	1
#define LIMITE_2POSI	2
#define LIMITE_3POSI	3

/* ===================================================== */
/*		ZONA DE DECLARA��O DE VARIAVEIS E FUN��ES        */
/* ===================================================== */

/* ----------------------------------------------------- */
/*  VARI�VEIS GLOBAIS									 */
/* ----------------------------------------------------- */
int tipoServidor		=	0;			//Se est� ligado a um servidor local ou remoto

//**** Vari�veis de configura��o do Jogo (come�am com o valor por defeito)****
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

//**** Vari�veis de configura��o de Objectos (come�am com o tempo de vida por defeito) ****
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

//**** Vari�veis de configura��o do Jogo ****
int numJogadoresLocal	=	0;				//Num Jogadores a jogar nesta maquina
int valorCobra1			=	0;				//valor da cobra do jogador 1 desta maquina
int valorCobra2			=	0;				//valor da cobra do jogador 2 desta maquina
TCHAR username1[SIZE_USERNAME];				//Nome do Jogador 1 desta Maquina
TCHAR username2[SIZE_USERNAME];				//Nome do Jogador 2 desta Maquina
int pId;									//Process Id deste cliente
int linhas;							
int colunas;
int mapa[MAX_LINHAS][MAX_COLUNAS];


/* ----------------------------------------------------- */
/*  PROTOTIPOS FUN��ES									 */
/* ----------------------------------------------------- */
int chamaCriaJogo(int *valor);
int chamaAssociaJogo(TCHAR username[SIZE_USERNAME], int codigo, int *valor);

/* --------------------------------------------------------- */
/*  PROTOTIPOS FUN��ES DE TRATAMENTO DE EVENTOS DE JANELAS	 */
/* --------------------------------------------------------- */
BOOL CALLBACK IndicaIPRemoto(HWND h, UINT m, WPARAM w, LPARAM l);
BOOL CALLBACK ConfiguraJogo(HWND h, UINT m, WPARAM w, LPARAM l);
BOOL CALLBACK ConfiguraObjectos(HWND h, UINT m, WPARAM w, LPARAM l);
BOOL CALLBACK Pede_NomeJogador1(HWND h, UINT m, WPARAM w, LPARAM l);
BOOL CALLBACK Pede_NomeJogador2(HWND h, UINT m, WPARAM w, LPARAM l);

// ========= FIM ZONA DECLARA��O DE VARIAVEIS E FUN��ES ========= //

/* ===================================================== */
/* Programa base (esqueleto) para aplica��es Windows     */
/* ===================================================== */
				// Cria uma janela de nome "Janela Principal" e pinta fundo de branco
				// Modelo para programas Windows:
				//  Composto por 2 fun��es: 
				//	WinMain()     = Ponto de entrada dos programas windows
				//			1) Define, cria e mostra a janela
				//			2) Loop de recep��o de mensagens provenientes do Windows
				//     TrataEventos()= Processamentos da janela (pode ter outro nome)
				//			1) � chamada pelo Windows (callback) 
				//			2) Executa c�digo em fun��o da mensagem recebida

LRESULT CALLBACK TrataEventos(HWND, UINT, WPARAM, LPARAM);

				// Nome da classe da janela (para programas de uma s� janela, normalmente este nome � 
				// igual ao do pr�prio programa) "szprogName" � usado mais abaixo na defini��o das 
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
HDC			hdc, auxdc;

// ============================================================================
// FUN��O DE IN�CIO DO PROGRAMA: WinMain()
// ============================================================================
				// Em Windows, o programa come�a sempre a sua execu��o na fun��o WinMain()que desempenha // o papel da fun��o main() do C em modo consola WINAPI indica o "tipo da fun��o" (WINAPI // para todas as declaradas nos headers do Windows e CALLBACK para as fun��es de        // processamento da janela)
				// Par�metros:
				//   hInst: Gerado pelo Windows, � o handle (n�mero) da inst�ncia deste programa 
				//   hPrevInst: Gerado pelo Windows, � sempre NULL para o NT (era usado no Windows 3.1)
				//   lpCmdLine: Gerado pelo Windows, � um ponteiro para uma string terminada por 0
				//              destinada a conter par�metros para o programa 
				//   nCmdShow:  Par�metro que especifica o modo de exibi��o da janela (usado em  
				//        	   ShowWindow()
				// ============================================================================
				// WinMain()
				// NOTA: Ver declara��o de HACCEL, LoadAccelerators() e TranslateAccelerator()
				//		 Estas s�o as modifica��es necess�rias para tratar as teclas de atalho
				//		 para op��es do menu
				// ============================================================================

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {

	HWND hWnd;				// hWnd � o handler da janela, gerado mais abaixo por CreateWindow()
	MSG lpMsg;				// MSG � uma estrutura definida no Windows para as mensagens
	WNDCLASSEX wcApp;		// WNDCLASSEX � uma estrutura cujos membros servem para 
							// definir as caracter�sticas da classe da janela
	BOOL ret;
	HACCEL hAccel;			// Handler da resource accelerators (teclas de atalho

	hInstGlobal = hInst;

	pId = GetCurrentProcessId();


	/* ---- Defini��o Pipes ---- */

						// ============================================================================
						// 1. Defini��o das caracter�sticas da janela "wcApp" 
						//    (Valores dos elementos da estrutura "wcApp" do tipo WNDCLASSEX)
						// ============================================================================

	wcApp.cbSize = sizeof(WNDCLASSEX);				// Tamanho da estrutura WNDCLASSEX
	wcApp.hInstance = hInst;						// Inst�ncia da janela actualmente exibida 
													// ("hInst" � par�metro de WinMain e vem 
													// inicializada da�)
	wcApp.lpszClassName = szProgName;				// Nome da janela (neste caso = nome do programa)
	wcApp.lpfnWndProc = TrataEventos;				// Endere�o da fun��o de processamento da janela 	// ("TrataEventos" foi declarada no in�cio e                 // encontra-se mais abaixo)
	wcApp.style = CS_HREDRAW | CS_VREDRAW;			// Estilo da janela: Fazer o redraw se for      // modificada horizontal ou verticalmente
													// CS_VREDRAW
													//wcApp.hIcon = LoadIcon(NULL, IDI_HAND);// "hIcon" = handler do �con normal
	wcApp.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDC_ICON));
													// Icon Grande		// IDI_APPLICATION		  //"NULL" = Icon definido no Windows
													// "IDI_AP..." �cone "aplica��o"
	wcApp.hIconSm = LoadIcon(hInst, IDI_ICON4);		// "hIconSm" = handler do �con pequeno   //   
													// Icon Small		// IDI_INFORMATION			 //"NULL" = Icon definido no Windows
													// "IDI_INF..." �con de informa��o
	wcApp.hCursor = LoadCursor(NULL, IDC_ARROW);		// "hCursor" = handler do cursor (rato) 
													// IDC_ARROW				// "NULL" = Forma definida no Windows
													// "IDC_ARROW" Aspecto "seta" 
	wcApp.lpszMenuName = (LPCTSTR)(IDR_MENU1);
	//wcApp.lpszMenuName = NULL;					// Classe do menu que a janela pode ter
													// (NULL = n�o tem menu)
	wcApp.cbClsExtra = 0;							// Livre, para uso particular
	wcApp.cbWndExtra = 0;							// Livre, para uso particular
	wcApp.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
													// WHITE_BRUSH
													// "hbrBackground" = handler para "brush" de pintura do fundo da janela. Devolvido por  // "GetStockObject".Neste caso o fundo ser� branco
	
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
		TEXT("Snake Multiplayer"),					// Texto que figura na barra do t�tulo
		// WS_VSCROLL |								// Adicinar Scrool Bar se necess�rio		
		WS_OVERLAPPEDWINDOW,						// Estilo da janela (WS_OVERLAPPED= normal)
		50,								//CW_USEDEFAULT,
													// Posi��o x pixels (default=� direita da �ltima)
		50,								// Posi��o y pixels (default=abaixo da �ltima)
		//CW_USEDEFAULT,		
		800,										// Largura da janela (em pixels)
		//CW_USEDEFAULT,		
		600,										// Altura da janela (em pixels)
		//CW_USEDEFAULT,		
		(HWND)HWND_DESKTOP,							// handle da janela pai (se se criar uma a partir de
													// outra) ou HWND_DESKTOP se a janela for a primeira, 
													// criada a partir do "desktop"
		(HMENU)hMenu,								// handle do menu da janela (se tiver menu)
		(HINSTANCE)hInst,							// handle da inst�ncia do programa actual ("hInst" � 
													// passado num dos par�metros de WinMain()
		0);											// N�o h� par�metros adicionais para a janela

	/* ============================================================================ */
	/* Carregar as defini��es das teclas aceleradoras (atalhos de op��es dos Menus) */
	/* LoadAccelerators(inst�ncia_programa, ID_da_resource_atalhos)				    */
	/* ============================================================================ */

	hAccel = LoadAccelerators(hInst, (LPCWSTR)IDR_ACCELERATOR1);

	/* ============================================================================ */
	/*				4. Mostrar a janela												*/
	/* ============================================================================ */

	ShowWindow(hWnd, nCmdShow);			// "hWnd"= handler da janela, devolvido por 
										// "CreateWindow"; "nCmdShow"= modo de exibi��o (p.e. 
										// normal/modal); � passado como par�metro de WinMain()
			UpdateWindow(hWnd);			// Refrescar a janela (Windows envia � janela uma 
										// mensagem para pintar, mostrar dados, (refrescar)� 
										// ============================================================================
										// 5. Loop de Mensagens
										// ============================================================================
										// O Windows envia mensagens �s janelas (programas). Estas mensagens ficam numa fila de
										// espera at� que GetMessage(...) possa ler "a mensagem seguinte"	
										// Par�metros de "getMessage":
										// 1)"&lpMsg"=Endere�o de uma estrutura do tipo MSG ("MSG lpMsg" ja foi declarada no  
										//   in�cio de WinMain()):
										//			HWND hwnd		handler da janela a que se destina a mensagem
										//			UINT message		Identificador da mensagem
										//			WPARAM wParam		Par�metro, p.e. c�digo da tecla premida
										//			LPARAM lParam		Par�metro, p.e. se ALT tamb�m estava premida
										//			DWORD time		Hora a que a mensagem foi enviada pelo Windows
										//			POINT pt		Localiza��o do mouse (x, y) 
										// 2)handle da window para a qual se pretendem receber mensagens (=NULL se se pretendem //   receber as mensagens para todas as janelas pertencentes � thread actual)
										// 3)C�digo limite inferior das mensagens que se pretendem receber
										// 4)C�digo limite superior das mensagens que se pretendem receber

										// NOTA: GetMessage() devolve 0 quando for recebida a mensagem de fecho da janela,
										// 	  terminando ent�o o loop de recep��o de mensagens, e o programa 

							// ### ALERTA ===>>>>>
							// ### ESTA CONSTANTEMENTE A RECEBER OS EVENTOS ***************************************************************
	
							/* ============================================================================ */
							/* Loop de Mensagens															*/	
							/* Para usar as teclas aceleradoras do menu � necess�rio chamar a fun��o		*/
							/* TranslateAccelerator(handler_janela, handler_accelerators, pont_mensagem)	*/
							/* Se esta fun��o devolver falso, n�o foi premida tecla de acelera��o			*/
							/* ============================================================================ */

	while ((ret = GetMessage(&lpMsg, NULL, 0, 0)) != 0) {
		if (ret != -1) {
			if (!TranslateAccelerator(hWnd, hAccel, &lpMsg)) {
				TranslateMessage(&lpMsg);	// Pr�-processamento da mensagem (p.e. obter c�digo 
											// ASCII da tecla premida)
				DispatchMessage(&lpMsg);	// Enviar a mensagem traduzida de volta ao Windows, que
											// aguarda at� que a possa reenviar � fun��o de 
											// tratamento da janela, CALLBACK TrataEventos (abaixo)
			}
		}
	}

	/* ============================================================================ */
	/*						6. Fim do programa										*/
	/* ============================================================================ */

	return((int)lpMsg.wParam);				// Retorna sempre o par�metro wParam da estrutura lpMsg
}
							// ============================================================================
							// FUN��O DE PROCESSAMENTO DA JANELA
							// Esta fun��o pode ter um nome qualquer: Apenas � neces�rio que na inicializa��o da     // estrutura "wcApp", feita no in�cio de WinMain(), se identifique essa fun��o. Neste   // caso "wcApp.lpfnWndProc = WndProc"
							//
							// WndProc recebe as mensagens enviadas pelo Windows (depois de lidas e pr�-processadas // no loop "while" da fun��o WinMain()
							//
							// Par�metros:
							//		hWnd	O handler da janela, obtido no CreateWindow()
							//		messg	Ponteiro para a estrutura mensagem (ver estrutura em 5. Loop...
							//		wParam	O par�metro wParam da estrutura messg (a mensagem)
							//		lParam	O par�metro lParam desta mesma estrutura
							//
							// NOTA:Estes par�metros est�o aqui acess�veis o que simplifica o acesso aos seus valores
							//
							// A fun��o EndProc � sempre do tipo "switch..." com "cases" que descriminam a mensagem // recebida e a tratar. Estas mensagens s�o identificadas por constantes (p.e. 
							// WM_DESTROY, WM_CHAR, WM_KEYDOWN, WM_PAINT...) definidas em windows.h ============================================================================


/* ============================================================================ */
/*					FUN��ES CALLBACK											*/
/* ============================================================================ */

/* ----------------------------------------------------- */
/*				FUN��O MENU PARA IP DO REMOTO			 */
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
	//N�o foi tratado o evento
	return 0;
}

/* ----------------------------------------------------- */
/*				FUN��O MENU PARA CONFIGURAR JOGO		 */
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
					MessageBox(h, TEXT("LINHAS: M�ximo 40 E Minimo 10 \n\nCOLUNAS: M�ximo 80 E Minimo 10"), TEXT("ATEN��O"), MB_OK);
				}
				else {
					colunasConfig = aux1;
					linhasConfig = aux2;
				}
			aux3 = GetDlgItemInt(h, IDC_EDIT3, NULL, TRUE);
				if (aux3 < TAMANHOSNAKE || aux3 > 6)
					MessageBox(h, TEXT("Tamanho Configur�vel das Cobras: Compreendido entre 3 e 6"), TEXT("ATEN��O"), MB_OK);
				else{
					tamanhoSnakes = aux3;			
				}
			aux4 = GetDlgItemInt(h, IDC_EDIT4, NULL, TRUE);
				if (aux4 < NUMAUTOSNAKE || aux4 > 6)
					MessageBox(h, TEXT("N�mero Cobras Autom�ticas: Compreendido entre 1 e 6"), TEXT("ATEN��O"), MB_OK);
				else {
					cobrasAutomaticas = aux4;
				}
			aux5 = GetDlgItemInt(h, IDC_EDIT5, NULL, TRUE);
				if (aux5 > MAXOBJECTOS)
					MessageBox(h, TEXT("N�mero M�ximo de Objectos: Limite � 30"), TEXT("ATEN��O"), MB_OK);
				else {
					numObjectos = aux5;
				}
			aux6 = GetDlgItemInt(h, IDC_EDIT6, NULL, TRUE);
				if (aux6 < NUMJOGADORES || aux6 > MAXJOGADORES)
					MessageBox(h, TEXT("N�mero de Jogadores: Compreendido entre 1 e 4"), TEXT("ATEN��O"), MB_OK);
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
			SetDlgItemInt(h, IDC_EDIT4, NUMAUTOSNAKE, TRUE);					// N� Cobras Autom�ticas
		SendDlgItemMessage(h, IDC_EDIT5, EM_LIMITTEXT, LIMITE_2POSI, NULL);
			SetDlgItemInt(h, IDC_EDIT5, NUMOBJETOS, TRUE);						// N� de objectos
		SendDlgItemMessage(h, IDC_EDIT6, EM_LIMITTEXT, LIMITE_1POSI, NULL);
			SetDlgItemInt(h, IDC_EDIT6, NUMJOGADORES, TRUE);					// N� Max Jogadores
		return 1;		
	}
	//N�o foi tratado o evento
	return 0;
}

/* ----------------------------------------------------- */
/*				FUN��O MENU PARA CONFIGURAR OBJECTOS	 */
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
					MessageBox(h, TEXT("VODKA: Compreendido entre 4 e 30"), TEXT("ATEN��O"), MB_OK);
				}
				else {
					vodka = aux4;
				}
			aux5 = GetDlgItemInt(h, IDC_EDIT17, NULL, TRUE);
				if (aux5 < OLEO || aux5 > 30) {
					MessageBox(h, TEXT("OLEO: Compreendido entre 5 e 30"), TEXT("ATEN��O"), MB_OK);
				}
				else {
					oleo = aux5;
				}
			aux6 = GetDlgItemInt(h, IDC_EDIT18, NULL, TRUE);
				if (aux6 < COLA || aux6 > 30) {
					MessageBox(h, TEXT("COLA: Compreendido entre 6 e 30"), TEXT("ATEN��O"), MB_OK);
				}
				else {
					cola = aux6;
				}
			aux7 = GetDlgItemInt(h, IDC_EDIT19, NULL, TRUE);
				if (aux7 < O_OLEO || aux7 > 30) {
					MessageBox(h, TEXT("O_OLEO: Compreendido entre 8 e 30"), TEXT("ATEN��O"), MB_OK);
				}
				else {
					o_oleo = aux7;
				}
			aux8 = GetDlgItemInt(h, IDC_EDIT20, NULL, TRUE);
				if (aux8 < O_COLA || aux8 > 30) {
					MessageBox(h, TEXT("O_COLA: Compreendido entre 9 e 30"), TEXT("ATEN��O"), MB_OK);
				}
				else {
					o_cola = aux8;
				}
			aux9 = GetDlgItemInt(h, IDC_EDIT21, NULL, TRUE);
				if (aux9 < O_VODKA || aux9 > 30) {
					MessageBox(h, TEXT("O_VODKA: Compreendido entre 7 e 30"), TEXT("ATEN��O"), MB_OK);
				}
				else {
					o_vodka = aux9;
				}
			aux10 = GetDlgItemInt(h, IDC_EDIT1, NULL, TRUE);
				if (aux10 < SURPRESA || aux10 > 30) {
					MessageBox(h, TEXT("SURPRESA: Compreendido entre 10 e 30"), TEXT("ATEN��O"), MB_OK);
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
	//N�o foi tratado o evento
	return 0;
}

/* ----------------------------------------------------- */
/*		FUN��O CALLBACK PARA PEDIR NOME JOGADOR 1		 */
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
/*		FUN��O CALLBACK PARA PEDIR NOME JOGADOR 2		 */
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

// ****************  FIM DAS FUN��ES CALLBACK GERAIS *************************************

/* ----------------------------------------------------- */
/*	FUN��O CALLBACK BASE DA APLICA��O / TRATA TECLADO    */
/* ----------------------------------------------------- */

LRESULT CALLBACK TrataEventos(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	HDC device;
	hMenu = GetMenu(hWnd);							// Para obter o Handle do Menu
	PAINTSTRUCT pintar;
	RECT area;
	static HPEN linha;
	static HBITMAP vodka, mouse, glue, comida;
	static HBITMAP cobra1cab1, parede;
	static HBRUSH fundo, fundo1, fundo2;
	int resposta, valor;

	switch (messg) {
		case WM_CREATE:
									cobra1cab1	=	LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_PNG7));
									parede		=	LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_PNG4));

						break;
		case WM_CLOSE:				if (MessageBox(hWnd, TEXT("Quer mesmo sair?"), TEXT("Snake Multiplayer."), MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
									PostQuitMessage(0);
						break;

		case WM_COMMAND:			// Para configura��o dos parametros de TABULEIRO
									// Sem PAI fica n�o bloqueante, pode ficar NULL ou Handle do Pai para bloquear
			switch (wParam)
					{
					case ID_SERVIDOR_REMOTO:	DialogBox(hInstGlobal, IDD_DIALOG3, hWnd, IndicaIPRemoto);
												EnableMenuItem(hMenu, ID_JOGO_ASSOCIAR, MF_ENABLED);
												EnableMenuItem(hMenu, ID_JOGO_CRIAR, MF_ENABLED);
												EnableMenuItem(hMenu, ID_JOGO_JOGAR, MF_ENABLED);
									break;
					case ID_SERVIDOR_LOCAL:		tipoServidor = LOCAL;
												preparaMemoriaPartilhada();
												pede_RegistarClienteLocal(pId);
												preparaMemoriaPartilhadaResposta(pId);
												WaitForSingleObject(hEventoResposta, INFINITE);
												ResetEvent(hEventoResposta);
												if (vistaResposta->resposta == SUCESSO) {
													MessageBox(hWnd, TEXT("Ligado a servidor Local"), TEXT("SUCESSO"), MB_OK);
													EnableMenuItem(hMenu, ID_JOGO_ASSOCIAR, MF_ENABLED);
													EnableMenuItem(hMenu, ID_JOGO_CRIAR, MF_ENABLED);
													EnableMenuItem(hMenu, ID_JOGO_JOGAR, MF_ENABLED);
												}
												else if (vistaResposta->resposta == INSUCESSO) {
													MessageBox(hWnd, TEXT("N�o foi possivel ligar ao servidor, contacte o utilizador da maquina"), TEXT("INSUCESSO"), MB_OK);
												}
												
									break;
					case ID_JOGO_CRIAR:			DialogBox(hInstGlobal, IDD_DIALOG5, hWnd, Pede_NomeJogador1);
												resposta = chamaCriaJogo(&valor);
												if (resposta == SUCESSO) {
													MessageBox(hWnd, TEXT("Jogo Criado"), TEXT("SUCESSO"), MB_OK);
													valorCobra1 = valor;
												}
												else if (resposta == INSUCESSO) {
													MessageBox(hWnd, TEXT("N�o � possivel criar jogo neste momento"), TEXT("INSUCESSO"), MB_OK);
												}
									break;
					case ID_JOGO_ASSOCIAR:		if (numJogadoresLocal == 0) {
													DialogBox(hInstGlobal, IDD_DIALOG5, hWnd, Pede_NomeJogador1);
													resposta = 	chamaAssociaJogo(username1, ASSOCIAR_JOGADOR1,&valor);
													if (resposta == SUCESSO) {
														MessageBox(hWnd, TEXT("Jogador 1 Associado"), TEXT("SUCESSO"), MB_OK);
														valorCobra1 = valor;
														numJogadoresLocal++;
													}
													else if (resposta == INSUCESSO) {
														if (valor == AGORANAO) {
															MessageBox(hWnd, TEXT("N�o � possivel associar ao jogo neste momento"), TEXT("INSUCESSO"), MB_OK);
														}
														else if (valor == JOGOCHEIO) {
															MessageBox(hWnd, TEXT("N�o existem mais vagas no jogo"), TEXT("INSUCESSO"), MB_OK);
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
															MessageBox(hWnd, TEXT("N�o � possivel associar ao jogo neste momento"), TEXT("INSUCESSO"), MB_OK);
														}
														else if (valor == JOGOCHEIO) {
															MessageBox(hWnd, TEXT("N�o existem mais vagas no jogo"), TEXT("INSUCESSO"), MB_OK);
														}
													}
												}
									break;
					case ID_JOGO_JOGAR:			resposta = chamaIniciaJogo(&valor);
												if (resposta == SUCESSO) {
													MessageBox(hWnd, TEXT("Jogo Iniciado"), TEXT("SUCESSO"), MB_OK);//lan�ar as threads de actualiza��o do mapa
												}
												else if (resposta == INSUCESSO) {
													if (valor == CRIADORERRADO) {
														MessageBox(hWnd, TEXT("Apenas o cliente que criou o jogo pode iniciar o mesmo!"), TEXT("INSUCESSO"), MB_OK);
													}
													if (valor == AGORANAO) {
														MessageBox(hWnd, TEXT("Deve iniciar o jogo s� depois de criar o mesmo!"), TEXT("INSUCESSO"), MB_OK);
													}
												}						
									break;		
					case ID_CONFIGURAR_TABULEIRO:	DialogBox(hInstGlobal, IDD_DIALOG1, hWnd, ConfiguraJogo);
			
									break;
					case ID_CONFIGURAR_OBJECTOS:	DialogBox(hInstGlobal, IDD_DIALOG4, hWnd, ConfiguraObjectos);
			
									break;
					case ID_EDITARJPGS:			if (CreateProcess(NULL, executavel, NULL, NULL, 0, 0, NULL, NULL, &si, &pi))
														return 1;
												   else
														return 0;
									break;
					case ID_SAIR40011:			if (MessageBox(hWnd, TEXT("Quer mesmo sair?"), TEXT("Snake Multiplayer."), MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
												PostQuitMessage(0);
									break;
					case ID_INFORMA40018:		MessageBoxA(hWnd, "\n\tSO2\n\n\nTrabalho Pr�tico 2016/2017\n\nPaulo Alves - Aluno n� 21170449\n\t&\nJean Matias - Aluno n� 21200943 \n", "Snake Multiplayer", MB_OK);
			
									break;
					case ID_AJUDA_AJUDA:		MessageBoxA(hWnd, "\n\tSO2\n\n\n 1� Deve escolher se joga localmente\n ou liga a computador remoto. \n", "Snake Multiplayer", MB_OK);
			
									break;
					}
		/*case WM_PAINT:
				hdc = BeginPaint(hWnd, &pintar);

				auxdc = CreateCompatibleDC(hdc);

				//  Parede
				SelectObject(auxdc, parede);
				for (int i = 0; i < 20; i++)
					for (int j = 0; j < 20; j++)
					BitBlt(hdc, j * 20, i * 20, 28, 28, auxdc, 0, 0, SRCCOPY);

				SelectObject(auxdc, cobra1cab1);
				for (int i = 0; i < 20; i++)
					for (int j = 0; j < 20; j++)
						BitBlt(hdc, j * 20, i * 20, 28, 28, auxdc, 0, 0, SRCCOPY);

				//ReleaseDC(hWnd, hdc);
				//InvalidateRect(hWnd, NULL, 1);

				DeleteDC(auxdc);
				EndPaint(hWnd, hdc);
				EndPaint(hWnd, &pintar);
				break;*/
		case WM_KEYUP:

				break;
						// TRATANDO O WM_CLOSE n�o � necess�rio tartar o WM_DESTROY  <<============  NOTA
						//case WM_DESTROY:	// Destruir a janela e terminar o programa 
						//					// "PostQuitMessage(Exit Status)"	
						//		PostQuitMessage(0);
						//	break;
		default:
						// Neste exemplo, para qualquer outra mensagem (p.e. "minimizar","maximizar","restaurar") // n�o � efectuado nenhum processamento, apenas se segue o "default" do Windows			
		return(DefWindowProc(hWnd, messg, wParam, lParam));
	}
	return(0);
}

/* ----------------------------------------------------- */
/*	    FUN��ES DE CHAMADA DE FUN��ES DE PEDIDOS         */
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
			//ler Resposta e s� depois validar
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
			//ler Resposta e s� depois validar
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
		//ler Resposta e s� depois validar
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

