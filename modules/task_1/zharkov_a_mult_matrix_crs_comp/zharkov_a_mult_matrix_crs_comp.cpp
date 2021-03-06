// Copyright 2022 Zharkov Andrey

#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../../modules/task_1/zharkov_a_mult_matrix_crs_comp/zharkov_a_mult_matrix_crs_comp.h"

CRS_Matrix::CRS_Matrix(const std::vector<std::vector<cpx>>& matrix) {
  row = matrix.size();
  col = matrix[0].size();
  for (const auto& elem : matrix)
    if (elem.size() != col)
      throw std::runtime_error("Different numbers of columns");
  size_t NonZeroCounter = 0;
  rowIndex.push_back(0);

  for (size_t i = 0; i < row; ++i) {
    for (size_t j = 0; j < col; ++j)
      if ((matrix[i][j].real() != 0) || (matrix[i][j].imag() != 0)) {
        val.push_back(matrix[i][j]);
        colIndex.push_back(j);
        NonZeroCounter++;
      }
    rowIndex.push_back(NonZeroCounter);
  }
}

bool CRS_Matrix::operator==(const CRS_Matrix& mat) const& {
  if ((row != mat.row) || (col != mat.col) || (colIndex != mat.colIndex) ||
      (rowIndex != mat.rowIndex) || (val.size() != mat.val.size()))
    return false;
  for (size_t i = 0; i < val.size(); ++i) {
    if ((val[i].real() - mat.val[i].real() != 0) ||
        (val[i].imag() - mat.val[i].imag()) != 0)
      return false;
  }
  return true;
}

CRS_Matrix CRS_Matrix::operator*(const CRS_Matrix& mat) const& {
  CRS_Matrix res;
  res.rowIndex.push_back(0);
  res.row = row;
  res.col = mat.row;
  size_t NonZeroCounter = 0;
  if (col != mat.col) throw std::runtime_error("Different numbers of cols");
  for (size_t i = 1; i < rowIndex.size(); ++i) {
    std::vector<cpx> tmpVec;
    std::vector<size_t> tmpCol;
    for (size_t j = 1; j < mat.rowIndex.size(); ++j) {
      cpx sum = 0;
      size_t lhsIter = rowIndex[i - 1], rhsIter = mat.rowIndex[j - 1];
      while ((lhsIter < rowIndex[i]) && (rhsIter < mat.rowIndex[j])) {
        if (colIndex[lhsIter] == mat.colIndex[rhsIter]) {
          sum += val[lhsIter++] * mat.val[rhsIter++];
        } else {
          if (colIndex[lhsIter] < mat.colIndex[rhsIter])
            lhsIter++;
          else
            rhsIter++;
        }
      }
      if (sum.real() != 0 || sum.imag() != 0) {
        tmpVec.push_back(sum);
        tmpCol.push_back(j - 1);
        NonZeroCounter++;
      }
    }
    for (const auto& elem : tmpVec) res.val.push_back(elem);
    for (const auto& elem : tmpCol) res.colIndex.push_back(elem);
    res.rowIndex.push_back(NonZeroCounter);
  }
  return res;
}

CRS_Matrix CRS_Matrix::transpose() {
  std::vector<std::vector<size_t>> index(col);
  std::vector<std::vector<cpx>> values(col);
  for (size_t i = 1; i < rowIndex.size(); ++i)
    for (size_t j = rowIndex[i - 1]; j < rowIndex[i]; ++j) {
      index[colIndex[j]].push_back(i - 1);
      values[colIndex[j]].push_back(val[j]);
    }
  CRS_Matrix res;
  res.col = row;
  res.row = col;
  size_t size = 0;
  res.rowIndex.push_back(0);
  for (size_t i = 0; i < col; ++i) {
    for (size_t j = 0; j < index[i].size(); ++j) {
      res.val.push_back(values[i][j]);
      res.colIndex.push_back(index[i][j]);
    }
    size += index[i].size();
    res.rowIndex.push_back(size);
  }
  return res;
}

void CRS_Matrix::print() {
  std::cout << "Value = [ ";
  for (const auto& elem : val) std::cout << elem << " ";
  std::cout << "] " << std::endl << "Col_Index = [ ";
  for (const auto& elem : colIndex) std::cout << elem << " ";
  std::cout << "] " << std::endl << "Row_Index = [ ";
  for (const auto& elem : rowIndex) std::cout << elem << " ";
  std::cout << "] " << std::endl;
}

std::vector<std::vector<cpx>> naiveMultiplication(
    const std::vector<std::vector<cpx>>& matrix1,
    const std::vector<std::vector<cpx>>& matrix2) {
  if (matrix1[0].size() != matrix2.size())
    throw std::runtime_error("Different numbers of cols");
  std::vector<std::vector<cpx>> res(matrix1.size(),
                                    std::vector<cpx>(matrix2[0].size()));
  for (size_t i = 0; i < matrix1.size(); ++i)
    for (size_t j = 0; j < matrix2[0].size(); ++j) {
      res[i][j] = 0;
      for (size_t k = 0; k < matrix1[0].size(); ++k)
        res[i][j] += matrix1[i][k] * matrix2[k][j];
    }
  return res;
}
