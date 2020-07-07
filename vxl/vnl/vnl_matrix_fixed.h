//
//  vnl_matrix_fixed.h
//  vxl
//
//  Created by Jimmy Chen on 2020-06-14.
//  Copyright © 2020 Nowhere Planet. All rights reserved.
//

#ifndef vnl_matrix_fixed_h
#define vnl_matrix_fixed_h

#include <Eigen/Dense>
#include <vnl/vnl_error.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_matrix.h>

template <typename T> class vnl_vector;
template <typename T, unsigned int n> class vnl_vector_fixed;
template <typename T> class vnl_matrix;
template <typename T, unsigned int num_rows, unsigned int num_cols> class vnl_matrix_fixed;


template <typename T, unsigned int num_rows, unsigned int num_cols>
class vnl_matrix_fixed : public Eigen::Matrix<T, num_rows, num_cols, Eigen::RowMajor>
{
    static constexpr size_t num_elements = num_rows*num_cols;
    static constexpr size_t num_bytes = num_elements*sizeof(T);
    using base_class = Eigen::Matrix<T, num_rows, num_cols, Eigen::RowMajor>;
    using abs_t = typename vnl_numeric_traits<T>::abs_t;
public:
    //: Construct an empty num_rows*num_cols matrix
    vnl_matrix_fixed() = default;
    
    //: Construct an m*n Matrix and copy rhs into it.
    //  Abort if rhs is not the same size.
    vnl_matrix_fixed(const vnl_matrix_fixed& rhs) = default;
    //: Copy another vnl_matrix_fixed<T,m,n> into this.
    vnl_matrix_fixed& operator=(const vnl_matrix_fixed& rhs) = default;
    
    // NOTE: move-assignment must be allowed to throw an exception, because we need to maintain
    //       backwards compatibility and the move-construction & move-aasignment
    //       operators fall back to the copy-assignment operator behavior in
    //       cases when the memory is externally managed.
    vnl_matrix_fixed(vnl_matrix_fixed&& other) = default;
    vnl_matrix_fixed& operator=(vnl_matrix_fixed&& rhs) = default;
    ~vnl_matrix_fixed() = default;
    
    //: Construct an m*n matrix and fill with value
    explicit vnl_matrix_fixed(T value)
    {
        this->setConstant(value);
    }
    
    //: Construct an m*n Matrix and copy data into it row-wise.
    explicit vnl_matrix_fixed(const T* datablck)
    {
        std::memcpy(this->data(), datablck, num_bytes);
    }
    
    //: Construct an m*n Matrix and copy rhs into it.
    //  Abort if rhs is not the same size.
    //vnl_matrix_fixed(const vnl_matrix_fixed& rhs) = default;
    
    // This constructor allows us to construct vnl_matrix_fixed from Eigen expressions
    // https://eigen.tuxfamily.org/dox/TopicCustomizing_InheritingMatrix.html
    template<typename OtherDerived>
    vnl_matrix_fixed(const Eigen::MatrixBase<OtherDerived>& other):
        base_class(other)
    {}
    
    // This method allows us to assign Eigen expressions to vnl_matrix_fixed
    template<typename OtherDerived>
    vnl_matrix_fixed& operator=(const Eigen::MatrixBase<OtherDerived>& other)
    {
        this->base_class::operator=(other);
        return *this;
    }
    
    //: Construct an m*n Matrix and copy rhs into it.
    //  Abort if rhs is not the same size.
    vnl_matrix_fixed(const vnl_matrix<T>& rhs)
    {
        assert(rhs.rows() == num_rows && rhs.columns() == num_cols);
        std::memcpy(this->data(), rhs.data_block(), num_bytes);
    }
    
    //: Set all elements to value v
    // Complexity $O(r.c)$
    vnl_matrix_fixed& operator= (T const&v)
    {
        this->setConstant(v);
        return *this;
    }
    
    //: Copy a vnl_matrix into this.
    //  Abort if rhs is not the same size.
    vnl_matrix_fixed& operator=(const vnl_matrix<T>& rhs);
    
    // Basic 2D-Array functionality-------------------------------------------
    
    //: Return the total number of elements stored by the matrix.
    // This equals rows() * cols()
    constexpr unsigned int size() const { return num_elements; }
    
    //: Return the number of rows.
    constexpr unsigned int rows() const { return num_rows; }
    
    //: Return the number of columns.
    // A synonym for columns().
    constexpr unsigned int cols() const { return num_cols; }
    
    //: Return the number of columns.
    // A synonym for cols().
    constexpr unsigned int columns() const { return num_cols; }
    
