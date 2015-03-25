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
#include "PropertyHelper.h"

using namespace Fusion::GoogleDrive;

int PropertyHelper::_PropertyInfoSize = -1;

const PropertyHelper::PROPERTYINFO PropertyHelper::PropertyInfo[] =
{
  { PKEY_ItemNameDisplay, SHCOLSTATE_ONBYDEFAULT, &PropertyHelper::_GetPropertyItemNameDisplay },
  { PKEY_ItemTypeText, SHCOLSTATE_ONBYDEFAULT, &PropertyHelper::_GetPropertyItemTypeText },
  { PKEY_ItemPathDisplay, SHCOLSTATE_SECONDARYUI, &PropertyHelper::_GetPropertyItemPathDisplay },
  { PKEY_ItemFolderNameDisplay, SHCOLSTATE_SECONDARYUI, &PropertyHelper::_GetPropertyItemFolderNameDisplay },
  { PKEY_ItemFolderPathDisplay, SHCOLSTATE_SECONDARYUI, &PropertyHelper::_GetPropertyItemFolderPathDisplay },
  { PKEY_ItemType, SHCOLSTATE_HIDDEN, &PropertyHelper::_GetPropertyItemType },
  { PKEY_ParsingName, SHCOLSTATE_HIDDEN, &PropertyHelper::_GetPropertyParsingName },
  { PKEY_ParsingPath, SHCOLSTATE_HIDDEN, &PropertyHelper::_GetPropertyItemPathDisplay },
  { PKEY_FileName, SHCOLSTATE_SECONDARYUI, &PropertyHelper::_GetPropertyFileName },
  { PKEY_FileExtension, SHCOLSTATE_HIDDEN, &PropertyHelper::_GetPropertyFileExtension },
  { PKEY_FileAttributes, SHCOLSTATE_HIDDEN, &PropertyHelper::_GetPropertyFileAttributes },
  { PKEY_DateModified, SHCOLSTATE_ONBYDEFAULT, &PropertyHelper::_GetPropertyDateModified },
  { PKEY_Size, SHCOLSTATE_ONBYDEFAULT, &PropertyHelper::_GetPropertySize },
  { PKEY_DateCreated, SHCOLSTATE_ONBYDEFAULT, &PropertyHelper::_GetPropertyDateCreated },
  { PKEY_DateAccessed, SHCOLSTATE_DEFAULT, &PropertyHelper::_GetPropertyDateAccessed },
  { PKEY_Status, SHCOLSTATE_SECONDARYUI, &PropertyHelper::_GetPropertyStatus },
  { PKEY_IsSendToTarget, SHCOLSTATE_HIDDEN, &PropertyHelper::_GetPropertyIsSendToTarget },
  { PKEY_PerceivedType, SHCOLSTATE_TYPE_INT | SHCOLSTATE_HIDDEN, &PropertyHelper::_GetPropertyPerceivedType },
  { PKEY_NamespaceCLSID, SHCOLSTATE_TYPE_INT | SHCOLSTATE_HIDDEN, &PropertyHelper::_GetPropertyNameSpaceCLSID },
  { PKEY_FindData, SHCOLSTATE_HIDDEN, &PropertyHelper::_GetPropertyFindData },
  { PKEY_Volume_IsRoot, SHCOLSTATE_TYPE_INT | SHCOLSTATE_HIDDEN, &PropertyHelper::_GetPropertyIsFileSystemRoot },
  { PKEY_PropList_TileInfo, SHCOLSTATE_TYPE_STR | SHCOLSTATE_HIDDEN, &PropertyHelper::_GetPropertyPropList_General },
  { PKEY_PropList_ExtendedTileInfo, SHCOLSTATE_TYPE_STR | SHCOLSTATE_HIDDEN, &PropertyHelper::_GetPropertyPropList_General },
  { PKEY_PropList_PreviewDetails, SHCOLSTATE_TYPE_STR | SHCOLSTATE_HIDDEN, &PropertyHelper::_GetPropertyPropList_General },
  { PKEY_PropList_InfoTip, SHCOLSTATE_TYPE_STR | SHCOLSTATE_HIDDEN, &PropertyHelper::_GetPropertyPropList_General },
  { PKEY_PropList_FileOperationPrompt, SHCOLSTATE_TYPE_STR | SHCOLSTATE_HIDDEN, &PropertyHelper::_GetPropertyPropList_General },
  { PKEY_PropList_PreviewTitle, SHCOLSTATE_TYPE_STR | SHCOLSTATE_HIDDEN, &PropertyHelper::_GetPropertyPropList_General },
};


