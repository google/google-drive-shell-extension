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
#include "ShellFolderViewCBHandler.h"

// Pulled these undocumented items from WINE source code
#define SFVM_SELECTIONCHANGED          8 /* undocumented */
#define SFVM_DRAWMENUITEM              9 /* undocumented */
#define SFVM_MEASUREMENUITEM          10 /* undocumented */
#define SFVM_EXITMENULOOP             11 /* undocumented */
#define SFVM_VIEWRELEASE              12 /* undocumented */
#define SFVM_GETNAMELENGTH            13 /* undocumented */

#define SFVM_WINDOWCLOSING            16 /* undocumented */
#define SFVM_LISTREFRESHED            17 /* undocumented */
#define SFVM_WINDOWFOCUSED            18 /* undocumented */
#define SFVM_REGISTERCOPYHOOK         20 /* undocumented */
#define SFVM_COPYHOOKCALLBACK         21 /* undocumented */

#define SFVM_ADDINGOBJECT             29 /* undocumented */
#define SFVM_REMOVINGOBJECT           30 /* undocumented */

#define SFVM_GETCOMMANDDIR            33 /* undocumented */
#define SFVM_GETCOLUMNSTREAM          34 /* undocumented */
#define SFVM_CANSELECTALL             35 /* undocumented */

#define SFVM_ISSTRICTREFRESH          37 /* undocumented */
#define SFVM_ISCHILDOBJECT            38 /* undocumented */

#define SFVM_GETEXTVIEWS              40 /* undocumented */

#define SFVM_GET_CUSTOMVIEWINFO       77 /* undocumented */
#define SFVM_ENUMERATEDITEMS          79 /* undocumented */
#define SFVM_GET_VIEW_DATA            80 /* undocumented */
#define SFVM_GET_WEBVIEW_LAYOUT       82 /* undocumented */
#define SFVM_GET_WEBVIEW_CONTENT      83 /* undocumented */
#define SFVM_GET_WEBVIEW_TASKS        84 /* undocumented */
#define SFVM_GET_WEBVIEW_THEME        86 /* undocumented */
#define SFVM_GETDEFERREDVIEWSETTINGS  92 /* undocumented */

HRESULT ShellFolderViewCBHandler::CreateInstanceReturnInterfaceTo(CGDriveShlExt *gDriveShlExt, REFIID riid, __deref_out void **ppv)
{
  Log::WriteOutput(LogType::Debug, L"ShellFolderViewCBHandler::CreateInstance()");

  *ppv = NULL;

  CComObject<ShellFolderViewCBHandler> *psprfvcb;

  HRESULT hr = CComObject<ShellFolderViewCBHandler>::CreateInstance(&psprfvcb);

  if (!SUCCEEDED(hr))
  {
    Log::WriteOutput(LogType::Error, L"CComObject<ShellFolderViewCBHandler>::CreateInstance returned hr=%d", hr);
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
        Log::LogUnknownIID(riid, L"ShellFolderViewCBHandler::CreateInstanceReturnInterfaceTo()");
      }
    }

    psprfvcb->Release();
  }

  return hr;
}

HRESULT ShellFolderViewCBHandler::_Initialize(CGDriveShlExt* gDriveShlExt)
{
  Log::WriteOutput(LogType::Debug, L"ShellFolderViewCBHandler::_Initialize(CGDriveShlExt* gDriveShlExt)");

  HRESULT hr = gDriveShlExt->GetCurFolder(&_spidl);

  if (SUCCEEDED(hr))
  {
    _gDriveShlExt = gDriveShlExt;

    _gDriveShlExt->AddRef();
  }

  return hr;
}

IFACEMETHODIMP ShellFolderViewCBHandler::GetColumnPropertyList(__in REFIID /*riid*/, __deref_out_opt void **ppv)
{
  Log::WriteOutput(LogType::Debug, L"ShellFolderViewCBHandler::GetColumnPropertyList(__in REFIID riid, __deref_out_opt void **ppv)");

  *ppv = NULL;

  return E_NOTIMPL;
}

