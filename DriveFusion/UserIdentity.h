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

class UserIdentity
{
private:
  UserIdentity(void)
  {
  }
  ~UserIdentity(void)
  {
  }

public:
  static bool IsCurrentProcessElevated();

  static DWORD CreateProcess(std::wstring exePath, std::wstring arguments);
  static DWORD CreateProcessWithToken(HANDLE token, std::wstring exePath, std::wstring arguments);
  static DWORD CreateProcessAsStandardUser(std::wstring exePath, std::wstring arguments);

  static int LastError;

  static const std::wstring& UserName();
  static const std::wstring& DomainName();

  static bool IsUserProcess(const DWORD processId);
  static bool GetUserFromToken(HANDLE token, std::wstring& userName, std::wstring& domainName);
  static bool GetUserFromProcess(const DWORD processId, std::wstring& userName, std::wstring& domainName);
  static DWORD GetUserProcessId(const std::wstring& processName);
  static DWORD GetUserSessionId();

private:
  static bool GetTokenInformation(TOKEN_INFORMATION_CLASS tokenInformationClass, LPVOID tokenInformation, DWORD tokenInformationLength, PDWORD returnLength);
  static bool GetUserNameAndDomain();
  static bool GetUserNameAndDomain(std::wstring& userName, std::wstring& domainName);
  static bool GetUserNameAndDomain(PTSTR szUser, PDWORD pcchUser, PTSTR szDomain, PDWORD pcchDomain);

  static std::wstring _UserName;
  static std::wstring _DomainName;
};
