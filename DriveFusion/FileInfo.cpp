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
#include "FileInfo.h"

using namespace Fusion::GoogleDrive;

std::wstring FileInfo::_ERROR_ROOTID = L"0";
std::wstring FileInfo::_ERROR_FILEID = L"ERROR";

FileInfo::FileInfo(void)
{
  Log::WriteOutput(LogType::Debug, L"FileInfo::FileInfo(void)");

  Copyable = false;
  Editable = false;
  Shared = false;
  Trashed = false;

  _type = FileInfoType::None;

  _isContentLoaded = false;
  _refCount = 0;
}

FileInfo::~FileInfo(void)
{
  Log::WriteOutput(LogType::Debug, L"FileInfo::~FileInfo(void)");

  _ClearFiles();
}

int FileInfo::RefCount()
{
  Log::WriteOutput(LogType::Debug, L"FileInfo::RefCount()");

  return _refCount;
}

void FileInfo::AddRef()
{
  Log::WriteOutput(LogType::Debug, L"FileInfo::AddRef()");

  _refCount++;
}

void FileInfo::Release()
{
  Log::WriteOutput(LogType::Debug, L"FileInfo::Release()");

  if (_refCount < 0)
    throw L"FileInfo::_refCount is less than zero.";

  _refCount--;
}

void FileInfo::_Release(const std::wstring& id)
{
  Log::WriteOutput(LogType::Debug, L"FileInfo::_Release(std::wstring id)");

  for (size_t i = 0; i < Files.size(); i++)
  {
    FileInfo* fileInfo = Files.at(i);

    fileInfo->_Release(id);

    if (fileInfo->Id == id)
    {
      Files.erase(Files.begin() + i);

      FileInfo::Release(&fileInfo);

      i--;
    }
  }
}

void FileInfo::Release(FileInfo** fileInfo)
{
  Log::WriteOutput(LogType::Debug, L"FileInfo::Release(FileInfo** fileInfo)");

  if (fileInfo != NULL && (*fileInfo) != NULL)
  {
    (*fileInfo)->Release();

    if ((*fileInfo)->RefCount() == 0)
    {
      delete (*fileInfo);
    }

    (*fileInfo) = NULL;
  }
}

const std::wstring& FileInfo::ERROR_ROOTID()
{
  Log::WriteOutput(LogType::Debug, L"FileInfo::ERROR_ROOTID()");

  return _ERROR_ROOTID;
}

const std::wstring& FileInfo::ERROR_FILEID()
{
  Log::WriteOutput(LogType::Debug, L"FileInfo::ERROR_FILEID()");

  return _ERROR_FILEID;
}

FileInfo* FileInfo::CreateInstance()
{
  Log::WriteOutput(LogType::Debug, L"FileInfo::CreateInstance()");

  FileInfo* fileInfo = new FileInfo();

  fileInfo->AddRef();

  return fileInfo;
}

FileInfo* FileInfo::CreateInstance(const std::wstring& xml, ErrorInfo** errorInfo)
{
  Log::WriteOutput(LogType::Debug, L"FileInfo::CreateInstance(std::wstring xml, ErrorInfo** errorInfo)");

  XmlReader xmlReader(xml);

  xmlReader.MoveToNextElement();

  return CreateInstance(&xmlReader, errorInfo);
}

FileInfo* FileInfo::CreateInstance(XmlReader* xmlReader, ErrorInfo** errorInfo)
{
  Log::WriteOutput(LogType::Debug, L"FileInfo::CreateInstance(XmlReader* xmlReader, ErrorInfo** errorInfo)");

  FileInfo* fileInfo = CreateInstance();

  if (!fileInfo->_Init(xmlReader, errorInfo))
  {
    FileInfo::Release(&fileInfo);

    return NULL;
  }

  return fileInfo;
}

