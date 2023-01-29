#pragma once

#include "Decima/Core/RTTIObject.h"
#include "Decima/Core/GGUUID.h"

class RTTIRefObject : public RTTIObject {
public:
    virtual void RTTIRefObject_Unk01() = 0;

    virtual void RTTIRefObject_Unk02() = 0;

public:
    GGUUID ObjectUUID;
    uint32_t RefCount;
};

assert_size(RTTIRefObject, 32);