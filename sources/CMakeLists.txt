#
# Test CMake version
#
CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)



# And now we add any targets that we want
add_executable(CppChess main.cpp AI.cpp Board.cpp Drawer.cpp Gamelogic.cpp Piece.cpp Player.cpp Primitives.cpp TextDrawer.cpp Trans_Table.cpp)
target_link_libraries(CppChess pthread SDL2 SDL2_ttf SDL2_image)