bool FileInfo::_Init(XmlReader* xmlReader, ErrorInfo** errorInfo)
{
  Log::WriteOutput(LogType::Debug, L"FileInfo::_Init(XmlReader* xmlReader, ErrorInfo** errorInfo)");

  if (xmlReader->ElementName() != L"File" || xmlReader->IsEndElement())
  {
    (*errorInfo) = ErrorInfo::FromMessage(L"Could not load file - Expected beginning element of 'File'.");

    return false;
  }

  Copyable = xmlReader->GetAttributeValue(L"Copyable", Copyable);
  CreatedDate = xmlReader->GetAttributeValue(L"CreatedDate", CreatedDate);
  Description = xmlReader->GetAttributeValue(L"Description", Description);
  Editable = xmlReader->GetAttributeValue(L"Editable", Editable);
  FileExtension = xmlReader->GetAttributeValue(L"FileExtension", FileExtension);

  Id = xmlReader->GetAttributeValue(L"Id", Id);
  LastModifyingUserName = xmlReader->GetAttributeValue(L"LastModifyingUserName", LastModifyingUserName);
  LastViewedByMeDate = xmlReader->GetAttributeValue(L"LastViewedByMeDate", LastViewedByMeDate);
  MimeType = xmlReader->GetAttributeValue(L"MimeType", MimeType);
  ModifiedByMeDate = xmlReader->GetAttributeValue(L"ModifiedByMeDate", ModifiedByMeDate);
  ModifiedDate = xmlReader->GetAttributeValue(L"ModifiedDate", ModifiedDate);
  OwnerName = xmlReader->GetAttributeValue(L"OwnerName", OwnerName);
  ParentId = xmlReader->GetAttributeValue(L"ParentId", ParentId);
  Shared = xmlReader->GetAttributeValue(L"Shared", Shared);
  SharedWithMeDate = xmlReader->GetAttributeValue(L"SharedWithMeDate", SharedWithMeDate);
  Title = xmlReader->GetAttributeValue(L"Title", Title);
  Trashed = xmlReader->GetAttributeValue(L"Trashed", Trashed);

  FileNameWithoutExtention = xmlReader->GetAttributeValue(L"FileName", FileNameWithoutExtention);
  Association.FileExtension = xmlReader->GetAttributeValue(L"AssociationFileExtension", Association.FileExtension);
  Association.FileType = xmlReader->GetAttributeValue(L"AssociationFileType", Association.FileType);
  Association.Application = xmlReader->GetAttributeValue(L"AssociationApplication", Association.Application);
  Association.DefaultIcon = xmlReader->GetAttributeValue(L"AssociationDefaultIcon", Association.DefaultIcon);
  Association.DefaultIconIndex = xmlReader->GetAttributeValue(L"AssociationDefaultIconIndex", Association.DefaultIconIndex);

  _filePath = xmlReader->GetAttributeValue(L"FilePath", _filePath);

  std::wstring type = xmlReader->GetAttributeValue(L"Type", L"");
  _type = (FileInfoType::eType)wcstoul(type.c_str(), NULL, 0);

  std::wstring fileSizeString = xmlReader->GetAttributeValue(L"FileSize", L"");

  if (fileSizeString.length() == 0)
  {
    FileSize = 0;
  }
  else
  {
    FileSize = std::stoull(fileSizeString);
  }

  return _LoadContent(xmlReader, errorInfo);
}

bool FileInfo::_LoadContent(XmlReader* xmlReader, ErrorInfo** errorInfo)
{
  Log::WriteOutput(LogType::Debug, L"FileInfo::_LoadContent(XmlReader* xmlReader, ErrorInfo** errorInfo)");

  _ClearFiles();

  _isContentLoaded = false;

  if (xmlReader->ElementName() != L"File" || xmlReader->IsEndElement())
  {
    (*errorInfo) = ErrorInfo::FromMessage(L"Could not load file content - Expected ending element of 'File'.");

    return false;
  }

  if (!xmlReader->MoveToNextElement())
  {
    (*errorInfo) = ErrorInfo::FromMessage(L"Could not load file content - Could not move to next/end element in 'File'.");

    return false;
  }

  if (xmlReader->ElementName() == L"File" && xmlReader->IsEndElement())
    return true;

  while (xmlReader->MoveToNextElement())
  {
    if (xmlReader->IsEndElement())
    {
      if (xmlReader->ElementName() == L"File")
        break;
    }
    else
    {
      if (xmlReader->ElementName() == L"Files")
      {
      }
      else if (xmlReader->ElementName() == L"File")
      {
        FileInfo* fileInfo = FileInfo::CreateInstance(xmlReader, errorInfo);

        if (fileInfo == NULL)
          return false;

        Files.push_back(fileInfo);
      }
    }
  }

  _isContentLoaded = true;

  return true;
}

