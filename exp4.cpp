#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <cmath>
#include <functional>
#include <iomanip>

struct BBox {
    float x1, y1, x2, y2, score;

    // 默认构造函数（必须！）
    BBox() : x1(0), y1(0), x2(0), y2(0), score(0) {}

    // 带参构造函数
    BBox(float a, float b, float c, float d, float s)
        : x1(a), y1(b), x2(c), y2(d), score(s) {}
};

// ==============================
// 排序算法实现
// ==============================

void quickSort(std::vector<BBox>& arr, int low, int high) {
    if (low >= high) return;
    float pivot = arr[high].score;
    int i = low - 1;
    for (int j = low; j < high; ++j) {
        if (arr[j].score >= pivot) {
            ++i;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);
    int pi = i + 1;
    quickSort(arr, low, pi - 1);
    quickSort(arr, pi + 1, high);
}
void quickSortWrapper(std::vector<BBox>& arr) {
    if (!arr.empty()) quickSort(arr, 0, arr.size() - 1);
}

void merge(std::vector<BBox>& arr, int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;
    std::vector<BBox> L(n1), R(n2);
    for (int i = 0; i < n1; ++i) L[i] = arr[l + i];
    for (int j = 0; j < n2; ++j) R[j] = arr[m + 1 + j];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (L[i].score >= R[j].score) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}
void mergeSort(std::vector<BBox>& arr, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}
void mergeSortWrapper(std::vector<BBox>& arr) {
    if (!arr.empty()) mergeSort(arr, 0, arr.size() - 1);
}

void heapify(std::vector<BBox>& arr, int n, int i) {
    int largest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    if (l < n && arr[l].score > arr[largest].score) largest = l;
    if (r < n && arr[r].score > arr[largest].score) largest = r;
    if (largest != i) {
        std::swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}
void heapSort(std::vector<BBox>& arr) {
    int n = arr.size();
    for (int i = n / 2 - 1; i >= 0; --i)
        heapify(arr, n, i);
    for (int i = n - 1; i > 0; --i) {
        std::swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
    std::reverse(arr.begin(), arr.end());
}

void bubbleSort(std::vector<BBox>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (arr[j].score < arr[j + 1].score) {
                std::swap(arr[j], arr[j + 1]);
            }
        }
    }
}

// ==============================
// 边界框生成函数
// ==============================

std::vector<BBox> generateRandomBoxes(int n) {
    std::vector<BBox> boxes;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> coord(0.0f, 900.0f);
    std::uniform_real_distribution<float> size(10.0f, 100.0f);
    std::uniform_real_distribution<float> score(0.0f, 1.0f);

    for (int i = 0; i < n; ++i) {
        float x1 = coord(gen);
        float y1 = coord(gen);
        float w = size(gen);
        float h = size(gen);
        float x2 = x1 + w;
        float y2 = y1 + h;
        boxes.emplace_back(x1, y1, x2, y2, score(gen));
    }
    return boxes;
}

std::vector<BBox> generateClusteredBoxes(int n, int clusters) {
    std::vector<BBox> boxes;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> center(200.0f, 800.0f);
    std::uniform_real_distribution<float> offset(-50.0f, 50.0f);
    std::uniform_real_distribution<float> size(10.0f, 60.0f);
    std::uniform_real_distribution<float> score(0.0f, 1.0f);

    std::vector<std::pair<float, float>> centers;
    for (int i = 0; i < clusters; ++i) {
        centers.push_back(std::make_pair(center(gen), center(gen)));
    }

    int perCluster = n / clusters;
    int remainder = n % clusters;

    for (int c = 0; c < clusters; ++c) {
        int count = perCluster + (c < remainder ? 1 : 0);
        float cx = centers[c].first;
        float cy = centers[c].second;
        for (int i = 0; i < count; ++i) {
            float x1 = cx + offset(gen);
            float y1 = cy + offset(gen);
            float w = size(gen);
            float h = size(gen);
            float x2 = x1 + w;
            float y2 = y1 + h;
            x1 = std::max(0.0f, std::min(1000.0f, x1));
            y1 = std::max(0.0f, std::min(1000.0f, y1));
            x2 = std::max(0.0f, std::min(1000.0f, x2));
            y2 = std::max(0.0f, std::min(1000.0f, y2));
            if (x1 > x2) std::swap(x1, x2);
            if (y1 > y2) std::swap(y1, y2);
            boxes.emplace_back(x1, y1, x2, y2, score(gen));
        }
    }
    return boxes;
}

// ==============================
// NMS 和 IoU
// ==============================

float computeIoU(const BBox& a, const BBox& b) {
    float inter_x1 = std::max(a.x1, b.x1);
    float inter_y1 = std::max(a.y1, b.y1);
    float inter_x2 = std::min(a.x2, b.x2);
    float inter_y2 = std::min(a.y2, b.y2);

    if (inter_x1 >= inter_x2 || inter_y1 >= inter_y2)
        return 0.0f;

    float inter_area = (inter_x2 - inter_x1) * (inter_y2 - inter_y1);
    float area_a = (a.x2 - a.x1) * (a.y2 - a.y1);
    float area_b = (b.x2 - b.x1) * (b.y2 - b.y1);
    float union_area = area_a + area_b - inter_area;
    return inter_area / union_area;
}

std::vector<BBox> nms(const std::vector<BBox>& boxes, float iou_threshold = 0.5f) {
    if (boxes.empty()) return {};
    std::vector<BBox> sorted_boxes = boxes;
    std::vector<bool> suppressed(sorted_boxes.size(), false);
    std::vector<BBox> result;

    for (size_t i = 0; i < sorted_boxes.size(); ++i) {
        if (suppressed[i]) continue;
        result.push_back(sorted_boxes[i]);
        for (size_t j = i + 1; j < sorted_boxes.size(); ++j) {
            if (suppressed[j]) continue;
            if (computeIoU(sorted_boxes[i], sorted_boxes[j]) > iou_threshold) {
                suppressed[j] = true;
            }
        }
    }
    return result;
}

// ==============================
// 测试框架
// ==============================

using SortFunc = std::function<void(std::vector<BBox>&)>;

struct TestCase {
    std::string name;
    std::function<std::vector<BBox>(int)> generator;
    TestCase(const std::string& n, std::function<std::vector<BBox>(int)> g)
        : name(n), generator(g) {}
};

struct Algorithm {
    std::string name;
    SortFunc sort_func;
    Algorithm(const std::string& n, SortFunc f)
        : name(n), sort_func(f) {}
};

void runExperiment(const std::vector<TestCase>& test_cases,
                   const std::vector<Algorithm>& algorithms,
                   const std::vector<int>& sizes) {
    const float iou_thresh = 0.5f;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "=== NMS Performance Comparison ===\n\n";

    for (int n : sizes) {
        std::cout << ">>> Testing with " << n << " bounding boxes:\n";
        for (const auto& tc : test_cases) {
            std::cout << "  Distribution: " << tc.name << "\n";
            auto boxes = tc.generator(n);

            for (const auto& algo : algorithms) {
                auto boxes_copy = boxes;
                auto start = std::chrono::high_resolution_clock::now();

                algo.sort_func(boxes_copy);
                auto result = nms(boxes_copy, iou_thresh);

                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
                double time_ms = duration / 1000.0;

                std::cout << "    " << std::setw(12) << algo.name << ": "
                          << std::setw(8) << time_ms << " ms\n";
            }
            std::cout << "\n";
        }
    }
}

// ==============================
// 主函数
// ==============================

int main() {
    // 使用 lambda 包装 clustered 生成器，使其只接受一个 int 参数
    auto clusteredWrapper = [](int n) -> std::vector<BBox> {
        return generateClusteredBoxes(n, 5);
    };

    std::vector<TestCase> test_cases;
    test_cases.push_back(TestCase("Random", generateRandomBoxes));
    test_cases.push_back(TestCase("Clustered", clusteredWrapper));

    std::vector<Algorithm> algorithms;
    algorithms.push_back(Algorithm("QuickSort", quickSortWrapper));
    algorithms.push_back(Algorithm("MergeSort", mergeSortWrapper));
    algorithms.push_back(Algorithm("HeapSort", heapSort));
    algorithms.push_back(Algorithm("BubbleSort", bubbleSort));

    std::vector<int> sizes = {100, 500, 1000}; // 可扩展，但 BubbleSort 在 10000 时极慢

    runExperiment(test_cases, algorithms, sizes);

    std::cout << "Note: BubbleSort is very slow for large N.\n";
    return 0;
}
