/*
Copyright 2015 Google Inc

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

#include <ShTypes.h>


// RAII class to handle item lists and common functions
class CIdList
{
  public:
    CIdList():idList_(nullptr) {}
    // Constructor takes ownership
    CIdList(LPITEMIDLIST pidl):idList_(pidl) {}
    ~CIdList() { CoTaskMemFree(idList_); }

    // Implicitly convertible to CONST pointer
    operator LPCITEMIDLIST() const { return idList_; }
    // Reassigns id list and takes ownership
    void Reset(LPITEMIDLIST& pidl) { CoTaskMemFree(idList_); idList_ = pidl; pidl = nullptr; }
    void Reset() { CoTaskMemFree(idList_); idList_ = nullptr; }
    // Returns pointer to id list and releases ownership
    LPITEMIDLIST Release() { LPITEMIDLIST idList = idList_; idList_ = nullptr; return idList; }

    static HRESULT Combine(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2, CIdList& pidlDest);
    static HRESULT Clone(LPCITEMIDLIST pidl, CIdList& pidlDest);
    static HRESULT CloneChild(LPCITEMIDLIST pidl, CIdList& pidlDest);

  private:
    LPITEMIDLIST idList_;

    // Disallow copy and assignment
    CIdList(const CIdList& src);
    CIdList& operator=(const CIdList& src);
};

inline HRESULT CIdList::Combine(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2, CIdList& pidlDest)
{
  LPITEMIDLIST combined = ILCombine(pidl1, pidl2);
  if (combined == nullptr)
    return E_OUTOFMEMORY;

  pidlDest.Reset(combined);
  return S_OK;
}

inline HRESULT CIdList::Clone(LPCITEMIDLIST pidl, CIdList& pidlDest)
{
  if (pidl == nullptr)
    return E_INVALIDARG;

  LPITEMIDLIST clone = ILClone(pidl);
  if (clone == nullptr)
    return E_OUTOFMEMORY;

  pidlDest.Reset(clone);
  return S_OK;
}

inline HRESULT CIdList::CloneChild(LPCITEMIDLIST pidl, CIdList& pidlDest)
{
  if (pidl == nullptr)
    return E_INVALIDARG;

  LPITEMIDLIST clone = ILCloneChild(pidl);
  if (clone == nullptr)
    return E_OUTOFMEMORY;

  pidlDest.Reset(clone);
  return S_OK;
}
