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

#include "FusionGDShell_i.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;

class ShellFolderViewCBHandler :
  public CComObjectRootEx<CComSingleThreadModel>,
  public IShellFolderViewCB,
  public IFolderViewSettings
{
public:
  ShellFolderViewCBHandler()
  {
    Log::WriteOutput(LogType::Debug, L"GDriveCBHandler::GDriveCBHandler()");

    _gDriveShlExt = NULL;
  }

  BEGIN_COM_MAP(ShellFolderViewCBHandler)
    COM_INTERFACE_ENTRY(IShellFolderViewCB)
    COM_INTERFACE_ENTRY(IFolderViewSettings)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  static HRESULT CreateInstanceReturnInterfaceTo(CGDriveShlExt *gDriveShlExt, REFIID riid, __deref_out void **ppv);

  HRESULT FinalConstruct()
  {
    return S_OK;
  }

  void FinalRelease()
  {
    Log::WriteOutput(LogType::Debug, L"GDriveCBHandler::FinalRelease()");

    _spidl.Free();

    if (_gDriveShlExt != NULL)
    {
      _gDriveShlExt->Release();
      _gDriveShlExt = NULL;
    }
  }

  // IShellFolderViewCB
  IFACEMETHODIMP MessageSFVCB(UINT uiMsg, WPARAM wParam, LPARAM lParam);

  // IFolderViewSettings
  IFACEMETHODIMP GetColumnPropertyList(__in REFIID riid, __deref_out_opt void **ppv);
  IFACEMETHODIMP GetGroupByProperty(__out PROPERTYKEY *pkey, __out BOOL *pfGroupAscending);
  IFACEMETHODIMP GetViewMode(__out FOLDERLOGICALVIEWMODE *plvm);
  IFACEMETHODIMP GetIconSize(__out UINT *puIconSize);
  IFACEMETHODIMP GetFolderFlags(__out FOLDERFLAGS *pfolderMask, __out FOLDERFLAGS *pfolderFlags);
  IFACEMETHODIMP GetSortColumns(__out_ecount_part(cColumnsIn, *pcColumnsOut) SORTCOLUMN *rgSortColumns, UINT cColumnsIn, __out UINT *pcColumnsOut);
  IFACEMETHODIMP GetGroupSubsetCount(__out UINT *pcVisibleRows);

private:
  HRESULT _Initialize(CGDriveShlExt* gDriveShlExt);

  std::wstring _GetMsgString(UINT uiMsg, WPARAM wParam, LPARAM lParam);

private:
  CGDriveShlExt* _gDriveShlExt;
  CComHeapPtr<ITEMIDLIST_ABSOLUTE> _spidl;
};

