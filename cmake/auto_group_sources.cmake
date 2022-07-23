function(auto_group_sources sources prefix)
	foreach(file ${${sources}})
    get_filename_component(parent_dir ${file} PATH)

    string(REPLACE ${prefix} "" group ${parent_dir})

    source_group(${group} FILES ${file})
  endforeach()
endfunction()