/*
Copyright 2014 Google Inc

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include "stdafx.h"
#include "Log.h"
#include "Scoped_Mutex.h"
#include <string>
#include <time.h>
#include <sstream>
#include <fstream>

namespace
{
  const auto fallbackLogLevel = LogType::Information | LogType::Warning | LogType::Error;

  std::wstring LogPath()
  {
    time_t unixTime;
    time(&unixTime);
    struct tm localDate;
    if ( localtime_s( &localDate, &unixTime ) != 0 )
    {
      return L"Google\\Drive Fusion\\Logs\\log.dat";
    }
    TCHAR timeBuffer[] = L"YYYY-MM-DD HHMMSS";
    if ( wcsftime( timeBuffer, sizeof(timeBuffer)/sizeof(timeBuffer[0]), L"%Y-%m-%d %H%M%S", &localDate ) == 0 )
    {
      return L"Google\\Drive Fusion\\Logs\\log.dat";
    }
    return ( L"Google\\Drive Fusion\\Logs\\log." + std::wstring(timeBuffer) + L".dat" );
  }

  std::wstring BuildLogFileName()
  {
    PWSTR appDataPath = NULL;
    if( SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &appDataPath) == S_OK )
    {
      std::wstring logFile = PathInfo::CombinePath(appDataPath, LogPath());
      CoTaskMemFree(appDataPath);
      return logFile;
    }
    else
    {
      return L"C:\\log.dat";
    }
  }

  LogType::eType GetLogLevel()
  {
    DWORD logLevel = fallbackLogLevel;
    HKEY driveFusionKey;
    if (RegOpenKeyEx(
            HKEY_CURRENT_USER,
            L"Software\\Google\\Drive Fusion",
            0,
            KEY_QUERY_VALUE,
            &driveFusionKey) != ERROR_SUCCESS)
      return LogType::eType(logLevel);

    DWORD sizeLogLevel = sizeof(logLevel);
    RegGetValue(
        driveFusionKey,
        NULL,
        L"logLevel",
        RRF_RT_REG_DWORD,
        NULL,
        &logLevel,
        &sizeLogLevel);

    return LogType::eType(logLevel);
  }
}

LogType::eType Log::_level = GetLogLevel();
std::wstring Log::_lastError = L"";
bool Log::_shouldLogToFile = true;
std::wstring Log::_logFile = BuildLogFileName();

void Log::Debug(const std::wstring& message)
{
  WriteOutput(LogType::Debug, L"%s", message.c_str());
}

void Log::Information(const std::wstring& message)
{
  WriteOutput(LogType::Information, L"%s", message.c_str());
}

void Log::Warning(const std::wstring& message)
{
  WriteOutput(LogType::Warning, L"%s", message.c_str());
}

void Log::Error(const std::wstring& message)
{
  WriteOutput(LogType::Error, L"%s", message.c_str());
}

void Log::WriteOutput(LogType::eType logType, const wchar_t* szFormat, ...)
{
  if ((_level & logType) == 0)
  {
    return;
  }

  std::wstringstream message;

  message << TITTLE_STR << L"(" << GetCurrentThreadId() << L") - ";

  time_t timeRaw;
  time(&timeRaw);
  TCHAR timeBuffer[26] = {};
  _wctime_s( timeBuffer,sizeof(timeBuffer)/sizeof(timeBuffer[0]),&timeRaw);
  std::wstring timeValue(timeBuffer);

  timeValue.erase(timeValue.size() - 1); // remove trailing newline

  message << timeValue;

  if (logType == LogType::Error)
  {
    message << L" - !!ERROR!!";
  }
  else if (logType == LogType::Error)
  {
    message << L" - !!EXCEPTION!!";
  }

  message << L" - ";

  wchar_t szBuff[4096] = {0};
  va_list arg = NULL;
  va_start(arg, szFormat);
  _vsnwprintf_s(szBuff, _TRUNCATE, szFormat, arg);
  va_end(arg);

  std::wstring buffer = (LPCWSTR)&szBuff;

  message << buffer;
  message << L"\n";

  std::wstring output = message.str();

#ifdef DEBUG
  OutputDebugString(output.c_str());
#endif

  if (logType == LogType::Error)
  {
    _lastError = output;
  }

  if (_shouldLogToFile && _logFile.length() > 0)
  {
    static Scoped_Mutex mutex;
    Scoped_Lock lock(&mutex);

    PathInfo::CreatePath(_logFile);
    std::wofstream myfile(_logFile, std::ofstream::app);
    myfile.write(output.c_str(), output.length());
  }
}

void Log::LogUnknownIID(REFIID riid, LPTSTR lpszFunction)
{
  ATL::CComHeapPtr<WCHAR> temp1;
  ATL::CComHeapPtr<WCHAR> temp2;

  HRESULT hr2 = StringFromCLSID(riid, &temp1);

  if (!SUCCEEDED(hr2))
  {
    Log::WriteOutput(LogType::Error, L"%s not handled (riid=? class=?)", lpszFunction, temp1, temp2);
  }
  else
  {
    hr2 = ProgIDFromCLSID(riid, &temp2);

    if (!SUCCEEDED(hr2))
    {
      Log::WriteOutput(LogType::Error, L"%s not handled (riid=%s class=?)", lpszFunction, temp1);
    }
    else
    {
      Log::WriteOutput(LogType::Error, L"%s not handled (riid=%s class=%s)", lpszFunction, temp1, temp2);
    }
  }
}

HRESULT Log::HRFromWin32(BOOL didNotFail, LPTSTR lpszFunction)
{
  HRESULT hr = S_OK;

  if (didNotFail == false)
  {
    DWORD dw = GetLastError();
    hr = HRESULT_FROM_WIN32(dw);

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;

    auto charCount = FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER |
      FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      dw,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPTSTR)&lpMsgBuf,
      0, NULL);

    // Display the error message and exit the process
    if (charCount != 0)
    {
      lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));

      if (lpDisplayBuf != NULL)
      {
        StringCchPrintf((LPTSTR)lpDisplayBuf,
          LocalSize(lpDisplayBuf) / sizeof(TCHAR),
          TEXT("%s failed with error %d: %s"),
          lpszFunction, dw, lpMsgBuf);

        Log::WriteOutput(LogType::Error, L"%s", lpDisplayBuf);

        LocalFree(lpMsgBuf);
        LocalFree(lpDisplayBuf);
      }
    }
  }

  return hr;
}
