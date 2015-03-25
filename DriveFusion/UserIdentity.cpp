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
#include "StdAfx.h"
#include "UserIdentity.h"
#include "DisposableHandle.h"

#include <comdef.h>
#include <stdio.h>
#include <stdarg.h>
#include <Windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <iostream>
#include <sstream>
#include <IO.h>
#include <limits>
#include <ctime>

int UserIdentity::LastError = ERROR_SUCCESS;
std::wstring UserIdentity::_UserName = L"";
std::wstring UserIdentity::_DomainName = L"";

#define MAX_NAME 256

bool UserIdentity::IsCurrentProcessElevated()
{
  LastError = ERROR_SUCCESS;

  TOKEN_ELEVATION_TYPE elevationType;
  DWORD returnLength;

  if (!GetTokenInformation(TokenElevationType, &elevationType, sizeof(elevationType), &returnLength))
    return false;

  return elevationType == TokenElevationTypeFull;  // elevated
}

DWORD UserIdentity::CreateProcess(std::wstring exePath, std::wstring arguments)
{
  LastError = ERROR_SUCCESS;

  STARTUPINFO startupInfo = {0};
  startupInfo.cb = sizeof(startupInfo);

  PROCESS_INFORMATION processInfo;

  if (!::CreateProcess(exePath.c_str(), (LPWSTR)arguments.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &startupInfo, &processInfo))
  {
    LastError = GetLastError();

    return 0;
  }

  CloseHandle(processInfo.hProcess);
  CloseHandle(processInfo.hThread);

  return processInfo.dwProcessId;
}

DWORD UserIdentity::CreateProcessWithToken(HANDLE token, std::wstring exePath, std::wstring arguments)
{
  LastError = ERROR_SUCCESS;

  std::wstring commandLine = exePath + L" " + arguments;

  STARTUPINFO startupInfo = {0};
  startupInfo.cb = sizeof(startupInfo);

  PROCESS_INFORMATION processInfo;

  if (!::CreateProcessWithTokenW(token, 0, exePath.c_str(), (LPWSTR)commandLine.c_str(), 0, NULL, NULL, &startupInfo, &processInfo))
  {
    LastError = GetLastError();

    return 0;
  }

  CloseHandle(processInfo.hProcess);
  CloseHandle(processInfo.hThread);

  return processInfo.dwProcessId;
}

DWORD UserIdentity::CreateProcessAsStandardUser(std::wstring exePath, std::wstring arguments)
{
  LastError = ERROR_SUCCESS;

  if (!IsCurrentProcessElevated())
    return CreateProcess(exePath, arguments);

  HANDLE tokenHandle = NULL;

  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &tokenHandle))
  {
    LastError = GetLastError();

    return 0;
  }

  DisposableHandle tokenDisposableHandle(tokenHandle);

  TOKEN_PRIVILEGES tokenPrivileges;

  tokenPrivileges.PrivilegeCount = 1;

  if (!LookupPrivilegeValueW(NULL, SE_INCREASE_QUOTA_NAME, &tokenPrivileges.Privileges[0].Luid))
  {
    LastError = GetLastError();

    return 0;
  }

  tokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

  if (!AdjustTokenPrivileges(tokenHandle, FALSE, &tokenPrivileges, 0, NULL, NULL))
  {
    LastError = GetLastError();

    return 0;
  }

  tokenDisposableHandle.Close();

  LastError = GetLastError();

  if (LastError != ERROR_SUCCESS)
    return 0;

  HWND windowHandle = GetShellWindow();

  if (windowHandle == NULL)
  {
    LastError = GetLastError();

    return 0;
  }

  DWORD windowPid = 0;

  GetWindowThreadProcessId(windowHandle, &windowPid);

  if (windowPid == 0)
  {
    LastError = GetLastError();

    return 0;
  }

  HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, windowPid);

  if (processHandle == NULL)
  {
    LastError = GetLastError();

    return 0;
  }

  DisposableHandle processDisposableHandle(processHandle);

  HANDLE processHandle2 = NULL;

  if (!OpenProcessToken(processHandle, TOKEN_DUPLICATE, &processHandle2))
  {
    LastError = GetLastError();

    return 0;
  }

  DisposableHandle processDisposableHandle2(processHandle2);

  const DWORD tokenRights = TOKEN_QUERY | TOKEN_ASSIGN_PRIMARY | TOKEN_DUPLICATE | TOKEN_ADJUST_DEFAULT | TOKEN_ADJUST_SESSIONID;

  HANDLE tokenHandle2 = NULL;

  if (!DuplicateTokenEx(processHandle2, tokenRights, NULL, SecurityImpersonation, TokenPrimary, &tokenHandle2))
  {
    LastError = GetLastError();

    return 0;
  }

  DisposableHandle tokenDisposableHandle2(tokenHandle2);

  return CreateProcessWithToken(tokenHandle2, exePath, arguments);
}

