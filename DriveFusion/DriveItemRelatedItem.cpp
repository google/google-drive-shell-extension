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
#include "DriveItemRelatedItem.h"
#include <sstream>
#include "DriveItemStream.h"

// {4F74D1CF-680C-4EA3-8020-4BDA6792DA3C}
const GUID GUID_UndocumentedStreamResource = { 0x4F74D1CF, 0x680C, 0x4EA3, 0x80, 0x20, 0x4B, 0xDA, 0x67, 0x92, 0xDA, 0x3C };

DWORD CDriveItemRelatedItem::_nextCookie = 0;

HRESULT CDriveItemRelatedItem::CreateInstanceReturnInterfaceTo(CGDriveShlExt* gDriveShlExt, REFIID riid, __deref_out void **ppv)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemRelatedItem::CreateInstance");

  *ppv = NULL;

  CComObject<CDriveItemRelatedItem> *psprfvcb;

  HRESULT hr = CComObject<CDriveItemRelatedItem>::CreateInstance(&psprfvcb);

  if (!SUCCEEDED(hr))
  {
    Log::WriteOutput(LogType::Error, L"CComObject<CDriveItemRelatedItem>::CreateInstance returned hr=%d", hr);
  }
  else
  {
    psprfvcb->AddRef();
    hr = psprfvcb->_Initialize(gDriveShlExt);

    if (SUCCEEDED(hr))
    {
      hr = psprfvcb->QueryInterface(riid, ppv);

      if (!SUCCEEDED(hr))
      {
        Log::LogUnknownIID(riid, L"CDriveItemRelatedItem::CreateInstanceReturnInterfaceTo()");
      }
    }

    psprfvcb->Release();
  }

  return hr;
}

void CDriveItemRelatedItem::FinalRelease()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemRelatedItem::FinalRelease()");

  _gDriveShlExt->Release();
}

HRESULT CDriveItemRelatedItem::_Initialize(CGDriveShlExt* gDriveShlExt)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemRelatedItem::_Initialize");

  _gDriveShlExt = gDriveShlExt;

  _gDriveShlExt->AddRef();

  return S_OK;
}

STDMETHODIMP CDriveItemRelatedItem::GetItem(__RPC__deref_out_opt IShellItem **ppsi)
{
  try
  {
    Log::WriteOutput(LogType::Debug, L"CDriveItemRelatedItem::GetItem()");

    CComHeapPtr<ITEMIDLIST_ABSOLUTE> pidl;

    HRESULT hr = _gDriveShlExt->GetCurFolder(&pidl);

    if (SUCCEEDED(hr))
    {
      HRESULT hr = SHCreateItemFromIDList(pidl, IID_PPV_ARGS(ppsi));

      if (!SUCCEEDED(hr))
      {
        Log::WriteOutput(LogType::Error, L"SHCreateItemFromIDList returned hr=%d", hr);
      }
    }

    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CDriveItemRelatedItem::GetItem");

    return E_FAIL;
  }
}

STDMETHODIMP CDriveItemRelatedItem::GetItemIDList(__RPC__deref_out_opt PIDLIST_ABSOLUTE *ppidl)
{
  try
  {
    Log::WriteOutput(LogType::Debug, L"CDriveItemRelatedItem::GetItemIDList()");

    return _gDriveShlExt->GetCurFolder(ppidl);
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CDriveItemRelatedItem::GetItemIDList");

    return E_FAIL;
  }
}

STDMETHODIMP CDriveItemRelatedItem::Advise(__in  ITransferAdviseSink *psink, __out  DWORD *pdwCookie)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemRelatedItem::Advise()");

  _sinkMap.insert(std::pair<DWORD, ITransferAdviseSink*>(_nextCookie, psink));

  *pdwCookie = _nextCookie;

  if (_nextCookie == (DWORD)std::numeric_limits<DWORD>::max)
  {
    _nextCookie = 0;
  }
  else
  {
    _nextCookie++;
  }

  return S_OK;
}

STDMETHODIMP CDriveItemRelatedItem::Unadvise(DWORD dwCookie)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemRelatedItem::Unadvise()");

  AdviseSinkMap::iterator it = _sinkMap.find(dwCookie);

  if (it != _sinkMap.end())
  {
    _sinkMap.erase(it);
  }

  return S_OK;
}

