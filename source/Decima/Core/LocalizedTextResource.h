#pragma once

#include "Decima/Core/Resource.h"

#include <cstdint>
#include <string_view>

class LocalizedTextResource : public Resource {
public:
    class Entry {
    public:
        Entry *mPrev;
        Entry *mNext;
        LocalizedTextResource *mResource;
        uint64_t mStreamLocator;
        uint32_t mStreamPath;
        uint64_t mStreamStart;
        uint32_t mStreamLength;
    };

    std::string_view Text() const {
        return {mText, mTextLength};
    }

    const char *mText{};
    uint16_t mTextLength{};
    Entry *mEntry{};
};