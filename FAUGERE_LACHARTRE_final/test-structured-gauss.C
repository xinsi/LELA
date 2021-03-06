/*
 * test-structured-gauss.C
 * Copyright 2012 Martani Fayssal (UPMC University Paris 06 / INRIA)
 *
 *  Created on: 24 mai 2012
 *      Author: martani  (UPMC University Paris 06 / INRIA)
 * 
 * ----------------------------------
 * Tests the structured Gaussian elmination method
 * run with; ./test-structured-gauss - -f path/to/matrix/file
 */

#include "consts-macros.h"
#include "types.h"
#include "matrix-utils.h"
#include "lela/matrix/dense.h"
#include "lela/util/commentator.h"
#include "../util/support.h"

#include "lela/algorithms/elimination.h"
#include "lela/algorithms/gauss-jordan.h"

#include "structured-gauss-lib.h"
#include "level3Parallel_echelon.h"

using namespace LELA;
using namespace std;

template <typename Ring, typename Matrix>
void test_structured_gauss_standard(const Ring& R, Matrix& A)
{
	size_t rank;

	std::ostream &report = commentator.report (Commentator::LEVEL_NORMAL, INTERNAL_DESCRIPTION);

	commentator.start("Structured Gauss Rref");
		rank = StructuredGauss::echelonize_reduced(R, A);
		report << "True rank " << rank << endl;

	commentator.stop(MSG_DONE);
}


template <typename Ring, typename Matrix>
void test_structured_gauss_acc64(const Ring& R, Matrix& A)
{
	size_t rank;

	std::ostream &report = commentator.report (Commentator::LEVEL_NORMAL, INTERNAL_DESCRIPTION);

	commentator.start("Structured Gauss Rref Accumulator 64");
		rank = StructuredGauss::echelonize_reduced_uint16(R, A);
		report << "True rank " << rank << endl;

	commentator.stop(MSG_DONE);
}

template <typename Ring, typename Matrix>
void test_RREF_LELA(const Ring& R, Matrix& A)
{
	Context<Ring> ctx (R);
	Elimination<Ring> elim (ctx);

	size_t rank;
	typename Ring::Element det;

	DenseMatrix<typename Ring::Element> L (A.rowdim (), A.rowdim ());
	typename GaussJordan<Ring>::Permutation P;

	std::ostream &report = commentator.report (Commentator::LEVEL_NORMAL, INTERNAL_DESCRIPTION);

	commentator.start("LELA echelonize_reduced");
		elim.echelonize_reduced (A, L, P, rank, det);
		report << "True rank " << rank << endl;

	commentator.stop(MSG_DONE);
}

template <typename Ring, typename Matrix>
void test_structured_gauss_parallel(const Ring& R, Matrix& A, int NB_THREADS)
{
	typedef SparseBlocMatrix<SparseMultilineBloc<typename Ring::Element, IndexType> > BlocMatrix;
	BlocMatrix bloc_matrix (A.rowdim(), A.coldim (), BlocMatrix::ArrangementDownTop_LeftRight, true);
	SparseMultilineMatrix<typename Ring::Element> multiline_matrix;

	cout << endl;
	commentator.start("Copying sparse to bloc matrix");
	MatrixUtils::copy(A, bloc_matrix);
	commentator.stop(MSG_DONE);
	cout << endl;

	cout << endl;
	commentator.start("Level3ParallelEchelon::echelonize__Parallel");
	uint32 rank = Level3ParallelEchelon::echelonize__Parallel(R, bloc_matrix, multiline_matrix, true, NB_THREADS);
	commentator.stop("echelonize__Parallel");
	cout << endl;

	cout << ">>> Rank " << rank << endl;
	cout << endl;

}

int main (int argc, char **argv)
{
	const char *file_name = "";
	int nb_threads = 0;

	static Argument args[] = {
		{ 'f', "-f File", "The file name where the matrix is stored", TYPE_STRING, &file_name},
		{ 'p', "-p NB_THREADS", "Number of threads in parallel", TYPE_INT, &nb_threads},
		{ '\0' }
	};

	parseArguments (argc, argv, args, "", 0);
	commentator.setReportStream (std::cout);
	commentator.getMessageClass (INTERNAL_DESCRIPTION).setMaxDepth (4);
	commentator.getMessageClass (INTERNAL_DESCRIPTION).setMaxDetailLevel (Commentator::LEVEL_NORMAL);
	commentator.getMessageClass (TIMING_MEASURE).setMaxDepth (3);
	commentator.getMessageClass (TIMING_MEASURE).setMaxDetailLevel (Commentator::LEVEL_NORMAL);
	commentator.getMessageClass (PROGRESS_REPORT).setMaxDepth (3);

	typedef uint16 modulus_type;
	typedef Modular<modulus_type> Ring;

	modulus_type modulus = MatrixUtils::loadF4Modulus(file_name);
	Ring R (modulus);

	Context<Ring> ctx (R);

	SparseMatrix<Ring::Element> A;

	std::ostream &report = commentator.report (Commentator::LEVEL_NORMAL, INTERNAL_DESCRIPTION);
	
	commentator.start("Loading matrix");
		MatrixUtils::loadF4Matrix__low_memory_syscall_no_checks(R, A, file_name);
	commentator.stop(MSG_DONE);
	report << endl;
	SHOW_MATRIX_INFO_SPARSE(A);

	MatrixUtils::show_mem_usage("Loading matrix");
	report << endl;

	if(nb_threads != 0)
	{
		test_structured_gauss_parallel(R, A, nb_threads);
	}

	test_structured_gauss_acc64(R, A);

	report << endl;
	SHOW_MATRIX_INFO_SPARSE(A);

	MatrixUtils::show_mem_usage("STRUCTURED GAUSS");
	return 0;
}
