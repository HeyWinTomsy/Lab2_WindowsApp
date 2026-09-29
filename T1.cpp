#include <windows.h>
#include <math.h>

BOOL RegClass(WNDPROC, LPCTSTR, UINT);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE hInstance;
wchar_t szClassName[] = L"GraphClass";

typedef struct
{
    wchar_t name[30];
    float x[100];
    float y[100];
} FUNC;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpszCmdLine, int nCmdShow)
{
    MSG msg;
    HWND hwnd;

    ::hInstance = hInstance;

    if (!RegClass(WndProc, szClassName, COLOR_WINDOW))
        return FALSE;

    hwnd = CreateWindow(
        szClassName,
        L"Графік y = 2cos(x - pi/6)",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        0, 0, hInstance, NULL
    );

    if (!hwnd)
        return FALSE;

    while (GetMessage(&msg, 0, 0, 0))
        DispatchMessage(&msg);

    return msg.wParam;
}

BOOL RegClass(WNDPROC Proc, LPCTSTR szName, UINT brBackground)
{
    WNDCLASS wc;

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.cbClsExtra = wc.cbWndExtra = 0;
    wc.lpfnWndProc = Proc;
    wc.hInstance = hInstance;
    wc.lpszClassName = szName;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(brBackground + 1);
    wc.lpszMenuName = NULL;

    return (RegisterClass(&wc) != 0);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static short cx, cy;
    static FUNC myfunc;

    switch (msg)
    {
    case WM_SIZE:
    {
        cx = LOWORD(lParam);
        cy = HIWORD(lParam);
        return 0;
    }

    case WM_CREATE:
    {
        wcscpy_s(myfunc.name, 30, L"y = 2*cos(x - pi/6)");

        // x змінюється від 0 до 1
        for (int i = 0; i < 100; i++)
        {
            myfunc.x[i] = (float)i / 99.0f;

            // y = 2*cos(x - pi/6)
            myfunc.y[i] =
                2.0f * cos(myfunc.x[i] - (float)M_PI / 6.0f);
        }

        return 0;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Область для малювання графіка
        int x0 = cx / 10;
        int xr = cx - x0;
        int y0 = cy / 10;
        int yc = cy / 2;
        int amp = yc - y0;

        float ymax = 0.0f;
        float ymin = 0.0f;

        // Назва функції
        TextOut(
            hdc,
            x0 + 4,
            y0 / 3,
            myfunc.name,
            (int)wcslen(myfunc.name)
        );

        // Назва осі X
        TextOut(
            hdc,
            xr + x0 / 2,
            yc + y0 / 10,
            L"x",
            1
        );

        // Вісь Y
        MoveToEx(hdc, x0, cy - y0 + y0 / 2, NULL);
        LineTo(hdc, x0, y0 - y0 / 2);

        // Вісь X
        MoveToEx(hdc, x0, yc, NULL);
        LineTo(hdc, xr + x0 / 2, yc);

        // Знаходимо максимум і мінімум функції
        for (int i = 0; i < 100; i++)
        {
            float ycurr = myfunc.y[i];

            if (ymax < ycurr)
                ymax = ycurr;

            if (ymin > ycurr)
                ymin = ycurr;
        }

        // Враховуємо найбільше за модулем значення
        if (ymax < fabs(ymin))
            ymax = (float)fabs(ymin);

        // Масштаб по Y
        float dy = (float)amp / ymax;

        // Масштаб по X
        float dx =
            (float)(xr - x0) /
            (myfunc.x[99] - myfunc.x[0]);

        // Синє перо
        HPEN hPen = CreatePen(
            PS_SOLID,
            2,
            RGB(0, 0, 255)
        );

        HPEN hOldPen =
            (HPEN)SelectObject(hdc, hPen);

        // Перша точка
        MoveToEx(
            hdc,
            x0,
            (int)(yc - dy * myfunc.y[0]),
            NULL
        );

        // Малюємо графік
        for (int i = 1; i < 100; i++)
        {
            int xcurr =
                (int)(
                    dx *
                    (myfunc.x[i] - myfunc.x[0])
                    + x0
                );

            int ycurr =
                (int)(
                    yc -
                    dy * myfunc.y[i]
                );

            LineTo(hdc, xcurr, ycurr);
        }

        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}