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

using namespace ATL;

class CDriveItemPropertyStoreFactory :
  public CComObjectRootEx<CComSingleThreadModel>,
  public IPropertyStoreFactory
{
public:
  CDriveItemPropertyStoreFactory()
  {
    Log::WriteOutput(LogType::Debug, L"CDriveItemPropertyStoreFactory::CDriveItemPropertyStoreFactory()");
    _gDriveShlExt = NULL;
  }

  BEGIN_COM_MAP(CDriveItemPropertyStoreFactory)
    COM_INTERFACE_ENTRY(IPropertyStoreFactory)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  static HRESULT CreateInstanceReturnInterfaceTo(CGDriveShlExt* ggDriveShlExt, PCUITEMID_CHILD pidl, REFIID riid, __deref_out void **ppv);

  HRESULT FinalConstruct()
  {
    return S_OK;
  }

  void FinalRelease();

  // IPropertyStoreFactory
  IFACEMETHODIMP GetPropertyStore(GETPROPERTYSTOREFLAGS flags, __in_opt IUnknown *punkFactory, REFIID riid, __deref_out void **ppv);
  IFACEMETHODIMP GetPropertyStoreForKeys(const PROPERTYKEY *rgKeys, UINT cKeys, GETPROPERTYSTOREFLAGS flags, REFIID riid, __deref_out void **ppv);

private:
  HRESULT _Initialize(CGDriveShlExt* gDriveShlExt, PCUITEMID_CHILD pidl);
  HRESULT _CreatePropertyKeyArray(GETPROPERTYSTOREFLAGS flags, __inout CAtlArray<PROPERTYKEY> &rgKeys);
  HRESULT _ValidateFlags(GETPROPERTYSTOREFLAGS flags);

private:
  CGDriveShlExt* _gDriveShlExt; // Parent
  CIdList _cpidl; // Me from Parent
};

