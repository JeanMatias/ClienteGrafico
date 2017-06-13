#include "ClienteGra.h"

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
	BOOL correuBem = TRUE;
	switch (m) {
	case WM_COMMAND:
		switch (LOWORD(w))
		{
		case IDCANCEL:
			EndDialog(h, 0);
			return 0;
		case IDOK:
			aux1 = GetDlgItemInt(h, IDC_EDIT1, NULL, TRUE);
			aux2 = GetDlgItemInt(h, IDC_EDIT2, NULL, TRUE);
			if (aux1 > MAX_LINHAS || aux1 < MIN_LINHAS || aux2 > MAX_COLUNAS || aux2 < MIN_COLUNAS) {
				MessageBox(h, TEXT("LINHAS: Máximo 30 E Minimo 15 \n\nCOLUNAS: Máximo 60 E Minimo 15"), TEXT("ATENÇÂO"), MB_OK);
				correuBem = FALSE;
			}
			else {
				colunasConfig = aux1;
				linhasConfig = aux2;
			}
			aux3 = GetDlgItemInt(h, IDC_EDIT3, NULL, TRUE);
			if (aux3 < TAMANHOSNAKE || aux3 > 6) {
				MessageBox(h, TEXT("Tamanho Configurável das Cobras: Compreendido entre 3 e 6"), TEXT("ATENÇÂO"), MB_OK);
				correuBem = FALSE;
			}	
			else{
				tamanhoSnakes = aux3;			
			}
			aux4 = GetDlgItemInt(h, IDC_EDIT4, NULL, TRUE);
			if (aux4 < NUMAUTOSNAKE || aux4 > 6) {
				MessageBox(h, TEXT("Número Cobras Automáticas: Compreendido entre 1 e 6"), TEXT("ATENÇÃO"), MB_OK);
				correuBem = FALSE;
			}				
			else {
				cobrasAutomaticas = aux4;
			}
			aux5 = GetDlgItemInt(h, IDC_EDIT5, NULL, TRUE);
			if (aux5 > MAXOBJECTOS) {
				MessageBox(h, TEXT("Número Máximo de Objectos: Limite é 30"), TEXT("ATENÇÃO"), MB_OK);
				correuBem = FALSE;
			}	
			else {
				numObjectos = aux5;
			}
			aux6 = GetDlgItemInt(h, IDC_EDIT6, NULL, TRUE);
			if (aux6 < NUMJOGADORES || aux6 > MAXJOGADORES) {
				MessageBox(h, TEXT("Número de Jogadores: Compreendido entre 1 e 4"), TEXT("ATENÇÃO"), MB_OK);
				correuBem = FALSE;
			}	
			else {
				numJogadores = aux6;
			}
			if(correuBem==TRUE)
				EndDialog(h, 0);
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
	static int escolha;
	switch (m) {
	case WM_COMMAND:
			switch (LOWORD(w)) {
			case IDC_RADIO1:escolha = 1;
				break;
			case IDC_RADIO2:escolha = 2;
				break;
			case IDCANCEL:
				EndDialog(h, INSUCESSO);
				return 1;
			case IDOK:
				GetDlgItemText(h, IDC_EDIT23, username1, sizeof(IDC_EDIT23));
				EndDialog(h, SUCESSO);
				if (escolha == 1) {
					teclasjogador1.baixo = 83;
					teclasjogador1.cima = 87;
					teclasjogador1.esquerda = 65;
					teclasjogador1.direita = 68;
					MessageBox(janelaglobal, TEXT("Vai jogar com a teclas por defeito. (W,A,S,D)"), TEXT("Selecção teclas do jogo."), MB_OK);
				}
				else if(escolha == 2)
					DialogBox(hInstGlobal, IDD_TECLAS1, janelaglobal, ConfigTeclas1);
				return 1;
			}
		return 1;
	case WN_CANCEL:
		EndDialog(h, INSUCESSO);
		return 1;
	case WM_INITDIALOG:
		CheckRadioButton(h, IDC_RADIO1, IDC_RADIO2, IDC_RADIO1);
		escolha = 1;
		SendDlgItemMessage(h, IDC_EDIT23, EM_LIMITTEXT, SIZE_USERNAME, NULL);
		return 1;
	}

	return 0;
}

/* ----------------------------------------------------- */
/*		FUNÇÃO CALLBACK PARA PEDIR NOME JOGADOR 2		 */
/* ----------------------------------------------------- */

BOOL CALLBACK Pede_NomeJogador2(HWND h, UINT m, WPARAM w, LPARAM l) {
	static int escolha;
	switch (m) {
	case WM_COMMAND:
		switch (LOWORD(w)) {
		case IDC_RADIO1:escolha = 1;
			break;
		case IDC_RADIO2:escolha = 2;
			break;
		case IDCANCEL:
			EndDialog(h, INSUCESSO);
			return 1;
		case IDOK:
			GetDlgItemText(h, IDC_EDIT24, username2, sizeof(IDC_EDIT24));
			EndDialog(h, SUCESSO);
			if (escolha == 1) {
				teclasjogador1.baixo = 98;
				teclasjogador1.cima = 104;
				teclasjogador1.esquerda = 100;
				teclasjogador1.direita = 102;
				MessageBox(janelaglobal, TEXT("Vai jogar com a teclas por defeito. (8,4,2,6 do teclado numerico)"), TEXT("Selecção teclas do jogo."), MB_OK);
			}	
			else if (escolha == 2)
				DialogBox(hInstGlobal, IDD_TECLAS2, janelaglobal, ConfigTeclas2);
			return 1;
		}
		return 1;
	case WN_CANCEL:
		EndDialog(h, INSUCESSO);
		return 1;
	case WM_INITDIALOG:
		CheckRadioButton(h, IDC_RADIO1, IDC_RADIO2, IDC_RADIO1);
		escolha = 1;
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
					teste = CreateMutex(NULL, TRUE, NOME_MUTEX_TESTE);
					if (GetLastError() == ERROR_ALREADY_EXISTS) {
						MessageBox(hWnd, TEXT("Já existe um cliente a correr! Este vai fechar."), TEXT("ERRO"), MB_OK);
						PostQuitMessage(0);
						return 1;
					}
					CarregaBitmaps(hWnd);// Carregar BitMaps para memoria
					return 1;
				break;

		case WM_ERASEBKGND:	break; //para eliminar a cintilação da janela

		case WM_PAINT:
			device = BeginPaint(hWnd, &pintar);
			BitBlt(device, 0, 0, maxX, maxY, memoriajanela, 0, 0, SRCCOPY);
			EndPaint(hWnd, &pintar);
			break;

		case WM_CLOSE:
			if (MessageBox(hWnd, TEXT("Quer mesmo sair?"), TEXT("Snake Multiplayer."), MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			{
				if (tipoServidor == LOCAL) {
					pede_Sair(pId, tId);
					fechaMemoriaPartilhadaGeral();
					fechaMemoriaPartilhadaResposta();
				}

				PostQuitMessage(0);
			}
			return 1;
			break;
/*==========================================================================================================================
													Mensagens do Menu:
  ==========================================================================================================================*/
		case WM_COMMAND:			
			switch (wParam)
			{					
			case ID_SERVIDOR_REMOTO:CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ligaServidorRemoto, (LPVOID)NULL, 0, &auxtid);
									break;
			case ID_SERVIDOR_LOCAL:	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ligaServidorLocal, (LPVOID)NULL, 0, &auxtid);
				break;
			case ID_JOGO_CRIAR:	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)criaJogo, (LPVOID)NULL, 0, &auxtid);
				break;
			case ID_JOGO_ASSOCIAR:CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)associaJogo, (LPVOID)NULL, 0, &auxtid);
				break;
			case ID_JOGO_JOGAR:	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)iniciaJogo, (LPVOID)NULL, 0, &auxtid);
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

