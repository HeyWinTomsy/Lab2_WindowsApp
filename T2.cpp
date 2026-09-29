#include <windows.h>
#include <vector>

void Rectangle(HDC dc, int l, int t, int r, int b, COLORREF color)
{
    HBRUSH brush = CreateSolidBrush(color);
    HGDIOBJ old = SelectObject(dc, brush);
    Rectangle(dc, l, t, r, b);
    SelectObject(dc, old);
    DeleteObject(brush);
}

void Poly(HDC dc, const POINT* pts, int n, COLORREF color)
{
    HBRUSH brush = CreateSolidBrush(color);
    HGDIOBJ old = SelectObject(dc, brush);
    Polygon(dc, pts, n);
    SelectObject(dc, old);
    DeleteObject(brush);
}

class Cloud
{
public:
    void show(HDC dc, int X, int Y)
    {
        COLORREF white = RGB(255, 255, 255);
        Ellipse(dc, X,      Y + 10, X + 50, Y + 35);
        Ellipse(dc, X + 20, Y,      X + 70, Y + 30);
        Ellipse(dc, X + 45, Y + 10, X + 95, Y + 35);
    }
};

class Lighthouse
{
public:
    void show(HDC dc, int X, int Y)
    {

        POINT tower[4] = { {X - 25, Y}, {X + 25, Y}, {X + 15, Y - 140}, {X - 15, Y - 140} };
        Poly(dc, tower, 4, RGB(255, 255, 255));

        POINT stripe[4] = { {X - 19, Y - 60}, {X + 21, Y - 90}, {X + 19, Y - 120}, {X - 18, Y - 90} };
        Poly(dc, stripe, 4, RGB(220, 30, 30));

        Rectangle(dc, X - 12, Y - 165, X + 12, Y - 140, RGB(255, 240, 80));

        POINT roof[3] = { {X - 16, Y - 165}, {X + 16, Y - 165}, {X, Y - 190} };
        Poly(dc, roof, 3, RGB(220, 30, 30));
    }
};

class Ship
{
public:
    void show(HDC dc, int X, int Y)
    {

        Rectangle(dc, X - 12, Y - 70, X + 12, Y - 60, RGB(255, 215, 0));
        Rectangle(dc, X - 12, Y - 80, X + 12, Y - 70, RGB(0, 87, 153));

        Rectangle(dc, X + 12 , Y - 60, X + 8, Y - 40, RGB(70, 70, 70));
        Rectangle(dc, X + 24 , Y - 75, X + 20, Y - 40, RGB(70, 70, 70));

        Rectangle(dc, X, Y - 40, X + 40, Y - 15, RGB(150, 150, 150));

        POINT hull[4] = { {X - 100, Y - 15}, {X + 100, Y - 20}, {X + 90, Y}, {X - 100, Y} };
        Poly(dc, hull, 4, RGB(70, 70, 70));
    }
};

class Shore {
public:
    void show(HDC dc, int X, int Y) {
        POINT hull[4] = { {X-80, Y - 15}, {X + 200, Y - 25}, {X + 200, Y + 250}, {X + 40, Y + 250} };
        Poly(dc, hull, 4, RGB(194, 178, 128));
    }
};

class Sea
{
public:
    void show(HDC dc, int X, int Y, int width, int height)
    {
        Rectangle(dc, X - 1, Y, X + width + 1, Y + height + 1, RGB(0, 90, 200));
    }
};

int g_W = 900, g_H = 600;
int g_shipX = 200;
std::vector<POINT> g_clouds;

void DrawScene(HDC dc)
{
    int horizon = g_H * 60 / 100;

    Rectangle(dc, -1, -1, g_W + 1, horizon, RGB(135, 206, 250));

    Cloud cloud;
    for (size_t i = 0; i < g_clouds.size(); i++)
        cloud.show(dc, g_clouds[i].x, g_clouds[i].y);

    Sea sea;
    sea.show(dc, 0, horizon, g_W, g_H - horizon);

    Shore shore;
    shore.show(dc, g_W - 160 , horizon);

    Lighthouse lighthouse;
    lighthouse.show(dc, g_W - 160, horizon + 10);

    Ship ship;
    ship.show(dc, g_shipX, horizon + 70);
    ship.show(dc, g_shipX + 120, horizon + 130);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        POINT c1 = { 60, 50 }, c2 = { 330, 90 }, c3 = { 600, 40 };
        g_clouds.push_back(c1);
        g_clouds.push_back(c2);
        g_clouds.push_back(c3);
        return 0;
    }
    case WM_SIZE:
        g_W = LOWORD(lParam);
        g_H = HIWORD(lParam);
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        DrawScene(hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_LBUTTONDOWN:
    {
        POINT p = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
        g_clouds.push_back(p);                 // нова хмара в точці кліку
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;
    }
    case WM_KEYDOWN:
        if (wParam == VK_LEFT)   g_shipX -= 15;
        if (wParam == VK_RIGHT)  g_shipX += 15;
        if (wParam == VK_ESCAPE) DestroyWindow(hwnd);
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"LighthouseSeaApp";

    WNDCLASS wc = {};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = CLASS_NAME;
    if (!RegisterClass(&wc)) return 0;

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"Маяк, кораблі, хмари, море",
        WS_OVERLAPPEDWINDOW, 100, 100, 900, 600,
        nullptr, nullptr, hInstance, nullptr);
    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}