IFACEMETHODIMP ShellFolderViewCBHandler::GetGroupByProperty(__out PROPERTYKEY * /*pkey*/, __out BOOL * /*pfGroupAscending*/)
{
  Log::WriteOutput(LogType::Debug, L"ShellFolderViewCBHandler::GetGroupByProperty(__out PROPERTYKEY *pkey, __out BOOL *pfGroupAscending)");

  return E_NOTIMPL;
}

IFACEMETHODIMP ShellFolderViewCBHandler::GetViewMode(__out FOLDERLOGICALVIEWMODE * /*plvm*/)
{
  Log::WriteOutput(LogType::Debug, L"ShellFolderViewCBHandler::GetViewMode(__out FOLDERLOGICALVIEWMODE *plvm)");

  return E_NOTIMPL;
}

IFACEMETHODIMP ShellFolderViewCBHandler::GetIconSize(__out UINT * /*puIconSize*/)
{
  Log::WriteOutput(LogType::Debug, L"ShellFolderViewCBHandler::GetIconSize(__out UINT *puIconSize)");

  return E_NOTIMPL;
}

IFACEMETHODIMP ShellFolderViewCBHandler::GetFolderFlags(__out FOLDERFLAGS * /*pfolderMask*/, __out FOLDERFLAGS * /*pfolderFlags*/)
{
  Log::WriteOutput(LogType::Debug, L"ShellFolderViewCBHandler::GetFolderFlags(__out FOLDERFLAGS *pfolderMask, __out FOLDERFLAGS *pfolderFlags)");

  return E_NOTIMPL;
}

IFACEMETHODIMP ShellFolderViewCBHandler::GetSortColumns(__out_ecount_part(/*cColumnsIn*/, * /*pcColumnsOut*/) SORTCOLUMN * /*rgSortColumns*/, UINT /*cColumnsIn*/, __out UINT * /*pcColumnsOut*/)
{
  Log::WriteOutput(LogType::Debug, L"ShellFolderViewCBHandler::GetSortColumns(__out_ecount_part(cColumnsIn, *pcColumnsOut) SORTCOLUMN *rgSortColumns, UINT cColumnsIn, __out UINT *pcColumnsOut)");

  return E_NOTIMPL;
}

IFACEMETHODIMP ShellFolderViewCBHandler::GetGroupSubsetCount(__out UINT * /*pcVisibleRows*/)
{
  Log::WriteOutput(LogType::Debug, L"ShellFolderViewCBHandler::GetGroupSubsetCount(__out UINT *pcVisibleRows)");

  return E_NOTIMPL;
}

IFACEMETHODIMP ShellFolderViewCBHandler::MessageSFVCB(UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  std::wstring sfvCBString = _GetMsgString(uiMsg, wParam, lParam);

  if (sfvCBString.find(L"UNKNOWN") != std::wstring::npos)
  {
    Log::WriteOutput(LogType::Information, L"ShellFolderViewCBHandler::MessageSFVCB %s UIMsg: %d", sfvCBString.c_str(), uiMsg);
  }
  else if (sfvCBString.find(L"SFVM_GETCOMMANDDIR") != std::wstring::npos || sfvCBString.find(L"SFVM_SELECTIONCHANGED") != std::wstring::npos)
  {
    Log::WriteOutput(LogType::Information, L"ShellFolderViewCBHandler::MessageSFVCB %s UIMsg: %d %d %d", sfvCBString.c_str(), uiMsg, wParam, lParam);
  }
  else
  {
    Log::WriteOutput(LogType::Information, L"ShellFolderViewCBHandler::MessageSFVCB %s", sfvCBString.c_str());
  }

  HRESULT hr = E_NOTIMPL; // signal caller to perform the default action

  switch (uiMsg)
  {
  case SFVM_GETCOMMANDDIR:
    _gDriveShlExt->SetLastViewUIMsg(uiMsg);
    // _DownloadFilesIfCommandReady();
    break;
  case SFVM_FSNOTIFY:
    // Notifies the callback object that an event has taken place that affects one of its items.
    {
      DWORD lEvent = (DWORD)lParam;
      std::wstring id = L"";

      switch (lEvent)
      {
      case SHCNE_DELETE:
        break;
      };

      break;
    }

  case SFVM_GETNOTIFY:
    // Specifies which events will generate an SFVM_FSNOTIFY message for a given item.
    *((LPITEMIDLIST*)wParam) = _spidl;  // evil weak reference
    *((LONG*)lParam) = SHCNE_ALLEVENTS;
    hr = S_OK;
    break;

  case SFVM_QUERYFSNOTIFY:
    break;
  }

  return hr;
}

