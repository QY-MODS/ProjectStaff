
#pragma once
namespace FormSerializer {
    inline RE::TESForm* GetForm(const char* fileName, RE::FormID localId) {
        const auto dataHandler = RE::TESDataHandler::GetSingleton();
        auto formId = dataHandler->LookupFormID(localId, fileName);
        return RE::TESForm::LookupByID(formId);
    }
}

class Serializer {
    SKSE::SerializationInterface* serializationInterface;

public:
    Serializer(SKSE::SerializationInterface* serializationInterface) : serializationInterface(serializationInterface) {}
    ~Serializer() {}

    template <class T>
    void Write(T value) {
        serializationInterface->WriteRecordData<T>(value);
    }

    template <class T>
    inline T Read() {
        T result;
        serializationInterface->ReadRecordData<T>(result);
        return result;
    }

    inline char* ReadString() {
        size_t arrayLength = Read<uint32_t>();
        char* result = new char[arrayLength + 1];
        for (size_t i = 0; i < arrayLength; i++) {
            result[i] = Read<char>();
        }
        result[arrayLength] = '\0';
        return result;
    }

    inline void WriteString(const char* string) {
        size_t arrayLength = strlen(string);
        Write<uint32_t>(static_cast<uint32_t>(arrayLength));
        for (size_t i = 0; i < arrayLength; i++) {
            char item = string[i];
            Write<char>(item);
        }
    }

    inline void WriteForm(RE::TESForm* form) {

        if (!form) {
            Write<char>(0);
            return;
        }

        auto formId = form->GetFormID();

        if (formId == 0) {
            return;
        }

        const auto dataHandler = RE::TESDataHandler::GetSingleton();

        auto modId = (formId >> 24) & 0xff;

        if (modId == 0xff) {
            Write<char>(2);
            Write<uint32_t>(formId);
        }
        else if (modId == 0xfe) {
            auto lightId = (formId >> 12) & 0xFFF;
            auto file = dataHandler->LookupLoadedLightModByIndex(lightId);
            if (file) {
                auto localId = formId & 0xFFF;
                std::string fileName = file->fileName;
                Write<char>(1);
                WriteString(fileName.c_str());
                Write<uint32_t>(localId);
            } else {
                Write<char>(0);
            }
        } else {
            auto file = dataHandler->LookupLoadedModByIndex(modId);
            if (file) {
                auto localId = formId & 0xFFFFFF;
                std::string fileName = file->fileName;
                Write<char>(1);
                WriteString(fileName.c_str());
                Write<uint32_t>(localId);
            } else {
                Write<char>(0);
            }
        }
    }

    inline RE::TESForm* ReadForm() {
        char fileRef = Read<char>();

        if (fileRef == 0) {
            logger::trace("nullref");
            return 0;
        }

        if (fileRef == 1) {
            logger::trace("existent ref");
            auto fileName = ReadString();
            uint32_t localId = Read<uint32_t>();
            return FormSerializer::GetForm(fileName, localId);
        }
        else if (fileRef == 2) {
            logger::trace("dynamic form");
            uint32_t id = Read<uint32_t>();
            return RE::TESForm::LookupByID(id);
        } 

        return 0;
    }

    template <class T>
    T* ReadForm() {
        auto form = ReadForm();
        if (!form) {
            logger::trace("nullptr");
            return nullptr;
        }
        auto result = form->As<T>();
        if (!result) {
            logger::trace("bad type:{}", form->GetName());
            return nullptr;
        }    

        return result;
    }


private:
};
