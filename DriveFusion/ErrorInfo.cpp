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
#include "ErrorInfo.h"

using namespace Fusion::GoogleDrive;

ErrorInfo::ErrorInfo(void)
{
  Log::WriteOutput(LogType::Debug, L"ErrorInfo::ErrorInfo(void)");

  Init();
}

ErrorInfo::ErrorInfo(const std::wstring& xml)
{
  Log::WriteOutput(LogType::Debug, L"ErrorInfo::ErrorInfo(const std::wstring& xml)");

  Init(xml);
}

ErrorInfo::ErrorInfo(XmlReader* xmlReader)
{
  Log::WriteOutput(LogType::Debug, L"ErrorInfo::ErrorInfo(XmlReader* xmlReader)");

  Init(xmlReader);
}

ErrorInfo::~ErrorInfo(void)
{
  Log::WriteOutput(LogType::Debug, L"ErrorInfo::~ErrorInfo(void)");
}

ErrorInfo* ErrorInfo::FromMessage(const std::wstring& message)
{
  Log::WriteOutput(LogType::Debug, L"ErrorInfo::FromMessage(const std::wstring& message)");

  ErrorInfo* errorInfo = new ErrorInfo();

  errorInfo->Message = message;

  return errorInfo;
}

void ErrorInfo::Init(void)
{
  Log::WriteOutput(LogType::Debug, L"ErrorInfo::Init(void)");

  Type = ErrorInfoType::None;
  Message = L"";
  Details.clear();
}

void ErrorInfo::Init(const std::wstring& xml)
{
  Log::WriteOutput(LogType::Debug, L"ErrorInfo::Init(const std::wstring& xml)");

  XmlReader xmlReader(xml);

  xmlReader.MoveToNextElement();

  Init(&xmlReader);
}

void ErrorInfo::Init(XmlReader* xmlReader)
{
  Log::WriteOutput(LogType::Debug, L"ErrorInfo::Init(XmlReader* xmlReader)");

  Init();

  if (xmlReader->ElementName() != L"Error" || xmlReader->IsEndElement())
    return;

  std::wstring type = xmlReader->GetAttributeValue(L"Type", L"");
  Type = (ErrorInfoType::eType)wcstoul(type.c_str(), NULL, 0);

  Message = xmlReader->GetAttributeValue(L"Message", Message);

  while (xmlReader->MoveToNextElement())
  {
    if (xmlReader->IsEndElement())
    {
      if (xmlReader->ElementName() == L"Error")
        break;
    }
    else
    {
      if (xmlReader->ElementName() == L"Detail")
      {
        std::wstring detail = xmlReader->GetAttributeValue(L"Message", L"");
      }
    }
  }
}

