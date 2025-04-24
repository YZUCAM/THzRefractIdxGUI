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
#include "imgui_redirect_log.h"


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

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    ImFontConfig font_cfg;
    font_cfg.FontDataOwnedByAtlas = false;
    font_cfg.OversampleH = 3;
    font_cfg.OversampleV = 1;
    font_cfg.PixelSnapH = true;
    io.Fonts->AddFontFromMemoryTTF(OGCourier_ttf, OGCourier_ttf_len, 18.0f, &font_cfg);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Declare a static instance of ImGuiStreamBuffer for use with std::cout
    static ImGuiStreamBuffer customStreamBuffer(new char[1024], 1024, 1024 * 5);    //max log size is 5kB
    static std::ostream customStream(&customStreamBuffer);

    int counter = 0;
    const size_t maxLines = 50;  // Display the last 50 lines
    const size_t maxLogSize = 1024 * 1; // Limit log size to 1KB


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
        ImGui::SetNextWindowSize(ImVec2((int)windowWidth*0.7, (int)windowHeight/3));
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
        ImGui::SetNextWindowPos(ImVec2(0, (int)windowHeight/3));
        ImGui::SetNextWindowSize(ImVec2((int)windowWidth*0.7,(int)windowHeight/3));
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
        ImGui::SetNextWindowPos(ImVec2(0, 2 * (int)windowHeight/3));
        ImGui::SetNextWindowSize(ImVec2((int)windowWidth*0.7,(int)windowHeight/3));
        ImGui::Begin("Plot3", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);                
        ImVec2 plot3_size = ImGui::GetContentRegionAvail(); 
        if (ImPlot::BeginPlot("Optimization Error", plot3_size)) 
        {
            static float xs[100], ys[100];
            for (int i = 0; i < 100; ++i) {
                xs[i] = i * 0.1f;
                ys[i] = cos(xs[i]);
            }
            ImPlot::PlotLine("Cosine", xs, ys, 100);
            ImPlot::EndPlot();
        }         
        // ImGui::Checkbox("Demo Window", &show_demo_window);      
        // ImGui::Checkbox("Another Window", &show_another_window);

        // ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);


        // ImGui::SetCursorPos(ImVec2(windowWidth-30, 0));
        // ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

        ImGui::End();

        // right pannel
        ImGui::SetNextWindowPos(ImVec2((int)windowWidth*0.7, 0));
        ImGui::SetNextWindowSize(ImVec2((int)windowWidth*0.3, windowHeight));
        ImGui::Begin("Control Panel", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse); 
        
        ImVec2 right_window_size = ImGui::GetWindowSize();
        
        std::cout.rdbuf(customStream.rdbuf());
        // Simulate program output
        std::cout << "Output power is 5mW with 100 percent test: " << counter << std::endl;
        counter++;




        // Scroll to the bottom of the log by default
        ImGui::SetCursorPos(ImVec2(10, right_window_size.y - 250));
        ImGui::Text("Program log output:");
        ImGui::SetCursorPos(ImVec2(10, right_window_size.y - 230));
        // Display the log buffer content in a scrollable text area
        ImGui::BeginChild("LogArea", ImVec2(right_window_size.x-10, 190), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

        // size_t linesShown = 0;
        // auto logBuffer = customStreamBuffer.GetLog();
        // size_t pos = logBuffer.find_last_of('\n', logBuffer.size() - 1);
        // while (pos != std::string::npos && linesShown < maxLines) {
        //     // Find the previous newline and display it
        //     size_t nextPos = logBuffer.find_last_of('\n', pos - 1);
        //     ImGui::TextWrapped(logBuffer.substr(nextPos + 1, pos - nextPos).c_str());
        //     pos = nextPos;
        //     linesShown++;
        // }
        auto logBuffer = customStreamBuffer.GetLog();
        if (logBuffer.size() > maxLogSize) 
        {
            logBuffer = logBuffer.substr(logBuffer.size() - maxLogSize); // Keep only the last 1KB of logs
        }
        ImGui::TextWrapped(logBuffer.c_str());

        // ImGui::TextWrapped(customStreamBuffer.GetLog().c_str());
        ImGui::EndChild();







        
        ImGui::SetCursorPos(ImVec2(10, right_window_size.y - 28));
        ImGui::Text("App FPS: %.1f", io.Framerate);
        ImGui::SetCursorPos(ImVec2(right_window_size.x - 200, right_window_size.y - 28));
        ImGui::Text("By Dr. Yi  V: 0.1.0");

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