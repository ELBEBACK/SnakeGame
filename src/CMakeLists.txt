add_library(snake_lib
    controller/Controller.cpp
    model/Model.cpp
    model/Snake.cpp
    model/Gecko.cpp
    view/View.cpp
    view/TextVisual.cpp
    view/GraphicVisual.cpp
)

target_include_directories(snake_lib
    PUBLIC ${CMAKE_SOURCE_DIR}/include
)


target_compile_options(snake_lib PRIVATE -Wall -Wextra -Wpedantic)

target_link_libraries(snake_lib PUBLIC SFML::Graphics)


add_executable(Snakes main.cpp)

target_link_libraries(Snakes PRIVATE snake_lib)

target_compile_options(Snakes PRIVATE
    -Wall -Wextra -Wpedantic
)
