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
#pragma once

#include "stdafx.h"
#include <Guiddef.h>

struct LogType
{
  enum eType
  {
    None = 0,
    Information = 1<<1,
    Warning = 1<<2,
    Error = 1<<3,
    Debug = 1<<4,
    Test = 1<<5,
    All = Information | Warning | Error | Debug,
  };
};

class Log
{
public:
  Log();
  ~Log(void);

public:
  static void Debug(const std::wstring& message);
  static void Information(const std::wstring& message);
  static void Warning(const std::wstring& message);
  static void Error(const std::wstring& message);
  static void WriteOutput(LogType::eType logType, const wchar_t* szFormat, ...);

  static void LogUnknownIID(REFIID riid, LPTSTR lpszFunction);
  static HRESULT HRFromWin32(BOOL didNotFail, LPTSTR lpszFunction);

private:
  static LogType::eType _level;
  static std::wstring _lastError;
  static bool _shouldLogToFile;
  static std::wstring _logFile;
};
