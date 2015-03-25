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
#include "XmlReader.h"
#include <algorithm>
#include <functional>
#include <locale>
#include <cctype>

using namespace Fusion::GoogleDrive;

XmlReader::XmlReader(void)
{
  Log::WriteOutput(LogType::Debug, L"XmlReader::XmlReader(void)");

  Init();
}

XmlReader::XmlReader(const std::wstring& xml)
{
  Log::WriteOutput(LogType::Debug, L"XmlReader::XmlReader(const std::wstring& xml)");

  Init(xml);
}

XmlReader::~XmlReader(void)
{
  Log::WriteOutput(LogType::Debug, L"XmlReader::~XmlReader(void)");
}

void XmlReader::Init(void)
{
  Log::WriteOutput(LogType::Debug, L"XmlReader::Init(void)");

  Init(L"");
}

void XmlReader::Init(const std::wstring& xml)
{
  Log::WriteOutput(LogType::Debug, L"XmlReader::Init(const std::wstring& xml)");

  _Xml = xml;

  Reset();
}

void XmlReader::Reset()
{
  Log::WriteOutput(LogType::Debug, L"XmlReader::Reset()");

  _StartIndex = 0;
  _EndIndex = 0;
  _ElementName = L"";
  _LastElementName = L"";
  _IsEndElement = false;
}

bool XmlReader::MoveToNextElement()
{
  Log::WriteOutput(LogType::Debug, L"XmlReader::MoveToNextElement()");

  _ElementName = L"";

  if (_StartIndex == std::wstring::npos || _StartIndex >= _Xml.length())
    return false;

  if (_EndIndex == std::wstring::npos || _EndIndex > _Xml.length() || _EndIndex < _StartIndex)
    return false;

  size_t startIndex = 0;

  if (!_IsEndElement && _EndIndex > 4)
  {
    startIndex = _EndIndex - 4;
  }
  else if (!_IsEndElement && _EndIndex > 1)
  {
    startIndex = _EndIndex - 1;
  }
  else
  {
    startIndex = _EndIndex;
  }

  if (startIndex > 0 && startIndex <= _StartIndex)
  {
    startIndex = _StartIndex + 1;
  }

  if (startIndex > _EndIndex)
  {
    _StartIndex = std::wstring::npos;

    return false;
  }

  _StartIndex = startIndex;

  std::wstring find1 = L"<";
  size_t endIndex1 = _Xml.find(find1, _StartIndex);

  std::wstring find2 = L" />";
  size_t endIndex2 = _Xml.find(find2, _StartIndex);

  std::wstring find = L"";
  size_t endIndex = std::wstring::npos;

  if (endIndex1 != std::wstring::npos && (endIndex1 < endIndex2 || endIndex2 == std::wstring::npos))
  {
    find = find1;
    endIndex = endIndex1;

    _StartIndex = _Xml.find(find, endIndex);

    if (_StartIndex == std::wstring::npos || _StartIndex >= _Xml.length())
      return false;

    find1 = L">";
    endIndex1 = _Xml.find(find1, _StartIndex);

    find2 = L" />";
    size_t endIndex2 = _Xml.find(find2, _StartIndex);

    find = L"";
    endIndex = std::wstring::npos;

    if (endIndex1 != std::wstring::npos && (endIndex1 < endIndex2 || endIndex2 == std::wstring::npos))
    {
      find = find1;
      endIndex = endIndex1;
    }
    else if (endIndex2 != std::wstring::npos && (endIndex2 < endIndex1 || endIndex1 == std::wstring::npos))
    {
      find = find2;
      endIndex = endIndex2;
    }
    else
    {
      _StartIndex = std::wstring::npos;

      return false;
    }

    _EndIndex = endIndex + find.length();

    size_t startIndex = _StartIndex + 1;
    size_t tempIndex = _Xml.find(L" ", startIndex);

    if (tempIndex != std::wstring::npos && tempIndex < _EndIndex)
    {
      endIndex = tempIndex;
    }

    if (endIndex >= _EndIndex)
    {
      _StartIndex = std::wstring::npos;

      return false;
    }

    bool isEndElement = false;

    if (_Xml[startIndex] == '/')
    {
      startIndex += 1;
      isEndElement = true;
    }

    size_t length = endIndex - startIndex;

    _ElementName = _Xml.substr(startIndex, length);
    _LastElementName = _ElementName;
    _IsEndElement = isEndElement;
  }
  else if (endIndex2 != std::wstring::npos && (endIndex2 < endIndex1 || endIndex1 == std::wstring::npos))
  {
    _StartIndex = endIndex2;

    _ElementName = _LastElementName;
    _IsEndElement = true;
  }
  else
  {
    _StartIndex = std::wstring::npos;

    return false;
  }

  if (_StartIndex == std::wstring::npos || _StartIndex > _Xml.length())
    return false;

  if (_EndIndex == std::wstring::npos || _EndIndex > _Xml.length() || _EndIndex < _StartIndex)
    return false;

  if (_ElementName.length() == 0)
  {
    _StartIndex = std::wstring::npos;

    return false;
  }

  return true;
}

