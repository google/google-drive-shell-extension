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
#include "DriveItemDataObject.h"
#include <cassert>
#include <sstream>

namespace {
  const WCHAR kGoogleDriveSouceIdFormatName[31] = L"Google Drive Source Identifier";
}

HRESULT CDriveItemDataObject::CreateInstanceReturnInterfaceTo(CGDriveShlExt* gDriveShlExt, REFIID riid, __deref_out void **ppv)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemDataObject::CreateInstance(CGDriveShlExt* gDriveShlExt, UINT cidl, PCUITEMID_CHILD_ARRAY rgpidl, REFIID riid, __deref_out void **ppv)");

  *ppv = NULL;

  CComObject<CDriveItemDataObject> *psprfvcb;

  HRESULT hr = CComObject<CDriveItemDataObject>::CreateInstance(&psprfvcb);

  if (!SUCCEEDED(hr))
  {
    Log::WriteOutput(LogType::Error, L"CComObject<CDriveItemDataObject>::CreateInstance returned hr=%d", hr);
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
        Log::LogUnknownIID(riid, L"CDriveItemDataObject::CreateInstanceReturnInterfaceTo()");
      }
    }

    psprfvcb->Release();
  }

  return hr;
}

void CDriveItemDataObject::FinalRelease()
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemDataObject::FinalRelease()");

  for (CBFormatList::iterator it = _CBFormatList.begin(); it != _CBFormatList.end(); it++)
  {
    ReleaseStgMedium(&it->StgMedium);
  }

  _CBFormatList.clear();

  if (_gDriveShlExt != NULL)
  {
    _gDriveShlExt->Release();
    _gDriveShlExt = NULL;
  }
}

HRESULT CDriveItemDataObject::_Initialize(CGDriveShlExt* gDriveShlExt)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemDataObject::_Initialize(CGDriveShlExt* gDriveShlExt, UINT cidl, PCUITEMID_CHILD_ARRAY rgpidl)");

  _gDriveShlExt = gDriveShlExt;
  _gDriveShlExt->AddRef();

  return S_OK;
}

static void _Get_HGDROP_Contents(STGMEDIUM *pmedium, std::vector<std::wstring>& fileList)
{
  if (pmedium->tymed != TYMED_HGLOBAL)
  {
    Log::WriteOutput(LogType::Error, L"_LogCF_HGDROP_Contents bad argument");
  }
  else
  {
    if (pmedium->hGlobal != NULL)
    {
      HGLOBAL gmem = pmedium->hGlobal;
      HDROP hdrop = (HDROP)GlobalLock(gmem);
      assert(hdrop != NULL);
      UINT numOfFiles = DragQueryFile(hdrop, 0xFFFFFFFF, NULL, 0);

      TCHAR buffer[MAX_PATH];
      for (UINT i = 0; i < numOfFiles; i++)
      {
        UINT charsCopied = DragQueryFile((HDROP)hdrop, i, buffer, MAX_PATH);
        if (charsCopied != 0)
        {
          fileList.push_back(buffer);
        }
      }

      GlobalUnlock(gmem);
    }
    else
    {
      Log::WriteOutput(LogType::Information, L"_LogCF_HGDROP_Contents no files");
    }
  }
}

static void _LogCF_HGDROP_Contents(STGMEDIUM *pmedium)
{
  std::vector<std::wstring> fileList;

  _Get_HGDROP_Contents(pmedium, fileList);

  size_t numOfFiles = fileList.size();

  for (size_t i = 0; i < numOfFiles; i++)
  {
    Log::WriteOutput(LogType::Warning, L"_LogCF_HGDROP_Contents %d %d %s", numOfFiles, i, fileList.at(i).c_str());
  }
}

STDMETHODIMP CDriveItemDataObject::GetData(FORMATETC *pformatetc, STGMEDIUM *pmedium)
{
  CBFormatList::iterator it;

  HRESULT hr = _LookupFormatEtc(pformatetc, it);

  if (SUCCEEDED(hr))
  {
    Log::WriteOutput(LogType::Warning, _AppendFormatName(L"CDriveItemDataObject::GetData returning stored format: ", pformatetc->cfFormat).c_str());

    hr = _AddRefStgMedium(&it->StgMedium, pmedium, false);

    assert(SUCCEEDED(hr));

    if (pmedium->tymed == TYMED_HGLOBAL && pmedium->hGlobal == NULL)
    {
      Log::WriteOutput(LogType::Warning, L"CDriveItemDataObject::GetData hglobal is null");
    }
    else if (pmedium->tymed == TYMED_GDI && pmedium->hBitmap == NULL)
    {
      Log::WriteOutput(LogType::Warning, L"CDriveItemDataObject::GetData hBitmap is null");
    }
    else if (pmedium->tymed == TYMED_MFPICT && pmedium->hMetaFilePict == NULL)
    {
      Log::WriteOutput(LogType::Warning, L"CDriveItemDataObject::GetData hMetaFilePict is null");
    }
    else if (pmedium->tymed == TYMED_ENHMF && pmedium->hEnhMetaFile == NULL)
    {
      Log::WriteOutput(LogType::Warning, L"CDriveItemDataObject::GetData hEnhMetaFile is null");
    }
    else if (pmedium->tymed == TYMED_FILE && pmedium->lpszFileName == NULL)
    {
      Log::WriteOutput(LogType::Warning, L"CDriveItemDataObject::GetData lpszFileName is null");
    }
    else if (pmedium->tymed == TYMED_ISTREAM && pmedium->pstm == NULL)
    {
      Log::WriteOutput(LogType::Warning, L"CDriveItemDataObject::GetData pstm is null");
    }
    else if (pmedium->tymed == TYMED_ISTORAGE && pmedium->pstg == NULL)
    {
      Log::WriteOutput(LogType::Warning, L"CDriveItemDataObject::GetData pstg is null");
    }

    if (pformatetc->cfFormat == CF_HDROP)
    {
      _LogCF_HGDROP_Contents(pmedium);
    }
  }
  else
  {
    Log::WriteOutput(LogType::Error, _AppendFormatName(L"CDriveItemDataObject::GetData not present format: ", pformatetc->cfFormat).c_str());
  }

  return hr;
}

STDMETHODIMP CDriveItemDataObject::GetDataHere(FORMATETC * /*pformatetc*/, STGMEDIUM * /*pmedium*/)
{
  Log::WriteOutput(LogType::Error, L"CDriveItemDataObject::GetDataHere DATA_E_FORMATETC");

  // IDataObject::GetDataHere can only be implemented if the IStream and IStorage interfaces are supported by the data object.
  // In our case we only support HGLOBAL data, so returning DATA_E_FORMATETC seems a sensible choice.

  return DATA_E_FORMATETC;
}

