#include <windows.h>
#include <string.h>
#include <stdio.h>


// Global variable
HINSTANCE hInst;
POINT Point;        // point where mouse button

UINT  MessageCount = 0;
UINT  Count = 0;

int rectX;
int rectY;

int brushR = 100;
int brushG = 100;
int brushB = 100;


// Function prototypes.
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
void onPaint(HWND hWnd);

// Application entry point. This is the same as main() in standart C.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
    BOOL bRet;
    WNDCLASS wcx;          // register class
    HWND hWnd;

    hInst = hInstance;     // Save the application-instance handle.
	    // Fill in the window class structure with parameters that describe the main window.

    wcx.style = CS_HREDRAW | CS_VREDRAW;              // redraw if size changes
    wcx.lpfnWndProc = (WNDPROC) MainWndProc;                    // points to window procedure
    wcx.cbClsExtra = 0;                               // no extra class memory
    wcx.cbWndExtra = 0;                               // no extra window memory
    wcx.hInstance = hInstance;                        // handle to instance
    wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);      // predefined app. icon
    wcx.hCursor = LoadCursor(NULL, IDC_ARROW);                                   // predefined arrow
    wcx.hbrBackground = GetStockObject(WHITE_BRUSH);  // white background brush
    wcx.lpszMenuName =  (LPCSTR) "MainMenu";                   // name of menu resource
    wcx.lpszClassName = (LPCSTR) "MainWClass";                 // name of window class

    // Register the window class.

    if (!RegisterClass(&wcx)) return FALSE;

    // create window of registered class

    hWnd = CreateWindow(
        "MainWClass",        // name of window class
        "ITU",       // title-bar string
        WS_OVERLAPPEDWINDOW, // top-level window
        50,                  // default horizontal position
        100,                 // default vertical position
        640,                 // default width
        480,                 // default height
        (HWND) NULL,         // no owner window
        (HMENU) NULL,        // use class menu
        hInstance,           // handle to application instance
        (LPVOID) NULL);      // no window-creation data
    if (!hWnd) return FALSE;

    // Show the window and send a WM_PAINT message to the window procedure.
    // Record the current cursor position.

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // loop of message processing

    while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
    {
        if (bRet == -1)
        {
            // handle the error and possibly exit
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int) msg.wParam;
}

// **************************************************************** //
LRESULT CALLBACK MainWndProc(
    HWND hWnd,        // handle to window
    UINT uMsg,        // message identifier
    WPARAM wParam,    // first message parameter
    LPARAM lParam)    // second message parameter
{

    switch (uMsg)
    {
        case WM_CREATE:
            // Initialize the window.
            return 0;

        case WM_SIZE:
            // Set the size and position of the window.
            return 0;

        case WM_DESTROY:
            // Clean up window-specific data objects.
            PostQuitMessage(0);
            return 0;

		case WM_CHAR:
			switch (wParam) {

			case 0x51:  // Q
				brushR = 100;
				brushG = 0;
				brushB = 0;
				InvalidateRect(hWnd, NULL, TRUE);
				break;

			case 0x57:  // W
				brushR = 0;
				brushG = 100;
				brushB = 0;
				InvalidateRect(hWnd, NULL, TRUE);
				break;

			case 0x45:  // E
				brushR = 0;
				brushG = 0;
				brushB = 100;
				InvalidateRect(hWnd, NULL, TRUE);
				break;

			default:    // displayable character

				break;
			}
			break;

		case WM_KEYDOWN:
			switch (wParam) {
			case VK_LEFT:   // LEFT ARROW
				rectY -= 15;
				break;

			case VK_RIGHT:  // RIGHT ARROW
				rectY += 15;
				break;

			case VK_UP:   // LEFT nahoru
				rectX -= 15;
				break;

			case VK_DOWN:  // RIGHT dolu
				rectX += 15;
				break;
			}

			InvalidateRect(hWnd, NULL, TRUE);
			break;


		case WM_MOUSEMOVE:
			GetCursorPos(&Point);
			ScreenToClient(hWnd, &Point);

			InvalidateRect(hWnd, NULL, TRUE);
			return 0;
    
		case WM_PAINT:
			// Paint the window's client area.
			onPaint(hWnd);
			return 0;

        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

	
}

void onPaint(HWND hWnd)
{
	PAINTSTRUCT ps;                 // information can be used to paint the client area of a window owned by that application
	HDC         hDC;                // device context
	char        text[256];          // buffer to store an output text
	HFONT       font;               // new large font
	HFONT       oldFont;            // saves the previous font

	hDC = BeginPaint(hWnd, &ps);    // prepares the specified window for painting
									//nastavení barvy štìtce do DC
	HBRUSH brush, oldbrush;
		
	
	font = CreateFont(40, 0, 0, 0, 0, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, 0);

	oldFont = (HFONT)SelectObject(hDC, font);

	sprintf(text, "[%05u,%05u]", Point.x, Point.y);  // output text
	TextOut(                        // writes a character string at the specified location, using the currently selected font, background color, and text color
		hDC,                        // handle to device context
		1, 1,
		text,                       // character string
		(int)strlen(text));              // number of characters
	brush = CreateSolidBrush(RGB(brushR, brushG, brushB));

	oldbrush = SelectObject(hDC, brush);
	DeleteObject(brush);
	SelectObject(hDC, oldFont);
	Rectangle(hDC, rectY, rectX, rectY + 100, rectX + 100);
	DeleteObject(font);

	DeleteDC(hDC);                  // deletes the specified device context
	EndPaint(hWnd, &ps);            // marks the end of painting in the specified window
}