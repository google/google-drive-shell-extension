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

using namespace ATL;

MIDL_INTERFACE(GUID_IDriveItemPropertyHandler)
IDriveItemPropertyHandler : public IUnknown
{
public:
  virtual HRESULT STDMETHODCALLTYPE GetProperty(
    /* [in] */ PCUITEMID_CHILD pidl,
    /* [in] */ REFPROPERTYKEY key,
    /* [out] */ PROPVARIANT *ppropvar) = 0;

  virtual HRESULT STDMETHODCALLTYPE SetProperty(
    /* [in] */ PCUITEMID_CHILD pidl,
    /* [in] */ REFPROPERTYKEY key,
    /* [in] */ REFPROPVARIANT propvar) = 0;
};