std::wstring ShellFolderViewCBHandler::_GetMsgString(UINT uiMsg, WPARAM /*wParam*/, LPARAM lParam)
{
  switch (uiMsg)
  {
  case SFVM_ADDPROPERTYPAGES:
    // Allows the callback object to provide a page to add to the Properties property sheet of the selected object.
    return L"SFVM_ADDPROPERTYPAGES";
  case SFVM_BACKGROUNDENUM:
    // Allows the callback object to request that enumeration be done on a background thread.
    return L"SFVM_BACKGROUNDENUM";
  case SFVM_BACKGROUNDENUMDONE:
    // Notifies the callback object that background enumeration is complete.
    return L"SFVM_BACKGROUNDENUMDONE";
  case SFVM_COLUMNCLICK:
    // Notifies the callback object that the user has clicked a column header to sort the list of objects in the folder view.
    return L"SFVM_COLUMNCLICK";
  case SFVM_DEFITEMCOUNT:
    // Allows the callback object to specify the number of items in the folder view.
    return L"SFVM_DEFITEMCOUNT";
  case SFVM_DEFVIEWMODE:
    // Allows the callback object to specify the view mode.
    return L"SFVM_DEFVIEWMODE";
  case SFVM_DIDDRAGDROP:
    // Notifies the callback function that a drag-and-drop operation has begun.
    return L"SFVM_DIDDRAGDROP";

  case SFVM_FSNOTIFY:
    // Notifies the callback object that an event has taken place that affects one of its items.
    {
      DWORD lEvent = (DWORD)lParam;

      switch (lEvent)
      {
      case SHCNE_ASSOCCHANGED:
        return L"SFVM_FSNOTIFY SHCNE_ASSOCCHANGED";
      case SHCNE_ATTRIBUTES:
        // The attributes of an item or folder have changed. SHCNF_IDLIST or SHCNF_PATH must be specified in uFlags. dwItem1 contains the item or folder that has changed. dwItem2 is not used and should be NULL.
        return L"SFVM_FSNOTIFY SHCNE_ATTRIBUTES";
      case SHCNE_CREATE:
        // A nonfolder item has been created. SHCNF_IDLIST or SHCNF_PATH must be specified in uFlags. dwItem1 contains the item that was created. dwItem2 is not used and should be NULL.
        return L"SFVM_FSNOTIFY SHCNE_CREATE";
      case SHCNE_DELETE:
        // A nonfolder item has been deleted. SHCNF_IDLIST or SHCNF_PATH must be specified in uFlags. dwItem1 contains the item that was deleted. dwItem2 is not used and should be NULL.
        return L"SFVM_FSNOTIFY SHCNE_DELETE";
      case SHCNE_DRIVEADD:
        // A drive has been added. SHCNF_IDLIST or SHCNF_PATH must be specified in uFlags. dwItem1 contains the root of the drive that was added. dwItem2 is not used and should be NULL.
        return L"SFVM_FSNOTIFY SHCNE_DRIVEADD";
      case SHCNE_DRIVEADDGUI:
        // Windows XP and later: Not used.
        return L"SFVM_FSNOTIFY SHCNE_DRIVEADDGUI";
      case SHCNE_DRIVEREMOVED:
        // A drive has been removed. SHCNF_IDLIST or SHCNF_PATH must be specified in uFlags. dwItem1 contains the root of the drive that was removed. dwItem2 is not used and should be NULL.
        return L"SFVM_FSNOTIFY SHCNE_DRIVEREMOVED";
      case SHCNE_EXTENDED_EVENT:
        // Not currently used.
        return L"SFVM_FSNOTIFY SHCNE_EXTENDED_EVENT";
      case SHCNE_FREESPACE:
        // The amount of free space on a drive has changed. SHCNF_IDLIST or SHCNF_PATH must be specified in uFlags. dwItem1 contains the root of the drive on which the free space changed. dwItem2 is not used and should be NULL.
        return L"SFVM_FSNOTIFY SHCNE_FREESPACE";
      case SHCNE_MEDIAINSERTED:
        // Storage media has been inserted into a drive. SHCNF_IDLIST or SHCNF_PATH must be specified in uFlags. dwItem1 contains the root of the drive that contains the new media. dwItem2 is not used and should be NULL.
        return L"SFVM_FSNOTIFY SHCNE_MEDIAINSERTED";
      case SHCNE_MEDIAREMOVED:
        // Storage media has been removed from a drive. SHCNF_IDLIST or SHCNF_PATH must be specified in uFlags. dwItem1 contains the root of the drive from which the media was removed. dwItem2 is not used and should be NULL.
        return L"SFVM_FSNOTIFY SHCNE_MEDIAREMOVED";
      case SHCNE_MKDIR:
        // A folder has been created. SHCNF_IDLIST or SHCNF_PATH must be specified in uFlags. dwItem1 contains the folder that was created. dwItem2 is not used and should be NULL.
        return L"SFVM_FSNOTIFY SHCNE_MKDIR";
      case SHCNE_NETSHARE:
        // A folder on the local computer is being shared via the network. SHCNF_IDLIST or SHCNF_PATH must be specified in uFlags. dwItem1 contains the folder that is being shared. dwItem2 is not used and should be NULL.
        return L"SFVM_FSNOTIFY SHCNE_NETSHARE";
      case SHCNE_NETUNSHARE:
        // A folder on the local computer is no longer being shared via the network. SHCNF_IDLIST or SHCNF_PATH must be specified in uFlags. dwItem1 contains the folder that is no longer being shared. dwItem2 is not used and should be NULL.
        return L"SFVM_FSNOTIFY SHCNE_NETUNSHARE";
      case SHCNE_RENAMEFOLDER:
        // The name of a folder has changed. SHCNF_IDLIST or SHCNF_PATH must be specified in uFlags. dwItem1 contains the previous PIDL or name of the folder. dwItem2 contains the new PIDL or name of the folder.
        return L"SFVM_FSNOTIFY SHCNE_RENAMEFOLDER";
      case SHCNE_RENAMEITEM:
        // The name of a nonfolder item has changed. SHCNF_IDLIST or SHCNF_PATH must be specified in uFlags. dwItem1 contains the previous PIDL or name of the item. dwItem2 contains the new PIDL or name of the item.
        return L"SFVM_FSNOTIFY SHCNE_RENAMEITEM";
      case SHCNE_RMDIR:
        // A folder has been removed. SHCNF_IDLIST or SHCNF_PATH must be specified in uFlags. dwItem1 contains the folder that was removed. dwItem2 is not used and should be NULL.
        return L"SFVM_FSNOTIFY SHCNE_RMDIR";
      case SHCNE_SERVERDISCONNECT:
        // The computer has disconnected from a server. SHCNF_IDLIST or SHCNF_PATH must be specified in uFlags. dwItem1 contains the server from which the computer was disconnected. dwItem2 is not used and should be NULL.
        return L"SFVM_FSNOTIFY SHCNE_SERVERDISCONNECT";
      case SHCNE_UPDATEDIR:
        // The contents of an existing folder have changed, but the folder still exists and has not been renamed. SHCNF_IDLIST or SHCNF_PATH must be specified in uFlags. dwItem1 contains the folder that has changed. dwItem2 is not used and should be NULL. If a folder has been created, deleted, or renamed, use SHCNE_MKDIR, SHCNE_RMDIR, or SHCNE_RENAMEFOLDER, respectively.
        return L"SFVM_FSNOTIFY SHCNE_UPDATEDIR";
      case SHCNE_UPDATEIMAGE:
        // An image in the system image list has changed. SHCNF_DWORD must be specified in uFlags. dwItem2 contains the index in the system image list that has changed. dwItem1 is not used and should be NULL.
        return L"SFVM_FSNOTIFY SHCNE_UPDATEIMAGE";
      case SHCNE_UPDATEITEM:
        // An existing item (a folder or a nonfolder) has changed, but the item still exists and has not been renamed. SHCNF_IDLIST or SHCNF_PATH must be specified in uFlags. dwItem1 contains the item that has changed. dwItem2 is not used and should be NULL. If a nonfolder item has been created, deleted, or renamed, use SHCNE_CREATE, SHCNE_DELETE, or SHCNE_RENAMEITEM, respectively, instead.
        return L"SFVM_FSNOTIFY SHCNE_UPDATEITEM";
      case SHCNE_DISKEVENTS:
        // Specifies a combination of all of the disk event identifiers.
        return L"SFVM_FSNOTIFY SHCNE_DISKEVENTS";
      case SHCNE_GLOBALEVENTS:
        // Specifies a combination of all of the global event identifiers.
        return L"SFVM_FSNOTIFY SHCNE_GLOBALEVENTS";
      case SHCNE_INTERRUPT:
        // The specified event occurred as a result of a system interrupt. As this value modifies other event values, it cannot be used alone.
        return L"SFVM_FSNOTIFY SHCNE_INTERRUPT";
      default:
        return L"SFVM_FSNOTIFY SHCNE_UNKNOWN";
      };
    }

  case SFVM_GETANIMATION:
    // Allows the callback object to specify that an animation be displayed while items are enumerated on a background thread.
    return L"SFVM_GETANIMATION";
  case SFVM_GETBUTTONINFO:
    // Allows the callback object to add buttons to the toolbar.
    return L"SFVM_GETBUTTONINFO";
  case SFVM_GETBUTTONS:
    // Allows the callback object to specify the buttons to be added to the toolbar.
    return L"SFVM_GETBUTTONS";
  case SFVM_GETDETAILSOF:
    // Allows the callback object to provide the details for an item in a Shell folder. Use only if a call to GetDetailsOf fails and there is no GetDetailsOf method available to call.
    return L"SFVM_GETDETAILSOF";
  case SFVM_GETHELPTEXT:
    // Allows the callback object to specify a help text string for menu items or toolbar buttons.
    return L"SFVM_GETHELPTEXT";
  case SFVM_GETHELPTOPIC:
    // Allows the callback object to specify a Help file and topic.
    return L"SFVM_GETHELPTOPIC";
  case SFVM_GETNOTIFY:
    // Specifies which events will generate an SFVM_FSNOTIFY message for a given item.
    return L"SFVM_GETNOTIFY";
  case SFVM_GETPANE:
    // Allows the callback object to provide the status bar pane in which to display the Internet zone information.
    return L"SFVM_GETPANE";
  case SFVM_GETSORTDEFAULTS:
    // Allows the callback object to specify default sorting parameters.
    return L"SFVM_GETSORTDEFAULTS";
  case SFVM_GETTOOLTIPTEXT:
    // Allows the callback object to specify a tooltip text string for menu items or toolbar buttons.
    return L"SFVM_GETTOOLTIPTEXT";
  case SFVM_GETZONE:
    // Allows the callback object to provide Internet zone information.
    return L"SFVM_GETZONE";
  case SFVM_INITMENUPOPUP:
    // Allows the callback object to modify an item's context menu.
    return L"SFVM_INITMENUPOPUP";
  case SFVM_INVOKECOMMAND:
    // Notifies the callback object that one of its toolbar or menu commands has been invoked.
    return L"SFVM_INVOKECOMMAND";
  case SFVM_MERGEMENU:
    // Allows the callback object to merge menu items into the Windows Explorer menus.
    return L"SFVM_MERGEMENU";
  case SFVM_QUERYFSNOTIFY:
    // Allows the callback object to register a folder so that changes to that folder's view will generate notifications.
    return L"SFVM_QUERYFSNOTIFY";
  case SFVM_SETISFV:
    // Notifies the callback object of the container site. This is used only when IObjectWithSite::SetSite is not supported and SHCreateShellFolderViewEx is used.
    return L"SFVM_SETISFV";
  case SFVM_SIZE:
    // Notifies the callback object that the folder view has been resized.
    return L"SFVM_SIZE";
  case SFVM_THISIDLIST:
    // Allows the callback object to specify the view's PIDL. This is used only when SetIDList and IPersistFolder2::GetCurFolder have failed.
    return L"SFVM_THISIDLIST";
  case SFVM_UNMERGEMENU:
    // Notifies the callback object that a menu is being removed.
    return L"SFVM_UNMERGEMENU";
  case SFVM_UPDATESTATUSBAR:
    // Allows the callback object to request that the status bar be updated.
    return L"SFVM_UPDATESTATUSBAR";
  case SFVM_WINDOWCREATED:
    // Notifies the callback object that the folder view window is being created.
    return L"SFVM_WINDOWCREATED";

  case SFVM_SELECTIONCHANGED:
    return L"SFVM_SELECTIONCHANGED undocumented";
  case SFVM_DRAWMENUITEM:
    return L"SFVM_DRAWMENUITEM undocumented";
  case SFVM_MEASUREMENUITEM:
    return L"SFVM_MEASUREMENUITEM undocumented";
  case SFVM_EXITMENULOOP:
    return L"SFVM_EXITMENULOOP undocumented";
  case SFVM_VIEWRELEASE:
    return L"SFVM_VIEWRELEASE undocumented";
  case SFVM_GETNAMELENGTH:
    return L"SFVM_GETNAMELENGTH undocumented";
  case SFVM_WINDOWCLOSING:
    return L"SFVM_WINDOWCLOSING undocumented";
  case SFVM_LISTREFRESHED:
    return L"SFVM_LISTREFRESHED undocumented";
  case SFVM_WINDOWFOCUSED:
    return L"SFVM_WINDOWFOCUSED undocumented";
  case SFVM_REGISTERCOPYHOOK:
    return L"SFVM_REGISTERCOPYHOOK undocumented";
  case SFVM_COPYHOOKCALLBACK:
    return L"SFVM_COPYHOOKCALLBACK undocumented";
  case SFVM_ADDINGOBJECT:
    return L"SFVM_ADDINGOBJECT undocumented";
  case SFVM_REMOVINGOBJECT:
    return L"SFVM_REMOVINGOBJECT undocumented";
  case SFVM_GETCOMMANDDIR:
    return L"SFVM_GETCOMMANDDIR undocumented"; // tenatively this could be used to trigger a download from a context menu, but it shows up during copy/paste too early
  case SFVM_GETCOLUMNSTREAM:
    return L"SFVM_GETCOLUMNSTREAM undocumented";
  case SFVM_CANSELECTALL:
    return L"SFVM_CANSELECTALL undocumented";
  case SFVM_ISSTRICTREFRESH:
    return L"SFVM_ISSTRICTREFRESH undocumented";
  case SFVM_ISCHILDOBJECT:
    return L"SFVM_ISCHILDOBJECT undocumented";
  case SFVM_GETEXTVIEWS:
    return L"SFVM_GETEXTVIEWS undocumented";
  case SFVM_GET_CUSTOMVIEWINFO:
    return L"SFVM_GET_CUSTOMVIEWINFO undocumented";
  case SFVM_ENUMERATEDITEMS:
    return L"SFVM_ENUMERATEDITEMS undocumented";
  case SFVM_GET_VIEW_DATA:
    return L"SFVM_GET_VIEW_DATA undocumented";
  case SFVM_GET_WEBVIEW_LAYOUT:
    return L"SFVM_GET_WEBVIEW_LAYOUT undocumented";
  case SFVM_GET_WEBVIEW_CONTENT:
    return L"SFVM_GET_WEBVIEW_CONTENT undocumented";
  case SFVM_GET_WEBVIEW_TASKS:
    return L"SFVM_GET_WEBVIEW_TASKS undocumented";
  case SFVM_GET_WEBVIEW_THEME:
    return L"SFVM_GET_WEBVIEW_THEME undocumented";
  case SFVM_GETDEFERREDVIEWSETTINGS:
    return L"SFVM_GETDEFERREDVIEWSETTINGS undocumented";
  default:
    return L"SFVM_UNKNOWN";
  }
}
