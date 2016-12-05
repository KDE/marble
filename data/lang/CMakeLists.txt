# custom translation catalog , for creation of bundled binary packages
# TODO: disable for builds in normal linux distri builds, to not confuse people


# Find Qt translation tools
find_package(Qt5LinguistTools CONFIG)

# enable target in any case
add_custom_target(bundle_translations)

if(NOT Qt5LinguistTools_FOUND)
    return()
endif()


if(TARGET Qt5::lconvert)
    set(lconvert_executable Qt5::lconvert)
else()
    # Qt < 5.3.1 does not define Qt5::lconvert
    get_target_property(lrelease_location Qt5::lrelease LOCATION)
    get_filename_component(lrelease_path ${lrelease_location} PATH)
    find_program(lconvert_executable
        NAMES lconvert-qt5 lconvert
        PATHS ${lrelease_path}
        NO_DEFAULT_PATH
    )
endif()

function(marble_process_po_files_as_qm lang po_file)
    # Create commands to turn po files into qm files
    get_filename_component(po_file ${po_file} ABSOLUTE)
    get_filename_component(filename_base ${po_file} NAME_WE)

    # Include ${lang} in build dir because we might be called multiple times
    # with the same ${filename_base}
    set(build_dir ${CMAKE_CURRENT_BINARY_DIR}/locale/${lang})
    set(ts_file ${build_dir}/${filename_base}.ts)
    set(qm_file ${build_dir}/${filename_base}.qm)

    file(MAKE_DIRECTORY ${build_dir})

    # lconvert from .po to .ts, then lrelease from .ts to .qm.
    add_custom_command(OUTPUT ${qm_file}
        COMMAND ${lconvert_executable}
            ARGS -i ${po_file} -o ${ts_file} -target-language ${lang}
        COMMAND Qt5::lrelease
            ARGS -removeidentical -nounfinished -silent ${ts_file} -qm ${qm_file}
        DEPENDS ${po_file}
    )
    install(
        FILES ${qm_file}
        DESTINATION ${data_dir}/locale/${lang}
        OPTIONAL # if not build, ignore it
    )

    string(REPLACE "@" "AT" _lang ${lang})
    set(target_name translation_${_lang}_${filename_base})
    add_custom_target(${target_name} DEPENDS ${qm_file})
    add_dependencies(bundle_translations ${target_name})
endfunction()

file(GLOB po_files "po/*/*.po")
foreach(po_file ${po_files})
    get_filename_component(po_dir ${po_file} DIRECTORY)
    get_filename_component(lang ${po_dir} NAME)
    marble_process_po_files_as_qm(${lang} ${po_file})
endforeach()
