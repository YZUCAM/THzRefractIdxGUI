// some custom widget are listed here.

#include "imgui_filedialog.h"
#include "global_logger.h"


void drawFileDialogGui() 
{ 

    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey",ImGuiWindowFlags_NoCollapse,ImVec2(700, 400))) // => will show a dialog
    { 
      if (ImGuiFileDialog::Instance()->IsOk()) // action if OK
      { 
        std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
        // std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
        // action

        // Use the selected file
        std::cout << "Selected file: " << filePathName.c_str() << std::endl;
        std::string message = "Selected file: " + filePathName;
        logger.Log(DataLogger::INFO, message);

        // pass it to global file variable to invoke backend function


      }
      
      // close
      ImGuiFileDialog::Instance()->Close();
    }
  }



  void CircleIndicator(bool status, const char* label) 
  {
    ImVec2 pos = ImGui::GetCursorScreenPos();
    float radius = 6.0f;

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImU32 color = status ? IM_COL32(36, 135, 0, 255) : IM_COL32(135, 0, 0, 255); // green or red

    draw_list->AddCircleFilled(ImVec2(pos.x + radius, pos.y + radius), radius, color);

    ImGui::Dummy(ImVec2(radius * 2, radius * 2)); // Reserve space
    if (label) {
        ImGui::SameLine();
        ImGui::TextUnformatted(label);
    }
}