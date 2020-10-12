#include "matrix.h"
#include <cmath>

using namespace task;

Matrix::Line::Line(size_t leng, double* arr) : length(leng), mas(arr) {}

double& Matrix::Line::operator[](size_t ind) {
    if (ind < 0 || ind >= length) {
        throw OutOfBoundsException();
    }
    else
    {
        return mas[ind];
    }
}

const double& Matrix::Line::operator[](size_t ind) const {
    if (ind < 0 || ind >= length) {
        throw OutOfBoundsException();
    }
    else
    {
        return mas[ind];
    }
}

Matrix::Matrix() : rows_(1), cols_(1) {
    matr = new double* [rows_];
    matr[0] = new double[cols_];
    matr[0][0] = 1;
}

Matrix::Matrix(size_t rows, size_t cols) : rows_(rows), cols_(cols) {
    matr = new double* [rows_];
    for (size_t i = 0; i < rows_; ++i)
        matr[i] = new double[cols_];
    for (size_t i = 0; i < std::min(rows_, cols_); ++i)
        matr[i][i] = 1;
}

Matrix::Matrix(const Matrix& copy) {
    rows_ = copy.rows_;
    cols_ = copy.cols_;
    matr = new double* [rows_];
    for (size_t i = 0; i < rows_; ++i) {
        matr[i] = new double[cols_];
        for (size_t j = 0; j < cols_; ++j)
            matr[i][j] = copy.matr[i][j];
    }
}

Matrix& Matrix::operator=(const Matrix& a) {
    if (this == &a) {
        return *this;
    }
    else {
        for (size_t i = 0; i < rows_; ++i)
            delete[] matr[i];
        delete[] matr;
        rows_ = a.rows_;
        cols_ = a.cols_;
        matr = new double* [rows_];
        for (size_t i = 0; i < rows_; ++i) {
            matr[i] = new double[cols_];
            for (size_t j = 0; j < cols_; ++j)
                matr[i][j] = a.matr[i][j];
        }
        return *this;
    }
}

double& Matrix::get(size_t row, size_t col) {
    if (row < 0 || col < 0 || row >= rows_ || col >= cols_) {
        throw OutOfBoundsException();
    }
    else
    {
        return matr[row][col];
    }
}

const double& Matrix::get(size_t row, size_t col) const {
    if (row < 0 || col < 0 || row >= rows_ || col >= cols_) {
        throw OutOfBoundsException();
    }
    else
    {
        return matr[row][col];
    }
}

void Matrix::set(size_t row, size_t col, const double& value) {
    if (row < 0 || col < 0 || row >= rows_ || col >= cols_) {
        throw OutOfBoundsException();
    }
    else
    {
        matr[row][col] = value;
    }
}

void Matrix::resize(size_t new_rows, size_t new_cols) {
    double** new_matr = new double* [new_rows];
    for (size_t i = 0; i < new_rows; ++i)
        new_matr[i] = new double[new_cols];
    for (size_t i = 0; i < new_rows; ++i)
        for (size_t j = 0; j < new_cols; ++j) {
            if (i < rows_ && j < cols_)
                new_matr[i][j] = matr[i][j];
            else
                new_matr[i][j] = 0;
        }
    for (size_t i = 0; i < rows_; ++i)
        delete[] matr[i];
    delete[] matr;
    matr = new_matr;
    rows_ = new_rows;
    cols_ = new_cols;
}

Matrix::Line Matrix::operator[](size_t row) {
    if (row < 0 || row >= rows_)
        throw OutOfBoundsException();
    else
    {
        Matrix::Line temp(cols_, matr[row]);
        return temp;
    }

}

const Matrix::Line Matrix::operator[](size_t row) const {
    if (row < 0 || row >= rows_)
        throw OutOfBoundsException();
    else
    {
        const Matrix::Line temp(cols_, matr[row]);
        return temp;
    }
}

Matrix& Matrix::operator+=(const Matrix& a) {
    if (rows_ != a.rows_ || cols_ != a.cols_) {
        throw SizeMismatchException();
    }
    else {
        for (size_t i = 0; i < rows_; ++i)
            for (size_t j = 0; j < cols_; ++j)
                matr[i][j] += a[i][j];
        return *this;
    }
}

Matrix& Matrix::operator-=(const Matrix& a) {
    if (rows_ != a.rows_ || cols_ != a.cols_) {
        throw SizeMismatchException();
    }
    else {
        for (size_t i = 0; i < rows_; ++i)
            for (size_t j = 0; j < cols_; ++j)
                matr[i][j] -= a[i][j];
        return *this;
    }

}

Matrix& Matrix::operator*=(const Matrix& a) {
    if (cols_ != a.rows_) {
        throw SizeMismatchException();
    }
    else {
        *this = (*this) * a;
        return *this;
    }

}

