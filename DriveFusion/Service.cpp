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
#include "Service.h"
#include "UserIdentity.h"

#include <ctime>

using namespace Fusion::GoogleDrive;

Service::Service(bool debug): _debug (debug), _errorInfo(NULL)
{
  Log::WriteOutput(LogType::Debug, L"Service::Service(bool debug)");
}

Service::~Service(void)
{
  Log::WriteOutput(LogType::Debug, L"Service::~Service(void)");

  _ClearError();
}

void Service::_ClearError() const
{
  Log::WriteOutput(LogType::Debug, L"Service::_ClearError()");

  if (_errorInfo != NULL)
  {
    delete _errorInfo;
    _errorInfo = NULL;
  }
}

ErrorInfo* Service::ErrorInfo() const
{
  Log::WriteOutput(LogType::Debug, L"Service::ErrorInfo()");

  return _errorInfo;
}

std::wstring Service::ErrorMessage() const
{
  Log::WriteOutput(LogType::Debug, L"Service::ErrorMessage()");

  if (_errorInfo != NULL)
    return _errorInfo->Message;

  return L"";
}

void Service::ErrorMessage(const std::wstring& value) const
{
  Log::WriteOutput(LogType::Debug, L"Service::ErrorMessage(const std::wstring& value)");

  if (_errorInfo == NULL)
  {
    _errorInfo = new Fusion::GoogleDrive::ErrorInfo();
  }

  _errorInfo->Message.assign(value);

  if (value.length() > 0)
  {
    std::wstring message = L"Error executing service - " + value;

    Log::WriteOutput(LogType::Error, message.c_str());
  }
}

bool Service::HasError() const
{
  Log::WriteOutput(LogType::Debug, L"Service::HasError()");

  if (_errorInfo != NULL)
    return true;

  return false;
}

bool Service::_StartServerAndOpenPipe(DisposableHandle* output)
{
  if (!_OpenPipeWithRetry(output, L"", true))
  {
    if (!_StartServer())
      return false;

    if (!_OpenPipeWithRetry(output))
      return false;
  }

  return true;
}

bool Service::_StartServer()
{
  bool alreadyRunning = false;

  if (!Wake(&alreadyRunning))
    return false;

  return true;
}

bool Service::Wake()
{
  bool alreadyRunning = false;

  return Wake(&alreadyRunning);
}

bool Service::Wake(bool* alreadyRunning)
{
  if (_IsServerRunning())
  {
    (*alreadyRunning) = true;

    return true;
  }

  Scoped_Lock lock(&_mutex);

  (*alreadyRunning) = false;

  Log::Debug(L"Attempting to start DriveProxy.Service.exe service");

  std::wstring filePath;

  if (!_GetGoogleDriveFilePath(&filePath))
  {
    Log::Error(ErrorMessage());

    return false;
  }

  Log::Information(L"Attempting to start ServicePipe process " + filePath);

  if (!UserIdentity::CreateProcessAsStandardUser(filePath, L" \"wake\""))
  {
    if (!UserIdentity::CreateProcess(filePath, L" \"wake\""))
    {
      DWORD err = GetLastError();

      std::wstring message = L"Create Process failed, win32 error code " + std::to_wstring((_ULonglong)err) + L" file path \'" + filePath + L"\'";

      ErrorMessage(message);

      return false;
    }
  }

  Log::Debug(L"Successfully started ServicePipe process");

  return true;
}

bool Service::_IsServerRunning() const
{
  DWORD processId = UserIdentity::GetUserProcessId(L"DriveProxy.Service.exe");

  if (processId > 0)
  {
    return true;
  }

  return false;
}

