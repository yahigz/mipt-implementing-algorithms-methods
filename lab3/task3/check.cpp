#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <climits>
#include <sstream>
#include <iomanip>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

struct TestCase {
    int n;
    vector<int> processing_times;
    vector<int> due_times;
};

int calculate_penalty(const TestCase& tc, const vector<int>& order) {
    int time = 0;
    int penalty = 0;
    for (int job_id : order) {
        time += tc.processing_times[job_id];
        penalty += max(0, time - tc.due_times[job_id]);
    }
    return penalty;
}

TestCase read_test_case(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        exit(1);
    }

    TestCase tc;
    file >> tc.n;
    
    tc.processing_times.resize(tc.n);
    for (int i = 0; i < tc.n; ++i) {
        file >> tc.processing_times[i];
    }
    
    tc.due_times.resize(tc.n);
    for (int i = 0; i < tc.n; ++i) {
        file >> tc.due_times[i];
    }
    
    return tc;
}

int brute_force_solution(const TestCase& tc) {
    vector<int> order(tc.n);
    for (int i = 0; i < tc.n; ++i) {
        order[i] = i;
    }

    int min_penalty = INT_MAX;
    do {
        int current_penalty = calculate_penalty(tc, order);
        if (current_penalty < min_penalty) {
            min_penalty = current_penalty;
        }
    } while (next_permutation(order.begin(), order.end()));

    return min_penalty;
}

int run_a_out(const TestCase& tc) {
    int pipe_stdin[2];  // Pipe for sending data to child's stdin
    int pipe_stdout[2]; // Pipe for reading child's stdout

    if (pipe(pipe_stdin)) {
        perror("pipe stdin");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipe_stdout)) {
        perror("pipe stdout");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Child process
        // Set up stdin
        close(pipe_stdin[1]);
        dup2(pipe_stdin[0], STDIN_FILENO);
        close(pipe_stdin[0]);

        // Set up stdout
        close(pipe_stdout[0]);
        dup2(pipe_stdout[1], STDOUT_FILENO);
        close(pipe_stdout[1]);

        execl("./a.out", "./a.out", NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    } else { // Parent process
        // Write test data to child's stdin
        close(pipe_stdin[0]);
        FILE* child_stdin = fdopen(pipe_stdin[1], "w");
        
        fprintf(child_stdin, "%d\n", tc.n);
        
        for (int i = 0; i < tc.n; ++i) {
            fprintf(child_stdin, "%d ", tc.processing_times[i]);
        }
        fprintf(child_stdin, "\n");
        
        for (int i = 0; i < tc.n; ++i) {
            fprintf(child_stdin, "%d ", tc.due_times[i]);
        }
        fprintf(child_stdin, "\n");
        
        fclose(child_stdin);

        // Read child's output
        close(pipe_stdout[1]);
        char buffer[128];
        string result;
        ssize_t count;
        while ((count = read(pipe_stdout[0], buffer, sizeof(buffer)-1)) > 0) {
            buffer[count] = '\0';
            result += buffer;
        }
        close(pipe_stdout[0]);
        wait(NULL);

        // Extract the penalty from the output
        int penalty;
        stringstream ss(result);
        ss >> penalty;
        return penalty;
    }
}

void run_test(const string& test_file) {
    TestCase tc = read_test_case(test_file);
    
    // Skip tests with too many jobs for brute force
    if (tc.n > 10) {
        cout << "Test: " << test_file << " (SKIPPED - too large for brute force, n = " << tc.n << ")" << endl;
        cout << "----------------------------------------" << endl;
        return;
    }

    int optimal_penalty = brute_force_solution(tc);
    int a_out_penalty = run_a_out(tc);

    cout << "Test: " << test_file << " (n = " << tc.n << ")" << endl;
    cout << "Optimal penalty (brute force): " << optimal_penalty << endl;
    cout << "a.out penalty: " << a_out_penalty << endl;
    
    if (optimal_penalty == a_out_penalty) {
        cout << "RESULT: CORRECT" << endl;
    } else {
        cout << "RESULT: INCORRECT (difference: " 
             << a_out_penalty - optimal_penalty << ")" << endl;
    }
    cout << "----------------------------------------" << endl;
}

int main() {
    const string test_dir = "tests/";
    const int num_tests = 30;
    
    for (int i = 0; i < num_tests; ++i) {
        stringstream filename;
        filename << test_dir << "random" << setw(2) << setfill('0') << i << ".txt";
        run_test(filename.str());
    }

    return 0;
}