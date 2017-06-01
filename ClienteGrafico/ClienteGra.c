#include <windows.h>
#include <tchar.h>
#include "resource.h"
#define MAX 256

#include "..\..\SnakeDLL\SnakeDLL\SnakeDLL.h"

// ========================================================== //
//			ZONA DE DECLARAÇÃO DE VARIAVEIS E FUNÇÕES         //
// ========================================================== //

/* ----------------------------------------------------- */
/*  VARIÁVEIS GLOBAIS									 */
/* ----------------------------------------------------- */
int tipoServidor=0;//Se está ligado a um servidor local ou remoto

//****Variáveis de configuração do Jogo (começam com o valor por defeito)****
int cobrasAutomaticas = NUMAUTOSNAKE;
int colunasConfig = COLUNAS;
int linhasConfig = LINHAS;
int numJogadores = NUMJOGADORES;
int numObjectos = NUMOBJETOS;
int tamanhoSnakes = TAMANHOSNAKE;
//****Variáveis de configuração do Jogo****
int numJogadoresLocal = 0;				//Num Jogadores a jogar nesta maquina
int valorCobra1 = 0;				//valor da cobra do jogador 1 desta maquina
int valorCobra2 = 0;				//valor da cobra do jogador 2 desta maquina
TCHAR username1[SIZE_USERNAME];		//Nome do Jogador 1 desta Maquina
TCHAR username2[SIZE_USERNAME];		//Nome do Jogador 2 desta Maquina
int pId;							//Process Id deste cliente
int linhas;							
int colunas;
int mapa[MAX_LINHAS][MAX_COLUNAS];

/* ----------------------------------------------------- */
/*  PROTOTIPOS FUNÇÕES									 */
/* ----------------------------------------------------- */
int chamaCriaJogo(void);
int chamaAssociaJogo(TCHAR username[SIZE_USERNAME], int codigo);

/* --------------------------------------------------------- */
/*  PROTOTIPOS FUNÇÕES DE TRATAMENTO DE EVENTOS DE JANELAS	 */
/* --------------------------------------------------------- */
BOOL CALLBACK IndicaIPRemoto(HWND h, UINT m, WPARAM w, LPARAM l);
BOOL CALLBACK ConfiguraJogo(HWND h, UINT m, WPARAM w, LPARAM l);
BOOL CALLBACK ConfiguraObjectos(HWND h, UINT m, WPARAM w, LPARAM l);
BOOL CALLBACK Pede_NomeJogador1(HWND h, UINT m, WPARAM w, LPARAM l);
BOOL CALLBACK Pede_NomeJogador2(HWND h, UINT m, WPARAM w, LPARAM l);

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

// *************************  HANDLES GLOBAIS ********************

