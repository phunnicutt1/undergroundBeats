#pragma once

#include "undergroundBeats/ml/AudioSourceSeparator.h"
#include "undergroundBeats/ml/ONNXModelLoader.h"
#include <onnxruntime_cxx_api.h>
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace undergroundBeats {
namespace ml {

/**
 * @class ONNXSourceSeparator
 * @brief Implements audio source separation using an ONNX model.
 */
class ONNXSourceSeparator : public AudioSourceSeparator {
public:
    /**
     * @brief Default constructor for creating an empty separator instance.
     */
    ONNXSourceSeparator();

    /**
     * @brief Constructor. Loads the ONNX model using the provided loader.
     * @param modelPath Path to the .onnx source separation model file.
     * @param modelLoader A reference to the ONNXModelLoader instance.
     */
    ONNXSourceSeparator(const std::string& modelPath, ONNXModelLoader& modelLoader);

    /**
     * @brief Destructor.
     */
    ~ONNXSourceSeparator() override = default;

    /**
     * @brief Processes an input audio buffer using the loaded ONNX model.
     * @param inputBuffer The input audio buffer containing the mixed signal.
     * @return A map of source names to separated audio buffers.
     */
    std::map<std::string, juce::AudioBuffer<float>> process(const juce::AudioBuffer<float>& inputBuffer) override;

    /**
     * @brief Gets the names of the sources produced by the loaded model.
     *        (Placeholder - needs implementation based on model metadata or configuration).
     * @return A vector of source names.
     */
    std::vector<std::string> getSourceNames() const override;

    /**
     * @brief Checks if the model was loaded successfully and is ready for inference.
     * @return True if the ONNX session is valid, false otherwise.
     */
    bool isReady() const override;
    
    /**
     * @brief Checks if the separator is initialized and has stems loaded.
     * @return True if initialized with separated stems, false otherwise.
     */
    bool isInitialized() const;
    
    /**
     * @brief Load and separate an audio file into stems.
     * @param audioFile The audio file to load and separate.
     * @return True if loading and separation succeeded, false otherwise.
     */
    bool loadAndSeparate(const juce::File& audioFile);
    
    /**
     * @brief Gets the number of available stems after separation.
     * @return The number of separated stems (0 if not separated yet).
     */
    int getNumberOfStems() const;
    
    /**
     * @brief Gets a specific stem's audio buffer by index.
     * @param stemIndex The index of the stem to retrieve.
     * @return The audio buffer for the specified stem.
     */
    juce::AudioBuffer<float> getStemBuffer(int stemIndex) const;
    
    /**
     * @brief Gets the sample rate of a specific stem.
     * @param stemIndex The index of the stem.
     * @return The sample rate of the stem in Hz.
     */
    double getStemSampleRate(int stemIndex) const;
    
    /**
     * @brief Replaces a stem buffer with new audio content.
     * @param stemIndex The index of the stem to replace.
     * @param newBuffer The new audio buffer to use.
     * @return True if the replacement succeeded, false otherwise.
     */
    bool replaceStemBuffer(int stemIndex, const juce::AudioBuffer<float>& newBuffer);

private:
    /**
     * @brief Preprocesses the input JUCE audio buffer into the format expected by the ONNX model.
     * @param inputBuffer The input audio buffer.
     * @return A vector of Ort::Value containing the preprocessed input tensors.
     */
    std::vector<Ort::Value> preprocess(const juce::AudioBuffer<float>& inputBuffer);

    /**
     * @brief Runs inference on the preprocessed input tensors using the ONNX session.
     * @param inputTensors The preprocessed input tensors.
     * @return A vector of Ort::Value containing the output tensors from the model.
     */
    std::vector<Ort::Value> runInference(std::vector<Ort::Value>& inputTensors);

    /**
     * @brief Postprocesses the output tensors from the model into JUCE audio buffers for each source.
     * @param outputTensors The output tensors from the model.
     * @return A map of source names to separated audio buffers.
     */
    std::map<std::string, juce::AudioBuffer<float>> postprocess(std::vector<Ort::Value>& outputTensors);

    // Member variables
    ONNXModelLoader& loader; // Reference to the model loader
    std::unique_ptr<Ort::Session> session; // ONNX inference session
    Ort::AllocatorWithDefaultOptions allocator; // Default allocator

    std::vector<std::string> inputNames;  // Names of the model's input nodes
    std::vector<std::string> outputNames; // Names of the model's output nodes
    std::vector<const char*> inputNamesCharPtr; // C-style char* for Ort::Session::Run
    std::vector<const char*> outputNamesCharPtr; // C-style char* for Ort::Session::Run

    std::vector<int64_t> inputDims; // Expected dimensions for input tensor (placeholder)
    std::vector<int64_t> outputDims; // Expected dimensions for output tensor(s) (placeholder)

    std::vector<std::string> sourceNames; // Names of the output sources (e.g., "drums", "bass") - needs initialization

    bool ready = false; // Flag indicating if the model loaded successfully

    // Storage for separated stems
    std::vector<juce::AudioBuffer<float>> stemBuffers;
    std::vector<std::string> stemNames;
    double stemSampleRate = 44100.0; // Default sample rate for stems
};

} // namespace ml
} // namespace undergroundBeats