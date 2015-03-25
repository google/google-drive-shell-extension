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
// FusionGDShell.cpp : Implementation of DLL Exports.


#include "stdafx.h"
#include "resource.h"
#include "FusionGDShell_i.h"
#include "dllmain.h"


// Used to determine whether the DLL can be unloaded by OLE.
STDAPI DllCanUnloadNow(void)
{
  return _AtlModule.DllCanUnloadNow();
}

// Returns a class factory to create an object of the requested type.
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
  return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}

// DllRegisterServer - Adds entries to the system registry.
STDAPI DllRegisterServer(void)
{
  // registers object, typelib and all interfaces in typelib
  return _AtlModule.DllRegisterServer();
}

// DllUnregisterServer - Removes entries from the system registry.
STDAPI DllUnregisterServer(void)
{
  return _AtlModule.DllUnregisterServer();
}

// DllInstall - Adds/Removes entries to the system registry per user per machine.
STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine)
{
  HRESULT hr = E_FAIL;
  static const wchar_t szUserSwitch[] = L"user";

  if (pszCmdLine != NULL)
  {
    if (_wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0)
    {
      ATL::AtlSetPerUserRegistration(true);
    }
  }

  if (bInstall)
  {
    hr = DllRegisterServer();

    if (FAILED(hr))
    {
      DllUnregisterServer();
    }
  }
  else
  {
    hr = DllUnregisterServer();
  }

  return hr;
}


