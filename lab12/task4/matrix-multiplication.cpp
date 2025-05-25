#pragma once
#include <iostream>
#include <vector>
#include <filesystem>
#include "matrix-transposition.cpp"

class MatrixMultiplier{
    private:

        int small_size_ = 32;

        int side_;

        std::string left_input_file_;
        std::string right_input_file_;
        std::string output_file_;

        std::string templ_ = "./tmp-multi/partial";

        int cnt_ = 0;

        std::vector<std::string> SplitIntoFiles(int n, std::ifstream& inp) {
            int k = (n >> 1);
            
            std::vector<std::string> filenames;
            std::vector<std::ofstream> outp;

            for (int i = 0; i < 4; ++i) {
                std::string curr = NewFile();
                filenames.push_back(curr);
                outp.push_back(std::ofstream(curr));
            }

            if (n & 1) {
                for (int i = 0; i < 2; ++i) {
                    std::string curr = NewFile();
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

        std::string NewFile() {
            ++cnt_;
            std::string curr = templ_ + std::to_string(cnt_) + ".txt";
            return curr;
        }

        std::string MultiplicateSmall(int n, std::ifstream& left_inp, std::ifstream& right_inp) {
            std::vector<std::vector<int>> a(n, std::vector<int>(n));
            std::vector<std::vector<int>> b(n, std::vector<int>(n));

            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    left_inp >> a[i][j];
                    right_inp >> b[i][j];
                }
            }

            std::vector<std::vector<int>> res(n, std::vector<int>(n));  

            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    for (int k = 0; k < n; ++k) {
                        res[i][j] += a[i][k] * b[j][k];
                    }
                }
            }

            std::string curr = NewFile();
            std::ofstream outp(curr);

            for (int i = 0; i < n; ++i) {
                for (int j = 0 ; j < n; ++j) {
                    outp << res[i][j] << ' ';
                }
                outp << '\n';
            }

            left_inp.close();
            right_inp.close();

