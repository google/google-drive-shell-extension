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
#include "DriveItem.h"
#include "FusionGDShell_i.h"
#include "DriveItemPropertyHandler.h"
#include "ContextMenuCBHandler.h"

#include <list>

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;

class ATL_NO_VTABLE CGDriveShlExt :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<CGDriveShlExt, &CLSID_GDriveShlExt>,
  public IObjectWithSiteImpl<CGDriveShlExt>,
  public CItemIDFactory<DriveItemSignature, DriveItemSignature_MAGIC>,
  public IPersistFolder2,
  public IShellFolder2,
  public IDriveItemPropertyHandler
{
public:
  CGDriveShlExt()
  {
    Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::CGDriveShlExt()");

    _fileManager.WakeService();
  }

  // DECLARE_REGISTRY_RESOURCEID(IDR_GDRIVESHLEXT)
  static HRESULT WINAPI UpdateRegistry(_In_ BOOL bRegister) throw();

  BEGIN_COM_MAP(CGDriveShlExt)
    COM_INTERFACE_ENTRY2(IPersist, IPersistFolder2)
    COM_INTERFACE_ENTRY2(IPersistFolder, IPersistFolder2)
    COM_INTERFACE_ENTRY(IPersistFolder2)
    COM_INTERFACE_ENTRY2(IShellFolder, IShellFolder2)
    COM_INTERFACE_ENTRY(IShellFolder2)
    COM_INTERFACE_ENTRY(IDriveItemPropertyHandler)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  HRESULT FinalConstruct()
  {
    return S_OK;
  }

  void FinalRelease()
  {
    if (_fileManager.IsRootId(_id))
    {
      Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::FinalRelease() root");
    }
    else
    {
      Log::WriteOutput(LogType::Debug, L"CGDriveShlExt::FinalRelease() ", _id.c_str());
    }

    _spidl.Reset();

    for (std::list<CComObject<ContextMenuCBHandler>*>::iterator it = _contextMenuCBHandlers.begin(); it != _contextMenuCBHandlers.end(); it++)
    {
      (*it)->Release();
    }

    _contextMenuCBHandlers.clear();
  }

  static HRESULT CreateInstance(PCIDLIST_ABSOLUTE pidl, PCUITEMID_CHILD pidlChild, CComObject<CGDriveShlExt>** drive);
  static HRESULT CreateInstanceReturnInterfaceTo(PCIDLIST_ABSOLUTE pidl, PCUITEMID_CHILD pidlChild, REFIID riid, __deref_out void **ppv);

  static FileManager* FileManager();
  static HRESULT GetDriveItemFromIDList(PCUIDLIST_RELATIVE pidl, bool updateCache, bool getChildren, CDriveItem& driveItem);
  static HRESULT GetDriveItemFromID(const std::wstring& id, bool updateCache, bool getChildren, CDriveItem& driveItem);
  static HRESULT DoDownload(FileInfo* fi);
  static HRESULT DoMoveFiles(const std::vector<PCIDLIST_RELATIVE>& ids, PCIDLIST_ABSOLUTE src, PCIDLIST_ABSOLUTE dst);
  static HRESULT DoCopyFiles(const std::vector<PCIDLIST_RELATIVE>& ids, PCIDLIST_ABSOLUTE src, PCIDLIST_ABSOLUTE dst);

  // IPersist
  IFACEMETHODIMP GetClassID(__out CLSID *pclsid);

  // IPersistFolder
  IFACEMETHODIMP Initialize(PCIDLIST_ABSOLUTE pidl);

  // IPersistFolder2
  IFACEMETHODIMP GetCurFolder(__out PIDLIST_ABSOLUTE *ppidl);

  // IShellFolder
  IFACEMETHODIMP ParseDisplayName(HWND hwnd, __in IBindCtx *pbc, __in PWSTR pszDisplayName, __inout ULONG *pchEaten, __deref_out PIDLIST_RELATIVE *ppidl, __inout ULONG *pdwAttributes);
  IFACEMETHODIMP EnumObjects(HWND hwnd, SHCONTF grfFlags, __deref_out IEnumIDList **ppenmIDList);
  IFACEMETHODIMP BindToObject(PCUIDLIST_RELATIVE pidl, __in IBindCtx *pbc, REFIID riid, __deref_out void **ppv);
  IFACEMETHODIMP BindToStorage(PCUIDLIST_RELATIVE pidl, __in IBindCtx *pbc, REFIID riid, __deref_out void **ppv);
  IFACEMETHODIMP CompareIDs(LPARAM lParam, PCUIDLIST_RELATIVE pidl1, PCUIDLIST_RELATIVE pidl2);
  IFACEMETHODIMP CreateViewObject(HWND hwndOwner, REFIID riid, __deref_out void **ppv);
  IFACEMETHODIMP GetAttributesOf(UINT cidl, __in_ecount_opt(cidl) PCUITEMID_CHILD_ARRAY rgpidl, __inout SFGAOF *rgfInOut);
  IFACEMETHODIMP GetUIObjectOf(HWND hwndOwner, UINT cidl, __in_ecount_opt(cidl) PCUITEMID_CHILD_ARRAY rgpidl, REFIID riid, __reserved UINT *rgfReserved, __deref_out void **ppv);
  IFACEMETHODIMP GetDisplayNameOf(PCUITEMID_CHILD pidl, SHGDNF uFlags, __out STRRET *psrName);
  IFACEMETHODIMP SetNameOf(HWND hwnd, PCUITEMID_CHILD pidl, LPCWSTR pszName, SHGDNF uFlags, __deref_out_opt PITEMID_CHILD *ppidlOut);

  // IShellFolder2
  IFACEMETHODIMP GetDefaultSearchGUID(__out GUID *pguid);
  IFACEMETHODIMP EnumSearches(__deref_out IEnumExtraSearch **ppenum);
  IFACEMETHODIMP GetDefaultColumn(DWORD dwRes, __out ULONG *plSort, __out ULONG *plDisplay);
  IFACEMETHODIMP GetDefaultColumnState(UINT iColumn, __out SHCOLSTATEF *pcsFlags);
  IFACEMETHODIMP GetDetailsEx(PCUITEMID_CHILD pidl, const PROPERTYKEY *pkey, __out VARIANT *pvar);
  IFACEMETHODIMP GetDetailsOf(__in_opt PCUITEMID_CHILD pidl, UINT iColumn, __out SHELLDETAILS *psd);
  IFACEMETHODIMP MapColumnToSCID(UINT iColumn, __out PROPERTYKEY *pkey);

  // IDriveItemPropertyHandler
  IFACEMETHODIMP GetProperty(PCUITEMID_CHILD pidl, REFPROPERTYKEY key, __out PROPVARIANT *ppropvar);
  IFACEMETHODIMP SetProperty(PCUITEMID_CHILD pidl, REFPROPERTYKEY key, REFPROPVARIANT propvar);

  UINT GetLastViewUIMsg();
  void SetLastViewUIMsg(UINT value);
  void ReleaseContextMenuCBHandler(ContextMenuCBHandler* contextMenuCBHandler);

  void SetDialogType(HWND hwnd);

  // CDriveShlExt
  const std::wstring& Id();

private:
  static const struct DISPLAYNAMEOFINFO
  {
    enum INDEX
    {
      GDNI_RELATIVEFRIENDLY = 0,
      GDNI_ABSOLUTEFRIENDLY,
      GDNI_RELATIVEPARSING,
      GDNI_ABSOLUTEPARSING
    };

    enum MASK
    {
      GDNM_INFOLDER = 0x00000001,
      GDNM_FORADDRESSBAR = 0x00000002,
      GDNM_FORPARSING = 0x00000004,
      GDNM_FOREDITING = 0x00000008
    };

    HRESULT (CGDriveShlExt::*_GetDisplayNameOf)(PCUITEMID_CHILD pidl, SHGDNF uFlags, __deref_out PWSTR *ppszPath);
  } _DisplayNameOfInfo[];

  static const struct MENUINFO
  {
    PCWSTR pszVerb;
    const char *pszVerbA;
    UINT uiCmdId;
    UINT uiHelpId;
  } _MenuInfo[];

  static const struct HOSTINFO
  {
    enum TYPE
    {
      Unknown,
      Explorer,
      Open,
      SaveAs,
    };
  };

  void _LogWhoAmI(PCIDLIST_RELATIVE pidl);
  void _LogRPC(REFIID riid, __deref_out void **ppv);
  static std::wstring _STGMToString(DWORD uFlags);
  static HRESULT _GetContextOptions(__in IBindCtx *pbc, __out BIND_OPTS& options);
  HRESULT _CreateContextMenu(HWND hwndOwner, UINT cidl, __in_ecount_opt(cidl) PCUITEMID_CHILD_ARRAY rgpidl, REFIID riid, __deref_out void **ppv);

  static HRESULT _CopyLPCWSTRToLPWSTR(LPCWSTR currentValue, LPWSTR* newValue);
  static HRESULT _CopyPROPVARIANTToWString(PROPVARIANT* ppropvar, std::wstring& value);
  static HRESULT _CopyWStringToLPWSTR(const std::wstring& currentValue, LPWSTR* newValue);
  static HRESULT _CopyWStringToLPOLESTR(const std::wstring& currentValue, LPOLESTR* newValue);
  static HRESULT _CopyWStringToPWSTR(const std::wstring& currentValue, PWSTR* newValue);
  static HRESULT _CopyWStringToWCHAR(const std::wstring& currentValue, WCHAR* newValue);

  static HRESULT _GetDataFromIDList(PCUIDLIST_RELATIVE pidl, bool updateCache, bool getChildren, FileInfo** fileInfo);
  static HRESULT _GetDataFromIDList(PCUIDLIST_RELATIVE pidl, bool updateCache, bool getChildren, CDriveItem& driveItem);
  static HRESULT _GetDataFromID(const std::wstring& id, bool updateCache, bool getChildren, FileInfo** fileInfo);
  static HRESULT _GetDataFromID(const std::wstring& id, bool updateCache, bool getChildren, CDriveItem& driveItem);

  static HRESULT _GetNameFromIDList(PCUIDLIST_RELATIVE pidl, SIGDN sigdn, std::wstring& name);
  static HRESULT _GetNameFromIDList(PCUIDLIST_RELATIVE pidl, SIGDN sigdn, PWSTR* ppszName);

  HRESULT _GetDetailsOf(__in IPropertyDescription *ppropdesc, __out SHELLDETAILS *psd);
  HRESULT _GetDefaultColumnState(__in IPropertyDescription *ppropdesc, __out SHCOLSTATEF *pcsFlags);
  HRESULT _EnumObjects(SHCONTF grfFlags, __inout std::vector<PITEMID_CHILD>& rgItems);
  HRESULT _CreateItemID(Fusion::GoogleDrive::FileInfo* fileInfo, CIdList* ppidl);
  HRESULT _GetAttributesOf(PCUITEMID_CHILD pidl, DWORD rgfIn, __out DWORD *prgfOut);
  HRESULT _GetAttributesOf(FileInfo* fileInfo, __out DWORD *prgfOut);

  HRESULT _CompareIDs(LPARAM lParam, PCUIDLIST_RELATIVE pidl1, PCUIDLIST_RELATIVE pidl2, int& result);
  static HRESULT _CompareDriveItem(LPARAM lParam, CDriveItem& pidl1, CDriveItem&  pidl2, int& result);
  static HRESULT _CompareForEmptyIDs(PCUIDLIST_RELATIVE pidl1, PCUIDLIST_RELATIVE pidl2, int& result);

  HRESULT _GetDisplayNameOfDisplayName(PCUITEMID_CHILD pidl, SHGDNF uFlags, __deref_out PWSTR *ppszName);
  HRESULT _GetDisplayNameOfDisplayPath(PCUITEMID_CHILD pidl, SHGDNF uFlags, __deref_out PWSTR *ppszPath);
  HRESULT _GetDisplayNameOfParsingName(PCUITEMID_CHILD pidl, SHGDNF uFlags, __deref_out PWSTR *ppszName);
  HRESULT _GetDisplayNameOfParsingPath(PCUITEMID_CHILD pidl, SHGDNF uFlags, __deref_out PWSTR *ppszPath);
  HRESULT _NextNameSegment(std::wstring& ppszInOut, std::wstring& ppszSegment, __deref_out_opt CIdList *ppidlOut, __inout ULONG *pdwAttributes);

  HRESULT _BindToSubfolder(PCUIDLIST_RELATIVE pidl, REFIID riid, __deref_out void **ppv) const;

public:
  bool _RenameFile(const PCUITEMID_CHILD pidl, const std::wstring& name);
  static HRESULT _PasteFiles(bool isMove, const std::vector<PCIDLIST_RELATIVE>& ids, PCIDLIST_ABSOLUTE src, PCIDLIST_ABSOLUTE dst);

private:
  CIdList _spidl;
  std::wstring _id;
  std::list<CComObject<ContextMenuCBHandler>*> _contextMenuCBHandlers;

  static bool _didUpdate;
  static HOSTINFO::TYPE _hostType;
  static DWORD _previousRGFIN;
  static UINT _lastViewUIMsg;
  static Fusion::GoogleDrive::FileManager _fileManager;
  static IDataObject* _contextMenuSelection;
  static bool _ignoreGetFilesError;
  static std::wstring _newFileName;
  static DWORD _newFileAttributes;
  static CIdList _newFilePidl;
};

OBJECT_ENTRY_AUTO(__uuidof(GDriveShlExt), CGDriveShlExt)
