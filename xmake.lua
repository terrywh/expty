set_project("algorithm")
set_languages("c17", "cxx20")

local vendor = {
    boost = "/data/vendor/boost",
}

add_rules("mode.debug", "mode.release")
add_includedirs(vendor["boost"] .. "/include")
add_linkdirs(vendor["boost"] .. "/lib")

target("expty")
    add_files("src/*.cpp")
    add_links("boost_program_options", "util", "pthread")
