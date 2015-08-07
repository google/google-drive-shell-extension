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
#include <ShellAPI.h>
#include "ChildItemIDPolicy.h"
#include "DriveItemDataObject.h"
#include "DriveItemPropertyStoreFactory.h"
#include "DriveItemPropertyStore.h"
#include "DriveItemRelatedItem.h"
#include "DriveItemStream.h"
#include "GDriveShlExt.h"
#include "PropertyHelper.h"
#include "ShellFolderViewCBHandler.h"

HRESULT WINAPI CGDriveShlExt::UpdateRegistry(_In_ BOOL bRegister) throw()
{
  // get program files path

  TCHAR programFiles[MAX_PATH];
  SHGetSpecialFolderPath(NULL, (LPWSTR)&programFiles, CSIDL_PROGRAM_FILES, FALSE);

  // get program files x86 path

  TCHAR programFilesX86[MAX_PATH];
  SHGetSpecialFolderPath(NULL, (LPWSTR)&programFilesX86, CSIDL_PROGRAM_FILESX86, FALSE);

  // define size of array

  const int size = 4;

  ATL::_ATL_REGMAP_ENTRY regMapEntries[size];
  memset(&regMapEntries[size - 1], 0, sizeof(ATL::_ATL_REGMAP_ENTRY));

  int regMapEntryIndex = -1;

  // add program files path

  regMapEntryIndex = 0;

  regMapEntries[regMapEntryIndex].szKey = L"PROGRAM_FILES";

  regMapEntries[regMapEntryIndex].szData = T2OLE_EX(programFiles, _ATL_SAFE_ALLOCA_DEF_THRESHOLD);

  // add program files x86 path

  regMapEntryIndex = 1;

  regMapEntries[regMapEntryIndex].szKey = L"PROGRAM_FILESX86";

  regMapEntries[regMapEntryIndex].szData = T2OLE_EX(programFilesX86, _ATL_SAFE_ALLOCA_DEF_THRESHOLD);

  // add program files x86 path

  regMapEntryIndex = 2;

  regMapEntries[regMapEntryIndex].szKey = L"COMMAND_OPEN";

  regMapEntries[regMapEntryIndex].szData = T2OLE_EX(L"--file=\"%1\"", _ATL_SAFE_ALLOCA_DEF_THRESHOLD);

  // add module

  __if_exists(_Module)
  {
    return _Module.UpdateRegistryFromResource(IDR_GDRIVESHLEXT, bRegister, regMapEntries);
  }
  __if_not_exists(_Module)
  {
    return ATL::_pAtlModule->UpdateRegistryFromResource(IDR_GDRIVESHLEXT, bRegister, regMapEntries);
  }
}


// These are some IDs we don't know what to do with, most have been mined from the registry

static const GUID SDefined_ITopViewAwareItem = // {8279FEB8-5CA4-45C4-BE27-770DCDEA1DEB} //RPC server, Can't find any information on this one, found name in registry
{0x8279FEB8, 0x5CA4, 0x45C4, {0xBE, 0x27, 0x77, 0x0D, 0xCD, 0xEA, 0x1D, 0xEB}};

static const GUID SDefined_IConnectionFactory = // {6FE2B64C-5012-4B88-BB9D-7CE4F45E3751} //RPC server, Can't find any information on this one, found name in registry
{0x6FE2B64C, 0x5012, 0x4B88, {0xBB, 0x9D, 0x7C, 0xE4, 0xF4, 0x5E, 0x37, 0x51}};

static const GUID SDefined_ILibraryDescription = // {86187C37-E662-4D1E-A122-7478676D7E6E} //RPC server, Can't find any information on this one, found name in registry
{0x86187C37, 0xE662, 0x4D1E, {0xA1, 0x22, 0x74, 0x78, 0x67, 0x6D, 0x7E, 0x6E}};

static const GUID SDefined_Unknown1 = // {FDBEE76E-F12B-408E-93AB-9BE8521000D9}
{0xFDBEE76E, 0xF12B, 0x408E, {0x93, 0xAB, 0x9B, 0xE8, 0x52, 0x10, 0x00, 0xD9}};

static const GUID SDefined_IShellFolder3 = // {711B2CFD-93D1-422B-BDF4-69BE923F2449} //Not in our developement path
{0x711B2CFD, 0x93D1, 0x422B, {0xBD, 0xF4, 0x69, 0xBE, 0x92, 0x3F, 0x24, 0x49}};

static const GUID SDefined_IFrameLayoutDefinition = // {176C11B1-4302-4164-8430-D5A9F0EEACDB} //Not in our developement path
{0x176C11B1, 0x4302, 0x4164, {0x84, 0x30, 0xD5, 0xA9, 0xF0, 0xEE, 0xAC, 0xDB}};

static const GUID SDefined_Unknown2 = // {93F81976-6A0D-42C3-94DD-AA258A155470}
{0x93F81976, 0x6A0D, 0x42C3, {0x94, 0xDD, 0xAA, 0x25, 0x8A, 0x15, 0x54, 0x70}};

static const GUID SDefined_IItemSetOperations = // {32AE3A1F-D90E-4417-9DD9-23B0DFA4621D} //RPC server, Can't find any information on this one, found name in registry
{0x32AE3A1F, 0xD90E, 0x4417, {0x9D, 0xD9, 0x23, 0xB0, 0xDF, 0xA4, 0x62, 0x1D}};

static const GUID SDefined_Unknown3 = // {CAD9AE9F-56E2-40F1-AFB6-3813E320DCFD}
{0xCAD9AE9F, 0x56E2, 0x40F1, {0xAF, 0xB6, 0x38, 0x13, 0xE3, 0x20, 0xDC, 0xFD}};

static const GUID SDefined_IFrameLayoutDefinitionFactory = // {7E734121-F3B4-45F9-AD43-2FBE39E533E2} //RPC server, Can't find any information on this one, found name in registry
{0x7E734121, 0xF3B4, 0x45F9, {0xAD, 0x43, 0x2F, 0xBE, 0x39, 0xE5, 0x33, 0xE2}};

static const GUID SDefined_INewItemAdvisor  = // {24D16EE5-10F5-4DE3-8766-D23779BA7A6D}
{0x24D16EE5, 0x10F5, 0x4DE3, {0x87, 0x66, 0xD2, 0x37, 0x79, 0xBA, 0x7A, 0x6D}};

static const GUID SDefined_IInterruptItem  = // {388EED6A-DF36-4009-B5EA-609D3309CC5D} //RPC server, Can't find any information on this one, found name in registry
{0x388EED6A, 0xDF36, 0x4009, {0xB5, 0xEA, 0x60, 0x9D, 0x33, 0x09, 0xCC, 0x5D}};

static const GUID SDefined_Unknown4 = // {42339A50-7A91-44F9-87AC-37E6EC1B1A88} //this one doesn't show up in Google at all
{0x42339A50, 0x7A91, 0x44F9, {0x87, 0xAC, 0x37, 0xE6, 0xEC, 0x1B, 0x1A, 0x88}};

//
const DriveItemSignature kNewFileSignature = { L"New File" };

bool CGDriveShlExt::_didUpdate = false;
DWORD CGDriveShlExt::_previousRGFIN = 0;
CGDriveShlExt::HOSTINFO::TYPE CGDriveShlExt::_hostType = CGDriveShlExt::HOSTINFO::Unknown;
UINT CGDriveShlExt::_lastViewUIMsg = 0;
IDataObject* CGDriveShlExt::_contextMenuSelection = NULL;
bool CGDriveShlExt::_ignoreGetFilesError = true;
std::wstring CGDriveShlExt::_newFileName;
DWORD CGDriveShlExt::_newFileAttributes = 0;
CIdList CGDriveShlExt::_newFilePidl;

#ifdef DEBUG
// both are false, really don't want to debug service calls right now
FileManager CGDriveShlExt::_fileManager = FileManager::CreateFileManager(false);
#else
FileManager CGDriveShlExt::_fileManager = FileManager::CreateFileManager(false);
#endif // DEBUG

static std::wstring ChildIdFromPidl(PCUIDLIST_RELATIVE pidl)
{
  auto child = ILFindLastID(pidl);
  auto childData = CGDriveShlExt::CItemIDFactory::GetDataFromIDList(child);
  if ( childData == NULL )
    return L"root"; // The only "not valid" pidl we should receive is the root pidl the shell hands us.

  DriveItemSignature alignedData; // childData may be unaligned, copy data to align it
  memcpy_s(&alignedData, sizeof(DriveItemSignature), childData->Id, sizeof(DriveItemSignature));

  return alignedData.Id;
}

void CGDriveShlExt::_LogWhoAmI(PCIDLIST_RELATIVE pidl)
{
  Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_LogWhoAmI(PCIDLIST_RELATIVE pidl)");

  std::wstring id = ChildIdFromPidl(pidl);

  CDriveItem item;
  HRESULT hr = _GetDataFromID(id, false, false, item);

  if (SUCCEEDED(hr))
  {
    Log::WriteOutput(LogType::Warning, L"_LogWhoAmI %s %s %d", item.ItemName().c_str(), id.c_str(), ILGetSize(pidl));
  }
}

void CGDriveShlExt::_LogRPC(REFIID riid, __deref_out void ** /*ppv*/)
{
  if (riid == SDefined_ITopViewAwareItem)
  {
    Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::CreateViewObject RPC SDefined_ITopViewAwareItem");
  }
  else if (riid == SDefined_IConnectionFactory)
  {
    Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::CreateViewObject RPC SDefined_IConnectionFactory");
  }
  else if (riid == SDefined_ILibraryDescription)
  {
    Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::BindToObject RPC SDefined_ILibraryDescription");
  }
  else if (riid == SDefined_Unknown1)
  {
    Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::BindToObject RPC SDefined_Unknown1");
  }
  else if (riid == SDefined_IItemSetOperations)
  {
    Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::CreateViewObject RPC SDefined_IItemSetOperations");
  }
  else if (riid == SDefined_IFrameLayoutDefinitionFactory)
  {
    Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::CreateViewObject RPC SDefined_IFrameLayoutDefinitionFactory");
  }
}

std::wstring CGDriveShlExt::_STGMToString(DWORD uFlags)
{
  Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_STGMToString(DWORD uFlags)");

  std::wstring r;

  if (!(uFlags & (STGM_WRITE | STGM_READWRITE))){r += L":READ:";} // If its not write or readwrite then it must be read
  if (uFlags & STGM_WRITE){r += L":WRITE:";}
  if (uFlags & STGM_READWRITE){r += L":READWRITE:";}
  if (uFlags & STGM_SHARE_DENY_NONE){r += L":SHARE_DENY_NONE:";}
  if (uFlags & STGM_SHARE_DENY_READ){r += L":SHARE_DENY_READ:";}
  if (uFlags & STGM_SHARE_DENY_WRITE){r += L":SHARE_DENY_WRITE:";}
  if (uFlags & STGM_SHARE_EXCLUSIVE){r += L":SHARE_EXCLUSIVE:";}
  if (uFlags & STGM_PRIORITY){r += L":PRIORITY:";}
  if (uFlags & STGM_CREATE){r += L":CREATE:";}
  if (uFlags & STGM_CONVERT){r += L":CONVERT:";}
  if (uFlags & STGM_FAILIFTHERE){r += L":FAILIFTHERE:";}
  if (uFlags & STGM_DIRECT){r += L":DIRECT:";}
  if (uFlags & STGM_TRANSACTED){r += L":TRANSACTED:";}
  if (uFlags & STGM_NOSCRATCH){r += L":NOSCRATCH:";}
  if (uFlags & STGM_NOSNAPSHOT){r += L":NOSNAPSHOT:";}
  if (uFlags & STGM_SIMPLE){r += L":SIMPLE:";}
  if (uFlags & STGM_DIRECT_SWMR){r += L":DIRECT_SWMR:";}
  if (uFlags & STGM_DELETEONRELEASE){r += L":DELETEONRELEASE:";}

  return r;
}

HRESULT CGDriveShlExt::_GetContextOptions(__in IBindCtx *pbc, __out BIND_OPTS& options)
{
  Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_LogBindctxUnhandledFlags(__in IBindCtx *pbc)");

  ZeroMemory(&options, sizeof(BIND_OPTS));
  options.cbStruct = sizeof(options);

  if (pbc == NULL)
    return S_OK;

  HRESULT hr = pbc->GetBindOptions(&options);
  if (!SUCCEEDED(hr))
    return hr;

  if (options.grfFlags & BIND_MAYBOTHERUSER)
  {
    Log::WriteOutput(LogType::Error, L"_LogBindctxUnhandledFlags MAYBOTHERUSER flag, unhandled");
  }
  if (options.grfFlags & BIND_JUSTTESTEXISTENCE)
  {
    Log::WriteOutput(LogType::Error, L"_LogBindctxUnhandledFlags JUSTTESTEXISTENCE flag, unhandled");
  }

  Log::WriteOutput(LogType::Debug, L"_LogBindctxUnhandledFlags STGM %s", _STGMToString(options.grfMode).c_str());

  return hr;
}

