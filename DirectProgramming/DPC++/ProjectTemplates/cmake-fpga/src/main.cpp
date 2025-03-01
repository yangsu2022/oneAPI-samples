//==============================================================
// Copyright © 2020, Intel Corporation. All rights reserved.
//
// SPDX-License-Identifier: MIT
// =============================================================

// located in $ONEAPI_ROOT/compiler/latest/linux/include/sycl/
#include <CL/sycl.hpp>
#include <sycl/ext/intel/fpga_extensions.hpp>
#include <cstdlib>
#include <iostream>

using namespace sycl;

// declare the kernel name as a global to reduce name mangling
class ExampleKernel;

int main() {
  // create device selector for the device of your interest
  // FPGA_EMULATOR defined in cmake-fpga/src/CMakeLists.txt
#if defined(FPGA_EMULATOR)
  // DPC++ extension: FPGA emulator selector on systems without FPGA card
  ext::intel::fpga_emulator_selector device_selector;
#else
  // DPC++ extension: FPGA selector on systems with FPGA card
  ext::intel::fpga_selector device_selector;
#endif

  // create a buffer
  constexpr int num=16;
  std::vector<int> out_data(num, -1);
  buffer A(out_data);

  // create a kernel
  queue q{device_selector };
  q.submit([&](handler& h) {
    auto out = A.get_access<access::mode::write>(h);
    h.single_task<ExampleKernel>([=]() {
      for (int index = 0; index < num; ++index) { out[index] = index; }
    });
  });

  // consume result
  auto result = A.get_access<access::mode::read>();
  for (int index = 0; index < num; ++index) {
    std::cout << result[index] << "\n";
  }

  return (EXIT_SUCCESS);
}