HINSTANCE hInstGlobal;

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
	HWND hWnd;		// hWnd é o handler da janela, gerado mais abaixo por CreateWindow()
	MSG lpMsg;		// MSG é uma estrutura definida no Windows para as mensagens
	WNDCLASSEX wcApp;	// WNDCLASSEX é uma estrutura cujos membros servem para 
	BOOL ret;			// definir as características da classe da janela
	HACCEL hAccel;		// Handler da resource accelerators (teclas de atalho

	hInstGlobal = hInst;

	pId = GetCurrentProcessId();


	/* ---- Definição Pipes ---- */

						// ============================================================================
						// 1. Definição das características da janela "wcApp" 
						//    (Valores dos elementos da estrutura "wcApp" do tipo WNDCLASSEX)
						// ============================================================================
	wcApp.cbSize = sizeof(WNDCLASSEX);	// Tamanho da estrutura WNDCLASSEX
	wcApp.hInstance = hInst;			// Instância da janela actualmente exibida 
										// ("hInst" é parâmetro de WinMain e vem 
										// inicializada daí)
	wcApp.lpszClassName = szProgName;	// Nome da janela (neste caso = nome do programa)
	wcApp.lpfnWndProc = TrataEventos;	// Endereço da função de processamento da janela 	// ("TrataEventos" foi declarada no início e                 // encontra-se mais abaixo)
	wcApp.style = CS_HREDRAW | CS_VREDRAW;// Estilo da janela: Fazer o redraw se for      // modificada horizontal ou verticalmente
										  // CS_VREDRAW
	//wcApp.hIcon = LoadIcon(NULL, IDI_HAND);// "hIcon" = handler do ícon normal
	wcApp.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDC_ICON));
											// Icon Grande		// IDI_APPLICATION		  //"NULL" = Icon definido no Windows
											// "IDI_AP..." Ícone "aplicação"
	wcApp.hIconSm = LoadIcon(NULL, IDI_WINLOGO);// "hIconSm" = handler do ícon pequeno   //   
												// Icon Small		// IDI_INFORMATION			 //"NULL" = Icon definido no Windows
												// "IDI_INF..." Ícon de informação
	wcApp.hCursor = LoadCursor(NULL, IDC_HAND);	// "hCursor" = handler do cursor (rato) 
												// IDC_ARROW				// "NULL" = Forma definida no Windows
												// "IDC_ARROW" Aspecto "seta" 
	wcApp.lpszMenuName =(IDR_MENU1);
	//wcApp.lpszMenuName = NULL;			// Classe do menu que a janela pode ter
										// (NULL = não tem menu)
	wcApp.cbClsExtra = 0;				// Livre, para uso particular
	wcApp.cbWndExtra = 0;				// Livre, para uso particular
	wcApp.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	// WHITE_BRUSH
	// "hbrBackground" = handler para "brush" de pintura do fundo da janela. Devolvido por  // "GetStockObject".Neste caso o fundo será branco
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
		TEXT("Snake Multiplayer"),// Texto que figura na barra do título
		// WS_VSCROLL |           // Adicinar Scrool Bar se necessário
		
		WS_OVERLAPPEDWINDOW,	// Estilo da janela (WS_OVERLAPPED= normal)
		CW_USEDEFAULT,
		//CW_USEDEFAULT,		// Posição x pixels (default=à direita da última)
		CW_USEDEFAULT,
		//CW_USEDEFAULT,		// Posição y pixels (default=abaixo da última)
		800,
		//CW_USEDEFAULT,		// Largura da janela (em pixels)
		600,
		//CW_USEDEFAULT,		// Altura da janela (em pixels)
		(HWND)HWND_DESKTOP,	// handle da janela pai (se se criar uma a partir de
							// outra) ou HWND_DESKTOP se a janela for a primeira, 
							// criada a partir do "desktop"
		(HMENU)NULL,			// handle do menu da janela (se tiver menu)
		(HINSTANCE)hInst,		// handle da instância do programa actual ("hInst" é 
								// passado num dos parâmetros de WinMain()
		0);				// Não há parâmetros adicionais para a janela
	// ============================================================================
	// Carregar as definições das teclas aceleradoras (atalhos de opções dos Menus)
	// LoadAccelerators(instância_programa, ID_da_resource_atalhos)
	// ============================================================================
	hAccel = LoadAccelerators(hInst, (LPCWSTR)IDR_ACCELERATOR1);
						// ============================================================================
						// 4. Mostrar a janela
						// ============================================================================
	ShowWindow(hWnd, nCmdShow);	// "hWnd"= handler da janela, devolvido por 
								// "CreateWindow"; "nCmdShow"= modo de exibição (p.e. 
								// normal/modal); é passado como parâmetro de WinMain()
	UpdateWindow(hWnd);		// Refrescar a janela (Windows envia à janela uma 
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
	// ============================================================================
	// Loop de Mensagens
	// Para usar as teclas aceleradoras do menu é necessário chamar a função
	// TranslateAccelerator(handler_janela, handler_accelerators, pont_mensagem)
	// Se esta função devolver falso, não foi premida tecla de aceleração 
	// ============================================================================
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
	// ============================================================================
	// 6. Fim do programa
	// ============================================================================
	return((int)lpMsg.wParam);	// Retorna sempre o parâmetro wParam da estrutura lpMsg
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

//  **************************************************************************************
//  *************** FUNÇÕES CALLBACK  ****************************************************
//  **************************************************************************************

//  *************** FUNÇÃO MENU PARA IP DO REMOTO ****************************************
BOOL CALLBACK IndicaIPRemoto(HWND h, UINT m, WPARAM w, LPARAM l) {
	switch (m) {
	case WM_COMMAND:
		switch (LOWORD(w))
		{
		case IDCANCEL:
			EndDialog(h, 0);
			//return 1;
		case IDOK:

		case WM_INITDIALOG:

			return 1;
		}
	case WM_CLOSE:
		EndDialog(h, 0);
		return 1;
	}
	//Não foi tratado o evento
	return 0;
}


