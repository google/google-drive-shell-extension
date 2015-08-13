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
#include "FileManager.h"

using namespace Fusion::GoogleDrive;

std::wstring FileManager::RootId = L"root";

bool FileManager::IsRootId(const std::wstring& id)
{
  if (id.length() == 0 ||
    id.compare(RootId) == 0)
  {
    return true;
  }

  return false;
}

FileManager::FileManager(bool debug) : _service(debug), _folders(), _filesToParent()
{
  Log::WriteOutput(LogType::Debug, L"FileManager::FileManager(bool debug)");

  _wakeService = true;
  _log = NULL;
  _fileInfo = NULL;
}

FileManager FileManager::CreateFileManager(bool debug)
{
  return FileManager(debug);
}

FileManager::~FileManager(void)
{
  Log::WriteOutput(LogType::Debug, L"FileManager::~FileManager(void)");

  _Clear();

  if (_log != NULL)
  {
    delete _log;
    _log = NULL;
  }

  if (_fileInfo != NULL)
  {
    FileInfo::Release(&_fileInfo);
  }
}

ErrorInfo* FileManager::ErrorInfo() const
{
  Log::WriteOutput(LogType::Debug, L"FileManager::ErrorInfo()");

  return _service.ErrorInfo();
}

std::wstring FileManager::ErrorMessage() const
{
  Log::WriteOutput(LogType::Debug, L"FileManager::ErrorMessage()");

  return _service.ErrorMessage();
}

void FileManager::ErrorMessage(const std::wstring& message)
{
  Log::WriteOutput(LogType::Debug, L"FileManager::ErrorMessage(std::wstring message)");

  _service.ErrorMessage(message);
}

bool FileManager::HasError() const
{
  Log::WriteOutput(LogType::Debug, L"FileManager::HasError()");

  return _service.HasError();
}

bool FileManager::WakeService()
{
  Log::WriteOutput(LogType::Debug, L"FileManager::WakeService()");

  if (!_wakeService)
    return true;

  if (!_service.Wake())
    return false;

  _wakeService = false;

  return true;
}

bool FileManager::AuthenticateService()
{
  Log::WriteOutput(LogType::Debug, L"FileManager::AuthenticateService()");

  return _service.Authenticate();
}

void FileManager::_Clear()
{
  Log::WriteOutput(LogType::Debug, L"FileManager::_Clear()");

  for(FileMap::iterator it = _folders.begin(); it != _folders.end(); it++)
  {
    FileInfo::Release(&(it->second));
  }

  for(FileMap::iterator it = _ignoredIds.begin(); it != _ignoredIds.end(); it++)
  {
    FileInfo::Release(&(it->second));
  }

  _folders.clear();
  _filesToParent.clear();
  _ignoredIds.clear();
}

const LogInfo* FileManager::GetLog()
{
  if (_log == NULL)
  {
    _log = _service.GetLog();
  }
    
  return _log;
}

const AboutInfo* FileManager::GetAbout()
{
  if (_about == NULL)
  {
    _about = _service.GetAbout();
  }

  return _about;
}

