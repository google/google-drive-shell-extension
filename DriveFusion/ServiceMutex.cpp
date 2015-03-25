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
#include "Scoped_Mutex.h"

Scoped_Mutex::Scoped_Mutex(void)
{
  _handle = CreateMutex(NULL, FALSE, NULL);
}

Scoped_Mutex::~Scoped_Mutex(void)
{
  if (_handle != NULL)
  {
    WaitForSingleObject(_handle, INFINITE);
    CloseHandle(_handle);
    _handle = NULL;
  }
}

bool Scoped_Mutex::_Lock(int timeout)
{
  if (_handle == NULL)
  {
    Log::HRFromWin32(false, L"Scoped_Mutex::_Lock");

    return false;
  }

  DWORD state = WaitForSingleObject(_handle, timeout);

  if (state != WAIT_OBJECT_0)
  {
    return false;
  }

  return true;
}

bool Scoped_Mutex::_Unlock()
{
  HRESULT hr = Log::HRFromWin32( (ReleaseMutex(_handle) > 0) , L"Scoped_Mutex::_Unlock");

  return SUCCEEDED(hr);
}