STDMETHODIMP CDriveItemDataObject::QueryGetData(__RPC__in_opt FORMATETC *pformatetc)
{
  CBFormatList::iterator it;
  HRESULT hr = E_INVALIDARG;

  if (pformatetc == NULL)
    return hr;

  if (_IsShellType(pformatetc->cfFormat))
    return S_OK;
  else
    hr = _LookupFormatEtc(pformatetc, it);

  if (SUCCEEDED(hr))
  {
    Log::WriteOutput(LogType::Warning, _AppendFormatName(L"CDriveItemDataObject::QueryGetData found a populated format: ", pformatetc->cfFormat).c_str());
  }
  else
  {
    Log::WriteOutput(LogType::Warning, _AppendFormatName(L"CDriveItemDataObject::QueryGetData does not support format: ", pformatetc->cfFormat).c_str());
  }

  return hr;
}

STDMETHODIMP CDriveItemDataObject::GetCanonicalFormatEtc(__RPC__in_opt FORMATETC * /*pformatectIn*/, __RPC__out FORMATETC *pformatetcOut)
{
  Log::WriteOutput(LogType::Error, L"CDriveItemDataObject::GetCanonicalFormatEtc E_NOTIMPL");
  // Apparently we have to set this field to NULL even though we don't do anything else
  pformatetcOut->ptd = NULL;
  return E_NOTIMPL;
}

STDMETHODIMP CDriveItemDataObject::SetData(FORMATETC *pformatetc, STGMEDIUM *pmedium, BOOL fRelease)
{
  if (!fRelease)
  {
    Log::WriteOutput(LogType::Error, L"CDriveItemDataObject::SetData fRelease is false: I'm not sure what we are expected to do");
    return E_NOTIMPL;
  }

  if (pformatetc->cfFormat == CF_HDROP)
  {
    _LogCF_HGDROP_Contents(pmedium);
  }

  CBFormatList::iterator it;

  HRESULT hr = _LookupFormatEtc(pformatetc, it);

  if (hr == DV_E_FORMATETC)
  {
    if (it == _CBFormatList.end())
    {
      if (pmedium->hGlobal != NULL)
      {
        hr = _AddFormat(pformatetc, pmedium);
      }
      else
      {
        Log::WriteOutput(LogType::Information, _AppendFormatName(L"CDriveItemDataObject::SetData hGlobal is NULL should this have special handling format: ", pformatetc->cfFormat).c_str());
      }
    }
    else
    {
      if (pmedium->hGlobal == NULL)
      {
        ReleaseStgMedium(&it->StgMedium);

        _CBFormatList.erase(it);
      }

      hr = S_OK;
    }
  }

  if (SUCCEEDED(hr))
  {
    Log::WriteOutput(LogType::Warning, _AppendFormatName(L"CDriveItemDataObject::SetData format: ", pformatetc->cfFormat).c_str());
  }
  else
  {
    Log::WriteOutput(LogType::Error, _AppendFormatName(L"CDriveItemDataObject::SetData failed format: ", pformatetc->cfFormat).c_str());
  }
  return hr;
}

STDMETHODIMP CDriveItemDataObject::EnumFormatEtc(DWORD dwDirection, __RPC__deref_out_opt IEnumFORMATETC **ppenumFormatEtc)
{
  // only the get direction is supported for OLE
  HRESULT hr = E_NOTIMPL;

  if (dwDirection == DATADIR_GET)
  {
    // for Win2k+ you can use the SHCreateStdEnumFmtEtc API call, however
    // to support all Windows platforms we need to implement IEnumFormatEtc ourselves.

    // maybe the format should be stored as two separate list, instead of a unified object
    std::vector<FORMATETC> list(_CBFormatList.size());

    size_t i = 0;

    // This is reversed because in _CreateRelatedFormats, we create and add formats in least helpful order.  Consumers expect formats to be returned in most helpful order, so start with the last format added, and work backwards
    for (CBFormatList::reverse_iterator it = _CBFormatList.rbegin(); it != _CBFormatList.rend(); it++, i++)
    {
      list[i] = it->Format;
    }

    hr = SHCreateStdEnumFmtEtc((UINT)list.size(), &list[0], ppenumFormatEtc);

    if (!SUCCEEDED(hr))
    {
      Log::WriteOutput(LogType::Error, L"CDriveItemDataObject::EnumFormatEtc failed");
    }
  }
  else
  {
    Log::WriteOutput(LogType::Error, L"CDriveItemDataObject::EnumFormatEtc E_NOTIMPL");
    // the direction specified is not supported for drag+drop
  }

  return hr;
}

STDMETHODIMP CDriveItemDataObject::DAdvise(__RPC__in FORMATETC * /*pformatetc*/, DWORD /*advf*/, __RPC__in_opt IAdviseSink * /*pAdvSink*/, __RPC__out DWORD * /*pdwConnection*/)
{
  Log::WriteOutput(LogType::Error, L"CDriveItemDataObject::DAdvise OLE_E_ADVISENOTSUPPORTED");
  return OLE_E_ADVISENOTSUPPORTED;
}

STDMETHODIMP CDriveItemDataObject::DUnadvise(DWORD /*dwConnection*/)
{
  Log::WriteOutput(LogType::Error, L"CDriveItemDataObject::DUnadvise OLE_E_ADVISENOTSUPPORTED");
  return OLE_E_ADVISENOTSUPPORTED;
}

STDMETHODIMP CDriveItemDataObject::EnumDAdvise(__RPC__deref_out_opt IEnumSTATDATA ** /*ppenumAdvise*/)
{
  Log::WriteOutput(LogType::Error, L"CDriveItemDataObject::EnumDAdvise OLE_E_ADVISENOTSUPPORTED");
  return OLE_E_ADVISENOTSUPPORTED;
}

