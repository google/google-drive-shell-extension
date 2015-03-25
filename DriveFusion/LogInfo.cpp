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
#include "LogInfo.h"

using namespace Fusion::GoogleDrive;

LogInfo::LogInfo(void)
{
  LogLevel = 0;
  LocalGoogleDriveData = L"C:\\";
}

LogInfo::~LogInfo(void)
{
}

LogInfo* LogInfo::CreateInstance()
{
  LogInfo* logInfo = new LogInfo();

  return logInfo;
}

LogInfo* LogInfo::CreateInstance(const std::wstring& xml, ErrorInfo** errorInfo)
{
  XmlReader xmlReader(xml);

  xmlReader.MoveToNextElement();

  return CreateInstance(&xmlReader, errorInfo);
}

LogInfo* LogInfo::CreateInstance(XmlReader* xmlReader, ErrorInfo** errorInfo)
{
  LogInfo* logInfo = CreateInstance();

  if (!logInfo->_Init(xmlReader, errorInfo))
  {
    return NULL;
  }

  return logInfo;
}

bool LogInfo::_Init(XmlReader* xmlReader, ErrorInfo** errorInfo)
{
  if (xmlReader->ElementName() != L"Log" || xmlReader->IsEndElement())
  {
    (*errorInfo) = ErrorInfo::FromMessage(L"Could not load log - Expected beginning element of 'Log'.");

    return false;
  }

  LogLevel = xmlReader->GetAttributeValue(L"LogLevel", LogLevel);
  FilePath = xmlReader->GetAttributeValue(L"FilePath", FilePath);
  LocalGoogleDriveData = xmlReader->GetAttributeValue(L"LocalGoogleDriveData", LocalGoogleDriveData);

  return true;
}


AboutInfo::AboutInfo(void)
{
}

AboutInfo::~AboutInfo(void)
{
}

AboutInfo* AboutInfo::CreateInstance()
{
  AboutInfo* logInfo = new AboutInfo();

  return logInfo;
}

AboutInfo* AboutInfo::CreateInstance(const std::wstring& xml, ErrorInfo** errorInfo)
{
  XmlReader xmlReader(xml);

  xmlReader.MoveToNextElement();

  return CreateInstance(&xmlReader, errorInfo);
}

AboutInfo* AboutInfo::CreateInstance(XmlReader* xmlReader, ErrorInfo** errorInfo)
{
  AboutInfo* logInfo = CreateInstance();

  if (!logInfo->_Init(xmlReader, errorInfo))
  {
    return NULL;
  }

  return logInfo;
}

bool AboutInfo::_Init(XmlReader* xmlReader, ErrorInfo** errorInfo)
{
  if (xmlReader->ElementName() != L"About" || xmlReader->IsEndElement())
  {
    (*errorInfo) = ErrorInfo::FromMessage(L"Could not load log - Expected beginning element of 'About'.");

    return false;
  }

  Name = xmlReader->GetAttributeValue(L"Name", Name);
  UserDisplayName = xmlReader->GetAttributeValue(L"UserDisplayName", UserDisplayName);
  QuotaBytesTotal = xmlReader->GetAttributeValue(L"QuotaBytesTotal", QuotaBytesTotal);
  QuotaBytesUsed = xmlReader->GetAttributeValue(L"QuotaBytesUsed", QuotaBytesUsed);
  QuotaBytesUsedAggregate = xmlReader->GetAttributeValue(L"QuotaBytesUsedAggregate", QuotaBytesUsedAggregate);
  QuotaBytesUsedInTrash = xmlReader->GetAttributeValue(L"QuotaBytesUsedInTrash", QuotaBytesUsedInTrash);
  RootFolderId = xmlReader->GetAttributeValue(L"RootFolderId", RootFolderId);

  return true;
}