/*==========================================================================================================================
												Tratar Teclas:
  ==========================================================================================================================*/
		case WM_KEYDOWN:
			if (emJogo) {
				if (wParam == teclasjogador1.baixo) {//Jogador 1 move-se para baixo
					chamaMudaDirecao(BAIXO, JOGADOR1);
				}
				else if (wParam == teclasjogador1.cima) {//Jogador 1 move-se para cima
					chamaMudaDirecao(CIMA, JOGADOR1);
				}
				else if (wParam == teclasjogador1.esquerda) {//Jogador 1 move-se para esquerda
					chamaMudaDirecao(ESQUERDA, JOGADOR1);
				}
				else if (wParam == teclasjogador1.direita) {//Jogador 1 move-se para a direita
					chamaMudaDirecao(DIREITA, JOGADOR1);
				}
				if (numJogadoresLocal == 2) {
					if (wParam == teclasjogador2.baixo) {//Jogador 2 move-se para baixo
						chamaMudaDirecao(BAIXO, JOGADOR2);
					}
					else if (wParam == teclasjogador2.cima) {//Jogador 2 move-se para cima
						chamaMudaDirecao(CIMA, JOGADOR2);
					}
					else if (wParam == teclasjogador2.esquerda) {//Jogador 2 move-se para esquerda
						chamaMudaDirecao(ESQUERDA, JOGADOR2);
					}
					else if (wParam == teclasjogador2.direita) {//Jogador 2 move-se para a direita
						chamaMudaDirecao(DIREITA, JOGADOR2);
					}
				}
			}
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
	ConfigObjecto objectos[NUMTIPOOBJECTOS];

	aux.A = cobrasAutomaticas;
	aux.C = colunasConfig;
	aux.L = linhasConfig;
	aux.N = numJogadores;
	aux.O = numObjectos;
	aux.T = tamanhoSnakes;

	objectos[ALIMENTO - 1].S = alimento;
	objectos[GELO - 1].S = gelo;
	objectos[GRANADA - 1].S = granada;
	objectos[VODKA - 1].S = vodka;
	objectos[OLEO - 1].S = oleo;
	objectos[COLA - 1].S = cola;
	objectos[O_VODKA - 1].S = o_vodka;
	objectos[O_OLEO - 1].S = o_oleo;
	objectos[O_COLA - 1].S = o_cola;
	objectos[SURPRESA - 1].S = surpresa;


	switch (tipoServidor)
	{
	case LOCAL:pede_CriaJogo(aux, pId,tId, username1,objectos);
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

void chamaMudaDirecao(int direcao, int jogador) {
	switch (tipoServidor) {
	case LOCAL:mudaDirecao(direcao, pId, tId, jogador);
		break;
	case REMOTO:
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
	
	getLimitesMapa(&linhas, &colunas);
	getMapa(mapa);
	desenhaMapaNaMemoria();
	InvalidateRect(janelaglobal, NULL, 0);
	while (1) {   //enquanto o jogo estiver a continuar
		WaitForSingleObject(hEventoMapa, INFINITE);
		getMapa(mapa);
		desenhaMapaNaMemoria();
		InvalidateRect(janelaglobal, NULL, 0);
	}
}

DWORD WINAPI ligaServidorLocal(LPVOID param) {
	preparaMemoriaPartilhada();
	pede_RegistarClienteLocal(pId, tId);
	Sleep(50);
	preparaMemoriaPartilhadaResposta(pId, tId);
	WaitForSingleObject(hEventoResposta, INFINITE);
	ResetEvent(hEventoResposta);
	if (vistaResposta->resposta == SUCESSO) {
		tipoServidor = LOCAL;
		MessageBox(janelaglobal, TEXT("Ligado a servidor Local"), TEXT("SUCESSO"), MB_OK);
		EnableMenuItem(hMenu, ID_JOGO_CRIAR, MF_ENABLED);
		EnableMenuItem(hMenu, ID_JOGO_ASSOCIAR, MF_ENABLED);
		EnableMenuItem(hMenu, ID_JOGO_JOGAR, MF_ENABLED);
		EnableMenuItem(hMenu, ID_SERVIDOR_REMOTO, MF_DISABLED);
		EnableMenuItem(hMenu, ID_SERVIDOR_LOCAL, MF_DISABLED);
	}
	else if (vistaResposta->resposta == INSUCESSO) {
		MessageBox(janelaglobal, TEXT("Não foi possivel ligar ao servidor, contacte o utilizador da maquina"), TEXT("INSUCESSO"), MB_OK);
	}
}

DWORD WINAPI ligaServidorRemoto(LPVOID param) {
	DialogBox(hInstGlobal, IDD_DIALOG3, janelaglobal, IndicaIPRemoto);
	EnableMenuItem(hMenu, ID_JOGO_CRIAR, MF_ENABLED);
	EnableMenuItem(hMenu, ID_JOGO_ASSOCIAR, MF_ENABLED);
	EnableMenuItem(hMenu, ID_JOGO_JOGAR, MF_ENABLED);
	EnableMenuItem(hMenu, ID_CONF_TECLAS_1, MF_ENABLED);
	EnableMenuItem(hMenu, ID_CONF_TECLAS_2, MF_ENABLED);
}


DWORD WINAPI criaJogo(LPVOID param) {
	int resposta, valor;
	DialogBox(hInstGlobal, IDD_DIALOG5, janelaglobal, Pede_NomeJogador1);
	resposta = chamaCriaJogo(&valor);
	if (resposta == SUCESSO) {
		MessageBox(janelaglobal, TEXT("Jogo Criado"), TEXT("SUCESSO"), MB_OK);
		valorCobra1 = valor;
		EnableMenuItem(hMenu, ID_JOGO_ASSOCIAR, MF_ENABLED);
		EnableMenuItem(hMenu, ID_JOGO_JOGAR, MF_ENABLED);
		EnableMenuItem(hMenu, ID_CONF_TECLAS_1, MF_ENABLED);
		EnableMenuItem(hMenu, ID_CONF_TECLAS_2, MF_ENABLED);
	}
	else if (resposta == INSUCESSO) {
		MessageBox(janelaglobal, TEXT("Não é possivel criar jogo neste momento"), TEXT("INSUCESSO"), MB_OK);
	}
}

DWORD WINAPI associaJogo(LPVOID param) {
	int resposta, valor;
	if (numJogadoresLocal == 0) {
		DialogBox(hInstGlobal, IDD_DIALOG5, janelaglobal, Pede_NomeJogador1);
		resposta = chamaAssociaJogo(username1, ASSOCIAR_JOGADOR1, &valor);
		if (resposta == SUCESSO) {
			MessageBox(janelaglobal, TEXT("Jogador 1 Associado"), TEXT("SUCESSO"), MB_OK);
			valorCobra1 = valor;
			numJogadoresLocal++;
		}
		else if (resposta == INSUCESSO) {
			if (valor == AGORANAO) {
				MessageBox(janelaglobal, TEXT("Não é possivel associar ao jogo neste momento"), TEXT("INSUCESSO"), MB_OK);
			}
			else if (valor == JOGOCHEIO) {
				MessageBox(janelaglobal, TEXT("Não existem mais vagas no jogo"), TEXT("INSUCESSO"), MB_OK);
			}
		}
	}
	else if (numJogadoresLocal == 1) {
		DialogBox(hInstGlobal, IDD_DIALOG6, janelaglobal, Pede_NomeJogador2);
		resposta = chamaAssociaJogo(username2, ASSOCIAR_JOGADOR2, &valor);
		if (resposta == SUCESSO) {
			MessageBox(janelaglobal, TEXT("Jogador 2 Associado"), TEXT("SUCESSO"), MB_OK);
			valorCobra2 = valor;
			numJogadoresLocal++;
		}
		else if (resposta == INSUCESSO) {
			if (valor == AGORANAO) {
				MessageBox(janelaglobal, TEXT("Não é possivel associar ao jogo neste momento"), TEXT("INSUCESSO"), MB_OK);
			}
			else if (valor == JOGOCHEIO) {
				MessageBox(janelaglobal, TEXT("Não existem mais vagas no jogo"), TEXT("INSUCESSO"), MB_OK);
			}
		}
	}
}

DWORD WINAPI iniciaJogo(LPVOID param) {
	int resposta, valor, auxtid;
	resposta = chamaIniciaJogo(&valor);
	if (resposta == SUCESSO) {
		MessageBox(janelaglobal, TEXT("Jogo Iniciado"), TEXT("SUCESSO"), MB_OK);//lançar as threads de actualização do mapa
		emJogo = TRUE;
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)esperaActualizacao, (LPVOID)NULL, 0, &auxtid);
	}
	else if (resposta == INSUCESSO) {
		if (valor == CRIADORERRADO) {
			MessageBox(janelaglobal, TEXT("Apenas o cliente que criou o jogo pode iniciar o mesmo!"), TEXT("INSUCESSO"), MB_OK);
		}
		if (valor == AGORANAO) {
			MessageBox(janelaglobal, TEXT("Deve iniciar o jogo só depois de criar o mesmo!"), TEXT("INSUCESSO"), MB_OK);
		}
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
			case ALIMENTO:BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcomida, 0, 0, SRCCOPY);
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
					case 0://não tem direção quer dizer que é corpo vamos ver o estado da cobra
						switch (dezenas) {
						case BEBADO:
							BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcobra2_corpo4, 0, 0, SRCCOPY);
							break;
						case LEBRE:
							BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcobra2_corpo3, 0, 0, SRCCOPY);
							break;
						case TARTARUGA:
							BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcobra2_corpo2, 0, 0, SRCCOPY);
							break;
						default://cor normal da cobra 2
							BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcobra2_corpo1, 0, 0, SRCCOPY);
							break;
						}
						break;
					}
				}
				else {//está outra cobra na posição
					switch (unidades)
					{//se tiver direção é cabeça senão é corpo da cobra
					case CIMA:BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcobra3_cab3_cima, 0, 0, SRCCOPY);
						break;
					case BAIXO:BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcobra3_cab3_baixo, 0, 0, SRCCOPY);
						break;
					case ESQUERDA:BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcobra3_cab3_esquerda, 0, 0, SRCCOPY);
						break;
					case DIREITA:BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcobra3_cab3_direita, 0, 0, SRCCOPY);
						break;
					case 0://não tem direção quer dizer que é corpo vamos ver o estado da cobra
						switch (dezenas) {
						case BEBADO:
							BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcobra3_corpo4, 0, 0, SRCCOPY);
							break;
						case LEBRE:
							BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcobra3_corpo3, 0, 0, SRCCOPY);
							break;
						case TARTARUGA:
							BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcobra3_corpo2, 0, 0, SRCCOPY);
							break;
						default://cor normal da cobra 2
							BitBlt(memoriajanela, x * 20, y * 20, 20, 20, hcobra3_corpo1, 0, 0, SRCCOPY);
							break;
						}
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
