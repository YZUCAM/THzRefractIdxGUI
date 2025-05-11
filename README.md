<div align="center">
<img src="https://raw.githubusercontent.com/YZUCAM/THzRefractIdxGUI/main/docsrc/example1.png"><br><br>
</div>


# Extract Complex Refractive Index from THz Spectroscopy (C++ version)
## About the project
The transmission THz spectrum contains rich information about the complex refractive index of the studied materials that THz pulse passed through. With known the thickness of the studied materials, one can successfully extracted the complex refractive index at the THz regime. This handy program is designed to serve this purpose. Once the THz spectrum is loaded, the stochastic gradient descent algo will extracted the complex refractive n and k as function of the THz frequency.


## Briefly working principle of the under hood GUI
The THz time-domain spectra are collected experimentally. The typical spectra requried are reference spectrum and sample spectrum. For a simple case, the reference spectrum (T0) can be just a blank THz spectrum without presented any sample in the pathline. The sample specturm (T1) is the one you load the sample in the optical path. <br><br>

The tricky part in the data preparation is how to window the raw data. Different groups have different ways to do it. But here we want to clarify and standardized it. If the T0 and the T1 pulses are close to each other (typically for thin film sample), it is safe to cut the data with same window starting and ending position for both T0 and T1. However, the T0 and the T1 pulse are far away from each other in the time axis, there will be a problem of lossing information for one of the THz trace if the data is cutted at the same window starting and ending position for both T0 and T1. To avoid this awkward situation, we independently cut the T0 and T1 data by using their own pulse peak location as a pivot. For example, one can choose data range from the pivot left 100 point to pivot right 100. In the end, the data size of T0 and T1 can be maintained. With the same scanning step for T0 and T1, Fourier transform to T0 and T1 can yeild same frequency points for two THz spectra, denoted as fty0 and fty1.



## Getting Started
The program is based on ImGui opensource c++ UI package. We here appreciate the effort of ImGui team made a fantasitic UI develop tool. Backend requires pytorch libtorch support.

### Prerequisites
1. libtorch
2. fast-cpp-csv-parser

### Installation
In the CMake file, configurated your libtorch lib and fast-cpp-csv-parser csv.h path in accordinaly section.<br>
run `cmake -S . -B build` after generated a makefile <br>
run `camke --build build`<br>

## Usage
**Mode 1:**<br>
The sample is freestanding. Select Mode to 1, Loading reference and sample spectrum, input the sample thickness. Click the extraction button. Check the fitting curve of transmission and phase plot. Measurement curve should overlap with fitting curve, indicating the extraction is accurate. <br>

**Mode 2:**<br>
The sample is deposited on the substrate. Select Mode to 2, Loading reference and sample spectrum, input both the sample and the substrate thickness. Click the extraction button. The program will first extracted the substrate complex refractive index in the background. Using the substrate complex refractive index, the program will extracte sample complex refractive index. Check the fitting curve of transmission and phase plot. Measurement curve should overlap with fitting curve, indicating the extraction is accurate. <br>

**Fabry–Pérot Effect**<br>
If the sample is thin enough, you may should consider Fabry–Pérot Effect. Ticking the Fabry–Pérot check box, the model will include the Fabry–Pérot Effect to the simulation.

**Data Format**<br>
The required data format is csv format.<br> 
For single THz trace, two column is required with first column is time, unit is ps, the second column is THz transmission .<br>
For dataset, six column is required. 
- Col1 and col2 is time and intensity for blank reference trance. 
- Col3 and col4 is time and intensity for substrate. 
- Col5 and col6 is time and intensity for sample on the substrate.


**Global Phase Delay**<br>
Those time delays are easily identified in the time domain spectrum. They are just those pulse delay. 
- Sub: the time difference between substrate trace and blank reference trace (used in mode 2).
- Sample: the time difference between sample trace and blank reference trace (used in mode 1).
- Sample+Sub: the time difference between sample on the substrate and the substrate trace (used in mode 2)

