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
#include "XmlReader.h"

namespace Fusion
{
  namespace GoogleDrive
  {
    struct ErrorInfoType
    {
      enum eType
      {
        None = 0,
        Authentication = 1,
      };
    };

    class ErrorInfo
    {
    public:
      ErrorInfo(void);
      ErrorInfo(const std::wstring& xml);
      ErrorInfo(XmlReader* xmlReader);
      ~ErrorInfo(void);

      static ErrorInfo* FromMessage(const std::wstring& message);

      ErrorInfoType::eType Type;
      std::wstring Message;
      std::vector<std::wstring> Details;

    protected:
      void Init(void);
      void Init(const std::wstring& xml);
      void Init(XmlReader* xmlReader);
    };
  }
}

