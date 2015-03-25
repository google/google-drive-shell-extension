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
#include "DriveItemSignature.h"

using namespace Fusion::GoogleDrive;

struct DriveItemType
{
  enum eType
  {
    None = 0,
    Root = 1,
    Folder = 2,
    File = 3
  };
};

class CDriveItem
{
public:
  CDriveItem();
  CDriveItem(FileInfo* fileInfo);
  ~CDriveItem();

  Fusion::GoogleDrive::FileInfo* FileInfo();
  void FileInfo(Fusion::GoogleDrive::FileInfo* value);
  std::vector<Fusion::GoogleDrive::FileInfo*>* Files();

  std::wstring Id();
  bool HasSubFolder();
  bool HasChildren();
  bool IsFile();
  bool IsFolder();
  bool IsGoogleDoc();
  bool IsRoot();
  DriveItemType::eType Type();

  std::wstring ItemName();
  std::wstring ItemPathDisplay();
  std::wstring ItemFolderPathDisplay(const std::wstring& rootPath, bool includeSelf);
  std::wstring ItemUrl();
  std::wstring ItemType();
  std::wstring ItemTypeText();

  std::wstring ParsingPath();

  std::wstring FileExtension();
  ULONG FileAttributes();
  std::wstring DateModified();
  std::wstring Size();
  std::wstring DateCreated();
  std::wstring DateAccessed();
  std::wstring Status();
  BOOL IsSendToTarget();

  BOOL TryWIN32FindData(WIN32_FIND_DATA& findData);

  HRESULT PreceivedType(__out std::wstring& preceivedType);

private:
  Fusion::GoogleDrive::FileInfo* _fileInfo;
};


