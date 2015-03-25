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
#include "DriveItemPropertyHandler.h"

using namespace ATL;

class CDriveItemPropertyStore :
  public CComObjectRootEx<CComSingleThreadModel>,
  public IPropertyStore
{
public:
  CDriveItemPropertyStore() : _flags(GPS_DEFAULT)
  {
    Log::WriteOutput(LogType::Debug, L"CDriveItemPropertyStore::CDriveItemPropertyStore()");

    _gDriveShlExt = NULL;
  }

  BEGIN_COM_MAP(CDriveItemPropertyStore)
    COM_INTERFACE_ENTRY(IPropertyStore)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  static HRESULT CreateInstanceReturnInterfaceTo(const PROPERTYKEY *rgKeys, UINT cKeys, GETPROPERTYSTOREFLAGS flags, CGDriveShlExt* gDriveShlExt, PCUITEMID_CHILD pidl, REFIID riid, __deref_out void **ppv);

  HRESULT FinalConstruct()
  {
    return S_OK;
  }

  void FinalRelease();

  // IPropertyStore
  IFACEMETHODIMP GetCount(__out DWORD *cProps);
  IFACEMETHODIMP GetAt(DWORD iProp, __out PROPERTYKEY *pkey);
  IFACEMETHODIMP GetValue(REFPROPERTYKEY key, __out PROPVARIANT *ppropvar);
  IFACEMETHODIMP SetValue(REFPROPERTYKEY key, REFPROPVARIANT propvar);
  IFACEMETHODIMP Commit();

private:
  HRESULT _Initialize(const PROPERTYKEY *rgKeys, UINT cKeys, GETPROPERTYSTOREFLAGS flags, CGDriveShlExt* gDriveShlExt, PCUITEMID_CHILD pidl);
  HRESULT _InitializePropertyStore(const PROPERTYKEY *rgKeys, UINT cKeys, GETPROPERTYSTOREFLAGS flags);

private:
  CGDriveShlExt* _gDriveShlExt;
  CIdList _cpidl;
  CComPtr<IPropertyStore> _sppropstore;
  GETPROPERTYSTOREFLAGS _flags;
  CAtlArray<PROPERTYKEY> _rgDelayedKeys;
  CAtlArray<PROPERTYKEY> _rgDirty;
};

