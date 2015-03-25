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
#include "DriveItem.h"
#include <stdio.h>

using namespace Fusion::GoogleDrive;

CDriveItem::CDriveItem()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::CDriveItem()");

  _fileInfo = NULL;
}

CDriveItem::CDriveItem(Fusion::GoogleDrive::FileInfo* fileInfo)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::CDriveItem(Fusion::GoogleDrive::FileInfo* fileInfo)");

  _fileInfo = NULL;

  FileInfo(fileInfo);
}

CDriveItem::~CDriveItem()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::~CDriveItem()");

  FileInfo(NULL);
}

Fusion::GoogleDrive::FileInfo* CDriveItem::FileInfo()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::FileInfo()");

  return _fileInfo;
}

void CDriveItem::FileInfo(Fusion::GoogleDrive::FileInfo* value)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::FileInfo(Fusion::GoogleDrive::FileInfo* value)");

  FileInfo::Release(&_fileInfo);

  _fileInfo = value;

  if (_fileInfo != NULL)
  {
    _fileInfo->AddRef();
  }
}

std::vector<FileInfo*>* CDriveItem::Files()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::Files()");

  return &_fileInfo->Files;
}

std::wstring CDriveItem::Id()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::Id()");

  return _fileInfo->Id;
}

bool CDriveItem::IsFolder()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::IsFolder()");

  return _fileInfo->IsFolder();
}

bool CDriveItem::IsFile()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::IsFile()");

  return _fileInfo->IsFile();
}

bool CDriveItem::IsGoogleDoc()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::IsGoogleDoc()");

  return _fileInfo->IsGoogleDoc();
}

bool CDriveItem::IsRoot()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::IsRoot()");

  return _fileInfo->IsRoot();
}

DriveItemType::eType CDriveItem::Type()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::Type()");

  if (IsRoot())
  {
    return DriveItemType::Root;
  }
  else if (IsFolder())
  {
    return DriveItemType::Folder;
  }
  else if (IsFile())
  {
    return DriveItemType::File;
  }
  else
  {
    return DriveItemType::None;
  }
}

bool CDriveItem::HasChildren()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::HasChildren()");

  bool hasChildren = false;

  if (_fileInfo->IsFolder())
  {
    hasChildren = _fileInfo->HasFoldersOrFiles();
  }
  else
  {
    hasChildren = false;
  }

  return hasChildren;
}

bool CDriveItem::HasSubFolder()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::HasSubFolder()");

  bool hasSubFolder = false;

  if (_fileInfo->IsFolder())
  {
    hasSubFolder = _fileInfo->HasFolders();
  }
  else
  {
    hasSubFolder = false;
  }

  return hasSubFolder;
}

std::wstring CDriveItem::ItemName()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::ItemName()");

  return _fileInfo->Title;
}

std::wstring CDriveItem::ItemPathDisplay()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::ItemPathDisplay()");

  std::wstring path = _fileInfo->FilePath();

  return path;
}

std::wstring CDriveItem::ItemFolderPathDisplay(const std::wstring& rootPath, bool includeSelf)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::ItemFolderPathDisplay(std::wstring& rootPath, bool includeSelf)");

  if (Type() == DriveItemType::None)
    return L"";


  if (IsRoot() || !includeSelf)
    return rootPath;

  std::wstring path = PathInfo::CombinePath(rootPath, _fileInfo->Title);

  return path;
}

std::wstring CDriveItem::ItemUrl()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::ItemUrl()");

  if (IsFolder())
    return L"";

  std::wstring path = _fileInfo->FilePath();

  path = PathInfo::GetFileUrl(path);

  return path;
}

std::wstring CDriveItem::ItemType()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::ItemType()");

  if (IsFolder())
  {
    return L"Folder";
  }
  else if (IsGoogleDoc())
  {
    return _fileInfo->MimeType;
  }
  else if (IsFile())
  {
    return _fileInfo->FileExtension;
  }
  else
  {
    return L"";
  }
}

