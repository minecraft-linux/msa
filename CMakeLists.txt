cmake_minimum_required (VERSION 2.6)

project(msa LANGUAGES CXX)

add_library(msa src/msa.cpp src/network.cpp)
target_include_directories(msa PUBLIC include/)
target_link_libraries(msa logger base64 rapidxml)