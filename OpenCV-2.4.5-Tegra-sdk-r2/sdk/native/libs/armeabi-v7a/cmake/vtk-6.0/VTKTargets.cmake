# Generated by CMake 2.8.12.2

if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 2.5)
   message(FATAL_ERROR "CMake >= 2.6.0 required")
endif()
cmake_policy(PUSH)
cmake_policy(VERSION 2.6)
#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Protect against multiple inclusion, which would fail when already imported targets are added once more.
set(_targetsDefined)
set(_targetsNotDefined)
set(_expectedTargets)
foreach(_expectedTarget vtksys vtkCommonCore vtkCommonMath vtkCommonMisc vtkCommonSystem vtkCommonTransforms vtkCommonDataModel vtkCommonComputationalGeometry vtkCommonExecutionModel vtkDICOMParser vtkFiltersCore vtkFiltersGeneral vtkImagingCore vtkImagingFourier vtkalglib vtkFiltersStatistics vtkFiltersExtraction vtkFiltersGeometry vtkFiltersSources vtkFiltersModeling vtkzlib vtkIOCore vtkjsoncpp vtkIOGeometry vtkmetaio vtkjpeg vtkpng vtktiff vtkIOImage vtkIOLegacy vtkInfovisCore vtklibxml2 vtkIOInfovis vtkIOPLY vtkexpat vtkIOXMLParser vtkIOXML vtkParallelCore vtkRenderingCore vtkfreetype vtkRenderingFreeType)
  list(APPEND _expectedTargets ${_expectedTarget})
  if(NOT TARGET ${_expectedTarget})
    list(APPEND _targetsNotDefined ${_expectedTarget})
  endif()
  if(TARGET ${_expectedTarget})
    list(APPEND _targetsDefined ${_expectedTarget})
  endif()
endforeach()
if("${_targetsDefined}" STREQUAL "${_expectedTargets}")
  set(CMAKE_IMPORT_FILE_VERSION)
  cmake_policy(POP)
  return()
endif()
if(NOT "${_targetsDefined}" STREQUAL "")
  message(FATAL_ERROR "Some (but not all) targets in this export set were already defined.\nTargets Defined: ${_targetsDefined}\nTargets not yet defined: ${_targetsNotDefined}\n")
endif()
unset(_targetsDefined)
unset(_targetsNotDefined)
unset(_expectedTargets)


# Compute the installation prefix relative to this file.
get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)

# Create imported target vtksys
add_library(vtksys STATIC IMPORTED)

# Create imported target vtkCommonCore
add_library(vtkCommonCore STATIC IMPORTED)

# Create imported target vtkCommonMath
add_library(vtkCommonMath STATIC IMPORTED)

# Create imported target vtkCommonMisc
add_library(vtkCommonMisc STATIC IMPORTED)

# Create imported target vtkCommonSystem
add_library(vtkCommonSystem STATIC IMPORTED)

# Create imported target vtkCommonTransforms
add_library(vtkCommonTransforms STATIC IMPORTED)

# Create imported target vtkCommonDataModel
add_library(vtkCommonDataModel STATIC IMPORTED)

# Create imported target vtkCommonComputationalGeometry
add_library(vtkCommonComputationalGeometry STATIC IMPORTED)

# Create imported target vtkCommonExecutionModel
add_library(vtkCommonExecutionModel STATIC IMPORTED)

# Create imported target vtkDICOMParser
add_library(vtkDICOMParser STATIC IMPORTED)

# Create imported target vtkFiltersCore
add_library(vtkFiltersCore STATIC IMPORTED)

# Create imported target vtkFiltersGeneral
add_library(vtkFiltersGeneral STATIC IMPORTED)

# Create imported target vtkImagingCore
add_library(vtkImagingCore STATIC IMPORTED)

# Create imported target vtkImagingFourier
add_library(vtkImagingFourier STATIC IMPORTED)

