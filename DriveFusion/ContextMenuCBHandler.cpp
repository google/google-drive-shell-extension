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
#include "stdafx.h"
#include <fstream>
#include "ContextMenuCBHandler.h"
#include "GDriveShlExt.h"

namespace
{
  void AddDefaultMenuItemForGDocs(QCMINFO & qcmInfo)
  {
    const std::wstring cmdVerb = L"Open in Browser";
    const MENUITEMINFO menuItemInfo =
    {
      sizeof(MENUITEMINFO),
      MIIM_STATE|MIIM_ID|MIIM_STRING,
      0,
      MFS_DEFAULT,
      qcmInfo.idCmdFirst++,
      NULL,
      NULL,
      NULL,
      NULL,
      (LPWSTR)cmdVerb.c_str(),
      (UINT)cmdVerb.size(),
      NULL
    };
    InsertMenuItem(qcmInfo.hmenu, qcmInfo.indexMenu, S_FALSE, &menuItemInfo);
  }

  std::wstring GetUrlFromGDoc(const std::wstring& gDoc)
  {
    const std::wstring urlId = L"\"url\"";
    const auto idPos = gDoc.find(urlId);
    if (idPos == std::wstring::npos)
      return L"";

    const auto urlStart = gDoc.find(L"\"", idPos+urlId.size());
    if (urlStart == std::wstring::npos)
      return L"";

    const auto urlEnd = gDoc.find(L"\"", urlStart+1);
    if (urlEnd == std::wstring::npos)
      return L"";

    return gDoc.substr(urlStart+1, urlEnd-urlStart-1);
  }

  HRESULT OpenGDocInBrowser(const std::wstring& filename)
  {
    std::wifstream gDocFile (filename);
    const std::wstring gDocString ((std::istreambuf_iterator<wchar_t>(gDocFile) ),
                                   (std::istreambuf_iterator<wchar_t>()));
    const std::wstring gDocUrl = GetUrlFromGDoc(gDocString);
    if (gDocUrl.empty())
      return S_FALSE;

    ShellExecute(NULL, L"open", gDocUrl.c_str(), NULL, NULL, SW_SHOWNORMAL);
    return S_OK;
  }
}

HRESULT ContextMenuCBHandler::CreateInstance(CGDriveShlExt *gDriveShlExt, __deref_out CComObject<ContextMenuCBHandler> **ppv)
{
  Log::WriteOutput(LogType::Information, L"ContextMenuCBHandler::CreateInstance()");

  *ppv = NULL;

  CComObject<ContextMenuCBHandler> *psprfvcb;

  HRESULT hr = CComObject<ContextMenuCBHandler>::CreateInstance(&psprfvcb);

  if (!SUCCEEDED(hr))
  {
    Log::WriteOutput(LogType::Error, L"CComObject<ContextMenuCBHandler>::CreateInstance returned hr=%d", hr);
  }
  else
  {
    psprfvcb->AddRef();
    hr = psprfvcb->_Initialize(gDriveShlExt);

    (*ppv) = psprfvcb;
  }

  return hr;
}


HRESULT ContextMenuCBHandler::ReturnInterfaceTo(REFIID riid, __deref_out void **ppv)
{
  HRESULT hr = this->QueryInterface(riid, ppv);

  if (!SUCCEEDED(hr))
  {
    Log::LogUnknownIID(riid, L"ContextMenuCBHandler::CreateInstanceReturnInterfaceTo()");
  }

  return hr;
}

void ContextMenuCBHandler::FinalRelease()
{
  Log::WriteOutput(LogType::Information, L"GDriveCBHandler::FinalRelease()");

  _ReleaseContextMenuSelection();

  if (_gDriveShlExt != NULL)
  {
    _gDriveShlExt->Release();
    _gDriveShlExt = NULL;
  }
}

HRESULT ContextMenuCBHandler::_Initialize(CGDriveShlExt* gDriveShlExt)
{
  Log::WriteOutput(LogType::Information, L"ContextMenuCBHandler::_Initialize(CGDriveShlExt* gDriveShlExt)");

  _gDriveShlExt = gDriveShlExt;

  _gDriveShlExt->AddRef();

  _pdo = NULL;

  _downloadWhenReady = false;

  return S_OK;
}

