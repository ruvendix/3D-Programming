// WindowsProject1.cpp: 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "WindowsProject1.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
HWND g_hWnd = nullptr;

HBRUSH g_hHighlightBrush = nullptr;

const static INT32 CELL_SIZE = 100;

INT32 g_clientMouseX = 0;
INT32 g_clientMouseY = 0;

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void  DrawLine(HDC hDC, INT32 startX, INT32 startY, INT32 endX, INT32 endY);

class GameBoard
{
public:
	GameBoard()
	{
		::SetRectEmpty(&m_rtGameBoard);
		m_cellIdxByPlayerClicked = -1;
		m_columnCnt = 3;
		m_rowCnt    = 3;
	}

	virtual ~GameBoard()
	{

	}

	void CalcGameBoard()
	{
		RECT rtClient;
		::SetRectEmpty(&rtClient);
		::GetClientRect(g_hWnd, &rtClient);

		INT32 width = rtClient.right - rtClient.left;
		INT32 height = rtClient.bottom - rtClient.top;

		// 화면 중앙에 오게 하는 방법
		m_rtGameBoard.left = (width - CELL_SIZE * 3) / 2;
		m_rtGameBoard.top = (height - CELL_SIZE * 3) / 2;

		m_rtGameBoard.right = m_rtGameBoard.left + CELL_SIZE * 3;
		m_rtGameBoard.bottom = m_rtGameBoard.top + CELL_SIZE * 3;
	}

	// 클라이언트 영역에서 클릭한 마우스 좌표를
	// 게임보드의 셀 인덱스로 변환해주는 함수
	void ConvertMousePosToCellIdx(INT32 clientMouseX, INT32 clientMouseY)
	{
		POINT ptClickedPos = { clientMouseX, clientMouseY };
		if (PtInRect(&m_rtGameBoard, ptClickedPos) == TRUE)
		{
			// 클릭한 마우스 좌표를 게임보드 위치와 맞춰줍니다.
			INT32 xPosInGameBoard = ptClickedPos.x - m_rtGameBoard.left;
			INT32 yPosInGameBoard = ptClickedPos.y - m_rtGameBoard.top;

			// 게임보드 내부의 좌표를 인덱스로 변환합니다.
			INT32 column = xPosInGameBoard / CELL_SIZE;
			INT32 row    = yPosInGameBoard / CELL_SIZE;

			m_cellIdxByPlayerClicked = column + row * 3;
		}
	}

	// 게임보드의 셀 인덱스를
	// 클라이언트 영역의 RECT 좌표로 변환해주는 함수
	RECT* ConvertCellIdxToGameBoardRect(INT32 cellIdx, RECT* pRtCell)
	{
        if ( (cellIdx < 0) ||
        (cellIdx > m_rowCnt * m_columnCnt - 1) )
		{
			return nullptr;
		}
		
		// 셀 인덱스를 행과 열로 분리합니다.
		INT32 row    = cellIdx / m_rowCnt;
		INT32 column = cellIdx % m_columnCnt;

		// 게임보드 위치를 맞춰주고
		// 셀 인덱스를 이용해서 셀 길이만큼 더해줍니다.
		pRtCell->left   = m_rtGameBoard.left + column * CELL_SIZE + 1;
		pRtCell->top    = m_rtGameBoard.top + row * CELL_SIZE + 1;
		pRtCell->right  = pRtCell->left + CELL_SIZE - 1;
		pRtCell->bottom = pRtCell->top + CELL_SIZE - 1;

		return pRtCell;
	}

	void ApplyCellRectByPlayerClicked()
	{
		ConvertCellIdxToGameBoardRect(m_cellIdxByPlayerClicked, &m_rtCellByPlayerClicked);
	}

	void DrawGameBoard(HDC hDC)
	{
		//::Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
		::FillRect(hDC, &m_rtGameBoard, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));

		for (INT32 i = 0; i < 4; ++i)
		{
			DrawLine(hDC, m_rtGameBoard.left + CELL_SIZE * i, m_rtGameBoard.top,
				m_rtGameBoard.left + CELL_SIZE * i, m_rtGameBoard.bottom);
			DrawLine(hDC, m_rtGameBoard.left, m_rtGameBoard.top + CELL_SIZE * i,
				m_rtGameBoard.right, m_rtGameBoard.top + CELL_SIZE * i);
		}

