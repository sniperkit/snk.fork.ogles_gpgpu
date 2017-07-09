set(test_name OGLES_GPGPU_Test)
set(test_app test-ogles_gpgpu)

add_executable(${test_app} test-ogles_gpgpu.cpp)
target_link_libraries(${test_app} PUBLIC ogles_gpgpu ${OGLES_GPGPU_TEST_LIBS})
set_property(TARGET ${test_app} PROPERTY FOLDER "app/tests")

if(OGLES_GPGPU_DO_GPU_TESTING)
  # TODO: Lightweight portable OpenGL context for mobile platforms  
  gauze_add_test(
    NAME "${test_name}"
    COMMAND "${test_app}"
    )
endif()
