# Python Triton gRPC Client Sample for NLP Core
# Author: 1proprogrammerchant
import tritonclient.grpc as grpcclient
import numpy as np

# Example input
identity_key = "e1b2c3d4e5f6a7b8c9d0e1f2a3b4c5d6"
from_surface = "the man"
to_surfaces = np.array(["the voice", "the silence", "", "", "", "", "", ""], dtype=object)
layer = np.array([1], dtype=np.int32)

# Connect to Triton
client = grpcclient.InferenceServerClient(url="localhost:8001")
inputs = [
    grpcclient.InferInput("IDENTITY_KEY", [1], "BYTES"),
    grpcclient.InferInput("FROM_SURFACE", [1], "BYTES"),
    grpcclient.InferInput("TO_SURFACES", [8], "BYTES"),
    grpcclient.InferInput("LAYER", [1], "INT32"),
]
inputs[0].set_data_from_numpy(np.array([identity_key], dtype=object))
inputs[1].set_data_from_numpy(np.array([from_surface], dtype=object))
inputs[2].set_data_from_numpy(to_surfaces)
inputs[3].set_data_from_numpy(layer)

# Run inference
results = client.infer(model_name="nlp_core_python", inputs=inputs)
proposals = results.as_numpy("PROPOSALS")
print("Received proposals:")
for p in proposals[0]:
    if p:
        print(p.decode())
