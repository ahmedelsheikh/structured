/* ========================================================================== */
/* === Supernodal/cholmod_super_solve ======================================= */
/* ========================================================================== */

/* -----------------------------------------------------------------------------
 * CHOLMOD/Supernodal Module.  Version 1.0.
 * Copyright (C) 2005, Timothy A. Davis
 * The CHOLMOD/Supernodal Module is licensed under Version 2.0 of the GNU
 * General Public License.  See gpl.txt for a text of the license.
 * CHOLMOD is also available under other licenses; contact authors for details.
 * http://www.cise.ufl.edu/research/sparse
 * -------------------------------------------------------------------------- */

/* Solve Lx=b or L'x=b for a supernodal factorization.  These routines do not
 * apply the permutation L->Perm.  See cholmod_solve for a more general
 * interface that performs that operation.
 */

#ifndef NSUPERNODAL

#include "cholmod_supernodal.h"
#include "cholmod_internal.h"

/* ========================================================================== */
/* === TEMPLATE ============================================================= */
/* ========================================================================== */

#define REAL
#include "generic/t_cholmod_super_solve.inc"
#define COMPLEX
#include "generic/t_cholmod_super_solve.inc"

/* ========================================================================== */
/* === cholmod_super_lsolve ================================================= */
/* ========================================================================== */

/* Solve Lx=b where x and b are of size n-by-nrhs.  b is overwritten by the
 * solution x.  On input, b is stored in col-major order with leading dimension
 * of d, and on output x is stored in the same manner.
 *
 * The contents of the workspace E are undefined on both input and output.
 *
 * workspace: none
 */

int CHOLMOD(super_lsolve)
(
    /* ---- input ---- */
    cholmod_factor *L,	/* factor to use for the forward solve */
    /* ---- output ---- */
    cholmod_dense *X,	/* b on input, solution to Lx=b on output */
    /* ---- workspace ---- */
    cholmod_dense *E,	/* workspace of size nrhs*(L->maxesize) */
    /* --------------- */
    cholmod_common *Common
)
{
    int ok = TRUE ;

    /* ---------------------------------------------------------------------- */
    /* check inputs */
    /* ---------------------------------------------------------------------- */

    RETURN_IF_NULL_COMMON (FALSE) ;
    RETURN_IF_NULL (L, FALSE) ;
    RETURN_IF_NULL (X, FALSE) ;
    RETURN_IF_NULL (E, FALSE) ;
    RETURN_IF_XTYPE_INVALID (L, CHOLMOD_REAL, CHOLMOD_COMPLEX, FALSE) ;
    RETURN_IF_XTYPE_INVALID (X, CHOLMOD_REAL, CHOLMOD_COMPLEX, FALSE) ;
    RETURN_IF_XTYPE_INVALID (E, CHOLMOD_REAL, CHOLMOD_COMPLEX, FALSE) ;
    if (L->xtype != X->xtype)
    {
	ERROR (CHOLMOD_INVALID, "L and X must have the same xtype") ;
	return (FALSE) ;
    }
    if (L->xtype != E->xtype)
    {
	ERROR (CHOLMOD_INVALID, "L and E must have the same xtype") ;
	return (FALSE) ;
    }
    if (X->d < X->nrow || L->n != X->nrow)
    {
	ERROR (CHOLMOD_INVALID, "X and L dimensions must match") ;
	return (FALSE) ;
    }
    if (E->nzmax < X->ncol * (L->maxesize))
    {
	ERROR (CHOLMOD_INVALID, "workspace E not large enough") ;
	return (FALSE) ;
    }
    if (!(L->is_ll) || !(L->is_super))
    {
	ERROR (CHOLMOD_INVALID, "L not supernodal") ;
	return (FALSE) ;
    }
    Common->status = CHOLMOD_OK ;
    ASSERT (IMPLIES (L->n == 0, L->nsuper == 0)) ;
    if (L->n == 0 || X->ncol == 0)
    {
	/* nothing to do */
	return (TRUE) ;
    }

    /* ---------------------------------------------------------------------- */
    /* solve Lx=b using template routine */
    /* ---------------------------------------------------------------------- */

    switch (L->xtype)
    {

	case CHOLMOD_REAL:
	    ok = r_cholmod_super_lsolve (L, X, E, Common) ;
	    break ;

	case CHOLMOD_COMPLEX:
	    ok = c_cholmod_super_lsolve (L, X, E, Common) ;
	    break ;
    }

    if (CHECK_BLAS_INT && !ok)
    {
	ERROR (CHOLMOD_TOO_LARGE, "problem too large for the BLAS") ;
    }
    return (ok) ;
}


