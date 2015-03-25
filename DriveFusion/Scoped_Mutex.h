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

// VS2010 does not have std::mutex
// This is a scope mutex, it will lock a single block of code, typically an entire function call
// This is NOT a recursive mutex.  It can not be called recursively, ex by a function that is called with in an already locked section.

class Scoped_Mutex
{
  friend class Scoped_Lock;

public:
  Scoped_Mutex(void);
  ~Scoped_Mutex(void);

private:
  HANDLE _handle;

  bool _Lock(int timeout = 30000);
  bool _Unlock();
};


class Scoped_Lock
{
public:
  Scoped_Lock(Scoped_Mutex* mutex, int timeout = 30000)
  {
    _mutex = mutex;
    _mutex->_Lock(timeout);
  }
  ~Scoped_Lock()
  {
    _mutex->_Unlock();
    _mutex = NULL;
  }

private:
  Scoped_Mutex* _mutex;
};
