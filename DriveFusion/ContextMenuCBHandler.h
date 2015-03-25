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

#include "FusionGDShell_i.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;

class CGDriveShlExt;
class CDriveItem;

class ContextMenuCBHandler :
  public CComObjectRootEx<CComSingleThreadModel>,
  public IContextMenuCB
{
public:
  ContextMenuCBHandler()
  {
    Log::WriteOutput(LogType::Debug, L"GDriveCBHandler::GDriveCBHandler()");

    _gDriveShlExt = NULL;
  }

  BEGIN_COM_MAP(ContextMenuCBHandler)
    COM_INTERFACE_ENTRY(IContextMenuCB)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  static HRESULT CreateInstance(CGDriveShlExt *gDriveShlExt, __deref_out CComObject<ContextMenuCBHandler> **ppv);

  HRESULT ReturnInterfaceTo(REFIID riid, __deref_out void **ppv);

  HRESULT FinalConstruct()
  {
    return S_OK;
  }

  void FinalRelease();

  // IContextMenuCB
  IFACEMETHODIMP CallBack(__in_opt IShellFolder *psf, HWND hwndOwner, __in_opt IDataObject *pdtobj, UINT uiMsg, WPARAM wParam, LPARAM lParam);

  void DownloadFiles();

private:
  HRESULT _Initialize(CGDriveShlExt* gDriveShlExt);

  HRESULT _ConvertDataObjectToVectorOfDriveItems(__in IDataObject *pdo, std::vector<CDriveItem>* driveItemList) const;
  void _ReleaseContextMenuSelection();
  void _DownloadFilesIfReady();
  std::wstring _GetContextMenuCBString(UINT uiMsg, WPARAM wParam, LPARAM lParam);
  bool _PdoIsGoogleDoc(IDataObject& pdo) const;

private:
  CGDriveShlExt* _gDriveShlExt;
  IDataObject* _pdo;
  bool _downloadWhenReady;
};