            outp.close();
            return curr;
        }

        std::string Sum(int n, std::string left_filename, std::string right_filename) {
            std::ifstream left_inp(left_filename);
            std::ifstream right_inp(right_filename);

            std::string curr = NewFile(); 
            std::ofstream outp(curr);
            
            int tmp;
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    right_inp >> tmp;
                    int tmp2;
                    left_inp >> tmp2;
                    outp << tmp + tmp2 << ' ';
                }
                outp << '\n';
            }

            left_inp.close();
            right_inp.close();
            
            outp.close();
            return curr;
        }

        std::string Multiplicate(int n, std::string left_filename, std::string right_filename) {
            std::ifstream left_inp(left_filename);
            std::ifstream right_inp(right_filename);

            if (n <= small_size_) {
                return MultiplicateSmall(n, left_inp, right_inp);
            }

            int k = n >> 1;

            std::vector<std::string> left_splitted = SplitIntoFiles(n, left_inp);
            std::vector<std::string> right_splitted = SplitIntoFiles(n, right_inp);

            std::vector<std::string> res = {
                Sum(k, Multiplicate(k, left_splitted[0], right_splitted[0]), Multiplicate(k, left_splitted[1], right_splitted[1])),
                Sum(k, Multiplicate(k, left_splitted[0], right_splitted[2]), Multiplicate(k, left_splitted[1], right_splitted[3])),
                Sum(k, Multiplicate(k, left_splitted[2], right_splitted[0]), Multiplicate(k, left_splitted[3], right_splitted[1])),
                Sum(k, Multiplicate(k, left_splitted[2], right_splitted[2]), Multiplicate(k, left_splitted[3], right_splitted[3]))
            };

            if (n & 1) {
                std::vector<std::string> new_res;
                for (int i = 0; i < 4; ++i) {
                    new_res.push_back(NewFile());
                }
                std::vector<std::ofstream> new_res_outp;
                for (auto elem : new_res) {
                    new_res_outp.push_back(std::ofstream(elem));
                }
                
                std::vector<std::ifstream> res_inp;
                for (auto elem : res) {
                    res_inp.push_back(std::ifstream(elem));
                }

                int tmp1, tmp2, tmp3;

                std::ifstream left_col(left_splitted[4]);
                std::ifstream right_col;

                for (int i = 0; i < k; ++i) {
                    left_col >> tmp2;
                    right_col = std::ifstream(right_splitted[4]);
                    for (int j = 0; j < k; ++j) {
                        res_inp[0] >> tmp1;
                        right_col >> tmp3;
                        new_res_outp[0] << tmp1 + tmp2 * tmp3 << ' ';
                    }
                    right_col.close();
                    new_res_outp[0] << '\n';
                }

                left_col >> tmp2;

                int middle_left = tmp2;
                int middle_right;
                
                for (int i = k + 1; i < n; ++i) {
                    left_col >> tmp2;
                    right_col = std::ifstream(right_splitted[4]);
                    for (int j = 0; j < k + 1; ++j) {
                        right_col >> tmp3;
                    }
                    for (int j = k + 1; j < n; ++j) {
                        res_inp[3] >> tmp1;
                        right_col >> tmp3;
                        new_res_outp[3] << tmp1 + tmp2 * tmp3 << ' ';
                    }
                    right_col.close();
                    new_res_outp[3] << '\n';
                }

                left_col.close();

                left_col = std::ifstream(left_splitted[4]);

                for (int i = 0; i < k; ++i) {
                    left_col >> tmp2;
                    right_col = std::ifstream(right_splitted[4]);
                    for (int j = 0; j < k + 1; ++j) {
                        right_col >> tmp3;
                    }
                    middle_right = tmp3;
                    for (int j = k + 1; j < n; ++j) {
                        res_inp[1] >> tmp1;
                        right_col >> tmp3;
                        new_res_outp[1] << tmp1 + tmp2 * tmp3 << ' ';
                    }
                    right_col.close();
                    new_res_outp[1] << '\n';
                }

                left_col >> tmp2;

                for (int i = k + 1; i < n; ++i) {
                    left_col >> tmp2;
                    right_col = std::ifstream(right_splitted[4]);
                    for (int j = 0; j < k; ++j) {
                        res_inp[2] >> tmp1;
                        right_col >> tmp3;
                        new_res_outp[2] << tmp1 + tmp2 * tmp3 << ' ';
                    }
                    right_col.close();
                    new_res_outp[2] << '\n';
                }

                left_col.close();

                for (auto& elem : new_res_outp) {
                    elem.close();
                }
                for (auto& elem : res_inp) {
                    elem.close();
                }

                std::string res_col = NewFile();
                std::string res_row = NewFile();
                std::ofstream outp_col(res_col);
                std::ofstream outp_row(res_row);

                std::ifstream a(left_splitted[0]);
                std::ifstream b(left_splitted[1]);
                std::ifstream c(left_splitted[2]);
                std::ifstream d(left_splitted[3]);

                left_col = std::ifstream(left_splitted[4]);

                for (int i = 0; i < k; ++i) {
                    left_col >> tmp1;
                    int res = tmp1 * middle_right;
                    std::ifstream right_row = std::ifstream(right_splitted[5]);
                    for (int j = 0; j < k; ++j) {
                        a >> tmp2;
                        right_row >> tmp3;
                        res += tmp2 * tmp3;
                    }
                    for (int j = k + 1; j < n; ++j) {
                        b >> tmp2;
                        right_row >> tmp3;
                        res += tmp2 * tmp3;
                    }
                    right_row.close();
                    outp_col << res << ' ';
                }

                {
                    left_col >> tmp1;
                    int res = middle_left * middle_right;
                    std::ifstream left_row = std::ifstream(left_splitted[5]);
                    std::ifstream right_row = std::ifstream(right_splitted[5]);
                    for (int i = 0; i < n - 1; ++i) {
                        left_row >> tmp2;
                        right_row >> tmp3;
                        res += tmp2 * tmp3;
                    }
                    left_row.close();
                    right_row.close();
                    outp_col << res << ' ';
                }

                for (int i = k + 1; i < n; ++i) {
                    left_col >> tmp1;
                    int res = tmp1 * middle_right;
                    std::ifstream right_row = std::ifstream(right_splitted[5]);
                    for (int j = 0; j < k; ++j) {
                        c >> tmp2;
                        right_row >> tmp3;
                        res += tmp2 * tmp3;
                    }
                    for (int j = k + 1; j < n; ++j) {
                        d >> tmp2;
                        right_row >> tmp3;
                        res += tmp2 * tmp3;
                    }
                    right_row.close();
                    outp_col << res << ' ';
                }

                left_col.close();

                a.close();
                b.close();
                c.close();
                d.close();

                std::ifstream e(right_splitted[0]);
                std::ifstream g(right_splitted[1]);
                std::ifstream f(right_splitted[2]);
                std::ifstream h(right_splitted[3]);

                right_col = std::ifstream(right_splitted[4]);

                for (int i = 0; i < k; ++i) {
                    right_col >> tmp1;
                    int res = tmp1 * middle_left;
                    std::ifstream left_row(left_splitted[5]);
                    for (int j = 0; j < k; ++j) {
                        e >> tmp2;
                        left_row >> tmp3;
                        res += tmp2 * tmp3;
                    }
                    for (int j = k + 1; j < n; ++j) {
                        g >> tmp2;
                        left_row >> tmp3;
                        res += tmp2 * tmp3;
                    }
                    left_row.close();
                    outp_row << res << ' ';
                }

                right_col >> tmp1;

                for (int i = k + 1; i < n; ++i) {
                    right_col >> tmp1;
                    int res = tmp1 * middle_left;
                    std::ifstream left_row(left_splitted[5]);
                    for (int j = 0; j < k; ++j) {
                        f >> tmp2;
                        left_row >> tmp3;
                        res += tmp2 * tmp3;
                    }
                    for (int j = k + 1; j < n; ++j) {
                        h >> tmp2;
                        left_row >> tmp3;
                        res += tmp2 * tmp3;
                    }
                    left_row.close();
                    outp_row << res << ' ';
                }

                right_col.close();

                e.close();
                g.close();
                f.close();
                h.close();

                outp_col.close();
                outp_row.close();

                std::string outp_file = NewFile();
                std::ofstream outp(outp_file);

                std::vector<std::ifstream> inp_t;
                for (auto elem : new_res) {
                    inp_t.push_back(std::ifstream(elem));
                }
                std::vector<std::ifstream> inp_help;
                inp_help.push_back(std::ifstream(res_col));
                inp_help.push_back(std::ifstream(res_row));
                
                int curr;

                for (int i = 0; i < k; ++i) {
                    for (int j = 0; j < k; ++j) {
                        inp_t[0] >> curr;
                        outp << curr << ' ';
                    }
                    inp_help[0] >> curr;
                    outp << curr << ' ';
                    for (int j = k + 1; j < n; ++j) {
                        inp_t[1] >> curr;
                        outp << curr << ' ';
                    }
                    outp << '\n';
                }

                for (int j = 0; j < k; ++j) {
                    inp_help[1] >> curr;
                    outp << curr << ' ';
                }
                inp_help[0] >> curr;
                outp << curr << ' ';
                for (int j = k + 1; j < n; ++j) {
                    inp_help[1] >> curr;
                    outp << curr << ' ';
                }
                outp << '\n';

                for (int i = k + 1; i < n; ++i) {
                    for (int j = 0; j < k; ++j) {
                        inp_t[2] >> curr;
                        outp << curr << ' ';
                    }
                    inp_help[0] >> curr;
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
                return outp_file;
            }   

            std::vector<std::ifstream> res_inp;
            for (auto elem : res) {
                res_inp.push_back(std::ifstream(elem));
            } 
            
            std::string curr = NewFile();
            std::ofstream outp(curr);

            int tmp;

            for (int i = 0; i < k; ++i) {
                for (int j = 0; j < k; ++j) {
                    res_inp[0] >> tmp;
                    outp << tmp << ' ';
                }
                for (int j = k; j < n; ++j) {
                    res_inp[1] >> tmp;
                    outp << tmp << ' ';
                }
                outp << '\n';
            }
            
            for (int i = k; i < n; ++i) {
                for (int j = 0; j < k; ++j) {
                    res_inp[2] >> tmp;
                    outp << tmp << ' ';
                }
                for (int j = k; j < n; ++j) {
                    res_inp[3] >> tmp;
                    outp << tmp << ' ';
                }
                outp << '\n';
            }
            
            for (auto& elem : res_inp) {
                elem.close();
            }
            outp.close();
            return curr;
        }

    public:
        MatrixMultiplier(int small_size, int side, std::string left_input, std::string right_input, std::string output): small_size_(small_size),
            side_(side), left_input_file_(left_input), right_input_file_(right_input), output_file_(output) {
                CleanUp();
            }

        void CleanUp() {
            std::filesystem::remove_all("./tmp-multi");
        }

        void GetMultiplication() {
            std::filesystem::create_directory("./tmp-multi");

            std::string right_transposed = "./tmp-multi/right";

            MatrixTransposer(small_size_, side_, right_input_file_, right_transposed).GetTransposed();

            std::string result = Multiplicate(side_, left_input_file_, right_transposed);
            std::ifstream inp(result);
            std::ofstream outp(output_file_);

            int tmp;

            for (int i = 0; i < side_; ++i) {
                for (int j = 0; j < side_; ++j) {
                    inp >> tmp;
                    outp << tmp << ' ';
                }
                outp << '\n';
            }

            inp.close();
            outp.close();

            std::filesystem::remove_all("./tmp-multi");
        }
};

int main() {
    MatrixMultiplier mm(8, 35, "./multiplication-tests/left.1.txt", "./multiplication-tests/right.1.txt", "./multiplication-tests/1.a.txt");
    mm.GetMultiplication();
}