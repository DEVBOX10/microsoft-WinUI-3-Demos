﻿#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
//DEMO
#include "microsoft.ui.xaml.window.h"
#include <chrono>
#include <tchar.h>
#include <psapi.h>
#include <filesystem>
#include <iostream>
#include <fstream>

#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

//DEMO
using namespace Windows::Foundation;
using namespace Windows::Storage::Streams;
using namespace Microsoft::UI::Xaml::Documents;
using namespace Microsoft::UI::Xaml::Controls;

namespace winrt::DemoBuildCpp::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();
    }

	void MainWindow::OnThemeRadioButtonChecked(IInspectable const& sender, RoutedEventArgs const& e)
	{
		RadioButton radiobutton = sender.as<RadioButton>();
		if (radiobutton != nullptr && radiobutton.Tag() != nullptr)
		{
			winrt::hstring selectedTheme = unbox_value<winrt::hstring>(radiobutton.Tag());

			SplitView splitView = radiobutton.XamlRoot().Content().as<SplitView>();
			if (winrt::to_string(selectedTheme) == "Dark") {
				splitView.RequestedTheme(Microsoft::UI::Xaml::ElementTheme::Dark);
			}
			else
			{
				splitView.RequestedTheme(Microsoft::UI::Xaml::ElementTheme::Light);
			}
		}
	}

	void MainWindow::OnRun(IInspectable const& sender, RoutedEventArgs const& e)
	{
		std::string list;
		winrt::hstring filePath = folderTextBox().Text();

		// Create a Path object from given path string
		std::filesystem::path pathObj(winrt::to_string(filePath));
		// Check if path exists and is of a regular file
		if (std::filesystem::exists(pathObj) && std::filesystem::is_directory(pathObj))
		{
			statusTextBlock().Text(L"Status: running");

			int depth = (int)numberBox().Value();

			list.clear();
			auto start = std::chrono::high_resolution_clock::now();
			ListDirectories(winrt::to_string(filePath), depth, list);
			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> diff = end - start;
			std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds> (diff);   // truncated

			richTextBlock().Blocks().Clear();
			Paragraph paragraph = Paragraph();
			Run run = Run();
			run.Text(winrt::to_hstring(list));
			paragraph.Inlines().Append(run);
			richTextBlock().Blocks().Append(paragraph);
			std::string status;
			status.append("Status: completed in ");
			status.append(std::to_string(ms.count()));
			status.append(" ms");
			statusTextBlock().Text(winrt::to_hstring(status));
		}
	}

	void MainWindow::ListDirectories(std::string dir, int depth, std::string& list)
	{
		if (depth < 0)
			return;
		std::filesystem::path pathObj(dir);
		for (auto& p : std::filesystem::directory_iterator(pathObj, std::filesystem::directory_options::follow_directory_symlink))
		{
			try
			{
				if (std::filesystem::is_directory(p))
				{
					list.append(p.path().generic_string());
					list.append("\n");

					ListDirectories(p.path().generic_string(), depth - 1, list);
				}
			}
			catch (const std::filesystem::filesystem_error& e) {}
		}
	}

	void MainWindow::OnHelp(IInspectable const& sender, RoutedEventArgs const& e)
	{
		teachingTip().IsOpen(true);
	}

	void MainWindow::OnSettings(IInspectable const& sender, RoutedEventArgs const& e)
	{
		splitView().IsPaneOpen(!splitView().IsPaneOpen());
	}

	void MainWindow::OnPaneOpened(SplitView const& sender, IInspectable const& args)
	{
		std::string currentTheme;
		if (sender.RequestedTheme() == ElementTheme::Default)
		{
			Application::Current().RequestedTheme() == ApplicationTheme::Dark ? currentTheme = "Dark" : currentTheme = "Light";
		}
		else
		{
			sender.RequestedTheme() == ElementTheme::Dark ? currentTheme = "Dark" : currentTheme = "Light";
		}
		for (auto uielememnt : themePanel().Children())
		{
			RadioButton radiobutton = uielememnt.as<RadioButton>();
			winrt::hstring selectedTheme = unbox_value<winrt::hstring>(radiobutton.Tag());
			if (winrt::to_string(selectedTheme) == currentTheme) {
				radiobutton.IsChecked(true);
				break;
			}
		}
	}

	fire_and_forget  MainWindow::PickFolder()
	{
		auto picker = winrt::Windows::Storage::Pickers::FolderPicker();
		HWND hwnd = GetWindowHandle();
		winrt::check_hresult(picker.as<IInitializeWithWindow>()->Initialize(hwnd));

		//File and Folder pickers on desktop requires nonempty filters
		picker.FileTypeFilter().Append(L"*");
		auto storageFolder = co_await picker.PickSingleFolderAsync();

		if (!storageFolder)
		{
			co_return;
		}
		folderTextBox().Text(storageFolder.Path());
	}

	void MainWindow::OnBrowse(IInspectable const& sender, RoutedEventArgs const& e)
	{
		PickFolder();
	}

	HWND MainWindow::GetWindowHandle()
	{
		HWND hwnd;
		Window window = this->try_as<Window>();
		window.as<IWindowNative>()->get_WindowHandle(&hwnd);
		return hwnd;
	}

	void MainWindow::OnWindowMinimize(IInspectable const& sender, RoutedEventArgs const& e)
	{
		HWND hwnd = GetWindowHandle();
		ShowWindow(hwnd, SW_MINIMIZE);
	}

	void MainWindow::OnWindowMaximize(IInspectable const& sender, RoutedEventArgs const& e)
	{
		HWND hwnd = GetWindowHandle();
		ShowWindow(hwnd, SW_MAXIMIZE);
	}

	void MainWindow::OnWindowRestore(IInspectable const& sender, RoutedEventArgs const& e)
	{
		HWND hwnd = GetWindowHandle();
		ShowWindow(hwnd, SW_RESTORE);
	}

	void MainWindow::OnSendToBottom(IInspectable const& sender, RoutedEventArgs const& e)
	{
		HWND hwnd = GetWindowHandle();
		SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	}

	void MainWindow::OnAbout(IInspectable const& sender, RoutedEventArgs const& e)
	{
		splitView().IsPaneOpen(false);

		std::string description = "";
		DWORD currentProcess = GetCurrentProcessId();
		description.append(PrintProcessNameAndID(currentProcess));
		cdTextBlock().Text(winrt::to_hstring(description));
		contentDialog().XamlRoot(gridRoot().XamlRoot());
		contentDialog().ShowAsync();
	}

	std::string MainWindow::PrintProcessNameAndID(DWORD processID)
	{
		std::string description = "";

		TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

		if (NULL != hProcess)
		{
			HMODULE hMod;
			DWORD cbNeeded;
			HMODULE hMods[1024];

			if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
			{
				for (int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
				{
					TCHAR szModName[MAX_PATH];

					// Get the full path to the module's file.

					if (GetModuleFileNameEx(hProcess, hMods[i], szModName,
						sizeof(szModName) / sizeof(TCHAR)))
					{
						// Print the process name and identifier.
						std::wstring des(szModName);
						description.append(des.begin(), des.end());
						description.append("\n");
					}
				}

			}

			// Release the handle to the process.
			CloseHandle(hProcess);
			return description;
		}
	}

	void MainWindow::OnKeyDownTextBox(IInspectable const& sender, Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e)
	{
		if (e.Key() == Windows::System::VirtualKey::Enter)
		{
			OnRun(nullptr, nullptr);
		}
	}
}
