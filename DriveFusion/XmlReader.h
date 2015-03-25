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

namespace Fusion
{
  namespace GoogleDrive
  {
    class XmlReader
    {
    public:
      XmlReader(void);
      XmlReader(const std::wstring& xml);
      ~XmlReader(void);

      std::wstring ElementName() { return _ElementName; }
      bool GetAttributeValue(const std::wstring& attributeName, bool defaultValue);
      int GetAttributeValue(const std::wstring& attributeName, int defaultValue);
      long GetAttributeValue(const std::wstring& attributeName, long defaultValue);
      std::wstring GetAttributeValue(const std::wstring& attributeName, const wchar_t* defaultValue);
      std::wstring GetAttributeValue(const std::wstring& attributeName, std::wstring defaultValue);
      std::wstring GetElementValue(const wchar_t* defaultValue);
      std::wstring GetElementValue(std::wstring defaultValue);
      std::vector<std::wstring> GetElementValues(const std::wstring& elementName);
      bool IsEndElement() { return _IsEndElement; }
      bool MoveToNextElement();
      void Reset();

    private:
      void Init(void);
      void Init(const std::wstring& xml);

      std::wstring _Xml;
      size_t _StartIndex;
      size_t _EndIndex;
      std::wstring _ElementName;
      std::wstring _LastElementName;
      bool _IsEndElement;
    };
  }
}