STDMETHODIMP CDriveItemRelatedItem::CreateItem(__in  LPCWSTR pszName, __in  DWORD dwAttributes, __in  ULONGLONG ullSize, __in  TRANSFER_SOURCE_FLAGS flags, __in  REFIID riidItem, __deref_out  void **ppvItem, __in  REFIID riidResources, __deref_out  void **ppvResources)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemRelatedItem::CreateItem()");

  if (ullSize != 0)
  {
    Log::WriteOutput(LogType::Error, L"CDriveItemRelatedItem::CreateItem(), unhandled case ullSize != 0");
  }

  if ((flags & TSF_MOVE_AS_COPY_DELETE) > 0)
  {
    Log::WriteOutput(LogType::Error, L"CDriveItemRelatedItem::CreateItem(), unhandled case TSF_MOVE_AS_COPY_DELETE");
  }

  CComHeapPtr<ITEMID_CHILD> childpidl;

  CComPtr<IBindCtx> pbc;
  BIND_OPTS options;
  ZeroMemory(&options, sizeof(BIND_OPTS));
  options.cbStruct = sizeof(BIND_OPTS);
  options.grfFlags = BIND_MAYBOTHERUSER;
  options.grfMode = STGM_CREATE;

  HRESULT hr = CreateBindCtx(0, &pbc);

  if (SUCCEEDED(hr))
  {
    hr = pbc->SetBindOptions(&options);

    if (SUCCEEDED(hr))
    {
      WCHAR name[MAX_PATH];
      hr = StringCchCopy(name, MAX_PATH, pszName);

      if (SUCCEEDED(hr))
      {
        hr = _gDriveShlExt->ParseDisplayName(NULL, pbc, name, NULL, &childpidl, &dwAttributes); // create a new file at google

        if (((flags & TSF_OVERWRITE_EXIST) == TSF_OVERWRITE_EXIST) && hr == HRESULT_FROM_WIN32(ERROR_FILE_EXISTS))
        {
          hr = _gDriveShlExt->ParseDisplayName(NULL, NULL, name, NULL, &childpidl, &dwAttributes); // get the existing file
        }

        if (SUCCEEDED(hr))
        {
          if (riidItem == IID_IShellItem)
          {
            CComPtr<IIdentityName> idName;

            hr = _gDriveShlExt->BindToObject(childpidl, NULL, IID_PPV_ARGS(&idName)); // returns an instance of CDriveItemRelatedItem

            if (SUCCEEDED(hr))
            {
              CComPtr<IShellItem> shellItem;

              hr = idName->GetItem(&shellItem);

              if (SUCCEEDED(hr))
              {
                hr = shellItem->QueryInterface(riidItem, ppvItem);

                if (SUCCEEDED(hr))
                {
                  hr = idName->QueryInterface(riidResources, ppvResources); // this works cause I know idName is an instance of CDriveItemRelatedItem, which also impliments IShellItemResources
                }

                shellItem.Release();
              }
              idName.Release();
            }
          }
          else
          {
            Log::LogUnknownIID(riidItem, L"CDriveItemRelatedItem::CreateItem()");
            hr = E_INVALIDARG;
          }
        }
      }
    }
    pbc.Release();
  }

  return hr;
}

// ITransferSource
STDMETHODIMP CDriveItemRelatedItem::SetProperties(__in  IPropertyChangeArray * /*pproparray*/)
{
  Log::WriteOutput(LogType::Error, L"CDriveItemRelatedItem::SetProperties E_NOTIMPL");

  return E_NOTIMPL;
}

// ITransferSource
STDMETHODIMP CDriveItemRelatedItem::OpenItem(__in  IShellItem *psi, __in  TRANSFER_SOURCE_FLAGS /*flags*/, __in  REFIID riid, __deref_out  void **ppv)
{
  Log::WriteOutput(LogType::Warning, L"CDriveItemRelatedItem::OpenItem()");

  HRESULT hr = E_NOINTERFACE;

  if (riid == IID_IShellItemResources)
  {
    hr = psi->BindToHandler(NULL, BHID_SFViewObject, riid, ppv);
  }
  else
  {
    Log::LogUnknownIID(riid, L"CDriveItemRelatedItem::OpenItem()");
  }

  return hr;

}

// ITransferSource
STDMETHODIMP CDriveItemRelatedItem::MoveItem(__in  IShellItem * /*psi*/, __in  IShellItem * /*psiParentDst*/, __in  LPCWSTR /*pszNameDst*/, __in TRANSFER_SOURCE_FLAGS /*flags*/, __deref_out  IShellItem ** /*ppsiNew*/)
{
  Log::WriteOutput(LogType::Warning, L"CDriveItemRelatedItem::MoveItem E_NOTIMPL");
  return E_NOTIMPL;
  // Cut from Drive, Paste to FileSystem, calls this function, but it doesn't seem we need to implement it
}

