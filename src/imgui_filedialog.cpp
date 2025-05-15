// some custom widget are listed here.

#include "imgui_filedialog.h"
// #include "global_logger.h"


// std::vector<float> pos;
torch::Tensor w;
// int skip_row = 1;
std::unordered_map<std::string, spectrum_dataset> spectrum_container;
spectrum_dataset ref_spectrum;
spectrum_dataset chip_spectrum;
spectrum_dataset sam_spectrum;
spectrum_dataset sam_chip_spectrum;

complex_transmission_dataset c_t_dataset;



void drawFileDialogGui() 
{   
    // reserve 3 slots for fast performance
    spectrum_container.reserve(4);
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
            if (selected_file_type == "dataset")
            {
                load_dataset_spectrum(filePathName, ref_spectrum, chip_spectrum, sam_chip_spectrum);
                spectrum_container["ref"] = std::move(ref_spectrum);
                ref_selected = true;
                message = "Load reference success.";
                logger.Log(DataLogger::INFO, message);

                spectrum_container["sub"] = std::move(chip_spectrum);
                chip_selected = true;
                message = "Load substrate success.";
                logger.Log(DataLogger::INFO, message);

                // spectrum_container["sam"] = std::move(sam_spectrum);
                // sam_selected = true;
                // message = "Load sample success.";
                // logger.Log(DataLogger::INFO, message);

                spectrum_container["sam_sub"] = std::move(sam_chip_spectrum);
                sam_sub_selected = true;
                message = "Load sample + substrate success.";
                logger.Log(DataLogger::INFO, message);

                construct_w(spectrum_container["ref"]);
                message = "Obtain angular frequency success.";
                logger.Log(DataLogger::INFO, message);

            }
            else if (selected_file_type == "ref")
            {
                load_spectrum(filePathName, ref_spectrum);
                spectrum_container["ref"] = std::move(ref_spectrum);
                ref_selected = true;
                message = "Load reference success.";
                logger.Log(DataLogger::INFO, message);
                construct_w(spectrum_container["ref"]);
                message = "Obtain angular frequency success.";
                logger.Log(DataLogger::INFO, message);
            } 
            else if (selected_file_type == "sub")
            {
                load_spectrum(filePathName, chip_spectrum);
                spectrum_container["sub"] = std::move(chip_spectrum);
                chip_selected = true;
                message = "Load substrate success.";
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
            else if (selected_file_type == "sam_sub")
            {
                load_spectrum(filePathName, sam_chip_spectrum);
                spectrum_container["sam_sub"] = std::move(sam_chip_spectrum);
                sam_sub_selected = true;
                message = "Load sample + substrate success.";
                logger.Log(DataLogger::INFO, message);
            }
            else
            {
                message = "Invalid spectrum type, please retry.";
                logger.Log(DataLogger::ERROR, message);
                ImGuiFileDialog::Instance()->Close();
            }
            

            // check if container has ref and chip and sam and sam_chip
            if ((!spectrum_container["ref"].Tm.empty()) && (!spectrum_container["sub"].Tm.empty())) 
            {
                // TODO AFTER CLEAR DATA, LOAD NEW DATA NO TM1_abs calculation results.
                c_t_dataset.Tm_sub = get_complex_transmission(spectrum_container["sub"], spectrum_container["ref"]);
                
                auto abs_Tm0 = torch::abs(c_t_dataset.Tm_sub).to(torch::kFloat);
                c_t_dataset.Tm_sub_abs.resize(abs_Tm0.size(0));
                std::memcpy(c_t_dataset.Tm_sub_abs.data(), abs_Tm0.data_ptr<float>(), abs_Tm0.numel() * sizeof(float));
            }

            if ((!spectrum_container["ref"].Tm.empty()) && (!spectrum_container["sam"].Tm.empty())) 
            {
                // TODO AFTER CLEAR DATA, LOAD NEW DATA NO TM1_abs calculation results.
                c_t_dataset.Tm_sam = get_complex_transmission(spectrum_container["sam"], spectrum_container["ref"]);
                
                auto abs_Tm1 = torch::abs(c_t_dataset.Tm_sam).to(torch::kFloat);
                c_t_dataset.Tm_sam_abs.resize(abs_Tm1.size(0));
                std::memcpy(c_t_dataset.Tm_sam_abs.data(), abs_Tm1.data_ptr<float>(), abs_Tm1.numel() * sizeof(float));
            }

            // should use sam_sub/sub instead of sam_sub/ref
            // if ((!spectrum_container["ref"].Tm.empty()) && (!spectrum_container["sam_sub"].Tm.empty())) 
            if ((!spectrum_container["sub"].Tm.empty()) && (!spectrum_container["sam_sub"].Tm.empty())) 
            {
                c_t_dataset.Tm_sam_sub = get_complex_transmission(spectrum_container["sam_sub"], spectrum_container["sub"]);
                // std::cout << "Tm2 calculated: " << Tm2[1] << std::endl;
                auto abs_Tm2 = torch::abs(c_t_dataset.Tm_sam_sub).to(torch::kFloat);
                c_t_dataset.Tm_sam_sub_abs.resize(abs_Tm2.size(0));
                std::memcpy(c_t_dataset.Tm_sam_sub_abs.data(), abs_Tm2.data_ptr<float>(), abs_Tm2.numel() * sizeof(float));
            }

        }
        first_load_plot = true;
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


void SaveToFile(const std::string& filename, const complex_refractive_index& data, const roi_dataset& data2) 
{
    // format data

    // std::ofstream file(filename);
    // if (file.is_open()) {
    //     file << data;
    //     file.close();
    //     std::cout << "Saved to " << filename << std::endl;
    // } else {
    //     std::cerr << "Failed to open or create file: " << filename << std::endl;
    // }

    size_t size = data2.roi_freqsTHz.size();

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file: " << filename << std::endl;
        return;
    }

    // Optional: write header
    file << "FreqsTHz,n,k\n";

    for (size_t i = 0; i < size; ++i) {
        file << data2.roi_freqsTHz[i] << "," << data.n2[i] << "," << data.k2[i] << "\n";
    }

    file.close();
    std::cout << "Saved to " << filename << std::endl;
}

void ShowSaveDialog() 
{
    if (ImGuiFileDialog::Instance()->Display("SaveFileDlg", ImGuiWindowFlags_NoCollapse,ImVec2(700, 400)))
    {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string path = ImGuiFileDialog::Instance()->GetFilePathName();
            SaveToFile(path, cri, ROI_data);
        }
        ImGuiFileDialog::Instance()->Close();
    }
}