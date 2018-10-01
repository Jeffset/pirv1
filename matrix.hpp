//
// Created by jeffset on 9/30/18.
//

#ifndef PIRV1_MATRIX_HPP
#define PIRV1_MATRIX_HPP

#include <vector>
#include <memory>
#include <cassert>
#include <type_traits>
#include <random>


template<typename M>
class Matrix {
 public:
  using value_t = M;

  explicit Matrix(size_t size) : size_(size) {
    data_.resize(size * size, value_t());
  }

  value_t& at(int i, int j) {
    return data_[i * size_ + j];
  }

  value_t at(int i, int j) const {
    return data_[i * size_ + j];
  }

  size_t size() const { return size_; }

  void print() const {
    for (int i = 0; i < size_; ++i) {
      printf("| ");
      for (int j = 0; j < size_; ++j) {
        printf("%d ", at(i, j));
      }
      printf("|\n");
    }
    printf("\n");
  }

  template<int N>
  void fill_from_array(M array[N][N]) {
    for (int i = 0; i < N; ++i) {
      for (int j = 0; j < N; ++j) {
        at(i, j) = array[i][j];
      }
    }
  }

 private:
  const size_t size_;
  std::vector<value_t> data_;
};

template<typename M>
class MatrixBlock {
 public:
  using value_t = M;

  MatrixBlock() = default;

  MatrixBlock(std::shared_ptr<Matrix<M>> base_matrix,
              int base_i, int base_j, size_t block_size) :
      base_matrix_(std::move(base_matrix)),
      base_i_(base_i),
      base_j_(base_j),
      size_(block_size) {}

  value_t& at(int i, int j) {
    assert(base_matrix_);
    return base_matrix_->at(base_i_ + i, base_j_ + j);
  }

  value_t at(int i, int j) const {
    assert(base_matrix_);
    return base_matrix_->at(base_i_ + i, base_j_ + j);
  }

  size_t size() const { return size_; }

 private:
  size_t size_ = 0;
  std::shared_ptr<Matrix<M>> base_matrix_;
  int base_i_ = 0, base_j_ = 0;
};

template<typename T>
Matrix<MatrixBlock<T>> blockize(std::shared_ptr<Matrix<T>> base_matrix,
                                size_t block_size) {
  auto size = base_matrix->size();
  if (size % block_size) {
    throw std::runtime_error("invalid block size!");
  }
  auto block_count = size / block_size;
  Matrix<MatrixBlock<T>> matrix{block_count};
  for (int i = 0; i < block_count; ++i) {
    for (int j = 0; j < block_count; ++j) {
      matrix.at(i, j) = MatrixBlock<T>(base_matrix,
                                       static_cast<int>(i * block_size),
                                       static_cast<int>(j * block_size),
                                       block_size);
    }
  }
  return matrix;
}

template<typename T>
void randomize(Matrix<T>& m, T min, T max) {
  static_assert(std::is_integral_v<T>, "integer types only supported");
  std::random_device rnd;
  std::uniform_int_distribution<T> distribution(min, max);
  auto size = m.size();
  for (int i = 0; i < size; ++i)
    for (int j = 0; j < size; ++j)
      m.at(i, j) = distribution(rnd);
}

extern int thread_count;

template<typename T>
void multiply(const T& a, const T& b, T& c) {
  if constexpr (std::is_arithmetic_v<T>) {
    c += a * b;
  } else {  // assume matrix type
    assert(a.size() == b.size() && b.size() == c.size());
    auto size = a.size();
    constexpr bool d = std::is_same_v<T, MatrixBlock<int>>;
    #pragma omp parallel for if (d) num_threads(thread_count)
    for (int i = 0; i < size; ++i)
      for (int j = 0; j < size; ++j)
        for (int k = 0; k < size; ++k)
          multiply(a.at(i, k), b.at(k, j), c.at(i, j));
  }
}

#endif //PIRV1_MATRIX_HPP
