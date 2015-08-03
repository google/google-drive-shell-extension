google-drive-proxy\.nuget\NuGet.exe restore google-drive-proxy\DriveProxy.sln
"C:\Program Files (x86)\MSBuild\12.0\Bin\MSBuild.exe" google-drive-proxy\DriveProxy.sln /p:Configuration=Debug
"C:\Program Files (x86)\MSBuild\12.0\Bin\MSBuild.exe" DriveFusion.sln /p:Configuration=Debug /p:Platform=x86
"C:\Program Files (x86)\MSBuild\12.0\Bin\MSBuild.exe" DriveFusion.sln /p:Configuration=Debug /p:Platform=x64
"C:\Program Files (x86)\NSIS\makensis.exe" /V2 /DOUTPUT_PATH=bin\DriveFusion.exe /D32BIT_MSI_FILE_PATH=.\bin\Debug\x86\Installer\DriveFusion.msi /D64BIT_MSI_FILE_PATH=.\bin\Debug\x64\Installer\DriveFusion.msi /DDRIVE_PROXY_MSI_PATH=google-drive-proxy\obj\Debug\DriveProxy.Installer\DriveProxy.msi /DVersion=1.0.0.0 /DPRODUCT_NAME="Google Drive Shell Extension" wrapper.nsi