STDMETHODIMP CDriveItemDataObject::DragEnter(__RPC__in_opt IDataObject * pDataObj, DWORD grfKeyState, POINTL /*pt*/, __RPC__inout DWORD *pdwEffect)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemDataObject::DragEnter()");
  // Indicates whether a drop can be accepted, and, if so, the effect of the drop

  if (pdwEffect == NULL || pDataObj == NULL)
  {
    return E_INVALIDARG;
  }

  // For now just check if there exists a CF_HDROP, we'll add other formats as needed
  FORMATETC format;
  _BuildFormat_CF_HDROP(format);

  HRESULT hr = pDataObj->QueryGetData(&format);

  if (SUCCEEDED(hr))
  {
    _dragEnterPdwEffect = _GetPdwEffectFromState(grfKeyState);
  }
  else
  {
    _dragEnterPdwEffect = DROPEFFECT_NONE;

#ifdef DEBUG
    // This is an expensive process, so release won't do it

    CComHeapPtr<IEnumFORMATETC> eFormat;

    HRESULT hr = pDataObj->EnumFormatEtc(DATADIR_GET, &eFormat);

    ULONG received = 0;
    FORMATETC rformatArray[100];

    hr = eFormat->Next(100, rformatArray, &received);
    std::wstring formats;

    for (size_t i = 0; i < received; i++)
    {
      std::wstring standardFormatName = _GetStandardFormatName(rformatArray[i].cfFormat);

      if (standardFormatName.length() > 0)
      {
        formats += standardFormatName + L" :: ";
      }
      else
      {
        WCHAR formatName[200];

        int result = GetClipboardFormatName(rformatArray[i].cfFormat, formatName, 200);

        if (result != 0)
        {
          formats += std::wstring(formatName) + L" :: ";
        }
      }
    }

    Log::WriteOutput(LogType::Error, L"CDriveItemDataObject::DragEnter() %s", formats.c_str());
#else
    Log::WriteOutput(LogType::Error, L"CDriveItemDataObject::DragEnter() cannot accept the data");
#endif

  }

  *pdwEffect = _dragEnterPdwEffect;

  return hr;
}

STDMETHODIMP CDriveItemDataObject::DragOver(DWORD grfKeyState, POINTL /*pt*/, __RPC__inout DWORD *pdwEffect)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemDataObject::DragOver");

  if (pdwEffect == NULL)
  {
    return E_INVALIDARG;
  }

  if (_dragEnterPdwEffect != DROPEFFECT_NONE)
  {
    _dragEnterPdwEffect = _GetPdwEffectFromState(grfKeyState);
  }

  *pdwEffect = _dragEnterPdwEffect;

  return S_OK;
}

STDMETHODIMP CDriveItemDataObject::DragLeave(void)
{
  // To implement IDropTarget::DragLeave, you must remove any target feedback that is currently displayed. You must also release any references you hold to the data transfer object.

  Log::WriteOutput(LogType::Debug, L"CDriveItemDataObject::DragLeave()");
  // I'm not holding onto anything yet... so nothing to release yet

  _dragEnterPdwEffect = DROPEFFECT_NONE;

  return S_OK;
}

STDMETHODIMP CDriveItemDataObject::Drop(__RPC__in_opt IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, __RPC__inout DWORD *pdwEffect)
{
  Log::WriteOutput(LogType::Debug, L"CDriveItemDataObject::Drop()");

  if (pdwEffect == NULL || pDataObj == NULL)
  {
    return E_INVALIDARG;
  }

  HRESULT hr = S_OK;

  STGMEDIUM stgMedium; // reusable in loop

  CComHeapPtr<IEnumFORMATETC> eFormat;

  hr = pDataObj->EnumFormatEtc(DATADIR_GET, &eFormat);

  CLIPFORMAT preferredDropEffectId = (CLIPFORMAT)RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
  CLIPFORMAT shellIDArray = (CLIPFORMAT)RegisterClipboardFormat(CFSTR_SHELLIDLIST);
  CLIPFORMAT googleDriveId = (CLIPFORMAT)RegisterClipboardFormat(kGoogleDriveSouceIdFormatName);

  DWORD dropEffect = DROPEFFECT_NONE;
  bool isGoogleDriveSource = false;

  std::vector<PCIDLIST_RELATIVE> pidlList;

  ULONG received = 0;
  FORMATETC rFormatArray[100];

  hr = eFormat->Next(100, rFormatArray, &received);
  std::wstring formats;

  for (size_t i = 0; SUCCEEDED(hr) && i < received; i++)
  {
    if (rFormatArray[i].cfFormat == preferredDropEffectId)
    {
      HRESULT hrGet = pDataObj->GetData(&rFormatArray[i], &stgMedium);

      if (SUCCEEDED(hrGet))
      {
        hrGet = _Extract_HGLOBAL(stgMedium, sizeof(DWORD), &dropEffect);
      }
    }
    else if (rFormatArray[i].cfFormat == googleDriveId)
    {
      isGoogleDriveSource = true;
    }
    else if (rFormatArray[i].cfFormat == shellIDArray)
    {
      HRESULT hrGet = pDataObj->GetData(&rFormatArray[i], &stgMedium);
      if (SUCCEEDED(hrGet))
      {
        hrGet = _Convert_CFSTR_SHELLIDLIST_to_pidlList(&stgMedium, pidlList);
      }
    }
  }

  if (SUCCEEDED(hr))
  {
    hr = this->DragOver(grfKeyState, pt, pdwEffect); // final check for _dragEnterPdwEffect
  }

  *pdwEffect = _dragEnterPdwEffect;
  bool isMove = dropEffect & DROPEFFECT_MOVE || (dropEffect == DROPEFFECT_NONE && (*pdwEffect & DROPEFFECT_MOVE));

  if (SUCCEEDED(hr) && pidlList.size() > 0)
  {
    if (isGoogleDriveSource)
    {
      CComHeapPtr<ITEMIDLIST_ABSOLUTE> dstPidl;

      hr = _gDriveShlExt->GetCurFolder(&dstPidl);

      std::vector<PCIDLIST_RELATIVE> items;
      items.assign(pidlList.begin()+1, pidlList.end()); // the first element is the ABSOLUTE pidlf or the source folder, other elements are relative to this folder

      if (SUCCEEDED(hr))
      {
        if (isMove)
        {
          if (!ILIsEqual(pidlList.front(), dstPidl))
          {
            _gDriveShlExt->DoMoveFiles(items, pidlList.front(), dstPidl);
          }
        }
        else
        {
          _gDriveShlExt->DoCopyFiles(items, pidlList.front(), dstPidl);
        }
      }
    }
    else
    {
      CComPtr<IIdentityName> idName;

      hr = _gDriveShlExt->CreateViewObject(NULL, IID_PPV_ARGS(&idName)); // returns an instance of CDriveItemRelatedItem

      if (SUCCEEDED(hr))
      {
        CComPtr<IShellItem> folderSI;

        hr = idName->GetItem(&folderSI);

        if (SUCCEEDED(hr))
        {
          CComPtr<IFileOperation> pfo;

          hr = pfo.CoCreateInstance(CLSID_FileOperation, NULL, CLSCTX_ALL);
          DWORD cookie = 0;

          if (SUCCEEDED(hr))
          {
            pfo->Advise(this, &cookie);

            if (SUCCEEDED(hr))
            {
              if (isMove)
              {
                hr = pfo->MoveItems(pDataObj, folderSI);
              }
              else
              {
                hr = pfo->CopyItems(pDataObj, folderSI);
              }

              if (SUCCEEDED(hr))
              {
                hr = pfo->PerformOperations();

                if (SUCCEEDED(hr))
                {
                  BOOL whereAnyOpperationsAborted = false;

                  hr = pfo->GetAnyOperationsAborted(&whereAnyOpperationsAborted);

                  if (SUCCEEDED(hr) && !whereAnyOpperationsAborted)
                  {
                    FORMATETC performedDropEffectFormat;
                    STGMEDIUM performedDropEffectMedium;

                    if (isMove)
                    {
                      hr = _Create_CFSTR_PERFORMEDDROPEFFECT(DROPEFFECT_MOVE, performedDropEffectFormat, performedDropEffectMedium);
                    }
                    else
                    {
                      hr = _Create_CFSTR_PERFORMEDDROPEFFECT(DROPEFFECT_COPY, performedDropEffectFormat, performedDropEffectMedium);
                    }

                    if (SUCCEEDED(hr))
                    {
                      // Note: I don't care what the result is here, the object can do whatever it wants
                      pDataObj->SetData(&performedDropEffectFormat, &performedDropEffectMedium, true);

                      if (isMove)
                      {
                        // Send PASTESUCCEEDED, has the same construction as PERFORMEDDROPEFFECT, be sure to change the format id
                        performedDropEffectFormat.cfFormat = (CLIPFORMAT)RegisterClipboardFormat(CFSTR_PASTESUCCEEDED);

                        pDataObj->SetData(&performedDropEffectFormat, &performedDropEffectMedium, true);

                        _Create_CFSTR_PERFORMEDDROPEFFECT(DROPEFFECT_MOVE, performedDropEffectFormat, performedDropEffectMedium);

                        // Send LOGICALPERFORMEDDROPEFFECT, has the same construction as PERFORMEDDROPEFFECT, be sure to change the format id
                        performedDropEffectFormat.cfFormat = (CLIPFORMAT)RegisterClipboardFormat(CFSTR_LOGICALPERFORMEDDROPEFFECT);

                        pDataObj->SetData(&performedDropEffectFormat, &performedDropEffectMedium, true);
                      }
                    }
                  }
                }
              }
            }

            pfo->Unadvise(cookie);
            pfo.Release();
          }
          folderSI.Release();
        }
        idName.Release();
      }
    }
  }

  return hr;
}