bool UserIdentity::GetTokenInformation(TOKEN_INFORMATION_CLASS tokenInformationClass, LPVOID tokenInformation, DWORD tokenInformationLength, PDWORD returnLength)
{
  HANDLE tokenHandle = NULL;

  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &tokenHandle))
  {
    LastError = GetLastError();

    return false;
  }

  DisposableHandle disposableHandle(tokenHandle);

  if (!::GetTokenInformation(tokenHandle, tokenInformationClass, tokenInformation, tokenInformationLength, returnLength))
  {
    LastError = GetLastError();

    return false;
  }

  return true;
}

const std::wstring& UserIdentity::UserName()
{
  GetUserNameAndDomain();

  return _UserName;
}

const std::wstring& UserIdentity::DomainName()
{
  GetUserNameAndDomain();

  return _DomainName;
}

bool UserIdentity::IsUserProcess(const DWORD processId)
{
  if (!GetUserNameAndDomain())
    return false;

  std::wstring processUserName;
  std::wstring processDomainName;

  if (!GetUserFromProcess(processId, processUserName, processDomainName))
    return false;

  int result = _wcsicmp(UserName().c_str(), processUserName.c_str());

  if (result != 0)
    return false;

  result = _wcsicmp(DomainName().c_str(), processDomainName.c_str());

  if (result != 0)
    return false;

  return true;
}

bool UserIdentity::GetUserNameAndDomain(PTSTR szUser, PDWORD pcchUser, PTSTR szDomain, PDWORD pcchDomain)
{

  HANDLE       hToken   = NULL;
  PTOKEN_USER  ptiUser  = NULL;
  DWORD        cbti     = 0;
  SID_NAME_USE snu;
  bool         result = false;

  __try
  {
    // Get the calling thread's access token.
    if (!OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, TRUE, &hToken))
    {
      if (GetLastError() != ERROR_NO_TOKEN)
        __leave;

      if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
        __leave;
    }

    // Obtain the size of the user information in the token.
    if (::GetTokenInformation(hToken, TokenUser, NULL, 0, &cbti))
    {
      // Call should have failed due to zero-length buffer.
      __leave;

    }
    else
    {
      // Call should have failed due to zero-length buffer.
      if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
        __leave;
    }

    // Allocate buffer for user information in the token.
    ptiUser = (PTOKEN_USER) HeapAlloc(GetProcessHeap(), 0, cbti);

    if (!ptiUser)
      __leave;

    // Retrieve the user information from the token.
    if (!::GetTokenInformation(hToken, TokenUser, ptiUser, cbti, &cbti))
      __leave;

    // Retrieve user name and domain name based on user's SID.
    if (!LookupAccountSid(NULL, ptiUser->User.Sid, szUser, pcchUser, szDomain, pcchDomain, &snu))
      __leave;

    result = true;
  }
  __finally
  {
    if (hToken)
      CloseHandle(hToken);

    if (ptiUser)
      HeapFree(GetProcessHeap(), 0, ptiUser);
  }

  return result;
}

bool UserIdentity::GetUserNameAndDomain(std::wstring& userName, std::wstring& domainName)
{
  TCHAR tempUserName[MAX_PATH];
  DWORD sizeUserName = MAX_PATH;
  TCHAR tempUserDomainName[MAX_PATH];
  DWORD sizeUserDomainName = MAX_PATH;

  if (GetUserNameAndDomain(&tempUserName[0], &sizeUserName, &tempUserDomainName[0], &sizeUserDomainName))
  {
    userName.assign(tempUserName);
    domainName.assign(tempUserDomainName);

    return true;
  }

  return false;
}

