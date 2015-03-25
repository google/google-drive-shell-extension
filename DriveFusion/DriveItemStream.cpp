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

HRESULT CDriveItemStream::CreateInstanceReturnInterfaceTo(CDriveItem& item, __in const BIND_OPTS& options, REFIID riid, __deref_out void **ppv)
{
  Log::WriteOutput(LogType::Information, L"CDriveItemStream::CreateInstance()");

  *ppv = NULL;

  HRESULT hr = E_NOINTERFACE;

  CComObject<CDriveItemStream> *psprfvcb;

  hr = CComObject<CDriveItemStream>::CreateInstance(&psprfvcb);

  if (!SUCCEEDED(hr))
  {
    Log::WriteOutput(LogType::Error, L"CComObject<CDriveItemStream>::CreateInstance returned hr=%d", hr);
  }
  else
  {
    psprfvcb->AddRef();
    hr = psprfvcb->_Initialize(item, options.grfMode);

    if (SUCCEEDED(hr))
    {
      hr = psprfvcb->QueryInterface(riid, ppv);

      if (!SUCCEEDED(hr))
      {
        Log::LogUnknownIID(riid, L"CDriveItemStream::CreateInstanceReturnInterfaceTo");
      }
    }

    psprfvcb->Release();
  }

  return hr;
}

void CDriveItemStream::FinalRelease()
{
  Log::WriteOutput(LogType::Information, L"CDriveItemStream::FinalRelease()");

  _CloseWithError(S_OK);
}

HRESULT CDriveItemStream::_Initialize(CDriveItem& item, DWORD grfMode)
{
  Log::WriteOutput(LogType::Information, L"CDriveItemStream::_Initialize(FileInfo* fileInfo)");

  HRESULT hr = S_OK;

  _item.FileInfo(item.FileInfo());
  _isFileDownloaded = false;
  _stgmMode = grfMode;
  _lockType = 0;
  _seekPos.QuadPart = 0;
  _file = NULL;

  return hr;
}

HRESULT CDriveItemStream::_DownloadFile()
{
  Log::WriteOutput(LogType::Information, L"CDriveItemStream::_DownloadFile()");

  HRESULT hr = S_OK;

  if (!_isFileDownloaded)
  {
    hr = CGDriveShlExt::DoDownload(_item.FileInfo());

    _isFileDownloaded = SUCCEEDED(hr);
  }

  return hr;
}

