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
#include "StdAfx.h"
#include "DriveItemStream.h"
#include "GDriveShlExt.h"

using namespace Fusion::GoogleDrive;

HRESULT CDriveItemStream::CreateInstance(FileInfo* fileInfo, REFIID riid, __deref_out void **ppv)
{
    Log::WriteOutput(LogType::LowPriority, L"CDriveItemStream::CreateInstance(FileInfo* fileInfo, REFIID riid, __deref_out void **ppv)");

  *ppv = NULL;

    CComObjectWithRef<CDriveItemStream> *psprfvcb;

  HRESULT hr = CComObjectWithRef<CDriveItemStream>::CreateInstance(&psprfvcb);

  if (!SUCCEEDED(hr))
  {
    Log::WriteOutput(LogType::Error, L"CComObjectWithRef<CDriveItemStream>::CreateInstance returned hr=%d", hr);
  }
  else
    {
        hr = psprfvcb->_Initialize(fileInfo);

        if (SUCCEEDED(hr))
        {
            hr = psprfvcb->QueryInterface(riid, ppv);

      if (!SUCCEEDED(hr))
      {
        Log::WriteOutput(LogType::Error, L"CComObjectWithRef<CDriveItemStream>->QueryInterface returned hr=%d", hr);
      }
        }

        psprfvcb->Release();
    }

    return hr;
}

HRESULT CDriveItemStream::_Initialize(FileInfo* fileInfo)
{
  Log::WriteOutput(LogType::LowPriority, L"CDriveItemStream::_Initialize(FileInfo* fileInfo)");

  HRESULT hr = S_OK;

  _fileInfo = fileInfo;
  _isFileDownloaded = false;
  _stgmMode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT_SWMR;
  _lockType = LOCKTYPE::LOCK_EXCLUSIVE;
  _position = 0;

    return S_OK;
}

bool CDriveItemStream::_HasError()
{
  int error = _GetError();

  if (error != 0)
    return true;

  return false;
}

int CDriveItemStream::_GetError()
{
  std::ios_base::iostate iostate = _stream.rdstate();

  return iostate;
}

HRESULT CDriveItemStream::_DownloadFile()
{
  HRESULT hr = S_OK;

  if (!_isFileDownloaded)
  {
    if (!_fileManager.DownloadFile(_fileInfo))
    {
      std::wstring message;

      if (_fileManager.HasError())
      {
        message = L"Could not download file " + _fileInfo->FilePath() + L" - " + _fileManager.ErrorMessage();
      }
      else
      {
        message = L"Could not download file " + _fileInfo->FilePath() + L" - Unknown error";
      }

      Log::WriteOutput(LogType::Error, message.c_str());

      hr = E_FAIL;
    }
    else
    {
      _isFileDownloaded = true;
    }
  }

  return hr;
}

HRESULT CDriveItemStream::_Open(bool downloadFile)
{
  HRESULT hr = S_OK;

  if (downloadFile)
  {
    hr = _DownloadFile();
  }

  if (SUCCEEDED(hr))
  {
    if (!_stream.is_open())
    {
      if (!Service::FileExists(_fileInfo->FilePath()))
      {
        Log::WriteOutput(LogType::Error, L"Could not open stream for file %s - File does not exist", _fileInfo->FilePath().c_str());

        hr = E_FAIL;
      }
      else
      {
        std::ios_base::open_mode open_mode = std::fstream::binary;

        if (_stgmMode == STGM_READ || _stgmMode == STGM_READWRITE)
        {
          open_mode |= std::fstream::in;
        }

        if (_stgmMode == STGM_WRITE || _stgmMode == STGM_READWRITE)
        {
          open_mode |= std::fstream::out;
        }

        _stream.open(_fileInfo->FilePath().c_str(), open_mode);

        int error = _GetError();

        if (!_stream.is_open())
        {
          if (_HasError())
          {
            Log::WriteOutput(LogType::Error, L"Could not open stream for file %s - IO error %d", _fileInfo->FilePath().c_str(), _GetError());
          }
          else
          {
            Log::WriteOutput(LogType::Error, L"Could not open stream for file %s - Unknown error", _fileInfo->FilePath().c_str());
          }

          hr = E_FAIL;
        }
      }
    }
  }

  return hr;
}

