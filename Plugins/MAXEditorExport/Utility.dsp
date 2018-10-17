# Microsoft Developer Studio Project File - Name="Utility" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Utility - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Utility.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Utility.mak" CFG="Utility - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Utility - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Utility - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/XRay Mesh Export (MAX 3.0 plug)", DQAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Utility - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UTILITY_EXPORTS" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /Ob2 /D "UTILITY_EXPORTS" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_EXPORT" /D "_MAX_EXPORT" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 d3dx.lib mesh.lib geom.lib gfx.lib maxutil.lib core.lib paramblk2.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Winmm.lib R:\MagicFM\Sdk\Library\Release\MagicFM.lib /nologo /base:"0x08970000" /dll /machine:I386 /out:"r:\3dsmax4\Plugins\XRayExportUtility.dlu" /libpath:"..\..\..\..\lib" /release
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Utility - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UTILITY_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /Gi /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_EXPORT" /D "_MAX_EXPORT" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 d3dx.lib mesh.lib geom.lib gfx.lib maxutil.lib core.lib paramblk2.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Winmm.lib R:\MagicFM\Sdk\Library\Debug\MagicFM.lib /nologo /base:"0x08970000" /dll /debug /machine:I386 /out:"r:\3dsmax4\Plugins\XRayExportUtility.dlu" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Utility - Win32 Release"
# Name "Utility - Win32 Debug"
# Begin Group "Export files"

# PROP Default_Filter ""
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\MeshExpUtility.rc
# End Source File
# Begin Source File

SOURCE=.\MeshExpUtility.rh
# End Source File
# End Group
# Begin Group "Shared Files"

# PROP Default_Filter ""
# Begin Group "Engine"

# PROP Default_Filter ""
# Begin Group "Math"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\_bitwise.h
# End Source File
# Begin Source File

SOURCE=.\_color.h
# End Source File
# Begin Source File

SOURCE=.\_compressed_normal.cpp
# End Source File
# Begin Source File

SOURCE=.\_compressed_normal.h
# End Source File
# Begin Source File

SOURCE=.\_d3d_extensions.h
# End Source File
# Begin Source File

SOURCE=.\_fbox.h
# End Source File
# Begin Source File

SOURCE=.\_fbox2.h
# End Source File
# Begin Source File

SOURCE=.\_fpoint.h
# End Source File
# Begin Source File

SOURCE=.\_frect.h
# End Source File
# Begin Source File

SOURCE=.\_intersection_quad.h
# End Source File
# Begin Source File

SOURCE=.\_ipoint.h
# End Source File
# Begin Source File

SOURCE=.\_irect.h
# End Source File
# Begin Source File

SOURCE=.\_math.cpp
# End Source File
# Begin Source File

SOURCE=.\_math.h
# End Source File
# Begin Source File

SOURCE=.\_matrix.h
# End Source File
# Begin Source File

SOURCE=.\_matrix33.h
# End Source File
# Begin Source File

SOURCE=.\_obb.h
# End Source File
# Begin Source File

SOURCE=.\_plane.h
# End Source File
# Begin Source File

SOURCE=.\_quaternion.cpp
# End Source File
# Begin Source File

SOURCE=.\_quaternion.h
# End Source File
# Begin Source File

SOURCE=.\_random.h
# End Source File
# Begin Source File

SOURCE=.\_sphere.cpp
# End Source File
# Begin Source File

SOURCE=.\_sphere.h
# End Source File
# Begin Source File

SOURCE=.\_std_extensions.h
# End Source File
# Begin Source File

SOURCE=.\_stl_extensions.h
# End Source File
# Begin Source File

SOURCE=.\_types.h
# End Source File
# Begin Source File

SOURCE=.\_vector2.h
# End Source File
# Begin Source File

SOURCE=.\_vector3d.h
# End Source File
# Begin Source File

SOURCE=.\_vector4.h
# End Source File
# Begin Source File

SOURCE=.\clsid.cpp
# End Source File
# Begin Source File

SOURCE=.\clsid.h
# End Source File
# Begin Source File

SOURCE=.\cpuid.cpp
# End Source File
# Begin Source File

SOURCE=.\cpuid.h
# End Source File
# Begin Source File

SOURCE=.\FixedVector.h
# End Source File
# Begin Source File

