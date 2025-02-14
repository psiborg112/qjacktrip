//*****************************************************************
/*
  JackTrip: A System for High-Quality Audio Network Performance
  over the Internet

  Copyright (c) 2024 Michael Dickey, Aaron Wyatt.

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation
  files (the "Software"), to deal in the Software without
  restriction, including without limitation the rights to use,
  copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following
  conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
  OTHER DEALINGS IN THE SOFTWARE.
*/
//*****************************************************************

/**
 * \file UserInterface.cpp
 * \author Michael Dickey, Aaron Wyatt
 * \date August 2024
 */

#include "UserInterface.h"
#include <QSettings>

#include "gui/qjacktrip.h"

#if !defined(NO_UPDATER) && !defined(__unix__)
#include "dblsqd/feed.h"
#include "dblsqd/update_dialog.h"
#endif  // !defined(NO_UPDATER) && !defined(__unix__)

#ifdef _WIN32
#include <psapi.h>
#include <tlhelp32.h>
#include <windows.h>

bool isRunFromCmd()
{
    // Get our parent process pid
    HANDLE h = NULL;
    PROCESSENTRY32 pe;
    ZeroMemory(&pe, sizeof(PROCESSENTRY32));
    DWORD pid  = GetCurrentProcessId();
    DWORD ppid = 0;
    pe.dwSize  = sizeof(PROCESSENTRY32);
    h          = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (Process32First(h, &pe)) {
        do {
            // Loop through the list of processes until we find ours.
            if (pe.th32ProcessID == pid) {
                ppid = pe.th32ParentProcessID;
                break;
            }
        } while (Process32Next(h, &pe));
    }
    CloseHandle(h);

    // Get the name of our parent process;
    char pname[MAX_PATH] = {0};
    DWORD size           = MAX_PATH;
    h                    = NULL;
    h                    = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, ppid);
    if (h) {
        if (QueryFullProcessImageNameA(h, 0, pname, &size)) {
            CloseHandle(h);

            // Check if our parent process is a command line.
            if (size >= 14 && strncmp(pname + size - 14, "powershell.exe", 14) == 0) {
                return true;
            }
            if (size >= 7 && strncmp(pname + size - 7, "cmd.exe", 7) == 0) {
                return true;
            }
            if (size >= 6 && strncmp(pname + size - 6, "wt.exe", 6) == 0) {
                return true;
            }
            // a few extras for msys/cygwin/etc
            if (size >= 8 && strncmp(pname + size - 8, "bash.exe", 8) == 0) {
                return true;
            }
            if (size >= 6 && strncmp(pname + size - 6, "sh.exe", 6) == 0) {
                return true;
            }
            if (size >= 7 && strncmp(pname + size - 7, "zsh.exe", 7) == 0) {
                return true;
            }
        } else {
            CloseHandle(h);
        }
    }

    return false;
}
#endif  // _WIN32

UserInterface::UserInterface(QSharedPointer<Settings>& settings) : m_cliSettings(settings)
{
}

UserInterface::~UserInterface()
{
    m_classic_ui.clear();
}

QCoreApplication* UserInterface::createApplication(int& argc, char* argv[])
{
#if defined(__unix__)
    // Check if X or Wayland environment variables are set.
    if (std::getenv("WAYLAND_DISPLAY") == nullptr && std::getenv("DISPLAY") == nullptr) {
        std::cout << "ERROR: Display not found. Make sure X or Wayland is running or "
                     "try running jacktrip in command line mode."
                  << std::endl;
        std::cout << "(To display a list of command line options run \"jacktrip -h\")"
                  << std::endl;
        std::exit(1);
    }
#endif

    QCoreApplication* app;
    app = QJackTrip::createApplication(argc, argv);
    app->setOrganizationName(QStringLiteral("psi-borg"));
    app->setOrganizationDomain(QStringLiteral("psi-borg.org"));
    app->setApplicationName(QStringLiteral("QJackTrip"));
    app->setApplicationVersion(gVersion);

    return app;
}

void UserInterface::start(QApplication* app)
{
#ifdef _WIN32
    // Remove the console that appears if we're on windows and not running from a
    // command line.
    if (!isRunFromCmd()) {
        std::cout << "This extra window is caused by a bug in Microsoft Windows. "
                  << "It can safely be ignored or closed." << std::endl
                  << std::endl
                  << "To fix this bug, please upgrade to the latest version of "
                  << "Windows Terminal available in the Microsoft App Store:" << std::endl
                  << "https://aka.ms/terminal" << std::endl;

        FreeConsole();
    }
#endif  // _WIN32

    m_classic_ui.reset(new QJackTrip(*this));
    QObject::connect(m_classic_ui.data(), &QJackTrip::signalExit, app,
                     &QCoreApplication::quit, Qt::QueuedConnection);
    m_classic_ui->show();

    QSettings settings;

#if !defined(NO_UPDATER) && !defined(__unix__)
#ifdef PSI
    //QString updateChannel =
        //settings.value(QStringLiteral("UpdateChannel"), "stable").toString().toLower();
    QString baseUrl = QStringLiteral("https://nuages.psi-borg.org/qjacktrip");
    // Setup auto-update feed
    dblsqd::Feed* feed = new dblsqd::Feed();
#ifdef _WIN32
    feed->setUrl(QUrl(QString("%1/%2-manifests.json").arg(baseUrl, "win")));
#endif
#ifdef Q_OS_MACOS
    feed->setUrl(QUrl(QString("%1/%2-manifests.json").arg(baseUrl, "mac")));
#endif
    if (feed) {
        dblsqd::UpdateDialog* updateDialog = new dblsqd::UpdateDialog(feed);
        updateDialog->setIcon(":/icon_32.png");
    }
#endif  // PSI
#endif  // !defined(NO_UPDATER) && !defined(__unix__)
}

void UserInterface::setMode(uiModeT m)
{
    //TODO: Refactor this code.
    if (m == MODE_VS) {
        std::cerr << "JackTrip was not built with support for Virtual Studio mode."
                  << std::endl;
    }
    m = MODE_CLASSIC;

    switch (m) {
    case MODE_UNSET:
    case MODE_CLASSIC:
        m_classic_ui->show();
        m_uiMode = MODE_CLASSIC;
        break;
    default:
        return;
    }
}

void UserInterface::enableNap()
{
#ifdef __APPLE__
    m_noNap.enableNap();
#endif
}

void UserInterface::disableNap()
{
#ifdef __APPLE__
    m_noNap.disableNap();
#endif
}
