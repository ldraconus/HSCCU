#include "resizenotifier.h"

#ifdef __wasm__
#include <emscripten.h>
#include <emscripten/html5.h>

static EM_BOOL resizeCallback(int, const EmscriptenUiEvent *, void *) {
    emit ResizeNotifier::instance().resized();
    return EM_FALSE;
}
#endif

ResizeNotifier &ResizeNotifier::instance() {
    static ResizeNotifier notifier;
    return notifier;
}

ResizeNotifier::ResizeNotifier(QObject *parent) : QObject(parent) {
#ifdef __wasm__
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, resizeCallback);
#endif
}
