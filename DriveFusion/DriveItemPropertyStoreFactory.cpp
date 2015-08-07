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
#include "DriveItemPropertyStoreFactory.h"
#include "DriveItemPropertyStore.h"

HRESULT CDriveItemPropertyStoreFactory::CreateInstanceReturnInterfaceTo(CGDriveShlExt* gDriveShlExt, PCUITEMID_CHILD pidl, REFIID riid, __deref_out void **ppv)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemPropertyStoreFactory::CreateInstance()");

  *ppv = NULL;

  CComObject<CDriveItemPropertyStoreFactory> *psprpsf;

  HRESULT hr = CComObject<CDriveItemPropertyStoreFactory>::CreateInstance(&psprpsf);

  if (!SUCCEEDED(hr))
  {
    Log::WriteOutput(LogType::Error, L"CComObject<CDriveItemPropertyStoreFactory>::CreateInstance returned hr=%d", hr);
  }
  else
  {
    psprpsf->AddRef();
    hr = psprpsf->_Initialize(gDriveShlExt, pidl);

    if (SUCCEEDED(hr))
    {
      hr = psprpsf->QueryInterface(riid, ppv);

      if (!SUCCEEDED(hr))
      {
        Log::LogUnknownIID(riid, L"CDriveItemPropertyStoreFactory::CreateInstanceReturnInterfaceTo()");
      }
    }

    psprpsf->Release();
  }

  return hr;
}

void CDriveItemPropertyStoreFactory::FinalRelease()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemPropertyStoreFactory::FinalRelease()");

  if (_gDriveShlExt != NULL)
  {
    _gDriveShlExt->Release();
    _gDriveShlExt = NULL;
  }
}

HRESULT CDriveItemPropertyStoreFactory::_Initialize(CGDriveShlExt* gDriveShlExt, PCUITEMID_CHILD pidl)
{
  HRESULT hr = S_OK;
  Log::WriteOutput(LogType::Debug, L"CDriveItemPropertyStoreFactory::_Initialize()");

  CHECK_HR(CIdList::CloneChild(pidl, _cpidl));

  _gDriveShlExt = gDriveShlExt;
  _gDriveShlExt->AddRef();
  
  return hr;
}

STDMETHODIMP CDriveItemPropertyStoreFactory::GetPropertyStore(GETPROPERTYSTOREFLAGS flags, __in_opt IUnknown * /*punkFactory*/, REFIID riid, __deref_out void **ppv)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemPropertyStoreFactory::GetPropertyStore()");

  CAtlArray<PROPERTYKEY> rgKeys;

  HRESULT hr = _CreatePropertyKeyArray(flags, rgKeys);

  if (SUCCEEDED(hr))
  {
    hr = GetPropertyStoreForKeys(rgKeys.GetData(), (UINT)rgKeys.GetCount(), flags, riid, ppv);
  }

  return hr;
}

HRESULT CDriveItemPropertyStoreFactory::_CreatePropertyKeyArray(GETPROPERTYSTOREFLAGS flags, __inout CAtlArray<PROPERTYKEY> &rgKeys)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemPropertyStoreFactory::_CreatePropertyKeyArray(GETPROPERTYSTOREFLAGS flags, __inout CAtlArray<PROPERTYKEY> &rgKeys)");

  CComPtr<IShellFolder2> spsf;

  HRESULT hr = _gDriveShlExt->QueryInterface(IID_PPV_ARGS(&spsf));

  if (!SUCCEEDED(hr))
  {
    Log::WriteOutput(LogType::Error, L"CComPtr<IShellFolder2>->QueryInterface returned hr=%d", hr);
  }
  else
  {
    try
    {
      PROPERTYKEY key;

      for (UINT i = 0; SUCCEEDED(spsf->MapColumnToSCID(i, &key)); i++)
      {
        SHCOLSTATEF scsf;

        hr = spsf->GetDefaultColumnState(i, &scsf);

        if (!SUCCEEDED(hr))
        {
          Log::WriteOutput(LogType::Error, L"spsf->GetDefaultColumnState returned hr=%d", hr);

          break;
        }

        if (!(flags & GPS_FASTPROPERTIESONLY) || !(scsf & SHCOLSTATE_SLOW))
        {
          rgKeys.Add(key);
        }
      }
    }
    catch (CAtlException &e)
    {
      hr = e.m_hr;

      if (!SUCCEEDED(hr))
      {
        Log::WriteOutput(LogType::Error, L"_CreatePropertyKeyArray caught exception which returned hr=%d", hr);
      }
    }
  }

  return hr;
}

STDMETHODIMP CDriveItemPropertyStoreFactory::GetPropertyStoreForKeys(const PROPERTYKEY *rgKeys, UINT cKeys, GETPROPERTYSTOREFLAGS flags, REFIID riid, __deref_out void **ppv)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemPropertyStoreFactory::GetPropertyStoreForKeys()");

  HRESULT hr = _ValidateFlags(flags);

  if (SUCCEEDED(hr))
  {
    CComPtr<IPropertyStore> sppropstore;

    hr = CDriveItemPropertyStore::CreateInstanceReturnInterfaceTo(rgKeys, cKeys, flags, _gDriveShlExt, _cpidl, IID_PPV_ARGS(&sppropstore));

    if (SUCCEEDED(hr))
    {
      hr = sppropstore->QueryInterface(riid, ppv);

      if (!SUCCEEDED(hr))
      {
        Log::LogUnknownIID(riid, L"CDriveItemPropertyStoreFactory::GetPropertyStoreForKeys()");
      }
    }
  }

  return hr;
}

HRESULT CDriveItemPropertyStoreFactory::_ValidateFlags(GETPROPERTYSTOREFLAGS flags)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemPropertyStoreFactory::_ValidateFlags(GETPROPERTYSTOREFLAGS flags)");

  HRESULT hr = S_OK;

  if (((flags & GPS_HANDLERPROPERTIESONLY) && (flags & GPS_FASTPROPERTIESONLY)) ||
    ((flags & GPS_READWRITE) && (flags & GPS_BESTEFFORT)) ||
    (((flags & (GPS_READWRITE | GPS_HANDLERPROPERTIESONLY)) && (flags & GPS_DELAYCREATION))))
  {
    hr = E_INVALIDARG;
  }

  return hr;
}