int XmlReader::GetAttributeValue(const std::wstring& attributeName, int defaultValue)
{
  Log::WriteOutput(LogType::Debug, L"XmlReader::GetAttributeValue(const std::wstring& attributeName, int defaultValue)");

  std::wstring tempValue = std::to_wstring((long double)defaultValue);

  tempValue = GetAttributeValue(attributeName, tempValue);

  int result = std::stoi(tempValue);

  return result;
}

long XmlReader::GetAttributeValue(const std::wstring& attributeName, long defaultValue)
{
  Log::WriteOutput(LogType::Debug, L"XmlReader::GetAttributeValue(const std::wstring& attributeName, long defaultValue)");

  std::wstring tempValue = std::to_wstring((long double)defaultValue);

  tempValue = GetAttributeValue(attributeName, tempValue);

  long result = std::stol(tempValue);

  return result;
}

bool XmlReader::GetAttributeValue(const std::wstring& attributeName, bool defaultValue)
{
  Log::WriteOutput(LogType::Debug, L"XmlReader::GetAttributeValue(const std::wstring& attributeName, bool defaultValue)");

  std::wstring tempValue = L"0";

  if (defaultValue)
  {
    tempValue = L"1";
  }

  tempValue = GetAttributeValue(attributeName, tempValue);

  if (tempValue == L"1" || tempValue == L"true" || tempValue == L"True" || tempValue == L"TRUE")
    return true;

  return false;
}

std::wstring XmlReader::GetAttributeValue(const std::wstring& attributeName, const wchar_t* defaultValue)
{
  Log::WriteOutput(LogType::Debug, L"XmlReader::GetAttributeValue(const std::wstring& attributeName, const wchar_t* defaultValue)");

  return GetAttributeValue(attributeName, std::wstring(defaultValue));
}

std::wstring ReplaceString(const std::wstring& value, const std::wstring& search, const std::wstring& replace)
{
  std::wstring result = value;

  while (true)
  {
    size_t index = result.find(search);

    if (index == -1)
      break;

    std::wstring left = result.substr(0, index);

    index += search.length();

    std::wstring right = result.substr(index, result.length() - index);

    result = left + replace + right;
  }

  return result;
}

std::wstring ReplaceSpecialCharacters(const std::wstring& value)
{
  std::wstring result = value;

  result = ReplaceString(result, L"&amp;", L"&");

  return result;
}

std::wstring XmlReader::GetAttributeValue(const std::wstring& attributeName, std::wstring defaultValue)
{
  Log::WriteOutput(LogType::Debug, L"XmlReader::GetAttributeValue(const std::wstring& attributeName, std::wstring defaultValue)");

  if (_StartIndex == std::wstring::npos || _StartIndex >= _Xml.length())
    return defaultValue;

  std::wstring find = L" " + attributeName + L"=\"";

  size_t startIndex = _Xml.find(find, _StartIndex);

  if (startIndex == std::wstring::npos)
    return defaultValue;

  startIndex += find.length();

  size_t endIndex = _Xml.find(L"\"", startIndex);

  if (endIndex == std::wstring::npos)
    return defaultValue;

  size_t length = endIndex - startIndex;

  std::wstring value = _Xml.substr(startIndex, length);

  value = ReplaceSpecialCharacters(value);

  return value;
}

std::wstring XmlReader::GetElementValue(const wchar_t* defaultValue)
{
  Log::WriteOutput(LogType::Debug, L"XmlReader::GetElementValue(const wchar_t* defaultValue)");

  return GetElementValue(std::wstring(defaultValue));
}

std::wstring XmlReader::GetElementValue(std::wstring defaultValue)
{
  Log::WriteOutput(LogType::Debug, L"XmlReader::GetElementValue(std::wstring defaultValue)");

  if (_StartIndex == std::wstring::npos || _StartIndex >= _Xml.length())
    return defaultValue;

  if (IsEndElement())
    return defaultValue;

  size_t startIndex = _EndIndex;

  if (!MoveToNextElement())
    return defaultValue;

  if (!IsEndElement())
    return defaultValue;

  size_t endIndex = _StartIndex;

  std::wstring value = _Xml.substr(startIndex, endIndex - startIndex);

  value = ReplaceSpecialCharacters(value);

  return value;
}

std::vector<std::wstring> XmlReader::GetElementValues(const std::wstring& elementName)
{
  std::vector<std::wstring> values;

  while (MoveToNextElement())
  {
    if (ElementName() == elementName && !IsEndElement())
    {
      std::wstring value = GetElementValue(L"");

      values.push_back(value);
    }
  }

  return values;
}
