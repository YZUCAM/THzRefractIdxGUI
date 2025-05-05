// Based on Dear ImGui base stone. Tailed for small lab gui.
//                                              By Dr. Yi Zhu
//                                                 23-Apr-2025

// third version change backend function
// TODO add FP mode selection button (check box)
// change second plot to linear plot
// second plot set as amplitude fitting data
// fourth plot set as phase fitting data.
// fix the second plot x axis unit issue
// use only simple model with known thickness.


#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "OGCourier.h"
#include <stdio.h>
#include <GLFW/glfw3.h> 
#include <implot.h>
#include <cmath>
#include <atomic>
#include <thread>
#include "data_logger.h"
#include "imgui_filedialog.h"
#include "global_logger.h"      // ---> store all global variables

// define global variables

std::atomic<bool> isTraining = false;
std::atomic<float> progress = 0;
std::atomic<bool> stopFlag = false;
std::thread trainingThread;
std::thread thicknessThread;


// Setup Datalogger
const size_t maxLines = 50;  // Display the last 50 lines
const size_t maxLogSize = 50; // Limit log size to 50 Byte
DataLogger logger(maxLogSize);    //log_buffer size 

// variables for backend THz calculation
// std::vector<double> pos;
// int skip_row = 0;

// global variable used in GUI
bool ref_selected = false;
bool chip_selected = false;
bool sam_selected = false;
bool sam_sub_selected = false;
bool first_load_plot = false;
bool roi_selector = false;
bool know_FP = false;

std::string selected_file_type = "";
std::string point = "";

float induced_phase_delay;

// float progress = 0.0;


static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}



