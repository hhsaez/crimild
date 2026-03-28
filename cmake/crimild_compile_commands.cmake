function(crimild_link_compile_commands)
  if(NOT CMAKE_EXPORT_COMPILE_COMMANDS)
    return()
  endif()

  set(link_path "${CMAKE_SOURCE_DIR}/compile_commands.json")
  set(target_path "${CMAKE_BINARY_DIR}/compile_commands.json")

  if(EXISTS "${link_path}" OR IS_SYMLINK "${link_path}")
    file(REMOVE "${link_path}")
  endif()

  execute_process(
    COMMAND "${CMAKE_COMMAND}" -E create_symlink "${target_path}" "${link_path}"
    RESULT_VARIABLE link_result
    ERROR_VARIABLE link_error
  )

  if(NOT link_result EQUAL 0)
    message(WARNING "Failed to create compile_commands.json symlink at project root: ${link_error}")
  endif()
endfunction()

