#include "load.h"
#include "global_logger.h"


void read_csv_columns(const std::string& filename, std::vector<float>& c1,
    std::vector<float>& c2, int skip_rows) 
{
    c1.clear();
    c2.clear();

    try 
    {
        io::CSVReader<2> in(filename);
        in.read_header(io::ignore_extra_column, "Time", "E");           // Kun's data
        // in.read_header(io::ignore_extra_column, "Var1", "Var2");

        float val1;
        float val2;
        int count = 0;

    while (in.read_row(val1, val2)) 
    {
        if (count++ < skip_rows) continue;

        c1.push_back(val1);
        c2.push_back(val2);
    }
    } 
    catch (const std::exception& e) 
    {
    std::cerr << "Error reading CSV: " << e.what() << std::endl;
    }
}

// encapsulate a load function? input filepath, 
// output 3D vector [time, Tm, freqs(unit Hz), freqsTH(unit THz), fty: tensor, fty_abs]
void load_spectrum(const std::string& filename, spectrum_dataset& data)
{
    // skip_row and pos are global variables no need define.
    read_csv_columns(filename, pos, data.Tm, skip_row);
    data.fty = fft(data.Tm);
    data.times = pos2time(pos);              // this is direct read pos data,  USE KUN's DATA, the time original is ps now use this function convert to s(NEED TO FIX IN FUTURE)
    // data.times = pos;                           // this is direct read time data, time unit is s
    data.freqs = construct_freqs(data.times);
    data.freqsTHz.resize(data.freqs.size());
    // freqsTHz(freqs.size());
    std::transform(data.freqs.begin(), data.freqs.end(), data.freqsTHz.begin(), [](auto x){return x * 1e-12;});
    // convert tensor data to vector data 
    auto abs_result = torch::abs(data.fty).to(torch::kFloat);
    data.fty_abs.resize(abs_result.size(0));
    std::memcpy(data.fty_abs.data(), abs_result.data_ptr<float>(), abs_result.numel() * sizeof(float));
}


