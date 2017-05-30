#include <windows.h>
#include <tchar.h>
#include "resource.h"
#define MAX 256

#include "..\..\SnakeDLL\SnakeDLL\SnakeDLL.h"


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

// ************************   DECLARA��ES  ************************************
BOOL CALLBACK IndicaIPRemoto(HWND h, UINT m, WPARAM w, LPARAM l);
BOOL CALLBACK ConfiguraJogo(HWND h, UINT m, WPARAM w, LPARAM l);
BOOL CALLBACK ConfiguraObjectos(HWND h, UINT m, WPARAM w, LPARAM l);


TCHAR *szProgName = TEXT("Snake");
TCHAR executavel[MAX] = TEXT("mspaint.exe");
//TCHAR executavel[MAX] = TEXT("%windir%\system32\mspaint.exe");

PROCESS_INFORMATION pi;
STARTUPINFO si;

// *************************  HANDLES GLOBAIS ********************

HINSTANCE hInstGlobal;

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
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
	HWND hWnd;		// hWnd � o handler da janela, gerado mais abaixo por CreateWindow()
	MSG lpMsg;		// MSG � uma estrutura definida no Windows para as mensagens
	WNDCLASSEX wcApp;	// WNDCLASSEX � uma estrutura cujos membros servem para 
	BOOL ret;			// definir as caracter�sticas da classe da janela
						// ============================================================================
						// 1. Defini��o das caracter�sticas da janela "wcApp" 
						//    (Valores dos elementos da estrutura "wcApp" do tipo WNDCLASSEX)
						// ============================================================================
	wcApp.cbSize = sizeof(WNDCLASSEX);	// Tamanho da estrutura WNDCLASSEX
	wcApp.hInstance = hInst;			// Inst�ncia da janela actualmente exibida 
										// ("hInst" � par�metro de WinMain e vem 
										// inicializada da�)
	wcApp.lpszClassName = szProgName;	// Nome da janela (neste caso = nome do programa)
	wcApp.lpfnWndProc = TrataEventos;	// Endere�o da fun��o de processamento da janela 	// ("TrataEventos" foi declarada no in�cio e                 // encontra-se mais abaixo)
	wcApp.style = CS_HREDRAW | CS_VREDRAW;// Estilo da janela: Fazer o redraw se for      // modificada horizontal ou verticalmente
										  // CS_VREDRAW
	wcApp.hIcon = LoadIcon(NULL, IDI_HAND);// "hIcon" = handler do �con normal
											// Icon Grande		// IDI_APPLICATION		  //"NULL" = Icon definido no Windows
											// "IDI_AP..." �cone "aplica��o"
	wcApp.hIconSm = LoadIcon(NULL, IDI_WINLOGO);// "hIconSm" = handler do �con pequeno   //   
												// Icon Small		// IDI_INFORMATION			 //"NULL" = Icon definido no Windows
												// "IDI_INF..." �con de informa��o
	wcApp.hCursor = LoadCursor(NULL, IDC_HAND);	// "hCursor" = handler do cursor (rato) 
												// IDC_ARROW				// "NULL" = Forma definida no Windows
												// "IDC_ARROW" Aspecto "seta" 
	wcApp.lpszMenuName =(IDR_MENU1);
	//wcApp.lpszMenuName = NULL;			// Classe do menu que a janela pode ter
										// (NULL = n�o tem menu)
	wcApp.cbClsExtra = 0;				// Livre, para uso particular
	wcApp.cbWndExtra = 0;				// Livre, para uso particular
	wcApp.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	// WHITE_BRUSH
	// "hbrBackground" = handler para "brush" de pintura do fundo da janela. Devolvido por  // "GetStockObject".Neste caso o fundo ser� branco
	// ============================================================================
	// 2. Registar a classe "wcApp" no Windows
	// ============================================================================
	if (!RegisterClassEx(&wcApp))
		return(0);
	// ============================================================================
	// 3. Criar a janela
	// ============================================================================
	hWnd = CreateWindow(
		szProgName,			// Nome da janela (programa) definido acima
		TEXT("Snake Multiplayer"),// Texto que figura na barra do t�tulo
		WS_OVERLAPPEDWINDOW,	// Estilo da janela (WS_OVERLAPPED= normal)
		CW_USEDEFAULT,
		//CW_USEDEFAULT,		// Posi��o x pixels (default=� direita da �ltima)
		CW_USEDEFAULT,
		//CW_USEDEFAULT,		// Posi��o y pixels (default=abaixo da �ltima)
		800,
		//CW_USEDEFAULT,		// Largura da janela (em pixels)
		600,
		//CW_USEDEFAULT,		// Altura da janela (em pixels)
		(HWND)HWND_DESKTOP,	// handle da janela pai (se se criar uma a partir de
							// outra) ou HWND_DESKTOP se a janela for a primeira, 
							// criada a partir do "desktop"
		(HMENU)NULL,			// handle do menu da janela (se tiver menu)
		(HINSTANCE)hInst,		// handle da inst�ncia do programa actual ("hInst" � 
								// passado num dos par�metros de WinMain()
		0);				// N�o h� par�metros adicionais para a janela
						// ============================================================================
						// 4. Mostrar a janela
						// ============================================================================
	ShowWindow(hWnd, nCmdShow);	// "hWnd"= handler da janela, devolvido por 
								// "CreateWindow"; "nCmdShow"= modo de exibi��o (p.e. 
								// normal/modal); � passado como par�metro de WinMain()
	UpdateWindow(hWnd);		// Refrescar a janela (Windows envia � janela uma 
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

	while ((ret = GetMessage(&lpMsg, NULL, 0, 0)) != 0) {
		if (ret != -1) {
			TranslateMessage(&lpMsg);	// Pr�-processamento da mensagem (p.e. obter c�digo 
										// ASCII da tecla premida)
			DispatchMessage(&lpMsg);	// Enviar a mensagem traduzida de volta ao Windows, que
										// aguarda at� que a possa reenviar � fun��o de 
										// tratamento da janela, CALLBACK TrataEventos (abaixo)
		}
	}
	// ============================================================================
	// 6. Fim do programa
	// ============================================================================
	return((int)lpMsg.wParam);	// Retorna sempre o par�metro wParam da estrutura lpMsg
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