size_t PropertyHelper::PropertyInfoSize()
{
  if (_PropertyInfoSize < 0)
  {
    _PropertyInfoSize = ARRAYSIZE(PropertyInfo);
  }

  return _PropertyInfoSize;
}

#ifdef DEBUG
std::wstring PropertyHelper::_GetPropertyKeyName(REFPROPERTYKEY key)
{
  if (IsEqualPropertyKey(key, PKEY_ItemNameDisplay)){return L"PKEY_ItemNameDisplay";}

  if (IsEqualPropertyKey(key, PKEY_ItemTypeText)){return L"PKEY_ItemTypeText";}
  if (IsEqualPropertyKey(key, PKEY_ItemPathDisplay)){return L"PKEY_ItemPathDisplay";}
  if (IsEqualPropertyKey(key, PKEY_ItemFolderNameDisplay)){return L"PKEY_ItemFolderNameDisplay";}
  if (IsEqualPropertyKey(key, PKEY_ItemFolderPathDisplay)){return L"PKEY_ItemFolderPathDisplay";}
  if (IsEqualPropertyKey(key, PKEY_ParsingName)){return L"PKEY_ParsingName";}
  if (IsEqualPropertyKey(key, PKEY_ParsingPath)){return L"PKEY_ParsingPath";}
  if (IsEqualPropertyKey(key, PKEY_FileName)){return L"PKEY_FileName";}

  if (IsEqualPropertyKey(key, PKEY_FileExtension)){return L"PKEY_FileExtension";}
  if (IsEqualPropertyKey(key, PKEY_FileAttributes)){return L"PKEY_FileAttributes";}
  if (IsEqualPropertyKey(key, PKEY_DateModified)){return L"PKEY_DateModified";}
  if (IsEqualPropertyKey(key, PKEY_Size)){return L"PKEY_Size";}

  if (IsEqualPropertyKey(key, PKEY_DateCreated)){return L"PKEY_DateCreated";}
  if (IsEqualPropertyKey(key, PKEY_DateAccessed)){return L"PKEY_DateAccessed";}
  if (IsEqualPropertyKey(key, PKEY_Status)){return L"PKEY_Status";}
  if (IsEqualPropertyKey(key, PKEY_IsSendToTarget)){return L"PKEY_IsSendToTarget";}

  if (IsEqualPropertyKey(key, PKEY_PropList_TileInfo)){return L"PKEY_PropList_TileInfo";}
  if (IsEqualPropertyKey(key, PKEY_PropList_ExtendedTileInfo)){return L"PKEY_PropList_ExtendedTileInfo";}
  if (IsEqualPropertyKey(key, PKEY_PropList_PreviewDetails)){return L"PKEY_PropList_PreviewDetails";}
  if (IsEqualPropertyKey(key, PKEY_PropList_InfoTip)){return L"PKEY_PropList_InfoTip";}
  if (IsEqualPropertyKey(key, PKEY_PropList_FileOperationPrompt)){return L"PKEY_PropList_FileOperationPrompt";}
  if (IsEqualPropertyKey(key, PKEY_PropList_PreviewTitle)){return L"PKEY_PropList_PreviewTitle";}
  if (IsEqualPropertyKey(key, PKEY_PerceivedType)){return L"PKEY_PerceivedType";}
  if (IsEqualPropertyKey(key, PKEY_ItemType)){return L"PKEY_ItemType";}
  if (IsEqualPropertyKey(key, PKEY_NamespaceCLSID)){return L"PKEY_NamespaceCLSID";}
  if (IsEqualPropertyKey(key, PKEY_FindData)){return L"PKEY_FindData";}
  if (IsEqualPropertyKey(key, PKEY_Volume_IsRoot)){return L"PKEY_Volume_IsRoot";}

  // The below are not handled right now. Do any of these matter??

  // There was a suggestion to create a MD5 hash over the icon image data
  // Create a WTS_THUMBNAILID and populate with a 16byte array, which is the size of a MD5 hash
  if (IsEqualPropertyKey(key, PKEY_ThumbnailCacheId)){return L"PKEY_ThumbnailCacheId";}
  if (IsEqualPropertyKey(key, PKEY_DescriptionID)){return L"PKEY_DescriptionID";}

  const PROPERTYKEY PKEY_Static_Unknown1 = {0xC9944A21, 0xA406, 0x48FE, 0x82, 0x25, 0xAE, 0xC7, 0xE2, 0x4C, 0x21, 0x1B, 510}; // Undocumented // occurs when you navigate into a sub folder
  if (IsEqualPropertyKey(key, PKEY_Static_Unknown1)){return L"PKEY_Static_Unknown1";} // Undocumented // occurs when you navigate into a sub folder

  return L"Unknown";

#else
std::wstring PropertyHelper::_GetPropertyKeyName(REFPROPERTYKEY /*key*/)
{
  return L"Not Logged in Release code";
#endif
}

HRESULT PropertyHelper::GetProperty(CGDriveShlExt* drive, PCUITEMID_CHILD pidl,  REFPROPERTYKEY key, __out PROPVARIANT *ppropvar)
{
  HRESULT hr = E_INVALIDARG;

  for (size_t i = 0; i < PropertyInfoSize(); i++)
  {
    if (IsEqualPropertyKey(key, PropertyInfo[i].key))
    {
      CDriveItem driveItem;

      hr = drive->GetDriveItemFromIDList(pidl, false, false, driveItem);

      if (SUCCEEDED(hr))
      {
        hr = (*PropertyInfo[i].GetProperty)(drive, pidl, driveItem, ppropvar);

        if (ppropvar->vt == VT_LPWSTR)
        {
          Log::WriteOutput(LogType::Debug, L"PropertyHelper::_GetProperty %s: %s: %s", driveItem.ItemName().c_str(), _GetPropertyKeyName(key).c_str(), ppropvar->pwszVal);
        }
        else
        {
          Log::WriteOutput(LogType::Debug, L"PropertyHelper::_GetProperty %s: %s", driveItem.ItemName().c_str(), _GetPropertyKeyName(key).c_str());
        }
      }

      return hr;
    }
  }

  Log::WriteOutput(LogType::Debug, L"PropertyHelper::_GetProperty unhandled property: %s", _GetPropertyKeyName(key).c_str());

  return hr;
}

HRESULT PropertyHelper::_GetPropertyItemName(CGDriveShlExt* /*drive*/, PCUITEMID_CHILD /*pidl*/,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar)
{
  Log::WriteOutput(LogType::Debug, L"PropertyHelper::_GetPropertyItemName()");

  HRESULT hr = _SetProperty(driveItem.ItemName(), ppropvar);

  return hr;
}

HRESULT PropertyHelper::_GetPropertyItemNameDisplay(CGDriveShlExt* /*drive*/, PCUITEMID_CHILD /*pidl*/,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar)
{
  Log::WriteOutput(LogType::Debug, L"PropertyHelper::_GetPropertyItemNameDisplay()");

  HRESULT hr = _SetProperty(driveItem.ItemName(), ppropvar);

  return hr;
}

HRESULT PropertyHelper::_GetPropertyItemPathDisplay(CGDriveShlExt* drive, PCUITEMID_CHILD pidl,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar)
{
  Log::WriteOutput(LogType::Debug, L"PropertyHelper::_GetPropertyItemPathDisplay()");

  HRESULT hr;

  WCHAR friendlyAbsoluteName[MAX_PATH];
  STRRET name;

  if (driveItem.IsFile())
  {
    hr = drive->GetDisplayNameOf(pidl, SHGDN_FORPARSING, &name);
  }
  else
  {
    hr = drive->GetDisplayNameOf(pidl, SHGDN_FORADDRESSBAR | SHGDN_FORPARSING, &name);
  }

  if (SUCCEEDED(hr))
  {
    hr = StrRetToBuf(&name, pidl, &friendlyAbsoluteName[0], MAX_PATH);

    if (SUCCEEDED(hr))
    {
      hr = _SetProperty(friendlyAbsoluteName, ppropvar);
    }
  }

  return hr;
}

HRESULT PropertyHelper::_GetPropertyItemFolderNameDisplay(CGDriveShlExt* /*drive*/, PCUITEMID_CHILD /*pidl*/,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar)
{
  Log::WriteOutput(LogType::Debug, L"PropertyHelper::_GetPropertyItemFolderNameDisplay()");

  CDriveItem parent;

  HRESULT hr = CGDriveShlExt::GetDriveItemFromID(driveItem.FileInfo()->ParentId, false, false, parent);

  if (SUCCEEDED(hr))
  {
    hr = _SetProperty(parent.ItemName(), ppropvar);
  }

  return hr;
}

HRESULT PropertyHelper::_GetPropertyItemFolderPathDisplay(CGDriveShlExt* drive, PCUITEMID_CHILD /*pidl*/,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar)
{
  Log::WriteOutput(LogType::Debug, L"PropertyHelper::_GetPropertyItemFolderPathDisplay()");

  CComHeapPtr<WCHAR> rootName;
  CComHeapPtr<ITEMIDLIST_ABSOLUTE> rootId;

  HRESULT hr = drive->GetCurFolder(&rootId);

  if (SUCCEEDED(hr))
  {
    hr = SHGetNameFromIDList(rootId, SIGDN_DESKTOPABSOLUTEEDITING, &rootName);

    if (SUCCEEDED(hr))
    {
      bool includeSelf = false;

      std::wstring rootNameStr(rootName);
      hr = _SetProperty(driveItem.ItemFolderPathDisplay(rootNameStr, includeSelf), ppropvar);
    }
  }

  return hr;
}

HRESULT PropertyHelper::_GetPropertyParsingName(CGDriveShlExt* /*drive*/, PCUITEMID_CHILD /*pidl*/,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar)
{
  Log::WriteOutput(LogType::Debug, L"PropertyHelper::_GetPropertyParsingName()");

  HRESULT hr = _SetProperty(driveItem.ItemName(), ppropvar);

  return hr;
}

HRESULT PropertyHelper::_GetPropertyItemUrl(CGDriveShlExt* /*drive*/, PCUITEMID_CHILD /*pidl*/,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar)
{
  Log::WriteOutput(LogType::Debug, L"PropertyHelper::_GetPropertyItemUrl()");

  HRESULT hr = _SetProperty(driveItem.ItemUrl(), ppropvar);

  return hr;
}

HRESULT PropertyHelper::_GetPropertyItemType(CGDriveShlExt* /*drive*/, PCUITEMID_CHILD /*pidl*/,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar)
{
  Log::WriteOutput(LogType::Debug, L"PropertyHelper::_GetPropertyItemType()");

  HRESULT hr = _SetProperty(driveItem.ItemType(), ppropvar);

  return hr;
}

HRESULT PropertyHelper::_GetPropertyItemTypeText(CGDriveShlExt* /*drive*/, PCUITEMID_CHILD /*pidl*/,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar)
{
  Log::WriteOutput(LogType::Debug, L"PropertyHelper::_GetPropertyItemTypeText()");

  HRESULT hr = _SetProperty(driveItem.ItemTypeText(), ppropvar);

  return hr;
}

HRESULT PropertyHelper::_GetPropertyFileName(CGDriveShlExt* /*drive*/, PCUITEMID_CHILD /*pidl*/,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar)
{
  Log::WriteOutput(LogType::Debug, L"PropertyHelper::_GetPropertyFileName()");

  HRESULT hr = _SetProperty(driveItem.ItemName(), ppropvar);

  return hr;
}

HRESULT PropertyHelper::_GetPropertyFileExtension(CGDriveShlExt* /*drive*/, PCUITEMID_CHILD /*pidl*/,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar)
{
  Log::WriteOutput(LogType::Debug, L"PropertyHelper::_GetPropertyFileExtension()");

  HRESULT hr = S_OK;

  if (driveItem.IsFolder())
  {
    ppropvar->vt = VT_EMPTY;
  }
  else
  {
    hr = _SetProperty(driveItem.FileExtension(), ppropvar);
  }

  return hr;
}

HRESULT PropertyHelper::_GetPropertyFileAttributes(CGDriveShlExt* /*drive*/, PCUITEMID_CHILD /*pidl*/,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar)
{
  Log::WriteOutput(LogType::Debug, L"PropertyHelper::_GetPropertyFileAttributes()");

  HRESULT hr = _SetProperty(driveItem.FileAttributes(), ppropvar);

  return hr;
}

HRESULT PropertyHelper::_GetPropertyDateModified(CGDriveShlExt* /*drive*/, PCUITEMID_CHILD /*pidl*/,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar)
{
  Log::WriteOutput(LogType::Debug, L"PropertyHelper::_GetPropertyDateModified()");

  HRESULT hr = E_FAIL;

  FILETIME ft;

  if (FileInfo::TryConvertDateStringToFILETIME(driveItem.DateModified(), ft))
  {
    hr = _SetProperty(ft, ppropvar);
  }

  return hr;
}

HRESULT PropertyHelper::_GetPropertySize(CGDriveShlExt* /*drive*/, PCUITEMID_CHILD /*pidl*/,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar)
{
  Log::WriteOutput(LogType::Debug, L"PropertyHelper::_GetPropertySize()");

  HRESULT hr = _SetProperty(driveItem.Size(), ppropvar);

  return hr;
}

HRESULT PropertyHelper::_GetPropertyDateCreated(CGDriveShlExt* /*drive*/, PCUITEMID_CHILD /*pidl*/,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar)
{
  Log::WriteOutput(LogType::Debug, L"PropertyHelper::_GetPropertyDateCreated()");

  HRESULT hr = E_FAIL;

  FILETIME ft;

  if(FileInfo::TryConvertDateStringToFILETIME(driveItem.DateCreated(), ft))
  {
    hr = _SetProperty(ft, ppropvar);
  }

  return hr;
}

HRESULT PropertyHelper::_GetPropertyDateAccessed(CGDriveShlExt* /*drive*/, PCUITEMID_CHILD /*pidl*/,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar)
{
  Log::WriteOutput(LogType::Debug, L"PropertyHelper::_GetPropertyDateAccessed()");

  HRESULT hr = E_FAIL;

  FILETIME ft;

  if(FileInfo::TryConvertDateStringToFILETIME(driveItem.DateAccessed(), ft))
  {
    hr = _SetProperty(ft, ppropvar);
  }

  return hr;
}

HRESULT PropertyHelper::_GetPropertyStatus(CGDriveShlExt* /*drive*/, PCUITEMID_CHILD /*pidl*/,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar)
{
  Log::WriteOutput(LogType::Debug, L"PropertyHelper::_GetPropertyStatus()");

  HRESULT hr = _SetProperty(driveItem.Status(), ppropvar);

  return hr;
}

HRESULT PropertyHelper::_GetPropertyIsSendToTarget(CGDriveShlExt* /*drive*/, PCUITEMID_CHILD /*pidl*/,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar)
{
  Log::WriteOutput(LogType::Debug, L"PropertyHelper::_GetPropertyIsSendToTarget()");

  HRESULT hr = _SetProperty(driveItem.IsSendToTarget(), ppropvar);

  return hr;
}

HRESULT PropertyHelper::_GetPropertyPerceivedType(CGDriveShlExt* /*drive*/, PCUITEMID_CHILD /*pidl*/,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar)
{
  Log::WriteOutput(LogType::Debug, L"PropertyHelper::_GetPropertyPerceivedType()");

  std::wstring perceivedType;

  HRESULT hr = driveItem.PreceivedType(perceivedType);

  if (SUCCEEDED(hr))
  {
    hr = _SetProperty(perceivedType, ppropvar);
  }

  return hr;
}

HRESULT PropertyHelper::_GetPropertyNameSpaceCLSID(CGDriveShlExt* drive, PCUITEMID_CHILD /*pidl*/,  CDriveItem& /*driveItem*/, __out PROPVARIANT *ppropvar)
{
  Log::WriteOutput(LogType::Debug, L"PropertyHelper::_GetPropertyNameSpaceCLSID()");

  CLSID clsid;

  HRESULT hr = drive->GetClassID(&clsid);

  if (SUCCEEDED(hr))
  {
    hr = _SetProperty(clsid, ppropvar);
  }

  return hr;
}

HRESULT PropertyHelper::_GetPropertyFindData(CGDriveShlExt* /*drive*/, PCUITEMID_CHILD /*pidl*/,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar)
{
  Log::WriteOutput(LogType::Debug, L"PropertyHelper::_GetPropertyNameSpaceCLSID()");

  HRESULT hr = E_FAIL;

  WIN32_FIND_DATA data;

  if(driveItem.TryWIN32FindData(data))
  {
    hr = _SetProperty((BYTE&)data, (UINT)sizeof(WIN32_FIND_DATA), ppropvar);
  }

  return hr;
}

HRESULT PropertyHelper::_GetPropertyIsFileSystemRoot(CGDriveShlExt* /*drive*/, PCUITEMID_CHILD /*pidl*/,  CDriveItem& /*driveItem*/, __out PROPVARIANT *ppropvar)
{
  Log::WriteOutput(LogType::Debug, L"PropertyHelper::_GetPropertyIsFileSystemRoot()");

  // I can't think of any reason this would ever be true
  HRESULT hr = _SetProperty(false, ppropvar);

  return hr;
}

HRESULT PropertyHelper::_GetPropertyPropList_General(CGDriveShlExt* /*drive*/, PCUITEMID_CHILD /*pidl*/,  CDriveItem& /*driveItem*/, __out PROPVARIANT *ppropvar)
{
  Log::WriteOutput(LogType::Debug, L"PropertyHelper::_GetPropertyPropList_TileInfo()");

  HRESULT hr = _BuildPropList(ppropvar);

  return hr;
}

HRESULT PropertyHelper::_BuildPropList(__out PROPVARIANT *ppropvar)
{
  Log::WriteOutput(LogType::Debug, L"PropertyHelper::_GetPropertyPropList()");

  std::wstring value = L"prop:";
  value += L"System.ItemTypeText;";
  value += L"System.DateModified;";
  value += L"System.Size;";
  value += L"System.DateCreated;";
  value += L"System.Status;";

  HRESULT hr = InitPropVariantFromString(value.c_str(), ppropvar);

  if (!SUCCEEDED(hr))
  {
    Log::WriteOutput(LogType::Error, L"InitPropVariantFromString returned hr=%d", hr);
  }

  return hr;
}

HRESULT PropertyHelper::_SetProperty(const std::wstring& value, __out PROPVARIANT *ppropvar)
{
  try
  {
    HRESULT hr = InitPropVariantFromString(value.c_str(), ppropvar);

    if (!SUCCEEDED(hr))
    {
      Log::WriteOutput(LogType::Error, L"InitPropVariantFromString returned hr=%d", hr);
    }

    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::_SetProperty");

    return E_FAIL;
  }
}

HRESULT PropertyHelper::_SetProperty(const ULONG value, __out PROPVARIANT *ppropvar)
{
  try
  {
    HRESULT hr = InitPropVariantFromUInt32(value, ppropvar);

    if (!SUCCEEDED(hr))
    {
      Log::WriteOutput(LogType::Error, L"InitPropVariantFromString returned hr=%d", hr);
    }

    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::_SetProperty");

    return E_FAIL;
  }
}

HRESULT PropertyHelper::_SetProperty(const ULONGLONG value, __out PROPVARIANT *ppropvar)
{
  try
  {
    HRESULT hr = InitPropVariantFromUInt64(value, ppropvar);

    if (!SUCCEEDED(hr))
    {
      Log::WriteOutput(LogType::Error, L"InitPropVariantFromString returned hr=%d", hr);
    }

    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::_SetProperty");

    return E_FAIL;
  }
}

HRESULT PropertyHelper::_SetProperty(const BOOL value, __out PROPVARIANT *ppropvar)
{
  try
  {
    HRESULT hr = InitPropVariantFromBoolean(value, ppropvar);

    if (!SUCCEEDED(hr))
    {
      Log::WriteOutput(LogType::Error, L"InitPropVariantFromString returned hr=%d", hr);
    }

    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::_SetProperty");

    return E_FAIL;
  }
}

HRESULT PropertyHelper::_SetProperty(const GUID value, __out PROPVARIANT *ppropvar)
{
  try
  {
    HRESULT hr = InitPropVariantFromCLSID(value, ppropvar);

    if (!SUCCEEDED(hr))
    {
      Log::WriteOutput(LogType::Error, L"InitPropVariantFromCLSID returned hr=%d", hr);
    }

    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::_SetProperty");

    return E_FAIL;
  }
}

HRESULT PropertyHelper::_SetProperty(const BYTE& value, UINT length, __out PROPVARIANT *ppropvar)
{
  try
  {
    HRESULT hr = InitPropVariantFromBuffer(&value, length, ppropvar);

    if (!SUCCEEDED(hr))
    {
      Log::WriteOutput(LogType::Error, L"InitPropVariantFromBuffer returned hr=%d", hr);
    }

    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::_SetProperty");

    return E_FAIL;
  }
}

HRESULT PropertyHelper::_SetProperty(const FILETIME& value, __out PROPVARIANT *ppropvar)
{
  try
  {
    HRESULT hr = InitPropVariantFromFileTime(&value, ppropvar);

    if (!SUCCEEDED(hr))
    {
      Log::WriteOutput(LogType::Error, L"InitPropVariantFromFileTime returned hr=%d", hr);
    }

    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::_SetProperty");

    return E_FAIL;
  }
}
