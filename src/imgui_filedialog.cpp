#include "imgui_filedialog.h"

void drawFileDialogGui() 
{ 

    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey",ImGuiWindowFlags_NoCollapse,ImVec2(700, 400))) // => will show a dialog
    { 
      if (ImGuiFileDialog::Instance()->IsOk()) // action if OK
      { 
        std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
        std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
        // action
        // Use the selected file
        // ImGui::Text("Selected file: %s", filePath.c_str());
        std::cout << "Selected file: " << filePath.c_str() << std::endl;
      }
      
      // close
      ImGuiFileDialog::Instance()->Close();
    }
  }