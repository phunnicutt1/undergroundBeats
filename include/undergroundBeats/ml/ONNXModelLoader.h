#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <juce_audio_basics/juce_audio_basics.h>

namespace undergroundBeats {
namespace ml {

/**
 * @class ONNXModelLoader
 * @brief Utility class for loading and using ONNX models
 * 
 * This class provides functionality to load ONNX models and run inference
 * for audio processing tasks.
 */
class ONNXModelLoader {
public:
    /**
     * @brief Constructor
     */
    ONNXModelLoader();
    
    /**
     * @brief Destructor
     */
    ~ONNXModelLoader();
    
    /**
     * @brief Load an ONNX model from file
     * @param modelPath Path to the ONNX model file
     * @return True if loading succeeds, false otherwise
     */
    bool loadModel(const std::string& modelPath);
    
    /**
     * @brief Run inference on audio data
     * @param inputData Input audio data as a flat vector
     * @param inputShape Shape of the input tensor
     * @param outputNames Names of the output tensors to retrieve
     * @return Map of output tensor names to output data
     */
    std::unordered_map<std::string, std::vector<float>> runInference(
        const std::vector<float>& inputData,
        const std::vector<int64_t>& inputShape,
        const std::vector<std::string>& outputNames);
    
    /**
     * @brief Check if a model is loaded
     * @return True if a model is loaded, false otherwise
     */
    bool isModelLoaded() const;
    
    /**
     * @brief Get the input tensor names
     * @return Vector of input tensor names
     */
    std::vector<std::string> getInputNames() const;
    
    /**
     * @brief Get the output tensor names
     * @return Vector of output tensor names
     */
    std::vector<std::string> getOutputNames() const;
    
    /**
     * @brief Get the input tensor shape
     * @param inputName Name of the input tensor
     * @return Vector representing the shape of the input tensor
     */
    std::vector<int64_t> getInputShape(const std::string& inputName) const;
    
    /**
     * @brief Get the output tensor shape
     * @param outputName Name of the output tensor
     * @return Vector representing the shape of the output tensor
     */
    std::vector<int64_t> getOutputShape(const std::string& outputName) const;
    
private:
    // Private implementation to hide ONNX dependencies
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace ml
} // namespace undergroundBeats
