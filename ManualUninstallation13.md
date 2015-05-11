# Uninstallation of 1.3.x #

---


So you are here, because regular uninstall did not work for you.

## Prerequisities ##

  * Registry editor (look for PHM RegEditor, it is for free)
  * Task manager that can stop a service (look for TaskManager from FdcSoft, it is for free)

## Uninstall ##

  1. Use the Task manager to stop pRSSreader service
  1. Deactivate Today plugin (if activated)
  1. End pRSSreader (**File | Exit**)
  1. Delete the folder with pRSSreader's binaries (default is **\Program Files\pRSSreader**)
  1. Delete librarires
    * **\windows\libexpat.dll**
    * **\windows\prssrsrv.dll**
    * **\windows\prssrtoday.dll**
    * **\windows\prssrenc.dll**
    * **\windows\zlib1.dll**
  1. If you are on WM5 or WM6, delete **\windows\startup\prssrsrv.lnk** and **\windows\stprssrsrv.exe**
  1. Use registry editor to delete the following branches:
    * `HKCU\Software\DaProfik\pRSSreader`
    * `HKLM\Services\pRSSreader`
    * `HKLM\Software\Microsoft\Today\Items\pRSSreader`
    * `HKCU\ControlPanel\Notifications\{5056DB7C-77D7-4DEE-A7B5-8B9578370A8F`}
  1. Then perform uninstall from **Start | Settings | System | Remove Programs** (to get rid of the entry in this dialog, it may report some errors, but you can ignore them)

pRSSreader should be uninstalled now.