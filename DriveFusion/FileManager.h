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
#include "Service.h"
#include "Scoped_Mutex.h"
#include <map>

namespace Fusion
{
  namespace GoogleDrive
  {
    class FileManager
    {
    public:
      static std::wstring RootId;
      static bool IsRootId(const std::wstring& id);

    private:
      explicit FileManager(bool debug);

    public:
      static FileManager CreateFileManager(bool debug);
      ~FileManager(void);

      ErrorInfo* ErrorInfo() const;
      std::wstring ErrorMessage() const;
      void ErrorMessage(const std::wstring& message);
      bool HasError() const;

      const LogInfo* GetLog();
      const AboutInfo* GetAbout();

      bool WakeService();
      bool AuthenticateService();
      FileInfo* GetFile(std::wstring id, bool updateCache, bool getChildren, bool ignoreError);

      bool DownloadFile(FileInfo* fileInfo);
      bool UploadFile(FileInfo* fileInfo);
      bool RenameFile(FileInfo* fileInfo, const std::wstring& name);
      bool DeleteFiles(const std::vector<std::wstring>& ids, std::vector<std::wstring>*const deletedIds);
      bool InsertFile(const std::wstring& parentId, const std::wstring& fileName, bool isFolder, FileInfo** fileInfo);
      bool MoveFiles(const std::vector<std::wstring>& ids, const std::wstring& dstId, std::vector<std::wstring>*const copiedIds);
      bool CopyFiles(const std::vector<std::wstring>& ids, const std::wstring& dstId, std::vector<std::wstring>*const copiedIds);

    protected:
      void _Clear();
      FileInfo* _GetFiles(const std::wstring& id, bool updateCache, bool getChildren, bool ignoreError);

      Service _service;

      typedef std::map<const std::wstring, FileInfo*> FileMap;
      typedef FileMap::value_type FileMapItem;
      typedef std::map<const std::wstring, const std::wstring> FileIdMap;
      typedef FileIdMap::value_type FileIdMapItem;

      FileMap _folders;
      // Note: for folders this also stores a reference to themself, to simplify lookup logic in other places
      FileIdMap _filesToParent;
      FileMap _ignoredIds;
      void _MoveFromCacheToIgnored(const std::wstring& id);

      Scoped_Mutex _mutex;
      bool _wakeService;
      LogInfo* _log;
      AboutInfo* _about;
      FileInfo* _fileInfo;
    };
  }
}

