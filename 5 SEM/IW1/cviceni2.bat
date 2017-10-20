dism /Get-ImageInfo /ImageFile:"D:\Windows 10 1703 x64 EN\sources\install.wim"
dism /Mount-Image /ImageFile:"D:\Windows 10 1703 x64 EN\sources\install.wim" /index:1 /MountDir:"D:\Deployment\mount"

dism /Image:"D:\Deployment\mount" /Add-Driver /Driver:"D:\Deployment\drivers" /Recurse
dism /image:"D:\Deployment\mount" /Add-Package /PackagePath:"D:\Deployment\updates"
dism /Commit-image /MountDir:"D:\Deployment\mount"
dism /Unmount-Image /Commit /MountDir:"D:\Deployment\mount"

rmdir "D:\Deployment\drivers" /Q /S
rmdir "D:\Deployment\updates" /Q /S
