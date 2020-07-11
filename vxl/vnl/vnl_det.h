// This is core/vnl/vnl_det.h
#ifndef vnl_det_h_
#define vnl_det_h_
//:
//  \file
//  \brief Direct evaluation of 2x2, 3x3 and 4x4 determinants.
//  \author fsm
//
// \verbatim
//  Modifications
//  Peter Vanroose - 15 Oct. 2001 - Renamed from vnl_determinant to vnl_det
//  Peter Vanroose - 15 Oct. 2001 - Added vnl_matrix_fixed interface
// \endverbatim

#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_export.h>

//: 2x2 matrix
template <class T> VNL_EXPORT T vnl_det(T const *row0,
                             T const *row1);

//: 3x3 matrix
template <class T> VNL_EXPORT T vnl_det(T const *row0,
                             T const *row1,
                             T const *row2);

//: 4x4 matrix
template <class T> VNL_EXPORT T vnl_det(T const *row0,
                             T const *row1,
                             T const *row2,
                             T const *row3);

//: Determinant of small size matrices
// \relatesalso vnl_matrix_fixed
template <class T>
inline T vnl_det(vnl_matrix_fixed<T,1,1> const& m) { return m[0][0]; }

//: Determinant of small size matrices
// \relatesalso vnl_matrix_fixed
template <class T>
inline T vnl_det(vnl_matrix_fixed<T,2,2> const& m) { return vnl_det(m[0],m[1]); }

//: Determinant of small size matrices
// \relatesalso vnl_matrix_fixed
template <class T>
inline T vnl_det(vnl_matrix_fixed<T,3,3> const& m) { return vnl_det(m[0],m[1],m[2]); }

//: Determinant of small size matrices
// \relatesalso vnl_matrix_fixed
template <class T>
inline T vnl_det(vnl_matrix_fixed<T,4,4> const& m) { return vnl_det(m[0],m[1],m[2],m[3]); }


// copy from .cpp
template <class T>
T vnl_det(T const *row0, T const *row1)
{
    return row0[0]*row1[1] - row0[1]*row1[0];
}

template <class T>
T vnl_det(T const *row0, T const *row1, T const *row2)
{
    return // the extra '+' makes it work nicely with emacs indentation.
    + row0[0]*row1[1]*row2[2]
    - row0[0]*row2[1]*row1[2]
    - row1[0]*row0[1]*row2[2]
    + row1[0]*row2[1]*row0[2]
    + row2[0]*row0[1]*row1[2]
    - row2[0]*row1[1]*row0[2];
}

template <class T>
T vnl_det(T const *row0, T const *row1, T const *row2, T const *row3)
{
    return
    + row0[0]*row1[1]*row2[2]*row3[3]
    - row0[0]*row1[1]*row3[2]*row2[3]
    - row0[0]*row2[1]*row1[2]*row3[3]
    + row0[0]*row2[1]*row3[2]*row1[3]
    + row0[0]*row3[1]*row1[2]*row2[3]
    - row0[0]*row3[1]*row2[2]*row1[3]
    - row1[0]*row0[1]*row2[2]*row3[3]
    + row1[0]*row0[1]*row3[2]*row2[3]
    + row1[0]*row2[1]*row0[2]*row3[3]
    - row1[0]*row2[1]*row3[2]*row0[3]
    - row1[0]*row3[1]*row0[2]*row2[3]
    + row1[0]*row3[1]*row2[2]*row0[3]
    + row2[0]*row0[1]*row1[2]*row3[3]
    - row2[0]*row0[1]*row3[2]*row1[3]
    - row2[0]*row1[1]*row0[2]*row3[3]
    + row2[0]*row1[1]*row3[2]*row0[3]
    + row2[0]*row3[1]*row0[2]*row1[3]
    - row2[0]*row3[1]*row1[2]*row0[3]
    - row3[0]*row0[1]*row1[2]*row2[3]
    + row3[0]*row0[1]*row2[2]*row1[3]
    + row3[0]*row1[1]*row0[2]*row2[3]
    - row3[0]*row1[1]*row2[2]*row0[3]
    - row3[0]*row2[1]*row0[2]*row1[3]
    + row3[0]*row2[1]*row1[2]*row0[3];
}

#endif // vnl_det_h_
