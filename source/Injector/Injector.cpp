#include "Injector.h"

#include "Decima/Core/ExportedSymbolGroup.h"
#include "Decima/Core/StreamingManager.h"
#include "Decima/Core/LocalizedTextResource.h"
#include "Decima/PCore/Ref.h"
#include "Util/Offsets.h"

#include <Windows.h>
#include <detours.h>
#include <nlohmann/json.hpp>

#include <fstream>
#include <unordered_map>

static void (*RTTIFactory_RegistersSymbols)(void *);

static void RTTIFactory_RegistersSymbols_Hook(void *inUnk) {
    RTTIFactory_RegistersSymbols(inUnk);

    static auto registered = []() {
        auto offset = Offsets::GetModule().first;

        for (auto &[symbol, hash]: ExportedSymbols::Get().mAllSymbols) {
            if (symbol->mKind == ExportedSymbol::Kind::Function) {
                Offsets::MapAddress(symbol->mName, reinterpret_cast<uintptr_t>(symbol->mLanguage[0].mAddress) - offset);
            }
        }
        return true;
    }();
}

static void *gMemAlloc(size_t inSize) {
    using MallocFn = void *(*)(size_t inSize);
    return Offsets::CallID<"gExportedMemAlloc", MallocFn>(inSize);
}

static void gMemFree(void *inPtr) {
    using FreeFn = void (*)(void *inPtr);
    return Offsets::CallID<"gExportedMemFree", FreeFn>(inPtr);
}

class LocalizationListener final : public IStreamingSystem::Events {
public:
    std::unordered_map<GGUUID, std::string> localization;

    static LocalizationListener &Get() {
        static LocalizationListener instance;
        return instance;
    }

    void OnFinishLoadGroup(const Array<Ref<RTTIRefObject> > &inObjects) override {
        for (const auto &Object: inObjects) {
            if (Object->GetRTTI()->Name() == "LocalizedTextResource") {
                auto &text = const_cast<LocalizedTextResource &>(static_cast<const LocalizedTextResource &>(*Object));
                auto it = localization.find(text.ObjectUUID);

                if (it == localization.cend()) {
                    // std::println("No localization found for {}", text.ObjectUUID);
                    continue;
                }

                auto actual = text.Text();
                auto replacement = it->second;

                if (actual == replacement)
                    continue;

                // std::println("Replacing text for {}: '{}' -> '{}'", text.ObjectUUID, actual, replacement);

                auto buffer = static_cast<char *>(gMemAlloc(replacement.size()));
                memcpy(buffer, replacement.data(), replacement.size());

                gMemFree(const_cast<char *>(text.mText));
                text.mText = static_cast<const char *>(buffer);
                text.mTextLength = static_cast<uint16_t>(replacement.size());
            }
        }
    }
};

static StreamingManager *(*StreamingManager_Constructor)(StreamingManager *);

static StreamingManager *StreamingManager_Constructor_Hook(StreamingManager *manager) {
    auto result = StreamingManager_Constructor(manager);
    manager->mStreamingSystem->AddEventListener(&LocalizationListener::Get());
    return result;
}

void Injector::Attach() {
    // @formatter:off
    Offsets::MapAddress("RTTIFactory::sExportedSymbols", Offsets::OffsetFromInstruction("48 8B 3D ? ? ? ? 48 63 0D ? ? ? ? 40 88 6C 24 ? 48 89 7C 24 ? 48 8D 04 CF 48", 3) - 8);
    Offsets::MapAddress("StreamingManager::Instance", Offsets::OffsetFromInstruction("48 89 05 ? ? ? ? 41 83 FA 07 77 36 49 63 C2 41 8B 8C 83 ? ? ? ? 49 03 CB FF E1 48 8B 05", 3));
    Offsets::MapSignature("StreamingManager::StreamingManager", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC 20 33 DB 48 8D 05");
    Offsets::MapSignature("RTTIFactory::RegisterSymbols", "48 89 4C 24 ? 41 56 48 83 EC 50 48 89 5C 24 ? 48 8D 0D ? ? ? ? 48 89");
    // @formatter:on

    StreamingManager_Constructor = Offsets::ResolveID<"StreamingManager::StreamingManager", decltype(StreamingManager_Constructor)>();
    RTTIFactory_RegistersSymbols = Offsets::ResolveID<"RTTIFactory::RegisterSymbols", decltype(RTTIFactory_RegistersSymbols)>();

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(reinterpret_cast<PVOID *>(&StreamingManager_Constructor), static_cast<PVOID>(StreamingManager_Constructor_Hook));
    DetourAttach(reinterpret_cast<PVOID *>(&RTTIFactory_RegistersSymbols), static_cast<PVOID>(RTTIFactory_RegistersSymbols_Hook));
    DetourTransactionCommit();

    std::ifstream stream{"localization.json"};
    if (stream.fail()) {
        // std::println("Failed to open localization file 'localization.json'");
        return;
    }

    const auto json = nlohmann::json::parse(stream);

    auto &localization = LocalizationListener::Get().localization;
    for (auto it = json.cbegin(); it != json.cend(); ++it) {
        auto guid = GGUUID::Parse(it.key());
        auto text = it.value().get<std::string>();
        localization.emplace(guid, std::move(text));
    }

    // std::println("Loaded {} localization entries", localization.size());
}

void Injector::Detach() {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(reinterpret_cast<PVOID *>(&RTTIFactory_RegistersSymbols), static_cast<PVOID>(RTTIFactory_RegistersSymbols_Hook));
    DetourTransactionCommit();
}
