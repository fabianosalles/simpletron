cmake_minimum_required(VERSION 3.20)
project(simpletron CXX)

# Define a versão do padrão C++ (ex: C++17 ou C++20)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# Inclui os arquivos de código-fonte da sua pasta 'src'
# Adicione aqui todos os arquivos .cpp que seu projeto usa para compilar
add_executable(simpletron
        src/main.cpp
        src/simpletron.cpp
)

target_include_directories(simpletron PRIVATE include)