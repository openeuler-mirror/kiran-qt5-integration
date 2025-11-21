configure_file(kiran-integration-theme.pc.in kiran-integration-theme.pc @ONLY)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/kiran-qt5-integration.ini
        DESTINATION ${KQI_INSTALL_SYSCONFDIR})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kiran-integration-theme.pc
        DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig/)