Matrix& Matrix::operator*=(const double& number) {
    for (size_t i = 0; i < rows_; ++i)
        for (size_t j = 0; j < cols_; ++j)
            matr[i][j] *= number;
    return *this;

}

Matrix Matrix::operator+(const Matrix& a) const {
    Matrix result = *this;
    result += a;
    return result;
}

Matrix Matrix::operator-(const Matrix& a) const {
    Matrix result = *this;
    result -= a;
    return result;
}

Matrix Matrix::operator*(const Matrix& a) const {
    if (cols_ != a.rows_) {
        throw SizeMismatchException();
    }
    else {
        Matrix result(rows_, a.cols_);
        result *= 0;
        for (size_t i = 0; i < rows_; ++i)
            for (size_t j = 0; j < cols_; ++j)
                for (size_t k = 0; k < a.cols_; ++k)
                    result[i][k] += matr[i][j] * a[j][k];
        return result;
    }
}

Matrix Matrix::operator*(const double& a) const {
    Matrix result = *this;
    result *= a;
    return result;
}

Matrix Matrix::operator-() const {
    Matrix result = *this;
    result *= -1;
    return result;
}

Matrix Matrix::operator+() const {
    Matrix result = *this;
    return result;
}

double Matrix::det() const {
    if (cols_ != rows_) {
        throw SizeMismatchException();
    }
    else {
        if (cols_ == 1)
            return matr[0][0];
        double determ = 0;
        for (size_t i = 0; i < cols_; ++i) {
            Matrix temp(rows_ - 1, cols_ - 1);
            for (size_t j = 1; j < rows_; ++j) {
                for (size_t k = 0; k < cols_; ++k) {
                    if (k != i) {
                        temp[j - 1][k - int(k > i)] = matr[j][k];
                    }
                }
            }
            determ += pow(-1, i) * matr[0][i] * temp.det();
        }
        return determ;
    }
}

void Matrix::transpose() {
    double** new_matr = new double* [cols_];
    for (size_t i = 0; i < cols_; ++i)
        new_matr[i] = new double[rows_];
    for (size_t i = 0; i < rows_; ++i)
        for (size_t j = 0; j < cols_; ++j)
            new_matr[j][i] = matr[i][j];
    for (size_t i = 0; i < rows_; ++i)
        delete[] matr[i];
    delete[] matr;
    matr = new_matr;
    size_t tmp = cols_;
    cols_ = rows_;
    rows_ = tmp;
}

Matrix Matrix::transposed() const {
    Matrix result = *this;
    result.transpose();
    return result;
}

double Matrix::trace() const {
    if (cols_ != rows_) {
        throw SizeMismatchException();
    }
    else {
        double result = 0;
        for (size_t i = 0; i < rows_; ++i)
            result += matr[i][i];
        return result;
    }

}

std::vector<double> Matrix::getRow(size_t row) {
    std::vector<double> result;
    for (size_t j = 0; j < cols_; ++j)
        result.push_back(matr[row][j]);
    return result;
}

std::vector<double> Matrix::getColumn(size_t column) {
    std::vector<double> result;
    for (size_t i = 0; i < rows_; ++i)
        result.push_back(matr[i][column]);
    return result;

}

bool Matrix::operator==(const Matrix& a) const {
    if (cols_ != a.cols_ || rows_ != a.rows_)
        throw SizeMismatchException();
    else {
        bool result = 1;
        for (size_t i = 0; i < rows_; ++i)
            for (size_t j = 0; j < cols_; ++j)
                result = result && (std::abs(matr[i][j] - a[i][j]) < EPS);
        return result;
    }
}

bool Matrix::operator!=(const Matrix& a) const {
    return !(*this == a);
}

Matrix task::operator*(const double& a, const Matrix& b) {
    return b * a;
}

std::ostream& task::operator<<(std::ostream& output, const Matrix& matrix) {
    for (size_t i = 0; i < matrix.rows_; ++i) {
        for (size_t j = 0; j < matrix.cols_; ++j)
            output << matrix[i][j] << " ";
        output << '\n';
    }
    return output;
}

std::istream& task::operator>>(std::istream& input, Matrix& matrix) {
    //int rows = 0, cols = 0;
    for (size_t i = 0; i < matrix.rows_; ++i)
        delete[] matrix.matr[i];
    delete[] matrix.matr;
    input >> matrix.rows_ >> matrix.cols_;
    matrix.matr = new double* [matrix.rows_];
    for (size_t i = 0; i < matrix.rows_; ++i) {
        matrix.matr[i] = new double[matrix.cols_];
        for (size_t j = 0; j < matrix.cols_; ++j)
            input >> matrix.matr[i][j];
    }
    return input;

}

Matrix::~Matrix() {
    for (size_t i = 0; i < rows_; ++i)
        delete[] matr[i];
    delete[] matr;
}
