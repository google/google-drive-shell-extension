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

class CChildItemIDPolicy
{
public:
  static void init(PITEMID_CHILD * ppidl)
  {
    *ppidl = nullptr;
  }
  static void destroy(PITEMID_CHILD * ppidl)
  {
    CoTaskMemFree(*ppidl);
    *ppidl = nullptr;
  }
  static HRESULT copy(PITEMID_CHILD *ppidlDst, PITEMID_CHILD *ppidlSrc)
  {
    if (*ppidlSrc == nullptr)
      return E_INVALIDARG;

    LPITEMIDLIST clone = ILCloneChild(*ppidlSrc);
    if (clone == nullptr)
      return E_OUTOFMEMORY;

    *ppidlDst = clone;
    return S_OK;
  }
};