int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // GL 3.0 + GLSL 130 (graphic rendering library)
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window with graphics context (1280 * 720)
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Complex Refractive Index Extractor", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwSetWindowAspectRatio(window, 16, 9);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui Style
    // ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();
    

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // sub window config
    int right_window_width = 400;   // 384
    

    // Load Fonts
    ImFontConfig font_cfg;
    font_cfg.FontDataOwnedByAtlas = false;
    font_cfg.OversampleH = 3;
    font_cfg.OversampleV = 1;
    font_cfg.PixelSnapH = true;
    io.Fonts->AddFontFromMemoryTTF(OGCourier_ttf, OGCourier_ttf_len, 18.0f, &font_cfg);

    // Our state
    // bool show_demo_window = true;
    // bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImVec4 buttonColor = ImGui::GetStyle().Colors[ImGuiCol_Button];
    ImVec4 hoveredColor = ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered];
    
    // state flag
    


    // GUI variables
    int mode = 0;
    int TimeFreqSelect = 0;

    // char thick_from[128] = "";
    // char thick_to[128] = "";
    // char thick_step[128] = "";
    char chip_gp[128] = "0";
    char sam_gp[128] = "0";
    char sam_sub_gp[128] = "0";
    char phase_delay[128] = "0";
    char ROI_from[128] = "";
    char ROI_to[128] = "";
    char learning_rate[128] = "";
    char iteration_num[128] = "";
    // char L[128] = "";
    
    char Thickness[128] = "0";
    




    // main program UI loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        int windowWidth, windowHeight;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);   // get glfwwindow size for automatically resize sub UI under this window


        // First Panel in the Left
        ImGui::SetNextWindowPos(ImVec2(0,0));
        ImGui::SetNextWindowSize(ImVec2((int)((windowWidth - right_window_width)/2), (int)windowHeight/2));
        ImGui::Begin("Plot1", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);     // Create a window called "Hello, world!" and append into it.
        ImVec2 plot1_size = ImGui::GetContentRegionAvail(); 
        if (first_load_plot){ImPlot::SetNextAxesToFit();}
        if (TimeFreqSelect == 0)
        {
            if (ImPlot::BeginPlot("Time Domain THz Spectrum", plot1_size)) 
            {
                ImPlot::SetupAxes("Time (ps)","E (a.u.)");
                ImPlot::SetupLegend(ImPlotLocation_NorthEast);
                if (mode == 0)
                {
                    ImPlot::PlotLine("Ref", spectrum_container["ref"].times.data(), spectrum_container["ref"].Tm.data(), spectrum_container["ref"].Tm.size());
                    ImPlot::PlotLine("Sam", spectrum_container["sam"].times.data(), spectrum_container["sam"].Tm.data(), spectrum_container["sam"].Tm.size());
                }
                else
                {
                    ImPlot::PlotLine("Ref", spectrum_container["ref"].times.data(), spectrum_container["ref"].Tm.data(), spectrum_container["ref"].Tm.size());
                    ImPlot::PlotLine("Sub", spectrum_container["sub"].times.data(), spectrum_container["sub"].Tm.data(), spectrum_container["sub"].Tm.size());
                    ImPlot::PlotLine("Sam+Sub", spectrum_container["sam_sub"].times.data(), spectrum_container["sam_sub"].Tm.data(), spectrum_container["sam_sub"].Tm.size());
                }
                // if (!spectrum_container["ref"].times.empty()){}
                ImPlot::EndPlot();
            }    
    
        }
        else if (TimeFreqSelect == 1)
        {
            if (ImPlot::BeginPlot("Frequency Domain THz Spectrum", plot1_size)) 
            {
                ImPlot::SetupAxes("Frequency (THz)","E (a.u.)");
                ImPlot::SetupLegend(ImPlotLocation_NorthEast);
                ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10);
                if (mode == 0)
                {
                    ImPlot::PlotLine("Ref", spectrum_container["ref"].freqsTHz.data(), spectrum_container["ref"].fty_abs.data(), spectrum_container["ref"].fty_abs.size());
                    ImPlot::PlotLine("Sam", spectrum_container["sam"].freqsTHz.data(), spectrum_container["sam"].fty_abs.data(), spectrum_container["sam"].fty_abs.size());
                }
                else
                {
                    ImPlot::PlotLine("Ref", spectrum_container["ref"].freqsTHz.data(), spectrum_container["ref"].fty_abs.data(), spectrum_container["ref"].fty_abs.size());
                    ImPlot::PlotLine("Sub", spectrum_container["sub"].freqsTHz.data(), spectrum_container["sub"].fty_abs.data(), spectrum_container["sub"].fty_abs.size());
                    ImPlot::PlotLine("Sam+Sub", spectrum_container["sam_sub"].freqsTHz.data(), spectrum_container["sam_sub"].fty_abs.data(), spectrum_container["sam_sub"].fty_abs.size());
                }
                ImPlot::EndPlot();
            }    
    
        }
        // ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();

    

        // Second Panel in the Left
        ImGui::SetNextWindowPos(ImVec2((int)(windowWidth - right_window_width)/2, 0));
        ImGui::SetNextWindowSize(ImVec2((int)(windowWidth - right_window_width)/2,(int)windowHeight/2));
        ImGui::Begin("Plot2", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);                
        ImVec2 plot2_size = ImGui::GetContentRegionAvail(); 
        if (first_load_plot){ImPlot::SetNextAxesToFit();}
        if (ImPlot::BeginPlot("Complex Tranmission", plot2_size)) 
        {
            ImPlot::SetupAxes("Frequency (THz)","E (a.u.)");
            ImPlot::SetupLegend(ImPlotLocation_NorthEast);
            // ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10);
            if (roi_selector)
            {
                if (mode == 0)
                {
                    
                    ImPlot::PlotLine("Tm_sam", ROI_data.roi_freqsTHz.data(), ROI_data.roi_Tm1_abs.data(), ROI_data.roi_Tm1_abs.size());
                }
                else
                {
                    ImPlot::PlotLine("Tm_sub", ROI_data.roi_freqsTHz.data(), ROI_data.roi_Tm0_abs.data(), ROI_data.roi_Tm0_abs.size());
                    ImPlot::PlotLine("Tm_sam_sub", ROI_data.roi_freqsTHz.data(), ROI_data.roi_Tm2_abs.data(), ROI_data.roi_Tm2_abs.size());
                }
            }
            else
            {
                if (mode == 0)
                {
                    ImPlot::PlotLine("Tm_sam", spectrum_container["ref"].freqsTHz.data(), c_t_dataset.Tm1_abs.data(), c_t_dataset.Tm1_abs.size());
                }
                else
                {
                    ImPlot::PlotLine("Tm_sub", spectrum_container["ref"].freqsTHz.data(), c_t_dataset.Tm0_abs.data(), c_t_dataset.Tm0_abs.size());
                    ImPlot::PlotLine("Tm_sam_sub", spectrum_container["ref"].freqsTHz.data(), c_t_dataset.Tm2_abs.data(), c_t_dataset.Tm2_abs.size());
                }
            }
            ImPlot::EndPlot();
        }    
        ImGui::End();


        
        // Third Panel in the Left
        ImGui::SetNextWindowPos(ImVec2(0, (int)windowHeight/2));
        ImGui::SetNextWindowSize(ImVec2((int)(windowWidth - right_window_width)/2,(int)windowHeight/2));
        ImGui::Begin("Plot3", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);                
        ImVec2 plot3_size = ImGui::GetContentRegionAvail(); 
        if (first_load_plot){ImPlot::SetNextAxesToFit();}
        if (ImPlot::BeginPlot("Complex Refractive Index", plot3_size)) 
        {
            ImPlot::SetupLegend(ImPlotLocation_NorthEast);
            
            // ImPlot::SetupAxis(ImAxis_X1, "Frequency (Hz)", ImPlotAxisFlags_AutoFit);
            // ImPlot::SetupAxis(ImAxis_Y1, "n", ImPlotAxisFlags_AutoFit);
            // ImPlot::SetupAxis(ImAxis_Y2, "k", ImPlotAxisFlags_AuxDefault | ImPlotAxisFlags_AutoFit);
            ImPlot::SetupAxis(ImAxis_X1, "Frequency (Hz)");
            ImPlot::SetupAxis(ImAxis_Y1, "n");
            ImPlot::SetupAxis(ImAxis_Y2, "k", ImPlotAxisFlags_AuxDefault);

            // Activate second axis (on the right)
            // ImPlot::SetupAxisLimits(ImAxis_X1, freq_min, freq_max);
            ImPlot::SetAxes(ImAxis_X1, ImAxis_Y1); // plot on primary y-axis
            ImPlot::PlotLine("n", ROI_data.roi_freqsTHz.data(), cri.n2.data(), cri.n2.size());

            ImPlot::SetAxes(ImAxis_X1, ImAxis_Y2); // plot on secondary y-axis
            ImPlot::PlotLine("k", ROI_data.roi_freqsTHz.data(), cri.k2.data(), cri.k2.size());

            // ImPlot::PlotLine("n", ROI_data.roi_freqsTHz.data(), cri.n2.data(), cri.n2.size());
            ImPlot::EndPlot();
        }         
        ImGui::End();



        // Fourth Panel in the Left
        ImGui::SetNextWindowPos(ImVec2((int)(windowWidth - right_window_width)/2, (int)windowHeight/2));
        ImGui::SetNextWindowSize(ImVec2((int)(windowWidth - right_window_width)/2,(int)windowHeight/2));
        ImGui::Begin("Plot4", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);                
        ImVec2 plot4_size = ImGui::GetContentRegionAvail(); 
        if (first_load_plot){ImPlot::SetNextAxesToFit();}
        if (ImPlot::BeginPlot("Transmission Phase", plot4_size)) 
        {
            ImPlot::SetupAxes("Frequency (Hz)", "Phase (rad)");
            ImPlot::SetupLegend(ImPlotLocation_NorthEast);
            ImPlot::PlotLine("Phi_sub", thickness_info.thickarry.data(), thickness_info.thick_error.data(), thickness_info.thick_error.size());
            // ImPlot::PlotLine("Phi_sam", thickness_info.thickarry.data(), thickness_info.thick_error.data(), thickness_info.thick_error.size());
            // ImPlot::PlotLine("Phi_sam_chip", thickness_info.thickarry.data(), thickness_info.thick_error.data(), thickness_info.thick_error.size());
            ImPlot::EndPlot();
        }         

        ImGui::End();
        first_load_plot = false;



        // Right Pannel
        ImGui::SetNextWindowPos(ImVec2((int)(windowWidth - right_window_width), 0));
        ImGui::SetNextWindowSize(ImVec2((int)right_window_width, windowHeight));
        ImGui::Begin("Control Panel", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse); 
        
        // DEVELOP USE ONLY: used to probe widget or window size 
        ImVec2 right_window_size = ImGui::GetWindowSize();


        // // Model Image Indicator Section
        // ImGui::Image();

        // TODO


        // check box for know and unknow thickness
        ImGui::SetCursorPos(ImVec2(10, 200));
        ImGui::Checkbox("Fabry-Pérot Effect", &know_FP);

        // Select File Section
        ImGui::SetCursorPos(ImVec2(right_window_size.x - 125, 30));
        if (ImGui::Button(" Open File ")) 
        {
            // add selection prompt
            ImGui::OpenPopup("Select File Type");
            
            IGFD::FileDialogConfig config;
            config.path = ".";
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".csv,.txt", config);           
        }
        if (ImGui::BeginPopupModal("Select File Type", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) 
            {
                ImGui::Text("Please select an spectrum type: ");
                if (ImGui::Button("Reference")) 
                {
                    selected_file_type =  "ref"; 
                    ImGui::CloseCurrentPopup();  // Close modal after selection
                }
                ImGui::SameLine();
                if (ImGui::Button("Substrate")) 
                {
                    selected_file_type =  "sub"; 
                    ImGui::CloseCurrentPopup();  // Close modal after selection
                }
                ImGui::SameLine();
                if (ImGui::Button("Sample")) 
                {
                    selected_file_type =  "sam"; 
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Sample+Substrate")) 
                {
                    selected_file_type =  "sam_sub"; 
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Dataset")) 
                {
                    selected_file_type =  "dataset"; 
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

        // DEVELOP WILL BE REMOVED LATER
        ImVec2 button_size_default = ImGui::GetItemRectSize();
        drawFileDialogGui();
        //indicator
        ImGui::SetCursorPos(ImVec2(right_window_size.x - 125, 60));
        ImGui::BeginGroup();
        CircleIndicator(ref_selected, "Ref");
        ImGui::SameLine();
        CircleIndicator(sam_selected, "Sam");
        CircleIndicator(chip_selected, "Sub");
        CircleIndicator(sam_sub_selected, "Sam+Sub");
        ImVec2 circle_indicator_size_default = ImGui::GetItemRectSize();


        // Mode Selection Section
        ImGui::Text(" Model");
        ImGui::RadioButton("Mode 1", mode == 0); if (ImGui::IsItemClicked()) mode = 0;
        ImGui::RadioButton("Mode 2", mode == 1); if (ImGui::IsItemClicked()) mode = 1;
        ImVec2 radio_button_size_default = ImGui::GetItemRectSize();
        // save button
        if (ImGui::Button(" Save Data "))
        {
            //save data
            // get_phase(c_t_dataset, std::string(ROI_from), std::string(ROI_to), check_phase);
            // test_phase = true;
        }
        ImGui::EndGroup();



        // Thickness Parameters Setting Section
        ImGui::Separator();
        ImGui::Text("Global Phases (Unit ps)");
        // ImVec2 text_size_default = ImGui::GetItemRectSize();
        ImGui::Text("Sub     ");
        ImGui::SameLine();
        ImGui::Text("Sample   ");
        ImGui::SameLine();
        ImGui::Text("Sample+Sub");

        ImGui::SetNextItemWidth(90); 
        ImGui::InputTextWithHint("##t1", "Sub", chip_gp, IM_ARRAYSIZE(chip_gp));
        ImGui::SameLine();
        ImGui::SetNextItemWidth(90); 
        ImGui::InputTextWithHint("##t2", "Sam", sam_gp, IM_ARRAYSIZE(sam_gp));
        ImGui::SameLine();
        ImGui::SetNextItemWidth(90); 
        ImGui::InputTextWithHint("##t3", "Sam+Sub", sam_sub_gp, IM_ARRAYSIZE(sam_sub_gp));
        ImGui::SameLine();
        ImGui::Text(point.c_str());
        ImGui::SameLine();
        if (ImGui::Button(" Init "))
        {
            // 1. initial thickness info data
            // init_thickness_scan(std::string(chip_gp), std::string(sam_gp), std::string(sam_chip_gp));
            // init the global phase for three transmission spectrum 
        }
        // ImGui::SetCursorPos(ImVec2(10, 310));
        // ImGui::Text((std::string("Optimized Thickness: ") + std::string(OptThickness)).c_str());
        ImGui::Text("Thickness(sam):");
        // ImGui::SameLine();
        // ImGui::SetNextItemWidth(150); 

        ImGui::SameLine();
        ImGui::SetNextItemWidth(60);
        ImGui::InputTextWithHint("##t8", "Thickness", Thickness, IM_ARRAYSIZE(Thickness));
        ImGui::SameLine();
        ImGui::Text("(mm)");
        ImGui::SameLine();
        if (ImGui::Button(" Set Sam "))
        {
            ROI_data.L = torch::tensor(std::stof(std::string(Thickness)), torch::dtype(torch::kFloat));
            logger.Log(DataLogger::INFO, "Thickness is set to: " + std::string(Thickness));
        }

        // substrate thickness
        ImGui::Text("Thickness(sub):");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60); 
        ImGui::InputTextWithHint("##t9", "Sub Thickness(mm)", phase_delay, IM_ARRAYSIZE(phase_delay));
        ImGui::SameLine();
        ImGui::Text("(mm)");
        ImGui::SameLine();
        if (ImGui::Button(" Set Sub "))
        {
            induced_phase_delay = std::stof(std::string(phase_delay));
            logger.Log(DataLogger::INFO, "Set substrate thickness: " + std::string(phase_delay));
        }

        // ImGui::Columns(2,"##c1");
        // ImGui::BeginGroup();
        // // controlled phase delay
        // ImGui::SetNextItemWidth(180); 
        // ImGui::InputTextWithHint("##t9", "Chip Thickness(mm)", phase_delay, IM_ARRAYSIZE(phase_delay));
        // // ImGui::SameLine();
        // if (ImGui::Button(" Set Phase Delay "))
        // {
        //     induced_phase_delay = std::stof(std::string(phase_delay));
        //     logger.Log(DataLogger::INFO, "Set introduced phase delay: " + std::string(phase_delay));
        // }
        // ImGui::EndGroup();
        // ImGui::NextColumn();

        
        // ImGui::Columns(1);



        // ImGui::BeginDisabled(isTraining);
        // if (ImGui::Button(" Find Thickness ", ImVec2(188, 40)))
        // {
        //     std::cout << "click find thickness button" << std::endl;
        //     //  start do recursive finding thickness algo
        //     stopFlag = false;
        //     isTraining = true;
        //     if (thicknessThread.joinable()){
        //         thicknessThread.join();}
        //     thicknessThread = std::thread(extraction_thickness_freestanding, std::string(learning_rate), std::string(iteration_num), std::string(ROI_from), std::string(ROI_to));
        //     // trainingThread.detach();
        //     first_load_plot = true;
        // }
        // ImGui::EndDisabled();


        // Extract Refractive Idx Section
        ImGui::Separator();
        ImGui::Text("Extract Complex Refractive Index");
        
        // Select region of interest
        ImGui::Text("Select region of interest: ");
        ImGui::BeginGroup();
        ImGui::SetNextItemWidth(100); 
        ImGui::InputTextWithHint("##t4", "ROI from", ROI_from, IM_ARRAYSIZE(ROI_from));
        ImGui::SetNextItemWidth(100); 
        ImGui::SameLine();
        ImGui::InputTextWithHint("##t5", "ROI to", ROI_to, IM_ARRAYSIZE(ROI_to));
        ImGui::EndGroup();
        ImGui::SameLine();
        if (ImGui::Button(" Set "))
        {
            // reset ROI region
            // logger.Log(DataLogger::INFO, "Get ROI struct info");
            // logger.Log(DataLogger::INFO, std::string(ROI_from));
            // logger.Log(DataLogger::INFO, std::string(ROI_to));
            set_ROI_dataset(std::string(ROI_from), std::string(ROI_to));
            roi_selector = true;
            first_load_plot = true;
        }
        ImGui::SameLine();
        if (ImGui::Button(" Reset "))
        {
            // reset ROI region
            roi_selector = false;
            first_load_plot = true;
        }

        // std::cout << ROI_data.L << std::endl;   //[ CPUDoubleType{} ]

        // ImGui::SetCursorPos(ImVec2(30, 332));
        ImGui::Columns(2,"##c2");
        ImGui::BeginGroup();
        ImGui::SetNextItemWidth(150); 
        ImGui::InputTextWithHint("##t6", "Learning rate", learning_rate, IM_ARRAYSIZE(learning_rate));
        ImGui::SetNextItemWidth(150); 
        ImGui::InputTextWithHint("##t7", "Iteration num", iteration_num, IM_ARRAYSIZE(iteration_num));
        ImGui::EndGroup();
        // ImGui::SetCursorPos(ImVec2(right_window_size.x - 210, 360));
        ImGui::NextColumn();
        ImGui::BeginDisabled(isTraining);
        if (ImGui::Button(" Extraction ", ImVec2(180, 30)))
        {
            //extraction refractive index interface
            stopFlag = false;
            isTraining = true;
            //initial model parameters to avoid crash
            prepare_network_prams();
            if (trainingThread.joinable()){
                trainingThread.join();}
            trainingThread = std::thread(extraction_freestanding, std::string(learning_rate), std::string(iteration_num), std::string(ROI_from), std::string(ROI_to));
            // trainingThread.detach();
            first_load_plot = true;
        }
        ImGui::EndDisabled();
        if (ImGui::Button(" Stop ", ImVec2(180, 30)))
        {
            //extraction stop interface
            stopFlag = true;
            if (trainingThread.joinable()) 
            {
                trainingThread.join();
            }
            isTraining = false;
        }
        ImGui::Columns(1); // back to single column
        ImGui::Separator(); 
        // ImGui::Text("Optimized Sample Thickness: ");
        // // ImGui::SameLine();
        // // add thickness display panel

        // ImGui::Separator(); 
        // if (isTraining) {
        ImGui::ProgressBar(progress, ImVec2(400,20), (std::to_string((int)(progress*100)) + "%").c_str());
        // }
        
        // display switch
        ImGui::RadioButton("Time Domain", TimeFreqSelect == 0); if (ImGui::IsItemClicked()) {TimeFreqSelect = 0; first_load_plot = true;}
        // ImGui::SameLine();
        ImGui::RadioButton("Freq Domain", TimeFreqSelect == 1); if (ImGui::IsItemClicked()) {TimeFreqSelect = 1; first_load_plot = true;}


        // clear all data
        ImGui::SetCursorPos(ImVec2(right_window_size.x - 155, right_window_size.y - (int)(right_window_size.y * 180 / 720) - 100));
        if (ImGui::Button("Clear ALL DATA"))
        {
            // clear all stored data
            clear_data();
            roi_selector = false;
            ref_selected = false;
            chip_selected = false;
            sam_selected = false;
            sam_sub_selected = false;
        } 


        // DataLogger Section
        ImGui::SetCursorPos(ImVec2(right_window_size.x - 105, right_window_size.y - (int)(right_window_size.y * 180 / 720) - 61));  // y offset 26
        if (ImGui::Button("Clear Log")) 
        {
            logger.ClearLog();
        }
        // program log output section
        ImGui::SetCursorPos(ImVec2(10, right_window_size.y - (int)(right_window_size.y * 180 / 720) - 55)); // y offset 20
        ImGui::Text("Program log output:");
        // ImGui::SetCursorPos(ImVec2(10, right_window_size.y - (int)(right_window_size.y * 230 / 720)));
        ImGui::SetCursorPos(ImVec2(10, right_window_size.y - (int)(right_window_size.y * 180 / 720) - 35));
        // Display the log buffer content in a scrollable text area
        ImGui::BeginChild("LogArea", ImVec2(right_window_size.x-10, (int)(right_window_size.y * 180 / 720)), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        auto logBuffer = logger.GetLogBuffer();
        for (const auto& logEntry : logBuffer) 
        {
            ImGui::TextWrapped("%s", logEntry.c_str());
        }
        ImGui::SetScrollHereY(1.0f);
        ImGui::EndChild();



        // Version Display Section        
        ImGui::SetCursorPos(ImVec2(10, right_window_size.y - 28));
        ImGui::Text("App FPS: %.1f", io.Framerate);
        ImGui::SetCursorPos(ImVec2(right_window_size.x - 200, right_window_size.y - 28));
        ImGui::Text("By Dr. Yi  V: 0.2.1");
        ImGui::End();



        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

    }



    //clean up
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}




// position finder helper
        // ----------------------------------------------------------------------------//
        // // Get the position of the current widget (top-left corner of the button)
        // ImVec2 buttonPos = ImGui::GetItemRectMin();  // Minimum corner (x, y)
        // ImVec2 buttonSize = ImGui::GetItemRectSize(); // Width, Height of the button

        // // Output the position and size
        // ImGui::Text("Button Position: (%.1f, %.1f)", buttonPos.x, buttonPos.y);
        // ImGui::Text("Button Size: (%.1f, %.1f)", buttonSize.x, buttonSize.y);
        // ----------------------------------------------------------------------------//





        // ---------------------------------------------------------------------------------------------------------------- //


        // // DEVELOP CHECK WINDOW SIZE
        // ImGui::SetCursorPos(ImVec2(10, 600));
        // if (ImGui::Button("Check Window"))
        // // ImVec2 size1 = ImGui::GetItemRectSize();
        // {
        //     std::string message = std::to_string(right_window_size.x) + "," + std::to_string(right_window_size.y);
        //     logger.Log(DataLogger::INFO, message);
        //     std::string message2 = "button: " + std::to_string(button_size_default.x) + "," + std::to_string(button_size_default.y);
        //     logger.Log(DataLogger::INFO, message2);
        //     std::string message3 = "Text : " + std::to_string(text_size_default.x) + "," + std::to_string(text_size_default.y);
        //     logger.Log(DataLogger::INFO, message3);
        //     std::string message4 = "Radio: " + std::to_string(radio_button_size_default.x) + "," + std::to_string(radio_button_size_default.y);
        //     logger.Log(DataLogger::INFO, message4);
        //     std::string message5 = "Circle : " + std::to_string(circle_indicator_size_default.x) + "," + std::to_string(circle_indicator_size_default.y);
        //     logger.Log(DataLogger::INFO, message5);
        //     std::string message6 = "Input : " + std::to_string(input_text_size_default.x) + "," + std::to_string(input_text_size_default.y);
        //     logger.Log(DataLogger::INFO, message6);
        //     std::string message7 = "TEST FOR MARCRO : " + std::to_string(BUTTON_SIZE.x) + "," + std::to_string(BUTTON_SIZE.y);
        //     logger.Log(DataLogger::INFO, message7);
        // }


        // ---------------------------------------------------------------------------------------------------------------- //