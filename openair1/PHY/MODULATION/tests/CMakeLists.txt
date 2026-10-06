# SPDX-License-Identifier: LicenseRef-CSSL-1.0

add_executable(test_nr_modulation test_nr_modulation.cpp)
target_link_libraries(test_nr_modulation PRIVATE PHY_NR GTest::gtest minimal_lib)
add_dependencies(tests test_nr_modulation)
add_test(NAME test_nr_modulation
  COMMAND ./test_nr_modulation)

# // !!!!!!!!! for modified OTFS !!!!!!!!!
add_executable(test_nr_motfs test_nr_motfs.cpp)
target_link_libraries(test_nr_motfs PRIVATE PHY_NR GTest::gtest minimal_lib)
add_dependencies(tests test_nr_motfs)
add_test(NAME test_nr_motfs
  COMMAND ./test_nr_motfs)

# // !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
add_executable(test_nr_ddchan test_nr_ddchan.cpp)
target_link_libraries(test_nr_ddchan PRIVATE PHY_NR GTest::gtest minimal_lib)
add_dependencies(tests test_nr_ddchan)
add_test(NAME test_nr_ddchan
  COMMAND ./test_nr_ddchan)
