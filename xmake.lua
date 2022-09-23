set_project("JobSystem")

add_rules("mode.debug", "mode.release")

set_warnings("all")
set_languages("cxx11")

-- xmake project -k vsxmake

target("main")
    set_kind("binary")
    add_files("src/*.cpp")
    add_headerfiles("src/*.h")
    -- add_includedirs("src/include")
    add_packages("glew", "glfw", "glm", "glad")