project(SimpleTracker)

# -+- Executables and their linked libraries

add_executable(play_video       src/PlayVideo.cpp       $<TARGET_OBJECTS:Exceptions> $<TARGET_OBJECTS:CVShortcuts> $<TARGET_OBJECTS:Geometry> $<TARGET_OBJECTS:Perception>)
target_link_libraries(play_video ${OpenCV_LIBS})

add_executable(simple_tracker   src/SimpleTracker.cpp ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_histories.sh   $<TARGET_OBJECTS:Exceptions> $<TARGET_OBJECTS:CVShortcuts> $<TARGET_OBJECTS:Geometry> $<TARGET_OBJECTS:Perception>)
target_link_libraries(simple_tracker ${OpenCV_LIBS})
#add_custom_command(TARGET simple_tracker POST_BUILD
#                   COMMAND ${CMAKE_COMMAND} -E copy_if_different
#                       ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_histories.sh
#                       $<TARGET_FILE_DIR:simple_tracker>)
configure_file(scripts/generate_histories.sh    generate_histories.sh COPYONLY)

add_executable(replay_history	src/ReplayHistory.cpp	$<TARGET_OBJECTS:Exceptions> $<TARGET_OBJECTS:CVShortcuts> $<TARGET_OBJECTS:Geometry> $<TARGET_OBJECTS:Perception>
                                scripts/generate_histories.sh)
target_link_libraries(replay_history ${OpenCV_LIBS})

#add_custom_command(OUTPUT generate_histories.sh
#                   COMMAND ${CMAKE_COMMAND} -E copy_if_different
#                           ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_histories.sh
#                           generate_histories.sh)
#add_custom_target(Scripts DEPENDS generate_histories.sh)
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_histories.sh
#     DESTINATION $<TARGET_FILE_DIR:replay_history>)