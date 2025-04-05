#ifndef ONNX_MODEL_LOADER_H
#define ONNX_MODEL_LOADER_H

#include <onnxruntime_cxx_api.h>
#include <string>

class ONNXModelLoader
{
public:
    ONNXModelLoader(const std::string& modelPath);
    ~ONNXModelLoader();

    Ort::Session& getSession();

    bool isModelLoaded() const;

private:
    Ort::Env env;
    Ort::Session session;
    bool loaded;
};

#endif // ONNX_MODEL_LOADER_H