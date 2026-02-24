# Triton Inference Server Deployment Example

This folder contains example scripts and configuration for deploying your NLP model with NVIDIA Triton Inference Server.

## Steps
1. Export your trained model from `python_nlp/` (e.g., as ONNX or TensorRT).
2. Place the exported model in the `models/` directory (create if needed).
3. Use the provided `triton_config.pbtxt` as a template for your model configuration.
4. Start Triton Inference Server with:
   ```
   tritonserver --model-repository=./models
   ```

## References
- https://github.com/triton-inference-server/server
- https://docs.nvidia.com/deeplearning/triton-inference-server
