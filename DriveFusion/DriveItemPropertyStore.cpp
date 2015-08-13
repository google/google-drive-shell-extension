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
#include "DriveItemPropertyStore.h"

HRESULT CDriveItemPropertyStore::CreateInstanceReturnInterfaceTo(const PROPERTYKEY *rgKeys, UINT cKeys, GETPROPERTYSTOREFLAGS flags, CGDriveShlExt* gDriveShlExt, PCUITEMID_CHILD pidl, REFIID riid, __deref_out void **ppv)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemPropertyStore::CreateInstance()");

  *ppv = NULL;

  CComObject<CDriveItemPropertyStore> *psprps;

  HRESULT hr = CComObject<CDriveItemPropertyStore>::CreateInstance(&psprps);

  if (!SUCCEEDED(hr))
  {
    Log::WriteOutput(LogType::Error, L"CComObject<CDriveItemPropertyStore>::CreateInstance returned hr=%d", hr);
  }
  else
  {
    psprps->AddRef();
    hr = psprps->_Initialize(rgKeys, cKeys, flags, gDriveShlExt, pidl);

    if (SUCCEEDED(hr))
    {
      hr = psprps->QueryInterface(riid, ppv);

      if (!SUCCEEDED(hr))
      {
        Log::LogUnknownIID(riid, L"CDriveItemPropertyStore::CreateInstanceReturnInterfaceTo()");
      }
    }

    psprps->Release();
  }

  return hr;
}

void CDriveItemPropertyStore::FinalRelease()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemPropertyStoreFactory::FinalRelease()");

  if (_gDriveShlExt != NULL)
  {
    _gDriveShlExt->Release();
    _gDriveShlExt = NULL;
  }
}

HRESULT CDriveItemPropertyStore::_Initialize(const PROPERTYKEY *rgKeys, UINT cKeys, GETPROPERTYSTOREFLAGS flags, CGDriveShlExt* gDriveShlExt, PCUITEMID_CHILD pidl)
{
  HRESULT hr = S_OK;
  Log::WriteOutput(LogType::Debug, L"CDriveItemPropertyStore::_Initialize(const PROPERTYKEY *rgKeys, UINT cKeys, GETPROPERTYSTOREFLAGS flags, __in IShellFolder *psf, PCUITEMID_CHILD pidl)");

  CHECK_HR(CIdList::CloneChild(pidl, _cpidl));
  
  _gDriveShlExt = gDriveShlExt;
  CHECK_HR(_InitializePropertyStore(rgKeys, cKeys, flags));
  _gDriveShlExt->AddRef();

  return hr;
}

HRESULT CDriveItemPropertyStore::_InitializePropertyStore(const PROPERTYKEY *rgKeys, UINT cKeys, GETPROPERTYSTOREFLAGS flags)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemPropertyStore::_InitializePropertyStore(const PROPERTYKEY *rgKeys, UINT cKeys, GETPROPERTYSTOREFLAGS flags)");

  HRESULT hr = PSCreateMemoryPropertyStore(IID_PPV_ARGS(&_sppropstore));

  if (!SUCCEEDED(hr))
  {
    Log::WriteOutput(LogType::Error, L"PSCreateMemoryPropertyStore returned hr=%d", hr);
  }
  else
  {
    _flags = flags;

    CComPtr<IDriveItemPropertyHandler> sprph;

    hr = _gDriveShlExt->QueryInterface(IID_PPV_ARGS(&sprph));

    if (!SUCCEEDED(hr))
    {
      Log::WriteOutput(LogType::Error, L"CComPtr<IDriveItemPropertyHandler>->QueryInterface returned hr=%d", hr);
    }
    else
    {
      for (UINT i = 0; i < cKeys; i++)
      {
        if (flags & GPS_DELAYCREATION)
        {
          _rgDelayedKeys.Add(rgKeys[i]);
        }
        else
        {
          CPropVariant spropvar;

          hr = sprph->GetProperty(_cpidl, rgKeys[i], &spropvar);

          if (FAILED(hr))
          {
            // Intentional stomping of the previous HRESULT
            // VT_EMPTY means that the value is not available for this item, so clear the
            // propvariant and insert an empty instead
            hr = spropvar.Reset();

            if (!SUCCEEDED(hr))
            {
              Log::WriteOutput(LogType::Error, L"spropvar.Clear returned hr=%d", hr);

              break;
            }
          }

          if (SUCCEEDED(hr))
          {
            // It's ok for the property to not be there, the PROPVARIANT will be VT_EMPTY
            hr = _sppropstore->SetValue(rgKeys[i], spropvar);

            if (!SUCCEEDED(hr))
            {
              Log::WriteOutput(LogType::Error, L"_sppropstore->SetValue returned hr=%d", hr);

              break;
            }
          }
        }
      }
    }
  }

  return hr;
}

