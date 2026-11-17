#ifndef LSIM_SERIALIZATION_H
#define LSIM_SERIALIZATION_H
#include <fstream>

#include "ECS/registry.h"

class Serialization {
public:
        static void InitSerialization();

        static void saveToFile(std::ofstream &file, Registry &registry);

        static void loadFromFile(const Device& device, Command& command, std::ifstream& file,
                                 Registry& registry, SharedState& sharedState, const std::string& workingDir);
};

#endif //LSIM_SERIALIZATION_H
