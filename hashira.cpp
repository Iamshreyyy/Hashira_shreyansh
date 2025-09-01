
#include <bits/stdc++.h>
using namespace std;


struct BigInt {
    bool neg;           
    string digits;      

    BigInt(long long num = 0) { *this = num; }
    BigInt(const string &s) { *this = s; }

    BigInt& operator=(long long num) {
        neg = (num < 0);
        if (num < 0) num = -num;
        digits.clear();
        if (num == 0) digits = "0";
        while (num > 0) {
            digits.push_back(num % 10 + '0');
            num /= 10;
        }
        trim();
        return *this;
    }

    BigInt& operator=(const string &s) {
        neg = false;
        string t = s;
        if (!t.empty() && t[0] == '-') {
            neg = true;
            t = t.substr(1);
        }
        digits = t;
        reverse(digits.begin(), digits.end());
        trim();
        return *this;
    }

    void trim() {
        while (digits.size() > 1 && digits.back() == '0') digits.pop_back();
        if (digits == "0") neg = false;
    }

    string str() const {
        string s = digits;
        reverse(s.begin(), s.end());
        if (neg && s != "0") s = "-" + s;
        return s;
    }


    static bool absLess(const BigInt &a, const BigInt &b) {
        if (a.digits.size() != b.digits.size()) return a.digits.size() < b.digits.size();
        for (int i = a.digits.size() - 1; i >= 0; i--) {
            if (a.digits[i] != b.digits[i]) return a.digits[i] < b.digits[i];
        }
        return false;
    }


    static BigInt absAdd(const BigInt &a, const BigInt &b) {
        BigInt res;
        res.neg = false;
        res.digits.clear();
        int carry = 0;
        for (size_t i = 0; i < max(a.digits.size(), b.digits.size()) || carry; i++) {
            int sum = carry;
            if (i < a.digits.size()) sum += a.digits[i] - '0';
            if (i < b.digits.size()) sum += b.digits[i] - '0';
            res.digits.push_back(sum % 10 + '0');
            carry = sum / 10;
        }
        res.trim();
        return res;
    }


    static BigInt absSub(const BigInt &a, const BigInt &b) {
        BigInt res;
        res.neg = false;
        res.digits.clear();
        int carry = 0;
        for (size_t i = 0; i < a.digits.size(); i++) {
            int diff = (a.digits[i] - '0') - carry - (i < b.digits.size() ? b.digits[i] - '0' : 0);
            if (diff < 0) { diff += 10; carry = 1; }
            else carry = 0;
            res.digits.push_back(diff + '0');
        }
        res.trim();
        return res;
    }


    BigInt operator+(const BigInt &b) const {
        if (neg == b.neg) {
            BigInt res = absAdd(*this, b);
            res.neg = neg;
            return res;
        } else {
            if (absLess(*this, b)) {
                BigInt res = absSub(b, *this);
                res.neg = b.neg;
                return res;
            } else {
                BigInt res = absSub(*this, b);
                res.neg = neg;
                return res;
            }
        }
    }


    BigInt operator-(const BigInt &b) const {
        BigInt nb = b;
        nb.neg = !nb.neg;
        return *this + nb;
    }


    BigInt operator*(const BigInt &b) const {
        BigInt res;
        res.neg = (neg != b.neg);
        res.digits.assign(digits.size() + b.digits.size(), '0');
        for (size_t i = 0; i < digits.size(); i++) {
            int carry = 0;
            for (size_t j = 0; j < b.digits.size() || carry; j++) {
                long long cur = (res.digits[i + j] - '0') +
                    (digits[i] - '0') * 1LL * (j < b.digits.size() ? b.digits[j] - '0' : 0) + carry;
                res.digits[i + j] = char(cur % 10 + '0');
                carry = cur / 10;
            }
        }
        res.trim();
        if (res.digits == "0") res.neg = false;
        return res;
    }


    BigInt operator*(int b) const {
        BigInt res;
        res.neg = (neg && b != 0 && b < 0);
        int ab = abs(b);
        res.digits.assign(digits.size() + 20, '0');
        long long carry = 0;
        for (size_t i = 0; i < digits.size() || carry; i++) {
            long long cur = carry;
            if (i < digits.size()) cur += (digits[i] - '0') * 1LL * ab;
            res.digits[i] = char(cur % 10 + '0');
            carry = cur / 10;
        }
        res.trim();
        if (res.digits == "0") res.neg = false;
        return res;
    }
};




BigInt baseToDecimal(const string &value, int base) {
    BigInt result(0);
    for (char c : value) {
        int digit;
        if (isdigit(c)) digit = c - '0';
        else if (isalpha(c)) digit = tolower(c) - 'a' + 10;
        else continue;
        result = result * base + BigInt(digit);
    }
    return result;
}


vector<BigInt> multiplyPoly(const vector<BigInt> &poly, const BigInt &root) {
    int n = poly.size();
    vector<BigInt> res(n + 1, BigInt(0));
    for (int i = 0; i < n; i++) {
        res[i] = res[i] + poly[i];           // coeff for x^i
        res[i + 1] = res[i + 1] - (poly[i] * root); // coeff for x^(i+1)
    }
    return res;
}


int main(int argc, char* argv[]) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string filename = "testcase.json";
    if (argc > 1) filename = argv[1];

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Could not open " << filename << "\n";
        return 1;
    }

    string input((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());


    int n = 0, k = 0;
    {
        regex nRegex("\"n\"\\s*:\\s*(\\d+)");
        regex kRegex("\"k\"\\s*:\\s*(\\d+)");
        smatch match;
        if (regex_search(input, match, nRegex)) n = stoi(match[1]);
        if (regex_search(input, match, kRegex)) k = stoi(match[1]);
    }

    vector<BigInt> roots;
    regex entryRegex("\"(\\d+)\"\\s*:\\s*\\{[^}]*\"base\"\\s*:\\s*\"?(\\d+)\"?[^}]*\"value\"\\s*:\\s*\"([^\"]+)\"");
    auto begin = sregex_iterator(input.begin(), input.end(), entryRegex);
    auto end = sregex_iterator();

    vector<pair<int, pair<int, string>>> entries;
    for (auto it = begin; it != end; ++it) {
        int id = stoi((*it)[1]);
        int base = stoi((*it)[2]);
        string value = (*it)[3];
        entries.push_back({id, {base, value}});
    }

    sort(entries.begin(), entries.end());

    for (int i = 0; i < k && i < (int)entries.size(); i++) {
        BigInt root = baseToDecimal(entries[i].second.second, entries[i].second.first);
        roots.push_back(root);
    }

    vector<BigInt> poly = {BigInt(1)};
    for (auto &r : roots) {
        poly = multiplyPoly(poly, r);
    }

    cout << "Polynomial Coefficients:\n";
    for (size_t i = 0; i < poly.size(); i++) {
        cout << poly[i].str();
        if (i + 1 != poly.size()) cout << " ";
    }
    cout << "\n";

    return 0;
}
