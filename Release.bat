google-drive-proxy\.nuget\NuGet.exe restore google-drive-proxy\DriveProxy.sln
C:\Windows\Microsoft.NET\Framework\v4.0.30319\MSBuild.exe google-drive-proxy\DriveProxy.sln /p:Configuration=Release
C:\Windows\Microsoft.NET\Framework\v4.0.30319\MSBuild.exe DriveFusion.sln /p:Configuration=Release /p:Platform=x86
C:\Windows\Microsoft.NET\Framework\v4.0.30319\MSBuild.exe DriveFusion.sln /p:Configuration=Release /p:Platform=x64
"C:\Program Files (x86)\NSIS\makensis.exe" /V2 /DOUTPUT_PATH=bin\DriveFusion.exe /D32BIT_MSI_FILE_PATH=.\bin\Release\x86\Installer\DriveFusion.msi /D64BIT_MSI_FILE_PATH=.\bin\Release\x64\Installer\DriveFusion.msi /DDRIVE_PROXY_MSI_PATH=google-drive-proxy\obj\Release\DriveProxy.Installer\DriveProxy.msi /DVersion=1.0.0.0 /DPRODUCT_NAME="Drive Fusion" wrapper.nsi
