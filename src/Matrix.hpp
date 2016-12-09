
#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <memory>
#include <vector>

class Matrix {
public:
   Matrix(unsigned int rows, unsigned int cols, const float* mat_data);
   Matrix(unsigned int rows, unsigned int cols, const std::vector<float> mat_data);
   Matrix(unsigned int rows, unsigned int cols);
   Matrix(const std::shared_ptr<Matrix> mat);
   Matrix(const Matrix* mat);
	virtual ~Matrix();
   
   unsigned int get_rows() const;
   unsigned int get_cols() const;
   unsigned int get_size() const;
   float at(unsigned int x, unsigned int y) const;
   float at(unsigned int i) const;

   std::shared_ptr<Matrix> add(const std::shared_ptr<Matrix> other) const;
   std::shared_ptr<Matrix> dot(const std::shared_ptr<Matrix> other) const;

   std::shared_ptr<Matrix> apply(float (*func)(float)) const;
   std::shared_ptr<Matrix> relu() const;

   void print(const char* name = 0) const;

   bool equals(const std::shared_ptr<Matrix> mat) const;

private:
   unsigned int rows, cols;
   unsigned int size;
   std::shared_ptr<std::vector<float>> data;
   
   unsigned int index(unsigned int x, unsigned int y) const;
   void set(unsigned int x, unsigned int y, float val);
   void set(unsigned int i, float val);
};


std::shared_ptr<Matrix> matrix_add(const std::shared_ptr<Matrix> mat_a,
                                   const std::shared_ptr<Matrix> mat_b);

std::shared_ptr<Matrix> matrix_dot(const std::shared_ptr<Matrix> mat_a,
                                   const std::shared_ptr<Matrix> mat_b);

std::shared_ptr<Matrix> matrix_relu(const std::shared_ptr<Matrix> mat);
                                  

#endif

