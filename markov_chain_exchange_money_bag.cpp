#include <iostream>
#include <vector>
#include <iomanip>
#include <random>    // 新增：用於隨機數生成
#include <algorithm> // 新增：用於 std::swap

// 定義矩陣和向量的類型別名，方便使用
using matrix = std::vector<std::vector<double>>;
using vector = std::vector<double>;

// 函式：印出向量
void print_vector(const vector& vec) {
    std::cout << "[ ";
    for (double val : vec) {
        std::cout << std::fixed << std::setprecision(5) << val << " ";
    }
    std::cout << "]" << std::endl;
}

// 函式：印出矩陣
void print_matrix(const matrix& mat) {
    std::cout << "轉移矩陣 P:" << std::endl;
    for (const auto& row : mat) {
        std::cout << "[ ";
        for (double val : row) {
            std::cout << std::fixed << std::setprecision(5) << val << " ";
        }
        std::cout << "]" << std::endl;
    }
}

// 函式：計算向量與矩陣的乘法 (pi_next = pi_current * P)
vector multiply_vector_matrix(const vector& vec, const matrix& mat) {
    int n = vec.size();
    vector result(n, 0.0);
    for (int j = 0; j < n; ++j) {
        for (int i = 0; i < n; ++i) {
            result[j] += vec[i] * mat[i][j];
        }
    }
    return result;
}

int main() {
    // =======================================================
    // ==           部分一：解析解 (Analytical Solution)        ==
    // =======================================================
    std::cout << "--- 解析解計算 ---" << std::endl;

    // 1. 定義問題參數
    const int NUM_STATES = 5;
    const int NUM_STEPS = 3;

    vector state_values = { 2.0, 4.0, 6.0, 8.0, 10.0 };

    matrix P = {
        {0.0,   1.0 / 3.0, 2.0 / 3.0, 0.0,     0.0    },
        {1.0 / 6.0, 1.0 / 6.0, 1.0 / 3.0, 1.0 / 3.0, 0.0    },
        {1.0 / 6.0, 1.0 / 6.0, 1.0 / 3.0, 1.0 / 6.0, 1.0 / 6.0},
        {0.0,   1.0 / 3.0, 1.0 / 3.0, 1.0 / 6.0, 1.0 / 6.0},
        {0.0,   0.0,   2.0 / 3.0, 1.0 / 3.0, 0.0    }
    };

    vector pi = { 0.0, 0.0, 1.0, 0.0, 0.0 };

    print_matrix(P);
    std::cout << "------------------------------------------" << std::endl;

    // 2. 進行 n 步轉移計算
    for (int step = 1; step <= NUM_STEPS; ++step) {
        pi = multiply_vector_matrix(pi, P);
    }
    std::cout << "第 " << NUM_STEPS << " 次交換後的最終機率分佈:" << std::endl;
    print_vector(pi);
    std::cout << "------------------------------------------" << std::endl;

    // 3. 計算最終的期望值
    double analytical_result = 0.0;
    for (int i = 0; i < NUM_STATES; ++i) {
        analytical_result += pi[i] * state_values[i];
    }

    std::cout << "【解析解結果】" << std::endl;
    std::cout << "經過 " << NUM_STEPS << " 次交換後，A袋金額的理論期望值為: "
        << std::fixed << std::setprecision(6) << analytical_result << std::endl;

    std::cout << "\n\n"; // 增加間隔

    // =======================================================
    // ==        部分二：蒙地卡羅模擬 (Monte Carlo)           ==
    // =======================================================
    std::cout << "--- 蒙地卡羅模擬驗證 ---" << std::endl;

    const int NUM_TRIALS = 500000; // 模擬次數，可以增加以提高精度
    double total_sum_from_trials = 0.0;

    // 設置高品質的隨機數生成器
    std::random_device rd; // 用於生成種子
    std::mt19937 rng(rd()); // 使用 Mersenne Twister 引擎
    std::uniform_int_distribution<int> pick_from_A(0, 1); // 隨機選 A 袋的索引 (0 或 1)
    std::uniform_int_distribution<int> pick_from_B(0, 2); // 隨機選 B 袋的索引 (0, 1 或 2)

    for (int i = 0; i < NUM_TRIALS; ++i) {
        // 為每次模擬重置袋子狀態
        std::vector<int> bagA = { 1, 5 };
        std::vector<int> bagB = { 1, 3, 5 };

        // 模擬 3 次交換
        for (int step = 0; step < NUM_STEPS; ++step) {
            int indexA = pick_from_A(rng);
            int indexB = pick_from_B(rng);
            std::swap(bagA[indexA], bagB[indexB]);
        }

        // 將本次模擬的結果加到總和中
        total_sum_from_trials += (bagA[0] + bagA[1]);
    }

    // 計算平均值
    double monte_carlo_result = total_sum_from_trials / NUM_TRIALS;

    std::cout << "【模擬解結果】" << std::endl;
    std::cout << "執行了 " << NUM_TRIALS << " 次模擬後，A袋金額的估計期望值為: "
        << std::fixed << std::setprecision(6) << monte_carlo_result << std::endl;

    return 0;
}