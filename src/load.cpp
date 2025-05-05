#include "load.h"
#include "global_logger.h"


void read_csv_columns(const std::string& filename, std::vector<float>& c1,
    std::vector<float>& c2) 
{
    c1.clear();
    c2.clear();

    try 
    {
        FILE* fp = std::fopen(filename.c_str(), "r");
        if (!fp)
            throw std::runtime_error("Failed to open file");

        // Skip the header line
        char line[1024];
        if (!std::fgets(line, sizeof(line), fp))
            throw std::runtime_error("Failed to read header line");

        // Initialize CSVReader with FILE* (after header skipped)
        io::CSVReader<2, io::trim_chars<>, io::no_quote_escape<','>> in(filename, fp);

        // io::CSVReader<2> in(filename);
        // read just first two column. So better dataset prepared only with two data column
        // in.read_header(io::ignore_extra_column, "Time", "E");           // Kun's data
        // in.read_header(io::ignore_extra_column, "Var1", "Var2");

        float val1;
        float val2;
        int count = 0;

    while (in.read_row(val1, val2)) 
    {
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
    read_csv_columns(filename, pos, data.Tm);
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


// load batch of spectrum 
void load_dataset_spectrum(const std::string& filename, spectrum_dataset& data1, spectrum_dataset& data2, spectrum_dataset& data3, spectrum_dataset& data4)
{

    data1.times.clear();
    data1.Tm.clear();
    data2.times.clear();
    data2.Tm.clear();
    data3.times.clear();
    data3.Tm.clear();
    data4.times.clear();
    data4.Tm.clear();

    try 
    {
        FILE* fp = std::fopen(filename.c_str(), "r");
        if (!fp)
            throw std::runtime_error("Failed to open file");

        // Skip the header line
        char line[1024];
        if (!std::fgets(line, sizeof(line), fp))
            throw std::runtime_error("Failed to read header line");

        // Initialize CSVReader with FILE* (after header skipped)
        io::CSVReader<8, io::trim_chars<>, io::no_quote_escape<','>> in(filename, fp);
        // io::CSVReader<8> in(filename);
        // read csv line by line in the order.
        // val1, val3, val5, val7 must be time unit in s

        float val1;
        float val2;

        float val3;
        float val4;

        float val5;
        float val6;

        float val7;
        float val8;

        int count = 0;

    while (in.read_row(val1, val2, val3, val4, val5, val6, val7, val8)) 
    {
        // if (count++ < skip_rows) continue;

        data1.times.push_back(val1);
        data1.Tm.push_back(val2);

        data2.times.push_back(val3);
        data2.Tm.push_back(val4);

        data3.times.push_back(val5);
        data3.Tm.push_back(val6);

        data4.times.push_back(val7);
        data4.Tm.push_back(val8);
    }
    } 
    catch (const std::exception& e) 
    {
    std::cerr << "Error reading CSV: " << e.what() << std::endl;
    }

    data1.fty = fft(data1.Tm);
    data1.freqs = construct_freqs(data1.times);
    data1.freqsTHz.resize(data1.freqs.size());
    std::transform(data1.freqs.begin(), data1.freqs.end(), data1.freqsTHz.begin(), [](auto x){return x * 1e-12;});
    auto abs_result1 = torch::abs(data1.fty).to(torch::kFloat);
    data1.fty_abs.resize(abs_result1.size(0));
    std::memcpy(data1.fty_abs.data(), abs_result1.data_ptr<float>(), abs_result1.numel() * sizeof(float));

    data2.fty = fft(data2.Tm);
    data2.freqs = construct_freqs(data2.times);
    data2.freqsTHz.resize(data2.freqs.size());
    std::transform(data2.freqs.begin(), data2.freqs.end(), data2.freqsTHz.begin(), [](auto x){return x * 1e-12;});
    auto abs_result2 = torch::abs(data2.fty).to(torch::kFloat);
    data2.fty_abs.resize(abs_result2.size(0));
    std::memcpy(data2.fty_abs.data(), abs_result2.data_ptr<float>(), abs_result2.numel() * sizeof(float));

    data3.fty = fft(data3.Tm);
    data3.freqs = construct_freqs(data3.times);
    data3.freqsTHz.resize(data3.freqs.size());
    std::transform(data3.freqs.begin(), data3.freqs.end(), data3.freqsTHz.begin(), [](auto x){return x * 1e-12;});
    auto abs_result3 = torch::abs(data3.fty).to(torch::kFloat);
    data3.fty_abs.resize(abs_result3.size(0));
    std::memcpy(data3.fty_abs.data(), abs_result3.data_ptr<float>(), abs_result3.numel() * sizeof(float));

    data4.fty = fft(data4.Tm);
    data4.freqs = construct_freqs(data4.times);
    data4.freqsTHz.resize(data4.freqs.size());
    std::transform(data4.freqs.begin(), data4.freqs.end(), data4.freqsTHz.begin(), [](auto x){return x * 1e-12;});
    auto abs_result4 = torch::abs(data4.fty).to(torch::kFloat);
    data4.fty_abs.resize(abs_result4.size(0));
    std::memcpy(data4.fty_abs.data(), abs_result4.data_ptr<float>(), abs_result4.numel() * sizeof(float));

}