HRESULT CDriveItemDataObject::_LookupFormatEtc(__in FORMATETC *pFormatEtc, __out CBFormatList::iterator& it)
{
  it = _CBFormatList.end();

  /* Comparing two DVTARGETDEVICE structures is hard, so we don't even try */
  if (pFormatEtc->ptd != NULL)
  {
    Log::WriteOutput(LogType::Error, L"CDriveItemDataObject::_LookupFormatEtc Comparing two DVTARGETDEVICE structures is hard, so we don't even try");
    return DV_E_DVTARGETDEVICE;
  }

  // check each of our formats in turn to see if one matches
  for (it = _CBFormatList.begin(); it != _CBFormatList.end(); it++)
  {
    if (it->Format.cfFormat == pFormatEtc->cfFormat &&
      it->Format.dwAspect == pFormatEtc->dwAspect &&
      it->Format.lindex == pFormatEtc->lindex)
    {
      if ((it->Format.tymed & pFormatEtc->tymed))
      {
        return S_OK;
      }
      else
      {
        return DV_E_TYMED;
      }
    }
  }

  return DV_E_FORMATETC;
}

HRESULT CDriveItemDataObject::_AddFormat(__in FORMATETC *pFormatEtc, __in STGMEDIUM *pMedium)
{
  CBFormat format;
  format.Format = *pFormatEtc;
  ZeroMemory(&format.StgMedium, sizeof(STGMEDIUM));

  _CBFormatList.push_back(format);

  STGMEDIUM* stgm = &_CBFormatList.back().StgMedium;

  HRESULT hr = _AddRefStgMedium(pMedium, stgm, true);

  _CBFormatList.back().Format.tymed = stgm->tymed;    /* Keep in sync */

  /* Subtlety!  Break circular reference loop */
  if (_GetCanonicalIUnknown(stgm->pUnkForRelease) ==
    _GetCanonicalIUnknown(static_cast<IDataObject*>(this))) {
      stgm->pUnkForRelease->Release();
      stgm->pUnkForRelease = NULL;
  }

  if (_ShouldCreateAdditionalFormatsForShell(pFormatEtc->cfFormat))
  {
    hr = _CreateRelatedFormats(pFormatEtc, pMedium);
  }

  return S_OK;
}

HRESULT CDriveItemDataObject::_AddRefStgMedium(__in STGMEDIUM *pmediumIn, __out STGMEDIUM *pmediumOut, __in BOOL fCopyIn)
{
  HRESULT hres = S_OK;

  STGMEDIUM stgmOut = *pmediumIn;

  if (pmediumIn->pUnkForRelease == NULL &&
    !(pmediumIn->tymed & (TYMED_ISTREAM | TYMED_ISTORAGE))) {
      if (fCopyIn) {
        /* Object needs to be cloned */
        if (pmediumIn->tymed == TYMED_HGLOBAL) {
          stgmOut.hGlobal = _DupGlobalMem(pmediumIn->hGlobal);
          if (!stgmOut.hGlobal) {
            hres = E_OUTOFMEMORY;
          }
        }
        else {
          hres = DV_E_TYMED;      /* Don't know how to clone GDI objects */
        }
      }
      else {
        stgmOut.pUnkForRelease = static_cast<IDataObject*>(this);
      }
  }

  if (SUCCEEDED(hres)) {
    switch (stgmOut.tymed) {
    case TYMED_ISTREAM:
      stgmOut.pstm->AddRef();
      break;
    case TYMED_ISTORAGE:
      stgmOut.pstg->AddRef();
      break;
    }
    if (stgmOut.pUnkForRelease) {
      stgmOut.pUnkForRelease->AddRef();
    }

    *pmediumOut = stgmOut;
  }

  return hres;
}
//
HRESULT CDriveItemDataObject::_CreateRelatedFormats(__in FORMATETC *pFormatEtc, __in STGMEDIUM *pmediumIn)
{
  HRESULT hr = S_OK;

  if (pFormatEtc->cfFormat == RegisterClipboardFormat(CFSTR_SHELLIDLIST))
  {
    if (pFormatEtc->tymed == TYMED_HGLOBAL)
    {
      std::vector<PCIDLIST_RELATIVE> pidlList;
      std::vector<std::wstring> files;
      std::vector<WIN32_FIND_DATA> win32FindData;
      std::vector<CComPtr<IStream> > streams;

      hr = _Convert_CFSTR_SHELLIDLIST_to_pidlList(pmediumIn, pidlList);

      if (SUCCEEDED(hr))
      {
        UINT fileCount = 0;

        for (size_t i = 1; i < pidlList.size(); i++) // the first element is the src folder, and should is not needed to be included here
        {
          CDriveItem item;

          hr = _gDriveShlExt->GetDriveItemFromIDList(pidlList[i], false, false, item);

          if (SUCCEEDED(hr) && item.IsFile())  // TODO: We are ignoring folders... this is clearly wrong and will need to be addressed eventually
          {
            files.push_back(item.FileInfo()->FilePath());

            win32FindData.resize(fileCount + 1);
            streams.resize(fileCount + 1);

            item.TryWIN32FindData(win32FindData[fileCount]);
            hr = _gDriveShlExt->BindToObject(pidlList[i], NULL, IID_PPV_ARGS(&streams[fileCount]));

            fileCount++;
          }
        }
      }

      if (SUCCEEDED(hr))
      {
        FORMATETC format;
        STGMEDIUM medium;

        hr = _Create_GOOGLEDRIVESOURCEID(format, medium);

        if (SUCCEEDED(hr))
        {
          hr = this->SetData(&format, &medium, true);
        }

        hr = _Create_CFSTR_FILEDESCRIPTOR(win32FindData, format, medium);

        if (SUCCEEDED(hr))
        {
          hr = this->SetData(&format, &medium, true);

          for (size_t i = 0; SUCCEEDED(hr) && i < streams.size(); i++)
          {
            hr = _Create_CFSTR_FILECONTENTS(i, streams[i], format, medium);

            if (SUCCEEDED(hr))
            {
              hr = this->SetData(&format, &medium, true);
            }
          }
        }

        // I can't figure out a way to detect when this format is made use of in a user scenerio
        // Avast's Context Menu -> Scan file, will surface when this format is avaiable, but there
        // is no way to 'catch' the command to download the file soon enough, while also NOT catching
        // a copy/paste invocation too soon.
        hr = _Create_CF_HDROP_FromList(files, format, medium);

        if (SUCCEEDED(hr))
        {
          hr = this->SetData(&format, &medium, true);
        }
      }
    }
    else
    {
      Log::WriteOutput(LogType::Error, L"CDriveItemDataObject::_CreateRelatedFormats CFSTR_SHELLIDLIST unknown medium");
    }
  }

  return hr;
}

