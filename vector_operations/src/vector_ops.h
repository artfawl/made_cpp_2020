#pragma once
#include <vector>
#include <iostream>
#include <cmath>


namespace task {
    const double ERROR = 10e-6;

    using std::vector;

    std::ostream& operator<<(std::ostream& os, const vector<double>& obj) {
        if (obj.size() > 0) {
            for (int i = 0; i < obj.size() - 1; ++i)
                os << obj[i] << " ";
            os << obj[obj.size() - 1] << "\n";
        }
        return os;
    }

    std::istream& operator>>(std::istream& is, vector<double>& obj) {
        obj.clear();
        int size;
        double value;
        is >> size;
        for (int i = 0; i < size; ++i) {
            is >> value;
            obj.push_back(value);
        }
        return is;
    }

    vector<double> operator+(const vector<double>& one, const vector<double>& two) {
        if (one.size() == two.size()) {
            vector<double> result;
            for (int i = 0; i < one.size(); ++i)
                result.push_back(one[i] + two[i]);
            return result;
        }
        else
        {
            vector<double> empty;
            return empty;
        }
    }

    vector<double> operator-(const vector<double>& one, const vector<double>& two) {
        if (one.size() == two.size()) {
            vector<double> result;
            for (int i = 0; i < one.size(); ++i)
                result.push_back(one[i] - two[i]);
            return result;
        }
        else
        {
            vector<double> empty;
            return empty;
        }
    }

    vector<double> operator+(const vector<double>& obj) {
        vector<double> result;
        for (int i = 0; i < obj.size(); ++i)
            result.push_back(obj[i]);
        return result;
    }

    vector<double> operator-(const vector<double>& obj) {
        vector<double> result;
        for (int i = 0; i < obj.size(); ++i)
            result.push_back(-obj[i]);
        return result;
    }

    double operator*(const vector<double>& one, const vector<double>& two) {
        if (one.size() == two.size()) {
            double result = 0;
            for (int i = 0; i < one.size(); ++i)
                result += one[i] * two[i];
            return result;
        }
        else
        {
            return -1;
        }

    }

    vector<double> operator%(const vector<double>& one, const vector<double>& two) {
        if (one.size() == two.size() && one.size() == 3) {
            vector<double> result;
            result.push_back(one[1] * two[2] - one[2] * two[1]);
            result.push_back(one[2] * two[0] - one[0] * two[2]);
            result.push_back(one[0] * two[1] - one[1] * two[0]);
            return result;
        }
        else
        {
            vector<double> empty;
            return empty;
        }
    }

    bool operator||(const vector<double>& one, const vector<double>& two) {
        if (one.size() == two.size()) {
            int i = 0;
            while (i < one.size() && one[i] == two[i] && one[i] == 0)
                ++i;
            if (i == one.size())
                return false;
            if (one[i] != two[i] && (one[i] == 0 || two[i] == 0))
                return false;
            double coef = one[i] / two[i];
            while (i < one.size()) {
                if (abs(coef * two[i] - one[i]) > ERROR)
                    return false;
                ++i;
            }
            return true;
        }
        else
        {
            return false;
        }
    }

    bool operator&&(const vector<double>& one, const vector<double>& two) {
        if (one.size() == two.size()) {
            int i = 0;
            while (i < one.size() && one[i] == two[i] && one[i] == 0)
                ++i;
            if (i == one.size())
                return false;
            if (one[i] != two[i] && (one[i] == 0 || two[i] == 0))
                return false;
            double coef = one[i] / two[i];
            while (i < one.size()) {
                if (abs(coef * two[i] - one[i]) > ERROR)
                    return false;
                ++i;
            }
            return coef > 0;
        }
        else
        {
            return false;
        }
    }

    void reverse(vector<double>& obj) {
        double temp;
        for (int i = 0; i < obj.size() / 2; ++i) {
            temp = obj[i];
            obj[i] = obj[obj.size() - 1 - i];
            obj[obj.size() - 1 - i] = temp;
        }
    }

    vector<int> operator&(const vector<int>& one, const vector<int>& two) {
        if (one.size() == two.size()) {
            vector<int> result;
            for (int i = 0; i < one.size(); ++i)
                result.push_back(one[i] & two[i]);
            return result;
        }
        else
        {
            vector<int> empty;
            return empty;
        }
    }

    vector<int> operator|(const vector<int>& one, const vector<int>& two) {
        if (one.size() == two.size()) {
            vector<int> result;
            for (int i = 0; i < one.size(); ++i)
                result.push_back(one[i] | two[i]);
            return result;
        }
        else
        {
            vector<int> empty;
            return empty;
        }
    }
}  // namespace task