// ITransferSource
STDMETHODIMP CDriveItemRelatedItem::RecycleItem(__in  IShellItem *psiSource, __in  IShellItem * /*psiParentDest*/, __in  TRANSFER_SOURCE_FLAGS flags, __deref_out  IShellItem **ppsiNewDest)
{
  Log::WriteOutput(LogType::Warning, L"CDriveItemRelatedItem::RecycleItem Skipping recycle, going to trash in on google instead");

  HRESULT hr = RemoveItem(psiSource, flags);

  *ppsiNewDest = NULL;

  return hr;
}

// ITransferSource
STDMETHODIMP CDriveItemRelatedItem::RemoveItem(__in  IShellItem *psiSource, __in  TRANSFER_SOURCE_FLAGS /*flags*/)
{
  Log::WriteOutput(LogType::Warning, L"CDriveItemRelatedItem::RemoveItem()");

  HRESULT hr = S_OK;

  std::unique_ptr<IIdentityName> idName;
  std::vector<std::wstring> ids;
  std::vector<std::wstring> deletedIds;

  {
    IIdentityName* tmpIdName = nullptr;
    hr = psiSource->BindToHandler(NULL, BHID_SFViewObject, IID_PPV_ARGS(&tmpIdName));
    idName.reset(tmpIdName);
  }

  if (SUCCEEDED(hr))
  {
    CIdList pidl;
    {
      LPITEMIDLIST tmpPidl = nullptr;
      hr = idName->GetItemIDList(&tmpPidl);
      pidl.Reset(tmpPidl);
    }

    if (SUCCEEDED(hr))
    {
      CDriveItem item;

      hr = _gDriveShlExt->GetDriveItemFromIDList(pidl, true, false, item);

      if (SUCCEEDED(hr))
      {
        ids.push_back(item.Id());

        if(!_gDriveShlExt->FileManager()->DeleteFiles(ids, &deletedIds))
        {
          std::wstring message = L"CDriveItemRelatedItem::RemoveItem() Cannot delete item(s) - " + _gDriveShlExt->FileManager()->ErrorMessage();

          Log::WriteOutput(LogType::Error, message.c_str());
        }
        else
        {
          if (deletedIds.size() == 1) // we only had one item, so if there is an item, then it must have been deleted ok
          {
            if (item.IsFile())
            {
              SHChangeNotify(SHCNE_DELETE, SHCNF_IDLIST | SHCNF_FLUSH, pidl, NULL);
            }
            else
            {
              SHChangeNotify(SHCNE_RMDIR, SHCNF_IDLIST | SHCNF_FLUSH, pidl, NULL);
            }

            std::unique_ptr<IShellItem> parent;
            {
              IShellItem* tmpParent = nullptr;
              hr = psiSource->GetParent(&tmpParent);
              parent.reset(tmpParent);
            }

            if (SUCCEEDED(hr))
            {
              std::unique_ptr<IIdentityName> parentIdName;
              {
                IIdentityName* tmpParentIdName = nullptr;
                hr = parent->BindToHandler(NULL, BHID_SFViewObject, IID_PPV_ARGS(&tmpParentIdName));
                parentIdName.reset(tmpParentIdName);
              }

              if (SUCCEEDED(hr))
              {
                CIdList pidl;
                {
                  LPITEMIDLIST tmpPidl = nullptr;
                  hr = parentIdName->GetItemIDList(&tmpPidl);
                  pidl.Reset(tmpPidl);
                }

                if (SUCCEEDED(hr))
                {
                  SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_IDLIST | SHCNF_FLUSH, pidl, NULL);
                }
              }
            }
          }
        }
      }
    }
  }

  return hr;
}

// ITransferSource
STDMETHODIMP CDriveItemRelatedItem::RenameItem(__in  IShellItem * /*psiSource*/, __in  LPCWSTR /*pszNewName*/, __in  TRANSFER_SOURCE_FLAGS /*flags*/, __deref_out  IShellItem ** /*ppsiNewDest*/)
{
  Log::WriteOutput(LogType::Error, L"CDriveItemRelatedItem::RenameItem E_NOTIMPL");

  return E_NOTIMPL;
}