    //: set element
    inline void put (unsigned r, unsigned c, T const& v)
    {
#if VNL_CONFIG_CHECK_BOUNDS
        if (r >= num_rows)                // If invalid size specified
            vnl_error_matrix_row_index("put", r); // Raise exception
        if (c >= num_cols)                // If invalid size specified
            vnl_error_matrix_col_index("put", c); // Raise exception
#endif
        (*this)(r, c) = v;
    }
    
    //: get element
    inline T get (unsigned r, unsigned c) const
    {
#if VNL_CONFIG_CHECK_BOUNDS
        if (r >= num_rows)                // If invalid size specified
            vnl_error_matrix_row_index("get", r); // Raise exception
        if (c >= num_cols)                // If invalid size specified
            vnl_error_matrix_col_index("get", c); // Raise exception
#endif
        return (*this)(r, c);
    }
    
    
    //: set element, and return *this
    vnl_matrix_fixed& set (unsigned r, unsigned c, T const& v) { (*this)(r,c) = v; return *this; }
    
    //: return pointer to given row
    // No boundary checking here.
    T       * operator[] (unsigned r) { return this->row(r).data(); }
    
    //: return pointer to given row
    // No boundary checking here.
    T const * operator[] (unsigned r) const { return this->row(r).data(); }
    
    //: Access an element for reading or writing
    // There are assert style boundary checks - #define NDEBUG to turn them off.
    //T       & operator() (unsigned r, unsigned c);
    
    //: Access an element for reading
    // There are assert style boundary checks - #define NDEBUG to turn them off.
    //T const & operator() (unsigned r, unsigned c) const;
    
    // ----------------------- Filling and copying -----------------------
    
    //: Sets all elements of matrix to specified value, and returns "*this".
    //  Complexity $O(r.c)$
    //  Returning "*this" allows "chaining" two or more operations:
    //  e.g., to set a matrix to a column-normalized all-elements-equal matrix, say
    //  \code
    //     M.fill(1).normalize_columns();
    //  \endcode
    //  Returning "*this" also allows passing such a matrix as argument
    //  to a function f, without having to name the constructed matrix:
    //  \code
    //     f(vnl_matrix_fixed<double,5,5>(1.0).normalize_columns());
    //  \endcode
    vnl_matrix_fixed& fill(T);
    
    //: Sets all diagonal elements of matrix to specified value; returns "*this".
    //  Complexity $O(\min(r,c))$
    //  Returning "*this" allows "chaining" two or more operations:
    //  e.g., to set a 3x3 matrix to [5 0 0][0 10 0][0 0 15], just say
    //  \code
    //     M.fill_diagonal(5).scale_row(1,2).scale_column(2,3);
    //  \endcode
    //  Returning "*this" also allows passing a diagonal-filled matrix as argument
    //  to a function f, without having to name the constructed matrix:
    //  \code
    //     f(vnl_matrix_fixed<double,3,3>().fill_diagonal(5));
    //  \endcode
    vnl_matrix_fixed& fill_diagonal(T);
    
    //: Sets the diagonal elements of this matrix to the specified list of values.
    //  Returning "*this" allows "chaining" two or more operations: see the
    //  reasoning (and the examples) in the documentation for method
    //  fill_diagonal().
    vnl_matrix_fixed& set_diagonal(vnl_vector<T> const&);
    
    //: Fills (laminates) this matrix with the given data, then returns it.
    //  We assume that the argument points to a contiguous rows*cols array, stored rowwise.
    //  No bounds checking on the array.
    //  Returning "*this" allows "chaining" two or more operations:
    //  e.g., to fill a square matrix column-wise, fill it rowwise then transpose:
    //  \code
    //     M.copy_in(array).inplace_transpose();
    //  \endcode
    //  Returning "*this" also allows passing a filled-in matrix as argument
    //  to a function f, without having to name the constructed matrix:
    //  \code
    //     f(vnl_matrix_fixed<double,3,3>().copy_in(array));
    //  \endcode
    vnl_matrix_fixed& copy_in(T const *);
    
    //: Fills (laminates) this matrix with the given data, then returns it.
    //  A synonym for copy_in()
    vnl_matrix_fixed& set(T const *d) { return copy_in(d); }
    
    //: Fills the given array with this matrix.
    //  We assume that the argument points to a contiguous rows*cols array, stored rowwise.
    //  No bounds checking on the array.
    void copy_out(T *) const;
    
    //: Transposes this matrix efficiently, if it is square, and returns it.
    //  Returning "*this" allows "chaining" two or more operations:
    //  e.g., to fill a square matrix column-wise, fill it rowwise then transpose:
    //  \code
    //     M.copy_in(array).inplace_transpose();
    //  \endcode
    vnl_matrix_fixed& inplace_transpose();
    
    
    
    
    
    
    // ----------------------- Arithmetic --------------------------------
    // note that these functions should not pass scalar as a const&.
    // Look what would happen to A /= A(0,0).
    