void ContextMenuCBHandler::DownloadFiles()
{
  _downloadWhenReady = true;

  _DownloadFilesIfReady();
}

STDMETHODIMP ContextMenuCBHandler::CallBack(__in_opt IShellFolder * /*psf*/, HWND hwndOwner, __in_opt IDataObject *pdo, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  try
  {
    std::wstring contextMenuCSString = _GetContextMenuCBString(uiMsg, wParam, lParam);

    if (contextMenuCSString.find(L"UNKNOWN") != std::wstring::npos)
    {
      Log::WriteOutput(LogType::Warning, L"CGDriveShlExt::CallBack %s UIMsg: %d", contextMenuCSString.c_str(), uiMsg);
    }
    else
    {
      Log::WriteOutput(LogType::Warning, L"CGDriveShlExt::CallBack %s", contextMenuCSString.c_str());
    }

    if (pdo != NULL && pdo != _pdo)
    {
      _ReleaseContextMenuSelection();

      _pdo = pdo;

      _pdo->AddRef();
    }

    _DownloadFilesIfReady();

    _gDriveShlExt->SetDialogType(hwndOwner);

    HRESULT hr = E_NOTIMPL;

    switch (uiMsg)
    {
    case DFM_MERGECONTEXTMENU:
      {
        if (_PdoIsGoogleDoc(*pdo))
        {
          AddDefaultMenuItemForGDocs(*(LPQCMINFO)lParam);
        }
      }
      hr = S_OK;
      break;
    case DFM_MERGECONTEXTMENU_TOP: // Not documented
      hr = S_OK;
      break;
    case DFM_MERGECONTEXTMENU_BOTTOM:
      hr = S_OK;
      break;

    case DFM_INVOKECOMMAND:
    case DFM_INVOKECOMMANDEX:
      {
        std::vector<CDriveItem> itemList;
        _ConvertDataObjectToVectorOfDriveItems(pdo, &itemList);
        if (!itemList.empty() && itemList[0].IsGoogleDoc())
        {
          auto fileInfo = itemList[0].FileInfo();
          hr = OpenGDocInBrowser(fileInfo->FilePath());
        }
        else
          hr = S_FALSE;
      }
      break;

    case DFM_GETHELPTEXTW:
      {
        if (_PdoIsGoogleDoc(*pdo))
        {
          WORD cchMax = (DWORD)wParam >> sizeof(WORD)*8;
          wcscpy_s((wchar_t*)lParam, cchMax, L"Open document in browser");
          hr = S_OK;
        }
      }
      break;
    case DFM_GETVERBW:
      {
        if (_PdoIsGoogleDoc(*pdo))
        {
          WORD cchMax = (DWORD)wParam >> sizeof(WORD)*8;
          wcscpy_s((wchar_t*)lParam, cchMax, L"OpenInBrowser");
          hr = S_OK;
        }
      }
      break;

    case DFM_MODIFYQCMFLAGS :
      // Allows the callback to add items to the bottom of the extended menu.
      *(UINT*)lParam = (UINT)wParam;
      hr = S_OK;
      break;

    case 3: // MENU_BEGIN
      _ReleaseContextMenuSelection(); // We'll get the new selection in a future command
      break;
    case 4: // MENU_END
      _ReleaseContextMenuSelection();
      _gDriveShlExt->ReleaseContextMenuCBHandler(this);
      break;
    }

    return hr;
  }
  catch (...)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::Callback unhandled exception");

    return E_FAIL;
  }
}

