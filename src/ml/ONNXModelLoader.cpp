// ONNXModelLoader.cpp

#include "../../include/undergroundBeats/ml/ONNXModelLoader.h"
#include <stdexcept>

namespace undergroundBeats {
namespace ml {

// Constructor: Initializes the ONNX Runtime environment.
// We don't load the model here anymore, just create the environment.
ONNXModelLoader::ONNXModelLoader()
    : env(ORT_LOGGING_LEVEL_WARNING, "ONNXModelLoaderEnv")
{
    // Initialization, potentially setup OrtEnv options
}

// Destructor: Handles any necessary cleanup.
ONNXModelLoader::~ONNXModelLoader()
{
    // Destructor logic if needed (OrtEnv handles its own cleanup).
}

// Returns the ONNX Runtime environment.
Ort::Env& ONNXModelLoader::getEnvironment()
{
    return env;
}

} // namespace ml
} // namespace undergroundBeats
