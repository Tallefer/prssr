# Prerequisites #

  * eVC 4.0 - [download](http://www.microsoft.com/downloads/details.aspx?FamilyId=1DACDB3D-50D1-41B2-A107-FA75AE960856), you will need a CD Key that is in the section Instructions on its download page.
  * SP4 for eVC 4.0 - [download](http://www.microsoft.com/downloads/details.aspx?FamilyID=4a4ed1f4-91d3-4dbe-986e-a812984318e5&displaylang=en)
  * SDK for Windows Mobile 2003-based Pocket PC - [download](http://www.microsoft.com/downloads/info.aspx?na=47&p=1&SrcDisplayLang=en&SrcCategoryId=&SrcFamilyId=1dacdb3d-50d1-41b2-a107-fa75ae960856&u=details.aspx%3ffamilyid%3d9996B314-0364-4623-9EDE-0B5FBB133652%26displaylang%3den)
  * SVN client, I recommend TortoiseSVN - [Download](http://tortoisesvn.net/downloads)
  * UnxUtils - [Download](http://sourceforge.net/project/showfiles.php?group_id=9328)

# Installation #

  1. The installation of eVC 4.0, SP4 for eVC 4.0 are standard Next, Next, Finish installation.
    1. It is sufficient to install the support for ARMV4 CPUs.
    1. You will need MFC related stuff.
    1. You do not have to install Standard SDK which comes with eVC 4.0
  1. The installation of SDK of PPC2003 is again Next, next, finish.
  1. TortoiseSVN is also next, next finish
  1. UnxUtils do not come with the installer, so you need to unpack it somewhere (I install it in the Program Files directory)
    1. Then you need to set up your environment so that you can use UnxUtils from the command line.
    1. On Windows XP:
      1. Right click My computer | Properties | Advanced tab | Environment variables
      1. Edit system-wide variable called Path
      1. Add the path to the UnxUtils, in my case I add **`C:\Program Files\!UnxUtils\usr\local\wbin`** (Do not forget to separate this new path by a semicolon (;).


# Getting the Source Code #

  * Create a working directory, for example **`C:\pda\prssr`**
  * Right click the created directory and select **SVN Checkout**
  * URL of the repository is: `http://prssr.googlecode.com/svn/branches/X.Y.Z/` (replace X.Y.Z with a latest version - for example 1.4.4)

# Before Making the Localization #

Before you start doing the localization, you have to understand what you are going to do, so please read this carefully:

  * prssr workspace contains several project and you will be working with three of them: the main application - **prssr**, the today plug-in - **prssrtoday**, and installation DLL - **setup**.
  * Each project contains so called resources that you will be modifying. Based on the preprocessor directives, the language specific resources are included and used during the compilation time.
  * Therefore, each language is going to have a specific configuration that will be used for building a localized version.

Before we start, we need to find out a few things:
  1. go to the http://msdn.microsoft.com/en-us/library/aa912040.aspx and find your language
  1. Take the value from **Language Code** (3-letter abbreviation)
  1. Note the value of Default code page

For example, Czech has **CSY** as a Language ID and **1250** as the Codepage.

# Making the Localization #

  1. Go into the `C:\pda\prssr` and open **prssr.vcw**

## Create Configurations ##

  1. Select **Build | Configurations**
  1. Click on **prssr**, then click **Add..**
    1. CPU has to be: `Win32 (WCE ARMV4) Release`
    1. Copy settings from **`prssr - Win32 (WCE ARMV4) Release`**
    1. Configuration: **ReleaseXXX** (replace XXX with the your language, in my case it is Czech - do not use spaces, otherwise you will have to do more edits in the project settings)
    1. click **OK**

  1. Repeat for **prssrtoday** and **setup** projects.
  1. Close the Configuration dialog

## Modify Project Settings ##

  1. Open **Project | Settings...**
  1. Select prssr (in the left part) and switch the **settings for:** `Win32 (WCE ARMV4) ReleaseXXX` (where `XXX` is your language, in my case Czech)
  1. Activate **Resources** tab
  1. In **Language**, select your language (in my case Czech)
  1. In **Preprocessor definitions** find `AFX_TARG_ENU` and replace it with `AFX_TARG_YYY`, where `YYY` is the three letter abbreviation from above (in my case CSY)
  1. Switch to the **Link** tab, Category **Input**
  1. Copy the content of **Additional library path** from Release configuration to your new configuration.
  1. Repeat for **prssrtoday** and **setup** projects.


## Edit .rc Files ##

  * Now we have to modify .rc files so that the language specified resource will be included during the compilation.
  * We have to do this outside the eVC and by hand

  1. Go to the http://msdn.microsoft.com/en-us/library/bb202928.aspx and find `LANG_LLL` and `SUBLANG_SSS` values for your language. In my case it is `LANG_CZECH`, and because there is no `SUBLANG_CZECH_MMM`, then my `SUBLANG_SSS` is `SUBLANG_DEFAULT`.
  1. Edit `<root>\prssr\prssr.rc` and go all the way down where you find
```
/////////////////////////////////////////////////////////////////////////////
// English (U.S.) resources
#if defined(AFX_TARG_ENU)
#ifdef _WIN32
LANGUAGE LANG_ENGLISH, SUBLANG_ENGLISH_US
#pragma code_page(1252)
#endif //_WIN32
#include "res\english.rc2"
```
  1. add the following:
```
/////////////////////////////////////////////////////////////////////////////
// Czech resources
#elif defined(AFX_TARG_CSY)
#ifdef _WIN32
LANGUAGE LANG_CZECH, SUBLANG_DEFAULT
#pragma code_page(1250)
#endif //_WIN32
#include "res\czech.rc2"
```
  1. Replace the following values with the values that you get above:
    * Czech
    * `AFX_TARG_CSY`
    * `LANG_CZECH`
    * `LANG_DEFAULT`
    * `codepage(1250)`
    * `res\czech.rc2` (chose suitable name for you language - do not remove the res\ part)

  1. Repeat this for files:
    * `<root>\prssrtoday\prssrtoday.rc`
    * `<root>\setup\setup.rc`

Note: it may happen that the section for your language in these files already exists, then use the existing one (it is a remnant from the previous versions)

## Create rc2 Files ##

  1. Go to the `<root>\prssr\res\`
  1. copy `english.rc2` to `your language.rc2` (in my case `czech.rc2`)
  1. Translate the file (see for details)
  1. Repeat for `<root>\prssrtoday\res\` and `<root>\setup\res\`

## Add the rc2 Files into Projects ##

  1. Switch back to eVC, and activate FileView
  1. Activate **prssr** project
  1. Right-click **Resource Files** item and select **Add Files to Folder**
  1. Go to `<root>\prssr\res\language.rc2` (where language is your language, in my case `czech.rc2`, has to match the file name you have chosen in the .rc file)
  1. Repeat for **prssrtoday** and **setup** projects

## Translate the rc2 Files ##

  * If you are not familiar with resource files, read below what to do
  * Open the `<root>\prssr\res\language.rc2` file (in my case `czech.rc2` file)
  * There are several sections you need to change (see below)
  * Repeat the translation for all your `language.rc2` files

### Dialogs ###

```
IDD_SITE_MANAGER DIALOG DISCARDABLE  0, 0, 160, 190
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Manage Subscriptions"
FONT 8, "Tahoma"
BEGIN
    CONTROL         "Tree1",IDC_SITES,"SysTreeView32",TVS_HASBUTTONS |
                    TVS_HASLINES | TVS_DISABLEDRAGDROP | TVS_SHOWSELALWAYS |
                    WS_TABSTOP,0,15,160,130
    LTEXT           "----",IDC_HRULE1,0,145,160,1
    PUSHBUTTON      "Up",IDC_MOVE_UP,45,150,36,12
    PUSHBUTTON      "Down",IDC_MOVE_DOWN,96,150,36,12
END
```

  * Translate the `CAPTION`
  * Do not touch anything the is not in the double quotes (`"`)
  * You may need to change positions and dimensions of the controls if the text does not fit in them, then change the four numbers, their meaning is: x, y, width, height
  * A lot of dialogs have the portrait and landscape version, you need to translate both

### Menus ###

  * Menu is easy to translate, just change what is in the double quotes (`"`)
  * You should not use very long captions, since the menus are then too wide


### String Tables ###

  * Strings are the same as menus
  * The only difference is that some times it may happen that the string is truncated, so you need to verify your files by building the binaries, see below
  * Please, do not touch the `IDR_MAINFRAME` string


# Building the Localized Version #

  * First you need to build the libraries: prssrenc, res096, res192
  * Then build: prssrnot

# Modifying Installation Files #

  * Go to the `<root>\install`
  * Edit the `pRSSreader.inf`
    * Find section `SourceDisksNames.ARM` and add:
```
[SourceDisksNames.XXX]
	 1 = ,"prssr",,..\prssr\prj\armv4releaseRRR
	 4 = ,"prssrtoday",,..\prssrtoday\prj\armv4releaseRRR
	 5 = ,"setup",,..\setup\prj\armv4releaseRRR
```
    * Replace the `XXX` with 3-letter abbreviation from above (in my case `CSY`)
    * Replace the `RRR` with the language (should match to what you chosen when creating the configurations)
  * Edit the `Makefile`
    * Find section for localizations (at the end) and add:
```
csy: $(PRODUCT)-$(VERSION)-cs.ARM.CAB

$(PRODUCT)-$(VERSION)-cs.ARM.CAB: $(INF)
	$(CABWIZ) $(INF) /cpu CSY || sleep 1
	mv -f $(PRODUCT).CSY.CAB $(PRODUCT)-$(VERSION)-cs.cab
```
    * replace **csy** and **CSY** with the 3-letter abbreviation from above
    * replace **cs** in the file name with a [iso code](http://www.iso.org/iso/english_country_names_and_code_elements). If you cannot find such a code, let me know in [prssr-devel](http://groups.google.com/group/prssr-devel)

# Building the Binaries #

  * Go to the  `<root>\install`
  * start the command line
  * type: `make xxx` (replace `xxx` with your 3-letter abbreviation, in my case `csy`)
  * congratulations, if you did everything correctly, you should get a CAB file which you should be able to install on your device

# Final Steps #

  * Please try the installation of the CAB file and check that the localization is ok (no truncated strings, everything makes sense, etc.)
  * If you thing the localization is ready:
    1. right-click the prssr directory (where you did the checkout)
    1. Select **TortoiseSVN | Create Patch**
    1. Check **Show unversioned files** option
    1. Include the rc2 files
    1. Click **OK**
    1. Select an export path, chose a file name, click **Save**
    1. Submit the patch file to [prssr-devel](http://groups.google.com/group/prssr-devel) group

# More Info #

If you get stuck, I can help you in [prssr-devel](http://groups.google.com/group/prssr-devel) group.