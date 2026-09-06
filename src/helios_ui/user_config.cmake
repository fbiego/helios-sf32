# =============================================================================
# user_config.cmake - Add your custom source files here
# =============================================================================
#
# This file is included by the generated CMakeLists.txt and allows you to
# add extra source files to the project without modifying generated files
# (which may be overwritten).
#
# To add your own sources, append them to LV_EDITOR_PROJECT_SOURCES:
#
#   list(APPEND LV_EDITOR_PROJECT_SOURCES
#       ${CMAKE_CURRENT_LIST_DIR}/src/my_widget.c
#       ${CMAKE_CURRENT_LIST_DIR}/src/my_screen.c
#   )
#
# Tip:
#   - Use ${CMAKE_CURRENT_LIST_DIR} to get paths relative to this file
#
# =============================================================================


list(APPEND LV_EDITOR_PROJECT_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/custom/apps/app_screens.c
    ${CMAKE_CURRENT_LIST_DIR}/custom/apps/app_init.c
    ${CMAKE_CURRENT_LIST_DIR}/custom/apps/app_manager.c
    ${CMAKE_CURRENT_LIST_DIR}/custom/watchfaces/watchface_manager.c
    ${CMAKE_CURRENT_LIST_DIR}/custom/apps/contacts/contacts.c
    ${CMAKE_CURRENT_LIST_DIR}/custom/apps/mpesa/mpesa.c
    ${CMAKE_CURRENT_LIST_DIR}/custom/apps/notifications/notifications.c
    ${CMAKE_CURRENT_LIST_DIR}/custom/apps/stopwatch/stopwatch.c
    ${CMAKE_CURRENT_LIST_DIR}/custom/apps/weather/weather.c
    ${CMAKE_CURRENT_LIST_DIR}/custom/subjects/subjects.c
    ${CMAKE_CURRENT_LIST_DIR}/custom/events/events.c
)

if(NOT DEFINED HELIOS_INCLUDE_WATCHFACE_PREVIEWS)
    if(DEFINED LVED_PROJECT_NAME)
        set(HELIOS_INCLUDE_WATCHFACE_PREVIEWS ON)
    else()
        set(HELIOS_INCLUDE_WATCHFACE_PREVIEWS OFF)
    endif()
endif()

if(HELIOS_INCLUDE_WATCHFACE_PREVIEWS)
    list(APPEND LV_EDITOR_PROJECT_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/custom/watchfaces/watchface_previews.c
        ${CMAKE_CURRENT_LIST_DIR}/images/preview/img_preview_240_data.c
        ${CMAKE_CURRENT_LIST_DIR}/images/preview/img_preview_280_data.c
        ${CMAKE_CURRENT_LIST_DIR}/images/preview/img_preview_360_data.c
        ${CMAKE_CURRENT_LIST_DIR}/images/preview/img_preview_390_data.c
        ${CMAKE_CURRENT_LIST_DIR}/images/preview/img_preview_410_data.c
        ${CMAKE_CURRENT_LIST_DIR}/images/preview/img_preview_466_data.c
    )
endif()

file(GLOB_RECURSE FACE_C_FILES CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_LIST_DIR}/custom/watchfaces/bin/*.c"
)
list(APPEND LV_EDITOR_PROJECT_SOURCES ${FACE_C_FILES})

file(GLOB_RECURSE APP_C_FILES CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_LIST_DIR}/custom/apps/apps/*.c"
)
list(APPEND LV_EDITOR_PROJECT_SOURCES ${APP_C_FILES})