bool Service::_GetGoogleDriveFilePath(std::wstring* output) const
{
  TCHAR fusionGoogleDriveExe[MAX_PATH];
  size_t length = MAX_PATH;

  _tgetenv_s(&length, fusionGoogleDriveExe, L"DriveProxy.Service.exe");

  std::wstring filePath;

  if (fusionGoogleDriveExe != NULL)
  {
    filePath = fusionGoogleDriveExe;
  }

  if (!PathInfo::FileExists(filePath))
  {
    // Look up the dll path via registry
    LSTATUS lRes = ERROR_CANCELLED;
    WCHAR szBuffer[MAX_PATH];
    HKEY hKey;

    lRes = RegOpenKeyExW(HKEY_CLASSES_ROOT, L"CLSID\\{30323693-0E1E-4365-99FB-5074A5C6F273}\\InprocServer32", 0, KEY_READ, &hKey);

    if (lRes == ERROR_SUCCESS)
    {
      std::wstring strValueOfBinDir;
      std::wstring strKeyDefaultValue;

      DWORD dwBufferSize = sizeof(szBuffer);

      lRes = RegQueryValueExW(hKey, L"", 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);

      if (ERROR_SUCCESS == lRes)
      {
        lRes = ERROR_CANCELLED;

        // cut off file name and appead target filename
        if (PathRemoveFileSpec(szBuffer))
        {
          if (PathAppend(szBuffer, L"DriveProxy.Service.exe"))
          {
            filePath = szBuffer;
          }
        }
      }
    }
  }

  if (!PathInfo::FileExists(filePath))
  {
    TCHAR programFilesX86[MAX_PATH];
    SHGetSpecialFolderPath(NULL, (LPWSTR)&programFilesX86, CSIDL_PROGRAM_FILESX86, FALSE);

    filePath = PathInfo::CombinePath(programFilesX86, L"Google\\" DriveService_PRODUCT_WSTR L"\\DriveProxy.Service.exe");
  }

  if (!PathInfo::FileExists(filePath))
  {
    TCHAR programFiles[MAX_PATH];
    SHGetSpecialFolderPath(NULL, (LPWSTR)&programFiles, CSIDL_PROGRAM_FILES, FALSE);

    filePath = PathInfo::CombinePath(programFiles, L"Google\\" DriveService_PRODUCT_WSTR L"\\DriveProxy.Service.exe");
  }

  if (!PathInfo::FileExists(filePath))
  {
    ErrorMessage(L"Could not find installed GoogleDrive server");

    return false;
  }

  output->assign(filePath);

  return true;
}

bool Service::_OpenPipeWithRetry(DisposableHandle* output, const std::wstring& pipeName, bool shortRetry) const
{
  int retryCount = 30;

  if (shortRetry)
  {
    retryCount = 10;
  }

  for (int i = 0; i < retryCount; i++)
  {
    if (_OpenPipe(output, pipeName))
    {
      if (i > 0)
      {
        Log::WriteOutput(LogType::Error, L"Service::_OpenPipeWithRetry() retried %d times", i);
      }

      return true;
    }

    ::Sleep(100);
  }

  return false;
}

bool Service::_OpenPipe(DisposableHandle* output, const std::wstring& pipeName) const
{
  Log::WriteOutput(LogType::Debug, L"Service::_OpenPipe(DisposableHandle* output, const std::wstring& pipeName)");

  _ClearError();

  output->Value(NULL);

  std::wstring tempName;

  if (pipeName.length() == 0)
  {
    if (UserIdentity::UserName().length() == 0)
    {
      std::wstring message = L"User name could be found";

      ErrorMessage(message);

      return false;
    }

    std::wstring sessionId = std::to_wstring((_ULonglong)UserIdentity::GetUserSessionId());

    tempName = L"\\\\.\\pipe\\DriveProxy.Service_" + UserIdentity::UserName() + L"." + UserIdentity::DomainName() + L"." + sessionId;
  }
  else
  {
    tempName = L"\\\\.\\pipe\\DriveProxy.Service_" + pipeName;
  }

  Log::Information(L"Attempting to open pipe " + tempName);

  HANDLE handle = ::CreateFile(tempName.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_HIDDEN, NULL);

  if (handle == NULL || handle == INVALID_HANDLE_VALUE)
  {
    handle = ::CreateFile(tempName.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, TRUNCATE_EXISTING, FILE_ATTRIBUTE_HIDDEN, NULL);
  }

  if (handle == NULL || handle == INVALID_HANDLE_VALUE)
  {
    handle = ::CreateFile(tempName.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, NULL);
  }

  if (handle == NULL || handle == INVALID_HANDLE_VALUE)
  {
    DWORD err = GetLastError();

    std::wstring message = L"Open pipe failed, win32 error code " + std::to_wstring((_ULonglong)err);

    Log::Error(L"Attempt to open pipe " + tempName + L" failed");

    ErrorMessage(message);

    return false;
  }

  output->Value(handle);

  return true;
}