//  *************** FUNÇÃO MENU PARA CONFIGURAR JOGO *************************************
BOOL CALLBACK ConfiguraJogo(HWND h, UINT m, WPARAM w, LPARAM l) {

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
		SetDlgItemInt(h, IDC_EDIT3, tamanhoSnakes, TRUE);
		return 1;
		
	}
	//Não foi tratado o evento
	return 0;
}
//  *************** FUNÇÃO MENU PARA CONFIGURAR OBJECTOS **********************************
BOOL CALLBACK ConfiguraObjectos(HWND h, UINT m, WPARAM w, LPARAM l) {

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

//  *************** FUNÇÃO CALLBACK PARA PEDIR NOME JOGADOR 1 **********************************
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

//  *************** FUNÇÃO CALLBACK PARA PEDIR NOME JOGADOR 2 **********************************
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

//  *************** FUNÇÃO CALLBACK BASE DA APLICAÇÃO / TRATA TECLADO ********************
LRESULT CALLBACK TrataEventos(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	HDC device;
	
	switch (messg) {
	case WM_CLOSE:
		if (MessageBox(hWnd, TEXT("Quer mesmo sair?"), TEXT("Snake Multiplayer."), MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			PostQuitMessage(0);
		break;

	case WM_COMMAND:    // Para configuração dos parametros de TABULEIRO
						// Sem PAI fica não bloqueante, pode ficar NULL ou Handle do Pai para bloquear
		switch (wParam)
		{
		case ID_SERVIDOR_REMOTO:DialogBox(hInstGlobal, IDD_DIALOG3, hWnd, IndicaIPRemoto);
			break;
		case ID_SERVIDOR_LOCAL:tipoServidor = LOCAL;
			preparaMemoriaPartilhada();
			break;
		case ID_JOGO_CRIAR:DialogBox(hInstGlobal, IDD_DIALOG5, hWnd, Pede_NomeJogador1);
			chamaCriaJogo();
			chamaAssociaJogo(username1, ASSOCIAR_JOGADOR1);
			break;
		case ID_JOGO_ASSOCIAR:if (numJogadores == 0) {
					DialogBox(hInstGlobal, IDD_DIALOG5, hWnd, Pede_NomeJogador1);
					chamaAssociaJogo(username1, ASSOCIAR_JOGADOR1);
				}
					else if (numJogadores == 1) {
						DialogBox(hInstGlobal, IDD_DIALOG6, hWnd, Pede_NomeJogador2);
						chamaAssociaJogo(username2, ASSOCIAR_JOGADOR2);
					}
					break;
		case ID_JOGO_JOGAR:chamaIniciaJogo();
			break;
		
		case ID_CONFIGURAR_TABULEIRO:DialogBox(hInstGlobal, IDD_DIALOG1, hWnd, ConfiguraJogo);
			break;
		case ID_CONFIGURAR_OBJECTOS:DialogBox(hInstGlobal, IDD_DIALOG4, hWnd, ConfiguraObjectos);
			break;
		case ID_EDITARJPGS:if (CreateProcess(NULL, executavel, NULL, NULL, 0, 0, NULL, NULL, &si, &pi))
								return 1;
						   else
								return 0;
			break;
		case ID_SAIR40011:if (MessageBox(hWnd, TEXT("Quer mesmo sair?"), TEXT("Snake Multiplayer."), MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			PostQuitMessage(0);
			break;
		case ID_INFORMA40018:MessageBoxA(hWnd, "\n\tSO2\n\n\nTrabalho Prático 2016/2017\n\nPaulo Alves - Aluno nº 21170449\n\t&\nJean Matias - Aluno nº 21200943 \n", "Snake Multiplayer", MB_OK);
			break;
		case ID_AJUDA_AJUDA:MessageBoxA(hWnd, "\n\tSO2\n\n\n 1º Deve escolher se joga localmente\n ou liga a computador remoto. \n", "Snake Multiplayer", MB_OK);
			break;
		}
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

// ==================================================================================== //
//		FUNÇÕES DE CHAMADA DE FUNÇÔES DE PEDIDOS                                        //
// ==================================================================================== //

int chamaCriaJogo(void) {
	ConfigInicial aux;

	aux.A = cobrasAutomaticas;
	aux.C = colunasConfig;
	aux.L = linhasConfig;
	aux.N = numJogadores;
	aux.O = numObjectos;
	aux.T = tamanhoSnakes;

	switch (tipoServidor)
	{
	case LOCAL:pede_CriaJogo(aux, pId);
			//ler Resposta e só depois validar
		return 1;
		break;
	case REMOTO://comunicar por pipes
		break;
	default:
		break;
	}
}

int chamaAssociaJogo(TCHAR username[SIZE_USERNAME], int codigo) {
	
	switch (tipoServidor)
	{
	case LOCAL:pede_AssociaJogo(pId, username, codigo);
			//ler Resposta e só depois validar
			numJogadores++;
			return 1;//sucesso
		break;
	case REMOTO://comunicar por pipes
		break;
	default:
		break;
	}
}

int chamaIniciaJogo(void) {

	switch (tipoServidor)
	{
	case LOCAL:pede_IniciaJogo(pId);
		//ler Resposta e só depois validar
		return 1;
		break;
	case REMOTO://comunicar por pipes
		break;
	default:
		break;
	}
}

