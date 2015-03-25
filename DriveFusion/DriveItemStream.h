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
#include "FileInfo.h"
#include "DriveItem.h"

using namespace Fusion::GoogleDrive;

#include "FusionGDShell_i.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;

class ATL_NO_VTABLE CDriveItemStream :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<CDriveItemStream, &CLSID_DriveItemStream>,
  public IStream,
  public IPersistStream,
  public IPersistStreamInit,
  public IPersistFile
{
public:
  CDriveItemStream() : _item()
  {
    Log::WriteOutput(LogType::Information, L"CDriveItemStream::CDriveItemStream()");

    _isFileDownloaded = false;
    _stgmMode = 0;
    _lockType = 0;
  }

  DECLARE_REGISTRY_RESOURCEID(IDR_DRIVEITEMSTREAM)

  BEGIN_COM_MAP(CDriveItemStream)
    COM_INTERFACE_ENTRY2(IUnknown, IStream)
    COM_INTERFACE_ENTRY(IStream)
    COM_INTERFACE_ENTRY2(IPersist, IPersistStream)
    COM_INTERFACE_ENTRY(IPersistStream)
    COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
    COM_INTERFACE_ENTRY(IPersistStreamInit)
    COM_INTERFACE_ENTRY2(IPersist, IPersistFile)
    COM_INTERFACE_ENTRY(IPersistFile)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  static HRESULT CreateInstanceReturnInterfaceTo(CDriveItem& item, __in const BIND_OPTS& options, REFIID riid, __deref_out void **ppv);

  HRESULT FinalConstruct()
  {
    return S_OK;
  }

  void FinalRelease();

  // IStream
  IFACEMETHODIMP Clone(__out __RPC__deref_out_opt IStream **ppstm);
  IFACEMETHODIMP Commit(__in DWORD grfCommitFlags);
  IFACEMETHODIMP CopyTo(__in IStream *pstm, __in ULARGE_INTEGER cb, __out_opt  ULARGE_INTEGER *pcbRead, __out_opt  ULARGE_INTEGER *pcbWritten);
  IFACEMETHODIMP LockRegion(__in ULARGE_INTEGER libOffset, __in ULARGE_INTEGER cb, __in DWORD dwLockType);
  IFACEMETHODIMP Read(__out void *pv, __in ULONG cb, __out ULONG *pcbRead);
  IFACEMETHODIMP Revert();
  IFACEMETHODIMP Seek(__in LARGE_INTEGER dlibMove, __in DWORD dwOrigin, __out_opt  ULARGE_INTEGER *plibNewPosition);
  IFACEMETHODIMP SetSize(__in ULARGE_INTEGER libNewSize);
  IFACEMETHODIMP Stat(__out __RPC__out STATSTG *pstatstg, __in DWORD grfStatFlag);
  IFACEMETHODIMP UnlockRegion(__in ULARGE_INTEGER libOffset, __in ULARGE_INTEGER cb, __in DWORD dwLockType);
  IFACEMETHODIMP Write(__in void const *pv, __in ULONG cb, __out ULONG *pcbWritten);

  // IPersist
  IFACEMETHODIMP GetClassID(__out CLSID *pclsid);

  // IPersistStream
  IFACEMETHODIMP IsDirty();
  IFACEMETHODIMP Load(__RPC__in_opt IStream *pStm);
  IFACEMETHODIMP Save(__RPC__in_opt IStream *pStm, BOOL fClearDirty);
  IFACEMETHODIMP GetSizeMax(__RPC__out ULARGE_INTEGER *pcbSize);

  // IPersistStreamInit
  IFACEMETHODIMP InitNew();

  // IPersistFile
  IFACEMETHODIMP Load(__RPC__in LPCOLESTR pszFileName, DWORD dwMode);
  IFACEMETHODIMP Save(__RPC__in_opt LPCOLESTR pszFileName, BOOL fRemember);
  IFACEMETHODIMP SaveCompleted(__RPC__in_opt LPCOLESTR pszFileName);
  IFACEMETHODIMP GetCurFile(__RPC__deref_out_opt LPOLESTR *ppszFileName);

private:
  HRESULT _Initialize(CDriveItem& item, DWORD grfMode);

  HRESULT _DownloadFile();
  HRESULT _Open();
  HRESULT _CloseWithError(HRESULT previousHR);
  HRESULT _Read(__out void *pv, __in ULONG cb, __out ULONG *pcbRead);
  HRESULT _Seek(__in LARGE_INTEGER dlibMove, __in DWORD dwOrigin, __out_opt  ULARGE_INTEGER *plibNewPosition);
  HRESULT _SetSize(__in ULARGE_INTEGER libNewSize);
  HRESULT _Write(__in void const *pv, __in ULONG cb, __out ULONG *pcbWritten);

  ULARGE_INTEGER _StatFileSize();
  BOOL _StatFileTimeModified(FILETIME& fi);
  BOOL _StatFileTimeCreated(FILETIME& fi);
  BOOL _StatFileTimeAccessed(FILETIME& fi);

private:
  CDriveItem _item;
  bool _isFileDownloaded;
  HANDLE _file;
  DWORD _stgmMode;
  DWORD _lockType;
  LARGE_INTEGER _seekPos;
};
