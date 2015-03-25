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
    class LogInfo
    {
    public:
      ~LogInfo(void);

      static LogInfo* CreateInstance();
      static LogInfo* CreateInstance(const std::wstring& xml, ErrorInfo** errorInfo);
      static LogInfo* CreateInstance(XmlReader* xmlReader, ErrorInfo** errorInfo);

      int LogLevel;
      std::wstring FilePath;
      std::wstring LocalGoogleDriveData;

    protected:
      explicit LogInfo();

      bool _Init(XmlReader* xmlReader, ErrorInfo** errorInfo);
    };

    class AboutInfo
    {
    public:
      ~AboutInfo(void);

      static AboutInfo* CreateInstance();
      static AboutInfo* CreateInstance(const std::wstring& xml, ErrorInfo** errorInfo);
      static AboutInfo* CreateInstance(XmlReader* xmlReader, ErrorInfo** errorInfo);

      std::wstring Name;
      std::wstring UserDisplayName;
      std::wstring QuotaBytesTotal;
      std::wstring QuotaBytesUsed;
      std::wstring QuotaBytesUsedAggregate;
      std::wstring QuotaBytesUsedInTrash;
      std::wstring RootFolderId;

    protected:
      explicit AboutInfo();

      bool _Init(XmlReader* xmlReader, ErrorInfo** errorInfo);
    };
  };
};