//  **************************************************************************************
//  *************** FUN��ES COOLBACK  ****************************************************
//  **************************************************************************************

//  *************** FUN��O MENU PARA IP DO REMOTO ****************************************
BOOL CALLBACK IndicaIPRemoto(HWND h, UINT m, WPARAM w, LPARAM l) {
	switch (m) {
	case WM_COMMAND:
		switch (LOWORD(w))
		{
		case IDCANCEL:
			EndDialog(h, 0);
			//return 1;
		case IDOK:
			/*GetDlgItemText(h, IDC_EDIT1, login, sizeof(login));
			GetDlgItemText(h, IDC_EDIT2, password, sizeof(password));
			SendDlgItemMessage(h, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)login);
			if (_tcslen(login) > 0 && _tcslen(password) > 0)
			MessageBox(h, login, password, MB_OK);
			else
			SetDlgItemText(h, IDC_LIST1, TEXT("Introduzir Login/password v�lido"));
			return 1;*/

		case WM_INITDIALOG:

			return 1;
		}
	case WM_CLOSE:
		EndDialog(h, 0);
		return 1;
	}
	//N�o foi tratado o evento
	return 0;
}


//  *************** FUN��O MENU PARA CONFIGURAR JOGO *************************************
BOOL CALLBACK ConfiguraJogo(HWND h, UINT m, WPARAM w, LPARAM l) {

	switch (m) {
	case WM_COMMAND:
		switch (LOWORD(w))
		{
		case IDCANCEL:
			EndDialog(h, 0);
			//return 1;
		case IDOK:
			/*GetDlgItemText(h, IDC_EDIT1, login, sizeof(login));
			GetDlgItemText(h, IDC_EDIT2, password, sizeof(password));
			SendDlgItemMessage(h, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)login);
			if (_tcslen(login) > 0 && _tcslen(password) > 0)
				MessageBox(h, login, password, MB_OK);
			else
				SetDlgItemText(h, IDC_LIST1, TEXT("Introduzir Login/password v�lido"));
			return 1;*/

		case WM_INITDIALOG:

			return 1;
		}
	case WM_CLOSE:
		EndDialog(h, 0);
		return 1;
	}
	//N�o foi tratado o evento
	return 0;
}
//  *************** FUN��O MENU PARA CONFIGURAR OBJECTOS **********************************
BOOL CALLBACK ConfiguraObjectos(HWND h, UINT m, WPARAM w, LPARAM l) {

	switch (m) {
	case WM_COMMAND:
		switch (LOWORD(w))
		{
		case IDCANCEL:
			EndDialog(h, 0);
			//return 1;
		case IDOK:
			/*GetDlgItemText(h, IDC_EDIT1, login, sizeof(login));
			GetDlgItemText(h, IDC_EDIT2, password, sizeof(password));
			SendDlgItemMessage(h, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)login);
			if (_tcslen(login) > 0 && _tcslen(password) > 0)
			MessageBox(h, login, password, MB_OK);
			else
			SetDlgItemText(h, IDC_LIST1, TEXT("Introduzir Login/password v�lido"));
			return 1;*/

		case WM_INITDIALOG:

			return 1;
		}
	case WM_CLOSE:
		EndDialog(h, 0);
		return 1;
	}
	//N�o foi tratado o evento
	return 0;
}
// ****************  FIM DAS FUN��ES COOLBACK GERAIS *************************************

//  *************** FUN��O COOLBACK BASE DA APLICA��O / TRATA TECLADO ********************
LRESULT CALLBACK TrataEventos(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	int retorno, x, y;
	static int xini, yini;
	int xfin, yfin;
	HDC device;
	TCHAR str[100];
	
	switch (messg) {
	case WM_CLOSE:
		if (MessageBox(hWnd, TEXT("Quer mesmo sair?"), TEXT("Snake Multiplayer."), MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			PostQuitMessage(0);
		break;

	case WM_COMMAND:    // Para configura��o dos parametros de TABULEIRO
						// Sem PAI fica n�o bloqueante, pode ficar NULL ou Handle do Pai para bloquear
		if (wParam == ID_SERVIDOR_REMOTO)
			DialogBox(hInstGlobal, IDD_DIALOG3, hWnd, IndicaIPRemoto);
		else
		if (wParam == ID_CONFIGURAR_TABULEIRO)
				DialogBox(hInstGlobal, IDD_DIALOG1, hWnd, ConfiguraJogo);
		else
		if (wParam == ID_CONFIGURAR_OBJECTOS)
				DialogBox(hInstGlobal, IDD_DIALOG4, hWnd, ConfiguraObjectos);
		else
			if (wParam == ID_EDITARJPGS) {
				if (CreateProcess(NULL, executavel, NULL, NULL, 0, CREATE_NEW, NULL, NULL, &si, &pi))
					return 1;
				else
					return 0;
			}
		else
		if (wParam == ID_SAIR40011) {
				if (MessageBox(hWnd, TEXT("Quer mesmo sair?"), TEXT("Snake Multiplayer."), MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
					PostQuitMessage(0);
				break;
			}

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