bool Service::_ReadFromServerWithRetry(const DisposableHandle& handle, std::wstring* output) const
{
  Log::Debug(L"Attempting to read message from ServicePipe process");

  DWORD length = 8;
  std::wstring header;

  if (!_ReadFromServerWithRetry(handle, length, &header))
    return false;

  length = std::stoul(header);

  if (length == 0)
  {
    output->assign(L"");

    return true;
  }

  if (!_ReadFromServerWithRetry(handle, length, output))
    return false;

  Log::Information(L"Received: " + *output);
  Log::Debug(L"Successfully read message from ServicePipe process");

  return true;
}

bool Service::_ReadFromServerWithRetry(const DisposableHandle& handle, DWORD length, std::wstring* output) const
{
  for (int i = 0; i < 300; i++)
  {
    if (_ReadFromServer(handle, length, output))
      return true;

    ::Sleep(10);
  }

  return false;
}

bool Service::_ReadFromServer(const DisposableHandle& handle, DWORD length, std::wstring* output) const
{
  _ClearError();

  length *= 2;

  DWORD waitInSeconds = 10;
  DWORD waitInMilliseconds = waitInSeconds * 1000;
  DWORD waited = 0;

  while (true)
  {
    DWORD bytesRead = 0;
    DWORD bytesAvail = 0;
    DWORD bytesLeft = 0;

    if (!::PeekNamedPipe(handle.Value(), NULL, 0, &bytesRead, &bytesAvail, &bytesLeft))
    {
      DWORD err = GetLastError();

      std::wstring message = L"Peek at server failed, win32 error code " + std::to_wstring((_ULonglong)err);

      ErrorMessage(message);

      return false;
    }

    if (bytesAvail >= length)
      break;

    if (waited >= waitInMilliseconds)
    {
      std::wstring message = L"Read from server timed out after " + std::to_wstring((_ULonglong)waitInSeconds) + L" seconds";

      ErrorMessage(message);

      return false;
    }

    int sleep = 1;

    if (waitInMilliseconds > 50)
    {
      sleep = 50;
    }

    ::Sleep(sleep);

    waited += sleep;
  }

  char bytes[4096];
  DWORD bytesLength = sizeof(bytes);

  if (length < bytesLength)
  {
    bytesLength = length;
  }

  DWORD bytesRead = 0;
  DWORD totalBytesRead = 0;
  std::wstring data;

  while (true)
  {
    if (!::ReadFile(handle.Value(), bytes, bytesLength, &bytesRead, NULL))
    {
      DWORD err = GetLastError();

      std::wstring message = L"Read pipe failed, win32 error code " + std::to_wstring((_ULonglong)err);

      ErrorMessage(message);

      return false;
    }

    if (bytesRead <= 0)
    {
      DWORD err = GetLastError();

      std::wstring message = L"No data read from pipe, win32 error code " + std::to_wstring((_ULonglong)err);

      ErrorMessage(message);

      return false;
    }

    if (bytesRead % 2 == 1)
    {
      DWORD err = GetLastError();

      std::wstring message = L"Odd data was received from pipe, is this unicode?" + std::to_wstring((_ULonglong)err);

      ErrorMessage(message);

      return false;
    }

    data.append((WCHAR*)bytes, bytesRead / sizeof(WCHAR));

    totalBytesRead += bytesRead;

    if (totalBytesRead >= length)
      break;
  }

  if (totalBytesRead < length)
  {
    std::wstring message = L"Not enough data read from pipe";

    ErrorMessage(message);

    return false;
  }

  output->assign(data);

  return true;
}