void FileInfo::_ClearFiles()
{
  Log::WriteOutput(LogType::Debug, L"FileInfo::_ClearFiles()");

  while (!Files.empty())
  {
    FileInfo* fileInfo = Files.at(0);

    Files.erase(Files.begin());

    fileInfo->_ClearFiles();

    Release(&fileInfo);
  }
}

void FileInfo::Copy(FileInfo* newFileInfo)
{
  Log::WriteOutput(LogType::Debug, L"FileInfo::Copy(FileInfo* newFileInfo)");

  Copyable = newFileInfo->Copyable;
  CreatedDate = newFileInfo->CreatedDate;
  Description = newFileInfo->Description;
  Editable = newFileInfo->Editable;
  FileExtension = newFileInfo->FileExtension;
  FileSize = newFileInfo->FileSize;
  Id = newFileInfo->Id;
  LastModifyingUserName = newFileInfo->LastModifyingUserName;
  LastViewedByMeDate = newFileInfo->LastViewedByMeDate;
  MimeType = newFileInfo->MimeType;
  ModifiedByMeDate = newFileInfo->ModifiedByMeDate;
  ModifiedDate = newFileInfo->ModifiedDate;
  OwnerName = newFileInfo->OwnerName;
  ParentId = newFileInfo->ParentId;
  Shared = newFileInfo->Shared;
  SharedWithMeDate = newFileInfo->SharedWithMeDate;
  Title = newFileInfo->Title;
  Trashed = newFileInfo->Trashed;

  FileNameWithoutExtention = newFileInfo->FileNameWithoutExtention;
  Association.FileExtension = newFileInfo->Association.FileExtension;
  Association.FileType = newFileInfo->Association.FileType;
  Association.Application = newFileInfo->Association.Application;
  Association.DefaultIcon = newFileInfo->Association.DefaultIcon;
  Association.DefaultIconIndex = newFileInfo->Association.DefaultIconIndex;

  _ClearFiles();

  if (newFileInfo->_isContentLoaded)
  {
    if (newFileInfo->Files.size() > 0)
    {
      for (size_t i = 0; i < Files.size(); i++)
      {
        FileInfo* childFileInfo = Files[i];
        bool found = false;

        for (size_t j = 0; j < newFileInfo->Files.size(); j++)
        {
          FileInfo* newChildFileInfo = newFileInfo->Files[j];

          if (childFileInfo->Id == newChildFileInfo->Id)
          {
            childFileInfo->Copy(newChildFileInfo);

            found = true;

            break;
          }
        }

        if (!found)
        {
          Files.erase(Files.begin() + i);

          FileInfo::Release(&childFileInfo);

          i--;
        }
      }

      for (size_t i = 0; i < newFileInfo->Files.size(); i++)
      {
        FileInfo* newChildFileInfo = newFileInfo->Files[i];
        bool found = false;

        for (size_t j = 0; j < Files.size(); j++)
        {
          FileInfo* childFileInfo = Files[j];

          if (newChildFileInfo->Id == childFileInfo->Id)
          {
            found = true;

            break;
          }
        }

        if (!found)
        {
          FileInfo* childFileInfo = FileInfo::CreateInstance();

          childFileInfo->Copy(newChildFileInfo);

          if (i >= Files.size())
          {
            Files.push_back(childFileInfo);
          }
          else
          {
            Files.insert(Files.begin() + i, childFileInfo);
          }
        }
      }
    }

    _isContentLoaded = newFileInfo->_isContentLoaded;
  }

  _filePath = newFileInfo->_filePath;
  _type = newFileInfo->_type;
}

bool FileInfo::HasFiles()
{
  Log::WriteOutput(LogType::Debug, L"FileInfo::HasFiles()");

  for (size_t i = 0; i < Files.size(); i++)
  {
    FileInfo* file = Files.at(i);

    if (file->IsFile())
      return true;
  }

  return false;
}