# Create imported target vtkalglib
add_library(vtkalglib STATIC IMPORTED)

# Create imported target vtkFiltersStatistics
add_library(vtkFiltersStatistics STATIC IMPORTED)

# Create imported target vtkFiltersExtraction
add_library(vtkFiltersExtraction STATIC IMPORTED)

# Create imported target vtkFiltersGeometry
add_library(vtkFiltersGeometry STATIC IMPORTED)

# Create imported target vtkFiltersSources
add_library(vtkFiltersSources STATIC IMPORTED)

# Create imported target vtkFiltersModeling
add_library(vtkFiltersModeling STATIC IMPORTED)

# Create imported target vtkzlib
add_library(vtkzlib STATIC IMPORTED)

# Create imported target vtkIOCore
add_library(vtkIOCore STATIC IMPORTED)

# Create imported target vtkjsoncpp
add_library(vtkjsoncpp STATIC IMPORTED)

# Create imported target vtkIOGeometry
add_library(vtkIOGeometry STATIC IMPORTED)

# Create imported target vtkmetaio
add_library(vtkmetaio STATIC IMPORTED)

# Create imported target vtkjpeg
add_library(vtkjpeg STATIC IMPORTED)

# Create imported target vtkpng
add_library(vtkpng STATIC IMPORTED)

# Create imported target vtktiff
add_library(vtktiff STATIC IMPORTED)

# Create imported target vtkIOImage
add_library(vtkIOImage STATIC IMPORTED)

# Create imported target vtkIOLegacy
add_library(vtkIOLegacy STATIC IMPORTED)

# Create imported target vtkInfovisCore
add_library(vtkInfovisCore STATIC IMPORTED)

# Create imported target vtklibxml2
add_library(vtklibxml2 STATIC IMPORTED)

# Create imported target vtkIOInfovis
add_library(vtkIOInfovis STATIC IMPORTED)

# Create imported target vtkIOPLY
add_library(vtkIOPLY STATIC IMPORTED)

# Create imported target vtkexpat
add_library(vtkexpat STATIC IMPORTED)

# Create imported target vtkIOXMLParser
add_library(vtkIOXMLParser STATIC IMPORTED)

# Create imported target vtkIOXML
add_library(vtkIOXML STATIC IMPORTED)

# Create imported target vtkParallelCore
add_library(vtkParallelCore STATIC IMPORTED)

# Create imported target vtkRenderingCore
add_library(vtkRenderingCore STATIC IMPORTED)

# Create imported target vtkfreetype
add_library(vtkfreetype STATIC IMPORTED)

# Create imported target vtkRenderingFreeType
add_library(vtkRenderingFreeType STATIC IMPORTED)

# Load information for each installed configuration.
get_filename_component(_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
file(GLOB CONFIG_FILES "${_DIR}/VTKTargets-*.cmake")
foreach(f ${CONFIG_FILES})
  include(${f})
endforeach()

# Cleanup temporary variables.
set(_IMPORT_PREFIX)

# Loop over all imported files and verify that they actually exist
foreach(target ${_IMPORT_CHECK_TARGETS} )
  foreach(file ${_IMPORT_CHECK_FILES_FOR_${target}} )
    if(NOT EXISTS "${file}" )
      message(FATAL_ERROR "The imported target \"${target}\" references the file
   \"${file}\"
but this file does not exist.  Possible reasons include:
* The file was deleted, renamed, or moved to another location.
* An install or uninstall procedure did not complete successfully.
* The installation package was faulty and contained
   \"${CMAKE_CURRENT_LIST_FILE}\"
but not all the files it references.
")
    endif()
  endforeach()
  unset(_IMPORT_CHECK_FILES_FOR_${target})
endforeach()
unset(_IMPORT_CHECK_TARGETS)

# This file does not depend on other imported targets which have
# been exported from the same project but in a separate export set.

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
cmake_policy(POP)