HRESULT ContextMenuCBHandler::_ConvertDataObjectToVectorOfDriveItems(__in IDataObject *pdo, std::vector<CDriveItem>* driveItemList) const
{
  if (driveItemList == NULL || driveItemList->size() > 0)
  {
    Log::WriteOutput(LogType::Error, L"CGDriveShlExt::_ConvertDataObjectToVectorOfDriveItems invalid vector argument");
    return E_INVALIDARG;
  }

  CComPtr<IShellItemArray> spsv;
  HRESULT hr = SHCreateShellItemArrayFromDataObject(pdo, IID_PPV_ARGS(&spsv));
  if (!SUCCEEDED(hr))
    return hr;

  DWORD count = 0;
  hr = spsv->GetCount(&count);
  if (!SUCCEEDED(hr))
    return hr;

  driveItemList->resize(count);
  for (DWORD i = 0; i < count; ++i)
  {
    CComPtr<IShellItem> si;

    hr = spsv->GetItemAt(i, &si);

    if (!SUCCEEDED(hr))
      continue;

    PIDLIST_ABSOLUTE pidl;
    hr = SHGetIDListFromObject(si, &pidl);

    if (!SUCCEEDED(hr))
      continue;

    _gDriveShlExt->GetDriveItemFromIDList(pidl, false, false, driveItemList->at(i));
  }

  return hr;
}

void ContextMenuCBHandler::_ReleaseContextMenuSelection()
{
  if (_pdo != NULL)
  {
    _pdo->Release();
    _pdo = NULL;
  }
}

void ContextMenuCBHandler::_DownloadFilesIfReady()
{
  if (_pdo != NULL && (_downloadWhenReady || _gDriveShlExt->GetLastViewUIMsg() == 33))
  {
    // Clear flags immediately, so it won't trigger again by a successive case
    _gDriveShlExt->SetLastViewUIMsg(0);
    _downloadWhenReady = false;

    std::vector<CDriveItem> driveItemList;

    _ConvertDataObjectToVectorOfDriveItems(_pdo, &driveItemList);

    for (size_t i = 0; i < driveItemList.size(); i++)
    {
      _gDriveShlExt->DoDownload(driveItemList.at(i).FileInfo());
    }
  }
}

bool ContextMenuCBHandler::_PdoIsGoogleDoc(IDataObject& pdo) const
{
  std::vector<CDriveItem> itemList;
  _ConvertDataObjectToVectorOfDriveItems(&pdo, &itemList);
  return !itemList.empty() && itemList[0].IsGoogleDoc();
}

