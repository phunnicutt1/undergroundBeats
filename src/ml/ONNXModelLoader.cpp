#include "undergroundBeats/ml/ONNXModelLoader.h"
#include <juce_core/juce_core.h>

#ifdef USE_ONNX
#include <onnxruntime_cxx_api.h>
#endif

namespace undergroundBeats {
namespace ml {

class ONNXModelLoader::Impl {
public:
    Impl() : modelLoaded(false) {
#ifdef USE_ONNX
        // Initialize ONNX Runtime environment
        env = std::make_unique<Ort::Env>(ORT_LOGGING_LEVEL_WARNING, "ONNXModelLoader");
        sessionOptions = std::make_unique<Ort::SessionOptions>();
        
        // Set graph optimization level
        sessionOptions->SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
        
        // Enable CUDA if available
        if (Ort::GetAvailableProviders().size() > 1) {
            // Check if CUDA is available
            auto providers = Ort::GetAvailableProviders();
            if (std::find(providers.begin(), providers.end(), "CUDAExecutionProvider") != providers.end()) {
                sessionOptions->AppendExecutionProvider_CUDA(0);
                juce::Logger::writeToLog("ONNX Runtime: Using CUDA execution provider");
            }
        }
#endif
    }
    
    ~Impl() = default;
    
    bool loadModel(const std::string& modelPath) {
#ifdef USE_ONNX
        try {
            // Create session
            session = std::make_unique<Ort::Session>(*env, modelPath.c_str(), *sessionOptions);
            
            // Get input and output names
            Ort::AllocatorWithDefaultOptions allocator;
            
            // Get input count and names
            size_t numInputNodes = session->GetInputCount();
            inputNames.clear();
            
            for (size_t i = 0; i < numInputNodes; i++) {
                char* inputName = session->GetInputName(i, allocator);
                inputNames.push_back(inputName);
                allocator.Free(inputName);
                
                // Get input node info
                auto typeInfo = session->GetInputTypeInfo(i);
                auto tensorInfo = typeInfo.GetTensorTypeAndShapeInfo();
                
                // Store input shapes
                inputShapes[inputNames.back()] = tensorInfo.GetShape();
            }
            
            // Get output count and names
            size_t numOutputNodes = session->GetOutputCount();
            outputNames.clear();
            
            for (size_t i = 0; i < numOutputNodes; i++) {
                char* outputName = session->GetOutputName(i, allocator);
                outputNames.push_back(outputName);
                allocator.Free(outputName);
                
                // Get output node info
                auto typeInfo = session->GetOutputTypeInfo(i);
                auto tensorInfo = typeInfo.GetTensorTypeAndShapeInfo();
                
                // Store output shapes
                outputShapes[outputNames.back()] = tensorInfo.GetShape();
            }
            
            modelLoaded = true;
            juce::Logger::writeToLog("ONNX Runtime: Model loaded successfully - " + juce::String(modelPath));
            return true;
        }
        catch (const Ort::Exception& e) {
            juce::Logger::writeToLog("ONNX Runtime Error: " + juce::String(e.what()));
            modelLoaded = false;
            return false;
        }
#else
        juce::Logger::writeToLog("ONNX Runtime not available: Compile with USE_ONNX flag");
        modelLoaded = false;
        return false;
#endif
    }
    