STDMETHODIMP CDriveItemPropertyStore::GetCount(__out DWORD *cProps)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemPropertyStore::GetCount(__out DWORD *cProps)");

  HRESULT hr = _sppropstore->GetCount(cProps);

  if (!SUCCEEDED(hr))
  {
    Log::WriteOutput(LogType::Error, L"_sppropstore->GetCount returned hr=%d", hr);
  }

  return hr;
}

STDMETHODIMP CDriveItemPropertyStore::GetAt(DWORD iProp, __out PROPERTYKEY *pkey)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemPropertyStore::GetAt(DWORD iProp, __out PROPERTYKEY *pkey)");

  HRESULT hr = _sppropstore->GetAt(iProp, pkey);

  if (!SUCCEEDED(hr))
  {
    Log::WriteOutput(LogType::Error, L"_sppropstore->GetAt returned hr=%d", hr);
  }

  return hr;
}

STDMETHODIMP CDriveItemPropertyStore::GetValue(REFPROPERTYKEY key, __out PROPVARIANT *ppropvar)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemPropertyStore::GetValue(REFPROPERTYKEY key, __out PROPVARIANT *ppropvar)");

  HRESULT hr = S_OK;

  if (_flags & GPS_DELAYCREATION)
  {
    // Find this prop in the delayed key list
    for (size_t i = 0; i < _rgDelayedKeys.GetCount(); i++)
    {
      if (IsEqualPropertyKey(key, _rgDelayedKeys[i]))
      {
        _rgDelayedKeys.RemoveAt(i);

        CComPtr<IDriveItemPropertyHandler> sprph;

        hr = _gDriveShlExt->QueryInterface(IID_PPV_ARGS(&sprph));

        if (!SUCCEEDED(hr))
        {
          Log::WriteOutput(LogType::Error, L"CComPtr<IDriveItemPropertyHandler>->QueryInterface returned hr=%d", hr);
        }
        else
        {
          CPropVariant spropvar;

          hr = sprph->GetProperty(_cpidl, key, &spropvar);

          if (!SUCCEEDED(hr) && hr != E_INVALIDARG)
          {
            Log::WriteOutput(LogType::Error, L"CComPtr<IDriveItemPropertyHandler>->GetProperty returned hr=%d", hr);
          }
          else
          {
            hr = _sppropstore->SetValue(key, spropvar);

            if (!SUCCEEDED(hr))
            {
              Log::WriteOutput(LogType::Error, L"_sppropstore->SetValue returned hr=%d", hr);
            }
          }

          break;
        }
      }
    }
  }

  if (SUCCEEDED(hr))
  {
    hr = _sppropstore->GetValue(key, ppropvar);

    if (!SUCCEEDED(hr))
    {
      Log::WriteOutput(LogType::Error, L"_sppropstore->GetValue returned hr=%d", hr);
    }
  }

  return hr;
}

STDMETHODIMP CDriveItemPropertyStore::SetValue(REFPROPERTYKEY key, REFPROPVARIANT propvar)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemPropertyStore::SetValue(REFPROPERTYKEY key, REFPROPVARIANT propvar)");

  HRESULT hr = _sppropstore->SetValue(key, propvar);

  if (!SUCCEEDED(hr))
  {
    Log::WriteOutput(LogType::Error, L"_sppropstore->SetValue returned hr=%d", hr);
  }
  else
  {
    try
    {
      _rgDirty.Add(key);
    }
    catch (CAtlException &e)
    {
      hr = e.m_hr;

      if (!SUCCEEDED(hr))
      {
        Log::WriteOutput(LogType::Error, L"_rgDirty.Add returned hr=%d", hr);
      }
    }
  }

  return hr;
}

STDMETHODIMP CDriveItemPropertyStore::Commit()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemPropertyStore::Commit()");

  CComPtr<IDriveItemPropertyHandler> sprph;

  HRESULT hr = _gDriveShlExt->QueryInterface(IID_PPV_ARGS(&sprph));

  if (!SUCCEEDED(hr))
  {
    Log::WriteOutput(LogType::Error, L"CComPtr<IDriveItemPropertyHandler>->QueryInterface returned hr=%d", hr);
  }
  else
  {
    for (size_t i = 0; i < _rgDirty.GetCount(); i++)
    {
      CPropVariant sppropvar;

      hr = _sppropstore->GetValue(_rgDirty.GetAt(i), &sppropvar);

      if (!SUCCEEDED(hr))
      {
        Log::WriteOutput(LogType::Error, L"_sppropstore->GetValue returned hr=%d", hr);

        break;
      }

      hr = sprph->SetProperty(_cpidl, _rgDirty.GetAt(i), sppropvar);

      if (!SUCCEEDED(hr))
      {
        Log::WriteOutput(LogType::Error, L"sprph->SetProperty returned hr=%d", hr);

        break;
      }
    }
  }

  return hr;
}