bool Service::_WriteToServerWithRetry(const DisposableHandle& handle, const std::wstring& message) const
{
  for (int i = 0; i < 300; i++)
  {
    if (_WriteToServer(handle, message))
      return true;

    ::Sleep(10);
  }

  return false;
}

bool Service::_WriteToServer(const DisposableHandle& handle, const std::wstring& message) const
{
  _ClearError();

  Log::WriteOutput(LogType::Debug, L"Service::_WriteToServer(): %s", message.c_str());
  std::wstring data = std::to_wstring((_ULonglong)message.length());

  while (data.length() < 8)
  {
    data += L" ";
  }

  data += message;

  DWORD bytesWritten = 0;

  if (data.size() > (std::numeric_limits<DWORD>::max)() / sizeof(wchar_t))
  {
    ErrorMessage(L"Data length exceeds maximum message size for a pipe write.");

    return false;
  }

  if (!::WriteFile(handle.Value(), data.c_str(), (DWORD)data.size() * sizeof(wchar_t), &bytesWritten, 0))
  {
    DWORD err = GetLastError();

    ErrorMessage(L"Write to pipe failed, win32 error code " + std::to_wstring((_ULonglong)err));

    return false;
  }

  return true;
}

bool Service::_PingServer() const
{
  Log::Information(L"Attempting to ping ServicePipe process");

  _ClearError();

  DisposableHandle handle;

  if (!_OpenPipeWithRetry(&handle))
    return false;

  std::wstring message = L"ping";

  if (!_WriteToServerWithRetry(handle, message))
    return false;

  std::wstring output;

  if (!_ReadFromServerWithRetry(handle, &output))
    return false;

  if (output != L"hello")
  {
    ErrorMessage(L"Received unexpected response from server.");

    return false;
  }

  Log::Information(L"Successfully pinged ServicePipe process");

  return true;
}

bool Service::_Execute(MethodInfo* methodInfo, std::wstring* output)
{
  Log::WriteOutput(LogType::Debug, L"Service::_Execute(MethodInfo* methodInfo, std::wstring* output)");

  return _ExecuteWhileWaiting(methodInfo, output);
}

bool Service::_ExecuteWhileWaiting(MethodInfo* methodInfo, std::wstring* output)
{
  Log::WriteOutput(LogType::Debug, L"Service::_ExecuteWhileWaiting(MethodInfo* methodInfo, std::wstring* output)");
  Log::WriteOutput(LogType::Information, L"Sending: %s args: %s", MethodType::ToString(methodInfo->Type).c_str(), methodInfo->ToString().c_str());


  unsigned int startTime = clock();

  _ClearError();

  DisposableHandle handle;

  if (!_StartServerAndOpenPipe(&handle))
  {
    _LogTimerDuration(methodInfo->Type, startTime);
    return false;
  }

  if (!_WriteToServerWithRetry(handle, L"redirect"))
  {
    _LogTimerDuration(methodInfo->Type, startTime);
    return false;
  }

  std::wstring pipeName;

  if (!_ReadFromServerWithRetry(handle, &pipeName))
  {
    _LogTimerDuration(methodInfo->Type, startTime);
    return false;
  }

  handle.Close();

  if (!_OpenPipeWithRetry(&handle, pipeName))
  {
    _LogTimerDuration(methodInfo->Type, startTime);
    return false;
  }

  std::wstring commandLine;

  if (_debug)
  {
    commandLine = L" Debug," + methodInfo->ToString();
  }
  else
  {
    commandLine = L" " + methodInfo->ToString();
  }

  if (!_WriteToServerWithRetry(handle, commandLine))
  {
    _LogTimerDuration(methodInfo->Type, startTime);
    return false;
  }

  if (!_ReadFromServerWithRetry(handle, output))
  {
    _LogTimerDuration(methodInfo->Type, startTime);
    return false;
  }

  Log::WriteOutput(LogType::Information, L"Received: %s", output->c_str());
  if (HasError() || output->length() == 0)
  {
    _LogTimerDuration(methodInfo->Type, startTime);
    return false;
  }

  XmlReader xmlReader = XmlReader((*output));

  if (!xmlReader.MoveToNextElement())
  {
    ErrorMessage(L"Output did not contain valid XML data.");

    _LogTimerDuration(methodInfo->Type, startTime);
    return false;
  }
  else if (!xmlReader.IsEndElement() && xmlReader.ElementName() == L"Error")
  {
    _errorInfo = new Fusion::GoogleDrive::ErrorInfo(&xmlReader);

    _LogTimerDuration(methodInfo->Type, startTime);
    return false;
  }

  _LogTimerDuration(methodInfo->Type, startTime);

  return true;
}

