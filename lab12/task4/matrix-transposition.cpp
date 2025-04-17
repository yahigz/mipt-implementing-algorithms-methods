#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

class MatrixTransposer {
    private:

        int small_size_ = 32;
        int side_;

        std::string input_file_;
        std::string output_file_;

        std::string templ_ = "./tmp/partial";

        int cnt_ = 0;

        std::string TransposeSmall(int n, std::ifstream& inp) {
            std::vector<std::vector<int>> a(n, std::vector<int>(n));
            for (auto& arr : a) {
                for (auto& elem : arr) {
                    inp >> elem;
                }
            }

            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < i; ++j) {
                    std::swap(a[i][j], a[j][i]);
                }
            }

            ++cnt_;
            std::string outp_filename = templ_ + std::to_string(cnt_) + ".txt";
            std::ofstream outp(outp_filename);

            for (auto& arr : a) {
                for (auto elem : arr) {
                    outp << elem << ' ';
                }
                outp << '\n';
            }

            inp.close();
            outp.close();

            return outp_filename;
        }

        std::vector<std::string> SplitIntoFiles(int n, std::ifstream& inp) {
            int k = (n >> 1);
            
            std::vector<std::string> filenames;
            std::vector<std::ofstream> outp;

            for (int i = 0; i < 4; ++i) {
                ++cnt_;
                std::string curr = templ_ + std::to_string(cnt_) + ".txt";
                filenames.push_back(curr);
                outp.push_back(std::ofstream(curr));
            }

            if (n & 1) {
                for (int i = 0; i < 2; ++i) {
                    ++cnt_;
                    std::string curr = templ_ + std::to_string(cnt_) + ".txt";
                    filenames.push_back(curr);
                    outp.push_back(std::ofstream(curr));
                }

                int curr;

                for (int i = 0; i < k; ++i) {
                    for (int j = 0; j < k; ++j) {
                        inp >> curr;
                        outp[0] << curr << ' ';
                    }

                    outp[0] << '\n';

                    inp >> curr;
                    outp[4] << curr << ' ';

                    for (int j = k + 1; j < n; ++j) {
                        inp >> curr;
                        outp[1] << curr << ' ';
                    }
                    outp[1] << '\n';
                }

                for (int j = 0; j < k; ++j) {
                    inp >> curr;
                    outp[5] << curr << ' ';
                }
                inp >> curr;
                outp[4] << curr << ' ';

                for (int j = k + 1; j < n; ++j) {
                    inp >> curr;
                    outp[5] << curr << ' ';
                }


                for (int i = k + 1; i < n; ++i) {
                    for (int j = 0; j < k; ++j) {
                        inp >> curr;
                        outp[2] << curr << ' ';
                    }
                    
                    outp[2] << '\n';

                    inp >> curr;
                    outp[4] << curr << ' ';
                    
                    for (int j = k + 1; j < n; ++j) {
                        inp >> curr;
                        outp[3] << curr << ' ';
                    }
                    outp[3] << '\n';
                }

                for (auto& elem : outp) {
                    elem.close();
                }

                inp.close();
                return filenames;
            }   

            int curr;

            for (int i = 0; i < k; ++i) {
                for (int j = 0; j < k; ++j) {
                    inp >> curr;
                    outp[0] << curr << ' ';
                }
                outp[0] << '\n';
                for (int j = k; j < n; ++j) {
                    inp >> curr;
                    outp[1] << curr << ' ';
                }
                outp[1] << '\n';
            }
            for (int i = k; i < n; ++i) {
                for (int j = 0; j < k; ++j) {
                    inp >> curr;
                    outp[2] << curr << ' ';
                }
                outp[2] << '\n';
                for (int j = k; j < n; ++j) {
                    inp >> curr;
                    outp[3] << curr << ' ';
                }
                outp[3] << '\n';
            }

            for (auto& elem : outp) {
                elem.close();
            }
            inp.close();

            return filenames;
        }
        
        std::string Transpose(int n, std::string filename) {
            std::ifstream inp(filename);

            if (n <= small_size_) {
               return TransposeSmall(n, inp);
            }

            int k = n >> 1;

            std::vector<std::string> partial = SplitIntoFiles(n, inp);

            std::swap(partial[1], partial[2]);

            std::vector<std::string> transposed(4);
            for (int i = 0; i < 4; ++i) {
                transposed[i] = Transpose(k, partial[i]);
            }

            ++cnt_;
            std::string result = templ_ + std::to_string(cnt_) + ".txt";
            std::ofstream outp(result);

            std::vector<std::ifstream> inp_t(4);
            for (int i = 0; i < 4; ++i) {
                inp_t[i] = std::ifstream(transposed[i]);
            }

            if (n & 1) {

                std::vector<std::ifstream> inp_help(2);
                for (int i = 0; i < 2; ++i) {
                    inp_help[i] = std::ifstream(partial[4 + i]);
                }

                int curr;

                for (int i = 0; i < k; ++i) {
                    for (int j = 0; j < k; ++j) {
                        inp_t[0] >> curr;
                        outp << curr << ' ';
                    }
                    inp_help[1] >> curr;
                    outp << curr << ' ';
                    for (int j = k + 1; j < n; ++j) {
                        inp_t[1] >> curr;
                        outp << curr << ' ';
                    }
                    outp << '\n';
                }

                for (int j = 0; j < n; ++j) {
                    inp_help[0] >> curr;
                    outp << curr << ' ';
                }
                outp << '\n';

                for (int i = k + 1; i < n; ++i) {
                    for (int j = 0; j < k; ++j) {
                        inp_t[2] >> curr;
                        outp << curr << ' ';
                    }
                    inp_help[1] >> curr;
                    outp << curr << ' ';
                    for (int j = k + 1; j < n; ++j) {
                        inp_t[3] >> curr;
                        outp << curr << ' ';
                    }
                    outp << '\n';
                }

                for (auto& elem : inp_t) {
                    elem.close();
                }

                for (auto& elem : inp_help) {
                    elem.close();
                }

                outp.close();

                return result;
            }

            int curr;
            for (int i = 0; i < k; ++i) {
                for (int j = 0; j < k; ++j) {
                    inp_t[0] >> curr;
                    outp << curr << ' ';
                }
                for (int j = k; j < n; ++j) {
                    inp_t[1] >> curr;
                    outp << curr << ' ';
                }
                outp << '\n';
            }

            for (int i = k; i < n; ++i) {
                for (int j = 0; j < k; ++j) {
                    inp_t[2] >> curr;
                    outp << curr << ' ';
                }
                for (int j = k; j < n; ++j) {
                    inp_t[3] >> curr;
                    outp << curr << ' ';
                }
                outp << '\n';
            }
            
            for (auto& elem : inp_t) {
                elem.close();
            }

            outp.close();

            return result;
        }

    public:

        MatrixTransposer(int small_size, int side, std::string input, std::string output): small_size_(small_size), side_(side),
                                                                           input_file_(input), output_file_(output) {
            CleanUp();
                                                                           }
        void GetTransposed() {
            std::filesystem::create_directory("./tmp");
            std::string filename = Transpose(side_, input_file_);
            std::ifstream inp(filename);
            std::ofstream outp(output_file_);

            int curr;
            for (int i = 0; i < side_; ++i) {
                for (int j = 0; j < side_; ++j) {
                    inp >> curr;
                    outp << curr << ' ';
                }
                outp << '\n';
            } 

            inp.close();
            outp.close();
            std::filesystem::remove_all("./tmp");
        }
        
        void CleanUp() {
            std::filesystem::remove_all("./tmp");
        }
        
};