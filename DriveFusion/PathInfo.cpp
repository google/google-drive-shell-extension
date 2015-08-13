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
#include "PathInfo.h"


PathInfo::PathInfo(void)
{
}


PathInfo::~PathInfo(void)
{
}


bool PathInfo::FileExists(const std::wstring& path)
{
  Log::WriteOutput(LogType::Debug, L"PathInfo::FileExists(const std::wstring& path)");

  if (path.length() == 0)
    return false;

  if (_waccess(path.c_str(), 0) == 0)
  {
    return true;
  }

  return false;
}

std::wstring PathInfo::CombinePath(const std::wstring& path1, const std::wstring& path2)
{
  Log::WriteOutput(LogType::Debug, L"PathInfo::CombinePath(const std::wstring& path1, const std::wstring& path2)");

  std::wstring path;

  if (path1.length() > 0 && path2.length() > 0)
  {
    path = path1 + L"\\" + path2;
  }
  else if (path1.length() > 0)
  {
    path = path1;
  }
  else if (path2.length() > 0)
  {
    path = path2;
  }

  return path;
}

std::wstring PathInfo::GetFileUrl(const std::wstring& path)
{
  Log::WriteOutput(LogType::Debug, L"PathInfo::GetFileUrl(const std::wstring& path)");

  std::wstring url = path;

  size_t startIndex = std::wstring::npos;

  while (startIndex < url.length())
  {
    size_t index = url.find(L"\\", startIndex);

    if (index == std::wstring::npos)
      break;

    url.replace(index, 1, L"/");

    startIndex = index + 1;
  }

  if (url.length() > 0)
  {
    url = L"file:///" + url;
  }

  return url;
}

void PathInfo::CreatePath(const std::wstring& path, size_t last)
{
  const auto parentPos = path.find_last_of(L"\\", last);
  const auto err = CreateDirectory(path.substr(0,parentPos).c_str(), NULL);
  if (err == 0 && GetLastError() == ERROR_PATH_NOT_FOUND)
  {
    CreatePath(path, parentPos-1);
    CreateDirectory(path.substr(0,parentPos).c_str(), NULL);
  }
}
