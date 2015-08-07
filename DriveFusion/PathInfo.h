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

class PathInfo
{
protected:
  PathInfo(void);
  ~PathInfo(void);

public:
  static bool FileExists(const std::wstring& path);
  static std::wstring CombinePath(const std::wstring& path1, const std::wstring& path2);
  static std::wstring GetFileUrl(const std::wstring& path);
  static void CreatePath(const std::wstring& path, size_t last = std::wstring::npos);
};