HRESULT CGDriveShlExt::_CreateContextMenu(HWND hwndOwner, UINT cidl, __in_ecount_opt(cidl) PCUITEMID_CHILD_ARRAY rgpidl, REFIID riid, __deref_out void **ppv)
{
  Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_CreateContextMenu()");

  CComObject<ContextMenuCBHandler> *contextMenuCBHandler;
  HRESULT hr = ContextMenuCBHandler::CreateInstance(this, &contextMenuCBHandler);
  if (!SUCCEEDED(hr))
    return hr;

  contextMenuCBHandler->AddRef();
  IContextMenuCB* contextMenuCB;
  hr = contextMenuCBHandler->ReturnInterfaceTo(IID_PPV_ARGS(&contextMenuCB));
  if (!SUCCEEDED(hr))
  {
    contextMenuCBHandler->Release();
    return hr;
  }

  UINT cKeys = 0;
  HKEY aKeys[3];

  if (cidl == 0) // this must be a bind for a background menu
  {
    HKEY result;
    LSTATUS st = RegOpenKeyEx(HKEY_CLASSES_ROOT, L"Directory\\Background", NULL, KEY_QUERY_VALUE, &result); // just check if it exists

    if (st == ERROR_SUCCESS)
    {
      aKeys[cKeys] = result;
      cKeys++;
    }
  }

  DEFCONTEXTMENU dcm =
  {
    hwndOwner,
    contextMenuCB,
    NULL, // _spidl,
    static_cast<IShellFolder2 *>(this),
    cidl,
    rgpidl,
    NULL,
    cKeys,
    aKeys
  };

  hr = SHCreateDefaultContextMenu(&dcm, riid, ppv);
  if (!SUCCEEDED(hr))
  {
    Log::LogUnknownIID(riid, L"CGDriveShlExt::_CreateContextMenu()");
    return hr;
  }

  _contextMenuCBHandlers.push_back(contextMenuCBHandler);
  return hr;
}


HRESULT CGDriveShlExt::CreateInstance(PCIDLIST_ABSOLUTE pidl, PCUITEMID_CHILD pidlChild, CComObject<CGDriveShlExt>** drive)
{
  try
  {
    Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::CreateInstance()");

    *drive = NULL;

    HRESULT hr = CComObject<CGDriveShlExt>::CreateInstance(drive);

    if (!SUCCEEDED(hr))
    {
      Log::WriteOutput(LogType::Error, L"CComObject<CGDriveShlExt>::CreateInstance returned hr=%d", hr);
    }
    else
    {
      (*drive)->AddRef();
      CIdList cpidl;
      hr = CIdList::Combine(pidl, pidlChild, cpidl);

      if (SUCCEEDED(hr))
      {
        hr = (*drive)->Initialize(cpidl);
      }
    }

    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::CreateInstance");

    return E_FAIL;
  }
}

HRESULT CGDriveShlExt::CreateInstanceReturnInterfaceTo(PCIDLIST_ABSOLUTE pidl, PCUITEMID_CHILD pidlChild, REFIID riid, __deref_out void **ppv)
{
  // pidlChild could be an empty pidl

  CComObject<CGDriveShlExt>* drive;

  HRESULT hr = CGDriveShlExt::CreateInstance(pidl, pidlChild, &drive);

  if (SUCCEEDED(hr))
  {
    drive->AddRef();
    hr = drive->QueryInterface(riid, ppv);

    if (!SUCCEEDED(hr))
    {
      Log::LogUnknownIID(riid, L"CGDriveShlExt::CreateInstanceReturnInterfaceTo");
    }
  }

  drive->Release();

  return hr;
}

HRESULT CGDriveShlExt::_CopyLPCWSTRToLPWSTR(LPCWSTR currentValue, LPWSTR* newValue)
{
  Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_CopyLPCWSTRToLPWSTR(LPCWSTR currentValue, LPWSTR* newValue)");

  HRESULT hr = SHStrDup(currentValue, newValue);

  if (!SUCCEEDED(hr))
  {
    Log::WriteOutput(LogType::Error, L"SHStrDup returned hr=%d", hr);
  }

  return hr;
}

HRESULT CGDriveShlExt::_CopyPROPVARIANTToWString(PROPVARIANT* ppropvar, std::wstring& value)
{
  Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_CopyPROPVARIANTToWString(PROPVARIANT* ppropvar, const std::wstring& value)");

  CComHeapPtr<WCHAR> temp;

  HRESULT hr = _CopyLPCWSTRToLPWSTR(ppropvar->pwszVal, &temp);

  if (SUCCEEDED(hr))
  {
    ppropvar->vt = VT_EMPTY;

    value.assign(temp);
  }

  return hr;
}

HRESULT CGDriveShlExt::_CopyWStringToLPWSTR(const std::wstring& currentValue, LPWSTR* newValue)
{
  Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_CopyWStringToLPWSTR(std::wstring* currentValue, LPWSTR* newValue)");

  HRESULT hr = _CopyLPCWSTRToLPWSTR(currentValue.c_str(), newValue);

  return hr;
}

HRESULT CGDriveShlExt::_CopyWStringToLPOLESTR(const std::wstring& currentValue, LPOLESTR* newValue)
{
  Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_CopyWStringToLPOLESTR(std::wstring* currentValue, LPOLESTR* newValue)");

  HRESULT hr = _CopyLPCWSTRToLPWSTR(currentValue.c_str(), newValue);

  return hr;
}

HRESULT CGDriveShlExt::_CopyWStringToPWSTR(const std::wstring& currentValue, PWSTR* newValue)
{
  Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_CopyWStringToPWSTR(std::wstring* currentValue, PWSTR* newValue)");

  HRESULT hr = _CopyLPCWSTRToLPWSTR(currentValue.c_str(), newValue);

  return hr;
}

HRESULT CGDriveShlExt::_CopyWStringToWCHAR(const std::wstring& currentValue, WCHAR* newValue)
{
  Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_CopyWStringToWCHAR(std::wstring* currentValue, WCHAR* newValue)");

  HRESULT hr = _CopyLPCWSTRToLPWSTR(currentValue.c_str(), (LPWSTR*)newValue);

  return hr;
}

FileManager* CGDriveShlExt::FileManager()
{
  return &_fileManager;
}

HRESULT CGDriveShlExt::DoDownload(FileInfo* fi)
{
  if (!fi->IsFile())
    return S_OK;

  if (!_fileManager.DownloadFile(fi))
  {
    std::wstring message;

    if (_fileManager.HasError())
    {
      message = L"CGDriveShlExt::DoDownload() Could not download file " + fi->FilePath() + L" - " + _fileManager.ErrorMessage();
    }
    else
    {
      message = L"CGDriveShlExt::DoDownload() Could not download file " + fi->FilePath() + L" - Unknown error";
    }

    Log::WriteOutput(LogType::Error, message.c_str());

    return E_FAIL;
  }
  else
  {
    return S_OK;
  }
}

HRESULT  CGDriveShlExt::DoMoveFiles(const std::vector<PCIDLIST_RELATIVE>& ids, PCIDLIST_ABSOLUTE src, PCIDLIST_ABSOLUTE dst)
{
  return _PasteFiles(true, ids, src, dst);
}

HRESULT  CGDriveShlExt::DoCopyFiles(const std::vector<PCIDLIST_RELATIVE>& ids, PCIDLIST_ABSOLUTE src, PCIDLIST_ABSOLUTE dst)
{
  return _PasteFiles(false, ids, src, dst);
}

HRESULT CGDriveShlExt::GetDriveItemFromIDList(PCUIDLIST_RELATIVE pidl, bool updateCache, bool getChildren, CDriveItem& driveItem)
{
  try
  {
    Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::GetDriveItemFromIDList(PCUIDLIST_RELATIVE pidl, bool updateCache, bool getChildren, CDriveItem& driveItem)");

    return _GetDataFromIDList(pidl, updateCache, getChildren, driveItem);
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::GetDriveItemFromIDList");

    return E_FAIL;
  }
}

HRESULT CGDriveShlExt::GetDriveItemFromID(const std::wstring& id, bool updateCache, bool getChildren, CDriveItem& driveItem)
{
  try
  {
    Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::GetDriveItemFromID(const std::wstring& id, bool updateCache, bool getChildren, CDriveItem& driveItem)");

    return _GetDataFromID(id, updateCache, getChildren, driveItem);
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::GetDriveItemFromID");

    return E_FAIL;
  }
}

HRESULT CGDriveShlExt::_GetDataFromIDList(PCUIDLIST_RELATIVE pidl, bool updateCache, bool getChildren, FileInfo** fileInfo)
{
  Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_GetDataFromIDList(LPCITEMIDLIST pidl, bool updateCache, bool getChildren, FileInfo** fileInfo)");

  return _GetDataFromID(ChildIdFromPidl(pidl), updateCache, getChildren, fileInfo);
}

HRESULT CGDriveShlExt::_GetDataFromIDList(PCUIDLIST_RELATIVE pidl, bool updateCache, bool getChildren, CDriveItem& driveItem)
{
  Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_GetCachedDataFromIDList(LPCITEMIDLIST pidl, bool updateCache, bool getChildren, CDriveItem& driveItem)");

  return _GetDataFromID(ChildIdFromPidl(pidl), updateCache, getChildren, driveItem);
}

HRESULT CGDriveShlExt::_GetDataFromID(const std::wstring& id, bool updateCache, bool getChildren, FileInfo** fileInfo)
{
  Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_GetDataFromID(std::wstring id, bool updateCache, bool getChildren, FileInfo** fileInfo)");

  HRESULT hr = S_OK;

  (*fileInfo) = _fileManager.GetFile(id, updateCache, getChildren, _ignoreGetFilesError);
  if ((*fileInfo) == NULL)
  {
    if (_fileManager.HasError())
    {
      Log::WriteOutput(LogType::Error, L"Could not find file with specified id=%s - %s", id.c_str(), _fileManager.ErrorMessage().c_str());
    }
    else
    {
      Log::WriteOutput(LogType::Error, L"Could not find file with specified id=%s", id.c_str());
    }
    CHECK_HR(HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND));
  }

  return hr;
}

HRESULT CGDriveShlExt::_GetDataFromID(const std::wstring& id, bool updateCache, bool getChildren, CDriveItem& driveItem)
{
  Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_GetDataFromID(std::wstring id, bool updateCache, bool getChildren, CDriveItem* driveItem)");

  HRESULT hr = S_OK;

  Fusion::GoogleDrive::FileInfo* fileInfo = NULL;

  hr = _GetDataFromID(id, updateCache, getChildren, &fileInfo);

  if (SUCCEEDED(hr))
  {
    driveItem.FileInfo(fileInfo);
  }

  return hr;
}


HRESULT CGDriveShlExt::_GetNameFromIDList(PCUIDLIST_RELATIVE pidl, SIGDN sigdn, std::wstring& name)
{
  Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_GetNameFromIDList(PCUIDLIST_RELATIVE pidl, SIGDN sigdn, const std::wstring& name)");

  CComHeapPtr<WCHAR> spszName;

  HRESULT hr = _GetNameFromIDList(pidl, sigdn, &spszName);

  if (SUCCEEDED(hr))
  {
    name = spszName;
  }

  return hr;
}

HRESULT CGDriveShlExt::_GetNameFromIDList(PCUIDLIST_RELATIVE pidl, SIGDN sigdn, PWSTR *ppszName)
{
  Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_GetNameFromIDList(PCUIDLIST_RELATIVE pidl, SIGDN sigdn, PWSTR* ppszName)");

  HRESULT hr = SHGetNameFromIDList(pidl, sigdn, ppszName);

  if (!SUCCEEDED(hr))
  {
    Log::WriteOutput(LogType::Error, L"SHGetNameFromIDList returned hr=%d", hr);
  }

  return hr;
}


STDMETHODIMP CGDriveShlExt::GetClassID(__out CLSID *pclsid)
{
  try
  {
    Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::GetClassID(__out CLSID *pclsid)");

    *pclsid = CLSID_GDriveShlExt;

    return S_OK;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::GetClassID");

    return E_FAIL;
  }
}

STDMETHODIMP CGDriveShlExt::Initialize(PCIDLIST_ABSOLUTE pidl)
{
  try
  {
    Log::WriteOutput(LogType::Debug, L"IPersistFolder::Initialize");

    _id = ChildIdFromPidl(pidl);
    return CIdList::Clone(pidl, _spidl);
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::Initialize");

    return E_FAIL;
  }
}

