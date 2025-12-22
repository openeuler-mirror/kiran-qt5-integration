set(TS_FILES 
    ${PROJECT_SOURCE_DIR}/translations/kiran-integration.zh_CN.ts)
    
set(TRANSLATIONS_SRC_DIR
    ${PROJECT_SOURCE_DIR}/lib
    ${PROJECT_SOURCE_DIR}/plugins)

execute_process(COMMAND lupdate-qt5 -no-obsolete ${TRANSLATIONS_SRC_DIR} -ts ${TS_FILES})
qt5_add_translation(INTEGRATION_QM_FILES ${TS_FILES})

# qm生成,默认构建依赖
add_custom_target(translations_qm ALL
    DEPENDS ${INTEGRATION_QM_FILES}
    COMMENT "Generating QM translation files"
)

install(FILES ${INTEGRATION_QM_FILES} DESTINATION ${KQI_INSTALL_TRANSLATIONDIR})