// ITransferSource
STDMETHODIMP CDriveItemRelatedItem::LinkItem(__in  IShellItem * /*psiSource*/, __in  IShellItem * /*psiParentDest*/, __in_opt  LPCWSTR /*pszNewName*/, __in  TRANSFER_SOURCE_FLAGS /*flags*/, __deref_out  IShellItem ** /*ppsiNewDest*/)
{
  Log::WriteOutput(LogType::Error, L"CDriveItemRelatedItem::LinkItem E_NOTIMPL");

  return E_NOTIMPL;
}

// ITransferSource
STDMETHODIMP CDriveItemRelatedItem::ApplyPropertiesToItem(__in  IShellItem * /*psiSource*/, __deref_out  IShellItem ** /*ppsiNew*/)
{
  Log::WriteOutput(LogType::Error, L"CDriveItemRelatedItem::ApplyPropertiesToItem E_NOTIMPL");

  return E_NOTIMPL;
}

// ITransferSource
STDMETHODIMP CDriveItemRelatedItem::GetDefaultDestinationName(__in  IShellItem *psiSource, __in  IShellItem * /*psiParentDest*/, __deref_out  LPWSTR *ppszDestinationName)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemRelatedItem::GetDefaultDestinationName()");

  HRESULT hr = psiSource->GetDisplayName(SIGDN_NORMALDISPLAY, ppszDestinationName);

  return hr;
}

// ITransferSource
STDMETHODIMP CDriveItemRelatedItem::EnterFolder(__in  IShellItem * /*psiChildFolderDest*/)
{
  Log::WriteOutput(LogType::Warning, L"CDriveItemRelatedItem::EnterFolder()");

  // This method is called when beginning to process a folder or subfolder in a recursive operation. For instance, when a source folder is copied to a destination folder, method ITransferSource::EnterFolder should be called with psiChildFolderDest set to the destination folder.

  // Nothing for us to do right now

  return S_OK;
}

// ITransferSource
STDMETHODIMP CDriveItemRelatedItem::LeaveFolder(__in  IShellItem * /*psiChildFolderDest*/)
{
  Log::WriteOutput(LogType::Warning, L"CDriveItemRelatedItem::LeaveFolder()");

  // This method is called at the end of recursive file operations on a destination folder.

  // Nothing for us to do right now

  return S_OK;
}

STDMETHODIMP CDriveItemRelatedItem::GetAttributes(__RPC__out DWORD *pdwAttributes)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemRelatedItem::GetAttributes()");

  CDriveItem item;

  HRESULT hr = _gDriveShlExt->GetDriveItemFromID(_gDriveShlExt->Id(), false, false, item);

  if (SUCCEEDED(hr))
  {
    *pdwAttributes = item.FileAttributes();
  }

  return hr;
}

STDMETHODIMP CDriveItemRelatedItem::GetSize(__RPC__out ULONGLONG *pullSize)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemRelatedItem::GetSize()");

  CDriveItem item;

  HRESULT hr = _gDriveShlExt->GetDriveItemFromID(_gDriveShlExt->Id(), false, false, item);

  if (SUCCEEDED(hr))
  {
    *pullSize = item.FileInfo()->FileSize;
  }

  return hr;
}

STDMETHODIMP CDriveItemRelatedItem::GetTimes(__RPC__out FILETIME *pftCreation, __RPC__out FILETIME *pftWrite, __RPC__out FILETIME *pftAccess)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemRelatedItem::GetTimes()");

  CDriveItem item;
  WIN32_FIND_DATA times;

  HRESULT hr = _gDriveShlExt->GetDriveItemFromID(_gDriveShlExt->Id(), false, false, item);

  if (SUCCEEDED(hr))
  {
    if (item.TryWIN32FindData(times))
    {
      *pftCreation = times.ftCreationTime;
      *pftWrite = times.ftLastWriteTime;
      *pftAccess = times.ftLastAccessTime;
    }
    else
    {
      hr = E_FAIL;
    }
  }

  return hr;
}

STDMETHODIMP CDriveItemRelatedItem::SetTimes(__RPC__in_opt const FILETIME * /*pftCreation*/, __RPC__in_opt const FILETIME * /*pftWrite*/, __RPC__in_opt const FILETIME * /*pftAccess*/)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemRelatedItem::SetTimes()");

  // We don't really care what this does, because Google and the filesystem is going to handle it

  return S_OK;
}

