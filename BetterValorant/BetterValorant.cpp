#define NOMINMAX
#include <Windows.h>
#include <ShlObj.h>
#include <ShlObj_core.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <thread>

using namespace std::chrono_literals;

auto GetConfigPath() -> std::filesystem::path
{
    std::filesystem::path path;

    if (PWSTR szPath; SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_DEFAULT, nullptr, &szPath)))
    {
        path = szPath;
        path /= "BetterValorant/config.txt";
        CoTaskMemFree(szPath);
    }

    return path;
}

auto ReadValueFromConfig() -> std::optional<std::pair<std::string, std::string>>
{
    std::ifstream config{GetConfigPath()};
    if (!config.is_open())
    {
        return std::nullopt;
    }

    std::string width, height;

    std::getline(config, width);
    std::getline(config, height);

    config.close();

    return std::pair{width, height};
}

auto WriteValueToConfig(std::string width, std::string height) -> bool
{
    auto path = GetConfigPath();

    std::filesystem::create_directories(path.parent_path());

    std::ofstream config{path};
    if (!config.is_open())
    {
        return false;
    }

    config << width << std::endl;
    config << height << std::endl;

    config.close();

    return true;
}

auto main() -> int
{
    std::cout << "[+] BetterValorant Version: 1.0\n";
    std::cout << "[+] Source code: https://github.com/l1m0n3/BetterValorant\n";
    std::cout << "[+] Config file at: %userprofile%/Documents/BetterValorant/config.txt\n";

    std::string sWidth, sHeight;

    if (std::filesystem::exists(GetConfigPath()))
    {
        auto values = ReadValueFromConfig();
        sWidth = values->first;
        sHeight = values->second;
    }
    else
    {
        std::cout << "[+] Enter your desired width: ";
        std::cin >> sWidth;

        std::cout << "[+] Enter your desired height: ";
        std::cin >> sHeight;

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    DWORD dwWidth;
    try
    {
        dwWidth = std::stoul(sWidth);
    }
    catch (const std::exception& ex)
    {
        std::cout << "[!] The width is invalid!\n";
        std::this_thread::sleep_for(3s);
        return EXIT_FAILURE;
    }

    DWORD dwHeight;
    try
    {
        dwHeight = std::stoul(sHeight);
    }
    catch (const std::exception& ex)
    {
        std::cout << "[!] The height is invalid!\n";
        std::this_thread::sleep_for(3s);
        return EXIT_FAILURE;
    }

    auto hWnd = FindWindow(nullptr, L"VALORANT  ");
    if (!hWnd)
    {
        std::cout << "[!] Failed to find the window!\n";
        std::this_thread::sleep_for(3s);
        return EXIT_FAILURE;
    }

    DEVMODE devmode;
    devmode.dmPelsWidth = dwWidth;
    devmode.dmPelsHeight = dwHeight;
    devmode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
    devmode.dmSize = sizeof(DEVMODE);

    auto lResult = ChangeDisplaySettings(&devmode, 0);
    if (lResult != DISP_CHANGE_SUCCESSFUL)
    {
        std::cout << "[!] Failed to change display settings!\n";
        std::this_thread::sleep_for(3s);
        return EXIT_FAILURE;
    }

    /*auto lStyle = WS_MAXIMIZEBOX | 
                   WS_MINIMIZEBOX | 
                   WS_THICKFRAME | 
                   WS_SYSMENU | 
                   WS_DLGFRAME | 
                   WS_CLIPSIBLINGS | 
                   WS_VISIBLE;*/
    auto lStyle = GetWindowLong(hWnd, GWL_STYLE);
    lStyle &= ~WS_BORDER;
    SetWindowLongPtr(hWnd, GWL_STYLE, lStyle);

    /*RECT rect;
    GetClientRect(hWnd, &rect);
    SetWindowPos(hWnd, HWND_TOPMOST, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW);*/

    SetForegroundWindow(hWnd);

    ShowWindow(hWnd, SW_MAXIMIZE);

    WriteValueToConfig(sWidth, sHeight);

    std::cout << "[+] Enjoy the stretched res :D\n";
    std::cout << "[+] Press the enter key when you are done playing valorant...\n";

    std::cin.get();

    ChangeDisplaySettings(nullptr, 0);

    return EXIT_SUCCESS;
}