void Service::_LogTimerDuration(MethodType::eType type, UINT startTime)
{
  const std::wstring typeString = MethodType::ToString(type);

  Log::WriteOutput(LogType::Debug, L"Method %s required %d ms to complete roundtrip", typeString.c_str(), clock() - startTime);
}

LogInfo* Service::GetLog()
{
  LogInfo* logInfo = LogInfo::CreateInstance();

  if (HasError() || logInfo == NULL)
    return NULL;

  return logInfo;
}

AboutInfo* Service::GetAbout()
{
  MethodInfo methodInfo(MethodType::GetAbout);

  std::wstring xml;

  if (!_Execute(&methodInfo, &xml))
    return NULL;

  AboutInfo* aboutInfo = AboutInfo::CreateInstance(xml, &_errorInfo);

  if (HasError() || aboutInfo == NULL)
    return NULL;

  return aboutInfo;
}

bool Service::Authenticate()
{
  Log::WriteOutput(LogType::Information, L"Service::Authenticate()");

  MethodInfo methodInfo(MethodType::Authenticate);

  std::wstring result;

  _Execute(&methodInfo, &result);

  return !HasError();
}

FileInfo* Service::GetFiles(const std::wstring& id, bool updateCache, bool getChildren, bool ignoreError)
{
  Log::WriteOutput(LogType::Information, L"Service::GetFiles(const std::wstring& id, bool updateCache, bool getChildren, bool ignoreError)");

  MethodInfo methodInfo(MethodType::GetFiles);
  methodInfo.AddParameter(id);

  methodInfo.AddParameter(L"1");

  if (updateCache)
  {
    methodInfo.AddParameter(L"1");
  }
  else
  {
    methodInfo.AddParameter(L"0");
  }

  if (getChildren)
  {
    methodInfo.AddParameter(L"1");
  }
  else
  {
    methodInfo.AddParameter(L"0");
  }

  if (ignoreError)
  {
    methodInfo.AddParameter(L"1");
  }
  else
  {
    methodInfo.AddParameter(L"0");
  }

  std::wstring xml;

  if (!_Execute(&methodInfo, &xml))
    return NULL;

  FileInfo* fileInfo = FileInfo::CreateInstance(xml, &_errorInfo);

  if (HasError() || fileInfo == NULL)
    return NULL;

  return fileInfo;
}

FileInfo* Service::DownloadFile(const std::wstring& id)
{
  Log::WriteOutput(LogType::Information, L"Service::DownloadFile(HWND hwnd, const std::wstring& id)");

  MethodInfo methodInfo(MethodType::DownloadFile);
  methodInfo.AddParameter(id);

  std::wstring xml;

  if (!_Execute(&methodInfo, &xml))
    return false;

  FileInfo* fileInfo = FileInfo::CreateInstance(xml, &_errorInfo);

  if (HasError() || fileInfo == NULL)
    return NULL;

  return fileInfo;
}

FileInfo* Service::UploadFile(const std::wstring& id, const std::wstring& filePath)
{
  Log::WriteOutput(LogType::Information, L"Service::UploadFile(HWND hwnd, const std::wstring& id, const std::wstring& filePath)");

  MethodInfo methodInfo(MethodType::UploadFile);
  methodInfo.AddParameter(id);
  methodInfo.AddParameter(filePath);

  std::wstring xml;

  if (!_Execute(&methodInfo, &xml))
    return false;

  FileInfo* fileInfo = FileInfo::CreateInstance(xml, &_errorInfo);

  if (HasError() || fileInfo == NULL)
    return NULL;

  return fileInfo;
}