		// 클릭한 셀 인덱스가 있다면 그 셀을 검은색으로 채웁니다.
		if (m_cellIdxByPlayerClicked != -1)
		{
			ConvertCellIdxToGameBoardRect(m_cellIdxByPlayerClicked, &m_rtCellByPlayerClicked);
			::FillRect(hDC, &m_rtCellByPlayerClicked, g_hHighlightBrush);
		}

#ifdef _DEBUG // 디버그 정보
		RECT rtDebug = { 0, 0, 256, 60 };
		::FillRect(hDC, &rtDebug, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));

		WCHAR szTemp[256];
		for (INT32 i = 0; i < 3; ++i)
		{
			for (INT32 j = 0; j < 3; ++j)
			{
				_snwprintf_s(szTemp, _countof(szTemp), L"%d", j + i * 3);
				TextOut(hDC,
					m_rtGameBoard.left + (CELL_SIZE * j) + (CELL_SIZE / 2),
					m_rtGameBoard.top + (CELL_SIZE * i) + (CELL_SIZE / 2) - 10,
					szTemp, wcslen(szTemp));
			}
		}

		POINT desktopMousePos;
		::GetCursorPos(&desktopMousePos);

		_snwprintf_s(szTemp, _countof(szTemp),
			L"데스크탑 마우스 좌표(%d, %d)",
			desktopMousePos.x, desktopMousePos.y);
		TextOut(hDC, 0, 0, szTemp, wcslen(szTemp));

		_snwprintf_s(szTemp, _countof(szTemp),
			L"클라이언트 마우스 좌표(%d, %d)",
			g_clientMouseX, g_clientMouseY);
		TextOut(hDC, 0, 20, szTemp, wcslen(szTemp));

		INT32 clickedIdx = getCellIdxByPlayerClicked();
		_snwprintf_s(szTemp, _countof(szTemp),
			L"클릭한 셀 인덱스 = %d", clickedIdx);
		TextOut(hDC, 0, 40, szTemp, wcslen(szTemp));
#endif
	}

	INT32 getCellIdxByPlayerClicked() const noexcept
	{
		return m_cellIdxByPlayerClicked;
	}

private:
	RECT  m_rtGameBoard;
	RECT  m_rtCellByPlayerClicked;
	INT32 m_cellIdxByPlayerClicked;
	INT32 m_columnCnt;
	INT32 m_rowCnt;
};

GameBoard g_gameBoard;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg;

    // 기본 메시지 루프입니다.
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(GetStockObject(GRAY_BRUSH));
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = g_hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다.
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_CREATE:
	{
		g_hHighlightBrush = ::CreateSolidBrush(RGB(0, 255, 0));
	}
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

			::SetBkMode(hdc, TRANSPARENT);
			::SetTextColor(hdc, RGB(0, 0, 0));

			HBRUSH hOldBrush = static_cast<HBRUSH>(::SelectObject(hdc, g_hHighlightBrush));

            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
			g_gameBoard.CalcGameBoard();
			g_gameBoard.DrawGameBoard(hdc);
						
			::SelectObject(hdc, hOldBrush);

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		::DeleteObject(g_hHighlightBrush);
        PostQuitMessage(0);
        break;
	case WM_LBUTTONDOWN:
	{
		g_gameBoard.ConvertMousePosToCellIdx(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		g_gameBoard.ApplyCellRectByPlayerClicked();
		InvalidateRect(hWnd, nullptr, true);
		break;
	}
	case WM_MOUSEMOVE:
	{
		g_clientMouseX = GET_X_LPARAM(lParam);
		g_clientMouseY = GET_Y_LPARAM(lParam);
		InvalidateRect(hWnd, nullptr, true);
		break;
	}
	case WM_GETMINMAXINFO: // 최대 최소화 제한 걸기
	{
		MINMAXINFO* pMinMax = reinterpret_cast<MINMAXINFO*>(lParam);
		if (pMinMax == nullptr)
		{
			return 0;
		}
	
		pMinMax->ptMinTrackSize.x = 100 * 5;
		pMinMax->ptMinTrackSize.y = 100 * 5;
		break;
	}
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void DrawLine(HDC hDC, INT32 startX, INT32 startY, INT32 endX, INT32 endY)
{
	MoveToEx(hDC, startX, startY, nullptr);
	LineTo(hDC, endX, endY);
}