TEMPLATE = aux

#ENABLE_DOCS = 1
#DOCS_ARGS = -md

!isEmpty(ENABLE_DOCS){
    LIVEDOC_PATH = $(LIVEDOC)

    # Debug and Release checks will make sure to trigger documentation generation only once
    # as qmake tends to pass through this file multiple times.
    isEmpty(DOCS_ARGS){
        Debug:first.commands += node \"$${LIVEDOC_PATH}\" --deploy \"$$DEPLOY_PATH\" \"$${PROJECT_ROOT}\"
        Release:first.commands += node \"$${LIVEDOC_PATH}\" --deploy \"$$DEPLOY_PATH\" \"$${PROJECT_ROOT}\"
    }
    !isEmpty(DOCS_ARGS){
        Debug:first.commands += node \"$${LIVEDOC_PATH}\" --deploy \"$$DEPLOY_PATH\" \"$${DOCS_ARGS}\" \"$${PROJECT_ROOT}\"
        Release:first.commands += node \"$${LIVEDOC_PATH}\" --deploy \"$$DEPLOY_PATH\" \"$${DOCS_ARGS}\" \"$${PROJECT_ROOT}\"
    }
    QMAKE_EXTRA_TARGETS += first
}
