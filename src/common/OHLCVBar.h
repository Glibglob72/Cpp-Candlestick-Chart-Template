#pragma once

#include "Types.h"
#include <algorithm>

namespace Origin {
    struct OHLCVBar {
        Timestamp timestamp;
        Price open;
        Price high;
        Price low;
        Price close;
        Volume volume;

        bool isBullish() const { return close >= open; }
        Price bodyTop() const { return std::max(open, close); }
        Price bodyBottom() const { return std::min(open, close); }
        Price range() const { return high - low; }
    };

    using OHLCVData = std::vector<OHLCVBar>;
}
