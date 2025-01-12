#pragma once

template <class T>
class FormSingleton {
    T* singleton = nullptr;
    const std::string editorId;
    public:
    FormSingleton(const std::string editorId) : editorId(editorId) {}
    inline T* Get() {
        if (!singleton) {
            singleton = RE::TESForm::LookupByEditorID<T>(editorId);
        }
        return singleton;
    }
};