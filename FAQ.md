# FAQ #

---


**I'm not able to uninstall pRSSreader 1.3.x. What can I do about it?**

See ManualUninstallation13.

**My ISP requires special User-Agent string for the Internet access. How can I change it?**

  * Tap **Tools | Options**, select **Retrieval tab**, change **User-Agent** string to **Pocket IE**.
  * If the above did not work for you, try to:
    1. run registry editor,
    1. open `HKCU\Software\DaProfik\pRSSreader` key,
    1. change **User Agent** value to whatever you want.

**I need to send special HTTP headers with every HTTP request. How can I do it?**

  1. run registry editor,
  1. open `HKCU\Software\DaProfik\pRSSreader` key,
  1. create **Additional HTTP Headers** value typed as **Multiline String Value**,
  1. Insert your special HTTP headers here.


**I want to run pRSSreader from my script. Are there any command line parameters?**

Yes, see [Comand line parameters](CommandLineParameters.md).


**I'm accessing the Internet via HTTP proxy and downloading of feeds via HTTPS is not working. Where is the problem?**

Accessing HTTPS sites via HTTP proxy is not possible at all due to man-in-the-middle attacks. It is not the problem of pRSSreader. It is given by HTTP specification.

Note: Accessing HTTPS via SOCKS proxy server is possible.


**Where is Skweezer option for HTML optimization?**

See FAQ: How to use Skweezer with pRSSreader.


**How to use Skweezer with pRSSreader?**

  1. Enter **Tools | Options**.
  1. Activate the **Retrieval** tab.
  1. Check **Use HTML Optimizer**.
  1. Enter `http://www.skweezer.net/skweeze.aspx?q=[%URL%]&i=1` (if you do not want images to be cached, omit '&i=1').


**How to use Google HTML optimizer?**

  1. Enter **Tools | Options**.
  1. Activate the **Retrieval** tab.
  1. Check **Use HTML Optimizer**.
  1. Enter `http://www.google.com/gwt/n?u=[%URL%]` (if you do not want images append `&_gwt_noimg=1`


**Cached pages are displayed in incorrect encoding. What should I do?**

Every page that is cached by pRSSreader is converted to UTF-8 encoding. The reason is to prevent annoying switching of code pages.

On Wm2003 in PIE, go **Tools | Options**, **General** page, set **Default character set**
to **Unicode (UTF-8)**, tap Ok and do not forget to **reload** the page.

TODO: The same for WM6


TODO: localization info