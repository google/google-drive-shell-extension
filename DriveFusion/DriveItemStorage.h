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
#include <fstream>

using namespace Fusion::GoogleDrive;

#include "FusionGDShell_i.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;
  COM_INTERFACE_ENTRY(IStorage)
  COM_INTERFACE_ENTRY2(IPersist, IPersistStorage)
  COM_INTERFACE_ENTRY(IOleItemContainer)
    public IStorage,
  public IPersistStorage,
  public IOleItemContainer,

class ATL_NO_VTABLE CDriveItemStream :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<CDriveItemStream, &CLSID_DriveItemStream>,
  public IStorage,
  public IPersist,
  public IPersistStorage,
  public IShellItem,
  public ITransferDestination,
  public ITransferSource,
  public IShellItem2
{
public:
  CDriveItemStream()
  {
    _fileInfo = NULL;
    _isFileDownloaded = false;
    _stgmMode = 0;
    _lockType = 0;
    _position = 0;
  }

DECLARE_REGISTRY_RESOURCEID(IDR_DRIVEITEMSTREAM)

BEGIN_COM_MAP(CDriveItemStream)
  COM_INTERFACE_ENTRY(IStream)
  COM_INTERFACE_ENTRY(IStorage)
  COM_INTERFACE_ENTRY(IPersist, IPersistStorage)
  COM_INTERFACE_ENTRY(IShellItem)
  COM_INTERFACE_ENTRY(ITransferDestination)
  COM_INTERFACE_ENTRY(ITransferSource)
  COM_INTERFACE_ENTRY(IShellItem2)
END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  static HRESULT CreateInstance(FileInfo* fileInfo, REFIID riid, __deref_out void **ppv);

  HRESULT FinalConstruct()
  {
    return S_OK;
  }

  void FinalRelease()
  {
    _Close(true);
  }

  // IStream
  IFACEMETHODIMP Clone(__out __RPC__deref_out_opt IStream **ppstm);
  IFACEMETHODIMP Commit(__in DWORD grfCommitFlags);
  IFACEMETHODIMP CopyTo(__in IStream *pstm, __in ULARGE_INTEGER cb, __out_opt  ULARGE_INTEGER *pcbRead, __out_opt  ULARGE_INTEGER *pcbWritten);
  IFACEMETHODIMP LockRegion(__in ULARGE_INTEGER libOffset, __in ULARGE_INTEGER cb, __in DWORD dwLockType);
  IFACEMETHODIMP Read(__out void *pv, __in ULONG cb, __out ULONG *pcbRead);
  IFACEMETHODIMP Revert(void);
  IFACEMETHODIMP Seek(__in LARGE_INTEGER dlibMove, __in DWORD dwOrigin, __out_opt  ULARGE_INTEGER *plibNewPosition);
  IFACEMETHODIMP SetSize(__in ULARGE_INTEGER libNewSize);
  IFACEMETHODIMP Stat(__out __RPC__out STATSTG *pstatstg, __in DWORD grfStatFlag);
  IFACEMETHODIMP UnlockRegion(__in ULARGE_INTEGER libOffset, __in ULARGE_INTEGER cb, __in DWORD dwLockType);
  IFACEMETHODIMP Write(__in void const *pv, __in ULONG cb, __out ULONG *pcbWritten);

  // IStorage
  IFACEMETHODIMP STDMETHODCALLTYPE CreateStream(
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName,
        /* [in] */ DWORD grfMode,
        /* [in] */ DWORD reserved1,
        /* [in] */ DWORD reserved2,
        /* [out] */ __RPC__deref_out_opt IStream **ppstm);

    IFACEMETHODIMP STDMETHODCALLTYPE OpenStream(
        /* [string][in] */ const OLECHAR *pwcsName,
        /* [unique][in] */ void *reserved1,
        /* [in] */ DWORD grfMode,
        /* [in] */ DWORD reserved2,
        /* [out] */ IStream **ppstm);

    IFACEMETHODIMP STDMETHODCALLTYPE CreateStorage(
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName,
        /* [in] */ DWORD grfMode,
        /* [in] */ DWORD reserved1,
        /* [in] */ DWORD reserved2,
        /* [out] */ __RPC__deref_out_opt IStorage **ppstg);

    IFACEMETHODIMP STDMETHODCALLTYPE OpenStorage(
        /* [string][unique][in] */ __RPC__in_opt_string const OLECHAR *pwcsName,
        /* [unique][in] */ __RPC__in_opt IStorage *pstgPriority,
        /* [in] */ DWORD grfMode,
        /* [unique][in] */ __RPC__deref_opt_in_opt SNB snbExclude,
        /* [in] */ DWORD reserved,
        /* [out] */ __RPC__deref_out_opt IStorage **ppstg);

    IFACEMETHODIMP STDMETHODCALLTYPE CopyTo(
        /* [in] */ DWORD ciidExclude,
        /* [size_is][unique][in] */ const IID *rgiidExclude,
        /* [annotation][unique][in] */
        __RPC__in_opt  SNB snbExclude,
        /* [unique][in] */ IStorage *pstgDest);

    IFACEMETHODIMP STDMETHODCALLTYPE MoveElementTo(
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName,
        /* [unique][in] */ __RPC__in_opt IStorage *pstgDest,
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsNewName,
        /* [in] */ DWORD grfFlags);
    //
    // IFACEMETHODIMP STDMETHODCALLTYPE Commit(
    //    /* [in] */ DWORD grfCommitFlags);
    //
    // IFACEMETHODIMP STDMETHODCALLTYPE Revert(void);
    //
    IFACEMETHODIMP STDMETHODCALLTYPE EnumElements(
        /* [in] */ DWORD reserved1,
        /* [size_is][unique][in] */ void *reserved2,
        /* [in] */ DWORD reserved3,
        /* [out] */ IEnumSTATSTG **ppenum);

    IFACEMETHODIMP STDMETHODCALLTYPE DestroyElement(
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName);

    IFACEMETHODIMP STDMETHODCALLTYPE RenameElement(
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsOldName,
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsNewName);

    IFACEMETHODIMP STDMETHODCALLTYPE SetElementTimes(
        /* [string][unique][in] */ __RPC__in_opt_string const OLECHAR *pwcsName,
        /* [unique][in] */ __RPC__in_opt const FILETIME *pctime,
        /* [unique][in] */ __RPC__in_opt const FILETIME *patime,
        /* [unique][in] */ __RPC__in_opt const FILETIME *pmtime);

    IFACEMETHODIMP STDMETHODCALLTYPE SetClass(
        /* [in] */ __RPC__in REFCLSID clsid);

    IFACEMETHODIMP STDMETHODCALLTYPE SetStateBits(
        /* [in] */ DWORD grfStateBits,
        /* [in] */ DWORD grfMask);
    //
    // IFACEMETHODIMP STDMETHODCALLTYPE Stat(
    //    /* [out] */ __RPC__out STATSTG *pstatstg,
    //    /* [in] */ DWORD grfStatFlag);

  // IPersist
  IFACEMETHODIMP GetClassID(__out CLSID *pclsid);

  // IPersistStorage
  IFACEMETHODIMP IsDirty( void);

    IFACEMETHODIMP InitNew(
        /* [unique][in] */ __RPC__in_opt IStorage *pStg);

    IFACEMETHODIMP Load(
        /* [unique][in] */ __RPC__in_opt IStorage *pStg);

    IFACEMETHODIMP Save(
        /* [unique][in] */ __RPC__in_opt IStorage *pStgSave,
        /* [in] */ BOOL fSameAsLoad);

    IFACEMETHODIMP SaveCompleted(
        /* [unique][in] */ __RPC__in_opt IStorage *pStgNew);

    IFACEMETHODIMP HandsOffStorage( void);

  IFACEMETHODIMP BindToHandler(
            /* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
            /* [in] */ __RPC__in REFGUID bhid,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ __RPC__deref_out_opt void **ppv);

    IFACEMETHODIMP GetParent(
            /* [out] */ __RPC__deref_out_opt IShellItem **ppsi);

     IFACEMETHODIMP GetDisplayName(
            /* [in] */ SIGDN sigdnName,
            /* [string][out] */ __RPC__deref_out_opt_string LPWSTR *ppszName);

    IFACEMETHODIMP GetAttributes(
            /* [in] */ SFGAOF sfgaoMask,
            /* [out] */ __RPC__out SFGAOF *psfgaoAttribs);

    IFACEMETHODIMP Compare(
            /* [in] */ __RPC__in_opt IShellItem *psi,
            /* [in] */ SICHINTF hint,
            /* [out] */ __RPC__out int *piOrder);

  IFACEMETHODIMP Advise(
            /* [annotation][in] */
            __in  ITransferAdviseSink *psink,
            /* [annotation][out] */
            __out  DWORD *pdwCookie);

        IFACEMETHODIMP Unadvise(
            /* [in] */ DWORD dwCookie);

        IFACEMETHODIMP CreateItem(
            /* [annotation][string][in] */
            __in  LPCWSTR pszName,
            /* [annotation][in] */
            __in  DWORD dwAttributes,
            /* [annotation][in] */
            __in  ULONGLONG ullSize,
            /* [annotation][in] */
            __in  TRANSFER_SOURCE_FLAGS flags,
            /* [annotation][in] */
            __in  REFIID riidItem,
            /* [annotation][iid_is][out] */
            __deref_out  void **ppvItem,
            /* [annotation][in] */
            __in  REFIID riidResources,
            /* [annotation][iid_is][out] */
            __deref_out  void **ppvResources);

        IFACEMETHODIMP SetProperties(
            /* [annotation][in] */
            __in  IPropertyChangeArray *pproparray);

        IFACEMETHODIMP OpenItem(
            /* [annotation][in] */
            __in  IShellItem *psi,
            /* [annotation][in] */
            __in  TRANSFER_SOURCE_FLAGS flags,
            /* [annotation][in] */
            __in  REFIID riid,
            /* [annotation][iid_is][out] */
            __deref_out  void **ppv);

        IFACEMETHODIMP MoveItem(
            /* [annotation][in] */
            __in  IShellItem *psi,
            /* [annotation][in] */
            __in  IShellItem *psiParentDst,
            /* [annotation][string][in] */
            __in  LPCWSTR pszNameDst,
            /* [in] */ TRANSFER_SOURCE_FLAGS flags,
            /* [annotation][out] */
            __deref_out  IShellItem **ppsiNew);

        IFACEMETHODIMP RecycleItem(
            /* [annotation][in] */
            __in  IShellItem *psiSource,
            /* [annotation][in] */
            __in  IShellItem *psiParentDest,
            /* [annotation][in] */
            __in  TRANSFER_SOURCE_FLAGS flags,
            /* [annotation][out] */
            __deref_out  IShellItem **ppsiNewDest);

        IFACEMETHODIMP RemoveItem(
            /* [annotation][in] */
            __in  IShellItem *psiSource,
            /* [annotation][in] */
            __in  TRANSFER_SOURCE_FLAGS flags);

        IFACEMETHODIMP RenameItem(
            /* [annotation][in] */
            __in  IShellItem *psiSource,
            /* [annotation][string][in] */
            __in  LPCWSTR pszNewName,
            /* [annotation][in] */
            __in  TRANSFER_SOURCE_FLAGS flags,
            /* [annotation][out] */
            __deref_out  IShellItem **ppsiNewDest);

        IFACEMETHODIMP LinkItem(
            /* [annotation][in] */
            __in  IShellItem *psiSource,
            /* [annotation][in] */
            __in  IShellItem *psiParentDest,
            /* [annotation][string][unique][in] */
            __in_opt  LPCWSTR pszNewName,
            /* [annotation][in] */
            __in  TRANSFER_SOURCE_FLAGS flags,
            /* [annotation][out] */
            __deref_out  IShellItem **ppsiNewDest);

        IFACEMETHODIMP ApplyPropertiesToItem(
            /* [annotation][in] */
            __in  IShellItem *psiSource,
            /* [annotation][out] */
            __deref_out  IShellItem **ppsiNew);

        IFACEMETHODIMP GetDefaultDestinationName(
            /* [annotation][in] */
            __in  IShellItem *psiSource,
            /* [annotation][in] */
            __in  IShellItem *psiParentDest,
            /* [annotation][string][out] */
            __deref_out  LPWSTR *ppszDestinationName);

        IFACEMETHODIMP EnterFolder(
            /* [annotation][in] */
            __in  IShellItem *psiChildFolderDest);

        IFACEMETHODIMP LeaveFolder(
            /* [annotation][in] */
            __in  IShellItem *psiChildFolderDest);

    IFACEMETHODIMP GetPropertyStore(
            /* [in] */ GETPROPERTYSTOREFLAGS flags,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ __RPC__deref_out_opt void **ppv);

        IFACEMETHODIMP GetPropertyStoreWithCreateObject(
            /* [in] */ GETPROPERTYSTOREFLAGS flags,
            /* [in] */ __RPC__in_opt IUnknown *punkCreateObject,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ __RPC__deref_out_opt void **ppv);

        IFACEMETHODIMP GetPropertyStoreForKeys(
            /* [size_is][in] */ __RPC__in_ecount_full(cKeys) const PROPERTYKEY *rgKeys,
            /* [in] */ UINT cKeys,
            /* [in] */ GETPROPERTYSTOREFLAGS flags,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ __RPC__deref_out_opt void **ppv);

        IFACEMETHODIMP GetPropertyDescriptionList(
            /* [in] */ __RPC__in REFPROPERTYKEY keyType,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ __RPC__deref_out_opt void **ppv);

        IFACEMETHODIMP Update(
            /* [unique][in] */ __RPC__in_opt IBindCtx *pbc);

        IFACEMETHODIMP GetProperty(
            /* [in] */ __RPC__in REFPROPERTYKEY key,
            /* [out] */ __RPC__out PROPVARIANT *ppropvar);

        IFACEMETHODIMP GetCLSID(
            /* [in] */ __RPC__in REFPROPERTYKEY key,
            /* [out] */ __RPC__out CLSID *pclsid);

        IFACEMETHODIMP GetFileTime(
            /* [in] */ __RPC__in REFPROPERTYKEY key,
            /* [out] */ __RPC__out FILETIME *pft);

        IFACEMETHODIMP GetInt32(
            /* [in] */ __RPC__in REFPROPERTYKEY key,
            /* [out] */ __RPC__out int *pi);

        IFACEMETHODIMP GetString(
            /* [in] */ __RPC__in REFPROPERTYKEY key,
            /* [string][out] */ __RPC__deref_out_opt_string LPWSTR *ppsz);

        IFACEMETHODIMP GetUInt32(
            /* [in] */ __RPC__in REFPROPERTYKEY key,
            /* [out] */ __RPC__out ULONG *pui);

        IFACEMETHODIMP GetUInt64(
            /* [in] */ __RPC__in REFPROPERTYKEY key,
            /* [out] */ __RPC__out ULONGLONG *pull);

        IFACEMETHODIMP GetBool(
            /* [in] */ __RPC__in REFPROPERTYKEY key,
            /* [out] */ __RPC__out BOOL *pf);


  // IStorage
  IFACEMETHODIMP CreateStream(
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName,
        /* [in] */ DWORD grfMode,
        /* [in] */ DWORD reserved1,
        /* [in] */ DWORD reserved2,
        /* [out] */ __RPC__deref_out_opt IStream **ppstm);

    IFACEMETHODIMP OpenStream(
        /* [string][in] */ const OLECHAR *pwcsName,
        /* [unique][in] */ void *reserved1,
        /* [in] */ DWORD grfMode,
        /* [in] */ DWORD reserved2,
        /* [out] */ IStream **ppstm);

    IFACEMETHODIMP CreateStorage(
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName,
        /* [in] */ DWORD grfMode,
        /* [in] */ DWORD reserved1,
        /* [in] */ DWORD reserved2,
        /* [out] */ __RPC__deref_out_opt IStorage **ppstg);

    IFACEMETHODIMP OpenStorage(
        /* [string][unique][in] */ __RPC__in_opt_string const OLECHAR *pwcsName,
        /* [unique][in] */ __RPC__in_opt IStorage *pstgPriority,
        /* [in] */ DWORD grfMode,
        /* [unique][in] */ __RPC__deref_opt_in_opt SNB snbExclude,
        /* [in] */ DWORD reserved,
        /* [out] */ __RPC__deref_out_opt IStorage **ppstg);

    IFACEMETHODIMP CopyTo(
        /* [in] */ DWORD ciidExclude,
        /* [size_is][unique][in] */ const IID *rgiidExclude,
        /* [annotation][unique][in] */
        __RPC__in_opt  SNB snbExclude,
        /* [unique][in] */ IStorage *pstgDest);

    IFACEMETHODIMP MoveElementTo(
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName,
        /* [unique][in] */ __RPC__in_opt IStorage *pstgDest,
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsNewName,
        /* [in] */ DWORD grfFlags);

    IFACEMETHODIMP Commit(
        /* [in] */ DWORD grfCommitFlags);

    IFACEMETHODIMP Revert(void);

    IFACEMETHODIMP EnumElements(
        /* [in] */ DWORD reserved1,
        /* [size_is][unique][in] */ void *reserved2,
        /* [in] */ DWORD reserved3,
        /* [out] */ IEnumSTATSTG **ppenum);

    IFACEMETHODIMP DestroyElement(
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName);

    IFACEMETHODIMP RenameElement(
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsOldName,
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsNewName);

    IFACEMETHODIMP SetElementTimes(
        /* [string][unique][in] */ __RPC__in_opt_string const OLECHAR *pwcsName,
        /* [unique][in] */ __RPC__in_opt const FILETIME *pctime,
        /* [unique][in] */ __RPC__in_opt const FILETIME *patime,
        /* [unique][in] */ __RPC__in_opt const FILETIME *pmtime);

    IFACEMETHODIMP SetClass(
        /* [in] */ __RPC__in REFCLSID clsid);

    IFACEMETHODIMP SetStateBits(
        /* [in] */ DWORD grfStateBits,
        /* [in] */ DWORD grfMask);

    IFACEMETHODIMP Stat(
        /* [out] */ __RPC__out STATSTG *pstatstg,
        /* [in] */ DWORD grfStatFlag);


  // IPersistStorage
  IFACEMETHODIMP IsDirty( void);

    IFACEMETHODIMP InitNew(
        /* [unique][in] */ __RPC__in_opt IStorage *pStg);

    IFACEMETHODIMP Load(
        /* [unique][in] */ __RPC__in_opt IStorage *pStg);

    IFACEMETHODIMP Save(
        /* [unique][in] */ __RPC__in_opt IStorage *pStgSave,
        /* [in] */ BOOL fSameAsLoad);

    IFACEMETHODIMP SaveCompleted(
        /* [unique][in] */ __RPC__in_opt IStorage *pStgNew);

    IFACEMETHODIMP HandsOffStorage( void);

  // IOleItemContainer
   IFACEMETHODIMP GetObject(
        /* [in] */ __RPC__in LPOLESTR pszItem,
        /* [in] */ DWORD dwSpeedNeeded,
        /* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
        /* [in] */ __RPC__in REFIID riid,
        /* [iid_is][out] */ __RPC__deref_out_opt void **ppvObject);

    IFACEMETHODIMP GetObjectStorage(
        /* [in] */ __RPC__in LPOLESTR pszItem,
        /* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
        /* [in] */ __RPC__in REFIID riid,
        /* [iid_is][out] */ __RPC__deref_out_opt void **ppvStorage);

    IFACEMETHODIMP IsRunning(
        /* [in] */ __RPC__in LPOLESTR pszItem);

  IFACEMETHODIMP EnumObjects(
        /* [in] */ DWORD grfFlags,
        /* [out] */ __RPC__deref_out_opt IEnumUnknown **ppenum);

    IFACEMETHODIMP LockContainer(
        /* [in] */ BOOL fLock);

  IFACEMETHODIMP ParseDisplayName(
        /* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
        /* [in] */ __RPC__in LPOLESTR pszDisplayName,
        /* [out] */ __RPC__out ULONG *pchEaten,
        /* [out] */ __RPC__deref_out_opt IMoniker **ppmkOut);


  // IStorage
  IFACEMETHODIMP STDMETHODCALLTYPE CreateStream(
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName,
        /* [in] */ DWORD grfMode,
        /* [in] */ DWORD reserved1,
        /* [in] */ DWORD reserved2,
        /* [out] */ __RPC__deref_out_opt IStream **ppstm);

    IFACEMETHODIMP STDMETHODCALLTYPE OpenStream(
        /* [string][in] */ const OLECHAR *pwcsName,
        /* [unique][in] */ void *reserved1,
        /* [in] */ DWORD grfMode,
        /* [in] */ DWORD reserved2,
        /* [out] */ IStream **ppstm);

    IFACEMETHODIMP STDMETHODCALLTYPE CreateStorage(
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName,
        /* [in] */ DWORD grfMode,
        /* [in] */ DWORD reserved1,
        /* [in] */ DWORD reserved2,
        /* [out] */ __RPC__deref_out_opt IStorage **ppstg);

    IFACEMETHODIMP STDMETHODCALLTYPE OpenStorage(
        /* [string][unique][in] */ __RPC__in_opt_string const OLECHAR *pwcsName,
        /* [unique][in] */ __RPC__in_opt IStorage *pstgPriority,
        /* [in] */ DWORD grfMode,
        /* [unique][in] */ __RPC__deref_opt_in_opt SNB snbExclude,
        /* [in] */ DWORD reserved,
        /* [out] */ __RPC__deref_out_opt IStorage **ppstg);

    IFACEMETHODIMP STDMETHODCALLTYPE CopyTo(
        /* [in] */ DWORD ciidExclude,
        /* [size_is][unique][in] */ const IID *rgiidExclude,
        /* [annotation][unique][in] */
        __RPC__in_opt  SNB snbExclude,
        /* [unique][in] */ IStorage *pstgDest);

    IFACEMETHODIMP STDMETHODCALLTYPE MoveElementTo(
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName,
        /* [unique][in] */ __RPC__in_opt IStorage *pstgDest,
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsNewName,
        /* [in] */ DWORD grfFlags);
    //
    // IFACEMETHODIMP STDMETHODCALLTYPE Commit(
    //    /* [in] */ DWORD grfCommitFlags);
    //
    // IFACEMETHODIMP STDMETHODCALLTYPE Revert(void);
    //
    IFACEMETHODIMP STDMETHODCALLTYPE EnumElements(
        /* [in] */ DWORD reserved1,
        /* [size_is][unique][in] */ void *reserved2,
        /* [in] */ DWORD reserved3,
        /* [out] */ IEnumSTATSTG **ppenum);

    IFACEMETHODIMP STDMETHODCALLTYPE DestroyElement(
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName);

    IFACEMETHODIMP STDMETHODCALLTYPE RenameElement(
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsOldName,
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsNewName);

    IFACEMETHODIMP STDMETHODCALLTYPE SetElementTimes(
        /* [string][unique][in] */ __RPC__in_opt_string const OLECHAR *pwcsName,
        /* [unique][in] */ __RPC__in_opt const FILETIME *pctime,
        /* [unique][in] */ __RPC__in_opt const FILETIME *patime,
        /* [unique][in] */ __RPC__in_opt const FILETIME *pmtime);

    IFACEMETHODIMP STDMETHODCALLTYPE SetClass(
        /* [in] */ __RPC__in REFCLSID clsid);

    IFACEMETHODIMP STDMETHODCALLTYPE SetStateBits(
        /* [in] */ DWORD grfStateBits,
        /* [in] */ DWORD grfMask);
    //
    // IFACEMETHODIMP STDMETHODCALLTYPE Stat(
    //    /* [out] */ __RPC__out STATSTG *pstatstg,
    //    /* [in] */ DWORD grfStatFlag);

private:
  HRESULT _Initialize(FileInfo* fileInfo);

  HRESULT _DownloadFile();
  HRESULT _Open(bool downloadFile = true);
  HRESULT _Close(bool uploadFile = false);
  HRESULT _UploadFile();

  bool _HasError();
  int _GetError();

  ULARGE_INTEGER _StatFileSize();
  FILETIME _StatFileTimeModified();
  FILETIME _StatFileTimeCreated();
  FILETIME _StatFileTimeAccessed();

  FileInfo* _fileInfo;
  bool _isFileDownloaded;
  std::fstream _stream;
  DWORD _stgmMode;
  DWORD _lockType;
  ULONGLONG _position;
};
