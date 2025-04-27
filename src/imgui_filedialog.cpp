// some custom widget are listed here.

#include "imgui_filedialog.h"
#include "global_logger.h"


std::unordered_map<std::string, spectrum_dataset> spectrum_container;
spectrum_dataset ref_spectrum;
spectrum_dataset sam_spectrum;
spectrum_dataset sam_delay_spectrum;

complex_transmission_dataset c_t_dataset;




void drawFileDialogGui() 
{   
    // reserve 3 slots for fast performance
    spectrum_container.reserve(3);
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey",ImGuiWindowFlags_NoCollapse,ImVec2(700, 400))) // => will show a dialog
    { 
        if (ImGuiFileDialog::Instance()->IsOk()) // action if OK
        { 
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            // std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            // action

            // Use the selected file
            // std::cout << "Selected file: " << filePathName.c_str() << std::endl;
            std::string message = "Selected file: " + filePathName;
            logger.Log(DataLogger::INFO, message);

            // pass it to global file variable to invoke backend function
            // NEED A PROMPT TO TELL WHAT NEED TO BE LOAD
            if (selected_file_type == "ref")
            {
                load_spectrum(filePathName, ref_spectrum);
                spectrum_container["ref"] = std::move(ref_spectrum);
                ref_selected = true;
                message = "Load reference success.";
                logger.Log(DataLogger::INFO, message);
            } 
            else if (selected_file_type == "sam")
            {
                load_spectrum(filePathName, sam_spectrum);
                spectrum_container["sam"] = std::move(sam_spectrum);
                sam_selected = true;
                message = "Load sample success.";
                logger.Log(DataLogger::INFO, message);
            }
            else if (selected_file_type == "sam_delay")
            {
                load_spectrum(filePathName, sam_delay_spectrum);
                spectrum_container["sam_delay"] = std::move(sam_delay_spectrum);
                sam_delay_selected = true;
                message = "Load sample + delay success.";
                logger.Log(DataLogger::INFO, message);
            }
            else
            {
                message = "Invalid spectrum type, please retry.";
                logger.Log(DataLogger::ERROR, message);
                ImGuiFileDialog::Instance()->Close();
            }
            first_load_plot = true;

            // check if container has ref and sam and sam_delay
            if ((!spectrum_container["ref"].Tm.empty()) && (!spectrum_container["sam"].Tm.empty())) 
            {
                c_t_dataset.Tm1 = get_complex_transmission(spectrum_container["sam"], spectrum_container["ref"]);
                // std::cout << "Tm1 calculated: " << Tm1[1] << std::endl;
                auto abs_Tm1 = torch::abs(c_t_dataset.Tm1).to(torch::kDouble);
                c_t_dataset.Tm1_abs.resize(abs_Tm1.size(0));
                std::memcpy(c_t_dataset.Tm1_abs.data(), abs_Tm1.data_ptr<double>(), abs_Tm1.numel() * sizeof(double));
            }

            if ((!spectrum_container["ref"].Tm.empty()) && (!spectrum_container["sam_delay"].Tm.empty())) 
            {
                c_t_dataset.Tm2 = get_complex_transmission(spectrum_container["sam_delay"], spectrum_container["ref"]);
                // std::cout << "Tm2 calculated: " << Tm2[1] << std::endl;
                auto abs_Tm2 = torch::abs(c_t_dataset.Tm2).to(torch::kDouble);
                c_t_dataset.Tm2_abs.resize(abs_Tm2.size(0));
                std::memcpy(c_t_dataset.Tm2_abs.data(), abs_Tm2.data_ptr<double>(), abs_Tm2.numel() * sizeof(double));
            }

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
    if (label) 
    {
        ImGui::SameLine();
        ImGui::TextUnformatted(label);
    }
}