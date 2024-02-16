# Source directories
file(GLOB_RECURSE SOURCES "src/**.cpp" "src/**.c" "src/**.hpp" "src/**.h")

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS "src")