bool UserIdentity::GetUserNameAndDomain()
{
  if (_UserName.length() == 0)
  {
    if (!GetUserNameAndDomain(_UserName, _DomainName))
      return false;
  }

  return true;
}

bool UserIdentity::GetUserFromToken(HANDLE token, std::wstring& userName, std::wstring& domainName)
{
  PTOKEN_USER ptu = NULL;
  DWORD dwLength = 0;
  bool result = FALSE;

  __try
  {
    if (token == NULL)
      __leave;

    if (!::GetTokenInformation(
      token,          // handle to the access token
      TokenUser,      // get information about the token's groups
      (LPVOID) ptu,   // pointer to PTOKEN_USER buffer
      0,              // size of buffer
      &dwLength       // receives required buffer size
      ))
    {
      if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
        __leave;

      ptu = (PTOKEN_USER)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwLength);

      if (ptu == NULL)
        __leave;
    }

    if (!::GetTokenInformation(
      token,          // handle to the access token
      TokenUser,      // get information about the token's groups
      (LPVOID) ptu,   // pointer to PTOKEN_USER buffer
      dwLength,       // size of buffer
      &dwLength       // receives required buffer size
      ))
    {
      __leave;
    }

    SID_NAME_USE SidType;
    wchar_t lpName[MAX_NAME];
    wchar_t lpDomain[MAX_NAME];
    DWORD dwSize = MAX_NAME;

    if (!LookupAccountSid( NULL , ptu->User.Sid, lpName, &dwSize, lpDomain, &dwSize, &SidType))
    {
      DWORD dwResult = GetLastError();

      if (dwResult == ERROR_NONE_MAPPED)
      {
        Log::WriteOutput(LogType::Error, L"LookupAccountSid Error : NONE_MAPPED");
      }
      else
      {
        Log::WriteOutput(LogType::Error, L"LookupAccountSid Error : %u", GetLastError());
      }
    }
    else
    {
      userName = lpName;
      domainName = lpDomain;
      result = TRUE;
    }
  }
  __finally
  {
    if (ptu != NULL)
      HeapFree(GetProcessHeap(), 0, (LPVOID)ptu);
  }

  return result;
}

bool UserIdentity::GetUserFromProcess(const DWORD processId, std::wstring& userName, std::wstring& domainName)
{
  HANDLE hProcess = NULL;
  HANDLE hToken = NULL;
  BOOL result = FALSE;

  __try
  {
    hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);

    if (hProcess == NULL)
      __leave;

    if (!OpenProcessToken(hProcess, TOKEN_QUERY, &hToken))
      __leave;

    result = GetUserFromToken(hToken, userName,  domainName);
  }
  __finally
  {
    if (hToken != NULL)
      CloseHandle(hToken);
    if (hProcess != NULL)
      CloseHandle(hProcess);
  }

  return (result > 0);
}

DWORD UserIdentity::GetUserProcessId(const std::wstring& processName)
{
  if (!GetUserNameAndDomain())
    return 0;

  PROCESSENTRY32 processInfo;
  processInfo.dwSize = sizeof(processInfo);

  HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

  if (processesSnapshot == NULL || processesSnapshot == INVALID_HANDLE_VALUE)
    return NULL;

  DWORD processId = 0;

  Process32First(processesSnapshot, &processInfo);

  if (!processName.compare(processInfo.szExeFile))
  {
    if (IsUserProcess(processInfo.th32ProcessID))
    {
      processId = processInfo.th32ProcessID;
    }
  }

  if (processId == 0)
  {
    while (Process32Next(processesSnapshot, &processInfo))
    {
      if (!processName.compare(processInfo.szExeFile))
      {
        if (IsUserProcess(processInfo.th32ProcessID))
        {
          processId = processInfo.th32ProcessID;

          break;
        }
      }
    }
  }

  CloseHandle(processesSnapshot);

  return processId;
}

DWORD UserIdentity::GetUserSessionId()
{
  DWORD processId = GetCurrentProcessId();
  DWORD sessionId = 0;

  if (!ProcessIdToSessionId(processId, &sessionId))
    return 0;

  return sessionId;
}
