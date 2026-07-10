#pragma once

#include "../../common/Types.h"
#include <functional>

namespace Origin {
    class ChartModel {
    public:
        using ChangeCallback = std::function<void()>;

        // Get/set number of days to display
        void setNumDays(int days);
        int getNumDays() const { return m_numDays; }

        // Get/set current file
        void setCurrentFile(const std::string& file);
        const std::string& getCurrentFile() const { return m_currentFile; }

        // Set callback for when model changes
        void setOnChange(ChangeCallback callback) { m_onChange = callback; }

    private:
        void notifyChange();

        int m_numDays = 5;
        std::string m_currentFile;
        ChangeCallback m_onChange;
    };
}