HGLOBAL CDriveItemDataObject::_DupGlobalMem(HGLOBAL hMem)
{
  // If this triggers a debug, we probably created the hGlobal wrong somewhere else
  DWORD len = (DWORD)GlobalSize(hMem);
  PVOID source = GlobalLock(hMem);
  assert(source != NULL);
  PVOID dest = GlobalAlloc(GMEM_FIXED, len);
  assert(dest != NULL);
  memcpy(dest, source, len);
  GlobalUnlock(hMem);
  return dest;
}

IUnknown* CDriveItemDataObject::_GetCanonicalIUnknown(IUnknown* punk)
{
  IUnknown* punkCanonical;
  if (punk && SUCCEEDED(punk->QueryInterface(IID_IUnknown, (LPVOID*)&punkCanonical))) {
    punkCanonical->Release();
  }
  else {
    punkCanonical = punk;
  }
  return punkCanonical;
}

std::wstring CDriveItemDataObject::_AppendFormatName(const PWCHAR logMessage, CLIPFORMAT formatId)
{
  std::wstringstream r;
  r << logMessage;

  std::wstring standardFormatName = _GetStandardFormatName(formatId);

  if (standardFormatName.length() > 0)
  {
    r << standardFormatName;
  }
  else
  {
    WCHAR formatName[200];

    int result = GetClipboardFormatName(formatId, formatName, 200);

    if (result != 0)
    {
      r << formatName;
    }
    else
    {
      r << L"Unknown registered formatid: ";
      r << formatId;
    }
  }

  return r.str();
}

std::wstring CDriveItemDataObject::_GetStandardFormatName(CLIPFORMAT formatId)
{
  switch (formatId)
  {
  case CF_TEXT:
    return L"CF_TEXT";
  case CF_BITMAP:
    return L"CF_BITMAP";
  case CF_METAFILEPICT:
    return L"CF_METAFILEPICT";
  case CF_SYLK:
    return L"CF_SYLK";
  case CF_DIF:
    return L"CF_DIF";
  case CF_TIFF:
    return L"CF_TIFF";
  case CF_OEMTEXT:
    return L"CF_OEMTEXT";
  case CF_DIB:
    return L"CF_DIB";
  case CF_PALETTE:
    return L"CF_PALETTE";
  case CF_PENDATA:
    return L"CF_PENDATA";
  case CF_RIFF:
    return L"CF_RIFF";
  case CF_WAVE:
    return L"CF_WAVE";
  case CF_UNICODETEXT:
    return L"CF_UNICODETEXT";
  case CF_ENHMETAFILE:
    return L"CF_ENHMETAFILE";
  case CF_HDROP:
    return L"CF_HDROP";
  case CF_LOCALE:
    return L"CF_LOCALE";
  case CF_DIBV5:
    return L"CF_DIBV5";
  default:
    return L"";
  }
}

bool CDriveItemDataObject::_ShouldCreateAdditionalFormatsForShell(CLIPFORMAT formatId)
{
  if (formatId == RegisterClipboardFormat(CFSTR_SHELLIDLIST))
  {
    return true;
  }

  return false;
}

bool CDriveItemDataObject::_IsShellType(CLIPFORMAT formatId)
{
  if (formatId == CF_HDROP ||
    formatId == RegisterClipboardFormat(CFSTR_FILECONTENTS) ||
    formatId == RegisterClipboardFormat(CFSTR_FILEDESCRIPTOR) ||
    formatId == RegisterClipboardFormat(CFSTR_FILENAME) ||
    formatId == RegisterClipboardFormat(CFSTR_FILENAMEMAP) ||
    formatId == RegisterClipboardFormat(CFSTR_MOUNTEDVOLUME) ||
    formatId == RegisterClipboardFormat(CFSTR_SHELLIDLIST) ||
    formatId == RegisterClipboardFormat(CFSTR_SHELLIDLISTOFFSET) ||
    formatId == RegisterClipboardFormat(CFSTR_INDRAGLOOP) ||
    formatId == RegisterClipboardFormat(CFSTR_LOGICALPERFORMEDDROPEFFECT) ||
    formatId == RegisterClipboardFormat(CFSTR_PASTESUCCEEDED) ||
    formatId == RegisterClipboardFormat(CFSTR_PERFORMEDDROPEFFECT) ||
    formatId == RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT) ||
    formatId == RegisterClipboardFormat(CFSTR_TARGETCLSID) ||
    formatId == RegisterClipboardFormat(CFSTR_UNTRUSTEDDRAGDROP) ||

    formatId == RegisterClipboardFormat(CFSTR_NETRESOURCES) ||
    formatId == RegisterClipboardFormat(CFSTR_INETURL) ||
    formatId == RegisterClipboardFormat(CFSTR_PRINTERGROUP) ||
    formatId == RegisterClipboardFormat(CFSTR_SHELLURL) ||
    formatId == RegisterClipboardFormat(CFSTR_FILENAMEW) ||
    formatId == RegisterClipboardFormat(CFSTR_PRINTERGROUP) ||
    formatId == RegisterClipboardFormat(CFSTR_FILE_ATTRIBUTES_ARRAY) ||
    formatId == RegisterClipboardFormat(CFSTR_INVOKECOMMAND_DROPPARAM) ||
    formatId == RegisterClipboardFormat(CFSTR_SHELLDROPHANDLER) ||
    formatId == RegisterClipboardFormat(CFSTR_DROPDESCRIPTION)
    )
  {
    return true;
  }

  return false;
}