FileInfo* FileManager::GetFile(std::wstring id, bool updateCache, bool getChildren, bool ignoreError)
{
  // Note: lets try always refreshing folders
  Log::WriteOutput(LogType::Debug, L"FileManager::GetFile %s, updateCache=%d, getChildren=%d, ignoreError=%d", id.c_str(), updateCache, getChildren, ignoreError);

  // public function should all be locked
  Scoped_Lock serviceLock(&_mutex);

  if (GetLog() == nullptr)
    return nullptr;

  if (IsRootId(id) && RootId.length() > 0)
  {
    id = RootId;
  }

  FileInfo* file = NULL;

  if (_fileInfo != NULL && !updateCache)
  {
    if (IsRootId(id) && _fileInfo->IsRoot())
    {
      file = _fileInfo;
    }
    else if (_fileInfo->Id == id)
    {
      file = _fileInfo;
    }
    else if (!getChildren)
    {
      for (size_t i = 0; i < _fileInfo->Files.size(); i++)
      {
        FileInfo* childInfo = _fileInfo->Files.at(i);

        if (childInfo->Id == id)
        {
          file = childInfo;

          break;
        }
      }
    }
  }

  if (file == NULL)
  {
    if (IsRootId(id))
    {
      // The root is well known
      if (updateCache || RootId.length() == 0)
      {
        file = _GetFiles(RootId, updateCache, getChildren, ignoreError);

        if (file == NULL)
          return file;
      }
      else
      {
        FileMap::iterator it = _folders.find(RootId);

        if (it != _folders.end())
        {
          file = it->second;
        }
      }
    }
    else
    {
      FileIdMap::iterator it = _filesToParent.find(id);

      if (updateCache || it == _filesToParent.end()) // we really want an update, the file is not known
      {
        FileMap::iterator ignored = _ignoredIds.find(id);

        if (updateCache || ignored == _ignoredIds.end()) // we really want an update, the file is not ignored
        {
          // don't know enough, get all the data for the id
          file = _GetFiles(id, updateCache, getChildren, ignoreError);
        }
        else
        {
          file = ignored->second;
        }
      }
      else
      {
        FileMap::iterator folderIt = _folders.find(it->second);

        if (folderIt != _folders.end())
        {
          // This is a a cached folder
          if (id == folderIt->second->Id)
          {
            if (updateCache)
            {
              file = _GetFiles(id, updateCache, getChildren, ignoreError);
            }
            else
            {
              file = folderIt->second;
            }
          }
          else
          {
            // Search the folder for a matching id
            for(FileInfo::FileList::iterator itemIt = folderIt->second->Files.begin(); itemIt != folderIt->second->Files.end(); itemIt++)
            {
              // this could return a file or an uncached folder
              if ((*itemIt)->Id == id)
              {
                file = (*itemIt);

                if (file->IsFolder() && _folders.find(file->Id) == _folders.end()) // Folder has not been cached before
                {
                  // Cache folder
                  // Slowing down process and does not look like this is required
                  // file = _GetFiles(id, updateCache, getChildren);
                }

                break;
              }
            }
          }
        }
      }
    }

    // Could not find the item, maybe its an unknown item, try one final time
    if (file == NULL)
    {
      file = _GetFiles(id, updateCache, getChildren, ignoreError);
    }
  }

  if (file == NULL || file->Trashed == true) // I'm not sure why I am having to check this right now.. lets see if it fixes the issue I'm having
  {
    // File definitely doesn't exist
    if (!IsRootId(id) && _ignoredIds.find(id) == _ignoredIds.end())
    {
      if (file != NULL)
      {
        file->AddRef();
      }

      _ignoredIds.insert(FileMapItem(id, file));

      Log::Error(L"FileManager::GetFile() We were asked to lookup an id that we are sure doesn't exist, adding id to the ignore list");
    }
  }

  if (file != NULL && updateCache && getChildren)
  {
    if (_fileInfo != NULL)
    {
      FileInfo::Release(&_fileInfo);
    }

    _fileInfo = file;

    _fileInfo->AddRef();
  }

  return file;
}