STDMETHODIMP CGDriveShlExt::CreateViewObject(HWND hwndOwner, REFIID riid, __deref_out void **ppv)
{
  try
  {
    Log::WriteOutput(LogType::Debug, L"IShellFolder::CreateViewObject");
    SetDialogType(hwndOwner);

    *ppv = NULL;

    HRESULT hr = E_NOINTERFACE;

    if (riid == IID_IShellView)
    {
      Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::CreateViewObject IID_IShellView::CViewCallback");

      SFV_CREATE sfvc = {0};

      sfvc.cbSize = sizeof(sfvc);

      hr = QueryInterface(IID_PPV_ARGS(&sfvc.pshf));

      if (!SUCCEEDED(hr))
      {
        Log::WriteOutput(LogType::Error, L"QueryInterface returned hr=%d", hr);
      }
      else
      {
        hr = ShellFolderViewCBHandler::CreateInstanceReturnInterfaceTo(this, IID_PPV_ARGS(&sfvc.psfvcb));

        if (!SUCCEEDED(hr))
        {
          Log::WriteOutput(LogType::Error, L"CViewCallback::CreateInstance returned hr=%d", hr);
        }
        else
        {
          CComPtr<IShellView> spsv;

          hr = SHCreateShellFolderView(&sfvc, &spsv);

          if (!SUCCEEDED(hr))
          {
            Log::WriteOutput(LogType::Error, L"SHCreateShellFolderView returned hr=%d", hr);
          }
          else
          {
            hr = spsv->QueryInterface(riid, ppv);

            if (!SUCCEEDED(hr))
            {
              Log::WriteOutput(LogType::Error, L"CViewCallback->QueryInterface returned hr=%d", hr);
            }
          }

          sfvc.psfvcb->Release();
        }

        sfvc.pshf->Release();
      }
    }
    else if (riid == IID_IContextMenu || riid == IID_IContextMenu2 || riid == IID_IContextMenu3)
    {
      Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::GetUIObjectOf IContextMenu");

      hr = _CreateContextMenu(hwndOwner, 0, NULL, riid, ppv);
    }
    else if (riid == IID_IIdentityName || riid == IID_ITransferDestination || riid == IID_ITransferSource || riid == IID_IShellItemResources)
    {
      if (riid == IID_IIdentityName)
      {
        Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::CreateViewObject IID_IIdentityName::CDriveItemRelatedItem");
      }
      else if (riid == IID_ITransferDestination)
      {
        Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::CreateViewObject IID_ITransferDestination::CDriveItemRelatedItem");
      }
      else if (riid == IID_ITransferSource)
      {
        Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::CreateViewObject IID_ITransferSource::CDriveItemRelatedItem");
      }
      else if (riid == IID_IShellItemResources)
      {
        Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::CreateViewObject IID_IShellItemResources::CDriveItemRelatedItem");
      }

      hr = CDriveItemRelatedItem::CreateInstanceReturnInterfaceTo(this, riid, ppv);
    }
    else if (riid == IID_IShellLink)
    {
      Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::CreateViewObject IID_IShellLink we won't be containing shortcuts, no need to impliment");
    }
    else if (riid == IID_IDropTarget)
    {
      Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::CreateViewObject IID_IDropTarget");

      hr = CDriveItemDataObject::CreateInstanceReturnInterfaceTo(this, riid, ppv);
    }
    else if (riid == IID_IExplorerCommandProvider)
    {
      Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::CreateViewObject IID_IExplorerCommandProvider Exposes methods to create Explorer commands and command enumerators");
    }
    else if (riid == IID_ICategoryProvider)
    {
      Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::CreateViewObject IID_ICategoryProvider Exposes a list of categorizers registered on an IShellFolder");
    }
    else if (riid == SDefined_ITopViewAwareItem || riid == SDefined_IConnectionFactory || riid == SDefined_IItemSetOperations || riid == SDefined_IFrameLayoutDefinitionFactory)
    {
      _LogRPC(riid, ppv);
    }
    else if (riid == SDefined_IFrameLayoutDefinition)
    {
      Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::CreateViewObject IID_IFrameLayoutDefinition not implimented, undocumented windows interface");
    }
    else if (riid == SDefined_INewItemAdvisor)
    {
      Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::CreateViewObject SDefined_INewItemAdvisor not implimented, undocumented windows interface");
    }
    else if (riid == SDefined_Unknown2)
    {
      Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::CreateViewObject SDefined_Unknown2 not implimented");
    }
    else if (riid == SDefined_Unknown3)
    {
      Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::CreateViewObject SDefined_Unknown3 not implimented");
    }
    else if (riid == SDefined_Unknown4)
    {
      Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::CreateViewObject SDefined_Unknown4 not implimented");
    }
    else
    {
      Log::LogUnknownIID(riid, L"CGDriveShlExt::CreateViewObject()");
    }

    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::CreateViewObject");

    return E_FAIL;
  }
}

STDMETHODIMP CGDriveShlExt::GetCurFolder(__out PIDLIST_ABSOLUTE *ppidl)
{
  try
  {
    Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::GetCurFolder %s", _id.c_str());
    HRESULT hr = S_OK;

    CIdList clone;
    CHECK_HR(CIdList::Clone(_spidl, clone));
    *ppidl = clone.Release();

    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::GetCurFolder");

    return E_FAIL;
  }
}

STDMETHODIMP CGDriveShlExt::GetDetailsOf(__in_opt PCUITEMID_CHILD pidl, UINT iColumn, __out SHELLDETAILS *psd)
{
  try
  {
    Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::GetDetailsOf(__in_opt PCUITEMID_CHILD pidl, UINT iColumn, __out SHELLDETAILS *psd)", pidl, iColumn);

    HRESULT hr = E_INVALIDARG;

    if (iColumn < PropertyHelper::PropertyInfoSize())
    {
      if (pidl)
      {
        CPropVariant spropvar;
        hr = PropertyHelper::GetProperty(this, pidl, PropertyHelper::PropertyInfo[iColumn].key, &spropvar);

        if (SUCCEEDED(hr))
        {
          CPropVariant stringProp;
          hr = PropVariantChangeType(&stringProp, spropvar, 0, VT_LPWSTR);

          if (!SUCCEEDED(hr))
          {
            Log::WriteOutput(LogType::Error, L"PropVariantChangeType returned hr=%d", hr);
          }
          else
          {
            psd->str.uType = STRRET_WSTR;
            psd->str.pOleStr = stringProp.pwszVal;

            stringProp.vt = VT_EMPTY; // Surrender ownership of the string to the caller
          }
        }
      }
      else
      {
        CComPtr<IPropertyDescription> sppropdesc;

        hr = PSGetPropertyDescription(PropertyHelper::PropertyInfo[iColumn].key, IID_PPV_ARGS(&sppropdesc));

        if (!SUCCEEDED(hr))
        {
          Log::WriteOutput(LogType::Error, L"PSGetPropertyDescription returned hr=%d", hr);
        }
        else
        {
          hr = _GetDetailsOf(sppropdesc, psd);
        }
      }
    }

    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::GetDetailsOf");

    return E_FAIL;
  }
}

HRESULT CGDriveShlExt::_GetDetailsOf(__in IPropertyDescription *ppropdesc, __out SHELLDETAILS *psd)
{
  Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_GetDetailsOf(__in IPropertyDescription *ppropdesc, __out SHELLDETAILS *psd)");

  PROPDESC_VIEW_FLAGS dvf;

  HRESULT hr = ppropdesc->GetViewFlags(&dvf);

  if (!SUCCEEDED(hr))
  {
    Log::WriteOutput(LogType::Error, L"ppropdesc->GetViewFlags returned hr=%d", hr);
  }
  else
  {
    int fmt = LVCFMT_LEFT; // default

    // Handle the mutually exclusive part
    if (dvf & PDVF_RIGHTALIGN)
    {
      fmt = LVCFMT_RIGHT;
    }
    else if (dvf & PDVF_CENTERALIGN)
    {
      fmt = LVCFMT_CENTER;
    }

    static const struct PROPVIEWFLAGSTOLVCFMT
    {
      PROPDESC_VIEW_FLAGS dvf;
      int fmt;
    }

    s_mapFlags[] =
    {
      { PDVF_BEGINNEWGROUP, LVCFMT_LINE_BREAK },
      { PDVF_FILLAREA, LVCFMT_FILL },
      { PDVF_HIDELABEL, LVCFMT_NO_TITLE },
      { PDVF_CANWRAP, LVCFMT_WRAP },
    };

    for (int i = 0; i < ARRAYSIZE(s_mapFlags); i++)
    {
      if (dvf & s_mapFlags[i].dvf)
      {
        fmt |= s_mapFlags[i].fmt;
      }
    }

    psd->fmt = fmt;

    hr = ppropdesc->GetDefaultColumnWidth((UINT *)&psd->cxChar);

    if (!SUCCEEDED(hr))
    {
      Log::WriteOutput(LogType::Error, L"ppropdesc->GetDefaultColumnWidth returned hr=%d", hr);
    }
    else
    {
      PROPDESC_TYPE_FLAGS dtf;

      hr = ppropdesc->GetTypeFlags(PDTF_ISVIEWABLE, &dtf);

      if (!SUCCEEDED(hr))
      {
        Log::WriteOutput(LogType::Error, L"ppropdesc->GetTypeFlags returned hr=%d", hr);
      }
      else
      {
        CComHeapPtr<WCHAR> spszDisplayName;

        hr = ppropdesc->GetDisplayName(&spszDisplayName);

        if (FAILED(hr) && !(dtf & PDTF_ISVIEWABLE))
        {
          // Hidden columns don't have to have a display name in the schema
          hr = _CopyLPCWSTRToLPWSTR(L"", &spszDisplayName);
        }

        if (SUCCEEDED(hr))
        {
          psd->str.uType = STRRET_WSTR;
          psd->str.pOleStr = spszDisplayName.Detach();
        }
      }
    }
  }

  return hr;
}

STDMETHODIMP CGDriveShlExt::MapColumnToSCID(UINT iColumn, __out PROPERTYKEY *pkey)
{
  try
  {
    Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::MapColumnToSCID(UINT iColumn=%d, __out PROPERTYKEY *pkey)", iColumn);

    HRESULT hr = E_INVALIDARG;

    if (iColumn < PropertyHelper::PropertyInfoSize())
    {
      *pkey = PropertyHelper::PropertyInfo[iColumn].key;

      hr = S_OK;
    }

    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::MapColumnToSCID");

    return E_FAIL;
  }
}

STDMETHODIMP CGDriveShlExt::GetDefaultColumn(DWORD /*dwRes*/, __out ULONG *plSort, __out ULONG *plDisplay)
{
  try
  {
    Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::GetDefaultColumn(DWORD dwRes, __out ULONG *plSort, __out ULONG *plDisplay)");

    *plSort = 0;
    *plDisplay = 0;

    return S_OK;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::GetDefaultColumn");

    return E_FAIL;
  }
}

STDMETHODIMP CGDriveShlExt::GetDefaultColumnState(UINT iColumn, __out SHCOLSTATEF *pcsFlags)
{
  try
  {
    Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::GetDefaultColumnState(UINT iColumn, __out SHCOLSTATEF *pcsFlags)");

    HRESULT hr = E_INVALIDARG;

    if (iColumn < PropertyHelper::PropertyInfoSize())
    {
      CComPtr<IPropertyDescription> sppropdesc;

      hr = PSGetPropertyDescription(PropertyHelper::PropertyInfo[iColumn].key, IID_PPV_ARGS(&sppropdesc));

      if (!SUCCEEDED(hr))
      {
        Log::WriteOutput(LogType::Error, L"PSGetPropertyDescription returned hr=%d", hr);
      }
      else
      {
        hr = _GetDefaultColumnState(sppropdesc, pcsFlags);

        if (SUCCEEDED(hr))
        {
          *pcsFlags |= PropertyHelper::PropertyInfo[iColumn].scsf;
        }
      }
    }

    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::GetDefaultColumnState");

    return E_FAIL;
  }
}

HRESULT CGDriveShlExt::_GetDefaultColumnState(__in IPropertyDescription *ppropdesc, __out SHCOLSTATEF *pcsFlags)
{
  try
  {
    Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_GetDefaultColumnState(__in IPropertyDescription *ppropdesc, __out SHCOLSTATEF *pcsFlags)");

    SHCOLSTATEF cs;

    HRESULT hr = ppropdesc->GetColumnState(&cs);

    if (!SUCCEEDED(hr))
    {
      Log::WriteOutput(LogType::Error, L"ppropdesc->GetColumnState returned hr=%d", hr);
    }
    else
    {
      PROPDESC_TYPE_FLAGS dtf;

      hr = ppropdesc->GetTypeFlags(PDTF_ISVIEWABLE, &dtf);

      if (!SUCCEEDED(hr))
      {
        Log::WriteOutput(LogType::Error, L"ppropdesc->GetTypeFlags returned hr=%d", hr);
      }
      else
      {
        if (!(dtf & PDTF_ISVIEWABLE))
        {
          cs |= SHCOLSTATE_HIDDEN;
        }

        PROPDESC_VIEW_FLAGS dvf;

        hr = ppropdesc->GetViewFlags(&dvf);

        if (!SUCCEEDED(hr))
        {
          Log::WriteOutput(LogType::Error, L"ppropdesc->GetViewFlags returned hr=%d", hr);
        }
        else
        {
          if (dvf & PDVF_SHOWBYDEFAULT)
          {
            cs |= SHCOLSTATE_ONBYDEFAULT;
          }

          if (dvf & PDVF_SHOWINSECONDARYLIST)
          {
            cs |= SHCOLSTATE_SECONDARYUI;
          }

          *pcsFlags = cs;
        }
      }
    }

    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::_GetDefaultColumnState");

    return E_FAIL;
  }
}

