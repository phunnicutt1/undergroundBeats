#pragma once

#include <string>
#include <memory>
#include <onnxruntime_cxx_api.h> // Include ONNX Runtime C++ API

namespace undergroundBeats {
namespace ml {

/**
 * @class ONNXModelLoader
 * @brief Responsible for loading and managing ONNX models.
 */
class ONNXModelLoader {
public:
    /**
     * @brief Constructor. Initializes the ONNX Runtime environment.
     */
    ONNXModelLoader();

    /**
     * @brief Destructor.
     */
    ~ONNXModelLoader();

    /**
     * @brief Loads an ONNX model from the specified file path.
     * @param modelPath Path to the .onnx model file.
     * @return A unique pointer to the ONNX session if successful, nullptr otherwise.
     */
    std::unique_ptr<Ort::Session> loadModel(const std::string& modelPath);

private:
    Ort::Env env; // ONNX Runtime environment
    Ort::SessionOptions sessionOptions; // Session options
};

} // namespace ml
} // namespace undergroundBeats
