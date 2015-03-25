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

#define DriveItemSignature_MAGIC 0x36db4e41
struct DriveItemSignature
{
  WCHAR Id[200]; // can't use a dynamically allocated string, lets hope this is long enough to hold the google drive ID
};

typedef __unaligned DriveItemSignature *PUDriveItemSignature;
typedef const __unaligned DriveItemSignature *PCUDriveItemSignature;
