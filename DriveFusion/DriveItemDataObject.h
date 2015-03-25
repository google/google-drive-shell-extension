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
#include <list>

#include "FusionGDShell_i.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;

class ATL_NO_VTABLE CDriveItemDataObject :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<CDriveItemDataObject, &CLSID_DriveItemDataObject>,
  public IDataObject,
  public IDropTarget,
  public IFileOperationProgressSink
{
public:
  CDriveItemDataObject() : _CBFormatList()
  {
    Log::WriteOutput(LogType::Debug, L"CDriveItemDataObject::CDriveItemDataObject()");
    _gDriveShlExt = NULL;
  }

  DECLARE_REGISTRY_RESOURCEID(IDR_DRIVEITEMDATAOBJECT)

  BEGIN_COM_MAP(CDriveItemDataObject)
    COM_INTERFACE_ENTRY(IDataObject)
    COM_INTERFACE_ENTRY(IDropTarget)
    COM_INTERFACE_ENTRY(IFileOperationProgressSink)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  static HRESULT CreateInstanceReturnInterfaceTo(CGDriveShlExt* gDriveShlExt, REFIID riid, __deref_out void **ppv);

  HRESULT FinalConstruct()
  {
    return S_OK;
  }

  void FinalRelease();

  // IDataObject
  IFACEMETHODIMP GetData(FORMATETC *pformatetc, STGMEDIUM *pmedium);
  IFACEMETHODIMP GetDataHere(FORMATETC *pformatetc, STGMEDIUM *pmedium);
  IFACEMETHODIMP QueryGetData(__RPC__in_opt FORMATETC *pformatetc);
  IFACEMETHODIMP GetCanonicalFormatEtc(__RPC__in_opt FORMATETC *pformatectIn, __RPC__out FORMATETC *pformatetcOut);
  IFACEMETHODIMP SetData(FORMATETC *pformatetc, STGMEDIUM *pmedium, BOOL fRelease);
  IFACEMETHODIMP EnumFormatEtc(DWORD dwDirection, __RPC__deref_out_opt IEnumFORMATETC **ppenumFormatEtc);
  IFACEMETHODIMP DAdvise(__RPC__in FORMATETC *pformatetc, DWORD advf, __RPC__in_opt IAdviseSink *pAdvSink, __RPC__out DWORD *pdwConnection);
  IFACEMETHODIMP DUnadvise(DWORD dwConnection);
  IFACEMETHODIMP EnumDAdvise(__RPC__deref_out_opt IEnumSTATDATA **ppenumAdvise);

  // IDropTarget
  IFACEMETHODIMP DragEnter(__RPC__in_opt IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, __RPC__inout DWORD *pdwEffect);
  IFACEMETHODIMP DragOver(DWORD grfKeyState, POINTL pt, __RPC__inout DWORD *pdwEffect);
  IFACEMETHODIMP DragLeave(void);
  IFACEMETHODIMP Drop(__RPC__in_opt IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, __RPC__inout DWORD *pdwEffect);

  // IFileOperationProgressSink
  IFACEMETHODIMP StartOperations(void);
  IFACEMETHODIMP FinishOperations( HRESULT hrResult);
  IFACEMETHODIMP PreRenameItem(DWORD dwFlags, __RPC__in_opt IShellItem *psiItem, __RPC__in_opt_string LPCWSTR pszNewName);
  IFACEMETHODIMP PostRenameItem( DWORD dwFlags, __RPC__in_opt IShellItem *psiItem,  __RPC__in_string LPCWSTR pszNewName, HRESULT hrRename, __RPC__in_opt IShellItem *psiNewlyCreated);
  IFACEMETHODIMP PreMoveItem( DWORD dwFlags, __RPC__in_opt IShellItem *psiItem, __RPC__in_opt IShellItem *psiDestinationFolder, __RPC__in_opt_string LPCWSTR pszNewName);
  IFACEMETHODIMP PostMoveItem( DWORD dwFlags, __RPC__in_opt IShellItem *psiItem, __RPC__in_opt IShellItem *psiDestinationFolder, __RPC__in_opt_string LPCWSTR pszNewName, HRESULT hrMove, __RPC__in_opt IShellItem *psiNewlyCreated);
  IFACEMETHODIMP PreCopyItem(DWORD dwFlags, __RPC__in_opt IShellItem *psiItem, __RPC__in_opt IShellItem *psiDestinationFolder, __RPC__in_opt_string LPCWSTR pszNewName);
  IFACEMETHODIMP PostCopyItem(DWORD dwFlags, __RPC__in_opt IShellItem *psiItem, __RPC__in_opt IShellItem *psiDestinationFolder, __RPC__in_opt_string LPCWSTR pszNewName, HRESULT hrCopy, __RPC__in_opt IShellItem *psiNewlyCreated);
  IFACEMETHODIMP PreDeleteItem(DWORD dwFlags, __RPC__in_opt IShellItem *psiItem);
  IFACEMETHODIMP PostDeleteItem(DWORD dwFlags, __RPC__in_opt IShellItem *psiItem, HRESULT hrDelete, __RPC__in_opt IShellItem *psiNewlyCreated);
  IFACEMETHODIMP PreNewItem(DWORD dwFlags, __RPC__in_opt IShellItem *psiDestinationFolder, __RPC__in_opt_string LPCWSTR pszNewName);
  IFACEMETHODIMP PostNewItem(DWORD dwFlags, __RPC__in_opt IShellItem *psiDestinationFolder, __RPC__in_opt_string LPCWSTR pszNewName, __RPC__in_opt_string LPCWSTR pszTemplateName, DWORD dwFileAttributes, HRESULT hrNew, __RPC__in_opt IShellItem *psiNewItem);
  IFACEMETHODIMP UpdateProgress(UINT iWorkTotal, UINT iWorkSoFar);
  IFACEMETHODIMP ResetTimer(void);
  IFACEMETHODIMP PauseTimer(void);
  IFACEMETHODIMP ResumeTimer(void);

private:
  HRESULT _Initialize(CGDriveShlExt* gDriveShlExt);

private:
  CGDriveShlExt* _gDriveShlExt;
  DWORD _dragEnterPdwEffect;

  // Notes avaible at
  // http://www.catch22.net/tuts/implementing-idataobject
  // http://msdn.microsoft.com/en-us/library/ms997502.aspx

  struct CBFormat
  {
    FORMATETC Format;
    STGMEDIUM StgMedium;
  };
  typedef std::list<CBFormat> CBFormatList;
  CBFormatList _CBFormatList;

  HRESULT _LookupFormatEtc(__in FORMATETC *pFormatEtc, __out CBFormatList::iterator& it);
  HRESULT _AddFormat(__in FORMATETC *pFormatEtc, __in STGMEDIUM *pMedium);
  HRESULT _AddRefStgMedium(__in STGMEDIUM *pmediumIn, __out STGMEDIUM *pmediumOut, __in BOOL fCopyIn);
  HRESULT _CreateRelatedFormats(__in FORMATETC *pFormatEtc, __in STGMEDIUM *pmedium);
  static HGLOBAL _DupGlobalMem(HGLOBAL hMem);
  static IUnknown* _GetCanonicalIUnknown(IUnknown* punk);

  static std::wstring _AppendFormatName(const PWCHAR logMessage, CLIPFORMAT formatId);
  static std::wstring _GetStandardFormatName(CLIPFORMAT formatId);
  static bool _ShouldCreateAdditionalFormatsForShell(CLIPFORMAT formatId);
  static bool _IsShellType(CLIPFORMAT formatId);
  static DWORD _GetPdwEffectFromState(DWORD grfKeyState);

  static HRESULT _Extract_HGLOBAL(__in const STGMEDIUM& medium, __in size_t storageSize, __out void* object);
  static HRESULT _Extract_HGLOBAL(__in const STGMEDIUM& medium, __in size_t offset, __in size_t length, __out void* object);
  static HRESULT _Create_CF_HDROP_FromList(__in std::vector<std::wstring>& fileList, __out FORMATETC& format, __out STGMEDIUM& medium);
  static HRESULT _Create_CFSTR_PREFERREDDROPEFFECT(__in DWORD dropEffect, __out FORMATETC& format, __out STGMEDIUM& medium);
  static HRESULT _Create_CFSTR_PERFORMEDDROPEFFECT(__in DWORD dropEffect, __out FORMATETC& format, __out STGMEDIUM& medium);
  static HRESULT _Create_CFSTR_FILEDESCRIPTOR(__in std::vector<WIN32_FIND_DATA>& win32FindData, __out FORMATETC& format, __out STGMEDIUM& medium);
  static HRESULT _Create_CFSTR_FILECONTENTS(__in size_t lindex, IStream* stream, __out FORMATETC& format, __out STGMEDIUM& medium);
  static HRESULT _Create_GOOGLEDRIVESOURCEID(__out FORMATETC& format, __out STGMEDIUM& medium);

  static void _BuildFormat_CF_HDROP(FORMATETC& format);
  static void _BuildFormat_CFSTR_PREFERREDDROPEFFECT(FORMATETC& format);
  static void _BuildFormat_CFSTR_PERFORMEDDROPEFFECT(FORMATETC& format);
  static void _BuildFormat_CFSTR_FILEDESCRIPTOR(FORMATETC& format);
  static void _BuildFormat_CFSTR_FILECONTENTS(size_t lindex, FORMATETC& format);
  static void _BuildFormat_GOOGLEDRIVESOURCEID(FORMATETC& format);

  static HRESULT _Create_HGLOBALSTGMEDIUMFromBuffer(__in void* buf, __in size_t length, __out STGMEDIUM& medium);
  static HRESULT _Create_ISTREAMSTGMEDIUM(__in IStream* stream, __out STGMEDIUM& medium);

  HRESULT _Convert_CFSTR_SHELLIDLIST_to_pidlList(__in STGMEDIUM *pmediumIn, __out std::vector<PCIDLIST_RELATIVE>& pidlList);
};

