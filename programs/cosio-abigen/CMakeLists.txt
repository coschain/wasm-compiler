set(SOURCES main.cpp)
find_package(LLVM 4.0 REQUIRED CONFIG)

link_directories(${LLVM_LIBRARY_DIR})

add_executable(cosio-abigen ${SOURCES})

set( CMAKE_CXX_STANDARD 14 )


target_link_libraries(cosio-abigen abi_generator ${Boost_LIBRARIES} nlohmann_json::nlohmann_json)

install( TARGETS
   cosio-abigen
   RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
   LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
   ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
)