    //: Add \a s to each element of lhs matrix in situ
    vnl_matrix_fixed& operator+= (T s)
    {
        this->array() += s;
        return *this;
    }
    
    //: Subtract \a s from each element of lhs matrix in situ
    vnl_matrix_fixed& operator-= (T s)
    {
        this->array() -= s;
        return *this;
    }
    
    //:
    vnl_matrix_fixed& operator*= (T s)
    {
        this->array() *= s;
        return *this;
    }
    
    //:
    vnl_matrix_fixed& operator/= (T s)
    {
        this->array() /= s;
        return *this;
    }
    
    //:
    vnl_matrix_fixed& operator+= (vnl_matrix_fixed const& m)
    {
        add( this->data(), m.data(), this->data() ); return *this;
    }
    
    vnl_matrix_fixed& operator+= (vnl_matrix<T> const& m)
    {
        assert( m.rows() == rows() && m.cols() == cols() );
        add( data_block(), m.data_block(), data_block() ); return *this;
    }
    
    
    
    //:
    vnl_matrix_fixed& operator-= (vnl_matrix_fixed const& m)
    {
        sub( this->data(), m.data(), this->data()); return *this;
    }
    
    //:
    vnl_matrix_fixed& operator-= (vnl_matrix<T> const& m)
    {
        assert( m.rows() == rows() && m.cols() == cols() );
        sub( data_block(), m.data_block(), data_block() );
        return *this;
    }
    
    
    
    //: Negate all elements of matrix
    vnl_matrix_fixed operator- () const
    {
        vnl_matrix_fixed r;
        sub( T(0), this->data(), r.data() );
        return r;
    }
    
    //:
    vnl_matrix_fixed& operator*= (vnl_matrix_fixed<T,num_cols,num_cols> const& s)
    {
        vnl_matrix_fixed<T, num_rows, num_cols> out;
        for (unsigned i = 0; i < num_rows; ++i){
            for (unsigned j = 0; j < num_cols; ++j)
            {
                T accum = this->data()[i*num_cols] * s(0,j);
                for (unsigned k = 1; k < num_cols; ++k)
                    accum += this->data()[i*num_cols+k] * s(k,j);
                out(i,j) = accum;
            }
        }
        return *this = out;
    }
    
    ////--------------------------- Additions ----------------------------
    
    //: Make a new matrix by applying function to each element.
    vnl_matrix_fixed apply(T (*f)(T)) const;
    
    //: Make a new matrix by applying function to each element.
    vnl_matrix_fixed apply(T (*f)(T const&)) const;
    
    //: Make a vector by applying a function across rows.
    vnl_vector_fixed<T,num_rows> apply_rowwise(T (*f)(vnl_vector_fixed<T,num_cols> const&)) const;
    
    //: Make a vector by applying a function across columns.
    vnl_vector_fixed<T,num_cols> apply_columnwise(T (*f)(vnl_vector_fixed<T,num_rows> const&)) const;
    
    //: Return transpose
    vnl_matrix_fixed<T,num_cols,num_rows> transpose() const;
    
    //: Return conjugate transpose
    vnl_matrix_fixed<T,num_cols,num_rows> conjugate_transpose() const;
    
    //: Set values of this matrix to those of M, starting at [top,left]
    vnl_matrix_fixed& update(vnl_matrix<T> const&, unsigned top=0, unsigned left=0);
    
    //: Set values of this matrix to those of M, starting at [top,left]
    vnl_matrix_fixed<T,num_rows,num_cols>& update(
                                                  vnl_matrix_fixed<T,num_rows,num_cols> const& m,
                                                  unsigned top=0, unsigned left=0);
    
    //: Set the elements of the i'th column to v[i]  (No bounds checking)
    vnl_matrix_fixed& set_column(unsigned i, T const * v);
    
    //: Set the elements of the i'th column to value, then return *this.
    vnl_matrix_fixed& set_column(unsigned i, T value );
    
    //: Set j-th column to v, then return *this.
    vnl_matrix_fixed& set_column(unsigned j, vnl_vector<T> const& v);
    
    //: Set j-th column to v, then return *this.
    vnl_matrix_fixed& set_column(unsigned j, vnl_vector_fixed<T,num_rows> const& v);
    
    //: Set columns to those in M, starting at starting_column, then return *this.
    vnl_matrix_fixed& set_columns(unsigned starting_column, vnl_matrix<T> const& M);
    
    //: Set the elements of the i'th row to v[i]  (No bounds checking)
    vnl_matrix_fixed& set_row   (unsigned i, T const * v);
    
    //: Set the elements of the i'th row to value, then return *this.
    vnl_matrix_fixed& set_row   (unsigned i, T value );
    
    //: Set the i-th row, then return *this.
    vnl_matrix_fixed& set_row   (unsigned i, vnl_vector<T> const&);
    
