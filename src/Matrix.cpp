
#include "Matrix.hpp"
#include <stdexcept>
#include <algorithm>
#include <cmath>

using namespace std;


// Constructors ---------------------------------------------------------------
Matrix::Matrix(unsigned int rows, unsigned int cols, const float* mat_data) : rows(rows), cols(cols) {
   size = rows * cols;
   data = make_shared<vector<float>>(size, 0.0);
   for(int i = 0; i < size; i++) {
      data->at(i) = mat_data[i];
   } 
} 

Matrix::Matrix(unsigned int rows, unsigned int cols, const vector<float> mat_data) : rows(rows), cols(cols) {
   size = rows * cols;
   data = make_shared<vector<float>>(size, 0.0);
   for(int i = 0; i < size; i++) {
      data->at(i) = mat_data[i];
   } 
} 

Matrix::Matrix(unsigned int rows, unsigned int cols) : rows(rows), cols(cols) {
   size = rows * cols;
   data = make_shared<vector<float>>(size, 0.0);
} 

Matrix::Matrix(const std::shared_ptr<Matrix> mat) {
   rows = mat->rows;
   cols = mat->cols;
   size = rows * cols;

   data = make_shared<vector<float>>(size, 0.0);
   for(int i = 0; i < size; i++) {
      data->at(i) = mat->at(i);
   } 
} 

Matrix::Matrix(const Matrix* mat) {
   rows = mat->rows;
   cols = mat->cols;
   size = rows * cols;

   data = make_shared<vector<float>>(size, 0.0);
   for(int i = 0; i < size; i++) {
      data->at(i) = mat->at(i);
   } 
}

Matrix::~Matrix() {} 

// Private --------------------------------------------------------------------
unsigned int Matrix::index(unsigned int x, unsigned int y) const {
   return y * cols + x;
} 

void Matrix::set(unsigned int x, unsigned int y, float val) {data->at(index(x,y)) = val;} 
void Matrix::set(unsigned int i, float val) {data->at(i) = val;} 

// Public ---------------------------------------------------------------------
unsigned int Matrix::get_rows() const {return this->rows;}
unsigned int Matrix::get_cols() const {return this->cols;}
unsigned int Matrix::get_size() const {return this->size;}

float Matrix::at(unsigned int x, unsigned int y) const {return data->at(index(x,y));} 
float Matrix::at(unsigned int i) const {return data->at(i);} 

shared_ptr<Matrix> Matrix::add(const shared_ptr<Matrix> other) const {
   if(this->rows != other->rows || this->cols != other->cols) {
      printf("Matrices must be of the same size to add them!\n");
      printf("Got (%d , %d) vs (%d , %d)\n", this->rows, this->cols, other->rows, other->cols);
      throw invalid_argument("Matrices must be of the same size to add them!");
   } 
   
   auto result = make_shared<Matrix>(this);
   for(int i = 0; i < size; i++) {
      result->set(i, this->at(i) + other->at(i));
   } 

   return result;
} 

shared_ptr<Matrix> Matrix::dot(const std::shared_ptr<Matrix> other) const {
   if(this->cols != other->rows) {
      printf("Matrices are of incompatible sizes to be dotted.");
      printf("Got (%d , %d) vs (%d , %d)\n", this->rows, this->cols, other->rows, other->cols);
      throw invalid_argument("Matrices are of incompatible sizes to be dotted.");
   } 
   
   unsigned int new_rows = this->rows;
   unsigned int new_cols = other->cols;
   auto result = make_shared<Matrix>(new_rows, new_cols);

   unsigned int len = this->cols;
   for(int ry = 0; ry < new_rows; ry++) {
      for(int rx = 0; rx < new_cols; rx++) {
         float val = 0;
         for(int i = 0; i < len; i++) {
            val += this->at(i, ry) * other->at(rx, i);
         } 
         result->set(rx,ry,val);
      }
   }  

   return result;
} 

std::shared_ptr<Matrix> Matrix::apply(float (*func)(float)) const {
   auto result = make_shared<Matrix>(this);
   for(int i = 0; i < result->size; i++) {
      result->set(i, func(result->at(i)));
   } 
   return result;
}

shared_ptr<Matrix> Matrix::relu() const {
   auto relu = [](float x) {return max(0.0f, x);};
   return this->apply(relu);
}

void Matrix::print(const char* name) const {
   if(name) {
      printf("%s = ", name);
   }
   printf(" (%d,%d) [\n", this->rows, this->cols);
   for(int y = 0; y < this->rows; ++y) {
      for(int x = 0; x < this->cols; ++x) {
         printf("%- 5.2f ", this->at(x,y));
      }
      printf("\n");
   }
   printf("];\n");
} 


bool Matrix::equals(const std::shared_ptr<Matrix> mat) const {
   if(this->rows != mat->rows || this->cols != mat-> cols)
      return false;
   
   const float epsilon = 0.000001;
   for(int i = 0; i < this->size; i++) {
      if(abs(this->at(i) - mat->at(i)) > epsilon) {
         return false; 
      } 
   } 
   return true;
} 

// General --------------------------------------------------------------------

std::shared_ptr<Matrix> matrix_add(const std::shared_ptr<Matrix> mat_a,
                                   const std::shared_ptr<Matrix> mat_b) {
   return mat_a->add(mat_b);
} 

std::shared_ptr<Matrix> matrix_dot(const std::shared_ptr<Matrix> mat_a,
                                   const std::shared_ptr<Matrix> mat_b) {
   return mat_a->dot(mat_b);
} 

std::shared_ptr<Matrix> matrix_relu(const std::shared_ptr<Matrix> mat) {
   return mat->relu();
} 

