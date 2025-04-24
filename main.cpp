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

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Setup Datalogger
    int counter = 0;
    const size_t maxLines = 50;  // Display the last 50 lines
    const size_t maxLogSize = 1024 * 5; // Limit log size to 1KB
    DataLogger logger(maxLogSize);    //log_buffer size 5 kB

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

    
    // state flag
    bool test_diaglog = false;


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

        // first panel in the left
        ImGui::SetNextWindowPos(ImVec2(0,0));
        ImGui::SetNextWindowSize(ImVec2((int)windowWidth*0.35, (int)windowHeight/2));
        ImGui::Begin("Plot1", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);     // Create a window called "Hello, world!" and append into it.

        ImVec2 plot1_size = ImGui::GetContentRegionAvail(); 
        if (ImPlot::BeginPlot("Time Domain THz Spectrum", plot1_size)) 
        {
            static float xs[100], ys[100];
            for (int i = 0; i < 100; ++i) {
                xs[i] = i * 0.1f;
                ys[i] = tan(xs[i]);
            }
            ImPlot::PlotLine("Cosine", xs, ys, 100);
            ImPlot::EndPlot();
        }    


        // ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();

        // second panel in the left
        ImGui::SetNextWindowPos(ImVec2((int)windowWidth*0.35, 0));
        ImGui::SetNextWindowSize(ImVec2((int)windowWidth*0.35,(int)windowHeight/2));
        ImGui::Begin("Plot2", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);                

        ImVec2 plot2_size = ImGui::GetContentRegionAvail(); 
        if (ImPlot::BeginPlot("Frequency Domain THz Spectrum", plot2_size)) 
        {
            static float xs[100], ys[100];
            for (int i = 0; i < 100; ++i) {
                xs[i] = i * 0.1f;
                ys[i] = sin(xs[i]);
            }
            ImPlot::PlotLine("Cosine", xs, ys, 100);
            ImPlot::EndPlot();
        }    
        ImGui::End();

        // third panel in the left
        ImGui::SetNextWindowPos(ImVec2(0, (int)windowHeight/2));
        ImGui::SetNextWindowSize(ImVec2((int)windowWidth*0.35,(int)windowHeight/2));
        ImGui::Begin("Plot3", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);                
        ImVec2 plot3_size = ImGui::GetContentRegionAvail(); 
        if (ImPlot::BeginPlot("Complex Redractive Index", plot3_size)) 
        {
            static float xs[100], ys[100];
            for (int i = 0; i < 100; ++i) {
                xs[i] = i * 0.1f;
                ys[i] = cos(xs[i]);
            }
            ImPlot::PlotLine("Cosine", xs, ys, 100);
            ImPlot::EndPlot();
        }         

        ImGui::End();

        // fourth panel in the left
        ImGui::SetNextWindowPos(ImVec2((int)windowWidth*0.35, (int)windowHeight/2));
        ImGui::SetNextWindowSize(ImVec2((int)windowWidth*0.35,(int)windowHeight/2));
        ImGui::Begin("Plot4", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);                
        ImVec2 plot4_size = ImGui::GetContentRegionAvail(); 
        if (ImPlot::BeginPlot("Optimization Error", plot4_size)) 
        {
            static float xs[100], ys[100];
            for (int i = 0; i < 100; ++i) {
                xs[i] = i * 0.1f;
                ys[i] = cos(xs[i]);
            }
            ImPlot::PlotLine("Cosine", xs, ys, 100);
            ImPlot::EndPlot();
        }         

        ImGui::End();

        // right pannel
        ImGui::SetNextWindowPos(ImVec2((int)windowWidth*0.7, 0));
        ImGui::SetNextWindowSize(ImVec2((int)windowWidth*0.3, windowHeight));
        ImGui::Begin("Control Panel", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse); 
        
        ImVec2 right_window_size = ImGui::GetWindowSize();
        

        // select file
        if (ImGui::Button("Open File Dialog")) 
        {
            IGFD::FileDialogConfig config;
            config.path = ".";
            // ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".csv,.txt", config);

            std::cout << "Test log output." << std::endl;
            logger.Log(DataLogger::INFO, "Test log output.");
        }
        // drawFileDialogGui();

        ImGui::Checkbox("Enable Feature", &test_diaglog);
        if (test_diaglog)
        {
            // Simulate program output
            std::cout << "Output power is 5mW with 100 percent test: " << counter << std::endl;
            std::string message = "Output power is 5mW with 100 percent test: " + std::to_string(counter);
            logger.Log(DataLogger::INFO, message);

            counter++;
        }


        ImGui::SetCursorPos(ImVec2(280, right_window_size.y - 255));
        if (ImGui::Button("Clear Log")) 
        {
            logger.ClearLog();
        }


        // program log output section
        ImGui::SetCursorPos(ImVec2(10, right_window_size.y - 250));
        ImGui::Text("Program log output:");
        ImGui::SetCursorPos(ImVec2(10, right_window_size.y - 230));
        // Display the log buffer content in a scrollable text area
        ImGui::BeginChild("LogArea", ImVec2(right_window_size.x-10, 190), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

        
        auto logBuffer = logger.GetLogBuffer();
     
        // ImGui::TextWrapped(logBuffer.c_str());

        for (const auto& logEntry : logBuffer) 
        {
            ImGui::TextWrapped("%s", logEntry.c_str());
        }

        ImGui::SetScrollHereY(1.0f);
        

        // ImGui::TextWrapped(customStreamBuffer.GetLog().c_str());
        ImGui::EndChild();







        
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