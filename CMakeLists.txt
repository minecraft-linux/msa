cmake_minimum_required (VERSION 2.6)

project(msa LANGUAGES CXX)

find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

add_library(msa src/account.cpp src/device_auth.cpp src/login_manager.cpp src/legacy_network.cpp include/msa/account.h include/msa/device_auth.h include/msa/login_manager.h include/msa/token.h include/msa/compact_token.h include/msa/legacy_token.h include/msa/token_response.h include/msa/storage_manager.h include/msa/scope.h include/msa/legacy_network.h src/network/request.cpp include/msa/network/request.h src/network/server_time.cpp include/msa/network/server_time.h src/network/device_add_request.cpp include/msa/network/device_add_request.h src/network/server_config.cpp include/msa/network/server_config.h include/msa/network/service_exception.h src/network/service_exception.cpp include/msa/parse_exception.h include/msa/xml_utils.h include/msa/network/security_token_request.h src/network/security_token_request.cpp include/msa/network/device_authenticate_request.h src/network/device_authenticate_request.cpp src/token.cpp src/legacy_token.cpp src/compact_token.cpp src/token_response.cpp include/msa/network/account_token_request.h src/network/account_token_request.cpp include/msa/network/request_utils.h src/network/request_utils.cpp include/msa/network/crypto_utils.h src/network/crypto_utils.cpp include/msa/network/xml_sign_context.h src/network/xml_sign_context.cpp src/xml_utils.cpp)
target_include_directories(msa PUBLIC include/)
target_include_directories(msa PRIVATE ${OPENSSL_INCLUDE_DIR} ${CURL_INCLUDE_DIRS})
target_link_libraries(msa logger base64 rapidxml)
target_link_libraries(msa curl ${CURL_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARY})