# Maximum Interleaving of a Loop
This FPGA tutorial explains how to use the `max_interleaving` attribute for loops.

| Optimized for                     | Description
|:---                                 |:---
| OS                                | Linux* Ubuntu* 18.04/20.04 <br> RHEL*/CentOS* 8 <br> SUSE* 15 <br> Windows* 10
| Hardware                          | Intel&reg; Programmable Acceleration Card (PAC) with Intel Arria&reg; 10 GX FPGA <br> Intel&reg; FPGA Programmable Acceleration Card (PAC) D5005 (with Intel Stratix&reg; 10 SX) <br> Intel&reg; FPGA 3rd party / custom platforms with oneAPI support <br> **Note**: Intel&reg; FPGA PAC hardware is only compatible with Ubuntu 18.04*
| Software                          | Intel&reg; oneAPI SYCL Compiler
| What you will learn               | The basic usage of the `max_interleaving` attribute <br> How the `max_interleaving` attribute affects loop resource use <br> How to apply the `max_interleaving` attribute to loops in your program
| Time to complete                  | 15 minutes

## Purpose
This tutorial demonstrates a method to reduce the area usage of inner loops that cannot realize throughput increases through interleaved execution. By default, the compiler will generate loop datapaths that enable multiple invocations of the same loop to execute simultaneously, called interleaving, in order to maximize throughput when II is greater than 1. In cases where interleaving is dynamically prohibited, e.g., due to data dependency preservation, the hardware resources used to enable interleaving are wasted. The `max_interleaving` attribute can instruct the compiler to limit allocation of these hardware resources in these cases.

### Description of the `max_interleaving` Attribute
Consider a pipelined inner loop `i` with an II > 1 contained inside a pipelined outer loop `j`. If the trip count of the `i` loop does not vary with respect to the iterations of the `j` loop, the compiler will automatically generate hardware that allows the `i` loop to interleave iterations from different invocations of itself. That is, each iteration of the outer `j` loop will contain a different invocation of the inner `i` loop. Each of these invocations of the `i` loop will issue iterations every II cycles, leaving II-1 cycles in between these iterations empty. These empty cycles can be used to issue iterations of a different invocation of `i` (corresponding to a different iteration of the `j` loop), which we called interleaving. Interleaving allows the generated design to have a high occupancy despite a high II, which improves throughput.

Although interleaving will generally improve throughput, there may be some scenarios in which users may not wish to incur the hardware cost of generating a pipelined datapath that allows interleaving. For example, in a nest of pipelined loops, pipelined iterations of an outer loop may be serialized by the compiler across an inner loop if the inner loop imposes a data dependency on the containing loop. For such scenarios, the generation of a loop datapath that supports interleaving iterations of the containing pipelined loop yields little to no benefit in throughput because the serialization to preserve the data dependency will prevent interleaving from occurring. Manually restricting or disabling the amount of interleaving on the inner loop reduces the area overhead imposed by a datapath generated to handle interleaved invocations. Users can mark a loop with the `max_interleaving` pragma to limit the number of interleaved invocations supported by the generated loop datapath.

### Example:

```
L1: for (size_t i = 0; i < kSize; i++) {
  L2: for (size_t j = 0; j < kSize; j++) {
        temp_r[j] = SomethingComplicated(temp_a[i][j], temp_r[j]);
  }
  temp_r[i] += temp_b[i];
}
```

In this loop nest, pipelined iterations of L1 are serialized across L2 to preserve the data dependency on the array variable 'temp_r'. This means that only one invocation of the `j` loop can be executing at any time, and therefore no dynamic interleaving of iterations from different invocations of the `j` loop can occur. By default, the compiler will generate a datapath that includes the capacity to run multiple interleaved iterations simultaneously. Since the data dependency prevents dynamic interleaving, the resources spent on an interleaving-capable datapath are wasted. Applying the `max_interleaving` attribute with an argument of `1` will instruct the compiler generate a datapath that restricts the interleaving capacity to a single `j` loop invocation.

### Additional Documentation
- [Explore SYCL* Through Intel&reg; FPGA Code Samples](https://software.intel.com/content/www/us/en/develop/articles/explore-dpcpp-through-intel-fpga-code-samples.html) helps you to navigate the samples and build your knowledge of FPGAs and SYCL.
- [FPGA Optimization Guide for Intel&reg; oneAPI Toolkits](https://software.intel.com/content/www/us/en/develop/documentation/oneapi-fpga-optimization-guide) helps you understand how to target FPGAs using SYCL and Intel&reg; oneAPI Toolkits.
- [Intel&reg; oneAPI Programming Guide](https://software.intel.com/en-us/oneapi-programming-guide) helps you understand target-independent, SYCL-compliant programming using Intel&reg; oneAPI Toolkits.

## Key Concepts
* The basic usage of the `max_interleaving` attribute.
* How the `max_interleaving` attribute affects loop throughput and resource use.
* How to apply the `max_interleaving` attribute to loops in your program.

## Building the `max_interleaving` Tutorial

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
 - (Linux only) Debug your GPU application with GDB for Intel&reg; oneAPI toolkits using the Generate Launch Configurations extension.

To learn more about the extensions and how to configure the oneAPI environment, see the 
[Using Visual Studio Code with Intel&reg; oneAPI Toolkits User Guide](https://software.intel.com/content/www/us/en/develop/documentation/using-vs-code-with-intel-oneapi/top.html).

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
   * Compile for simulation (fast compile time, targets simulator FPGA device):
     ```
     make fpga_sim
     ```
   * Generate the optimization report:
     ```
     make report
     ```
   * Compile for FPGA hardware (longer compile time, targets FPGA device):
     ```
     make fpga
     ```

3. (Optional) As the FPGA hardware compile may take several hours to complete, FPGA precompiled binaries (compatible with Linux* Ubuntu* 18.04) can be downloaded <a href="https://iotdk.intel.com/fpga-precompiled-binaries/latest/max_interleaving.fpga.tar.gz" download>here</a>.

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
   * Compile for simulation (fast compile time, targets simulator FPGA device):
     ```
     nmake fpga_sim
     ```
   * Generate the optimization report:
     ```
     nmake report
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
[Learn more](https://software.intel.com/content/www/us/en/develop/documentation/diagnostic-utility-user-guide/top.html).

### In Third-Party Integrated Development Environments (IDEs)

You can compile and run this tutorial in the Eclipse* IDE (in Linux*) and the Visual Studio* IDE (in Windows*). For instructions, refer to the following link: [FPGA Workflows on Third-Party IDEs for Intel&reg; oneAPI Toolkits](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-oneapi-dpcpp-fpga-workflow-on-ide.html).

## Examining the Reports
Locate `report.html` in the `max_interleaving_report.prj/reports/` directory. Open the report in Chrome*, Firefox*, Edge*, or Internet Explorer*.

In the "Loops analysis" view from the "Throughput Analysis" drop-down menu, in the Details pane for loop L1 (choose either Compute<0>.B6 or Compute<1>.B6 from the "Loop List" pane, then the click the "Source Location" link in the "Loop Analysis" pane):

Iteration executed serially across KernelCompute<0>.B8. Only a single loop iteration will execute inside this region due to data dependency on variable(s):
```
temp_r (max_interleaving.cpp: 56)
```
As described in the Example section above, generating a loop datapath that supports interleaving iterations of the containing pipelined loop in this scenario yields little to no benefit in throughput because preservation of the data dependency on `temp_r` requires that only one invocation of the inner loop be executed at a time. Adding the `max_interleaving(1)` attribute informs the compiler to not allocated hardware resources for interleaving on the inner loop, reducing the area overhead.

Referring to the generated report again, choose the "Summary" view, expand the "System Resource Utilization Summary" section in the Summary pane, and select the "Compile Estimated Kernel Resource Utilization Summary" subsection. In this subsection, you will see resource utilization estimates for two kernels: Compute<0> and Compute<1>. These correspond to two nearly-identical kernels, with the only difference being the `max_interleaving` attribute applied to the inner loop at line 58 taking either a 0 or 1 as its argument (`max_interleaving(0)` specifies unlimited interleaving, which is the same as the default `max_interleaving` limit when no attribute is set). Note that Compute<1>, which has restricted interleaving, uses fewer ALUTs, REGs, and MLABs than Compute<0>.

The area usage information can also be accessed on the main report page in the Summary pane. Scroll down to the section titled "System Resource Utilization Summary". Each kernel name ends in the `max_interleaving` attribute argument used for that kernel, e.g., `KernelCompute<1>` uses a `max_interleaving` attribute value of 1. You can verify that the number of ALUTs, REGs, and MLABs used for each kernel decreases when `max_interleaving` is limited to 1 compared to unlimited interleaving when `max_interleaving` is set to 0.

## Running the Sample

 1. Run the sample on the FPGA emulator (the kernel executes on the CPU):
     ```
     ./max_interleaving.fpga_emu     (Linux)
     max_interleaving.fpga_emu.exe   (Windows)
     ```
2. Run the sample on the FPGA simulator device:
     ```
     ./max_interleaving.fpga_sim     (Linux)
     max_interleaving.fpga_sim.exe   (Windows)
     ```
3. Run the sample on the FPGA device:
     ```
     ./max_interleaving.fpga         (Linux)
     max_interleaving.fpga.exe       (Windows)
     ```


### Example of Output
```
Max interleaving 0 kernel time : 0.019088 ms
Throughput for kernel with max_interleaving 0: 0.007 GFlops
Max interleaving 1 kernel time : 0.015 ms
Throughput for kernel with max_interleaving 1: 0.009 GFlops
PASSED: The results are correct
```

### Discussion of Results

The stdout output shows the giga-floating point operations per second (GFlops) for each kernel.

When run on the Intel&reg; PAC with Intel Arria10&reg; 10 GX FPGA hardware board, we see that the throughput remains unchanged when using `max_interleaving(0)` or `max_interleaving(1)`. However, the kernel using `max_interleaving(1)` uses fewer hardware resources, as shown in the reports.

When run on the FPGA emulator, the `max_interleaving` attribute has no effect on runtime. You may notice that the emulator achieved higher throughput than the FPGA in this example. This anomaly occurs because this trivial example uses only a tiny fraction of the compute resources available on the FPGA.

## License
Code samples are licensed under the MIT license. See
[License.txt](https://github.com/oneapi-src/oneAPI-samples/blob/master/License.txt) for details.

Third party program Licenses can be found here: [third-party-programs.txt](https://github.com/oneapi-src/oneAPI-samples/blob/master/third-party-programs.txt).
