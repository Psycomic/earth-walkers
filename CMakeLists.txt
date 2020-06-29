project(EarthWalkers)

include_directories(
  "C:/Users/phi/Downloads/glew-2.1.0/include"
  "C:/Users/phi/Downloads/glfw-3.3.2.bin.WIN64/include")
link_directories(
  "C:/Users/phi/Downloads/glew-2.1.0/lib/Release/x64"
  "c:/Users/phi/Downloads/glfw-3.3.2.bin.WIN64/lib-vc2019")

add_executable(EarthWalkers
  noise.c
  linear_algebra.c
  render.c
  misc.c
  terrain.c
  main.c)

target_link_libraries(EarthWalkers PRIVATE opengl32 glew32 glfw3)
