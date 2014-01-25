# TODO implement proper resolution of GTest/GMock

SET(GMOCK_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../download/gmock-1.6.0")

LINK_DIRECTORIES("${GMOCK_DIR}/build")
INCLUDE_DIRECTORIES("${GMOCK_DIR}/include")
LINK_DIRECTORIES("${GMOCK_DIR}/gtest/build")
INCLUDE_DIRECTORIES("${GMOCK_DIR}/gtest/include")