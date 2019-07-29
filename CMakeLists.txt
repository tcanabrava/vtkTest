cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(RenderWindowNoUiFile)

set(CMAKE_AUTOMOC true)

find_package(Qt5 COMPONENTS Core Gui Widgets)

find_package(VTK COMPONENTS 
  vtkCommonColor
  vtkCommonCore
  vtkFiltersSources
  vtkGUISupportQt
  vtkInteractionStyle
  vtkRenderingContextOpenGL2
  vtkRenderingCore
  vtkRenderingFreeType
  vtkRenderingGL2PSOpenGL2
  vtkRenderingOpenGL2 QUIET)
if (NOT VTK_FOUND)
  message("Skipping RenderWindowNoUiFile: ${VTK_NOT_FOUND_MESSAGE}")
  return ()
endif()
message (STATUS "VTK_VERSION: ${VTK_VERSION}")

set(SOURCES main.cpp
    main.cpp
    pingcallbacks.cpp
    pingviewer.cpp
)

if (VTK_VERSION VERSION_LESS "8.90.0")
  # old system
  include(${VTK_USE_FILE})
  add_executable(RenderWindowNoUiFile MACOSX_BUNDLE ${SOURCES} )
  target_link_libraries(RenderWindowNoUiFile PRIVATE ${VTK_LIBRARIES} Qt5::Core Qt5::Gui Qt5::Widgets)
else ()
  # include all components
  add_executable(RenderWindowNoUiFile MACOSX_BUNDLE ${SOURCES} )
  target_link_libraries(RenderWindowNoUiFile PRIVATE ${VTK_LIBRARIES}i Qt5::Core Qt5::Gui Qt5::Widgets)

  # vtk_module_autoinit is needed
  vtk_module_autoinit(
    TARGETS RenderWindowNoUiFile
    MODULES ${VTK_LIBRARIES}
    )
endif ()