HRESULT CDriveItemStream::_Close(bool uploadFile)
{
  HRESULT hr = S_OK;

  if (_stream.is_open())
  {
    _stream.close();
  }

  if (uploadFile && _isFileDownloaded)
  {
    // TODO: hr = _UploadFile();
  }

  return hr;
}

HRESULT CDriveItemStream::_UploadFile()
{
  if (!_isFileDownloaded)
  {
    Log::WriteOutput(LogType::Error, L"Could not upload file %s - File has not been downloaded", _fileInfo->FilePath().c_str());

    return false;
  }

  if (!_fileManager.UploadFile(_fileInfo))
  {
    std::wstring message;

    if (_fileManager.HasError())
    {
      message = L"Could not upload file " + _fileInfo->FilePath() + L" - " + _fileManager.ErrorMessage();
    }
    else
    {
      message = L"Could not upload file " + _fileInfo->FilePath() + L" - Unknown error";
    }

    Log::WriteOutput(LogType::Error, message.c_str());

    return false;
  }

  _isFileDownloaded = false;

  return true;
}

STDMETHODIMP CDriveItemStream::Clone(__out __RPC__deref_out_opt IStream **ppstm)
{
  Log::WriteOutput(LogType::HighPriority, L"CDriveItemStream::Clone");

  // Creates a new stream object that references the same bytes as the original stream but provides a separate seek pointer to those bytes.

  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::Commit(__in DWORD grfCommitFlags)
{
  Log::WriteOutput(LogType::HighPriority, L"CDriveItemStream::Commit");

  // Ensures that any changes made to a stream object open in transacted mode are reflected in the parent storage object.

  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::CopyTo(__in IStream *pstm, __in ULARGE_INTEGER cb, __out_opt  ULARGE_INTEGER *pcbRead, __out_opt  ULARGE_INTEGER *pcbWritten)
{
  Log::WriteOutput(LogType::HighPriority, L"CDriveItemStream::CopyTo");

  // Copies a specified number of bytes from the current seek pointer in the stream to the current seek pointer in another stream.

  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::LockRegion(__in ULARGE_INTEGER libOffset, __in ULARGE_INTEGER cb, __in DWORD dwLockType)
{
  Log::WriteOutput(LogType::HighPriority, L"CDriveItemStream::LockRegion");

  // Restricts access to a specified range of bytes in the stream. Supporting this functionality is optional since some file systems do not provide it.

  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::Read(__out void *pv, __in ULONG cb, __out ULONG *pcbRead)
{
  Log::WriteOutput(LogType::HighPriority, L"CDriveItemStream::Read");

  // Reads a specified number of bytes from the stream object into memory starting at the current seek pointer. (Inherited from ISequentialStream)

  // pv [out]
  // A pointer to the buffer which the stream data is read into.

  // cb [in]
  // The number of bytes of data to read from the stream object.

  // pcbRead [out]
  // A pointer to a ULONG variable that receives the actual number of bytes read from the stream object.

  HRESULT hr = S_OK;

  hr = _Open();

  if (SUCCEEDED(hr))
  {
    _stream.read((char*)pv, cb);

    if (_HasError())
    {
      hr = E_FAIL;

      Log::WriteOutput(LogType::Error, L"Could not read stream for file %s - IO error %d", _fileInfo->FilePath().c_str(), _GetError());
    }
    else
    {
      std::streamsize bytesRead = _stream.gcount();

      if (pcbRead == NULL)
        return STG_E_INVALIDPOINTER;

      *pcbRead = (ULONG)bytesRead;

      if (bytesRead < cb)
      {
        hr = S_FALSE;
      }
    }
  }

  _Close();

  return hr;
}

STDMETHODIMP CDriveItemStream::Revert(void)
{
  Log::WriteOutput(LogType::HighPriority, L"CDriveItemStream::Revert");

  // Discards all changes that have been made to a transacted stream since the last call to IStream::Commit.

  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::Seek(__in LARGE_INTEGER dlibMove, __in DWORD dwOrigin, __out_opt  ULARGE_INTEGER *plibNewPosition)
{
  Log::WriteOutput(LogType::HighPriority, L"CDriveItemStream::Seek");

  // Changes the seek pointer to a new location relative to the beginning of the stream, the end of the stream, or the current seek pointer.

  // dlibMove [in]
  // The displacement to be added to the location indicated by the dwOrigin parameter.
  // If dwOrigin is STREAM_SEEK_SET, this is interpreted as an unsigned value rather than a signed value.

  // dwOrigin [in]
  // The origin for the displacement specified in dlibMove. The origin can be the beginning
  // of the file (STREAM_SEEK_SET), the current seek pointer (STREAM_SEEK_CUR), or the end of the file (STREAM_SEEK_END).
  // For more information about values, see the STREAM_SEEK enumeration.

  // plibNewPosition [out]
  // A pointer to the location where this method writes the value of the new seek pointer from the beginning of the stream.
  // You can set this pointer to NULL. In this case, this method does not provide the new seek pointer.

  /*
  if (!Sync())
    return E_FAIL;
*/
  ULONGLONG position = 0;

  if (dwOrigin == STREAM_SEEK_SET)
  {
    // The new seek pointer is an offset relative to the beginning of the stream.
    // In this case, the dlibMove parameter is the new seek position relative to the beginning of the stream.
    position = dlibMove.QuadPart;
  }
  else if (dwOrigin == STREAM_SEEK_CUR)
  {
    // The new seek pointer is an offset relative to the current seek pointer location.
    // In this case, the dlibMove parameter is the signed displacement from the current seek position.
    position = _position + dlibMove.QuadPart;
  }
  else if (dwOrigin == STREAM_SEEK_END)
  {
    // The new seek pointer is an offset relative to the end of the stream.
    // In this case, the dlibMove parameter is the new seek position relative to the end of the stream.
    position = _StatFileSize().QuadPart + dlibMove.QuadPart;
  }

  if (position < 0)
    return STG_E_INVALIDFUNCTION;

  _position = position;

  if (plibNewPosition == NULL)
    return STG_E_INVALIDPOINTER;

  plibNewPosition->QuadPart = position;

  return S_OK;
}

STDMETHODIMP CDriveItemStream::SetSize(__in ULARGE_INTEGER libNewSize)
{
  Log::WriteOutput(LogType::HighPriority, L"CDriveItemStream::SetSize");

  // Changes the size of the stream object.

  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::Stat(__out __RPC__out STATSTG *pstatstg, __in DWORD grfStatFlag)
{
  Log::WriteOutput(LogType::HighPriority, L"CDriveItemStream::Stat");

  // Retrieves the STATSTG structure for this stream.

  if (grfStatFlag != STATFLAG_NONAME)
  {
    HRESULT hr = SHStrDup(_fileInfo->Title.c_str(), &pstatstg->pwcsName);

    if (!SUCCEEDED(hr))
    {
      Log::WriteOutput(LogType::Error, L"SHStrDup returned hr=%d", hr);

      return hr;
    }
  }

  pstatstg->type = STGTY_STREAM;
  pstatstg->cbSize = _StatFileSize();
  pstatstg->mtime = _StatFileTimeModified();
  pstatstg->ctime = _StatFileTimeCreated();
  pstatstg->atime = _StatFileTimeAccessed();
  pstatstg->grfMode = _stgmMode;
  pstatstg->grfLocksSupported = _lockType;
  pstatstg->clsid = CLSID_NULL;
  pstatstg->grfStateBits = 0;
  pstatstg->reserved = 0;

  return S_OK;
}

ULARGE_INTEGER CDriveItemStream::_StatFileSize()
{
  ULARGE_INTEGER result;

  result.HighPart = 0;
  result.LowPart = 0;
  result.QuadPart = 0;
  result.u.HighPart = 0;
  result.u.LowPart = 0;

  if (_fileInfo->FileSize != L"")
  {
    result.QuadPart = std::stoull(_fileInfo->FileSize);
  }

  return result;
}

FILETIME GetStatFileTime(std::wstring& value)
{
  FILETIME fileTime;

  fileTime.dwHighDateTime = 0;
  fileTime.dwLowDateTime = 0;

  if (value.length() == 24)
  {
    std::wstring year = value.substr(0, 4);
    std::wstring month = value.substr(5, 2);
    std::wstring day = value.substr(8, 2);
    std::wstring hour = value.substr(11, 2);
    std::wstring minute = value.substr(14, 2);
    std::wstring second = value.substr(17, 2);
    std::wstring milli = value.substr(20, 3);

    SYSTEMTIME systemTime;

    systemTime.wYear = std::stoul(year);
    systemTime.wMonth = std::stoul(month);
    systemTime.wDay = std::stoul(day);
    systemTime.wHour = std::stoul(hour);
    systemTime.wMinute = std::stoul(minute);
    systemTime.wSecond = std::stoul(second);
    systemTime.wMilliseconds = std::stoul(milli);

    SystemTimeToFileTime(&systemTime, &fileTime);
  }

  return fileTime;
}

FILETIME CDriveItemStream::_StatFileTimeModified()
{
  FILETIME result = GetStatFileTime(_fileInfo->ModifiedDate);

  return result;
}

FILETIME CDriveItemStream::_StatFileTimeCreated()
{
  FILETIME result = GetStatFileTime(_fileInfo->CreatedDate);

  return result;
}

FILETIME CDriveItemStream::_StatFileTimeAccessed()
{
  FILETIME result = GetStatFileTime(_fileInfo->LastViewedByMeDate);

  return result;
}

STDMETHODIMP CDriveItemStream::UnlockRegion(__in ULARGE_INTEGER libOffset, __in ULARGE_INTEGER cb, __in DWORD dwLockType)
{
  Log::WriteOutput(LogType::HighPriority, L"CDriveItemStream::UnlockRegion");

  // Removes the access restriction on a range of bytes previously restricted with IStream::LockRegion.

  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::Write(__in void const *pv, __in ULONG cb, __out ULONG *pcbWritten)
{
  Log::WriteOutput(LogType::HighPriority, L"CDriveItemStream::Write");

  // Writes a specified number of bytes into the stream object starting at the current seek pointer. (Inherited from ISequentialStream)

  return E_NOTIMPL;
}



STDMETHODIMP CDriveItemStream::CreateStream(
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName,
        /* [in] */ DWORD grfMode,
        /* [in] */ DWORD reserved1,
        /* [in] */ DWORD reserved2,
        /* [out] */ __RPC__deref_out_opt IStream **ppstm)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::OpenStream(
        /* [string][in] */ const OLECHAR *pwcsName,
        /* [unique][in] */ void *reserved1,
        /* [in] */ DWORD grfMode,
        /* [in] */ DWORD reserved2,
        /* [out] */ IStream **ppstm)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::CreateStorage(
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName,
        /* [in] */ DWORD grfMode,
        /* [in] */ DWORD reserved1,
        /* [in] */ DWORD reserved2,
        /* [out] */ __RPC__deref_out_opt IStorage **ppstg)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::OpenStorage(
        /* [string][unique][in] */ __RPC__in_opt_string const OLECHAR *pwcsName,
        /* [unique][in] */ __RPC__in_opt IStorage *pstgPriority,
        /* [in] */ DWORD grfMode,
        /* [unique][in] */ __RPC__deref_opt_in_opt SNB snbExclude,
        /* [in] */ DWORD reserved,
        /* [out] */ __RPC__deref_out_opt IStorage **ppstg)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::CopyTo(
        /* [in] */ DWORD ciidExclude,
        /* [size_is][unique][in] */ const IID *rgiidExclude,
        /* [annotation][unique][in] */
        __RPC__in_opt  SNB snbExclude,
        /* [unique][in] */ IStorage *pstgDest)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::MoveElementTo(
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName,
        /* [unique][in] */ __RPC__in_opt IStorage *pstgDest,
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsNewName,
        /* [in] */ DWORD grfFlags)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::EnumElements(
        /* [in] */ DWORD reserved1,
        /* [size_is][unique][in] */ void *reserved2,
        /* [in] */ DWORD reserved3,
        /* [out] */ IEnumSTATSTG **ppenum)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::DestroyElement(
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::RenameElement(
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsOldName,
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsNewName)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::SetElementTimes(
        /* [string][unique][in] */ __RPC__in_opt_string const OLECHAR *pwcsName,
        /* [unique][in] */ __RPC__in_opt const FILETIME *pctime,
        /* [unique][in] */ __RPC__in_opt const FILETIME *patime,
        /* [unique][in] */ __RPC__in_opt const FILETIME *pmtime)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::SetClass(
        /* [in] */ __RPC__in REFCLSID clsid)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::SetStateBits(
        /* [in] */ DWORD grfStateBits,
        /* [in] */ DWORD grfMask)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::IsDirty( void)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::InitNew(
    /* [unique][in] */ __RPC__in_opt IStorage *pStg)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::Load(
    /* [unique][in] */ __RPC__in_opt IStorage *pStg)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::Save(
    /* [unique][in] */ __RPC__in_opt IStorage *pStgSave,
    /* [in] */ BOOL fSameAsLoad)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::SaveCompleted(
    /* [unique][in] */ __RPC__in_opt IStorage *pStgNew)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::HandsOffStorage( void)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::GetClassID(__out CLSID *pclsid)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::BindToHandler(
        /* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
        /* [in] */ __RPC__in REFGUID bhid,
        /* [in] */ __RPC__in REFIID riid,
        /* [iid_is][out] */ __RPC__deref_out_opt void **ppv)
{

  if (bhid == BHID_SFObject)
  {
    bool b = true;
  }
  else if (bhid == BHID_SFUIObject)
  {
    bool b = true;
  }
  else if (bhid == BHID_SFViewObject)
  {
    bool b = true;
  }
  else if (bhid == BHID_Storage)
  {
    bool b = true;
  }
  else if (bhid == BHID_Stream)
  {
    bool b = true;
  }
  else if (bhid == BHID_LinkTargetItem)
  {
    bool b = true;
  }
  else if (bhid == BHID_StorageEnum)
  {
    bool b = true;
  }
  else if (bhid == BHID_Transfer)
  {
    bool b = true;
  }
  else if (bhid == BHID_PropertyStore)
  {
    bool b = true;
  }
  else if (bhid == BHID_ThumbnailHandler)
  {
    bool b = true;
  }
  else if (bhid == BHID_EnumItems)
  {
    bool b = true;
  }
  else if (bhid == BHID_DataObject)
  {
    bool b = true;
  }
  else if (bhid == BHID_AssociationArray)
  {
    bool b = true;
  }
  else if (bhid == BHID_Filter)
  {
    bool b = true;
  }
  else if (bhid == BHID_EnumAssocHandlers)
  {
    bool b = true;
  }
  else
  {
    bool b = true;
  }

  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::GetParent(
        /* [out] */ __RPC__deref_out_opt IShellItem **ppsi)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::GetDisplayName(
        /* [in] */ SIGDN sigdnName,
        /* [string][out] */ __RPC__deref_out_opt_string LPWSTR *ppszName)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::GetAttributes(
        /* [in] */ SFGAOF sfgaoMask,
        /* [out] */ __RPC__out SFGAOF *psfgaoAttribs)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::Compare(
        /* [in] */ __RPC__in_opt IShellItem *psi,
        /* [in] */ SICHINTF hint,
        /* [out] */ __RPC__out int *piOrder)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::Advise(
    /* [annotation][in] */
    __in  ITransferAdviseSink *psink,
    /* [annotation][out] */
    __out  DWORD *pdwCookie)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::Unadvise(
    /* [in] */ DWORD dwCookie)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::CreateItem(
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
    __deref_out  void **ppvResources)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::SetProperties(
    /* [annotation][in] */
    __in  IPropertyChangeArray *pproparray)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::OpenItem(
    /* [annotation][in] */
    __in  IShellItem *psi,
    /* [annotation][in] */
    __in  TRANSFER_SOURCE_FLAGS flags,
    /* [annotation][in] */
    __in  REFIID riid,
    /* [annotation][iid_is][out] */
    __deref_out  void **ppv)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::MoveItem(
    /* [annotation][in] */
    __in  IShellItem *psi,
    /* [annotation][in] */
    __in  IShellItem *psiParentDst,
    /* [annotation][string][in] */
    __in  LPCWSTR pszNameDst,
    /* [in] */ TRANSFER_SOURCE_FLAGS flags,
    /* [annotation][out] */
    __deref_out  IShellItem **ppsiNew)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::RecycleItem(
    /* [annotation][in] */
    __in  IShellItem *psiSource,
    /* [annotation][in] */
    __in  IShellItem *psiParentDest,
    /* [annotation][in] */
    __in  TRANSFER_SOURCE_FLAGS flags,
    /* [annotation][out] */
    __deref_out  IShellItem **ppsiNewDest)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::RemoveItem(
    /* [annotation][in] */
    __in  IShellItem *psiSource,
    /* [annotation][in] */
    __in  TRANSFER_SOURCE_FLAGS flags)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::RenameItem(
    /* [annotation][in] */
    __in  IShellItem *psiSource,
    /* [annotation][string][in] */
    __in  LPCWSTR pszNewName,
    /* [annotation][in] */
    __in  TRANSFER_SOURCE_FLAGS flags,
    /* [annotation][out] */
    __deref_out  IShellItem **ppsiNewDest)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::LinkItem(
    /* [annotation][in] */
    __in  IShellItem *psiSource,
    /* [annotation][in] */
    __in  IShellItem *psiParentDest,
    /* [annotation][string][unique][in] */
    __in_opt  LPCWSTR pszNewName,
    /* [annotation][in] */
    __in  TRANSFER_SOURCE_FLAGS flags,
    /* [annotation][out] */
    __deref_out  IShellItem **ppsiNewDest)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::ApplyPropertiesToItem(
    /* [annotation][in] */
    __in  IShellItem *psiSource,
    /* [annotation][out] */
    __deref_out  IShellItem **ppsiNew)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::GetDefaultDestinationName(
    /* [annotation][in] */
    __in  IShellItem *psiSource,
    /* [annotation][in] */
    __in  IShellItem *psiParentDest,
    /* [annotation][string][out] */
    __deref_out  LPWSTR *ppszDestinationName)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::EnterFolder(
    /* [annotation][in] */
    __in  IShellItem *psiChildFolderDest)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::LeaveFolder(
    /* [annotation][in] */
    __in  IShellItem *psiChildFolderDest)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::GetPropertyStore(
    /* [in] */ GETPROPERTYSTOREFLAGS flags,
    /* [in] */ __RPC__in REFIID riid,
    /* [iid_is][out] */ __RPC__deref_out_opt void **ppv)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::GetPropertyStoreWithCreateObject(
    /* [in] */ GETPROPERTYSTOREFLAGS flags,
    /* [in] */ __RPC__in_opt IUnknown *punkCreateObject,
    /* [in] */ __RPC__in REFIID riid,
    /* [iid_is][out] */ __RPC__deref_out_opt void **ppv)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::GetPropertyStoreForKeys(
    /* [size_is][in] */ __RPC__in_ecount_full(cKeys) const PROPERTYKEY *rgKeys,
    /* [in] */ UINT cKeys,
    /* [in] */ GETPROPERTYSTOREFLAGS flags,
    /* [in] */ __RPC__in REFIID riid,
    /* [iid_is][out] */ __RPC__deref_out_opt void **ppv)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::GetPropertyDescriptionList(
    /* [in] */ __RPC__in REFPROPERTYKEY keyType,
    /* [in] */ __RPC__in REFIID riid,
    /* [iid_is][out] */ __RPC__deref_out_opt void **ppv)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::Update(
    /* [unique][in] */ __RPC__in_opt IBindCtx *pbc)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::GetProperty(
    /* [in] */ __RPC__in REFPROPERTYKEY key,
    /* [out] */ __RPC__out PROPVARIANT *ppropvar)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::GetCLSID(
    /* [in] */ __RPC__in REFPROPERTYKEY key,
    /* [out] */ __RPC__out CLSID *pclsid)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::GetFileTime(
    /* [in] */ __RPC__in REFPROPERTYKEY key,
    /* [out] */ __RPC__out FILETIME *pft)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::GetInt32(
    /* [in] */ __RPC__in REFPROPERTYKEY key,
    /* [out] */ __RPC__out int *pi)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::GetString(
    /* [in] */ __RPC__in REFPROPERTYKEY key,
    /* [string][out] */ __RPC__deref_out_opt_string LPWSTR *ppsz)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::GetUInt32(
    /* [in] */ __RPC__in REFPROPERTYKEY key,
    /* [out] */ __RPC__out ULONG *pui)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::GetUInt64(
    /* [in] */ __RPC__in REFPROPERTYKEY key,
    /* [out] */ __RPC__out ULONGLONG *pull)
{
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::GetBool(
    /* [in] */ __RPC__in REFPROPERTYKEY key,
    /* [out] */ __RPC__out BOOL *pf)
{
  return E_NOTIMPL;
}



STDMETHODIMP CGDriveShlExt::CreateStream(
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName,
        /* [in] */ DWORD grfMode,
        /* [in] */ DWORD reserved1,
        /* [in] */ DWORD reserved2,
        /* [out] */ __RPC__deref_out_opt IStream **ppstm)
{
  return E_NOTIMPL;
}

STDMETHODIMP CGDriveShlExt::OpenStream(
        /* [string][in] */ const OLECHAR *pwcsName,
        /* [unique][in] */ void *reserved1,
        /* [in] */ DWORD grfMode,
        /* [in] */ DWORD reserved2,
        /* [out] */ IStream **ppstm)
{
  return E_NOTIMPL;
}

STDMETHODIMP CGDriveShlExt::CreateStorage(
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName,
        /* [in] */ DWORD grfMode,
        /* [in] */ DWORD reserved1,
        /* [in] */ DWORD reserved2,
        /* [out] */ __RPC__deref_out_opt IStorage **ppstg)
{
  return E_NOTIMPL;
}

STDMETHODIMP CGDriveShlExt::OpenStorage(
        /* [string][unique][in] */ __RPC__in_opt_string const OLECHAR *pwcsName,
        /* [unique][in] */ __RPC__in_opt IStorage *pstgPriority,
        /* [in] */ DWORD grfMode,
        /* [unique][in] */ __RPC__deref_opt_in_opt SNB snbExclude,
        /* [in] */ DWORD reserved,
        /* [out] */ __RPC__deref_out_opt IStorage **ppstg)
{
  return E_NOTIMPL;
}

STDMETHODIMP CGDriveShlExt::CopyTo(
        /* [in] */ DWORD ciidExclude,
        /* [size_is][unique][in] */ const IID *rgiidExclude,
        /* [annotation][unique][in] */
        __RPC__in_opt  SNB snbExclude,
        /* [unique][in] */ IStorage *pstgDest)
{
  return E_NOTIMPL;
}

STDMETHODIMP CGDriveShlExt::MoveElementTo(
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName,
        /* [unique][in] */ __RPC__in_opt IStorage *pstgDest,
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsNewName,
        /* [in] */ DWORD grfFlags)
{
  return E_NOTIMPL;
}

STDMETHODIMP CGDriveShlExt::EnumElements(
        /* [in] */ DWORD reserved1,
        /* [size_is][unique][in] */ void *reserved2,
        /* [in] */ DWORD reserved3,
        /* [out] */ IEnumSTATSTG **ppenum)
{
  return E_NOTIMPL;
}

STDMETHODIMP CGDriveShlExt::DestroyElement(
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName)
{
  return E_NOTIMPL;
}

STDMETHODIMP CGDriveShlExt::RenameElement(
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsOldName,
        /* [string][in] */ __RPC__in_string const OLECHAR *pwcsNewName)
{
  return E_NOTIMPL;
}

STDMETHODIMP CGDriveShlExt::SetElementTimes(
        /* [string][unique][in] */ __RPC__in_opt_string const OLECHAR *pwcsName,
        /* [unique][in] */ __RPC__in_opt const FILETIME *pctime,
        /* [unique][in] */ __RPC__in_opt const FILETIME *patime,
        /* [unique][in] */ __RPC__in_opt const FILETIME *pmtime)
{
  return E_NOTIMPL;
}

STDMETHODIMP CGDriveShlExt::SetClass(
        /* [in] */ __RPC__in REFCLSID clsid)
{
  return E_NOTIMPL;
}

STDMETHODIMP CGDriveShlExt::SetStateBits(
        /* [in] */ DWORD grfStateBits,
        /* [in] */ DWORD grfMask)
{
  return E_NOTIMPL;
}

STDMETHODIMP CGDriveShlExt::Commit(
    /* [in] */ DWORD grfCommitFlags)
{
  return E_NOTIMPL;
}

STDMETHODIMP CGDriveShlExt::Revert(void)
{
  return E_NOTIMPL;
}

STDMETHODIMP CGDriveShlExt::Stat(
    /* [out] */ __RPC__out STATSTG *pstatstg,
    /* [in] */ DWORD grfStatFlag)
{
  return E_NOTIMPL;
}

STDMETHODIMP CGDriveShlExt::IsDirty( void)
{
  return E_NOTIMPL;
}

STDMETHODIMP CGDriveShlExt::InitNew(
    /* [unique][in] */ __RPC__in_opt IStorage *pStg)
{
  return E_NOTIMPL;
}

STDMETHODIMP CGDriveShlExt::Load(
    /* [unique][in] */ __RPC__in_opt IStorage *pStg)
{
  return E_NOTIMPL;
}

STDMETHODIMP CGDriveShlExt::Save(
    /* [unique][in] */ __RPC__in_opt IStorage *pStgSave,
    /* [in] */ BOOL fSameAsLoad)
{
  return E_NOTIMPL;
}

STDMETHODIMP CGDriveShlExt::SaveCompleted(
    /* [unique][in] */ __RPC__in_opt IStorage *pStgNew)
{
  return E_NOTIMPL;
}

STDMETHODIMP CGDriveShlExt::HandsOffStorage( void)
{
  return E_NOTIMPL;
}

STDMETHODIMP CGDriveShlExt::GetObject(
    /* [in] */ __RPC__in LPOLESTR pszItem,
    /* [in] */ DWORD dwSpeedNeeded,
    /* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
    /* [in] */ __RPC__in REFIID riid,
    /* [iid_is][out] */ __RPC__deref_out_opt void **ppvObject)
{
  return E_NOTIMPL;
}

STDMETHODIMP CGDriveShlExt::GetObjectStorage(
    /* [in] */ __RPC__in LPOLESTR pszItem,
    /* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
    /* [in] */ __RPC__in REFIID riid,
    /* [iid_is][out] */ __RPC__deref_out_opt void **ppvStorage)
{
  return E_NOTIMPL;
}

STDMETHODIMP CGDriveShlExt::IsRunning(
    /* [in] */ __RPC__in LPOLESTR pszItem)
{
  return E_NOTIMPL;
}

STDMETHODIMP CGDriveShlExt::EnumObjects(
    /* [in] */ DWORD grfFlags,
    /* [out] */ __RPC__deref_out_opt IEnumUnknown **ppenum)
{
  return E_NOTIMPL;
}

STDMETHODIMP CGDriveShlExt::LockContainer(
    /* [in] */ BOOL fLock)
{
  return E_NOTIMPL;
}

STDMETHODIMP CGDriveShlExt::ParseDisplayName(
    /* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
    /* [in] */ __RPC__in LPOLESTR pszDisplayName,
    /* [out] */ __RPC__out ULONG *pchEaten,
    /* [out] */ __RPC__deref_out_opt IMoniker **ppmkOut)
{
  return E_NOTIMPL;
}
