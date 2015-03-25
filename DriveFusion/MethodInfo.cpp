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
#include "MethodInfo.h"

using namespace Fusion::GoogleDrive;

MethodInfo::MethodInfo(void)
{
  Log::WriteOutput(LogType::Debug, L"MethodInfo::MethodInfo(void)");

  Init();
}

MethodInfo::MethodInfo(MethodType::eType methodType)
{
  Log::WriteOutput(LogType::Debug, L"MethodInfo::MethodInfo(MethodType::eType methodType)");

  Init(methodType);
}

MethodInfo::~MethodInfo(void)
{
  Log::WriteOutput(LogType::Debug, L"MethodInfo::~MethodInfo(void)");
}

void MethodInfo::Init(void)
{
  Log::WriteOutput(LogType::Debug, L"MethodInfo::Init(void)");

  Init(MethodType::Unknown);
}

void MethodInfo::Init(MethodType::eType methodType)
{
  Log::WriteOutput(LogType::Debug, L"MethodInfo::Init(MethodType::eType methodType)");

  Type = methodType;
  Parameters.clear();
}

void MethodInfo::AddParameter(const std::wstring& value)
{
  Log::WriteOutput(LogType::Debug, L"MethodInfo::AddParameter(const std::wstring& value)");

  Parameters.push_back(value);
}

void MethodInfo::AddParameter(const std::vector<std::wstring>& values)
{
  std::wstring count = std::to_wstring((_ULonglong)values.size());

  AddParameter(count);

  for (size_t i = 0; i < values.size(); i++)
  {
    AddParameter(values[i]);
  }
}

std::wstring MethodInfo::ToString()
{
  Log::WriteOutput(LogType::Debug, L"MethodInfo::ToString()");

  std::wstring result = std::to_wstring((unsigned long long)Type);

  for (size_t i = 0; i < Parameters.size(); i++)
  {
    std::wstring parameter = Parameters.at(i);

    result += L"," + parameter;
  }

  return result;
}