bool FileInfo::HasFolders()
{
  Log::WriteOutput(LogType::Debug, L"FileInfo::HasFolders()");

  for (size_t i = 0; i < Files.size(); i++)
  {
    FileInfo* file = Files.at(i);

    if (file->IsFolder())
      return true;
  }

  return false;
}

bool FileInfo::HasFoldersAndFiles()
{
  Log::WriteOutput(LogType::Debug, L"FileInfo::HasFoldersAndFiles()");

  bool hasDirectory = false;
  bool hasFile = false;

  for (size_t i = 0; i < Files.size(); i++)
  {
    FileInfo* file = Files.at(i);

    if (file->IsFolder())
    {
      hasDirectory = true;
    }
    else if (file->IsFile())
    {
      hasFile = true;
    }

    if (hasDirectory && hasFile)
      return true;
  }

  return false;
}

bool FileInfo::HasFoldersOrFiles()
{
  Log::WriteOutput(LogType::Debug, L"FileInfo::HasFoldersOrFiles()");

  if (Files.size() > 0)
    return true;

  return false;
}

const std::wstring& FileInfo::FilePath() const
{
  Log::WriteOutput(LogType::Debug, L"FileInfo::FilePath()");

  return _filePath;
}

bool FileInfo::IsFile() const
{
  Log::WriteOutput(LogType::Debug, L"FileInfo::IsFile()");

  if (!IsFolder() && _type != FileInfoType::None)
    return true;

  return false;
}

bool FileInfo::IsFolder() const
{
  Log::WriteOutput(LogType::Debug, L"FileInfo::IsFolder()");

  if (IsRoot() || _type == FileInfoType::Folder)
  {
    return true;
  }

  return false;
}

bool FileInfo::IsGoogleDoc() const
{
  Log::WriteOutput(LogType::Debug, L"FileInfo::IsGoogleDoc()");

  if (IsFile() && _type != FileInfoType::NonGoogleDoc)
    return true;

  return false;
}

bool FileInfo::IsRoot() const
{
  Log::WriteOutput(LogType::Debug, L"FileInfo::IsRoot()");

  if (_type == FileInfoType::Root)
    return true;

  return false;
}

FileInfoType::eType FileInfo::Type()
{
  Log::WriteOutput(LogType::Debug, L"FileInfo::Type()");

  return _type;
}

BOOL FileInfo::TryConvertDateStringToFILETIME(const std::wstring& value, FILETIME& fileTime)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemStream::GetStatFileTime(std::wstring& value)");

  ZeroMemory(&fileTime, sizeof(FILETIME));

  fileTime.dwHighDateTime = 0;
  fileTime.dwLowDateTime = 0;

  if (value.length() == 23)
  {
    std::wstring year = value.substr(0, 4);
    std::wstring month = value.substr(5, 2);
    std::wstring day = value.substr(8, 2);
    std::wstring hour = value.substr(11, 2);
    std::wstring minute = value.substr(14, 2);
    std::wstring second = value.substr(17, 2);
    std::wstring milli = value.substr(20, 3);

    SYSTEMTIME systemTime;

    systemTime.wYear = (unsigned short)std::stoul(year);
    systemTime.wMonth = (unsigned short)std::stoul(month);
    systemTime.wDay = (unsigned short)std::stoul(day);
    systemTime.wHour = (unsigned short)std::stoul(hour);
    systemTime.wMinute = (unsigned short)std::stoul(minute);
    systemTime.wSecond = (unsigned short)std::stoul(second);
    systemTime.wMilliseconds = (unsigned short)std::stoul(milli);

    return SystemTimeToFileTime(&systemTime, &fileTime);
  }
  else
  {
    Log::WriteOutput(LogType::Error, L"CDriveItemStream::GetStatFileTime invalid string length");
    return false;
  }
}

bool FileInfo::CreatePathTo()
{
  std::wstring directory;
  const size_t last_slash_idx = this->FilePath().rfind('\\');
  if (std::string::npos != last_slash_idx)
  {
    directory = this->FilePath().substr(0, last_slash_idx);
  }

  if (!CreateDirectory(directory.c_str(), NULL))
  {
    if (GetLastError() != ERROR_ALREADY_EXISTS)
    {
      return false;
    }
  }

  return true;
}