    //: Set the i-th row, then return *this.
    vnl_matrix_fixed& set_row   (unsigned i, vnl_vector_fixed<T,num_cols> const&);
    
    //: Extract a sub-matrix of size r x c, starting at (top,left)
    //  Thus it contains elements  [top,top+r-1][left,left+c-1]
    vnl_matrix<T> extract (unsigned r,  unsigned c,
                           unsigned top=0, unsigned left=0) const;
    
    //: Extract a sub-matrix starting at (top,left)
    //
    //  The output is stored in \a sub_matrix, and it should have the
    //  required size on entry.  Thus the result will contain elements
    //  [top,top+sub_matrix.rows()-1][left,left+sub_matrix.cols()-1]
    void extract ( vnl_matrix<T>& sub_matrix,
                  unsigned top=0, unsigned left=0) const;
    
    //: Get a vector equal to the given row
    vnl_vector_fixed<T,num_cols> get_row   (unsigned row) const;
    
    //: Get a vector equal to the given column
    vnl_vector_fixed<T,num_rows> get_column(unsigned col) const;
    
    //: Get a matrix composed of rows from the indices specified in the supplied vector.
    vnl_matrix<T> get_rows(const vnl_vector<unsigned int> &i) const;
    
    //: Get a matrix composed of columns from the indices specified in the supplied vector.
    vnl_matrix<T> get_columns(const vnl_vector<unsigned int> &i) const;
    
    //: Get n rows beginning at rowstart
    vnl_matrix<T> get_n_rows   (unsigned rowstart, unsigned n) const;
    
    //: Get n columns beginning at colstart
    vnl_matrix<T> get_n_columns(unsigned colstart, unsigned n) const;
    
    //: Return a vector with the content of the (main) diagonal
    vnl_vector<T> get_diagonal() const;
    
    //: Flatten row-major (C-style)
    vnl_vector_fixed<T,num_rows*num_cols> flatten_row_major() const;
    
    //: Flatten column-major (Fortran-style)
    vnl_vector_fixed<T,num_rows*num_cols> flatten_column_major() const;
    
    // ==== mutators ====
    
    //: Sets this matrix to an identity matrix, then returns "*this".
    //  Returning "*this" allows e.g. passing an identity matrix as argument to
    //  a function f, without having to name the constructed matrix:
    //  \code
    //     f(vnl_matrix_fixed<double,5,5>().set_identity());
    //  \endcode
    //  Returning "*this" also allows "chaining" two or more operations:
    //  e.g., to set a 3x3 matrix to [3 0 0][0 2 0][0 0 1], one could say
    //  \code
    //     M.set_identity().scale_row(0,3).scale_column(1,2);
    //  \endcode
    //  If the matrix is not square, anyhow set main diagonal to 1, the rest to 0.
    vnl_matrix_fixed& set_identity();
    
    //: Reverses the order of rows, and returns "*this".
    //  Returning "*this" allows "chaining" two or more operations:
    //  e.g., to flip both up-down and left-right, one could just say
    //  \code
    //     M.flipud().fliplr();
    //  \endcode
    vnl_matrix_fixed& flipud();
    
    //: Reverses the order of columns, and returns "*this".
    //  Returning "*this" allows "chaining" two or more operations:
    //  e.g., to flip both up-down and left-right, one could just say
    //  \code
    //     M.flipud().fliplr();
    //  \endcode
    vnl_matrix_fixed& fliplr();
    
    //: Normalizes each row so it is a unit vector, and returns "*this".
    //  Zero rows are not modified
    //  Returning "*this" allows "chaining" two or more operations:
    //  e.g., to set a matrix to a row-normalized all-elements-equal matrix, say
    //  \code
    //     M.fill(1).normalize_rows();
    //  \endcode
    //  Returning "*this" also allows passing such a matrix as argument
    //  to a function f, without having to name the constructed matrix:
    //  \code
    //     f(vnl_matrix_fixed<double,5,5>(1.0).normalize_rows());
    //  \endcode
    vnl_matrix_fixed& normalize_rows();
    
    //: Normalizes each column so it is a unit vector, and returns "*this".
    //  Zero columns are not modified
    //  Returning "*this" allows "chaining" two or more operations:
    //  e.g., to set a matrix to a column-normalized all-elements-equal matrix, say
    //  \code
    //     M.fill(1).normalize_columns();
    //  \endcode
    //  Returning "*this" also allows passing such a matrix as argument
    //  to a function f, without having to name the constructed matrix:
    //  \code
    //     f(vnl_matrix_fixed<double,5,5>(1.0).normalize_columns());
    //  \endcode
    vnl_matrix_fixed& normalize_columns();
    
