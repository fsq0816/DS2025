#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <cctype>
#include <cstring>
#include <string>

using namespace std;

// ========================
// 1. Bitmap 类（精简修复版）
// ========================
typedef int Rank;

class Bitmap {
private:
    unsigned char* M;
    Rank N, _sz;

protected:
    void init(Rank n) {
        M = new unsigned char[N = (n + 7) / 8];
        memset(M, 0, N);
        _sz = 0;
    }

public:
    // 默认构造：创建能存 n 位的位图
    Bitmap(Rank n = 8) { init(n); }

    ~Bitmap() {
        delete[] M;
        M = nullptr;
        _sz = 0;
    }

    Rank size() const { return _sz; }

    // 自动扩展容量
    void expand(Rank k) {
        if (k < 8 * N) return;
        Rank oldN = N;
        unsigned char* oldM = M;
        init(2 * (k + 1)); // 扩容至至少 k+1 位
        memcpy(M, oldM, oldN);
        delete[] oldM;
    }

    void set(Rank k) {
        expand(k);
        _sz++;
        M[k >> 3] |= (0x80 >> (k & 0x07));
    }

    void clear(Rank k) {
        expand(k);
        _sz--;
        M[k >> 3] &= ~(0x80 >> (k & 0x07));
    }

    bool test(Rank k) {
        expand(k);
        return M[k >> 3] & (0x80 >> (k & 0x07));
    }

    // 将前 n 位转为 "01" 字符串（用于输出）
    char* bits2string(Rank n) {
        expand(n - 1);
        char* s = new char[n + 1];
        s[n] = '\0';
        for (Rank i = 0; i < n; ++i) {
            s[i] = test(i) ? '1' : '0';
        }
        return s;
    }
};

// ========================
// 2. 二叉树节点结构
// ========================
struct BinTree {
    char ch;               // 叶节点存储字符
    int freq;              // 频率
    BinTree* left;
    BinTree* right;

    BinTree(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
    BinTree(int f, BinTree* l, BinTree* r) : ch(0), freq(f), left(l), right(r) {}
};

// 优先队列比较器：小顶堆
struct Compare {
    bool operator()(BinTree* a, BinTree* b) {
        return a->freq > b->freq;
    }
};

// ========================
// 3. 全局编码表
// ========================
unordered_map<char, Bitmap*> huffTable;

// 递归生成 Huffman 编码表
void buildCodeTable(BinTree* node, Bitmap* code, int depth) {
    if (!node) return;

    if (!node->left && !node->right) {
        // 叶子节点：保存该字符的编码
        Bitmap* copy = new Bitmap(depth);
        for (int i = 0; i < depth; ++i) {
            if (code->test(i)) copy->set(i);
        }
        huffTable[node->ch] = copy;
        return;
    }

    // 左子树：0
    code->clear(depth);
    buildCodeTable(node->left, code, depth + 1);

    // 右子树：1
    code->set(depth);
    buildCodeTable(node->right, code, depth + 1);
}

// ========================
// 4. 获取演讲文本（确保包含所有字母）
// ========================
string getSpeechText() {
    return
        "I have a dream that one day this nation will rise up and live out the true meaning of its creed: "
        "We hold these truths to be self-evident, that all men are created equal. "
        "I have a dream that one day on the red hills of Georgia, the sons of former slaves and the sons of former slave owners "
        "will be able to sit down together at the table of brotherhood. "
        "I have a dream that my four little children will one day live in a nation where they will not be judged by the color of their skin "
        "but by the content of their character. I have a dream today! "
        "abcdefghijklmnopqrstuvwxyz"; // 确保26字母都出现
}

// 统计26个字母频次（不区分大小写）
vector<int> countLetterFreq(const string& text) {
    vector<int> freq(26, 0);
    for (char c : text) {
        if (isalpha(c)) {
            freq[tolower(c) - 'a']++;
        }
    }
    return freq;
}

// ========================
// 5. 主函数
// ========================
int main() {
    // 1. 统计频次
    string speech = getSpeechText();
    vector<int> freq = countLetterFreq(speech);

    // 2. 构建叶子节点并加入优先队列
    priority_queue<BinTree*, vector<BinTree*>, Compare> pq;
    for (int i = 0; i < 26; ++i) {
        if (freq[i] > 0) {
            pq.push(new BinTree('a' + i, freq[i]));
        }
    }

    // 3. 构建 Huffman 树
    while (pq.size() > 1) {
        BinTree* left = pq.top(); pq.pop();
        BinTree* right = pq.top(); pq.pop();
        BinTree* parent = new BinTree(left->freq + right->freq, left, right);
        pq.push(parent);
    }

    BinTree* root = pq.empty() ? nullptr : pq.top();

    // 4. 生成编码表
    if (root) {
        Bitmap* code = new Bitmap(100);
        buildCodeTable(root, code, 0);
        delete code;
    }

    // 5. 对单词进行编码
    vector<string> words = {"dream", "freedom", "king", "hope"};

    for (const string& word : words) {
        cout << "\nEncoding word: \"" << word << "\"\n";
        Bitmap* full = new Bitmap(500);
        int pos = 0;
        bool valid = true;

        for (char c : word) {
            c = tolower(c);
            if (huffTable.find(c) == huffTable.end()) {
                cout << "[Error] Letter '" << c << "' not found in Huffman table.\n";
                valid = false;
                break;
            }

            Bitmap* code = huffTable[c];
            // 计算该编码的实际长度（找最高设定位 +1）
            int len = 0;
            for (int i = 0; i < 100; ++i) {
                if (code->test(i)) len = i + 1;
            }

            // 拷贝到完整编码中
            for (int i = 0; i < len; ++i) {
                if (code->test(i)) {
                    full->set(pos);
                } else {
                    full->clear(pos);
                }
                pos++;
            }

            // 打印单个字母编码
            char* s = code->bits2string(len);
            cout << c << ": " << s << endl;
            delete[] s;
        }

        if (valid) {
            char* result = full->bits2string(pos);
            cout << "Full code: " << result << endl;
            delete[] result;
        }
        delete full;
    }

    // 6. 清理内存
    for (auto& kv : huffTable) {
        delete kv.second;
    }
    // 注意：BinTree 节点未释放（作业场景可接受；实际项目应递归 delete）

    return 0;
}
