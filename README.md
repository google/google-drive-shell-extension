# About Google Drive Shell Extension

Google Drive Shell Extension is a Shell Namespace Extension for Microsoft
Windows. It creates a virtual folder on My Computer, mirrored in the user's home
directory, that allows the user to interact with his Google Drive account
transparently through Windows Explorer. This includes "File Open" and "Browse"
commands from within other Windows applications, etc. It connects to Google
Drive using the Google Drive Proxy service.

The main feature of the project is that syncronization is performed only as
needed. Instead of holding the entire contents of your Google Drive folder on
disk, it only keeps what it needs. When you open or copy files it will download
them. It will then upload any changes you make locally to keep the folders in
sync. This means that your content is not available while offline.

# Supported OS

- Windows 7 32-bit and 64-bit
- Windows 8
- Windows Server
- Citrix Server

# Contents

The bundled Visual Studio Solution file will work with Visual Studio 2010 and
later, and includes:
- the DriveFusion dll project
  This project builds the Shell Namespace Extension.
- the DriveFusion.Installer WiX project
  This project builds the Installer for the Shell Namespace Extension. This only
  includes Google Drive Shell Extension, it will not install the Drive Proxy
  service.

# Build instructions

Project dependencies:
- Visual Studio - http://www.visualstudio.com/
- NuGet package manager - https://www.nuget.org/
- WiX Toolset - http://wixtoolset.org/
- Nullsoft Scriptable Install System - http://nsis.sourceforge.net/

Once everything is installed, edit Release.bat and make sure the paths to
MSBuild.exe and makensis.exe are correct for your machine.

Execute Release.bat.

# Installation instructions

After building, go to the ./bin folder and execute DriveFusion.exe. This will
launch the installer, which will automatically install both Google Drive Shell
Extension and the Google Drive Proxy service to the appropriate Program Files
folder. The only required interaction is a UAC prompt to grant admin privilege.

The Google Drive Proxy service is always installed to the 32-bit Program Files
folder, under "Google/Drive Proxy". Google Drive Shell Extension will match your
Windows' architecture Program Files folder, under "Google/Drive Fusion".

# Usage

After installing the project, you'll find a new virtual folder in My Computer.
The first time you attempt to open it, it will launch a browser window and
request your Google Account username and password. It will then request that you
authorize the application to access Google Drive. Once it has authenticated your
credentials, you will be able to view the contents of your Google Drive account
and treat it as any other folder. You can create folders, create new files, drag
and drop into and out of it, etc.

# Contact

For questions and answers join/view the [google-drive-shell-extension]
(https://groups.google.com/d/forum/google-drive-shell-extension) Google Group.