HRESULT CDriveItemStream::_Open()
{
  Log::WriteOutput(LogType::Information, L"CDriveItemStream::_Open()");

  HRESULT hr = S_OK;

  if (_item.IsFolder())
  {
    return S_OK;
  }

  if (!_isFileDownloaded)
  {
    hr = _DownloadFile();
  }

  if (SUCCEEDED(hr))
  {
    if (_file == NULL)
    {
      ACCESS_MASK openMode = NULL;

      if ((_stgmMode == STGM_READ) || (_stgmMode & STGM_READWRITE))
      {
        openMode |= GENERIC_READ;
      }

      if ((_stgmMode & STGM_WRITE) || (_stgmMode & STGM_READWRITE))
      {
        openMode |= GENERIC_WRITE;
      }

      _file = CreateFile(_item.FileInfo()->FilePath().c_str(), openMode, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

      if (_file == INVALID_HANDLE_VALUE || _file == NULL)
      {
        hr = Log::HRFromWin32(false, L"CDriveItemStream::_Open()");
        _file = NULL;
      }

      if (SUCCEEDED(hr))
      {
        hr = _Seek(_seekPos, FILE_BEGIN, NULL);
      }
    }
  }

  return hr;
}

HRESULT CDriveItemStream::_CloseWithError(HRESULT previousHR)
{
  Log::WriteOutput(LogType::Information, L"CDriveItemStream::_Close");

  HRESULT hr = S_OK;

  if (_file != NULL)
  {
    hr = Log::HRFromWin32( CloseHandle(_file) , L"CDriveItemStream::_Close()");

    if (SUCCEEDED(hr))
    {
      _file = NULL;
    }
  }

  if (!SUCCEEDED(previousHR))
  {
    return previousHR;
  }

  return hr;
}

HRESULT CDriveItemStream::_Read(__out void *pv, __in ULONG cb, __out ULONG *pcbRead)
{
  HRESULT hr = S_OK;

  if (_file == NULL)
  {
    return E_FAIL;
  }

  if (pv == NULL)
    return STG_E_INVALIDPOINTER;

  DWORD bRead = 0;

  hr = Log::HRFromWin32(ReadFile(_file, pv, cb, &bRead, NULL), L"CDriveItemStream::Read()");

  if (SUCCEEDED(hr) && pcbRead != NULL)
  {
    if (pcbRead != NULL)
    {
      *pcbRead = bRead;
    }

    _seekPos.QuadPart += bRead;
  }

  return hr;
}

HRESULT CDriveItemStream::_Seek(__in LARGE_INTEGER dlibMove, __in DWORD dwOrigin, __out_opt  ULARGE_INTEGER *plibNewPosition)
{
  if (_file == NULL)
  {
    return E_FAIL;
  }

  LARGE_INTEGER resultPos;

  HRESULT hr = Log::HRFromWin32(SetFilePointerEx(_file, dlibMove, &resultPos, dwOrigin), L"CDriveItemStream::Read() set");

  if (plibNewPosition != NULL)
    plibNewPosition->QuadPart = resultPos.QuadPart;

  if (SUCCEEDED(hr))
  {
    _seekPos.QuadPart = resultPos.QuadPart;
  }

  return hr;
}

HRESULT CDriveItemStream::_SetSize(__in ULARGE_INTEGER libNewSize)
{
  if (_file == NULL)
  {
    return E_FAIL;
  }

  LARGE_INTEGER newSize;
  newSize.QuadPart = libNewSize.QuadPart;

  LARGE_INTEGER currentPos;
  currentPos.QuadPart = _seekPos.QuadPart;

  HRESULT hr = _Seek(newSize, FILE_BEGIN, NULL); // seek to extend/truncate position

  if (SUCCEEDED(hr))
  {
    hr = Log::HRFromWin32(SetEndOfFile(_file), L"CDriveItemStream::SetSize()"); // resize file

    if (SUCCEEDED(hr))
    {
      hr = _Seek(currentPos, FILE_BEGIN, NULL); // return pointer to original position
    }
  }

  return hr;
}

HRESULT CDriveItemStream::_Write(__in void const *pv, __in ULONG cb, __out ULONG *pcbWritten)
{
  if (_file == NULL)
  {
    return E_FAIL;
  }

  HRESULT hr = S_OK;

  if (pv == NULL)
    return STG_E_INVALIDPOINTER;

  DWORD bWritten;

  hr = Log::HRFromWin32(WriteFile(_file, pv, cb, &bWritten, NULL), L"CDriveItemStream::Write()");

  if (SUCCEEDED(hr))
  {
    if (pcbWritten != NULL)
    {
      *pcbWritten = bWritten;
    }

    _seekPos.QuadPart += bWritten;
  }

  return hr;
}

STDMETHODIMP CDriveItemStream::Clone(__out __RPC__deref_out_opt IStream ** /*ppstm*/)
{
  Log::WriteOutput(LogType::Error, L"CDriveItemStream::Clone() E_NOTIMPL");

  // Creates a new stream object that references the same bytes as the original stream but provides a separate seek pointer to those bytes.

  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::Commit(__in DWORD /*grfCommitFlags*/)
{
  Log::WriteOutput(LogType::Warning, L"CDriveItemStream::Commit() doesn't do anyting, returning S_OK");

  return S_OK;
}

STDMETHODIMP CDriveItemStream::CopyTo(__in IStream * /*pstm*/, __in ULARGE_INTEGER /*cb*/, __out_opt  ULARGE_INTEGER * /*pcbRead*/, __out_opt  ULARGE_INTEGER * /*pcbWritten*/)
{
  Log::WriteOutput(LogType::Error, L"CDriveItemStream::CopyTo() E_NOTIMPL");

  // Copies a specified number of bytes from the current seek pointer in the stream to the current seek pointer in another stream.

  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::LockRegion(__in ULARGE_INTEGER /*libOffset*/, __in ULARGE_INTEGER /*cb*/, __in DWORD /*dwLockType*/)
{
  Log::WriteOutput(LogType::Error, L"CDriveItemStream::LockRegion() E_NOTIMPL");

  // Restricts access to a specified range of bytes in the stream. Supporting this functionality is optional since some file systems do not provide it.

  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::Read(__out void *pv, __in ULONG cb, __out ULONG *pcbRead)
{
  Log::WriteOutput(LogType::Warning, L"CDriveItemStream::Read");

  // Reads a specified number of bytes from the stream object into memory starting at the current seek pointer. (Inherited from ISequentialStream)

  // pv [out]
  // A pointer to the buffer which the stream data is read into.

  // cb [in]
  // The number of bytes of data to read from the stream object.

  // pcbRead [out]
  // A pointer to a ULONG variable that receives the actual number of bytes read from the stream object.

  HRESULT hr = _Open();

  if (SUCCEEDED(hr))
  {
    hr = _Read(pv, cb, pcbRead);
  }
  else
  {
    pv = NULL;
  }

  return _CloseWithError(hr);
}

STDMETHODIMP CDriveItemStream::Revert()
{
  Log::WriteOutput(LogType::Error, L"CDriveItemStream::Revert() E_NOTIMPL");

  // Discards all changes that have been made to a transacted stream since the last call to IStream::Commit.

  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::Seek(__in LARGE_INTEGER dlibMove, __in DWORD dwOrigin, __out_opt  ULARGE_INTEGER *plibNewPosition)
{
  Log::WriteOutput(LogType::Warning, L"CDriveItemStream::Seek");

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

  HRESULT hr = _Open();

  if (SUCCEEDED(hr))
  {
    hr = _Seek(dlibMove, dwOrigin, plibNewPosition);
  }

  return _CloseWithError(hr);
}

STDMETHODIMP CDriveItemStream::SetSize(__in ULARGE_INTEGER libNewSize)
{
  Log::WriteOutput(LogType::Information, L"CDriveItemStream::SetSize()");
  // Changes the size of the stream object.
  HRESULT hr = _Open();

  if (SUCCEEDED(hr))
  {
    hr = _SetSize(libNewSize);
  }

  return _CloseWithError(hr);
}

STDMETHODIMP CDriveItemStream::Stat(__out __RPC__out STATSTG *pstatstg, __in DWORD grfStatFlag)
{
  Log::WriteOutput(LogType::Warning, L"CDriveItemStream::Stat");

  // Retrieves the STATSTG structure for this stream.

  if (grfStatFlag != STATFLAG_NONAME)
  {
    HRESULT hr = SHStrDup(_item.ItemName().c_str(), &pstatstg->pwcsName);

    if (!SUCCEEDED(hr))
    {
      Log::WriteOutput(LogType::Error, L"CDriveItemStream::Stat SHStrDup returned hr=%d", hr);

      return hr;
    }
  }

  pstatstg->type = STGTY_STREAM;
  pstatstg->cbSize = _StatFileSize();
  pstatstg->grfMode = _stgmMode;
  pstatstg->grfLocksSupported = _lockType;
  pstatstg->clsid = CLSID_NULL;
  pstatstg->grfStateBits = 0;
  pstatstg->reserved = 0;


  if(_StatFileTimeModified(pstatstg->mtime) &&
    _StatFileTimeCreated(pstatstg->ctime) &&
    _StatFileTimeAccessed(pstatstg->atime))
  {
    return S_OK;
  }
  else
  {
    Log::WriteOutput(LogType::Error, L"CDriveItemStream::Stat filed to get file times");
    return E_FAIL;
  }
}

ULARGE_INTEGER CDriveItemStream::_StatFileSize()
{
  Log::WriteOutput(LogType::Warning, L"CDriveItemStream::_StatFileSize()");

  ULARGE_INTEGER result;
  ZeroMemory(&result, sizeof(ULARGE_INTEGER));

  result.QuadPart = _item.FileInfo()->FileSize;

  return result;
}

BOOL CDriveItemStream::_StatFileTimeModified(FILETIME& fi)
{
  Log::WriteOutput(LogType::Information, L"CDriveItemStream::_StatFileTimeModified()");

  return FileInfo::TryConvertDateStringToFILETIME(_item.DateModified(), fi);
}

BOOL CDriveItemStream::_StatFileTimeCreated(FILETIME& fi)
{
  Log::WriteOutput(LogType::Information, L"CDriveItemStream::_StatFileTimeCreated()");

  return FileInfo::TryConvertDateStringToFILETIME(_item.DateCreated(), fi);
}

BOOL CDriveItemStream::_StatFileTimeAccessed(FILETIME& fi)
{
  Log::WriteOutput(LogType::Information, L"CDriveItemStream::_StatFileTimeAccessed()");

  return FileInfo::TryConvertDateStringToFILETIME(_item.DateAccessed(), fi);
}

STDMETHODIMP CDriveItemStream::UnlockRegion(__in ULARGE_INTEGER /*libOffset*/, __in ULARGE_INTEGER /*cb*/, __in DWORD /*dwLockType*/)
{
  Log::WriteOutput(LogType::Error, L"CDriveItemStream::UnlockRegion() E_NOTIMPL");

  // Removes the access restriction on a range of bytes previously restricted with IStream::LockRegion.

  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::Write(__in void const *pv, __in ULONG cb, __out ULONG *pcbWritten)
{
  Log::WriteOutput(LogType::Information, L"CDriveItemStream::Write()");

  // Writes a specified number of bytes into the stream object starting at the current seek pointer. (Inherited from ISequentialStream)

  HRESULT hr = _Open();

  if (SUCCEEDED(hr))
  {
    hr = _Write(pv, cb, pcbWritten);
  }

  return _CloseWithError(hr);
}

STDMETHODIMP CDriveItemStream::GetClassID(__out CLSID * /*pclsid*/)
{
  Log::WriteOutput(LogType::Error, L"CDriveItemStream::GetClassID() E_NOTIMPL");
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::IsDirty()
{
  Log::WriteOutput(LogType::Error, L"CDriveItemStream::IsDirty() E_NOTIMPL");
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::Load(__RPC__in_opt IStream * /*pStm*/)
{
  Log::WriteOutput(LogType::Error, L"CDriveItemStream::Load() E_NOTIMPL");
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::Save(__RPC__in_opt IStream * /*pStm*/, BOOL /*fClearDirty*/)
{
  Log::WriteOutput(LogType::Error, L"CDriveItemStream::Save() E_NOTIMPL");
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::GetSizeMax(__RPC__out ULARGE_INTEGER * /*pcbSize*/)
{
  Log::WriteOutput(LogType::Error, L"CDriveItemStream::GetSizeMax() E_NOTIMPL");
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::InitNew()
{
  Log::WriteOutput(LogType::Error, L"CDriveItemStream::InitNew() E_NOTIMPL");
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::Load(__RPC__in LPCOLESTR /*pszFileName*/, DWORD /*dwMode*/)
{
  Log::WriteOutput(LogType::Error, L"CDriveItemStream::Load() E_NOTIMPL");
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::Save(__RPC__in_opt LPCOLESTR /*pszFileName*/, BOOL /*fRemember*/)
{
  Log::WriteOutput(LogType::Error, L"CDriveItemStream::Save() E_NOTIMPL");
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::SaveCompleted(__RPC__in_opt LPCOLESTR /*pszFileName*/)
{
  Log::WriteOutput(LogType::Error, L"CDriveItemStream::SaveCompleted() E_NOTIMPL");
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemStream::GetCurFile(__RPC__deref_out_opt LPOLESTR * /*ppszFileName*/)
{
  Log::WriteOutput(LogType::Error, L"CDriveItemStream::GetCurFile() E_NOTIMPL");
  return E_NOTIMPL;
}
