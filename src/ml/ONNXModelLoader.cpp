// ONNXModelLoader.cpp

#include "ONNXModelLoader.h"
#include <stdexcept>

// Constructor: Initializes the ONNX Runtime environment and loads the model.
ONNXModelLoader::ONNXModelLoader(const std::string& modelPath)
    : env(ORT_LOGGING_LEVEL_WARNING, "ONNXModelLoader"),
      session(env, modelPath.c_str(), Ort::SessionOptions{nullptr}),
      loaded(true)
{
    // Additional session configurations can be added here if necessary.
}

// Destructor: Handles any necessary cleanup.
ONNXModelLoader::~ONNXModelLoader()
{
    // Destructor logic if needed.
}

// Returns the ONNX Runtime session.
Ort::Session& ONNXModelLoader::getSession()
{
    if (!loaded)
    {
        throw std::runtime_error("Model is not loaded.");
    }
    return session;
}

// Checks if the model has been successfully loaded.
bool ONNXModelLoader::isModelLoaded() const
{
    return loaded;
}
