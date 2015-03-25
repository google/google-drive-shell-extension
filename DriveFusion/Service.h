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
#include <string>
#include <vector>
#include <windows.h>
#include <stdio.h>
#include "MethodInfo.h"
#include "ErrorInfo.h"
#include "FileInfo.h"
#include "LogInfo.h"
#include "Scoped_Mutex.h"
#include "DisposableHandle.h"

namespace Fusion
{
  namespace GoogleDrive
  {
    class Service
    {
    public:
      Service(bool debug);
      ~Service(void);

      ErrorInfo* ErrorInfo() const;
      std::wstring ErrorMessage() const;
      void ErrorMessage(const std::wstring& message) const;
      bool HasError() const;

      LogInfo* GetLog();
      AboutInfo* GetAbout();

      bool Wake();
      bool Wake(bool* alreadyRunning);
      bool Authenticate();
      FileInfo* GetFiles(const std::wstring& id, bool updateCache, bool getChildren, bool ignoreError);
      FileInfo* DownloadFile(const std::wstring& id);
      FileInfo* UploadFile(const std::wstring& id, const std::wstring& filePath);
      FileInfo* RenameFile(const std::wstring& id, const std::wstring& name);
      FileInfo* InsertFile(const std::wstring& parentId, const std::wstring& filename, bool isFolder);
      std::unique_ptr<std::vector<std::wstring>> TrashFiles(const std::vector<std::wstring>& ids);
      std::unique_ptr<std::vector<std::wstring>> MoveFiles(const std::vector<std::wstring>& ids, const std::wstring& dstId);
      std::unique_ptr<std::vector<std::wstring>> CopyFiles(const std::vector<std::wstring>& ids, const std::wstring& dstId);

    protected:
      void _ClearError() const;
      bool _Execute(MethodInfo* methodInfo, std::wstring* output);
      bool _ExecuteWhileWaiting(MethodInfo* methodInfo, std::wstring* output);

      static void _LogTimerDuration(MethodType::eType type, UINT startTime);

      bool _debug;
      Scoped_Mutex _mutex;
      mutable Fusion::GoogleDrive::ErrorInfo* _errorInfo;

      bool _StartServerAndOpenPipe(DisposableHandle* output);
      bool _StartServer();
      bool _IsServerRunning() const;
      bool _GetGoogleDriveFilePath(std::wstring* output) const;
      bool _OpenPipeWithRetry(DisposableHandle* output, const std::wstring& pipeName = L"", bool shortRetry = false) const;
      bool _OpenPipe(DisposableHandle* output, const std::wstring& pipeName = L"") const;
      bool _PingServer() const;
      bool _ReadFromServerWithRetry(const DisposableHandle& handle, std::wstring* output) const;
      bool _ReadFromServerWithRetry(const DisposableHandle& handle, DWORD length, std::wstring* output) const;
      bool _ReadFromServer(const DisposableHandle& handle, DWORD length, std::wstring* output) const;
      bool _WriteToServerWithRetry(const DisposableHandle& handle, const std::wstring& message) const;
      bool _WriteToServer(const DisposableHandle& handle, const std::wstring& message) const;
    };
  }
}