std::wstring ContextMenuCBHandler::_GetContextMenuCBString(UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uiMsg)
  {
  case 3:
    return L"MENU_BEGIN";
  case 4:
    return L"MENU_END";
  case DFM_MERGECONTEXTMENU:
    // Allows the callback to add items to the menu.
    return L"DFM_MERGECONTEXTMENU";
  case DFM_GETHELPTEXT:
    // Allows the callback object to specify a help text string.
    return L"DFM_GETHELPTEXT";
  case DFM_GETHELPTEXTW:
    // Allows the callback object to specify a help text string.
    return L"DFM_GETHELPTEXTW";
  case DFM_WM_MEASUREITEM:
    // Sent to the owner window of a control or menu item when the control or menu is created.
    return L"DFM_WM_MEASUREITEM";
  case DFM_WM_DRAWITEM:
    // Sent to the parent window of an owner-drawn control or menu when a visual aspect of the control or menu has changed.
    return L"DFM_WM_DRAWITEM";
  case DFM_WM_INITMENUPOPUP:
    // Sent when a drop-down menu or submenu is about to become active. This allows an application to modify the menu before it is displayed, without changing the entire menu.
    return L"DFM_WM_INITMENUPOPUP";
  case DFM_VALIDATECMD:
    // Sent to verify the existence of a menu command.
    return L"DFM_VALIDATECMD";
  case DFM_INVOKECOMMAND:
  case DFM_INVOKECOMMANDEX:
    // Sent by the default context menu implementation to request LPFNDFMCALLBACK to invoke an extended menu command.
    {
      int idCmd = (int)wParam;
      LPWSTR args = NULL;
      PDFMICS pdfmics = NULL;

      if (uiMsg == DFM_INVOKECOMMAND)
      {
        args = (LPWSTR)lParam;
      }
      else if (uiMsg == DFM_INVOKECOMMANDEX)
      {
        pdfmics = (PDFMICS)lParam;
      }
      switch (idCmd)
      {
      case DFM_CMD_DELETE:
        // Windows Vista and later. Delete the current item.
        return L"DFM_INVOKECOMMAND/DFM_INVOKECOMMANDEX DFM_CMD_DELETE";
      case DFM_CMD_MOVE:
        // Windows Vista and later. Move the current item.
        return L"DFM_INVOKECOMMAND/DFM_INVOKECOMMANDEX DFM_CMD_MOVE";
      case DFM_CMD_COPY:
        // Windows Vista and later. Copy the current item.
        return L"DFM_INVOKECOMMAND/DFM_INVOKECOMMANDEX DFM_CMD_COPY";
      case DFM_CMD_LINK:
        // Windows Vista and later. Create a link to the current item.
        return L"DFM_INVOKECOMMAND/DFM_INVOKECOMMANDEX DFM_CMD_LINK";
      case DFM_CMD_PROPERTIES:
        // Show the Properties UI for the item on which the menu was invoked.
        return L"DFM_INVOKECOMMAND/DFM_INVOKECOMMANDEX DFM_CMD_PROPERTIES";
      case DFM_CMD_NEWFOLDER:
        // Not supported.
        return L"DFM_INVOKECOMMAND/DFM_INVOKECOMMANDEX DFM_CMD_NEWFOLDER";
      case DFM_CMD_PASTE:
        // Windows Vista and later. Paste an item to the current location.
        return L"DFM_INVOKECOMMAND/DFM_INVOKECOMMANDEX DFM_CMD_PASTE";
      case DFM_CMD_VIEWLIST:
        // Not supported.
        return L"DFM_INVOKECOMMAND/DFM_INVOKECOMMANDEX DFM_CMD_VIEWLIST";
      case DFM_CMD_VIEWDETAILS:
        // Not supported.
        return L"DFM_INVOKECOMMAND/DFM_INVOKECOMMANDEX DFM_CMD_VIEWDETAILS";
      case DFM_CMD_PASTELINK:
        // Windows Vista and later. Paste a link at the current location.
        return L"DFM_INVOKECOMMAND/DFM_INVOKECOMMANDEX DFM_CMD_PASTELINK";
      case DFM_CMD_PASTESPECIAL:
        // Not supported.
        return L"DFM_INVOKECOMMAND/DFM_INVOKECOMMANDEX DFM_CMD_PASTESPECIAL";
      case DFM_CMD_MODALPROP:
        // Not supported.
        return L"DFM_INVOKECOMMAND/DFM_INVOKECOMMANDEX DFM_CMD_MODALPROP";
      case DFM_CMD_RENAME:
        // Windows Vista and later. Rename the current item.
        return L"DFM_INVOKECOMMAND/DFM_INVOKECOMMANDEX DFM_CMD_RENAME";
      default:
        return L"DFM_INVOKECOMMAND/DFM_INVOKECOMMANDEX DFM_CMD_UNKNOWN";
      };
      break;
    }
  case DFM_MAPCOMMANDNAME:
    // Sent by the default context menu implementation to assign a name to a menu command.
    return L"DFM_MAPCOMMANDNAME";
  case DFM_GETDEFSTATICID:
    // Sent by the default context menu implementation during creation, specifying the default menu command and allowing an alternate choice to be made. Used by LPFNDFMCALLBACK.
    return L"DFM_GETDEFSTATICID";
  case DFM_GETVERBA:
    // Sent by the default context menu implementation to get the verb for the given command ID in the context menu.
    return L"DFM_GETVERBA";
  case DFM_GETVERBW:
    // Sent by the default context menu implementation to get the verb for the given command ID in the context menu.
    return L"DFM_GETVERBW";
  case DFM_MERGECONTEXTMENU_BOTTOM:
    // Allows the callback to add items to the bottom of the extended menu.
    return L"DFM_MERGECONTEXTMENU_BOTTOM";
  case DFM_MERGECONTEXTMENU_TOP:
    // Allows the callback to add items to the top of the extended menu.
    return L"DFM_MERGECONTEXTMENU_TOP";
  case DFM_MODIFYQCMFLAGS:
    // Allows the callback to modify the CFM_XXX values passed to IContextMenu::QueryContextMenu
    return L"DFM_MODIFYQCMFLAGS";
  default:
    return L"DFM_UNKNOWN";
  }
}
