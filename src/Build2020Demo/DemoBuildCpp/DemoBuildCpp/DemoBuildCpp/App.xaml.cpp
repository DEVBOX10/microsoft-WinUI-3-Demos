﻿#include "pch.h"

#include "App.xaml.h"
#include "MainWindow.xaml.h"
#include "microsoft.ui.xaml.window.h" //For using IWindowNative

using namespace winrt;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Navigation;
using namespace DemoBuildCpp;
using namespace DemoBuildCpp::implementation;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

/// <summary>
/// Initializes the singleton application object.  This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
App::App()
{
    InitializeComponent();

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
    UnhandledException([this](IInspectable const&, UnhandledExceptionEventArgs const& e)
    {
        if (IsDebuggerPresent())
        {
            auto errorMessage = e.Message();
            __debugbreak();
        }
    });
#endif
}

/// <summary>
/// Invoked when the application is launched normally by the end user.  Other entry points
/// will be used such as when the application is launched to open a specific file.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void App::OnLaunched(LaunchActivatedEventArgs const&)
{
    HWND hwnd;
    window = make<MainWindow>();
    window.as<IWindowNative>()->get_WindowHandle(&hwnd);
    window.Activate();

    // The Window object doesn't have Width and Height properties in WInUI 3 Desktop yet.
    // To set the Width and Height, you can use the Win32 API SetWindowPos.
    // Note, you should apply the DPI scale factor if you are thinking of dpi instead of pixels.
    setWindowSize(hwnd, 800, 600);
}

void winrt::DemoBuildCpp::implementation::App::setWindowSize(const HWND& hwnd, int width, int height)
{
    auto dpi = GetDpiForWindow(hwnd);
    float scalingFactor = static_cast<float>(dpi) / 96;
    RECT scale;
    scale.left = 0;
    scale.top = 0;
    scale.right = static_cast<LONG>(width * scalingFactor);
    scale.bottom = static_cast<LONG>(height * scalingFactor);
    SetWindowPos(hwnd, HWND_TOP, 0, 0, scale.right - scale.left, scale.bottom - scale.top, SWP_NOMOVE);
}