/* ========================================================================== */
/* === cholmod_super_ltsolve ================================================ */
/* ========================================================================== */

/* Solve L'x=b where x and b are of size n-by-nrhs.  b is overwritten by the
 * solution x.  On input, b is stored in col-major order with leading dimension
 * of d, and on output x is stored in the same manner.
 *
 * The contents of the workspace E are undefined on both input and output.
 *
 * workspace: none
 */

int CHOLMOD(super_ltsolve)
(
    /* ---- input ---- */
    cholmod_factor *L,	/* factor to use for the backsolve */
    /* ---- output ---- */
    cholmod_dense *X,	/* b on input, solution to L'x=b on output */
    /* ---- workspace ---- */
    cholmod_dense *E,	/* workspace of size nrhs*(L->maxesize) */
    /* --------------- */
    cholmod_common *Common
)
{
    int ok = TRUE ;

    /* ---------------------------------------------------------------------- */
    /* check inputs */
    /* ---------------------------------------------------------------------- */

    RETURN_IF_NULL_COMMON (FALSE) ;
    RETURN_IF_NULL (L, FALSE) ;
    RETURN_IF_NULL (X, FALSE) ;
    RETURN_IF_NULL (E, FALSE) ;
    RETURN_IF_XTYPE_INVALID (L, CHOLMOD_REAL, CHOLMOD_COMPLEX, FALSE) ;
    RETURN_IF_XTYPE_INVALID (X, CHOLMOD_REAL, CHOLMOD_COMPLEX, FALSE) ;
    RETURN_IF_XTYPE_INVALID (E, CHOLMOD_REAL, CHOLMOD_COMPLEX, FALSE) ;
    if (L->xtype != X->xtype)
    {
	ERROR (CHOLMOD_INVALID, "L and X must have the same xtype") ;
	return (FALSE) ;
    }
    if (L->xtype != E->xtype)
    {
	ERROR (CHOLMOD_INVALID, "L and E must have the same xtype") ;
	return (FALSE) ;
    }
    if (X->d < X->nrow || L->n != X->nrow)
    {
	ERROR (CHOLMOD_INVALID, "X and L dimensions must match") ;
	return (FALSE) ;
    }
    if (E->nzmax < X->ncol * (L->maxesize))
    {
	ERROR (CHOLMOD_INVALID, "workspace E not large enough") ;
	return (FALSE) ;
    }
    if (!(L->is_ll) || !(L->is_super))
    {
	ERROR (CHOLMOD_INVALID, "L not supernodal") ;
	return (FALSE) ;
    }
    Common->status = CHOLMOD_OK ;
    ASSERT (IMPLIES (L->n == 0, L->nsuper == 0)) ;
    if (L->n == 0 || X->ncol == 0)
    {
	/* nothing to do */
	return (TRUE) ;
    }

    /* ---------------------------------------------------------------------- */
    /* solve Lx=b using template routine */
    /* ---------------------------------------------------------------------- */

    switch (L->xtype)
    {

	case CHOLMOD_REAL:
	    ok = r_cholmod_super_ltsolve (L, X, E, Common) ;
	    break ;

	case CHOLMOD_COMPLEX:
	    ok = c_cholmod_super_ltsolve (L, X, E, Common) ;
	    break ;
    }

    if (CHECK_BLAS_INT && !ok)
    {
	ERROR (CHOLMOD_TOO_LARGE, "problem too large for the BLAS") ;
    }

    return (ok) ;
}
#endif
