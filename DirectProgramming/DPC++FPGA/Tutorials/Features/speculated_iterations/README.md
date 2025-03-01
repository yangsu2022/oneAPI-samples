
# Speculated Iterations of a Loop
This FPGA tutorial demonstrates applying the `speculated_iterations` attribute to a loop in a task kernel to enable more efficient loop pipelining.

| Optimized for                     | Description
|:---                               |:---
| OS                                | Linux* Ubuntu* 18.04/20.04 <br> RHEL*/CentOS* 8 <br> SUSE* 15 <br> Windows* 10
| Hardware                          | Intel&reg; Programmable Acceleration Card (PAC) with Intel Arria&reg; 10 GX FPGA <br> Intel&reg; FPGA Programmable Acceleration Card (PAC) D5005 (with Intel Stratix&reg; 10 SX) <br> Intel&reg; FPGA 3rd party / custom platforms with oneAPI support <br> *__Note__: Intel&reg; FPGA PAC hardware is only compatible with Ubuntu 18.04*
| Software                          | Intel&reg; oneAPI SYCL Compiler <br> Intel&reg; FPGA Add-On for oneAPI Base Toolkit
| What you will learn               |  What the `speculated_iterations` attribute does <br> How to apply the `speculated_iterations` attribute to loops in your program <br> How to determine the optimal number of speculated iterations
| Time to complete                  | 15 minutes

## Purpose
Loop speculation is an advanced loop pipelining optimization technique. It enables loop iterations to be initiated before determining whether they should have been initiated. "Speculated iterations" are those iterations that launch before the exit condition computation has completed. This is beneficial when the computation of the exit condition is preventing effective loop pipelining.

The `speculated_iterations` attribute is a loop attribute that enables you to directly control the number of speculated iterations for a loop.  The attribute  `[[intelfpga::speculated_iterations(N)]]` takes an integer argument `N` to specify the permissible number of iterations to speculate.

### Simple example
```
  [[intelfpga::speculated_iterations(1)]]
  while (sycl::log10(x) < N) {
      x += 1;
  }
  dst[0] = x;
```
The loop in this example will have one speculated iteration.
### Operations with side effects
When launching speculated iterations, operations with side-effects (such as stores to memory) must be predicated by the exit condition to ensure functional correctness. For this reason, operations with side-effects must be scheduled until after the exit condition has been computed.

### Optimizing the number of speculated iterations
Loop speculation is beneficial when the computation of the loop exit condition is the bottleneck preventing the compiler from achieving a smaller initiation interval (II). In such instances, increasing the number of speculated iterations often improves the II.  Note that this may also uncover additional bottlenecks preventing the further optimization of the loop.

However, adding speculated iterations is not without cost. They introduce overhead in nested loops, reducing overall loop occupancy. Consider the code snippet below:
```c++
for (size_t i = 0; i < kMany; ++i) {
  // The compiler may automatically infer speculated iterations
  for (size_t j = 0; complex_exit_condition(j); ++j) {
    output[i,j] = some_function(input[i,j]);
  }
}
```
The *i+1*th invocation of the inner loop cannot begin until all real and speculated iterations of its *i*th invocation have completed. This overhead is negligible if the number of speculated iterations is much less than the number of real iterations. However, when the inner loop's trip count is small on average, the overhead becomes non-negligible, and the speculated iterations can become detrimental to throughput. In such circumstances, the `speculated_iterations` attribute can be used to *reduce* the number of speculated iterations chosen by the compiler's heuristics.

In both increasing and decreasing cases, some experimentation is usually necessary. Choosing too few speculated iterations could increase the II because multiple cycles are required to evaluate the exit condition. Choosing too many speculated iterations creates unneeded "dead space" between sequential invocations of an inner loop.

### Tutorial example
In the tutorial design's kernel, the loop's exit condition involves a logarithm and a compare operation. This complex exit condition prevents the loop from achieving ```II=1```.

The design enqueues variants of the kernel with 0, 10, and 27 speculated iterations, respectively, to demonstrate the effect of the `speculated_iterations` attribute on the Intel&reg; PAC with Intel Arria&reg; 10 GX FPGA. Different numbers are chosen for the Intel&reg; FPGA PAC D5005 (with Intel Stratix&reg; 10 SX) accordingly.