STDMETHODIMP CDriveItemRelatedItem::GetResourceDescription(__RPC__in const SHELL_ITEM_RESOURCE * /*pcsir*/, __RPC__deref_out_opt_string LPWSTR * /*ppszDescription*/)
{
  Log::WriteOutput(LogType::Error, L"CDriveItemRelatedItem::GetResourceDescription E_NOTIMPL");

  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemRelatedItem::EnumResources(__RPC__deref_out_opt IEnumResources **ppenumr)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemRelatedItem::EnumResources()");

  CSimpleArray<SHELL_ITEM_RESOURCE> aList;

  SHELL_ITEM_RESOURCE FileStream = { GUID_UndocumentedStreamResource, L"" };

  aList.Add(FileStream);

  typedef CComEnum< IEnumResources, &IID_IEnumResources, SHELL_ITEM_RESOURCE, _Copy<SHELL_ITEM_RESOURCE> > CEnumResources;

  CComObject<CEnumResources>* pEnumResources;

  HRESULT hr = CComObject<CEnumResources>::CreateInstance(&pEnumResources);

  if (SUCCEEDED(hr))
  {
    hr = pEnumResources->Init(aList.GetData(), aList.GetData() + aList.GetSize(), NULL, AtlFlagCopy);

    if (SUCCEEDED(hr))
    {
      hr = pEnumResources->QueryInterface(IID_PPV_ARGS(ppenumr));
    }
  }

  return hr;
}

STDMETHODIMP CDriveItemRelatedItem::SupportsResource(__RPC__in const SHELL_ITEM_RESOURCE *pcsir)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemRelatedItem::SupportsResource()");

  if(pcsir->guidType == GUID_UndocumentedStreamResource)
  {
    return S_OK;
  }

  return E_NOINTERFACE;
}

STDMETHODIMP CDriveItemRelatedItem::OpenResource(__RPC__in const SHELL_ITEM_RESOURCE *pcsir, __RPC__in REFIID riid, __RPC__deref_out_opt void **ppv)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemRelatedItem::OpenResource()");

  HRESULT hr = E_NOINTERFACE;

  if(pcsir->guidType == GUID_UndocumentedStreamResource )
  {
    CDriveItem item;

    hr = _gDriveShlExt->GetDriveItemFromID(_gDriveShlExt->Id(), false, false, item);

    if (SUCCEEDED(hr))
    {
      if (riid == IID_IUnknown)
      {
        BIND_OPTS options;
        ZeroMemory(&options, sizeof(BIND_OPTS));
        options.cbStruct = sizeof(BIND_OPTS);
        options.grfFlags = BIND_MAYBOTHERUSER;
        // other properties are zero from ZeroMemory op above

        hr = CDriveItemStream::CreateInstanceReturnInterfaceTo(item, options, riid, ppv);
      }
      else
      {
        Log::LogUnknownIID(riid, L"CDriveItemRelatedItem::OpenResource()");
        hr = E_INVALIDARG;
      }
    }
  }
  else
  {
    Log::LogUnknownIID(riid, L"CDriveItemRelatedItem::OpenResource()");
  }

  return hr;
}

STDMETHODIMP CDriveItemRelatedItem::CreateResource(__RPC__in const SHELL_ITEM_RESOURCE *pcsir, __RPC__in REFIID riid, __RPC__deref_out_opt void **ppv)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemRelatedItem::CreateResource()");

  HRESULT hr = E_NOINTERFACE;

  if(pcsir->guidType == GUID_UndocumentedStreamResource )
  {
    CDriveItem item;

    hr = _gDriveShlExt->GetDriveItemFromID(_gDriveShlExt->Id(), false, false, item);

    if (SUCCEEDED(hr))
    {
      if (riid == IID_IUnknown)
      {
        BIND_OPTS options;
        ZeroMemory(&options, sizeof(BIND_OPTS));
        options.cbStruct = sizeof(BIND_OPTS);
        options.grfFlags = BIND_MAYBOTHERUSER;
        options.grfMode = STGM_WRITE;
        // other properties are zero from ZeroMemory op above

        hr = CDriveItemStream::CreateInstanceReturnInterfaceTo(item, options, riid, ppv);
      }
      else
      {
        Log::LogUnknownIID(riid, L"CDriveItemRelatedItem::CreateResource()");
        hr = E_INVALIDARG;
      }
    }
  }
  else
  {
    Log::LogUnknownIID(riid, L"CDriveItemRelatedItem::CreateResource()");
  }

  return hr;
}

STDMETHODIMP CDriveItemRelatedItem::MarkForDelete(void)
{
  Log::WriteOutput(LogType::Error, L"CDriveItemRelatedItem::MarkForDelete E_NOTIMPL");

  return E_NOTIMPL;
}