    //: Scales elements in given row by a factor T, and returns "*this".
    //  Returning "*this" allows "chaining" two or more operations:
    //  e.g., to set a 3x3 matrix to [3 0 0][0 2 0][0 0 1], one could say
    //  \code
    //     M.set_identity().scale_row(0,3).scale_column(1,2);
    //  \endcode
    vnl_matrix_fixed& scale_row   (unsigned row, T value);
    
    //: Scales elements in given column by a factor T, and returns "*this".
    //  Returning "*this" allows "chaining" two or more operations:
    //  e.g., to set a 3x3 matrix to [3 0 0][0 2 0][0 0 1], one could say
    //  \code
    //     M.set_identity().scale_row(0,3).scale_column(1,2);
    //  \endcode
    vnl_matrix_fixed& scale_column(unsigned col, T value);
    
    //: Swap this matrix with that matrix
    void swap(vnl_matrix_fixed<T,num_rows,num_cols> & that);
    
    //: Type def for norms.
    //typedef typename vnl_c_vector<T>::abs_t abs_t;
    /*
    //: Return sum of absolute values of elements
    abs_t array_one_norm() const { return vnl_c_vector<T>::one_norm(begin(), size()); }
    
    //: Return square root of sum of squared absolute element values
    abs_t array_two_norm() const { return vnl_c_vector<T>::two_norm(begin(), size()); }
    
    //: Return largest absolute element value
    abs_t array_inf_norm() const { return vnl_c_vector<T>::inf_norm(begin(), size()); }
   
    //: Return sum of absolute values of elements
    abs_t absolute_value_sum() const { return array_one_norm(); }
    
    //: Return largest absolute value
    abs_t absolute_value_max() const { return array_inf_norm(); }
     */
    // $ || M ||_1 := \max_j \sum_i | M_{ij} | $
    abs_t operator_one_norm() const;
    
    // $ || M ||_\inf := \max_i \sum_j | M_{ij} | $
    abs_t operator_inf_norm() const;
    
    /*
    //: Return Frobenius norm of matrix (sqrt of sum of squares of its elements)
    abs_t frobenius_norm() const { return vnl_c_vector<T>::two_norm(begin(), size()); }
    
    //: Return Frobenius norm of matrix (sqrt of sum of squares of its elements)
    abs_t fro_norm() const { return frobenius_norm(); }
    */
    
    //: Return RMS of all elements
    //abs_t rms() const { return vnl_c_vector<T>::rms_norm(begin(), size()); }
    
    //: Return minimum value of elements
    T min_value() const;
    
    //: Return maximum value of elements
    T max_value() const;
    
    //: Return location of minimum value of elements
    unsigned arg_min() const;
    
    //: Return location of maximum value of elements
    unsigned arg_max() const;
    
    /*
    //: Return mean of all matrix elements
    T mean() const { return vnl_c_vector<T>::mean(begin(), size()); }
    */
    // predicates
    
    //: Return true iff the size is zero.
    bool empty() const { return num_rows==0 && num_cols==0; }
    
    //:  Return true if all elements equal to identity.
    bool is_identity() const;
    
    //:  Return true if all elements equal to identity, within given tolerance
    bool is_identity(double tol) const;
    
    //: Return true if all elements equal to zero.
    bool is_zero() const;
    
    //: Return true if all elements equal to zero, within given tolerance
    bool is_zero(double tol) const;
    
    //:  Return true if all elements of both matrices are equal, within given tolerance
    bool is_equal(vnl_matrix_fixed<T,num_rows,num_cols> const& rhs, double tol) const;
    
    //: Return true if finite
    bool is_finite() const;
    
    //: Return true if matrix contains NaNs
    bool has_nans() const;
    
    
    /*
    //----------------------------------------------------------------------
    // Conversion to vnl_matrix_ref.
    
    // The const version of as_ref should return a const vnl_matrix_ref
    // so that the vnl_matrix_ref::non_const() cannot be used on
    // it. This prevents a const vnl_matrix_fixed from being cast into a
    // non-const vnl_matrix reference, giving a slight increase in type safety.
    
    //: Explicit conversion to a vnl_matrix_ref or vnl_matrix.
    // This is a cheap conversion for those functions that have an interface
    // for vnl_matrix but not for vnl_matrix_fixed. There is also a
    // conversion operator that should work most of the time.
    // \sa vnl_matrix_ref::non_const
    vnl_matrix_ref<T> as_ref() { return vnl_matrix_ref<T>( num_rows, num_cols, data_block() ); }
    const vnl_matrix_ref<T> as_ref() const { return vnl_matrix_ref<T>( num_rows, num_cols, data_block() ); }
     */
    //: Access the contiguous block storing the elements in the matrix row-wise. O(1).
    // 1d array, row-major order.
    T const* data_block() const {return this->data();}
    