DWORD CDriveItemDataObject::_GetPdwEffectFromState(DWORD grfKeyState)
{
  // combinations of MK_CONTROL, MK_SHIFT, MK_ALT, MK_BUTTON, MK_LBUTTON, MK_MBUTTON, and MK_RBUTTON
  // Don't support LINKING
  if ((grfKeyState & MK_CONTROL) == MK_CONTROL)
  {
    return DROPEFFECT_COPY;
  }

  return DROPEFFECT_MOVE;
}

HRESULT CDriveItemDataObject::_Extract_HGLOBAL(__in const STGMEDIUM& medium, __in size_t storageSize, __out void* object)
{
  if (medium.tymed != TYMED_HGLOBAL)
  {
    Log::WriteOutput(LogType::Error, L"CDriveItemDataObject::_Extract_HGLOBAL() not a HGLOBAL, weird");
    return DV_E_TYMED;
  }

  size_t size = GlobalSize(medium.hGlobal);

  if (storageSize > size)
  {
    Log::WriteOutput(LogType::Error, L"CDriveItemDataObject::_Extract_HGLOBAL() wrong size");
    return DV_E_STGMEDIUM;
  }

  LPVOID hGlobal = GlobalLock(medium.hGlobal);

  if (hGlobal == NULL)
  {
    Log::WriteOutput(LogType::Error, L"CDriveItemDataObject::_Extract_HGLOBAL() GlobalLock failed");
    return E_FAIL;
  }

  CopyMemory(object, hGlobal, storageSize);

  GlobalUnlock(medium.hGlobal);

  return S_OK;
}

HRESULT CDriveItemDataObject::_Extract_HGLOBAL(__in const STGMEDIUM& medium, __in size_t offset, __in size_t length, __out void* object)
{
  char* memBLOB = new char[offset + length];

  HRESULT hr = _Extract_HGLOBAL(medium, offset + length, memBLOB);

  if (SUCCEEDED(hr))
  {
    // this is a 2nd copy, lets just assume the list is small enough it doesn't matter
    CopyMemory(object, memBLOB + offset, length);
  }

  delete[] memBLOB;

  return hr;
}

HRESULT CDriveItemDataObject::_Create_CF_HDROP_FromList(__in std::vector<std::wstring>& fileList, __out FORMATETC& format, __out STGMEDIUM& medium)
{
  HRESULT hr = S_OK;

  if (fileList.size() == 0)
  {
    hr = E_FAIL;
  }
  else
  {
    _BuildFormat_CF_HDROP(format);

    // We are going to structure a list of c strings, that has a header of type DROPFILES, and a terminating null
    size_t length = sizeof(DROPFILES)+(fileList.size() + 1) * sizeof(TCHAR); // length of header + (number of string + final) * terminating NULL

    for (size_t i = 0; SUCCEEDED(hr) && i < fileList.size(); i++)
    {
      size_t len = 0;

      hr = StringCbLength(fileList[i].c_str(), STRSAFE_MAX_CCH * sizeof(TCHAR), &len);

      if (SUCCEEDED(hr))
      {
        length += len;
      }
    }

    if (SUCCEEDED(hr))
    {
      size_t size = length; // going to use length for error checking, making a copy for later
      char* buf = new char[size];

      if (buf == NULL)
      {
        return E_OUTOFMEMORY;
      }

      ZeroMemory(buf, size);

      DROPFILES* pDrop = (DROPFILES*)&buf[0];

      // Fill in the DROPFILES struct.
      pDrop->pFiles = sizeof(DROPFILES);

      // If we're compiling for Unicode, set the Unicode flag in the struct to
      // indicate it contains Unicode strings.
      pDrop->fWide = TRUE;

      TCHAR* pszBuff;
      pszBuff = (TCHAR*)(LPBYTE(pDrop) + sizeof(DROPFILES));

      length -= sizeof(DROPFILES);

      for (size_t i = 0; SUCCEEDED(hr) && i < fileList.size(); i++)
      {
        const wchar_t* path = fileList[i].c_str();
        size_t len;

        hr = StringCbLength(path, STRSAFE_MAX_CCH * sizeof(TCHAR), &len);

        if (SUCCEEDED(hr))
        {
          len += sizeof(TCHAR); // add terminating null
          hr = StringCbCopy(pszBuff, length, path);

          if (SUCCEEDED(hr))
          {
            pszBuff += (len / sizeof(TCHAR));
            length -= len;
          }
        }
      }

      length -= sizeof(TCHAR); // final terminating empty string

      if (length != 0)
      {
        hr = E_FAIL; // something strange happened, we should be at the end of our buffer
      }

      hr = _Create_HGLOBALSTGMEDIUMFromBuffer(buf, size, medium);

      delete[] buf;
    }
  }


  return hr;
}

HRESULT CDriveItemDataObject::_Create_CFSTR_PREFERREDDROPEFFECT(__in DWORD dropEffect, __out FORMATETC& format, __out STGMEDIUM& medium)
{
  _BuildFormat_CFSTR_PREFERREDDROPEFFECT(format);

  return _Create_HGLOBALSTGMEDIUMFromBuffer(&dropEffect, sizeof(DWORD), medium);
}

HRESULT CDriveItemDataObject::_Create_CFSTR_PERFORMEDDROPEFFECT(__in DWORD dropEffect, __out FORMATETC& format, __out STGMEDIUM& medium)
{
  _BuildFormat_CFSTR_PERFORMEDDROPEFFECT(format);

  return _Create_HGLOBALSTGMEDIUMFromBuffer(&dropEffect, sizeof(DWORD), medium);
}

