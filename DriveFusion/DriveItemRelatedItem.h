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
#include "GDriveShlExt.h"
#include <map>

#include "FusionGDShell_i.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;

class ATL_NO_VTABLE CDriveItemRelatedItem :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<CDriveItemRelatedItem, &CLSID_DriveItemRelatedItem>,
  public IIdentityName,
  public ITransferMediumItem,
  public IPreviewItem,
  public ICurrentItem,
  public IDisplayItem,
  public IViewStateIdentityItem,
  public ITransferDestination,
  public ITransferSource,
  public IShellItemResources
{
public:
  CDriveItemRelatedItem()
  {
    Log::WriteOutput(LogType::Debug, L"CDriveItemRelatedItem::CDriveItemRelatedItem()");
    _gDriveShlExt = NULL;
  }

  BEGIN_COM_MAP(CDriveItemRelatedItem)
    COM_INTERFACE_ENTRY2(IRelatedItem, IIdentityName)
    COM_INTERFACE_ENTRY(IIdentityName)
    COM_INTERFACE_ENTRY(ITransferMediumItem)
    COM_INTERFACE_ENTRY(IPreviewItem)
    COM_INTERFACE_ENTRY(ICurrentItem)
    COM_INTERFACE_ENTRY(IDisplayItem)
    COM_INTERFACE_ENTRY(IViewStateIdentityItem)
    COM_INTERFACE_ENTRY(ITransferDestination)
    COM_INTERFACE_ENTRY(ITransferSource)
    COM_INTERFACE_ENTRY(IShellItemResources)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  static HRESULT CreateInstanceReturnInterfaceTo(CGDriveShlExt* gDriveShlExt, REFIID riid, __deref_out void **ppv);

  HRESULT FinalConstruct()
  {
    return S_OK;
  }

  void FinalRelease();

  // IRelatedItem
  IFACEMETHODIMP GetItem(__RPC__deref_out_opt IShellItem **ppsi);
  IFACEMETHODIMP GetItemIDList(__RPC__deref_out_opt PIDLIST_ABSOLUTE *ppidl);

  // ITransferDestination and ITransferSource
  IFACEMETHODIMP Advise(__in  ITransferAdviseSink *psink, __out  DWORD *pdwCookie) ;
  IFACEMETHODIMP Unadvise(DWORD dwCookie);
  IFACEMETHODIMP CreateItem(__in  LPCWSTR pszName, __in  DWORD dwAttributes, __in  ULONGLONG ullSize, __in  TRANSFER_SOURCE_FLAGS flags, __in  REFIID riidItem, __deref_out  void **ppvItem, __in  REFIID riidResources, __deref_out  void **ppvResources);
  IFACEMETHODIMP SetProperties(__in  IPropertyChangeArray *pproparray);
  IFACEMETHODIMP OpenItem(__in  IShellItem *psi, __in  TRANSFER_SOURCE_FLAGS flags, __in  REFIID riid, __deref_out  void **ppv);
  IFACEMETHODIMP MoveItem(__in  IShellItem *psi, __in  IShellItem *psiParentDst, __in  LPCWSTR pszNameDst, __in TRANSFER_SOURCE_FLAGS flags, __deref_out  IShellItem **ppsiNew);
  IFACEMETHODIMP RecycleItem(__in  IShellItem *psiSource, __in  IShellItem *psiParentDest, __in  TRANSFER_SOURCE_FLAGS flags, __deref_out  IShellItem **ppsiNewDest);
  IFACEMETHODIMP RemoveItem(__in  IShellItem *psiSource, __in  TRANSFER_SOURCE_FLAGS flags);
  IFACEMETHODIMP RenameItem(__in  IShellItem *psiSource, __in  LPCWSTR pszNewName, __in  TRANSFER_SOURCE_FLAGS flags, __deref_out  IShellItem **ppsiNewDest);
  IFACEMETHODIMP LinkItem(__in  IShellItem *psiSource, __in  IShellItem *psiParentDest, __in_opt  LPCWSTR pszNewName, __in  TRANSFER_SOURCE_FLAGS flags, __deref_out  IShellItem **ppsiNewDest);
  IFACEMETHODIMP ApplyPropertiesToItem(__in  IShellItem *psiSource, __deref_out  IShellItem **ppsiNew);
  IFACEMETHODIMP GetDefaultDestinationName(__in  IShellItem *psiSource, __in  IShellItem *psiParentDest, __deref_out  LPWSTR *ppszDestinationName);
  IFACEMETHODIMP EnterFolder(__in  IShellItem *psiChildFolderDest);
  IFACEMETHODIMP LeaveFolder(__in  IShellItem *psiChildFolderDest);

  // IShellItemResources
  IFACEMETHODIMP GetAttributes(__RPC__out DWORD *pdwAttributes);
  IFACEMETHODIMP GetSize(__RPC__out ULONGLONG *pullSize);
  IFACEMETHODIMP GetTimes(__RPC__out FILETIME *pftCreation, __RPC__out FILETIME *pftWrite, __RPC__out FILETIME *pftAccess);
  IFACEMETHODIMP SetTimes(__RPC__in_opt const FILETIME *pftCreation, __RPC__in_opt const FILETIME *pftWrite, __RPC__in_opt const FILETIME *pftAccess);
  IFACEMETHODIMP GetResourceDescription(__RPC__in const SHELL_ITEM_RESOURCE *pcsir, __RPC__deref_out_opt_string LPWSTR *ppszDescription);
  IFACEMETHODIMP EnumResources(__RPC__deref_out_opt IEnumResources **ppenumr);
  IFACEMETHODIMP SupportsResource(__RPC__in const SHELL_ITEM_RESOURCE *pcsir);
  IFACEMETHODIMP OpenResource(__RPC__in const SHELL_ITEM_RESOURCE *pcsir, __RPC__in REFIID riid, __RPC__deref_out_opt void **ppv);
  IFACEMETHODIMP CreateResource(__RPC__in const SHELL_ITEM_RESOURCE *pcsir, __RPC__in REFIID riid, __RPC__deref_out_opt void **ppv);
  IFACEMETHODIMP MarkForDelete(void);

private:
  HRESULT _Initialize(CGDriveShlExt* gDriveShlExt);

private:
  CGDriveShlExt* _gDriveShlExt;

  typedef std::map<DWORD, ITransferAdviseSink*> AdviseSinkMap;
  AdviseSinkMap _sinkMap;
  static DWORD _nextCookie;
};