FileInfo* FileManager::_GetFiles(const std::wstring& id, bool updateCache, bool getChildren, bool ignoreError)
{
  Log::WriteOutput(LogType::Debug, L"FileManager::_GetFiles(FileInfo* fileInfo, bool updateCache, bool getChildren, bool ignoreError)");

  // fileInfo returns with 1 reference by default, even though its not actually referenced yet
  // I'm not going to change that logic for now

  FileInfo* fileInfo = _service.GetFiles(id, updateCache, getChildren, ignoreError);

  if (fileInfo != NULL)
  {
    // Add or Update known file
    if (fileInfo->IsRoot() && RootId.length() == 0)
    {
      RootId = fileInfo->Id;
    }

    if (fileInfo->IsFolder())
    {
      FileIdMap::iterator parentIt = _filesToParent.find(fileInfo->Id);

      if (parentIt != _filesToParent.end())
      {
        _filesToParent.erase(parentIt);
      }

      _filesToParent.insert(FileIdMapItem(fileInfo->Id, fileInfo->Id));

      if (_folders.find(fileInfo->Id) == _folders.end())
      {
        // add
        _folders.insert(FileMapItem(fileInfo->Id, fileInfo));
      }
      else
      {
        // Update, but don't change the existing pointer, don't want to invalidate references
        FileInfo* _existing = _folders.at(fileInfo->Id);
        _existing->Copy(fileInfo);

        // release working item
        FileInfo::Release(&fileInfo);
        // assign existing pointer to working item
        fileInfo = _existing;
      }

      for (FileInfo::FileList::iterator it = fileInfo->Files.begin(); it != fileInfo->Files.end(); it++)
      {
        // If the file is being ignored for some reason, then unignore it...
        FileMap::iterator ignored = _ignoredIds.find((*it)->Id);

        if (ignored != _ignoredIds.end())
        {
          FileInfo::Release(&(ignored->second));
          _ignoredIds.erase(ignored);
        }

        // add or replace parent
        if (_filesToParent.find((*it)->Id) != _filesToParent.end())
        {
          _filesToParent.erase((*it)->Id);
        }

        // TODO:
        // This creates a lot of strings that could be references if we're careful, reducing cpu and memory cost
        _filesToParent.insert(FileIdMapItem((*it)->Id, fileInfo->Id));
      }
    }
    else
    {
      FileIdMap::iterator currentParent = _filesToParent.find(fileInfo->Id);

      if (currentParent == _filesToParent.end())
      {
        // Some unknown file
        // Update its parent and forget about it
        _GetFiles(fileInfo->ParentId, false, true, true);
      }
      else
      {
        // Some known file
        // Why are we here... maybe a forced update

        // Update old parent if needed
        if (currentParent->second != fileInfo->ParentId)
        {
          _GetFiles(_filesToParent.find(fileInfo->Id)->second, false, true, true);
        }

        // Update current Parent
        _GetFiles(fileInfo->ParentId, false, true, true);
      }
    }
  }

  return fileInfo;
}

bool FileManager::DownloadFile(FileInfo* fileInfo)
{
  Log::WriteOutput(LogType::Debug, L"FileManager::DownloadFile(HWND hwnd, FileInfo* fileInfo)");

  // public function should all be locked
  Scoped_Lock serviceLock(&_mutex);

  if (!fileInfo->IsFile())
    return true;

  FileInfo* currentFileInfo = _service.DownloadFile(fileInfo->Id);

  if (currentFileInfo == NULL)
    return false;

  fileInfo->Copy(currentFileInfo);

  FileInfo::Release(&currentFileInfo);

  ::Sleep(500);

  return true;
}

bool FileManager::UploadFile(FileInfo* fileInfo)
{
  Log::WriteOutput(LogType::Debug, L"FileManager::UploadFile(HWND hwnd, FileInfo* fileInfo)");

  // public function should all be locked
  Scoped_Lock serviceLock(&_mutex);

  if (!fileInfo->IsFile())
    return true;

  FileInfo* currentFileInfo = _service.UploadFile(fileInfo->Id, fileInfo->FilePath());

  if (currentFileInfo == NULL)
    return false;

  fileInfo->Copy(currentFileInfo);

  FileInfo::Release(&currentFileInfo);

  return true;
}

bool FileManager::RenameFile(FileInfo* fileInfo, const std::wstring& name)
{
  Log::WriteOutput(LogType::Debug, L"FileManager::RenameFile(HWND hwnd, FileInfo* fileInfo, std::wstring name)");

  // public function should all be locked
  Scoped_Lock serviceLock(&_mutex);

  if (fileInfo->Title == name)
    return true;

  FileInfo* currentFileInfo = _service.RenameFile(fileInfo->Id, name);

  if (currentFileInfo == NULL)
    return false;

  fileInfo->Copy(currentFileInfo);

  FileInfo::Release(&currentFileInfo);

  return true;
}

