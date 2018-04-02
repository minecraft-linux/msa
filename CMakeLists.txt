cmake_minimum_required (VERSION 2.6)

project(msa LANGUAGES CXX)

add_library(msa src/account.cpp src/device_auth.cpp src/login_manager.cpp src/network.cpp include/msa/account.h include/msa/device_auth.h include/msa/login_manager.h include/msa/token.h include/msa/compact_token.h include/msa/legacy_token.h include/msa/token_response.h include/msa/storage_manager.h include/msa/scope.h include/msa/network.h)
target_include_directories(msa PUBLIC include/)
target_link_libraries(msa logger base64 rapidxml)