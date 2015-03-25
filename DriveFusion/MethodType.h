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

namespace Fusion
{
  namespace GoogleDrive
  {
    struct MethodType
    {
      enum eType
      {
        Unknown = 0,
        GetFiles = 1,
        DownloadFile = 2,
        RenameFile = 4,
        TrashFiles = 5,
        UntrashFile = 6,
        UploadFile = 8,
        Authenticate = 9,
        InsertFile = 10,
        MoveFiles = 11,
        CopyFiles = 12,
        GetLog = 13,
        GetAbout = 15
      };

      static const std::wstring ToString(eType type)
      {
        switch (type)
        {
        case Fusion::GoogleDrive::MethodType::GetFiles:
          return L"GetFiles";
        case Fusion::GoogleDrive::MethodType::DownloadFile:
          return L"DownloadFile";
        case Fusion::GoogleDrive::MethodType::RenameFile:
          return L"RenameFile";
        case Fusion::GoogleDrive::MethodType::TrashFiles:
          return L"TrashFiles";
        case Fusion::GoogleDrive::MethodType::UntrashFile:
          return L"UntrashFile";
        case Fusion::GoogleDrive::MethodType::UploadFile:
          return L"UploadFile";
        case Fusion::GoogleDrive::MethodType::Authenticate:
          return L"Authenticate";
        case Fusion::GoogleDrive::MethodType::InsertFile:
          return L"InsertFile";
        case Fusion::GoogleDrive::MethodType::MoveFiles:
          return L"MoveFiles";
        case Fusion::GoogleDrive::MethodType::CopyFiles:
          return L"CopyFiles";
        case Fusion::GoogleDrive::MethodType::GetLog:
          return L"GetLog";
        case Fusion::GoogleDrive::MethodType::GetAbout:
          return L"GetAbout";
        case Fusion::GoogleDrive::MethodType::Unknown:
        default:
          return L"Unknown";
        };
      }
    };
  }
}
