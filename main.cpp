#include <utility>

#include <iostream>
#include <vector>
#include <string>
#include <chrono>

#import "matrix.hpp"


#if defined(DEBUG) || 1
#define DLOG(msg) std::cout << "[DEBUG]: " << msg
#else
#define DLOG(msg)
#endif

struct StartupArgs {
  size_t N;  // matrix size
  size_t R;  // block size
  int T;  // thread count

  bool is_valid() {
    return N > 0 && R > 0 && T > 0 && N >= R;
  }
};

void print_usage(const std::string& binary_name) {
  std::cerr << "You're holding it wrong!\n" <<
            "Correct way to call this binary is:\n" <<
            binary_name << " -n<N:int> -r<R:int> -t<T:int>\n" <<
            "Where N is matrix size, R is block size, T is thread count.\n"
            "For example: " <<
            binary_name << " -n1000 -r100 -t4\n";
}

class Tracer {
 public:
  explicit Tracer(std::string name)
      : start_(std::chrono::high_resolution_clock::now()),
        name_(std::move(name)) {}

  ~Tracer() {
    using namespace std::chrono;
    DLOG(name_ << ": " << duration_cast<std::chrono::milliseconds>(
        high_resolution_clock::now() - start_).count() << " ms\n");
  }

 private:
  std::chrono::high_resolution_clock::time_point start_;
  std::string name_;
};

bool parse_args(int argc, char** argv, StartupArgs& startup_args) {
  std::string binary_name = argv[0];
  std::vector<std::string> args(static_cast<unsigned long>(argc - 1));
  for (int i = 0; i < argc - 1; ++i)
    args[i] = std::string{argv[i + 1]};
  for (auto&& arg : args) {
    if (arg.size() <= 2 || arg[0] != '-') {
      print_usage(binary_name);
      return false;
    }
    if (arg[1] == 'n') {
      startup_args.N = std::stoul(arg.substr(2));
    } else if (arg[1] == 'r') {
      startup_args.R = std::stoul(arg.substr(2));
    } else if (arg[1] == 't') {
      startup_args.T = std::stoi(arg.substr(2));
    } else {
      print_usage(binary_name);
      return false;
    }
  }
  if (!startup_args.is_valid()) {
    print_usage(binary_name);
    return false;
  }
  return true;
}

int a[4][4] = {
    {7,  8,  -9, 10},
    {-2, 2,  -9, 0},
    {1,  -1, 4,  2},
    {7,  -4, 4,  0},
};

int b[4][4] = {
    {2,  7,  0,  1},
    {-2, -8, -9, -1},
    {3,  -9, 1,  2},
    {5,  4,  1,  1},
};

int main(int argc, char** argv) {
  StartupArgs args{};
  if (!parse_args(argc, argv, args)) {
    return 1;
  }
  DLOG("N=" << args.N << " R=" << args.R << " T=" << args.T << '\n');

  auto A = std::make_shared<Matrix<int>>(args.N);
  auto B = std::make_shared<Matrix<int>>(args.N);
  //A->fill_from_array(a);
  //B->fill_from_array(b);
  randomize(*A, 0, 9);
  randomize(*B, 0, 9);
  //A->print();
  //B->print();

  auto C = std::make_shared<Matrix<int>>(args.N);
  auto C1 = std::make_shared<Matrix<int>>(args.N);

  auto Ab = blockize(A, args.R);
  auto Bb = blockize(B, args.R);
  auto Cb = blockize(C, args.R);

  {
    Tracer t{"multiply simply"};
    multiply(*A, *B, *C1, args.T);
  }
  //C1->print();
  {
    Tracer t{"multiply blocky"};
    multiply(Ab, Bb, Cb, args.T);
  }
  //C->print();

  return 0;
}