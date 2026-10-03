#ifndef LSIM_EDITORGUI_H
#define LSIM_EDITORGUI_H
#include <optional>

#include "LSIMtypes.h"
#include "sharedState.h"

class Engine;

class EditorGUI {
private:

        friend class Engine;

        static std::optional<LSIM::Error> updateGUI(SharedState &sharedState, Scene &scene, EntityHandle skybox, double mouseX, double mouseY);
};

#endif //LSIM_EDITORGUI_H
