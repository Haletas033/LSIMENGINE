#ifndef LSIMENGINE_STYLES_CSS_H
#define LSIMENGINE_STYLES_CSS_H

#include "style.h"

//Currently a test
inline void LSIMENGINE_Styles() {
    Style LSIMENGINE_H2("#LSIMENGINE h2");
    LSIMENGINE_H2
        .put("color", "orangered");

    Builder::Build("LSIMENGINE_Styles");
}

#endif //LSIMENGINE_STYLES_CSS_H
