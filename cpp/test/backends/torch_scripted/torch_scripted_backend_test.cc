#include <fmt/format.h>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>

#include "src/backends/torch_scripted/torch_scripted_backend.hh"
#include "src/utils/message.hh"

namespace torchserve {
  class TorchScriptedBackendTest : public ::testing::Test {
    protected:
    void SetUp() {
      backend_ = std::make_shared<torchserve::torchscripted::Backend>();

      load_model_request_base_ = std::make_shared<torchserve::LoadModelRequest>(
        "test/resources/torchscript_model/mnist/base_handler",
        "mnist_scripted_v2",
        -1,
        "",
        "",
        1,
        false
      );

      load_model_request_mnist_ = std::make_shared<torchserve::LoadModelRequest>(
        "test/resources/torchscript_model/mnist/mnist_handler",
        "mnist_scripted_v2",
        -1,
        "",
        "",
        1,
        false
      );
    };
    uint8_t batch_size_ = 2;
    std::shared_ptr<torchserve::Backend> backend_;
    std::shared_ptr<torchserve::LoadModelRequest> load_model_request_base_;
    std::shared_ptr<torchserve::LoadModelRequest> load_model_request_mnist_;
  };

  TEST_F(TorchScriptedBackendTest, TestPredictBase) {
    backend_->Initialize("test/resources/torchscript_model/mnist/base_handler");
    auto result = backend_->LoadModel(std::move(load_model_request_base_));
    ASSERT_EQ(result->code, 200);

    std::ifstream input("test/resources/torchscript_model/mnist/0_png.pt", std::ios::in | std::ios::binary);
    std::vector<char> image(
      (std::istreambuf_iterator<char>(input)),
      (std::istreambuf_iterator<char>()));
    input.close();

    auto inference_request_batch = std::make_shared<torchserve::InferenceRequestBatch>();
    for (uint8_t i = 0; i < batch_size_; i++) {
      torchserve::InferenceRequest inference_request;
      inference_request.request_id = fmt::format("mnist_ts_{}", i);
      inference_request.headers[torchserve::PayloadType::kHEADER_NAME_DATA_TYPE] = 
        torchserve::PayloadType::kDATA_TYPE_BYTES;
      inference_request.parameters[torchserve::PayloadType::kPARAMETER_NAME_DATA] = image;

      (*inference_request_batch).emplace_back(inference_request);
    }
    
    auto inference_response_batch = backend_->GetModelInstance()->Predict(inference_request_batch);
    for (const auto& kv : *inference_response_batch) {
      ASSERT_EQ(kv.second->code, 200);
    }
  }

  TEST_F(TorchScriptedBackendTest, TestPredictMnist) {
    backend_->Initialize("test/resources/torchscript_model/mnist/mnist_handler");
    auto result = backend_->LoadModel(std::move(load_model_request_mnist_));
    ASSERT_EQ(result->code, 200);

    std::ifstream input("test/resources/torchscript_model/mnist/0_png.pt", std::ios::in | std::ios::binary);
    std::vector<char> image(
      (std::istreambuf_iterator<char>(input)),
      (std::istreambuf_iterator<char>()));
    input.close();

    auto inference_request_batch = std::make_shared<torchserve::InferenceRequestBatch>();
    for (uint8_t i = 0; i < batch_size_; i++) {
      torchserve::InferenceRequest inference_request;
      inference_request.request_id = fmt::format("mnist_ts_{}", i);
      inference_request.headers[torchserve::PayloadType::kHEADER_NAME_DATA_TYPE] = 
        torchserve::PayloadType::kDATA_TYPE_BYTES;
      inference_request.parameters[torchserve::PayloadType::kPARAMETER_NAME_DATA] = image;

      (*inference_request_batch).emplace_back(inference_request);
    }
    auto inference_response_batch = backend_->GetModelInstance()->Predict(inference_request_batch);
    for (const auto& kv : *inference_response_batch) {
      ASSERT_EQ(kv.second->code, 200);
    }
  }
} //namespace