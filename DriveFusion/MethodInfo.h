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
#include "MethodType.h"

namespace Fusion
{
  namespace GoogleDrive
  {
    class MethodInfo
    {
    public:
      MethodInfo(void);
      MethodInfo(MethodType::eType methodType);
      ~MethodInfo(void);

      MethodType::eType Type;
      std::vector<std::wstring> Parameters;

      void AddParameter(const std::wstring& value);
      void AddParameter(const std::vector<std::wstring>& values);
      std::wstring ToString();

    protected:
      void Init(void);
      void Init(MethodType::eType methodType);
    };
  }
}