    //: Access the contiguous block storing the elements in the matrix row-wise. O(1).
    // 1d array, row-major order.
    T      * data_block() {return this->data();}
    
    vnl_matrix<T> as_matrix() const {
        return vnl_matrix<T>(const_cast<T*>(this->data()),num_rows, num_cols);
    }
    
    
    typedef T element_type;
    
    //: Iterators
    typedef T       *iterator;
    //: Iterator pointing to start of data
    inline iterator       begin() { return this->data(); }
    //: Iterator pointing to element beyond end of data
    inline iterator       end() { return this->data() + size(); }
    
    //: Const iterators
    typedef T const *const_iterator;
    //: Iterator pointing to start of data
    inline const_iterator begin() const { return this->data(); }
    //: Iterator pointing to element beyond end of data
    inline const_iterator end() const { return this->data() + size(); }
    
    //: Return true if *this == rhs
    bool operator_eq (vnl_matrix_fixed const & rhs) const
    {
        return equal( this->data_block(), rhs.data_block() );
    }
    
    //: Equality operator
    bool operator==(vnl_matrix_fixed const &that) const { return  this->operator_eq(that); }
    
    //: Inequality operator
    bool operator!=(vnl_matrix_fixed const &that) const { return !this->operator_eq(that); }
    
    //: Equality operator
    bool operator==(vnl_matrix<T> const &that) const { return  this->operator_eq(that); }
    
    //: Inequality operator
    bool operator!=(vnl_matrix<T> const &that) const { return !this->operator_eq(that); }
    
    
    // Helper routines for arithmetic. These routines know the size from
    // the template parameters. The vector-vector operations are
    // element-wise.
    
    static void add( const T* a, const T* b, T* r );
    static void add( const T* a, T b, T* r );
    static void sub( const T* a, const T* b, T* r );
    static void sub( const T* a, T b, T* r );
    static void sub( T a, const T* b, T* r );
    static void mul( const T* a, const T* b, T* r );
    static void mul( const T* a, T b, T* r );
    static void div( const T* a, const T* b, T* r );
    static void div( const T* a, T b, T* r );
    
    static bool equal( const T* a, const T* b );
    
};

// fill and copy
template<typename T, unsigned nrows, unsigned ncols>
vnl_matrix_fixed<T,nrows,ncols> & vnl_matrix_fixed<T,nrows,ncols>::fill(const T v)
{
    this->setConstant(v);
    return *this;
}


//: Return a vector with the content of the (main) diagonal
template<typename T, unsigned nrows, unsigned ncols>
vnl_vector<T> vnl_matrix_fixed<T,nrows,ncols>::get_diagonal() const
{
    return this->diagonal();
}

//: Flatten row-major (C-style)
template <class T, unsigned nrows, unsigned ncols>
vnl_vector_fixed<T,nrows*ncols> vnl_matrix_fixed<T,nrows,ncols>::flatten_row_major() const
{
    vnl_vector_fixed<T,nrows*ncols> v;
    std::copy(this->data(), this->data()+nrows*ncols, v.data());
    return v;
}

//: Flatten column-major (Fortran-style)
template <class T, unsigned nrows, unsigned ncols>
vnl_vector_fixed<T,nrows*ncols> vnl_matrix_fixed<T,nrows,ncols>::flatten_column_major() const
{
    vnl_vector_fixed<T,nrows*ncols> v;
    for (unsigned int c = 0; c < ncols; ++c)
        for (unsigned int r = 0; r < nrows; ++r)
            v[c*nrows+r] = (*this)(r,c);
    return v;
}
////--------------------------- Additions------------------------------------


template<class T, unsigned nrows, unsigned ncols>
vnl_matrix_fixed<T,ncols,nrows>
vnl_matrix_fixed<T,nrows,ncols>::transpose() const
{
    vnl_matrix_fixed<T,ncols,nrows> result;
    for (unsigned int i = 0; i < cols(); ++i)
        for (unsigned int j = 0; j < rows(); ++j)
            result(i,j) = (*this)(j, i);
    return result;
}

template<class T, unsigned nrows, unsigned ncols>
vnl_matrix_fixed<T,ncols,nrows>
vnl_matrix_fixed<T,nrows,ncols>::conjugate_transpose() const
{
    // matrix.adjoint() (or matrix.conjugate().transpose()
    //https://stackoverflow.com/questions/30451040/complex-number-matrix-multiplication-eigen-vs-matlab
    return this->conjugate().transpose();
}

