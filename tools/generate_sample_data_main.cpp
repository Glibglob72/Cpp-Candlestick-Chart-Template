#include "SampleDataGenerator.h"
#include <cstdlib>
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: generate_sample_data <output.csv> [tradingDays] [seed]\n";
        return 1;
    }

    Origin::SampleDataOptions options;
    if (argc > 2) options.tradingDays = std::atoi(argv[2]);
    if (argc > 3) options.seed = static_cast<unsigned int>(std::strtoul(argv[3], nullptr, 10));

    if (options.tradingDays <= 0) {
        std::cerr << "tradingDays must be positive\n";
        return 1;
    }

    Origin::writeSampleCsv(argv[1], options);
    std::cout << "Wrote " << options.tradingDays << " trading days of sample data to "
              << argv[1] << "\n";
    return 0;
}
