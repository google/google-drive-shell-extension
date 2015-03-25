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
#include "DriveItem.h"
#include "GDriveShlExt.h"

namespace Fusion
{
  namespace GoogleDrive
  {
    class PropertyHelper
    {
    public:
#pragma warning(push)              // Issuing a warning that the type can't be instantiated is a known bug for aggregate types.
#pragma warning(disable:4610 4510) // Ref: https://connect.microsoft.com/VisualStudio/feedback/details/488660/improper-issuance-of-c4610
                                   // The associated warning, C4510: default constructor could not be generated, is correct but unhelpful
                                   // since the struct is meant to use the aggregate initialization syntax.
      static const struct PROPERTYINFO
      {
        REFPROPERTYKEY key;
        SHCOLSTATEF scsf;
        HRESULT (*GetProperty)(CGDriveShlExt* drive, PCUITEMID_CHILD pidl,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar);
      } PropertyInfo[];

#pragma warning(pop)

      static size_t PropertyInfoSize();

      static HRESULT GetProperty(CGDriveShlExt* drive, PCUITEMID_CHILD pidl,  REFPROPERTYKEY key, __out PROPVARIANT* ppropvar);

    private:
      static int _PropertyInfoSize;

      static std::wstring _GetPropertyKeyName(REFPROPERTYKEY key);

      static HRESULT _GetPropertyItemName(CGDriveShlExt* drive, PCUITEMID_CHILD pidl,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar);
      static HRESULT _GetPropertyItemNameDisplay(CGDriveShlExt* drive, PCUITEMID_CHILD pidl,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar);
      static HRESULT _GetPropertyItemPathDisplay(CGDriveShlExt* drive, PCUITEMID_CHILD pidl,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar);
      static HRESULT _GetPropertyItemFolderNameDisplay(CGDriveShlExt* drive, PCUITEMID_CHILD pidl,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar);
      static HRESULT _GetPropertyItemFolderPathDisplay(CGDriveShlExt* drive, PCUITEMID_CHILD pidl,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar);
      static HRESULT _GetPropertyParsingName(CGDriveShlExt* drive, PCUITEMID_CHILD pidl,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar);
      static HRESULT _GetPropertyItemUrl(CGDriveShlExt* drive, PCUITEMID_CHILD pidl,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar);
      static HRESULT _GetPropertyItemType(CGDriveShlExt* drive, PCUITEMID_CHILD pidl,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar);
      static HRESULT _GetPropertyItemTypeText(CGDriveShlExt* drive, PCUITEMID_CHILD pidl,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar);
      static HRESULT _GetPropertyFileName(CGDriveShlExt* drive, PCUITEMID_CHILD pidl,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar);
      static HRESULT _GetPropertyFileAttributes(CGDriveShlExt* drive, PCUITEMID_CHILD pidl,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar);
      static HRESULT _GetPropertyFileExtension(CGDriveShlExt* drive, PCUITEMID_CHILD pidl,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar);
      static HRESULT _GetPropertyDateModified(CGDriveShlExt* drive, PCUITEMID_CHILD pidl,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar);
      static HRESULT _GetPropertySize(CGDriveShlExt* drive, PCUITEMID_CHILD pidl,  CDriveItem& driveItem, __out PROPVARIANT* ppropvar);
      static HRESULT _GetPropertyDateCreated(CGDriveShlExt* drive, PCUITEMID_CHILD pidl,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar);
      static HRESULT _GetPropertyDateAccessed(CGDriveShlExt* drive, PCUITEMID_CHILD pidl,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar);
      static HRESULT _GetPropertyStatus(CGDriveShlExt* drive, PCUITEMID_CHILD pidl,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar);
      static HRESULT _GetPropertyIsSendToTarget(CGDriveShlExt* drive, PCUITEMID_CHILD pidl,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar);
      static HRESULT _GetPropertyPerceivedType(CGDriveShlExt* drive, PCUITEMID_CHILD pidl,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar);
      static HRESULT _GetPropertyNameSpaceCLSID(CGDriveShlExt* drive, PCUITEMID_CHILD pidl,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar);
      static HRESULT _GetPropertyFindData(CGDriveShlExt* drive, PCUITEMID_CHILD pidl,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar);
      static HRESULT _GetPropertyIsFileSystemRoot(CGDriveShlExt* drive, PCUITEMID_CHILD pidl,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar);
      static HRESULT _GetPropertyPropList_General(CGDriveShlExt* drive, PCUITEMID_CHILD pidl,  CDriveItem& driveItem, __out PROPVARIANT *ppropvar);

      static HRESULT _BuildPropList(__out PROPVARIANT *ppropvar);

      static HRESULT _SetProperty(const std::wstring& value, __out PROPVARIANT *ppropvar);
      static HRESULT _SetProperty(const ULONG value, __out PROPVARIANT *ppropvar);
      static HRESULT _SetProperty(const ULONGLONG value, __out PROPVARIANT *ppropvar);
      static HRESULT _SetProperty(const BOOL value, __out PROPVARIANT *ppropvar);
      static HRESULT _SetProperty(const GUID value, __out PROPVARIANT *ppropvar);
      static HRESULT _SetProperty(const BYTE& value, UINT length, __out PROPVARIANT *ppropvar);
      static HRESULT _SetProperty(const FILETIME& value, __out PROPVARIANT *ppropvar);
    };
  }
}