HRESULT CDriveItemDataObject::_Create_CFSTR_FILEDESCRIPTOR(__in std::vector<WIN32_FIND_DATA>& win32FindData, __out FORMATETC& format, __out STGMEDIUM& medium)
{
  HRESULT hr = S_OK;

  if (win32FindData.size() == 0)
  {
    hr = E_FAIL;
  }
  else
  {
    _BuildFormat_CFSTR_FILEDESCRIPTOR(format);

    size_t size = sizeof(UINT) + sizeof(FILEDESCRIPTOR) * win32FindData.size();
    char* buf = new char[size];

    if (buf == NULL)
    {
      return E_OUTOFMEMORY;
    }

    ZeroMemory(buf, size);

    FILEGROUPDESCRIPTOR* fileGroup = (FILEGROUPDESCRIPTOR*)buf;
    fileGroup->cItems = (UINT)win32FindData.size();

    for (size_t i = 0; SUCCEEDED(hr) && i < fileGroup->cItems; i++)
    {
      WIN32_FIND_DATA* fileData = &win32FindData[i];
      FILEDESCRIPTOR* file = &fileGroup->fgd[i];

      hr = StringCbCopy(file->cFileName, MAX_PATH, fileData->cFileName);
      file->dwFileAttributes = fileData->dwFileAttributes;
      file->ftCreationTime = fileData->ftCreationTime;
      file->ftLastAccessTime = fileData->ftLastAccessTime;
      file->ftLastWriteTime = fileData->ftLastWriteTime;
      file->nFileSizeHigh = fileData->nFileSizeHigh;
      file->nFileSizeLow = fileData->nFileSizeLow;
      file->dwFlags = (DWORD)(FD_ATTRIBUTES | FD_CREATETIME | FD_ACCESSTIME | FD_WRITESTIME | FD_FILESIZE | FD_PROGRESSUI | FD_UNICODE);
    }

    hr = _Create_HGLOBALSTGMEDIUMFromBuffer(buf, size, medium);

    delete[] buf;
  }

  return hr;
}

HRESULT CDriveItemDataObject::_Create_CFSTR_FILECONTENTS(__in size_t lindex, IStream* stream, __out FORMATETC& format, __out STGMEDIUM& medium)
{
  _BuildFormat_CFSTR_FILECONTENTS(lindex, format);

  return _Create_ISTREAMSTGMEDIUM(stream, medium);
}

HRESULT CDriveItemDataObject::_Create_GOOGLEDRIVESOURCEID(__out FORMATETC& format, __out STGMEDIUM& medium)
{
  _BuildFormat_GOOGLEDRIVESOURCEID(format);

  char zeroBuf = 0;

  return _Create_HGLOBALSTGMEDIUMFromBuffer(&zeroBuf, 1, medium);
}

void CDriveItemDataObject::_BuildFormat_CF_HDROP(FORMATETC& format)
{
  ZeroMemory(&format, sizeof(FORMATETC));

  format.cfFormat = CF_HDROP;
  format.dwAspect = DVASPECT_CONTENT;
  format.lindex = -1;
  format.tymed = TYMED_HGLOBAL;
}

void CDriveItemDataObject::_BuildFormat_CFSTR_PREFERREDDROPEFFECT(FORMATETC& format)
{
  ZeroMemory(&format, sizeof(FORMATETC));

  format.cfFormat = (CLIPFORMAT)RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
  format.dwAspect = DVASPECT_CONTENT;
  format.lindex = -1;
  format.tymed = TYMED_HGLOBAL;
}

void CDriveItemDataObject::_BuildFormat_CFSTR_PERFORMEDDROPEFFECT(FORMATETC& format)
{
  ZeroMemory(&format, sizeof(FORMATETC));

  format.cfFormat = (CLIPFORMAT)RegisterClipboardFormat(CFSTR_PERFORMEDDROPEFFECT);
  format.dwAspect = DVASPECT_CONTENT;
  format.lindex = -1;
  format.tymed = TYMED_HGLOBAL;
}

void CDriveItemDataObject::_BuildFormat_CFSTR_FILEDESCRIPTOR(FORMATETC& format)
{
  ZeroMemory(&format, sizeof(FORMATETC));

  format.cfFormat = (CLIPFORMAT)RegisterClipboardFormat(CFSTR_FILEDESCRIPTOR);
  format.dwAspect = DVASPECT_CONTENT;
  format.lindex = -1;
  format.tymed = TYMED_HGLOBAL;
}

void CDriveItemDataObject::_BuildFormat_CFSTR_FILECONTENTS(size_t lindex, FORMATETC& format)
{
  ZeroMemory(&format, sizeof(FORMATETC));

  format.cfFormat = (CLIPFORMAT)RegisterClipboardFormat(CFSTR_FILECONTENTS);
  format.dwAspect = DVASPECT_CONTENT;
  format.lindex = (LONG)lindex;
  format.tymed = TYMED_ISTREAM;
}

void CDriveItemDataObject::_BuildFormat_GOOGLEDRIVESOURCEID(FORMATETC& format)
{
  ZeroMemory(&format, sizeof(FORMATETC));

  format.cfFormat = (CLIPFORMAT)RegisterClipboardFormat(kGoogleDriveSouceIdFormatName);
  format.dwAspect = DVASPECT_CONTENT;
  format.lindex = -1;
  format.tymed = TYMED_HGLOBAL;
}

HRESULT CDriveItemDataObject::_Create_HGLOBALSTGMEDIUMFromBuffer(__in void* buf, __in size_t length, __out STGMEDIUM& medium)
{
  ZeroMemory(&medium, sizeof(STGMEDIUM));
  medium.tymed = TYMED_HGLOBAL;

  HGLOBAL hBlob;
  void* blob;

  // Allocate memory from the heap
  hBlob = GlobalAlloc(GHND, length);

  if (hBlob == NULL)
  {
    return E_OUTOFMEMORY;
  }

  blob = GlobalLock(hBlob);

  if (blob == NULL)
  {
    GlobalFree(hBlob);
    return E_OUTOFMEMORY;
  }

  CopyMemory(blob, buf, length);

  GlobalUnlock(hBlob);

  medium.hGlobal = hBlob;

  return S_OK;
}

HRESULT CDriveItemDataObject::_Create_ISTREAMSTGMEDIUM(__in IStream* stream, __out STGMEDIUM& medium)
{
  ZeroMemory(&medium, sizeof(STGMEDIUM));
  medium.tymed = TYMED_ISTREAM;
  medium.pstm = stream;

  return S_OK;
}

HRESULT CDriveItemDataObject::_Convert_CFSTR_SHELLIDLIST_to_pidlList(__in STGMEDIUM *pmediumIn, __out std::vector<PCIDLIST_RELATIVE>& pidlList)
{
  HRESULT hr = E_FAIL;

  if (pmediumIn != NULL && pmediumIn->tymed == TYMED_HGLOBAL && pmediumIn->hGlobal != NULL)
  {
    LPIDA pida = (LPIDA)pmediumIn->hGlobal;
    CComPtr<IShellFolder> ppsf;
    LPCITEMIDLIST pidlParent = (LPCITEMIDLIST)(((LPBYTE)pida) + (pida)->aoffset[0]);

    hr = SHBindToObject(NULL, pidlParent, NULL, IID_PPV_ARGS(&ppsf));

    pidlList.resize(pida->cidl + 1);

    for (UINT i = 0; SUCCEEDED(hr) && i < pida->cidl + 1; i++) // +1, the parent element was first, and we are includeing it, it is also an PCIDLIST_ABSOLUTE
    {
      pidlList[i] = (LPCITEMIDLIST)(((LPBYTE)pida) + (pida)->aoffset[i]);
    }
  }

  return hr;
}

