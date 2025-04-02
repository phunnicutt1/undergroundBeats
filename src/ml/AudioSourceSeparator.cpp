#include "undergroundBeats/ml/AudioSourceSeparator.h"
#include "undergroundBeats/ml/ONNXSourceSeparator.h"
#include <algorithm>

namespace undergroundBeats {
namespace ml {

std::unique_ptr<AudioSourceSeparator> AudioSourceSeparator::createByName(const std::string& name)
{
    // For now, we'll assume all models are in the "models" directory
    const std::string modelsDir = "models/";
    
    // Convert name to lowercase for case-insensitive comparison
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    
    // Create the appropriate separator based on the name
    if (lowerName == "spleeter" || lowerName == "spleeter4stems")
    {
        return std::make_unique<ONNXSourceSeparator>(modelsDir + "spleeter_4stems.onnx");
    }
    else if (lowerName == "demucs" || lowerName == "demucs_v4")
    {
        return std::make_unique<ONNXSourceSeparator>(modelsDir + "demucs_v4.onnx");
    }
    
    // Default model if name not recognized
    if (lowerName.empty())
    {
        return std::make_unique<ONNXSourceSeparator>(modelsDir + "spleeter_4stems.onnx");
    }
    
    // No matching model found
    return nullptr;
}

std::vector<std::string> AudioSourceSeparator::getAvailableModels()
{
    // Return a list of available models
    return {
        "Spleeter4Stems",  // Deezer's Spleeter model with 4 stems (bass, drums, vocals, other)
        "Demucs_v4"        // Facebook's Demucs v4 model
    };
}

} // namespace ml
} // namespace undergroundBeats