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
#include "XmlReader.h"
#include "ErrorInfo.h"

namespace Fusion
{
  namespace GoogleDrive
  {
    struct FileInfoType
    {
      enum eType
      {
        // Generic Types
        NonGoogleDoc = 0,

        // Google Doc Types
        NotImplemented = 100,
        None = 101,
        Root = 102,
        Folder = 103,
        Audio = 104,
        Document = 105,
        Drawing = 106,
        File = 107,
        Form = 108,
        FusionTable = 109,
        Photo = 110,
        Presentation = 111,
        Script = 112,
        Sites = 113,
        Spreadsheet = 114,
        Unknown = 115,
        Video = 116,
      };
    };

    struct FileInfoStatus
    {
      enum eType
      {
        Unknown = 0,
        InCloud = 1,
        OnDisk = 2,
        ModifiedOnDisk = 3
      };
    };

    struct FileInfoAssociation
    {
    public:
      std::wstring FileExtension;
      std::wstring FileType;
      std::wstring Application;
      std::wstring DefaultIcon;
      std::wstring DefaultIconIndex;
    };

    class FileInfo
    {
    public:
      friend class FileManager;

      static const std::wstring& ERROR_ROOTID();
      static const std::wstring& ERROR_FILEID();

      static FileInfo* CreateInstance();
      static FileInfo* CreateInstance(const std::wstring& xml, ErrorInfo** errorInfo);
      static FileInfo* CreateInstance(XmlReader* xmlReader, ErrorInfo** errorInfo);
      static void Release(FileInfo** fileInfo);

      int RefCount();
      void AddRef();
      void Release();

      void Copy(FileInfo* newFileInfo);
      bool HasFiles();
      bool HasFolders();
      bool HasFoldersAndFiles();
      bool HasFoldersOrFiles();

      bool Copyable;
      std::wstring CreatedDate;
      std::wstring Description;
      bool Editable;
      std::wstring FileExtension;
      unsigned long long FileSize;
      std::wstring Id;
      std::wstring LastModifyingUserName;
      std::wstring LastViewedByMeDate;
      std::wstring MimeType;
      std::wstring ModifiedByMeDate;
      std::wstring ModifiedDate;
      std::wstring OwnerName;
      std::wstring ParentId;
      bool Shared;
      std::wstring SharedWithMeDate;
      std::wstring Title;
      bool Trashed;

      std::wstring FileNameWithoutExtention;
      FileInfoAssociation Association;

      const std::wstring& FilePath() const;
      bool IsFile() const;
      bool IsFolder() const;
      bool IsGoogleDoc() const;
      bool IsRoot() const;
      FileInfoType::eType Type();

      static BOOL TryConvertDateStringToFILETIME(const std::wstring& value, FILETIME& fileTime);
      bool CreatePathTo();

      typedef std::vector<FileInfo*> FileList;
      FileList Files;

    protected:
      explicit FileInfo();
      ~FileInfo(void);

      void _Release(const std::wstring& id);
      void _ClearFiles();
      bool _Init(XmlReader* xmlReader, ErrorInfo** errorInfo);
      bool _LoadContent(XmlReader* xmlReader, ErrorInfo** errorInfo);

      std::wstring _filePath;
      FileInfoType::eType _type;
      bool _isContentLoaded;
      int _refCount;

      static std::wstring _ERROR_ROOTID;
      static std::wstring _ERROR_FILEID;
    };
  }
}