### Additional Documentation
- [Explore SYCL* Through Intel&reg; FPGA Code Samples](https://software.intel.com/content/www/us/en/develop/articles/explore-dpcpp-through-intel-fpga-code-samples.html) helps you to navigate the samples and build your knowledge of FPGAs and SYCL.
- [FPGA Optimization Guide for Intel&reg; oneAPI Toolkits](https://software.intel.com/content/www/us/en/develop/documentation/oneapi-fpga-optimization-guide) helps you understand how to target FPGAs using SYCL and Intel&reg; oneAPI Toolkits.
- [Intel&reg; oneAPI Programming Guide](https://software.intel.com/en-us/oneapi-programming-guide) helps you understand target-independent, SYCL-compliant programming using Intel&reg; oneAPI Toolkits.

## Key Concepts
* Description of the `speculated_iterations` attribute.
* How to apply the `speculated_iterations` attribute to loops in your program.
* Optimizing the number of speculated iterations.

## Building the `speculated_iterations` Tutorial
> **Note**: If you have not already done so, set up your CLI
> environment by sourcing  the `setvars` script located in
> the root of your oneAPI installation.
>
> Linux*:
> - For system wide installations: `. /opt/intel/oneapi/setvars.sh`
> - For private installations: `. ~/intel/oneapi/setvars.sh`
>
> Windows*:
> - `C:\Program Files(x86)\Intel\oneAPI\setvars.bat`
>
>For more information on environment variables, see **Use the setvars Script** for [Linux or macOS](https://www.intel.com/content/www/us/en/develop/documentation/oneapi-programming-guide/top/oneapi-development-environment-setup/use-the-setvars-script-with-linux-or-macos.html), or [Windows](https://www.intel.com/content/www/us/en/develop/documentation/oneapi-programming-guide/top/oneapi-development-environment-setup/use-the-setvars-script-with-windows.html).



### Running Samples in Intel&reg; DevCloud
If running a sample in the Intel&reg; DevCloud, remember that you must specify the type of compute node and whether to run in batch or interactive mode. Compiles to FPGA are only supported on fpga_compile nodes. Executing programs on FPGA hardware is only supported on fpga_runtime nodes of the appropriate type, such as fpga_runtime:arria10 or fpga_runtime:stratix10.  Neither compiling nor executing programs on FPGA hardware are supported on the login nodes. For more information, see the Intel&reg; oneAPI Base Toolkit Get Started Guide ([https://devcloud.intel.com/oneapi/documentation/base-toolkit/](https://devcloud.intel.com/oneapi/documentation/base-toolkit/)).

When compiling for FPGA hardware, it is recommended to increase the job timeout to 12h.


### Using Visual Studio Code*  (Optional)

You can use Visual Studio Code (VS Code) extensions to set your environment, create launch configurations,
and browse and download samples.

The basic steps to build and run a sample using VS Code include:
 - Download a sample using the extension **Code Sample Browser for Intel&reg; oneAPI Toolkits**.
 - Configure the oneAPI environment with the extension **Environment Configurator for Intel&reg; oneAPI Toolkits**.
 - Open a Terminal in VS Code (**Terminal>New Terminal**).
 - Run the sample in the VS Code terminal using the instructions below.
 - (Linux only) Debug your GPU application with GDB for Intel&reg; oneAPI toolkits using the **Generate Launch Configurations** extension.

To learn more about the extensions, see the
[Using Visual Studio Code with Intel&reg; oneAPI Toolkits User Guide](https://www.intel.com/content/www/us/en/develop/documentation/using-vs-code-with-intel-oneapi/top.html).


### On a Linux* System

1. Generate the `Makefile` by running `cmake`.
     ```
   mkdir build
   cd build
   ```
   To compile for the Intel&reg; PAC with Intel Arria&reg; 10 GX FPGA, run `cmake` using the command:
    ```
    cmake ..
   ```
   Alternatively, to compile for the Intel&reg; FPGA PAC D5005 (with Intel Stratix&reg; 10 SX), run `cmake` using the command:

   ```
   cmake .. -DFPGA_DEVICE=intel_s10sx_pac:pac_s10
   ```
   You can also compile for a custom FPGA platform. Ensure that the board support package is installed on your system. Then run `cmake` using the command:
   ```
   cmake .. -DFPGA_DEVICE=<board-support-package>:<board-variant>
   ```

2. Compile the design through the generated `Makefile`. The following build targets are provided, matching the recommended development flow:

   * Compile for emulation (fast compile time, targets emulated FPGA device):
      ```
      make fpga_emu
      ```
   * Generate the optimization report:
     ```
     make report
     ```
   * Compile for simulation (fast compile time, targets simulated FPGA device, reduced data size):
     ```
     make fpga_sim
     ```
   * Compile for FPGA hardware (longer compile time, targets FPGA device):
     ```
     make fpga
     ```
3. (Optional) As the above hardware compile may take several hours to complete, FPGA precompiled binaries (compatible with Linux* Ubuntu* 18.04) can be downloaded <a href="https://iotdk.intel.com/fpga-precompiled-binaries/latest/speculated_iterations.fpga.tar.gz" download>here</a>.

### On a Windows* System

1. Generate the `Makefile` by running `cmake`.
     ```
   mkdir build
   cd build
   ```
   To compile for the Intel&reg; PAC with Intel Arria&reg; 10 GX FPGA, run `cmake` using the command:
    ```
    cmake -G "NMake Makefiles" ..
   ```
   Alternatively, to compile for the Intel&reg; FPGA PAC D5005 (with Intel Stratix&reg; 10 SX), run `cmake` using the command:

   ```
   cmake -G "NMake Makefiles" .. -DFPGA_DEVICE=intel_s10sx_pac:pac_s10
   ```
   You can also compile for a custom FPGA platform. Ensure that the board support package is installed on your system. Then run `cmake` using the command:
   ```
   cmake -G "NMake Makefiles" .. -DFPGA_DEVICE=<board-support-package>:<board-variant>
   ```

2. Compile the design through the generated `Makefile`. The following build targets are provided, matching the recommended development flow:

   * Compile for emulation (fast compile time, targets emulated FPGA device):
     ```
     nmake fpga_emu
     ```
   * Generate the optimization report:
     ```
     nmake report
     ```
   * Compile for simulation (fast compile time, targets simulated FPGA device, reduced data size:
     ```
     nmake fpga_sim
     ```
   * Compile for FPGA hardware (longer compile time, targets FPGA device):
     ```
     nmake fpga
     ```

> **Note**: The Intel&reg; PAC with Intel Arria&reg; 10 GX FPGA and Intel&reg; FPGA PAC D5005 (with Intel Stratix&reg; 10 SX) do not support Windows*. Compiling to FPGA hardware on Windows* requires a third-party or custom Board Support Package (BSP) with Windows* support.

> **Note**: If you encounter any issues with long paths when compiling under Windows*, you may have to create your ‘build’ directory in a shorter path, for example c:\samples\build.  You can then run cmake from that directory, and provide cmake with the full path to your sample directory.

### Troubleshooting
If an error occurs, you can get more details by running `make` with
the `VERBOSE=1` argument:
``make VERBOSE=1``
For more comprehensive troubleshooting, use the Diagnostics Utility for
Intel&reg; oneAPI Toolkits, which provides system checks to find missing
dependencies and permissions errors.
[Learn more](https://www.intel.com/content/www/us/en/develop/documentation/diagnostic-utility-user-guide/top.html).

### In Third-Party Integrated Development Environments (IDEs)

You can compile and run this tutorial in the Eclipse* IDE (in Linux*) and the Visual Studio* IDE (in Windows*). For instructions, refer to the following link: [FPGA Workflows on Third-Party IDEs for Intel&reg; oneAPI Toolkits](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-oneapi-dpcpp-fpga-workflow-on-ide.html).

## Examining the Reports
Locate `report.html` in the `speculated_iterations_report.prj/reports/` directory. Open the report in Chrome*, Firefox*, Edge*, or Internet Explorer*.

In the "Loop Analysis" section of the report, check the II of the loop in each kernel version. Use the kernel with 0 speculated iteration as a base version, check its loop II as a hint for the ideal number for speculated iterations. The information shown below is from compiling on the Intel&reg; PAC with Intel Arria&reg; 10 GX FPGA.

* When the number of  `speculated iterations` is set to 0, loop II is 27.
* Setting the `speculated iterations` to 27 yielded an II of 1.
* Setting the `speculated iterations` to an intermediate value of 10 results in an II of 3.


These results make sense when you recall that the loop exit computation has a latency of 27 cycles (suggested by looking at loop II with 0 speculation). With no speculation, a new iteration can only be launched every 27 cycles. Increasing the speculation to 27 enables a new iteration to launch every cycle. Reducing the speculation to 10 results in an II of 3 because 10 speculated iterations multiplied by 3 cycles between iterations leave 30 cycles in which to compute the exit condition, sufficient to cover the 27-cycle exit condition.

## Running the Sample

 1. Run the sample on the FPGA emulator (the kernel executes on the CPU):
     ```bash
     ./speculated_iterations.fpga_emu     (Linux)
     speculated_iterations.fpga_emu.exe   (Windows)
     ```

 2. Run the sample on the FPGA simulator device:
     ```bash
     ./speculated_iterations.fpga_sim     (Linux)
     speculated_iterations.fpga_sim.exe   (Windows)
     ```

 3. Run the sample on the FPGA device:
     ```bash
     ./speculated_iterations.fpga         (Linux)
     speculated_iterations.fpga.exe       (Windows)
     ```

### Example of Output
```
Speculated Iterations: 0 -- kernel time: 8564.98 ms
Performance for kernel with 0 speculated iterations: 11675 MFLOPs
Speculated Iterations: 10 -- kernel time: 952 ms
Performance for kernel with 10 speculated iterations: 105076 MFLOPs
Speculated Iterations: 27 -- kernel time: 317 ms
Performance for kernel with 27 speculated iterations: 315181 MFLOPs
PASSED: The results are correct
```
The execution time and throughput for each kernel are displayed.

> **Note**: The performance difference will be apparent only when running on FPGA hardware. The emulator, while useful for verifying functionality, will generally not reflect differences in performance.

## License
Code samples are licensed under the MIT license. See
[License.txt](https://github.com/oneapi-src/oneAPI-samples/blob/master/License.txt) for details.

Third party program Licenses can be found here: [third-party-programs.txt](https://github.com/oneapi-src/oneAPI-samples/blob/master/third-party-programs.txt).