template<class T, unsigned nrows, unsigned ncols>
vnl_matrix_fixed<T,nrows,ncols>&
vnl_matrix_fixed<T,nrows,ncols>::update (vnl_matrix<T> const& m,
                                         unsigned top, unsigned left)
{
    const unsigned int bottom = top + m.rows();
    const unsigned int right = left + m.cols();
#ifndef NDEBUG
    if (nrows < bottom || ncols < right)
        vnl_error_matrix_dimension ("update",
                                    bottom, right, m.rows(), m.cols());
#endif
    for (unsigned int i = top; i < bottom; ++i)
        for (unsigned int j = left; j < right; ++j)
            (*this)(i, j) = m(i-top,j-left);
    return *this;
}

template<class T, unsigned nrows, unsigned ncols>
vnl_matrix_fixed<T,nrows,ncols>&
vnl_matrix_fixed<T,nrows,ncols>::update (vnl_matrix_fixed<T,nrows,ncols> const& m,
                                         unsigned top, unsigned left)
{
    const unsigned int bottom = top + m.rows();
    const unsigned int right = left + m.cols();
#ifndef NDEBUG
    if (nrows < bottom || ncols < right)
        vnl_error_matrix_dimension ("update",
                                    bottom, right, m.rows(), m.cols());
#endif
    for (unsigned int i = top; i < bottom; ++i)
        for (unsigned int j = left; j < right; ++j)
            (*this)(i, j) = m(i-top,j-left);
    return *this;
}

//: Extract a sub-matrix of size r x c, starting at (top,left)
//  Thus it contains elements  [top,top+r-1][left,left+c-1]
template<typename T, unsigned nrows, unsigned ncols>
vnl_matrix<T> vnl_matrix_fixed<T, nrows, ncols>::extract(unsigned r, unsigned c,
                                     unsigned top, unsigned left) const
{
    vnl_matrix<T> result = this->block(top, left, r, c);
    return result;
}

template <class T, unsigned nrows, unsigned ncols>
void vnl_matrix_fixed<T, nrows, ncols>::extract( vnl_matrix<T>& submatrix,
                            unsigned top, unsigned left) const {
    unsigned const rowz = submatrix.rows();
    unsigned const colz = submatrix.cols();
#ifndef NDEBUG
    unsigned int bottom = top + rowz;
    unsigned int right = left + colz;
    if ((this->rows() < bottom) || (this->cols() < right))
        vnl_error_matrix_dimension ("extract",
                                    this->rows(), this->cols(), bottom, right);
#endif
    for (unsigned int i = 0; i < rowz; i++)      // actual copy of all elements
        for (unsigned int j = 0; j < colz; j++)    // in submatrix
            submatrix(i, j) = (*this)(top+i, left+j);
}

template<typename T, unsigned nrows, unsigned ncols>
unsigned int vnl_matrix_fixed<T,nrows,ncols>::arg_min() const {
    unsigned int r, c;
    this->minCoeff(&r, &c);
    return r * this->cols() + c;
}

//: Return location of maximum value of elements
template<typename T, unsigned nrows, unsigned ncols>
unsigned int vnl_matrix_fixed<T,nrows,ncols>::arg_max() const {
    unsigned int r, c;
    this->maxCoeff(&r, &c);
    return r * this->cols() + c;
}

//: Return minimum value of elements
template<typename T, unsigned nrows, unsigned ncols>
T vnl_matrix_fixed<T,nrows,ncols>::min_value() const {
    unsigned int r, c;
    T min_v = this->minCoeff(&r, &c);
    return min_v;
}

//: Return maximum value of elements
template<typename T, unsigned nrows, unsigned ncols>
T vnl_matrix_fixed<T,nrows,ncols>::max_value() const {
    unsigned int r, c;
    T max_v = this->maxCoeff(&r, &c);
    return max_v;
}

// implement + - * / and equal
template<class T, unsigned nrows, unsigned ncols>
void
vnl_matrix_fixed<T,nrows,ncols>::add( const T* a, const T* b, T* r )
{
    unsigned int count = nrows*ncols;
    while ( count-- )
        *(r++) = *(a++) + *(b++);
}


template<class T, unsigned nrows, unsigned ncols>
void
vnl_matrix_fixed<T,nrows,ncols>::add( const T* a, T b, T* r )
{
    unsigned int count = nrows*ncols;
    while ( count-- )
        *(r++) = *(a++) + b;
}

template<class T, unsigned nrows, unsigned ncols>
void
vnl_matrix_fixed<T,nrows,ncols>::sub( const T* a, const T* b, T* r )
{
    unsigned int count = nrows*ncols;
    while ( count-- )
        *(r++) = *(a++) - *(b++);
}

template<class T, unsigned nrows, unsigned ncols>
void
vnl_matrix_fixed<T,nrows,ncols>::sub( const T* a, T b, T* r )
{
    unsigned int count = nrows*ncols;
    while ( count-- )
        *(r++) = *(a++) - b;
}

template<class T, unsigned nrows, unsigned ncols>
void
vnl_matrix_fixed<T,nrows,ncols>::sub( T a, const T* b, T* r )
{
    unsigned int count = nrows*ncols;
    while ( count-- )
        *(r++) = a - *(b++);
}

