add_library(snake_lib
    controller/Controller.cpp
    model/Model.cpp
    model/Snake.cpp
    model/Gecko.cpp
    view/View.cpp
    view/TextVisual.cpp
)

target_include_directories(snake_lib
    PUBLIC ${CMAKE_SOURCE_DIR}/include
)

add_executable(Snakes main.cpp)
target_link_libraries(Snakes PRIVATE snake_lib)