std::wstring CDriveItem::ItemTypeText()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::ItemTypeText()");

  if (IsFolder())
  {
    return _fileInfo->Association.FileType;
  }
  else if (IsGoogleDoc())
  {
    return _fileInfo->Association.FileType;
  }
  else if (IsFile())
  {
    return _fileInfo->Association.FileType;
  }
  else
  {
    return L"";
  }
}

std::wstring CDriveItem::ParsingPath()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::ParsingPath()");

  std::wstring path = ItemPathDisplay();

  return path;
}

std::wstring CDriveItem::FileExtension()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::FileExtension()");

  if (IsFolder())
  {
    return L"";
  }
  else
  {
    return _fileInfo->FileExtension;
  }
}

ULONG CDriveItem::FileAttributes()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::FileAttributes()");

  ULONG value = FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_NOT_CONTENT_INDEXED;

  if (IsFolder())
  {
    value |= FILE_ATTRIBUTE_DIRECTORY;
  }

  if (!_fileInfo->Editable)
  {
    value |= FILE_ATTRIBUTE_READONLY;
  }

  return value;
}

std::wstring CDriveItem::DateModified()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::DateModified()");

  if (_fileInfo->ModifiedDate.length() > 0)
  {
    return _fileInfo->ModifiedDate;
  }
  else
  {
    return this->DateCreated();
  }
}

std::wstring CDriveItem::Size()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::Size()");

  if (!IsFile())
    return L"";

  if (IsFolder() || IsGoogleDoc())
    return L"";

  return std::to_wstring(_fileInfo->FileSize);
}

std::wstring CDriveItem::DateCreated()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::DateCreated()");

  return _fileInfo->CreatedDate;
}

std::wstring CDriveItem::DateAccessed()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::DateAccessed()");

  if (_fileInfo->LastViewedByMeDate.length() > 0)
  {
    return _fileInfo->LastViewedByMeDate;
  }
  else
  {
    return this->DateModified();
  }
}

std::wstring CDriveItem::Status()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::Status()");

  if (_fileInfo->Trashed)
  {
    return L"Trashed";
  }
  else if (_fileInfo->Shared)
  {
    return L"Shared";
  }
  else
  {
    return L"";
  }
}

BOOL CDriveItem::IsSendToTarget()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::IsSendToTarget()");

  if (IsFolder())
  {
    return false;
  }
  else if (IsFile())
  {
    return true;
  }
  else
  {
    return false;
  }
}

BOOL CDriveItem::TryWIN32FindData(WIN32_FIND_DATA& findData)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItem::WIN32FindData()");

  BOOL r = false;

  ZeroMemory(&findData, sizeof(WIN32_FIND_DATA));

  findData.dwFileAttributes = FileAttributes();
  findData.nFileSizeHigh = FileInfo()->FileSize >> 32;
  findData.nFileSizeLow = FileInfo()->FileSize & 0xFFFFFFFF;
  r = FileInfo::TryConvertDateStringToFILETIME(DateCreated(), findData.ftCreationTime);

  if (r)
  {
    r = FileInfo::TryConvertDateStringToFILETIME(DateAccessed(), findData.ftLastAccessTime);
  }

  if (r)
  {
    r = FileInfo::TryConvertDateStringToFILETIME(DateModified(), findData.ftLastWriteTime);
  }

  if (r)
  {
    std::wstring itemName = ItemName();
    r = wcsncpy_s(&findData.cFileName[0], MAX_PATH, itemName.c_str(),  _TRUNCATE) == ERROR_SUCCESS;
  }

  return r;
}

HRESULT CDriveItem::PreceivedType(__out std::wstring& preceivedString)
{
  try
  {
    HRESULT hr = S_OK;

    if (this->IsFile())
    {
      PERCEIVED perceivedType;
      PERCEIVEDFLAG perceivedTypeFlag;
      CComHeapPtr<WCHAR> perceivedTypeString;

      hr = AssocGetPerceivedType(this->FileExtension().c_str(), &perceivedType, &perceivedTypeFlag, &perceivedTypeString);

      if (SUCCEEDED(hr))
      {
        preceivedString.assign(perceivedTypeString);
      }
    }
    else
    {
      preceivedString.assign(L"Folder");
    }

    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CDriveItem::PreceivedType");

    return E_FAIL;
  }
}