bool FileManager::DeleteFiles(const std::vector<std::wstring>& ids, std::vector<std::wstring>*const deletedIds)
{
  Log::WriteOutput(LogType::Debug, L"FileManager::DeleteFiles(HWND hwnd, std::vector<std::wstring> ids, std::vector<std::wstring>** deletedIds)");

  // public function should all be locked
  Scoped_Lock serviceLock(&_mutex);

  std::unique_ptr<std::vector<std::wstring>> tempIds = _service.TrashFiles(ids);

  if (tempIds == NULL)
    return false;

  FileIdMap::iterator it;

  for (std::vector<std::wstring>::iterator it = tempIds->begin(); it != tempIds->end(); it++)
  {
    _MoveFromCacheToIgnored((*it));
  }

  *deletedIds = std::move(*tempIds);
  return true;
}

void FileManager::_MoveFromCacheToIgnored(const std::wstring& id)
{
  // find the parent
  FileIdMap::iterator fileIt = _filesToParent.find(id);

  if (fileIt != _filesToParent.end())
  {
    FileMap::iterator folderIt = _folders.find(fileIt->second);

    if (folderIt != _folders.end())
    {
      if (fileIt->first == fileIt->second)
      {
        // id is a folder
        _ignoredIds.insert(FileMapItem(folderIt->first, folderIt->second));
        _folders.erase(folderIt);
      }
      else
      {
        // search the parent for the removed id
        FileInfo::FileList::iterator item = folderIt->second->Files.begin();

        for(item; item < folderIt->second->Files.end(); item++)
        {
          if ((*item)->Id == id)
          {
            // remove the id
            _ignoredIds.insert(FileMapItem((*item)->Id, (*item)));
            folderIt->second->Files.erase(item);
            break;
          }
        }
      }
    }

    _filesToParent.erase(fileIt);
  }
}

bool FileManager::InsertFile(const std::wstring& parentId, const std::wstring& filename, bool isFolder, FileInfo** newFile)
{
  Log::WriteOutput(LogType::Debug, L"FileManager::InsertFile(HWND hwnd, FileInfo* parent, std::wstring& filename, FileInfo* newFile)");

  Scoped_Lock serviceLock(&_mutex);

  if ((*newFile) != NULL || filename.length() == 0)
  {
    return false;
  }

  (*newFile) = _service.InsertFile(parentId, filename, isFolder);

  if ((*newFile) == NULL)
    return false;

  FileInfo* parent = GetFile(parentId, false, false, false);

  parent->Files.push_back((*newFile));

  (*newFile)->AddRef();

  return true;
}

bool FileManager::MoveFiles(const std::vector<std::wstring>& ids, const std::wstring& dstId, std::vector<std::wstring>*const copiedIds)
{
  Log::WriteOutput(LogType::Debug, L"FileManager::MoveFiles(HWND hwnd, const std::vector<std::wstring>& ids, std::vector<std::wstring>** copiedIds)");

  Scoped_Lock serviceLock(&_mutex);

  std::unique_ptr<std::vector<std::wstring>> tempIds = _service.MoveFiles(ids, dstId);

  if (tempIds == NULL)
    return false;

  *copiedIds = std::move(*tempIds);
  return true;
}

bool FileManager::CopyFiles(const std::vector<std::wstring>& ids, const std::wstring& dstId, std::vector<std::wstring>*const copiedIds)
{
  Log::WriteOutput(LogType::Debug, L"FileManager::CopyFiles(HWND hwnd, const std::vector<std::wstring>& ids, std::vector<std::wstring>** movedIds)");

  Scoped_Lock serviceLock(&_mutex);

  std::unique_ptr<std::vector<std::wstring>> tempIds = _service.CopyFiles(ids, dstId);

  if (tempIds == NULL)
    return false;

  *copiedIds = std::move(*tempIds);
  return true;
}
