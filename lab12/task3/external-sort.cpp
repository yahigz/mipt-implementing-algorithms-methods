#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

class ExternalSorter{
private:
    std::string input_file_;
    std::string output_file_;

    std::string to_print = "./tmp/partial";

    int ram_size_;
    int batch_size_;
    int hdd_size_;

    void WriteRAMIntoFile(std::vector<int>& ram, std::ofstream& outp) {
        for (int done = 0; done < ram.size(); ++done) {
            outp << ram[done] << '\n';
        }

        ram.clear();
    }

    void WriteAndSortRAMIntoFile(std::vector<int>& ram, std::ofstream& outp, int cnt) {
        std::vector<int> index(cnt);

        for (int done = 0; done < ram.size(); ++done) {
            int ind_min = -1;
            for (int k = 0; k < cnt; ++k) {
                if (index[k] == batch_size_ || index[k] + k * batch_size_ >= ram.size()) {
                    continue;
                }
                if (ind_min == -1 || ram[index[k] + k * batch_size_] < ram[index[ind_min] + ind_min * batch_size_]) {
                    ind_min = k;
                }
            }
            outp << ram[index[ind_min] + ind_min * batch_size_] << '\n';
            ++index[ind_min];
        }

        ram.clear();
    }

    void MergeFiles(std::vector<int>& files, std::ofstream& outp) {
        std::vector<std::ifstream> inp(files.size());
        for (int i = 0; i < inp.size(); ++i) {
            inp[i] = std::ifstream(to_print + std::to_string(files[i]) + ".txt");
        }
        std::vector<int> ram;
        std::string line;

        std::vector<bool> done(inp.size());
        std::vector<int> last(inp.size());

        for (int i = 0; i < inp.size(); ++i) {
            if (inp[i] >> line) {
                last[i] = std::atoi(line.c_str());
            } else {
                done[i] = true;
            }
        }

        bool exist = true;
        
        while (exist) {
            exist = false;
            int ind_min = -1;
            for (int i = 0; i < inp.size(); ++i) {
                if (done[i]) {
                    continue;
                }

                if (ind_min == -1 || last[i] < last[ind_min]) {
                    ind_min = i;
                }

                exist = true;
            }

            if (!exist) {
                break;
            }

            ram.push_back(last[ind_min]);

            if (ram.size() == ram_size_) {
                WriteRAMIntoFile(ram, outp);
            }

            if (inp[ind_min] >> line) {
                last[ind_min] = std::atoi(line.c_str());
            } else {
                done[ind_min] = true;
            }
        }

        if (!ram.empty()) {
            WriteRAMIntoFile(ram, outp);
        }

        for (int i = 0; i < inp.size(); ++i) {
            inp[i].close();
        }
    }

    int SplitIntoFiles() {
        std::vector<char> custom_buffer(batch_size_);

        std::ifstream inp(input_file_);
        inp.rdbuf()->pubsetbuf(custom_buffer.data(), batch_size_);

        std::string line;

        std::vector<int> ram;
        int cnt = 0;
        int cnt_files = 0;

        for (int i = 0; i < hdd_size_;) {
            int j = 0;
            std::vector<int> batch;
            while (j < batch_size_ && i + j < hdd_size_) {
                inp >> line;

                batch.push_back(std::atoi(line.c_str()));

                ++j;
            }

            i += j;
            sort(batch.begin(), batch.end());
            for (auto elem : batch) {
                ram.push_back(elem);
            }

            ++cnt;

            if (ram.size() == ram_size_) {
                ++cnt_files;
                std::vector<char> custom_buffer_outp(batch_size_);
                std::ofstream outp(to_print + std::to_string(cnt_files) + ".txt");
                outp.rdbuf()->pubsetbuf(custom_buffer_outp.data(), batch_size_);

                WriteAndSortRAMIntoFile(ram, outp, cnt);
                outp.close();
                cnt = 0;
            }
        }

        if (cnt != 0) {
            ++cnt_files;
            std::vector<char> custom_buffer_outp(batch_size_);
            std::ofstream outp(to_print + std::to_string(cnt_files) + ".txt");
            outp.rdbuf()->pubsetbuf(custom_buffer_outp.data(), batch_size_);

            WriteAndSortRAMIntoFile(ram, outp, cnt);
            outp.close();
            cnt = 0;
        }

        inp.close();
        
        return cnt_files;
    }

public:

    ExternalSorter(std::string input_file, std::string output_file, int ram_size, int batch_size, int hdd_size): input_file_(input_file), output_file_(output_file),
                                                                                                                ram_size_(ram_size), batch_size_(batch_size),
                                                                                                                hdd_size_(hdd_size) {}
    void Sort() {
        std::filesystem::create_directory("./tmp");

        int old_cnt_files = 0;

        int cnt_files = SplitIntoFiles();
        
        int need_cnt = ram_size_ / batch_size_;

        while (cnt_files - old_cnt_files > need_cnt) {
            int sv_old = old_cnt_files;
            old_cnt_files = cnt_files;
            for (int i = sv_old + 1; i <= old_cnt_files; ) {
                int j = 0;
                std::vector<int> num;
                while (i + j <= old_cnt_files && j < need_cnt) {
                    num.push_back(i + j);
                    ++j;
                }

                i += j;
                    
                ++cnt_files;
                std::vector<char> custom_buffer_outp(batch_size_);
                std::ofstream outp(to_print + std::to_string(cnt_files) + ".txt");
                outp.rdbuf()->pubsetbuf(custom_buffer_outp.data(), batch_size_);
                MergeFiles(num, outp);
                outp.close();
            }
        }

        std::vector<int> num;
        for (int i = old_cnt_files + 1; i <= cnt_files; ++i) {
            num.push_back(i);
        }


        std::vector<char> custom_buffer_outp(batch_size_);
        std::ofstream outp(output_file_);
        outp.rdbuf()->pubsetbuf(custom_buffer_outp.data(), batch_size_);
        MergeFiles(num, outp);
        outp.close();

        std::filesystem::remove_all("./tmp");
    }
};

int main() {
    ExternalSorter srt("./custom_test/input.txt", "./custom_test/output.txt", 256, 64, 10000);
    srt.Sort();
}