// IFileOperationProgressSink
HRESULT CDriveItemDataObject::StartOperations(void) {
  Log::WriteOutput(LogType::Error, L"IFileOperationProgressSink::StartOperations");
  return S_OK;
}

// IFileOperationProgressSink
HRESULT CDriveItemDataObject::FinishOperations(HRESULT /*hrResult*/) {
  Log::WriteOutput(LogType::Error, L"IFileOperationProgressSink::FinishOperations");
  return S_OK;
}

// IFileOperationProgressSink
HRESULT CDriveItemDataObject::PreRenameItem(DWORD /*dwFlags*/, __RPC__in_opt IShellItem * /*psiItem*/, __RPC__in_opt_string LPCWSTR /*pszNewName*/) {
  Log::WriteOutput(LogType::Error, L"IFileOperationProgressSink::PreRenameItem");
  return S_OK;
}

// IFileOperationProgressSink
HRESULT CDriveItemDataObject::PostRenameItem(DWORD /*dwFlags*/, __RPC__in_opt IShellItem * /*psiItem*/, __RPC__in_string LPCWSTR /*pszNewName*/, HRESULT /*hrRename*/, __RPC__in_opt IShellItem * /*psiNewlyCreated*/) {
  Log::WriteOutput(LogType::Error, L"IFileOperationProgressSink::PostRenameItem");
  return S_OK;
}

// IFileOperationProgressSink
HRESULT CDriveItemDataObject::PreMoveItem(DWORD /*dwFlags*/, __RPC__in_opt IShellItem * /*psiItem*/, __RPC__in_opt IShellItem * /*psiDestinationFolder*/, __RPC__in_opt_string LPCWSTR /*pszNewName*/) {
  Log::WriteOutput(LogType::Error, L"IFileOperationProgressSink::PreMoveItem");
  return S_OK;
}

// IFileOperationProgressSink
HRESULT CDriveItemDataObject::PostMoveItem(DWORD /*dwFlags*/, __RPC__in_opt IShellItem * /*psiItem*/, __RPC__in_opt IShellItem * /*psiDestinationFolder*/, __RPC__in_opt_string LPCWSTR /*pszNewName*/, HRESULT /*hrMove*/, __RPC__in_opt IShellItem * /*psiNewlyCreated*/) {
  Log::WriteOutput(LogType::Error, L"IFileOperationProgressSink::PostMoveItem");
  return S_OK;
}

// IFileOperationProgressSink
HRESULT CDriveItemDataObject::PreCopyItem(DWORD /*dwFlags*/, __RPC__in_opt IShellItem * /*psiItem*/, __RPC__in_opt IShellItem * /*psiDestinationFolder*/, __RPC__in_opt_string LPCWSTR /*pszNewName*/) {
  Log::WriteOutput(LogType::Error, L"IFileOperationProgressSink::PreCopyItem");
  return S_OK;
}

// IFileOperationProgressSink
HRESULT CDriveItemDataObject::PostCopyItem(DWORD /*dwFlags*/, __RPC__in_opt IShellItem * /*psiItem*/, __RPC__in_opt IShellItem * /*psiDestinationFolder*/, __RPC__in_opt_string LPCWSTR /*pszNewName*/, HRESULT /*hrCopy*/, __RPC__in_opt IShellItem * /*psiNewlyCreated*/) {
  Log::WriteOutput(LogType::Error, L"IFileOperationProgressSink::PostCopyItem");
  return S_OK;
}

// IFileOperationProgressSink
HRESULT CDriveItemDataObject::PreDeleteItem(DWORD /*dwFlags*/, __RPC__in_opt IShellItem * /*psiItem*/) {
  Log::WriteOutput(LogType::Error, L"IFileOperationProgressSink::PreDeleteItem");
  return S_OK;
}

// IFileOperationProgressSink
HRESULT CDriveItemDataObject::PostDeleteItem(DWORD /*dwFlags*/, __RPC__in_opt IShellItem * /*psiItem*/, HRESULT /*hrDelete*/, __RPC__in_opt IShellItem * /*psiNewlyCreated*/) {
  Log::WriteOutput(LogType::Error, L"IFileOperationProgressSink::PostDeleteItem");
  return S_OK;
}

// IFileOperationProgressSink
HRESULT CDriveItemDataObject::PreNewItem(DWORD /*dwFlags*/, __RPC__in_opt IShellItem * /*psiDestinationFolder*/, __RPC__in_opt_string LPCWSTR /*pszNewName*/) {
  Log::WriteOutput(LogType::Error, L"IFileOperationProgressSink::PreNewItem");
  return S_OK;
}

// IFileOperationProgressSink
HRESULT CDriveItemDataObject::PostNewItem(DWORD /*dwFlags*/, __RPC__in_opt IShellItem * /*psiDestinationFolder*/, __RPC__in_opt_string LPCWSTR /*pszNewName*/, __RPC__in_opt_string LPCWSTR /*pszTemplateName*/, DWORD /*dwFileAttributes*/, HRESULT /*hrNew*/, __RPC__in_opt IShellItem * /*psiNewItem*/) {
  Log::WriteOutput(LogType::Error, L"IFileOperationProgressSink::PostNewItem");
  return S_OK;
}

// IFileOperationProgressSink
HRESULT CDriveItemDataObject::UpdateProgress(UINT /*iWorkTotal*/, UINT /*iWorkSoFar*/) {
  Log::WriteOutput(LogType::Error, L"IFileOperationProgressSink::UpdateProgress");
  return S_OK;
}

// IFileOperationProgressSink
HRESULT CDriveItemDataObject::ResetTimer(void) {
  Log::WriteOutput(LogType::Error, L"IFileOperationProgressSink::ResetTimer");
  return S_OK;
}

// IFileOperationProgressSink
HRESULT CDriveItemDataObject::PauseTimer(void) {
  Log::WriteOutput(LogType::Error, L"IFileOperationProgressSink::PauseTimer");
  return S_OK;
}

// IFileOperationProgressSink
HRESULT CDriveItemDataObject::ResumeTimer(void) {
  Log::WriteOutput(LogType::Error, L"IFileOperationProgressSink::ResumeTimer");
  return S_OK;
}