STDMETHODIMP CGDriveShlExt::EnumObjects(HWND hwnd, SHCONTF grfFlags, __deref_out IEnumIDList **ppenmIDList)
{
  try
  {
    Log::WriteOutput(LogType::Debug, L"IShellFolder::EnumObjects(HWND hwnd, SHCONTF grfFlags, __deref_out IEnumIDList **ppenmIDList)", hwnd, grfFlags);
    SetDialogType(hwnd);

    typedef CComEnum<IEnumIDList, &__uuidof(IEnumIDList), PITEMID_CHILD, CChildItemIDPolicy> CEnumIDList;

    std::vector<PITEMID_CHILD> rgItems;

    HRESULT hr = _EnumObjects(grfFlags, rgItems);

    if (SUCCEEDED(hr))
    {
      CComObject<CEnumIDList>* pspenm;

      hr = CComObject<CEnumIDList>::CreateInstance(&pspenm);

      if (!SUCCEEDED(hr))
      {
        Log::WriteOutput(LogType::Error, L"CComObject<CEnumIDList>::CreateInstance returned hr=%d", hr);
      }
      else
      {
        pspenm->AddRef();

        if (rgItems.size() > 0)
        {
          hr = pspenm->Init(&rgItems[0], &rgItems[0] + rgItems.size(), NULL, AtlFlagCopy);
        }
        else
        {
          hr = pspenm->Init(NULL, NULL, NULL, AtlFlagCopy);
        }

        if (!SUCCEEDED(hr))
        {
          Log::WriteOutput(LogType::Error, L"CComObject<CEnumIDList>->Init returned hr=%d", hr);
        }
        else
        {
          hr = pspenm->QueryInterface(IID_PPV_ARGS(ppenmIDList));

          if (!SUCCEEDED(hr))
          {
            Log::WriteOutput(LogType::Error, L"CComObject<CEnumIDList>->QueryInterface returned hr=%d", hr);
          }
        }

        pspenm->Release();
      }
    }

    for (size_t i = 0; i < rgItems.size(); i++)
    {
      CoTaskMemFree(rgItems.at(i));
    }

    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::EnumObjects");

    return E_FAIL;
  }
}

HRESULT CGDriveShlExt::_EnumObjects(SHCONTF grfFlags, __inout std::vector<PITEMID_CHILD>& rgItems)
{
  try
  {
    Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_EnumObjects(grfFlags=%X)", grfFlags);

    CDriveItem driveItem;

    bool updateCache = true;

    if (CGDriveShlExt::_didUpdate == true)
    {
      updateCache = false;

      CGDriveShlExt::_didUpdate = false;
    }

    // gets the job done without adding overloads for the new parameter
    HRESULT hr = _GetDataFromID(_id, updateCache, true, driveItem);

    if (SUCCEEDED(hr))
    {
      bool includeFolders = false;
      bool includeFiles = false;

      if (grfFlags & SHCONTF_FOLDERS)
      {
        includeFolders = true;
      }

      if (grfFlags & SHCONTF_NONFOLDERS)
      {
        includeFiles = true;
      }

      rgItems.resize(driveItem.Files()->size()); // assume all elements will be used

      size_t k = 0; // count actual number used

      for (size_t i = 0; SUCCEEDED(hr) && i < driveItem.Files()->size(); i++)
      {
        Fusion::GoogleDrive::FileInfo* file = driveItem.Files()->at(i);

        if ((includeFolders && file->IsFolder()) ||
          (includeFiles && file->IsFile()) ||
          (file->Id == Fusion::GoogleDrive::FileInfo::ERROR_FILEID()))
        {
          {
            CIdList item = rgItems[k];
            hr = _CreateItemID(file, &item);
            rgItems[k] = item.Release();
          }

          Log::WriteOutput(LogType::Warning, L"CGDriveShlExt::_EnumObjects ADD  includeFolder:%d includeFiles:%d numItems:%d i:%d %s \"%s\" trashed:%d", includeFolders, includeFiles, driveItem.Files()->size(), i, file->Id.c_str(), file->Title.c_str(), file->Trashed);

          k++;
        }
        else
        {
          Log::WriteOutput(LogType::Warning, L"CGDriveShlExt::_EnumObjects SKIP includeFolder:%d includeFiles:%d numItems:%d i:%d %s \"%s\" trashed:%d", includeFolders, includeFiles, driveItem.Files()->size(), i, file->Id.c_str(), file->Title.c_str(), file->Trashed);
        }
      }

      rgItems.resize(k); // set real size
    }

    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::_EnumObjects");

    return E_FAIL;
  }
}

HRESULT CGDriveShlExt::_CreateItemID(Fusion::GoogleDrive::FileInfo* fileInfo, CIdList* ppidl)
{
  try
  {
    HRESULT hr = S_OK;
    Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_CreateItemID(Fusion::GoogleDrive::FileInfo* fileInfo, PITEMID_CHILD* ppidl)");
    CHECK_TRUE(fileInfo->Id.length() <= 200,
        // If we fail this check, we'll need to increase the size of the
        // DriveItemSignature buffer.
        E_FAIL);

    DriveItemSignature pri;
    CHECK_TRUE(wcscpy_s(pri.Id, fileInfo->Id.c_str()) == 0,
        E_FAIL);

    {
      LPITEMIDLIST tmpPidl;
      // We used to create an IPropertyStore, but we were not using it for
      // anything, and also have problems with comparing the same ID which had
      // different property stores.
      CHECK_HR(CreateItemID(&pri, nullptr, &tmpPidl));
      ppidl->Reset(tmpPidl);
    }

    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::_CreateItemID");

    return E_FAIL;
  }
}

STDMETHODIMP CGDriveShlExt::GetAttributesOf(UINT cidl, __in_ecount_opt(cidl) PCUITEMID_CHILD_ARRAY rgpidl, __inout SFGAOF *rgfInOut)
{
  try
  {
    Log::WriteOutput(LogType::Debug, L"IShellFolder::GetAttributesOf(UINT cidl, __in_ecount_opt(cidl) PCUITEMID_CHILD_ARRAY rgpidl, __inout SFGAOF *rgfInOut)");
    HRESULT hr = S_OK;
    
    CHECK_ARG(rgpidl != nullptr);
    CHECK_ARG(rgfInOut != nullptr);

    ULONG rgfOut = *rgfInOut;

    for (UINT i = 0; i < cidl; i++)
    {
      DWORD rgfItem;
      CHECK_HR(_GetAttributesOf(rgpidl[i], *rgfInOut, &rgfItem));
      rgfOut &= rgfItem;
    }

    *rgfInOut = rgfOut;

    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::GetAttributesOf");

    return E_FAIL;
  }
}

