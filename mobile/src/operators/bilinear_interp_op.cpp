/* Copyright (c) 2018 PaddlePaddle Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#ifdef BILINEAR_INTERP_OP

#include "operators/bilinear_interp_op.h"
#include <vector>

namespace paddle_mobile {
namespace operators {
template <typename DeviceType, typename T>
void BilinearOp<DeviceType, T>::InferShape() const {
  PADDLE_MOBILE_ENFORCE(this->param_.InputX() != nullptr,
                        "Input(X) of BilinearInterOp should not be null.");
  PADDLE_MOBILE_ENFORCE(this->param_.Out() != nullptr,
                        "Output(Out) of BilinearInterOp should not be null.");

  auto dim_x = this->param_.InputX()->dims();  // NCHW format
  int out_h = this->param_.OutH();
  int out_w = this->param_.OutW();
  PADDLE_MOBILE_ENFORCE(dim_x.size() == 4, "X's dimension must be 4");
  bool ignore_scale = false;
  if (out_h > 0 && out_w > 0) {
    ignore_scale = true;
  }
  if (this->param_.InputOutPutSize() != nullptr) {
    auto out_size_dim = this->param_.InputOutPutSize()->dims();

    PADDLE_MOBILE_ENFORCE(out_size_dim.size() == 1,
                          "OutSize's dimension size must be 1");
    PADDLE_MOBILE_ENFORCE(out_size_dim[0] == 2, "OutSize's dim[0] must be 2");
  }

  if (this->param_.HasScale() && !ignore_scale) {
    const float scale = this->param_.Scale();
    DLOG << "scale_:  " << scale;
    std::vector<int64_t> dim_out({dim_x[0], dim_x[1],
                                  static_cast<int>(dim_x[2] * scale),
                                  static_cast<int>(dim_x[3] * scale)});
    this->param_.Out()->Resize(framework::make_ddim(dim_out));
    DLOG << "interp -- dim_out: " << dim_out;

  } else {
    std::vector<int64_t> dim_out({dim_x[0], dim_x[1], out_h, out_w});
    this->param_.Out()->Resize(framework::make_ddim(dim_out));
    DLOG << "interp -- dim_out: " << dim_out;
  }
}

}  // namespace operators
}  // namespace paddle_mobile

namespace ops = paddle_mobile::operators;
#ifdef PADDLE_MOBILE_CPU
REGISTER_OPERATOR_CPU(bilinear_interp, ops::BilinearOp);
#endif

#if PADDLE_MOBILE_CL
REGISTER_OPERATOR_CL(bilinear_interp, ops::BilinearOp)
#endif

#ifdef PADDLE_MOBILE_FPGA
#endif

#endif
