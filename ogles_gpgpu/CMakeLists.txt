include(sugar_include)
sugar_include(.)

add_library(ogles_gpgpu ${OGLES_GPGPU_SRCS})

if(OGLES_GPGPU_VERBOSE)
  target_compile_definitions(ogles_gpgpu PUBLIC OGLES_GPGPU_VERBOSE=1)
endif()

## #################################################################
## Dependencies - OpenGL stuff
## #################################################################

if(APPLE)
  if(IOS)
    target_link_libraries(ogles_gpgpu PUBLIC
      "-framework ImageIO"
      "-framework CoreFoundation"
      "-framework Foundation" # NSLog
      "-framework OpenGLES"      
      )    
  else()
    target_link_libraries(ogles_gpgpu PUBLIC
      "-framework ImageIO"
      "-framework CoreFoundation"
      "-framework CoreVideo"
      "-framework OpenGL"
      )
  endif()
elseif(ANDROID)
  find_package(Android REQUIRED)
  find_package(Log REQUIRED)
  find_package(EGL REQUIRED)
  find_package(OpenGLES2 REQUIRED)
  target_link_libraries(ogles_gpgpu PUBLIC log android EGL GLESv2)
  target_compile_definitions(ogles_gpgpu PUBLIC EGL_EGLEXT_PROTOTYPES GL_GLEXT_PROTOTYPES)
else()

  if(OGLES_GPGPU_USE_OSMESA)

    find_library(OSMESA_LIBRARY OSMesa)
    if(NOT OSMESA_LIBRARY)
      message(FATAL_ERROR "OGLES_GPGPU was configured to use OSMesa, but the library could not be found")
    endif()

    get_filename_component(OSMESA_PATH ${OSMESA_LIBRARY} DIRECTORY)

    find_library(GLAPI_LIBRARY glapi HINTS ${OSMESA_PATH} PATHS ${OSMESA_PATH})
    if(NOT GLAPI_LIBRARY)
      message(FATAL_ERROR "OGLES_GPGPU was configured to use OSMesa, but the GLAPI dependency was not found")
    endif()

    find_library(EGL_LIBRARY EGL HINTS ${OSMESA_PATH} PATHS ${OSMESA_PATH})
    if(NOT EGL_LIBRARY)
      message(FATAL_ERROR "EGL_LIBRARY was configured to use OSMesa, but the EGL dependency was not found")
    endif()
    mark_as_advanced(OSMESA_LIBRARY GLAPI_LIBRARY EGL_LIBRARY)

    target_link_libraries(ogles_gpgpu PUBLIC ${OSMESA_LIBRARY} ${GLAPI_LIBRARY} ${EGL_LIBRARY})

    message("OSMESA_LIBRARY = ${OSMESA_LIBRARY}")
    message("GLAPI_LIBRARY = ${GLAPI_LIBRARY}")
    message("EGL_LIBRARY = ${EGL_LIBRARY}")
    message("OSMESA_PATH = ${OSMESA_PATH}")

  else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(ogles_gpgpu PUBLIC OpenGL::GL)
  endif()
  
  if(MSVC)
    # ogles_gpgpu/platform/opengl/gl_includes.h: #include <gl/glew.h>
    hunter_add_package(glew)
    find_package(glew CONFIG REQUIRED)
    target_link_libraries(ogles_gpgpu PUBLIC glew::glew)
    target_compile_definitions(ogles_gpgpu PUBLIC NOMINMAX) # avoid std::{min,max} conflicts
    target_compile_definitions(ogles_gpgpu PUBLIC _USE_MATH_DEFINES) # M_PI, etc
  endif()
endif()
  
set_property(TARGET ${library} PROPERTY FOLDER "libs/ogles_gpgpu")
target_include_directories(
    ogles_gpgpu PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>"
)

## #################################################################
## Testing: 
## #################################################################

if(OGLES_GPGPU_BUILD_TESTS)
  add_subdirectory(ut)
endif()

#
# Provide interface lib for clean package config use
#

if(NOT OGLES_GPGPU_INSTALL)
  return()
endif()

### Install ###
set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

set(config_install_dir "lib/cmake/${PROJECT_NAME}")
set(include_install_dir "include")

set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(targets_export_name "${PROJECT_NAME}Targets")
set(namespace "${PROJECT_NAME}::")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${version_config}" COMPATIBILITY SameMajorVersion
)

# Note: use 'targets_export_name'
configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/Config.cmake.in"
    "${project_config}"
     INSTALL_DESTINATION "${config_install_dir}"
)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT "${targets_export_name}"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib"
    RUNTIME DESTINATION "bin"
    INCLUDES DESTINATION "${include_install_dir}"
)

install(
    FILES
    ${OGLES_GPGPU_PUBLIC_HDRS}
    DESTINATION "${include_install_dir}/${PROJECT_NAME}"
)

install(
  DIRECTORY "${CMAKE_SOURCE_DIR}/ogles_gpgpu/" # Trailing slash is required
  DESTINATION "${include_install_dir}/${PROJECT_NAME}"
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
  )

install(
  FILES "${project_config}" "${version_config}"
  DESTINATION "${config_install_dir}"
  )

install(
  EXPORT "${targets_export_name}"
  NAMESPACE "${namespace}"
  DESTINATION "${config_install_dir}"
  )
