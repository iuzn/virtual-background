/* Copyright 2018 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#include <cstdio>
#include <emscripten.h>
// #include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"
#include "tensorflow/lite/optional_debug_tools.h"
#include "mediapipe/util/tflite/operations/transpose_conv_bias.h"

// This is an example that is minimal to read a model
// from disk and perform inference. There is no data being loaded
// that is up to you to add as a user.
//
// NOTE: Do not add any dependencies to this that cannot be built with
// the minimal makefile. This example must remain trivial to build with
// the minimal build tool.
//
// Usage: tflite <tflite model>

#define TFLITE_MINIMAL_CHECK(x)                                \
    if (!(x)) {                                                \
      fprintf(stderr, "Error at %s:%d\n", __FILE__, __LINE__); \
      return 1;                                                \
    }

// int main(int argc, char* argv[]) {
//   if (argc != 2) {
//     fprintf(stderr, "minimal <tflite model>\n");
//     return 1;
//   }
//   const char* filename = argv[1];

//   // Load model
//   std::unique_ptr<tflite::FlatBufferModel> model =
//       tflite::FlatBufferModel::BuildFromFile(filename);
//   TFLITE_MINIMAL_CHECK(model != nullptr);

  // // Build the interpreter with the InterpreterBuilder.
  // // Note: all Interpreters should be built with the InterpreterBuilder,
  // // which allocates memory for the Intrepter and does various set up
  // // tasks so that the Interpreter can read the provided model.
  // tflite::ops::builtin::BuiltinOpResolver resolver;
  // resolver.AddCustom("Convolution2DTransposeBias", mediapipe::tflite_operations::RegisterConvolution2DTransposeBias());
  // tflite::InterpreterBuilder builder(*model, resolver);
  // std::unique_ptr<tflite::Interpreter> interpreter;
  // builder(&interpreter);
  // TFLITE_MINIMAL_CHECK(interpreter != nullptr);

  // // Allocate tensor buffers.
  // TFLITE_MINIMAL_CHECK(interpreter->AllocateTensors() == kTfLiteOk);
  // printf("=== Pre-invoke Interpreter State ===\n");
  // tflite::PrintInterpreterState(interpreter.get());

  // // Fill input buffers
  // // TODO(user): Insert code to fill input tensors.
  // // Note: The buffer of the input tensor with index `i` of type T can
  // // be accessed with `T* input = interpreter->typed_input_tensor<T>(i);`

  // // Run inference
  // TFLITE_MINIMAL_CHECK(interpreter->Invoke() == kTfLiteOk);
  // printf("\n\n=== Post-invoke Interpreter State ===\n");
  // tflite::PrintInterpreterState(interpreter.get());

  // // Read output buffers
  // // TODO(user): Insert getting data out code.
  // // Note: The buffer of the output tensor with index `i` of type T can
  // // be accessed with `T* output = interpreter->typed_output_tensor<T>(i);`

//   return 0;
// }

// TODO Cleanup the whole file

char modelBuffer[410000];
std::unique_ptr<tflite::Interpreter> interpreter;

extern "C" {

EMSCRIPTEN_KEEPALIVE
char* getModelBufferOffset() {
  return modelBuffer;
}

EMSCRIPTEN_KEEPALIVE
int loadModel(int bufferSize) {
  printf("[WASM] Loading model of size: %d\n", bufferSize);

  // Load model
  std::unique_ptr<tflite::FlatBufferModel> model =
    tflite::FlatBufferModel::BuildFromBuffer(modelBuffer, bufferSize);
  TFLITE_MINIMAL_CHECK(model != nullptr);

  // Build the interpreter with the InterpreterBuilder.
  // Note: all Interpreters should be built with the InterpreterBuilder,
  // which allocates memory for the Intrepter and does various set up
  // tasks so that the Interpreter can read the provided model.
  tflite::ops::builtin::BuiltinOpResolver resolver;
  resolver.AddCustom("Convolution2DTransposeBias",
    mediapipe::tflite_operations::RegisterConvolution2DTransposeBias());
  tflite::InterpreterBuilder builder(*model, resolver);
  builder(&interpreter);
  TFLITE_MINIMAL_CHECK(interpreter != nullptr);

  printf("[WASM] Inputs: %lu\n", interpreter->inputs().size());
  printf("[WASM] Input(0)\n");
  printf("[WASM]   name: %s\n", interpreter->GetInputName(0));
  TfLiteIntArray* inputDims = interpreter->input_tensor(0)->dims;
  printf("[WASM]   height: %d\n", inputDims->data[1]);
  printf("[WASM]   width: %d\n", inputDims->data[2]);
  printf("[WASM]   channels: %d\n", inputDims->data[3]);
  printf("[WASM] ---------------\n");
  printf("[WASM] Outputs: %lu\n", interpreter->outputs().size());
  printf("[WASM] Output(0)\n");
  printf("[WASM]   name: %s\n", interpreter->GetOutputName(0));
  TfLiteIntArray* outputDims = interpreter->output_tensor(0)->dims;
  printf("[WASM]   height: %d\n", outputDims->data[1]);
  printf("[WASM]   width: %d\n", outputDims->data[2]);
  printf("[WASM]   channels: %d\n", outputDims->data[3]);

  // Allocate tensor buffers.
  TFLITE_MINIMAL_CHECK(interpreter->AllocateTensors() == kTfLiteOk);

  return 0;
}

EMSCRIPTEN_KEEPALIVE
int runInference() {
  TFLITE_MINIMAL_CHECK(interpreter->Invoke() == kTfLiteOk);
  return 0;
}

}