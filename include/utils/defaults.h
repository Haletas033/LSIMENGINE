//
// Created by halet on 9/5/2025.
//

#ifndef DEFAULTS_H
#define DEFAULTS_H

struct Defaults {
    unsigned int MAX_LIGHTS = 8;
    unsigned int defaultWindowWidth = 1920;
    unsigned int defaultWindowHeight = 1080;

    //Terrain defaults
    unsigned int size = 256;
    float gridScale = 2.0f;
    float heightScale = 80.0f;
    float scale = 15.0f;
    int octaves = 8;
    float persistence = 0.5f;
    float lacunarity = 2.0f;

    //Sphere defaults
    int sphereSlices = 20;
    int sphereStacks = 20;

    //Torus defaults
    int torusRingSegments = 40;
    int torusTubeSegments = 20;
    float torusRingRadius = 1.0f;
    float torusTubeRadius = 0.3f;

    //Camera defaults
    float FOVdeg = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 10000.0f;
    float sensitivity = 100.0f;
    float speedMultiplier = 50.0f;

    //Input defaults
    float transformSpeed = 0.1f;
};

#endif //DEFAULTS_H