    std::unordered_map<std::string, std::vector<float>> runInference(
        const std::vector<float>& inputData,
        const std::vector<int64_t>& inputShape,
        const std::vector<std::string>& requestedOutputNames) {
        
        std::unordered_map<std::string, std::vector<float>> results;
        
#ifdef USE_ONNX
        if (!modelLoaded || !session) {
            juce::Logger::writeToLog("ONNX Runtime: Model not loaded");
            return results;
        }
        
        try {
            // Create input tensor
            Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(
                OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);
            
            // Prepare input tensors
            std::vector<Ort::Value> inputTensors;
            std::vector<const char*> inputNamesCStr;
            
            if (inputNames.empty()) {
                juce::Logger::writeToLog("ONNX Runtime: No input names available");
                return results;
            }
            
            // Use the first input for now (most models have just one input)
            inputNamesCStr.push_back(inputNames[0].c_str());
            
            // Create input tensor
            Ort::Value inputTensor = Ort::Value::CreateTensor<float>(
                memoryInfo,
                const_cast<float*>(inputData.data()),
                inputData.size(),
                inputShape.data(),
                inputShape.size());
            
            inputTensors.push_back(std::move(inputTensor));
            
            // Prepare output names
            std::vector<const char*> outputNamesCStr;
            for (const auto& name : requestedOutputNames) {
                outputNamesCStr.push_back(name.c_str());
            }
            
            // Run inference
            auto outputTensors = session->Run(
                Ort::RunOptions{nullptr},
                inputNamesCStr.data(),
                inputTensors.data(),
                inputTensors.size(),
                outputNamesCStr.data(),
                outputNamesCStr.size());
            
            // Process outputs
            for (size_t i = 0; i < outputNamesCStr.size(); i++) {
                std::string outputName = requestedOutputNames[i];
                
                // Get output tensor data
                float* outputData = outputTensors[i].GetTensorMutableData<float>();
                auto tensorInfo = outputTensors[i].GetTensorTypeAndShapeInfo();
                auto outputShape = tensorInfo.GetShape();
                size_t outputSize = tensorInfo.GetElementCount();
                
                // Copy to result vector
                results[outputName] = std::vector<float>(outputData, outputData + outputSize);
            }
        }
        catch (const Ort::Exception& e) {
            juce::Logger::writeToLog("ONNX Runtime Inference Error: " + juce::String(e.what()));
        }
#else
        juce::Logger::writeToLog("ONNX Runtime not available: Compile with USE_ONNX flag");
#endif
        
        return results;
    }
    
    bool isModelLoaded() const {
        return modelLoaded;
    }
    
    std::vector<std::string> getInputNames() const {
        return inputNames;
    }
    
    std::vector<std::string> getOutputNames() const {
        return outputNames;
    }
    
    std::vector<int64_t> getInputShape(const std::string& inputName) const {
        auto it = inputShapes.find(inputName);
        if (it != inputShapes.end()) {
            return it->second;
        }
        return {};
    }
    
    std::vector<int64_t> getOutputShape(const std::string& outputName) const {
        auto it = outputShapes.find(outputName);
        if (it != outputShapes.end()) {
            return it->second;
        }
        return {};
    }
    
private:
#ifdef USE_ONNX
    std::unique_ptr<Ort::Env> env;
    std::unique_ptr<Ort::SessionOptions> sessionOptions;
    std::unique_ptr<Ort::Session> session;
#endif
    
    std::vector<std::string> inputNames;
    std::vector<std::string> outputNames;
    std::unordered_map<std::string, std::vector<int64_t>> inputShapes;
    std::unordered_map<std::string, std::vector<int64_t>> outputShapes;
    bool modelLoaded;
};

ONNXModelLoader::ONNXModelLoader() : pImpl(std::make_unique<Impl>()) {}

ONNXModelLoader::~ONNXModelLoader() = default;

bool ONNXModelLoader::loadModel(const std::string& modelPath) {
    return pImpl->loadModel(modelPath);
}

std::unordered_map<std::string, std::vector<float>> ONNXModelLoader::runInference(
    const std::vector<float>& inputData,
    const std::vector<int64_t>& inputShape,
    const std::vector<std::string>& outputNames) {
    
    return pImpl->runInference(inputData, inputShape, outputNames);
}

bool ONNXModelLoader::isModelLoaded() const {
    return pImpl->isModelLoaded();
}

std::vector<std::string> ONNXModelLoader::getInputNames() const {
    return pImpl->getInputNames();
}

std::vector<std::string> ONNXModelLoader::getOutputNames() const {
    return pImpl->getOutputNames();
}

std::vector<int64_t> ONNXModelLoader::getInputShape(const std::string& inputName) const {
    return pImpl->getInputShape(inputName);
}

std::vector<int64_t> ONNXModelLoader::getOutputShape(const std::string& outputName) const {
    return pImpl->getOutputShape(outputName);
}

} // namespace ml
} // namespace undergroundBeats
