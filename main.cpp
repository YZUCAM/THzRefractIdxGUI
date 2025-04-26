// Based on Dear ImGui base stone. Tailed for small lab gui.
//                                              By Dr. Yi Zhu
//                                                 23-Apr-2025

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "OGCourier.h"
#include <stdio.h>
#include <GLFW/glfw3.h> 
#include <implot.h>
#include <cmath>
#include "data_logger.h"
#include "imgui_filedialog.h"
#include "global_logger.h"      // ---> store all global variables

// define global variables
// Setup Datalogger
const size_t maxLines = 50;  // Display the last 50 lines
const size_t maxLogSize = 50; // Limit log size to 50 Byte
DataLogger logger(maxLogSize);    //log_buffer size 

// variables for backend THz calculation
std::vector<double> pos;
int skip_row = 0;

bool ref_selected = false;
bool sam_selected = false;
bool sam_delay_selected = false;
bool first_load_plot = false;

std::string selected_file_type = "";



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

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Complex Refractive Index Extractor", nullptr, nullptr);
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
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

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
    


    // state variable
    int mode = 0;
    int TimeFreqSelect = 0;

    char thick_from[128] = "";
    char thick_to[128] = "";
    char thick_step[128] = "";
    char learning_rate[128] = "";
    char iteration_num[128] = "";
    std::string point = "25";
    std::string OptThickness = "0";
    float progress = 0.99;




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
                ImPlot::SetupAxes("Time (s)","E (a.u.)");
                ImPlot::SetupLegend(ImPlotLocation_NorthEast);
                ImPlot::PlotLine("Ref", spectrum_container["ref"].times.data(), spectrum_container["ref"].Tm.data(), spectrum_container["ref"].Tm.size());
                // if (!spectrum_container["ref"].times.empty()){}
                ImPlot::PlotLine("Sam", spectrum_container["sam"].times.data(), spectrum_container["sam"].Tm.data(), spectrum_container["sam"].Tm.size());
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
                ImPlot::PlotLine("Ref", spectrum_container["ref"].freqsTHz.data(), spectrum_container["ref"].fty_abs.data(), spectrum_container["ref"].fty_abs.size());
                ImPlot::PlotLine("Sam", spectrum_container["sam"].freqsTHz.data(), spectrum_container["sam"].fty_abs.data(), spectrum_container["sam"].fty_abs.size());
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
            ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10);
            ImPlot::PlotLine("Tm1", spectrum_container["ref"].freqsTHz.data(), Tm1_abs.data(), Tm1_abs.size());
            ImPlot::PlotLine("Tm2", spectrum_container["ref"].freqsTHz.data(), Tm2_abs.data(), Tm2_abs.size());
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
            static float xs[100], ys[100];
            for (int i = 0; i < 100; ++i) {
                xs[i] = i * 0.1f;
                ys[i] = cos(xs[i]);
            }
            ImPlot::SetupLegend(ImPlotLocation_NorthEast);
            ImPlot::PlotLine("Cosine", xs, ys, 100);
            ImPlot::EndPlot();
        }         

        ImGui::End();

        // Fourth Panel in the Left
        ImGui::SetNextWindowPos(ImVec2((int)(windowWidth - right_window_width)/2, (int)windowHeight/2));
        ImGui::SetNextWindowSize(ImVec2((int)(windowWidth - right_window_width)/2,(int)windowHeight/2));
        ImGui::Begin("Plot4", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);                
        ImVec2 plot4_size = ImGui::GetContentRegionAvail(); 
        if (first_load_plot){ImPlot::SetNextAxesToFit();}
        if (ImPlot::BeginPlot("Optimization Error", plot4_size)) 
        {
            static float xs[100], ys[100];
            for (int i = 0; i < 100; ++i) {
                xs[i] = i * 0.1f;
                ys[i] = cos(xs[i]);
            }
            ImPlot::SetupLegend(ImPlotLocation_NorthEast);
            ImPlot::PlotLine("Cosine", xs, ys, 100);
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
                if (ImGui::Button("Sample")) 
                {
                    selected_file_type =  "sam"; 
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Sample + Delay")) 
                {
                    selected_file_type =  "sam_delay"; 
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
        CircleIndicator(sam_delay_selected, "Sam+Delay");
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

        }
        ImGui::EndGroup();

        // Thickness Parameters Setting Section
        ImGui::Separator();
        ImGui::Text("Thickness Optimization");
        ImVec2 text_size_default = ImGui::GetItemRectSize();
        ImGui::SetNextItemWidth(100); 
        ImGui::InputTextWithHint("##t1", "From", thick_from, IM_ARRAYSIZE(thick_from));
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100); 
        ImGui::InputTextWithHint("##t2", "To", thick_to, IM_ARRAYSIZE(thick_to));
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100); 
        ImGui::InputTextWithHint("##t3", "Step", thick_step, IM_ARRAYSIZE(thick_step));
        ImVec2 input_text_size_default = ImGui::GetItemRectSize();
        ImGui::SameLine();
        ImGui::Text(point.c_str());

        ImGui::SetCursorPos(ImVec2(10, 265));
        ImGui::Text((std::string("Optimized Thickness: ") + OptThickness).c_str());
        if (ImGui::Button(" Use Opt-Thickness ", ImVec2(188, 40)))
        {
            //select optimized thickness interface
        }
        ImGui::SameLine();
        if (ImGui::Button(" Find Thickness ", ImVec2(188, 40)))
        {
            //find thickness interface
        }
        

        // Extract Refractive Idx Section
        ImGui::Separator();
        ImGui::Text("Extract Complex Refractive Index");
        // ImGui::SetCursorPos(ImVec2(30, 332));
        ImGui::Columns(2);
        ImGui::BeginGroup();
        ImGui::SetNextItemWidth(150); 
        ImGui::InputTextWithHint("##t4", "Learning rate", learning_rate, IM_ARRAYSIZE(learning_rate));
        ImGui::SetNextItemWidth(150); 
        ImGui::InputTextWithHint("##t5", "Iteration num", iteration_num, IM_ARRAYSIZE(iteration_num));
        ImGui::EndGroup();
        // ImGui::SetCursorPos(ImVec2(right_window_size.x - 210, 360));
        ImGui::NextColumn();
        if (ImGui::Button(" Extraction ", ImVec2(180, 30)))
        {
            //extraction refractive index interface
        }
        if (ImGui::Button(" Stop ", ImVec2(180, 30)))
        {
            //extraction stop interface
        }
        ImGui::Columns(1); // back to single column
        ImGui::Separator(); 
        ImGui::ProgressBar(progress, ImVec2(400,20), (std::to_string((int)(progress*100)) + "%").c_str());
        
        // display switch
        ImGui::RadioButton("Time Domain", TimeFreqSelect == 0); if (ImGui::IsItemClicked()) {TimeFreqSelect = 0; first_load_plot = true;}
        ImGui::SameLine();
        ImGui::RadioButton("Freq Domain", TimeFreqSelect == 1); if (ImGui::IsItemClicked()) {TimeFreqSelect = 1; first_load_plot = true;}


        // DataLogger Section
        ImGui::SetCursorPos(ImVec2(right_window_size.x - 105, right_window_size.y - (int)(right_window_size.y * 190 / 720) - 61));  // y offset 26
        if (ImGui::Button("Clear Log")) 
        {
            logger.ClearLog();
        }
        // program log output section
        ImGui::SetCursorPos(ImVec2(10, right_window_size.y - (int)(right_window_size.y * 190 / 720) - 55)); // y offset 20
        ImGui::Text("Program log output:");
        // ImGui::SetCursorPos(ImVec2(10, right_window_size.y - (int)(right_window_size.y * 230 / 720)));
        ImGui::SetCursorPos(ImVec2(10, right_window_size.y - (int)(right_window_size.y * 190 / 720) - 35));
        // Display the log buffer content in a scrollable text area
        ImGui::BeginChild("LogArea", ImVec2(right_window_size.x-10, (int)(right_window_size.y * 190 / 720)), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
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
        ImGui::Text("By Dr. Yi  V: 0.1.1");
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