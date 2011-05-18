/* linbox/blas/level3-m4ri.h
 * Copyright 2011 Bradford Hovinen <hovinen@gmail.com>
 *
 * Generic implementations of level 3 BLAS interface
 * ------------------------------------
 *
 * See COPYING for license information.
 */

#ifndef __BLAS_LEVEL3_M4RI_H
#define __BLAS_LEVEL3_M4RI_H

#ifndef __LINBOX_HAVE_M4RI
#  error "This header file requires that LinBox be configured with libm4ri enabled. Please ensure that libm4ri is properly installed and re-run configure."
#endif

#include <m4ri/m4ri.h>

#include "linbox/blas/context.h"
#include "linbox/blas/level3.h"
#include "linbox/blas/level3-generic.h"
#include "linbox/matrix/matrix-traits.h"
#include "linbox/matrix/m4ri-matrix.h"
#include "linbox/field/gf2.h"

namespace LinBox
{

namespace BLAS3
{

DenseMatrix<bool> &copy_impl (const GF2 &F, M4RIModule &M, const DenseMatrix<bool> &A, DenseMatrix<bool> &B,
			      MatrixCategories::RowMatrixTag, MatrixCategories::RowMatrixTag)
	{ mzd_copy (B._rep, A._rep); return B; }

DenseMatrix<bool> &scal_impl (const GF2 &F, M4RIModule &M, bool a, DenseMatrix<bool> &A, MatrixCategories::RowMatrixTag);

DenseMatrix<bool> &axpy_impl (const GF2 &F, M4RIModule &M, bool a, const DenseMatrix<bool> &A, DenseMatrix<bool> &B,
			      MatrixCategories::RowMatrixTag, MatrixCategories::RowMatrixTag);

DenseMatrix<bool> &gemm_impl (const GF2 &F, M4RIModule &M,
			      bool a, const DenseMatrix<bool> &A, const DenseMatrix<bool> &B, bool b, DenseMatrix<bool> &C,
			      MatrixCategories::RowMatrixTag, MatrixCategories::RowMatrixTag, MatrixCategories::RowMatrixTag);

DenseMatrix<bool> &trmm_impl (const GF2 &F, M4RIModule &M, bool a, const DenseMatrix<bool> &A, DenseMatrix<bool> &B, TriangularMatrixType type, bool diagIsOne,
			      MatrixCategories::RowMatrixTag, MatrixCategories::RowMatrixTag);

DenseMatrix<bool> &trsm_impl (const GF2 &F, M4RIModule &M, bool a, const DenseMatrix<bool> &A, DenseMatrix<bool> &B, TriangularMatrixType type, bool diagIsOne,
			      MatrixCategories::RowMatrixTag, MatrixCategories::RowMatrixTag);

template <class Iterator>
DenseMatrix<bool> &permute_rows_impl (const GF2 &F, M4RIModule &M, Iterator P_begin, Iterator P_end, DenseMatrix<bool> &A, MatrixCategories::RowMatrixTag);

template <class Iterator>
DenseMatrix<bool> &permute_cols_impl (const GF2 &F, M4RIModule &M, Iterator P_begin, Iterator P_end, DenseMatrix<bool> &A, MatrixCategories::RowMatrixTag);

bool equal_impl (const GF2 &F, M4RIModule &M, const DenseMatrix<bool> &A, const DenseMatrix<bool> &B,
		 MatrixCategories::RowMatrixTag, MatrixCategories::RowMatrixTag)
	{ return mzd_equal (A._rep, B._rep); }

bool is_zero_impl (const GF2 &F, M4RIModule &M, const DenseMatrix<bool> &A, MatrixCategories::RowMatrixTag)
	{ return mzd_is_zero (A._rep); }

} // namespace BLAS3

} // namespace LinBox

#include "linbox/blas/level3-m4ri.tcc"

#endif // __BLAS_LEVEL3_M4RI_H

// Local Variables:
// mode: C++
// tab-width: 8
// indent-tabs-mode: t
// c-basic-offset: 8
// End:

// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s:syntax=cpp.doxygen:foldmethod=syntax