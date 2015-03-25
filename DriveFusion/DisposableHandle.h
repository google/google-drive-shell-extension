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

class DisposableHandle
{
public:
  DisposableHandle(void);
  DisposableHandle(HANDLE handle);
  ~DisposableHandle(void);

  HANDLE Value() const;
  void Value(HANDLE handle);
  void Close();

protected:
  HANDLE _handle;

private:
  DisposableHandle(DisposableHandle const& copy);
  DisposableHandle & operator=(DisposableHandle const& copyAssignment);
};