FileInfo* Service::RenameFile(const std::wstring& id, const std::wstring& name)
{
  Log::WriteOutput(LogType::Information, L"Service::RenameFile(HWND hwnd, const std::wstring& id, const std::wstring& name)");

  MethodInfo methodInfo(MethodType::RenameFile);
  methodInfo.AddParameter(id);
  methodInfo.AddParameter(name);

  std::wstring xml;

  if (!_Execute(&methodInfo, &xml))
    return false;

  FileInfo* fileInfo = FileInfo::CreateInstance(xml, &_errorInfo);

  if (HasError() || fileInfo == NULL)
    return NULL;

  return fileInfo;
}

FileInfo* Service::InsertFile(const std::wstring& parentId, const std::wstring& filename, bool isFolder)
{
  Log::WriteOutput(LogType::Information, L"Service::InsertFile(HWND hwnd, const std::wstring& parentId, const std::wstring& filename)");

  MethodInfo methodInfo(MethodType::InsertFile);
  methodInfo.AddParameter(parentId);
  methodInfo.AddParameter(filename);

  if (isFolder)
  {
    methodInfo.AddParameter(L"application/vnd.google-apps.folder");
  }
  else
  {
    methodInfo.AddParameter(L""); // mime type, let the service figure it out from the extension for now
  }

  std::wstring xml;

  if (!_Execute(&methodInfo, &xml))
    return false;

  FileInfo* fileInfo = FileInfo::CreateInstance(xml, &_errorInfo);

  if (HasError() || fileInfo == NULL)
    return NULL;

  return fileInfo;
}

std::unique_ptr<std::vector<std::wstring>> Service::TrashFiles(const std::vector<std::wstring>& ids)
{
  Log::WriteOutput(LogType::Information, L"Service::TrashFiles(HWND hwnd, const std::vector<std::wstring>& ids)");

  MethodInfo methodInfo(MethodType::TrashFiles);
  methodInfo.AddParameter(ids);

  std::wstring xml;

  if (!_Execute(&methodInfo, &xml))
    return false;

  XmlReader xmlReader(xml);

  std::unique_ptr<std::vector<std::wstring>> deletedIds (new std::vector<std::wstring>(xmlReader.GetElementValues(L"Id")));

  return deletedIds;
}

std::unique_ptr<std::vector<std::wstring>> Service::MoveFiles(const std::vector<std::wstring>& ids, const std::wstring& dstId)
{
  Log::WriteOutput(LogType::Information, L"Service::MoveFiles(HWND hwnd, const std::vector<std::wstring>& ids)");

  MethodInfo methodInfo(MethodType::MoveFiles);
  methodInfo.AddParameter(dstId);
  methodInfo.AddParameter(ids);

  std::wstring xml;

  if (!_Execute(&methodInfo, &xml))
    return false;

  XmlReader xmlReader(xml);

  std::unique_ptr<std::vector<std::wstring>> movedIds (new std::vector<std::wstring>(xmlReader.GetElementValues(L"Id")));

  return movedIds;
}

std::unique_ptr<std::vector<std::wstring>> Service::CopyFiles(const std::vector<std::wstring>& ids, const std::wstring& dstId)
{
  Log::WriteOutput(LogType::Information, L"Service::CopyFiles(HWND hwnd, const std::vector<std::wstring>& ids)");

  MethodInfo methodInfo(MethodType::CopyFiles);
  methodInfo.AddParameter(dstId);
  methodInfo.AddParameter(ids);

  std::wstring xml;

  if (!_Execute(&methodInfo, &xml))
    return false;

  XmlReader xmlReader(xml);

  std::unique_ptr<std::vector<std::wstring>> copiedIds (new std::vector<std::wstring>(xmlReader.GetElementValues(L"Id")));

  return copiedIds;
}