HRESULT CGDriveShlExt::_GetAttributesOf(PCUITEMID_CHILD pidl, DWORD rgfIn, __out DWORD *prgfOut)
{
  try
  {
    HRESULT hr = S_OK;

    CHECK_ARG(pidl != nullptr);
    CHECK_ARG(prgfOut != nullptr);

    if (ChildIdFromPidl(pidl) == kNewFileSignature.Id)
    {
      if (_newFilePidl == nullptr)
      {
        FileInfo* child = nullptr;
        bool isFolder = (_newFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        CHECK_TRUE(_fileManager.InsertFile(_id, _newFileName, isFolder, &child),
            E_FAIL);
        CHECK_HR(_CreateItemID(child, &_newFilePidl));
        
        CHECK_TRUE(child->CreatePathTo(), E_FAIL);

        CIdList pidlPath;
        CHECK_HR(CIdList::Combine(_spidl, _newFilePidl, pidlPath));
        _didUpdate = true;

        LONG eventId = isFolder? SHCNE_MKDIR : SHCNE_CREATE;
        SHChangeNotify(eventId, SHCNF_IDLIST | SHCNF_FLUSH, pidlPath, pidlPath);
        SHChangeNotify(SHCNE_UPDATEDIR | SHCNE_UPDATEITEM, SHCNF_IDLIST | SHCNF_FLUSH, _spidl, NULL);
      }
      pidl = _newFilePidl;
    }

    FileInfo* fileInfo = nullptr;
    CHECK_HR(_GetDataFromIDList(pidl, false, false, &fileInfo));
    Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_GetAttributesOf %s, flags=%0x", fileInfo->Id.c_str(), rgfIn);

    // The shell expects the file to already exist
    // It doesn't since that's the general idea of this project.
    // I'm resorting to looking for a series of checks that I have observed

    if (fileInfo->IsFile())
    {
      if (CGDriveShlExt::_previousRGFIN == 0x30c10000 && rgfIn == 0x40000000)  // select item via dbl click in open dialog or save dialog
      {
        Log::WriteOutput(LogType::Warning, L"_previousRGFIN=%0x, rgfIn=%0x", CGDriveShlExt::_previousRGFIN, rgfIn);

        if (_hostType == HOSTINFO::SaveAs)
        {
          // When the user navigates to save a file
          // but the file doesn't actually exist on disk
          // We need to create a target file so the user doesn't get a can not locate file error.
          if (!PathInfo::FileExists(fileInfo->FilePath()))
          {
            if(!fileInfo->CreatePathTo())
            {
              Log::Error(L"CGDriveShlExt::_GetAttributesOf failed to create directory for placeholder file");
            }

            if (fileInfo->Description == L"Created by Google Drive Shell Extension" &&
              fileInfo->CreatedDate >= fileInfo->ModifiedDate)
            {
              // this was a file we inserted, and DO NOT want a temp file for, so the application will not prompt to overwrite
            }
            else
            {
              HANDLE hFile = CreateFile(fileInfo->FilePath().c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

              if (hFile == INVALID_HANDLE_VALUE)
              {
                Log::Error(L"CGDriveShlExt::_GetAttributesOf failed when attempting to create a placeholder file");
              }
              else
              {
                CloseHandle(hFile);
              }
            }
          }
        }
        else
        {
          hr = DoDownload(fileInfo);
        }

        CGDriveShlExt::_previousRGFIN = 0;
      }
      else if (rgfIn == 0x30c10000)
      {
        CGDriveShlExt::_previousRGFIN = rgfIn;
      }
      else
      {
        CGDriveShlExt::_previousRGFIN = 0;
      }
    }

    if (SUCCEEDED(hr))
    {
      DWORD dwMask = 0;
      CHECK_HR(_GetAttributesOf(fileInfo, &dwMask));
      *prgfOut = rgfIn & dwMask;
    }

    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::_GetAttributesOf");

    return E_FAIL;
  }
}

HRESULT CGDriveShlExt::_GetAttributesOf(FileInfo* fileInfo, __out DWORD *prgfOut)
{
  try
  {
    Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_GetAttributesOf(FileInfo* fileInfo, __out DWORD *prgfOut)");

    DWORD dwMask = SFGAO_BROWSABLE;

    if (fileInfo->Editable)
    {
      dwMask |= SFGAO_CANRENAME | SFGAO_CANDELETE | SFGAO_CANMOVE;
    }

    // if the item is here, then we can read it, and so we can copy it, it doesn't matter what Google's api tells us
    dwMask |= SFGAO_CANCOPY;

    if (fileInfo->IsFolder())
    {
      dwMask |= SFGAO_FOLDER | SFGAO_FILESYSANCESTOR | SFGAO_STORAGE | SFGAO_DROPTARGET; // SFGAO_STORAGE is required for context menu|background > new > folder

      if (_hostType == HOSTINFO::Open || _hostType == HOSTINFO::SaveAs)
      {
        dwMask |= SFGAO_FILESYSTEM;
      }

      dwMask |= SFGAO_HASSUBFOLDER;
    }
    else // is a file
    {
      dwMask |= SFGAO_FILESYSTEM | SFGAO_STREAM; // SFGAO_STREAM was required to dbl click files in explorer
    }

    *prgfOut = dwMask;

    return S_OK;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::_GetAttributesOf");

    return E_FAIL;
  }
}

HRESULT CGDriveShlExt::_BindToSubfolder(PCUIDLIST_RELATIVE pidl, REFIID riid, __deref_out void **ppv) const
{
  Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_BindToSubfolder");
  CIdList absPidlChild;
  HRESULT hr = CIdList::Combine(_spidl, pidl, absPidlChild);
  if ( !SUCCEEDED(hr) )
    return hr;

  CComObject<CGDriveShlExt> * subFolder;
  hr = CComObject<CGDriveShlExt>::CreateInstance(&subFolder);
  if ( !SUCCEEDED(hr) )
    return hr;

  subFolder->AddRef();
  hr = subFolder->Initialize(absPidlChild);
  if ( !SUCCEEDED(hr) )
  {
    subFolder->Release();
    return hr;
  }

  hr = subFolder->QueryInterface( riid, ppv );
  subFolder->Release();
  return hr;
}

STDMETHODIMP CGDriveShlExt::BindToObject(PCUIDLIST_RELATIVE pidl, __in IBindCtx *pbc, REFIID riid, __deref_out void **ppv)
{
  try
  {
    HRESULT hr = S_OK;

    CHECK_ARG(pidl != nullptr);
    CHECK_ARG(ppv != nullptr);

    Log::WriteOutput(LogType::Debug, L"IShellFolder::BindToObject");

    if (ChildIdFromPidl(pidl) == kNewFileSignature.Id)
    {
      pidl = _newFilePidl;
    }

    *ppv = NULL;
    hr = E_NOINTERFACE;

    if (riid == IID_IShellFolder || riid == IID_IShellFolder2)
    {
      CHECK_HR(_BindToSubfolder(pidl, riid, ppv));
    }
    else if (ILIsChild(pidl))
    {
      if (riid == IID_IStream || riid == IID_IPersistFile)
      {
        if (riid == IID_IStream)
        {
          Log::WriteOutput(LogType::Warning, L"CGDriveShlExt::BindToObject IID_IStream::CDriveItemStream");
        }
        else if (riid == IID_IPersistFile)
        {
          Log::WriteOutput(LogType::Warning, L"CGDriveShlExt::BindToObject IID_IPersistFile::CDriveItemStream");
        }

        CDriveItem driveItem;

        CHECK_HR(_GetDataFromIDList(pidl, false, false, driveItem));
        BIND_OPTS options;
        CHECK_HR(_GetContextOptions(pbc, options));
        CHECK_HR(CDriveItemStream::CreateInstanceReturnInterfaceTo(driveItem, options, riid, ppv));
      }
      else if (riid == IID_IIdentityName || riid == IID_ITransferMediumItem || riid == IID_IViewStateIdentityItem || riid == IID_IDisplayItem)
      {
        if (riid == IID_IIdentityName)
        {
          Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::BindToObject IID_IIdentityName::CDriveItemRelatedItem");
        }
        else if (riid == IID_ITransferMediumItem)
        {
          Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::BindToObject IID_ITransferMediumItem::CDriveItemRelatedItem");
        }
        else if (riid == IID_IViewStateIdentityItem)
        {
          Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::BindToObject IID_IViewStateIdentityItem::CDriveItemRelatedItem");
        }
        else if (riid == IID_IDisplayItem)
        {
          Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::BindToObject IID_IDisplayItem::CDriveItemRelatedItem");
        }

        CComObject<CGDriveShlExt>* drive;

        CHECK_HR(CGDriveShlExt::CreateInstance(_spidl, pidl, &drive));
        drive->AddRef();
        hr = CDriveItemRelatedItem::CreateInstanceReturnInterfaceTo(drive, riid, ppv);
        drive->Release();
      }
      else if (riid == IID_IPropertyStoreFactory)
      {
        Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::BindToObject IID_IPropertyStoreFactory::CDriveItemPropertyStoreFactory");

        CHECK_HR(CDriveItemPropertyStoreFactory::CreateInstanceReturnInterfaceTo(this, pidl, riid, ppv));
      }
      else if (riid == SDefined_ILibraryDescription || riid == SDefined_Unknown1 || riid == SDefined_IItemSetOperations || riid == SDefined_IInterruptItem)
      {
        _LogRPC(riid, ppv);
      }
      else
      {
        Log::LogUnknownIID(riid, L"CGDriveShlExt::BindToObject()");
      }
    }
    else
    {
      CComPtr<IShellFolder> ppsf;
      PCUITEMID_CHILD pidlChild;

      CHECK_HR(SHBindToFolderIDListParent(this, pidl, IID_PPV_ARGS(&ppsf), &pidlChild));
      CHECK_HR(ppsf->BindToObject(pidlChild, pbc, riid, ppv));
    }
    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::BindToObject");

    return E_FAIL;
  }
}

STDMETHODIMP CGDriveShlExt::BindToStorage(PCUIDLIST_RELATIVE pidl, __in IBindCtx *pbc, REFIID riid, __deref_out void **ppv)
{
  try
  {
    Log::WriteOutput(LogType::Warning, L"IShellFolder::BindToStorage(PCUIDLIST_RELATIVE pidl, __in IBindCtx *pbc, REFIID riid, __deref_out void **ppv)", pidl);
    return BindToObject(pidl, pbc, riid, ppv);
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::BindToStorage");

    return E_FAIL;
  }
}

STDMETHODIMP CGDriveShlExt::CompareIDs(LPARAM lParam, PCUIDLIST_RELATIVE pidl1, PCUIDLIST_RELATIVE pidl2)
{
  HRESULT hr = S_OK;
  Log::WriteOutput(LogType::Debug, L"IShellFolder::CompareIDs LPARAM:%X pidlLength (%d, %d) itemLength (%d, %d)", lParam, ILGetSize(pidl1), ILGetSize(pidl2), pidl1->mkid.cb, pidl2->mkid.cb);
  
  int result = 0;
  CHECK_HR(_CompareIDs(lParam, pidl1, pidl2, result));
  hr = MAKE_HRESULT(SEVERITY_SUCCESS, 0, (unsigned short)result);

  Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::CompareIDs resultCode (%08X)", hr);
  return hr;
}

HRESULT CGDriveShlExt::_CompareIDs(LPARAM lParam, PCUIDLIST_RELATIVE pidl1, PCUIDLIST_RELATIVE pidl2, int& result)
{
  HRESULT hr = S_OK;
  Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_CompareIDs(LPARAM lParam, PCUIDLIST_RELATIVE pidl1, PCUIDLIST_RELATIVE pidl2, int& result)");

  result = 0;
  CHECK_HR(_CompareForEmptyIDs(pidl1, pidl2, result));

  if (result == 0)
  {
    std::wstring id1 = ChildIdFromPidl(pidl1);
    std::wstring id2 = ChildIdFromPidl(pidl2);

    // compare by id, which are unique
    // if the id is the same then its the same item
    // possible when comparing pidls with multiple levels
    // the current level could be the same item for both
    if (id1.compare(id2) == 0)
    {
      Log::WriteOutput(LogType::Information, L"CGDriveShlExt::_CompareIDs SameID parentid:%s childid:%s", this->Id().c_str(), id1.c_str());

      // Do a recursive comparison
      CComPtr<IShellFolder2> spsf2;

      CHECK_HR(BindToObject(pidl1, NULL, IID_PPV_ARGS(&spsf2)));
      CHECK_HR(spsf2->CompareIDs(lParam, ILNext(pidl1), ILNext(pidl2)));

      if ((short)(hr & 0xffff) < 0)
      {
        result = -1;
      }
      else if ((short)(hr & 0xffff) > 0)
      {
        result = 1;
      }

    }
    else
    {
      CDriveItem driveItem1;
      CDriveItem driveItem2;

      CHECK_HR(_GetDataFromID(id1, false, false, driveItem1));
      CHECK_HR(_GetDataFromID(id2, false, false, driveItem2));
      CHECK_HR(_CompareDriveItem(lParam, driveItem1, driveItem2, result)); // comparing left most item
    }
  }

  return hr;
}

HRESULT CGDriveShlExt::_CompareDriveItem(LPARAM lParam, CDriveItem& driveItem1, CDriveItem&  driveItem2, int& result)
{
  Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_CompareDriveItem(LPARAM lParam, const CDriveItem& pidl1, const CDriveItem&  pidl2, int& result)");

  HRESULT hr = S_OK;
  result = 0;

  // Comparison rules:
  //   Default is by folder/not folder and name
  //   If another column is chosen, we sort by folder/not folder, column, and name.
  DriveItemType::eType driveItemType1 = driveItem1.Type();
  DriveItemType::eType driveItemType2 = driveItem2.Type();

  if (driveItemType1 < driveItemType2)
  {
    result = -1;
  }
  else if (driveItemType1 > driveItemType2)
  {
    result = 1;
  }
  else if ((lParam & SHCIDS_COLUMNMASK) != 0)
  {
    switch (lParam)
    {
      case (0x1): // Type (File Extension)
        result = _wcsicmp(driveItem1.FileExtension().c_str(), driveItem2.FileExtension().c_str());
        break;
      case (0xb): // Modified Date
        result = _wcsicmp(driveItem1.DateModified().c_str(), driveItem2.DateModified().c_str());
        break;
      case (0xc): // Size
        if (!driveItem1.Size().empty() && !driveItem2.Size().empty())
        {
          const unsigned long long size1 = stoull(driveItem1.Size());
          const unsigned long long size2 = stoull(driveItem2.Size());

          if (size1 > size2)
            result = 1;
          else if (size1 < size2)
            result = -1;
        }
        break;
      case (0xd): // Creation Date
        result = _wcsicmp(driveItem1.DateCreated().c_str(), driveItem2.DateCreated().c_str());
        break;
      default:
        Log::WriteOutput(LogType::Error, L"CGDriveShlExt::_CompareIDs requested to sort by unsupported lparam: 0x%X", lParam);
    }
  }

  const std::wstring& driveItemTypeName1 = driveItem1.ItemName();
  const std::wstring& driveItemTypeName2 = driveItem2.ItemName();
  if (result == 0)
  {
    result = _wcsicmp( driveItemTypeName1.c_str(), driveItemTypeName2.c_str());
  }

  if (result == 0)
  {
    Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_CompareIDs I1(%s) is the same as I2(%s)", driveItemTypeName1.c_str(), driveItemTypeName2.c_str());
  }
  else if (result < 0)
  {
    Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_CompareIDs I1(%s) comes before I2(%s)", driveItemTypeName1.c_str(), driveItemTypeName2.c_str());
  }
  else
  {
    Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_CompareIDs I2(%s) comes before I1(%s)", driveItemTypeName2.c_str(), driveItemTypeName1.c_str());
  }

  return hr;
}

HRESULT CGDriveShlExt::_CompareForEmptyIDs(PCUIDLIST_RELATIVE pidl1, PCUIDLIST_RELATIVE pidl2, int& result)
{
  result = 0;
  HRESULT hr = S_OK;

  // sanity checks
  if (ILIsEmpty(pidl1))
  {
    if (ILIsEmpty(pidl2))
    {
      Log::WriteOutput(LogType::Error, L"CGDriveShlExt::_CompareIDs SameID, and no more children. Returned E_INVALIDARG");
      // Verified this result by
      // getting the desktop folder
      // getting a pidl for a file in C:\
      // copied the ID
      // asked the desktop folder to CompareID, the ID to the Copy
      hr = E_INVALIDARG;
    }
    else
    {
      Log::WriteOutput(LogType::Warning, L"CGDriveShlExt::_CompareIDs pidl1 has less levels");
      result = -1;
    }
  }
  else if (ILIsEmpty(pidl2))
  {
    Log::WriteOutput(LogType::Warning, L"CGDriveShlExt::_CompareIDs pidl1 has more levels");
    result = 1;
  }

  return hr;
}

STDMETHODIMP CGDriveShlExt::GetDetailsEx(PCUITEMID_CHILD pidl, const PROPERTYKEY *pkey, __out VARIANT *pvar)
{
  try
  {
    HRESULT hr = S_OK;
    Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::GetDetailsEx(PCUITEMID_CHILD pidl, const PROPERTYKEY *pkey=%d, __out VARIANT *pvar)", pkey->pid);

    CPropVariant spropvar;

    CHECK_HR(PropertyHelper::GetProperty(this, pidl, *pkey, &spropvar));
    CHECK_HR(PropVariantToVariant(&spropvar, pvar));

    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::GetDetailsEx");

    return E_FAIL;
  }
}

STDMETHODIMP CGDriveShlExt::GetUIObjectOf(HWND hwndOwner, UINT cidl, __in_ecount_opt(cidl) PCUITEMID_CHILD_ARRAY rgpidl, REFIID riid, __reserved UINT * /*rgfReserved*/, __deref_out void **ppv)
{
  try
  {
    Log::WriteOutput(LogType::Debug, L"IShellFolder::GetUIObjectOf");
    SetDialogType(hwndOwner);

    HRESULT hr = E_NOINTERFACE;

    if (riid == IID_IContextMenu || riid == IID_IContextMenu2 || riid == IID_IContextMenu3)
    {
      Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::GetUIObjectOf IID_IContextMenu");

      hr = _CreateContextMenu(hwndOwner, cidl, rgpidl, riid, ppv);
    }
    else if (riid == IID_IDataObject)
    {
      Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::GetUIObjectOf IID_IDataObject");

      IDataObject* pdtInner = NULL;

      hr = CDriveItemDataObject::CreateInstanceReturnInterfaceTo(this, riid, (void**)(&pdtInner));

      if (SUCCEEDED(hr))
      {
        hr = SHCreateDataObject(_spidl, cidl, rgpidl, pdtInner, riid, ppv);

        if (SUCCEEDED(hr))
        {
          size_t folderCount = 0;

          for (UINT i = 0; i < cidl; i++)
          {
            CDriveItem item;

            GetDriveItemFromIDList(rgpidl[i], false, false, item);

            if (item.IsFolder())
            {
              folderCount++;
            }
          }

          if (SUCCEEDED(hr))
          {

            UINT index = 90;// Only 1 document;

            if (cidl == 1) // Only 1 item
            {
              if (folderCount > 0) // Item is a Folder
              {
                index = 4;
              }
            }
            else // Multiple Items
            {
              if (folderCount == cidl) // All are Folders
              {
                index = 4;
              }
              else if (folderCount == 0) // No Folders
              {
                index = 90;
              }
              else if (folderCount == 1) // mixed content, single folder
              {
                index = 112;
              }
              else // mixed content, multiple folders
              {
                index = 112;
              }
            }

            int iconSize = 256;
            HMODULE hMod = GetModuleHandle(L"imageres.dll");

            if (hMod == NULL)
            {
              hMod = LoadLibrary(L"imageres.dll");
            }

            // Sticking to some well known windows icons for now..
            HICON hIcon = (HICON)LoadImage( hMod, MAKEINTRESOURCE(index), IMAGE_ICON, iconSize,iconSize, LR_DEFAULTCOLOR | LR_SHARED );

            if (hIcon != NULL)
            {
              HDC hDC = CreateCompatibleDC(NULL);

              if (hDC != NULL)
              {
                HBITMAP hBitmap = CreateBitmap(iconSize, iconSize, 1, 32, NULL);

                if (hBitmap != NULL)
                {
                  HBITMAP hbmPrev = (HBITMAP)SelectObject(hDC, hBitmap);

                  if (hbmPrev != NULL)
                  {
                    if(DrawIconEx(hDC, 0, 0, hIcon, iconSize, iconSize, 0, NULL, DI_NORMAL))
                    {
                      SelectObject(hDC, hbmPrev);

                      if(DeleteDC(hDC))
                      {
                        CComPtr<IDragSourceHelper2> helper;

                        hr = CoCreateInstance(CLSID_DragDropHelper, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&helper));

                        if (SUCCEEDED(hr))
                        {
                          SHDRAGIMAGE image;
                          ZeroMemory(&image, sizeof(SHDRAGIMAGE));
                          image.sizeDragImage.cx = iconSize;
                          image.sizeDragImage.cy = iconSize;
                          image.hbmpDragImage = hBitmap;
                          image.crColorKey = CLR_NONE;
                          image.ptOffset.x = 48;
                          image.ptOffset.y = 88;

                          hr = helper->InitializeFromBitmap(&image, (IDataObject*)*ppv);

                          helper.Release();
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    else if (riid == IID_IDropTarget)
    {
      Log::WriteOutput(LogType::Error, L"CGDriveShlExt::GetUIObjectOf IID_IDropTarget");

      if (cidl != 1)
      {
        hr = E_INVALIDARG;

        Log::WriteOutput(LogType::Error, L"CGDriveShlExt::GetUIObjectOf Request for IID_IDropTarget can not be cidl != 1 per documentation");
      }
      else
      {
        CDriveItem driveItem;

        hr = GetDriveItemFromIDList(rgpidl[0], false, false, driveItem);

        if (SUCCEEDED(hr) && driveItem.IsFolder())
        {
          CComObject<CGDriveShlExt>* drive;

          hr = CGDriveShlExt::CreateInstance(_spidl, rgpidl[0], &drive);

          if (SUCCEEDED(hr))
          {
            drive->AddRef();
            hr = CDriveItemDataObject::CreateInstanceReturnInterfaceTo(drive, riid, ppv);
          }

          drive->Release();
        }
        else
        {
          hr = E_NOINTERFACE;
        }
      }
    }
    else if (riid == IID_IExtractIcon)
    {
      Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::GetUIObjectOf IID_IExtractIcon");

      if (cidl != 1)
      {
        hr = E_INVALIDARG;

        Log::WriteOutput(LogType::Error, L"CGDriveShlExt::GetUIObjectOf Request for IID_IExtractIcon can not be cidl != 1 per documentation");
      }
      else
      {
        CDriveItem driveItem;

        hr = _GetDataFromIDList(rgpidl[0], false, false, driveItem);

        if (SUCCEEDED(hr))
        {
          int icon = 0;

          switch (driveItem.Type())
          {
          case DriveItemType::Root:
            icon = -IDI_ROOT;
            break;

          case DriveItemType::Folder:
            icon = -IDI_FOLDER;
            break;

          default:
            icon = -IDI_FILE;
            break;
          }

          TCHAR strModulePath[MAX_PATH] = {};

          if (!driveItem.IsFile() || driveItem.FileInfo()->Association.DefaultIcon.length() == 0)
          {
            if (!GetModuleFileName(_AtlBaseModule.GetModuleInstance(), strModulePath, MAX_PATH))
            {
              hr = AtlHresultFromLastError();

              if (!SUCCEEDED(hr))
              {
                Log::WriteOutput(LogType::Error, L"GetModuleFileName returned hr=%d", hr);
              }
            }
          }

          if (SUCCEEDED(hr))
          {
            CComPtr<IDefaultExtractIconInit> spdxi;

            hr = SHCreateDefaultExtractIcon(IID_PPV_ARGS(&spdxi));

            if (!SUCCEEDED(hr))
            {
              Log::WriteOutput(LogType::Error, L"SHCreateDefaultExtractIcon returned hr=%d", hr);
            }
            else
            {
              /*
              This should not be GIL_PERINSTANCE as that will take up lots of space in the icon
              cache. Instead this should be per type. but to do that we need to report the type
              of the items properly using a PKEY (I need to investigate the details of this)
              */

              hr = spdxi->SetFlags(GIL_PERINSTANCE);

              if (!SUCCEEDED(hr))
              {
                Log::WriteOutput(LogType::Error, L"CComPtr<IDefaultExtractIconInit>->SetFlags returned hr=%d", hr);
              }
              else
              {
                if (_tcsnlen(strModulePath, MAX_PATH) > 0)
                {
                  hr = spdxi->SetNormalIcon(strModulePath, icon);
                }
                else if (driveItem.FileInfo()->Association.DefaultIcon.length() > 0)
                {
                  LPCWSTR defaultIcon = (const wchar_t*)driveItem.FileInfo()->Association.DefaultIcon.c_str();

                  if (driveItem.FileInfo()->Association.DefaultIconIndex.length() == 0)
                  {
                    icon = 0;
                  }
                  else
                  {
                    icon = std::stoi(driveItem.FileInfo()->Association.DefaultIconIndex.c_str());
                  }

                  hr = spdxi->SetNormalIcon(defaultIcon, icon);
                }

                if (!SUCCEEDED(hr))
                {
                  Log::WriteOutput(LogType::Error, L"CComPtr<IDefaultExtractIconInit>->SetNormalIcon returned hr=%d", hr);
                }
                else
                {
                  hr = spdxi->QueryInterface(riid, ppv);

                  if (!SUCCEEDED(hr))
                  {
                    Log::WriteOutput(LogType::Error, L"CComPtr<IDefaultExtractIconInit>->QueryInterface returned hr=%d", hr);
                  }
                }
              }
            }
          }
        }
      }
    }
    else if (riid == IID_IQueryAssociations)
    {
      Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::GetUIObjectOf IQueryAssociations");

      if (cidl != 1)
      {
        hr = E_INVALIDARG;

        Log::WriteOutput(LogType::Error, L"CGDriveShlExt::GetUIObjectOf Request for IID_IQueryAssociations can not be cidl != 1 per documentation");
      }
      else
      {
        CDriveItem driveItem;

        hr = _GetDataFromIDList(rgpidl[0], false, false, driveItem);

        if (SUCCEEDED(hr))
        {
          if (driveItem.IsFile())
          {
            hr = E_NOTIMPL;

            HRESULT hr2 = E_FAIL;

            if (driveItem.IsGoogleDoc())
            {
              PCWSTR app = (const wchar_t*)driveItem.FileInfo()->Association.Application.c_str();

              ASSOCIATIONELEMENT rgAssoc[] =
              {
                { ASSOCCLASS_APP_STR, NULL, app },
              };

              HRESULT hr2 = AssocCreateForClasses(rgAssoc, ARRAYSIZE(rgAssoc), riid, ppv);

              if (!SUCCEEDED(hr2))
              {
                Log::WriteOutput(LogType::Error, L"AssocCreateForClasses returned hr2=%d", hr2);
              }
              else
              {
                hr = S_OK;
              }
            }
            else
            {
              // Check rundll32, its being launched for something, figure it out

              UINT entryCount = 0;
              std::wstring extention = driveItem.FileInfo()->Association.FileExtension;

              std::wstring perceivedTypeString;
              HRESULT perceivedTypeHR = driveItem.PreceivedType(perceivedTypeString);

              if (SUCCEEDED(perceivedTypeHR))
              {
                entryCount += 2;
              }

              DWORD cbData = 200;
              DWORD dwType = REG_SZ;

              WCHAR key[200] = L"SystemFileAssociations\\";
              wcscat_s(key, 200, extention.c_str());
              HKEY result;

              LSTATUS systemFileAssociationsSt = RegOpenKeyEx(HKEY_CLASSES_ROOT, key, NULL, KEY_QUERY_VALUE, &result); // just check if it exists

              if (systemFileAssociationsSt == ERROR_SUCCESS)
              {
                entryCount += 1;
              }

              LSTATUS extentionRootSt = RegOpenKeyEx(HKEY_CLASSES_ROOT, extention.c_str(), NULL, KEY_QUERY_VALUE, &result);
              LSTATUS extentionRootDefaultSt = ERROR_FILE_NOT_FOUND;
              WCHAR extentionRootDefaultType[200];

              if (extentionRootSt == ERROR_SUCCESS)
              {
                entryCount += 1;

                extentionRootDefaultSt = SHGetValue(HKEY_CLASSES_ROOT, extention.c_str(), NULL, &dwType, extentionRootDefaultType, &cbData);

                if (extentionRootDefaultSt == ERROR_SUCCESS)
                {
                  entryCount += 1;
                }
              }

              if (entryCount == 0)
              {
                entryCount++; // If the above checks don't exist, we'll add an unknown element.
              }

              entryCount += 2; // AllFilesystemObjects and * are always added are always generated.

              std::vector<ASSOCIATIONELEMENT> rgAssoc(entryCount);
              size_t idx = 0;

              if (SUCCEEDED(perceivedTypeHR))
              {
                rgAssoc[idx].ac = ASSOCCLASS_PROGID_STR;
                rgAssoc[idx].hkClass = NULL;
                rgAssoc[idx].pszClass = perceivedTypeString.c_str();
                idx++;
              }

              if (extentionRootSt == ERROR_SUCCESS)
              {
                rgAssoc[idx].ac = ASSOCCLASS_PROGID_STR;
                rgAssoc[idx].hkClass = NULL;
                rgAssoc[idx].pszClass = extention.c_str();
                idx++;
              }

              if (extentionRootSt == ERROR_SUCCESS)
              {
                if (extentionRootDefaultSt == ERROR_SUCCESS)
                {
                  rgAssoc[idx].ac = ASSOCCLASS_PROGID_STR;
                  rgAssoc[idx].hkClass = NULL;
                  rgAssoc[idx].pszClass = extentionRootDefaultType;
                  idx++;
                }
              }

              if (systemFileAssociationsSt == ERROR_SUCCESS)
              {
                rgAssoc[idx].ac = ASSOCCLASS_SYSTEM_STR;
                rgAssoc[idx].hkClass = NULL;
                rgAssoc[idx].pszClass = extention.c_str();
                idx++;
              }

              if (SUCCEEDED(perceivedTypeHR))
              {
                rgAssoc[idx].ac = ASSOCCLASS_SYSTEM_STR;
                rgAssoc[idx].hkClass = NULL;
                rgAssoc[idx].pszClass = perceivedTypeString.c_str();
                idx++;
              }

              rgAssoc[idx].ac = ASSOCCLASS_STAR;
              rgAssoc[idx].hkClass = NULL;
              rgAssoc[idx].pszClass = NULL;
              idx++;

              rgAssoc[idx].ac = ASSOCCLASS_PROGID_STR;
              rgAssoc[idx].hkClass = NULL;
              rgAssoc[idx].pszClass = L"AllFilesystemObjects";
              idx++;

              if (!SUCCEEDED(perceivedTypeHR) &&
                !(systemFileAssociationsSt == ERROR_SUCCESS) &&
                !(extentionRootSt == ERROR_SUCCESS))
              {
                // Use if we don't know anything else
                // HKEY_CLASSES_ROOT\Unknown
                rgAssoc[idx].ac = ASSOCCLASS_PROGID_STR;
                rgAssoc[idx].hkClass = NULL;
                rgAssoc[idx].pszClass = L"Unknown";
                idx++;
              }

              // Need to set ASSOCCLASS_SYSTEM_STR, the perceived type
              // I want to look into this more, I think we can do better
              // http://msdn.microsoft.com/en-us/library/windows/desktop/ee872122%28v=vs.85%29.aspx

              hr = AssocCreateForClasses(&rgAssoc[0], entryCount, riid, ppv);

              if (!SUCCEEDED(hr))
              {
                Log::WriteOutput(LogType::Error, L"AssocCreateForClasses building classes for file failed, trying some simple defaults", hr2);

                ASSOCIATIONELEMENT rgAssoc2[] =
                {
                  { ASSOCCLASS_PROGID_STR, NULL, extention.c_str() },
                  { ASSOCCLASS_STAR, NULL, NULL },
                };

                hr = AssocCreateForClasses(rgAssoc2, ARRAYSIZE(rgAssoc2), riid, ppv);

                if (!SUCCEEDED(hr))
                {
                  Log::WriteOutput(LogType::Error, L"AssocCreateForClasses simple defaults also failed", hr2);
                }
              }
            }
          }
          else
          {
            ASSOCIATIONELEMENT rgAssoc[] =
            {
              { ASSOCCLASS_FOLDER, NULL, NULL },
              { ASSOCCLASS_PROGID_STR, NULL, L"Directory" },
              { ASSOCCLASS_PROGID_STR, NULL, L"AllFilesystemObjects" },
            };

            hr = AssocCreateForClasses(rgAssoc, ARRAYSIZE(rgAssoc), riid, ppv);

            if (!SUCCEEDED(hr))
            {
              Log::WriteOutput(LogType::Error, L"AssocCreateForClasses failed for folders", hr);

            }
          }
        }
      }
    }
    else if (riid == IID_IQueryInfo)
    {
      Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::GetUIObjectOf Request for IQueryInfo is not implemented");
    }
    else if (riid == IID_IShellLink)
    {
      Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::CreateViewObject IID_IShellLink we won't be containing shortcuts, no need to impliment");
    }
    else if (riid == IID_IExtractImage)
    {
      Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::GetUIObjectOf IID_IExtractImage TODO Exposes methods that request a thumbnail image from a Shell folder.  Used for Thumbnails");
    }
    else if (riid == IID_IItemNameLimits)
    {
      Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::GetUIObjectOf IID_IItemNameLimits DO NOT NEED TO IMPLIMENT. Retrieves a list of valid and invalid characters or the maximum length of a name in the namespace. Use this interface for validation parsing and translation.");
    }
    else
    {
      Log::LogUnknownIID(riid, L"CGDriveShlExt::GetUIObjectOf()");
    }

    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::GetUIObjectOf");

    return E_FAIL;
  }
}

STDMETHODIMP CGDriveShlExt::ParseDisplayName(HWND hwnd, __in IBindCtx *pbc, __in PWSTR pszDisplayName, __inout ULONG *pchEaten, __deref_out PIDLIST_RELATIVE *ppidl, __inout ULONG *pdwAttributes)
{
  try
  {
    Log::WriteOutput(LogType::Warning, L"IShellFolder::ParseDisplayName name=%s", pszDisplayName);
    HRESULT hr = S_OK;

    CHECK_ARG(pszDisplayName != nullptr);
    CHECK_ARG(ppidl != nullptr);

    if (pdwAttributes != nullptr)
    {
      Log::WriteOutput(LogType::Warning, L"Attributes= %d", *pdwAttributes);
    }

    SetDialogType(hwnd);

    // pdwAttrbiute is a set of SFGAOF flags, it's optional to filter by these

    std::wstring nameOfChild; // To be determined
    std::wstring pathFollowingChild(pszDisplayName);
    CIdList ppidlOfChild;

    BIND_OPTS options;
    CHECK_HR(_GetContextOptions(pbc, options));
    Log::WriteOutput(LogType::Warning, L"Options flags= %d, mode = %d", options.grfFlags, options.grfMode);

    if (SUCCEEDED(
        _NextNameSegment(pathFollowingChild, nameOfChild, &ppidlOfChild, pdwAttributes)))
    {
      if (pathFollowingChild.length() == 0)
      {
        CHECK_TRUE((options.grfMode & STGM_CREATE) == 0,
            // We found a matching entry, but they want to create a new item
            // with the same name, this we will not allow, so fail it out.
            HRESULT_FROM_WIN32(ERROR_FILE_EXISTS)); 
        CHECK_TRUE((options.grfFlags & BIND_JUSTTESTEXISTENCE) == 0,
            HRESULT_FROM_WIN32(ERROR_FILE_EXISTS));
        
        CIdList outPidl;
        CHECK_HR(CIdList::Clone(ppidlOfChild, outPidl));
        *ppidl = outPidl.Release();
      }
      else
      {
        CComPtr<IShellFolder> spsf;

        CHECK_HR(BindToObject(ppidlOfChild, pbc, IID_PPV_ARGS(&spsf)));

        CIdList spidlNext;
        {
          LPITEMIDLIST tmpNext;
          // This const cast is dangerous, and relies on knowing that ParseDisplayName will not attempt to overwrite the parameter
          // Since we know we're calling this on only our own objects, we can be sure.
          // DO NOT EDIT pszDisplayName in this function
          CHECK_HR(spsf->ParseDisplayName(hwnd, pbc, const_cast<WCHAR*>(pathFollowingChild.c_str()), NULL, &tmpNext, pdwAttributes));
          spidlNext.Reset(tmpNext);
        }

        CIdList outPidl;
        CHECK_HR(CIdList::Combine(ppidlOfChild, spidlNext, outPidl));
        *ppidl = outPidl.Release();
      }
    }
    else
    {
      CHECK_TRUE((options.grfMode & STGM_CREATE) != 0,
          HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND));
      CHECK_TRUE(pathFollowingChild.length() == 0 && nameOfChild.length() > 0,
          E_FAIL);
      CHECK_TRUE(nameOfChild.find(L"Google Drive") != 0,
          // Service cannot create root file
          E_FAIL);
    
      _newFileName = nameOfChild;
      _newFileAttributes = pdwAttributes != nullptr? *pdwAttributes : 0;
      CHECK_HR(CreateItemID(&kNewFileSignature, nullptr, ppidl));
      _newFilePidl.Reset();
    }

    if (pchEaten != NULL)
    {
      // We parse the entire string, or we fail.
      (*pchEaten) = (ULONG)_tcslen(pszDisplayName);
    }

    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::ParseDisplayName");

    return E_FAIL;
  }
}

HRESULT CGDriveShlExt::_NextNameSegment(__inout std::wstring& ppszInOut, __out std::wstring& ppszSegment, __deref_out_opt CIdList *ppidlOut, ULONG *pdwAttributes)
{
  try
  {
    Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_NextNameSegment");
    HRESULT hr = S_OK;

    std::wstring seperator = L"\\";

    size_t seperatorPos = ppszInOut.find_first_of(seperator);

    if (seperatorPos != std::wstring::npos)
    {
      ppszSegment.assign(ppszInOut.substr(0, seperatorPos));
      ppszInOut.assign(ppszInOut.substr(seperatorPos + seperator.length()));
    }
    else
    {
      ppszSegment.assign(ppszInOut);
      ppszInOut.assign(L"");
    }

    CDriveItem item;

    CHECK_HR(_GetDataFromIDList(_spidl, false, true, item));

    hr = E_FAIL;
    auto& itemFiles = *item.Files();
    for (auto it = itemFiles.begin(); it != itemFiles.end(); it++)
    {
      FileInfo* child = *it;

      // This showed up with WantsFORPARSING was set, which is not normally set
      // lets just keep this code in place till we find a reason not to
      if (child->Id.compare(ppszSegment) == 0 && child->Title.compare(ppszInOut) == 0)
      {
        // changing ppszSegment here, so the next compare is different
        ppszSegment.assign(ppszInOut);
        ppszInOut.assign(L"");
      }

      if (child->Title.compare(ppszSegment) == 0) // Should this be case insensitive?  Windows doesn't allow it but Google does
      {
        if (pdwAttributes != NULL)
        {
          CHECK_HR(_CreateItemID(child, ppidlOut));
          DWORD dwMask = 0;
          CHECK_HR(_GetAttributesOf(child, &dwMask));
          *pdwAttributes = *pdwAttributes & dwMask;
        }
      }
    }

    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::_NextNameSegment");

    return E_FAIL;
  }
}

STDMETHODIMP CGDriveShlExt::SetNameOf(HWND hwnd, PCUITEMID_CHILD pidl, LPCWSTR pszName, SHGDNF /*uFlags*/, __deref_out_opt PITEMID_CHILD *ppidlOut)
{
  try
  {
    Log::WriteOutput(LogType::Warning, L"IShellFolder::SetNameOf(HWND hwnd, PCUITEMID_CHILD pidl, LPCWSTR pszName, SHGDNF uFlags, __deref_out_opt PITEMID_CHILD *ppidlOut)");
    SetDialogType(hwnd);

    if (ppidlOut != NULL)
    {
      *ppidlOut = NULL;
    }

    if (wcslen(pszName) <= 0)
      return S_OK;

    if (!_RenameFile(pidl, pszName))
      return E_FAIL;

    HRESULT hr = S_OK;

    if (ppidlOut != NULL)
    {
      {
        CIdList tmpPidl = *ppidlOut;
        hr = CIdList::CloneChild(pidl, tmpPidl);
        *ppidlOut = tmpPidl.Release();
      }

      if (!SUCCEEDED(hr))
      {
        Log::WriteOutput(LogType::Error, L"CloneIDList returned hr=%d", hr);
      }
    }

    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::SetNameOf");

    return E_FAIL;
  }
}

STDMETHODIMP CGDriveShlExt::GetDefaultSearchGUID(__out GUID * /*pguid*/)
{
  try
  {
    Log::WriteOutput(LogType::Warning, L"CGDriveShlExt::GetDefaultSearchGUID(__out GUID *pguid) ");

    return E_NOTIMPL;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::GetDefaultSearchGUID");

    return E_FAIL;
  }
}

STDMETHODIMP CGDriveShlExt::EnumSearches(__deref_out IEnumExtraSearch ** /*ppenum*/)
{
  try
  {
    Log::WriteOutput(LogType::Warning, L"CGDriveShlExt::EnumSearches(_deref_out IEnumExtraSearch **ppenum)");

    return E_NOTIMPL;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::EnumSearches");

    return E_FAIL;
  }
}

const CGDriveShlExt::DISPLAYNAMEOFINFO CGDriveShlExt::_DisplayNameOfInfo[] =
{
  { &CGDriveShlExt::_GetDisplayNameOfDisplayName },
  { &CGDriveShlExt::_GetDisplayNameOfDisplayPath },
  { &CGDriveShlExt::_GetDisplayNameOfParsingName },
  { &CGDriveShlExt::_GetDisplayNameOfParsingPath },
};

STDMETHODIMP CGDriveShlExt::GetDisplayNameOf(PCUITEMID_CHILD pidl, SHGDNF uFlags, __out STRRET *psrName)
{
  try
  {
    HRESULT hr = S_OK;
    Log::WriteOutput(LogType::Debug, L"IShellFolder::GetDisplayNameOf(PCUITEMID_CHILD pidl, SHGDNF uFlags, __out STRRET *psrName)");

    static const BYTE _indices[] =
    {
      //  FOREDITING  FORPARSING  FORADDRESSBAR  INFOLDER
      /*       0           0            0            0    */  DISPLAYNAMEOFINFO::GDNI_RELATIVEFRIENDLY,
      /*       0           0            0            1    */  DISPLAYNAMEOFINFO::GDNI_RELATIVEFRIENDLY,
      /*       0           0            1            0    */  DISPLAYNAMEOFINFO::GDNI_RELATIVEFRIENDLY,
      /*       0           0            1            1    */  DISPLAYNAMEOFINFO::GDNI_RELATIVEFRIENDLY,
      /*       0           1            0            0    */  DISPLAYNAMEOFINFO::GDNI_ABSOLUTEPARSING,
      /*       0           1            0            1    */  DISPLAYNAMEOFINFO::GDNI_RELATIVEPARSING,
      /*       0           1            1            0    */  DISPLAYNAMEOFINFO::GDNI_ABSOLUTEFRIENDLY,
      /*       0           1            1            1    */  DISPLAYNAMEOFINFO::GDNI_RELATIVEFRIENDLY,
      /*       1           0            0            0    */  DISPLAYNAMEOFINFO::GDNI_RELATIVEFRIENDLY,
      /*       1           0            0            1    */  DISPLAYNAMEOFINFO::GDNI_RELATIVEFRIENDLY,
      /*       1           0            1            0    */  DISPLAYNAMEOFINFO::GDNI_RELATIVEFRIENDLY,
      /*       1           0            1            1    */  DISPLAYNAMEOFINFO::GDNI_RELATIVEFRIENDLY,
      /*       1           1            0            0    */  DISPLAYNAMEOFINFO::GDNI_ABSOLUTEPARSING,
      /*       1           1            0            1    */  DISPLAYNAMEOFINFO::GDNI_RELATIVEPARSING,
      /*       1           1            1            0    */  DISPLAYNAMEOFINFO::GDNI_ABSOLUTEFRIENDLY,
      /*       1           1            1            1    */  DISPLAYNAMEOFINFO::GDNI_RELATIVEFRIENDLY
    };

    DWORD index = 0;

    if (uFlags & SHGDN_INFOLDER)
    {
      index |= DISPLAYNAMEOFINFO::GDNM_INFOLDER;
    }

    if (uFlags & SHGDN_FORPARSING)
    {
      index |= DISPLAYNAMEOFINFO::GDNM_FORPARSING;
    }

    if (uFlags & SHGDN_FORADDRESSBAR)
    {
      index |= DISPLAYNAMEOFINFO::GDNM_FORADDRESSBAR;
    }

    if (uFlags & SHGDN_FOREDITING)
    {
      index |= DISPLAYNAMEOFINFO::GDNM_FOREDITING;
    }

    if (ChildIdFromPidl(pidl) == kNewFileSignature.Id)
    {
      pidl = _newFilePidl;
    }

    CHECK_HR((this->*_DisplayNameOfInfo[_indices[index]]._GetDisplayNameOf)(pidl, uFlags, &psrName->pOleStr));
    psrName->uType = STRRET_WSTR;

    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::GetDisplayNameOf");
    return E_FAIL;
  }
}

std::wstring _SHGDNFToString(SHGDNF uFlags)
{
  Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_SHGDNFToString(SHGDNF uFlags)");

  std::wstring r;

  if (uFlags & SHGDN_INFOLDER)
  {
    r += L":INFOLDER:";
  }

  if (uFlags & SHGDN_FORPARSING)
  {
    r += L":FORPARSING:";
  }

  if (uFlags & SHGDN_FORADDRESSBAR)
  {
    r += L":FORADDRESSBAR:";
  }

  if (uFlags & SHGDN_FOREDITING)
  {
    r += L":FOREDITING:";
  }

  return r;
}

void _LogName(PCWCHAR title, PCWCHAR returnedName, bool isError, SHGDNF uFlags)
{
  Log::WriteOutput(LogType::Debug, L"_LogName(PCWCHAR title, PCWCHAR returnedName, bool isError, SHGDNF uFlags)");

  if (isError)
  {
    Log::WriteOutput(LogType::Error, L"GetDisplayNameOf %s %s", title, _SHGDNFToString(uFlags));
  }
  else
  {
    Log::WriteOutput(LogType::Debug, L"GetDisplayNameOf %s %s %s", title, _SHGDNFToString(uFlags).c_str(), returnedName);
  }
}

HRESULT CGDriveShlExt::_GetDisplayNameOfDisplayName(PCUITEMID_CHILD pidl, SHGDNF uFlags, __deref_out PWSTR *ppszName)
{
  HRESULT hr = S_OK;
  Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_GetDisplayNameOfDisplayName(PCUITEMID_CHILD pidl, SHGDNF uFlags, __deref_out PWSTR *ppszName)");

  CDriveItem driveItem;

  CHECK_HR(_GetDataFromIDList(pidl, false, false, driveItem));
  CHECK_HR(_CopyWStringToPWSTR(driveItem.ItemName(), ppszName));
  _LogName(driveItem.ItemName().c_str(), *ppszName, !SUCCEEDED(hr), uFlags);

  return hr;
}

HRESULT CGDriveShlExt::_GetDisplayNameOfDisplayPath(PCUITEMID_CHILD pidl, SHGDNF uFlags, __deref_out PWSTR *ppszPath)
{
  HRESULT hr = S_OK;
  Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_GetDisplayNameOfDisplayPath(PCUITEMID_CHILD pidl, SHGDNF uFlags, __deref_out PWSTR *ppszPath)");

  CDriveItem driveItem;
  CHECK_HR(_GetDataFromIDList(pidl, false, false, driveItem));

  std::wstring rootName;
  CHECK_HR(_GetNameFromIDList(_spidl, SIGDN_DESKTOPABSOLUTEEDITING, rootName));
  CHECK_HR(_CopyWStringToPWSTR(driveItem.ItemFolderPathDisplay(rootName, true), ppszPath));
  _LogName(driveItem.ItemName().c_str(), *ppszPath, !SUCCEEDED(hr), uFlags);

  return hr;
}

HRESULT CGDriveShlExt::_GetDisplayNameOfParsingName(PCUITEMID_CHILD pidl, SHGDNF uFlags, __deref_out PWSTR *ppszName)
{
  HRESULT hr = S_OK;
  Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_GetDisplayNameOfParsingName(PCUITEMID_CHILD pidl, SHGDNF uFlags, __deref_out PWSTR *ppszName)");

  CDriveItem driveItem;
  CHECK_HR(_GetDataFromIDList(pidl, false, false, driveItem));
  CHECK_HR(_CopyWStringToPWSTR(driveItem.ItemName(), ppszName));
  _LogName(driveItem.ItemName().c_str(), *ppszName, !SUCCEEDED(hr), uFlags);

  return hr;
}

HRESULT CGDriveShlExt::_GetDisplayNameOfParsingPath(PCUITEMID_CHILD pidl, SHGDNF uFlags, __deref_out PWSTR *ppszPath)
{
  HRESULT hr = S_OK;
  Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_GetDisplayNameOfParsingPath(PCUITEMID_CHILD pidl, SHGDNF uFlags, __deref_out PWSTR *ppszPath)");

  std::wstring id = ChildIdFromPidl(pidl);
  if (_fileManager.IsRootId(id))
  {
    // a bit of a hack, if its the root, we pull the location from the FileManger's log properties
    if (_fileManager.GetLog() != NULL)
    {
      hr = _CopyWStringToPWSTR(_fileManager.GetLog()->LocalGoogleDriveData, ppszPath);
    }
    else
    {
      hr = _CopyWStringToPWSTR(L"C:\\", ppszPath);
    }
    _LogName(_fileManager.RootId.c_str(), *ppszPath, !SUCCEEDED(hr), uFlags);
  }
  else
  {
    CDriveItem driveItem;

    CHECK_HR(_GetDataFromID(id, false, false, driveItem));
    std::wstring rootName;
    CHECK_HR(_CopyWStringToPWSTR(driveItem.ItemPathDisplay(), ppszPath));
    _LogName(driveItem.ItemName().c_str(), *ppszPath, !SUCCEEDED(hr), uFlags);
  }

  return hr;
}


STDMETHODIMP CGDriveShlExt::GetProperty(PCUITEMID_CHILD pidl, REFPROPERTYKEY key, __out PROPVARIANT *ppropvar)
{
  try
  {
    Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::GetProperty(PCUITEMID_CHILD pidl, REFPROPERTYKEY key=%d, __out PROPVARIANT *ppropvar)", key.pid);

    return PropertyHelper::GetProperty(this, pidl, key, ppropvar);
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::GetProperty");

    return E_FAIL;
  }
}

STDMETHODIMP CGDriveShlExt::SetProperty(PCUITEMID_CHILD /*pidl*/, REFPROPERTYKEY /*key*/, REFPROPVARIANT /*propvar*/)
{
  try
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::SetProperty E_NOTIMPL");

    HRESULT hr = E_NOTIMPL;

    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::SetProperty");

    return E_FAIL;
  }
}

UINT CGDriveShlExt::GetLastViewUIMsg()
{
  return _lastViewUIMsg;
}
void CGDriveShlExt::SetLastViewUIMsg(UINT value)
{
  _lastViewUIMsg = value;

  if (_lastViewUIMsg == 33) // SFVM_GETCOMMANDDIR
  {
    for (std::list<CComObject<ContextMenuCBHandler>*>::iterator it = _contextMenuCBHandlers.begin(); it != _contextMenuCBHandlers.end(); it++)
    {
      (*it)->DownloadFiles();
    }
  }
}

void CGDriveShlExt::ReleaseContextMenuCBHandler(ContextMenuCBHandler* contextMenuCBHandler)
{
  for (std::list<CComObject<ContextMenuCBHandler>*>::iterator it = _contextMenuCBHandlers.begin(); it != _contextMenuCBHandlers.end(); it++)
  {
    if ((*it) == contextMenuCBHandler)
    {
      (*it)->Release();
      _contextMenuCBHandlers.erase(it);
      break;
    }
  }
}

void CGDriveShlExt::SetDialogType(HWND hwnd)
{
  if (hwnd != NULL)
  {
    WCHAR title[200];
    if(GetWindowText(hwnd, title, 200))
    {
      std::wstring titleAsStdString = title;

      const std::size_t idxOpen = titleAsStdString.find(L"Open"); // case sensitive
      if (idxOpen != std::string::npos)
      {
        _hostType = HOSTINFO::Open;
      }

      const std::size_t idxSave = titleAsStdString.find(L"Save"); // case sensitive

      if (idxSave != std::string::npos)
      {
        _hostType = HOSTINFO::SaveAs;
      }

      if (_hostType == HOSTINFO::Unknown)
      {
        _hostType = HOSTINFO::Explorer;
        Log::WriteOutput(LogType::Warning, L"_DetermineDialogType: Unhandled Window Title: %s", title);
      }
    }
    else
    {
      Log::WriteOutput(LogType::Error, L"_GetDialogTitle failed to get type");
    }
  }
}

const std::wstring& CGDriveShlExt::Id()
{
  Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::Id()");

  return _id;
}

bool CGDriveShlExt::_RenameFile(const PCUITEMID_CHILD pidl, const std::wstring& name)
{
  try
  {
    Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::_RenameFile(HWND hwnd, PCUITEMID_CHILD pidl, const std::wstring& name)");

    FileInfo* fileInfo = NULL;

    HRESULT hr = _GetDataFromIDList(pidl, false, false, &fileInfo);

    if (!SUCCEEDED(hr))
    {
      Log::WriteOutput(LogType::Error, L"Cannot rename item - Could not find item from pidl");

      return false;
    }

    if (name.length() > MAX_PATH)
    {
      Log::WriteOutput(LogType::Error, L"Cannot rename item - Name is longer than MAX_PATH");

      return false;
    }

    if (!_fileManager.RenameFile(fileInfo, name))
    {
      std::wstring message = L"Cannot rename item - " + _fileManager.ErrorMessage();

      Log::WriteOutput(LogType::Error, message.c_str());

      return false;
    }

    _didUpdate = true;

    CIdList pidlPath;

    hr = CIdList::Combine(_spidl, pidl, pidlPath);

    if (!SUCCEEDED(hr))
    {
      Log::WriteOutput(LogType::Error, L"CombineIDLists returned hr=%d", hr);
    }
    else
    {
      if (fileInfo->IsFolder())
      {
        SHChangeNotify(SHCNE_RENAMEFOLDER, SHCNF_IDLIST | SHCNF_FLUSH, pidlPath, pidlPath);
      }
      else
      {
        SHChangeNotify(SHCNE_RENAMEITEM, SHCNF_IDLIST | SHCNF_FLUSH, pidlPath, pidlPath);
      }
    }

    SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_IDLIST | SHCNF_FLUSH, _spidl, NULL);

    return true;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::_RenameFile");

    return false;
  }
}

HRESULT CGDriveShlExt::_PasteFiles(bool isMove, const std::vector<PCIDLIST_RELATIVE>& ids, PCIDLIST_ABSOLUTE src, PCIDLIST_ABSOLUTE dst)
{
  HRESULT hr = S_OK;

  std::vector<std::wstring> sourceIds(ids.size());
  std::vector<std::wstring> movedIds;
  CDriveItem dstItem;

  std::wstring action = L"Copy";

  if (isMove)
  {
    action = L"Move";
  }

  hr = GetDriveItemFromIDList(dst, 0, 0, dstItem);

  if (SUCCEEDED(hr) && dstItem.IsFolder())
  {
    for (size_t i = 0; SUCCEEDED(hr) && i < ids.size(); i++)
    {
      sourceIds[i] = ChildIdFromPidl(ids[i]);
    }

    if (SUCCEEDED(hr))
    {
      bool isGood = false;

      if (isMove)
      {
        isGood = _fileManager.MoveFiles(sourceIds, dstItem.Id(), &movedIds);
      }
      else
      {
        isGood = _fileManager.CopyFiles(sourceIds, dstItem.Id(), &movedIds);
      }

      if (!isGood)
      {
        std::wstring message = L"CGDriveShlExt::_PasteFiles() Could not " + action + L" files - ";


        if (_fileManager.HasError())
        {
          message += _fileManager.ErrorMessage();
        }
        else
        {
          message += L"Unknown error";
        }

        Log::WriteOutput(LogType::Error, message.c_str());

        hr = E_FAIL;
      }
      else
      {
        hr = S_OK;
      }

      if (isMove)
      {
        for (size_t i = 0; SUCCEEDED(hr) && i < ids.size(); i++)
        {
          CIdList itemPidl;
          CDriveItem item;

          hr = CIdList::Combine(src, ids[i], itemPidl);

          if (SUCCEEDED(hr))
          {
            hr = GetDriveItemFromIDList(ids[i], false, false, item);

            if (SUCCEEDED(hr))
            {
              if (item.IsFile())
              {
                SHChangeNotify(SHCNE_DELETE, SHCNF_IDLIST | SHCNF_FLUSH, itemPidl, NULL);
              }
              else
              {
                SHChangeNotify(SHCNE_RMDIR, SHCNF_IDLIST | SHCNF_FLUSH, itemPidl, NULL);
              }
            }
          }
        }
      }

      if (isMove)
      {
        SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_IDLIST | SHCNF_FLUSH, src, NULL);
      }
      SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_IDLIST | SHCNF_FLUSH, dst, NULL);
    }
  }

  if (!SUCCEEDED(hr))
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::_PasteFiles() %s failed with unknown error", action.c_str());
  }

  return hr;
}
