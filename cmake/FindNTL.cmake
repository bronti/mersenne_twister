# Locate NTL
# This module defines
# NTL_LIBRARY
# NTL_FOUND, if false, do not try to link to OpenAL 
# NTL_INCLUDE_DIR, where to find the headers
#
# Created by Tai Chi Minh Ralph Eastwood <tcmreastwood@gmail.com>
# Modified by Anya Yaveyn <anya.yaveyn@yandex.ru>

FIND_PATH(NTL_INCLUDE_DIR
  NAMES NTL/ZZ.h
  HINTS $ENV{NTLDIR}
  PATH_SUFFIXES include
  PATHS
    ~/Library/Frameworks
    ~/opt
    /Library/Frameworks
    /usr/local
    /usr
    /sw # Fink
    /opt/local # DarwinPorts
    /opt/csw # Blastwave
    /opt
)

FIND_LIBRARY(NTL_LIBRARY
  NAMES ntl
  HINTS $ENV{NTLDIR}
  PATH_SUFFIXES lib64 lib libs64 libs libs/Win32 libs/Win64
  PATHS
    ~/Library/Frameworks
    ~/opt
    /Library/Frameworks
    /usr/local
    /usr
    /sw
    /opt/local
    /opt/csw
    /opt
)


# handle the QUIETLY and REQUIRED arguments and set NTL_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(NTL DEFAULT_MSG  NTL_LIBRARY NTL_INCLUDE_DIR)

MARK_AS_ADVANCED(NTL_LIBRARY NTL_INCLUDE_DIR)