SOURCE=.\vector.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Fmesh.h
# End Source File
# Begin Source File

SOURCE=.\FS.cpp
# End Source File
# Begin Source File

SOURCE=.\FS.h
# End Source File
# Begin Source File

SOURCE=.\FTimer.h
# End Source File
# Begin Source File

SOURCE=.\LzHuf.cpp
# End Source File
# Begin Source File

SOURCE=.\lzhuf.h
# End Source File
# Begin Source File

SOURCE=.\progmesh.h
# End Source File
# Begin Source File

SOURCE=.\Properties.h
# End Source File
# Begin Source File

SOURCE=.\Xr_ini.cpp
# End Source File
# Begin Source File

SOURCE=.\xr_ini.h
# End Source File
# Begin Source File

SOURCE=.\xr_tokens.h
# End Source File
# End Group
# Begin Group "Editor"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BlenderListLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\BlenderListLoader.h
# End Source File
# Begin Source File

SOURCE=.\Engine.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine.h
# End Source File
# Begin Source File

SOURCE=.\FileSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\FileSystem.h
# End Source File
# Begin Source File

SOURCE=.\FS2.CPP
# End Source File
# Begin Source File

SOURCE=.\FS2.H
# End Source File
# Begin Source File

SOURCE=.\Log.cpp
# End Source File
# Begin Source File

SOURCE=.\Log.h
# End Source File
# Begin Source File

SOURCE=.\lwchunks.h
# End Source File
# Begin Source File

SOURCE=.\NetDeviceLog.cpp
# End Source File
# Begin Source File

SOURCE=.\NetDeviceLog.h
# End Source File
# Begin Source File

SOURCE=.\Shader_xrLC.h
# End Source File
# Begin Source File

SOURCE=.\xr_trims.cpp
# End Source File
# Begin Source File

SOURCE=.\xr_trims.h
# End Source File
# End Group
# End Group
# Begin Group "Export Object"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\bone.cpp
# End Source File
# Begin Source File

SOURCE=.\bone.h
# End Source File
# Begin Source File

SOURCE=.\EditMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\EditMesh.h
# End Source File
# Begin Source File

SOURCE=.\EditMeshExportMAX.cpp
# End Source File
# Begin Source File

SOURCE=.\EditMeshIO.cpp
# End Source File
# Begin Source File

SOURCE=.\EditMeshModify.cpp
# End Source File
# Begin Source File

SOURCE=.\EditObject.cpp
# End Source File
# Begin Source File

SOURCE=.\EditObject.h
# End Source File
# Begin Source File

SOURCE=.\EditObjectExportLWO.cpp
# End Source File
# Begin Source File

SOURCE=.\EditObjectExportMAX.cpp
# End Source File
# Begin Source File

SOURCE=.\EditObjectIO.cpp
# End Source File
# Begin Source File

SOURCE=.\EditObjectMotions.cpp
# End Source File
# Begin Source File

SOURCE=.\Envelope.cpp
# End Source File
# Begin Source File

SOURCE=.\envelope.h
# End Source File
# Begin Source File

SOURCE=.\interp.cpp
# End Source File
# Begin Source File

SOURCE=.\motion.cpp
# End Source File
# Begin Source File

SOURCE=.\motion.h
# End Source File
# Begin Source File

SOURCE=.\SceneClassList.h
# End Source File
# End Group
# Begin Group "Export Skin"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Exporter.cpp
# End Source File
# Begin Source File

SOURCE=.\Exporter.h
# End Source File
# Begin Source File

SOURCE=.\Face.h
# End Source File
# Begin Source File

SOURCE=.\Helper.cpp
# End Source File
# Begin Source File

SOURCE=.\Helper.h
# End Source File
# Begin Source File

SOURCE=.\Vert.h
# End Source File
# Begin Source File

SOURCE=.\xrSKIN_BUILD.cpp
# End Source File
# Begin Source File

SOURCE=.\xrSKIN_BUILD.h
# End Source File
# End Group
# Begin Group "Export Skin Keys"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ExportKeys.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\MeshExpPlugin.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshExpUtility.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshExpUtility.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Stdafx.h
# End Source File
# Begin Source File

SOURCE=.\Utility.def
# End Source File
# End Group
# End Target
# End Project
