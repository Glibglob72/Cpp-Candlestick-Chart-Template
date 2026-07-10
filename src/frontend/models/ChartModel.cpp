#include "ChartModel.h"

namespace Origin {

void ChartModel::setNumDays(int days) {
    if (days != m_numDays && days > 0) {
        m_numDays = days;
        notifyChange();
    }
}

void ChartModel::setCurrentFile(const std::string& file) {
    if (file != m_currentFile) {
        m_currentFile = file;
        notifyChange();
    }
}

void ChartModel::notifyChange() {
    if (m_onChange) {
        m_onChange();
    }
}

} // namespace Origin