template<class T, unsigned nrows, unsigned ncols>
void
vnl_matrix_fixed<T,nrows,ncols>::mul( const T* a, const T* b, T* r )
{
    unsigned int count = nrows*ncols;
    while ( count-- )
        *(r++) = *(a++) * *(b++);
}

template<class T, unsigned nrows, unsigned ncols>
void
vnl_matrix_fixed<T,nrows,ncols>::mul( const T* a, T b, T* r )
{
    unsigned int count = nrows*ncols;
    while ( count-- )
        *(r++) = *(a++) * b;
}

template<class T, unsigned nrows, unsigned ncols>
void
vnl_matrix_fixed<T,nrows,ncols>::div( const T* a, const T* b, T* r )
{
    unsigned int count = nrows*ncols;
    while ( count-- )
        *(r++) = *(a++) / *(b++);
}

template<class T, unsigned nrows, unsigned ncols>
void
vnl_matrix_fixed<T,nrows,ncols>::div( const T* a, T b, T* r )
{
    unsigned int count = nrows*ncols;
    while ( count-- )
        *(r++) = *(a++) / b;
}

template<class T, unsigned nrows, unsigned ncols>
bool
vnl_matrix_fixed<T,nrows,ncols>::equal( const T* a, const T* b )
{
    unsigned int count = nrows*ncols;
    while ( count-- )
        if ( *(a++) != *(b++) )  return false;
    return true;
}



// --- Matrix-scalar -------------------------------------------------------------

template <class T, unsigned m, unsigned n>
inline
vnl_matrix_fixed<T,m,n> operator+( const vnl_matrix_fixed<T,m,n>& mat1, const vnl_matrix_fixed<T,m,n>& mat2 )
{
    vnl_matrix_fixed<T,m,n> r;
    vnl_matrix_fixed<T,m,n>::add( mat1.data(), mat2.data(), r.data() );
    return r;
}

template <class T, unsigned m, unsigned n>
inline
vnl_matrix_fixed<T,m,n> operator+( const vnl_matrix_fixed<T,m,n>& mat, T s )
{
    vnl_matrix_fixed<T,m,n> r;
    vnl_matrix_fixed<T,m,n>::add( mat.data(), s, r.data() );
    return r;
}

template <class T, unsigned m, unsigned n>
inline
vnl_matrix_fixed<T,m,n> operator+( const T& s,
                                  const vnl_matrix_fixed<T,m,n>& mat )
{
    vnl_matrix_fixed<T,m,n> r;
    vnl_matrix_fixed<T,m,n>::add( mat.data(), s, r.data() );
    return r;
}

template <class T, unsigned m, unsigned n>
inline
vnl_matrix_fixed<T,m,n> operator-( const vnl_matrix_fixed<T,m,n>& mat1, const vnl_matrix_fixed<T,m,n>& mat2 )
{
    vnl_matrix_fixed<T,m,n> r;
    vnl_matrix_fixed<T,m,n>::sub( mat1.data(), mat2.data(), r.data() );
    return r;
}

template <class T, unsigned m, unsigned n>
inline
vnl_matrix_fixed<T,m,n> operator-( const vnl_matrix_fixed<T,m,n>& mat, T s )
{
    vnl_matrix_fixed<T,m,n> r;
    vnl_matrix_fixed<T,m,n>::sub( mat.data(), s, r.data() );
    return r;
}

template <class T, unsigned m, unsigned n>
inline
vnl_matrix_fixed<T,m,n> operator-( const T& s,
                                  const vnl_matrix_fixed<T,m,n>& mat )
{
    vnl_matrix_fixed<T,m,n> r;
    vnl_matrix_fixed<T,m,n>::sub( s, mat.data(), r.data() );
    return r;
}

template <class T, unsigned m, unsigned n>
inline
vnl_matrix_fixed<T,m,n> element_product( const vnl_matrix_fixed<T,m,n>& mat1,
                                        const vnl_matrix_fixed<T,m,n>& mat2 )
{
    vnl_matrix_fixed<T,m,n> r;
    vnl_matrix_fixed<T,m,n>::mul( mat1.data_block(), mat2.data_block(), r.data_block() );
    return r;
}


template <class T, unsigned m, unsigned n>
inline
vnl_matrix_fixed<T,m,n> element_quotient( const vnl_matrix_fixed<T,m,n>& mat1,
                                         const vnl_matrix_fixed<T,m,n>& mat2)
{
    vnl_matrix_fixed<T,m,n> r;
    vnl_matrix_fixed<T,m,n>::div( mat1.data_block(), mat2.data_block(), r.data_block() );
    return r;
}


#endif /* vnl_matrix_fixed_h */
