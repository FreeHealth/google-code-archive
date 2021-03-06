# Qt 5.1 cache system
qt:greaterThan(QT_MAJOR_VERSION, 4) {
    cache()
}

TEMPLATE = subdirs

SUBDIRS += translation \
    aggregation \
    extension \
    utils \
    medical

LIBSPATH=../../libs

aggregation.subdir  = $${LIBSPATH}/aggregation
aggregation.depends =

extension.subdir    = $${LIBSPATH}/extensionsystem
extension.depends   += aggregation

translation.subdir  = $${LIBSPATH}/translationutils
translation.depends =

utils.subdir        = $${LIBSPATH}/utils
utils.depends       += translation

medical.subdir      = $${LIBSPATH}/medicalutils
