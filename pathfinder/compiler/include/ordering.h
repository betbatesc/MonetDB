/**
 * @file
 *
 * Deal with table orderings in physical algebra plans.
 *
 * $Id$
 */

#ifndef ORDERING_H
#define ORDERING_H

/* Always include pathfinder.h first! */
#include "pathfinder.h"

/* Orderings use arrays of column names. */
#include "array.h"

/* We speak about algebra attribute names. */
#include "algebra.h"

/**
 * We denote the lexicographic ordering @f$ O @f$ on attributes
 * @f$ A_1 @f$ (major) and @f$ A_2 @f$ (minor) of a relation @f$ R @f$
 * as @f$ O = [A_1, A_2] @f$.  An ordering may be @em refined:
 * @f[
 *       [A_1, A_2] + A_3  = [A_1, A_2, A_3] .
 * @f]
 */
typedef PFarray_t * PFord_ordering_t;

/**
 * We often deal with sets of orderings.
 *
 * Technically, they are implemented as #PFarray_t arrays.  Some
 * helper functions let you easily deal with these arrays.
 */
typedef PFarray_t * PFord_set_t;

/**
 * Create a new #PFord_ordering_t object.
 */
PFord_ordering_t PFordering (void);

/**
 * Create an empty set of orderings.
 */
PFord_set_t PFord_set (void);

/**
 * Refine an existing ordering by one more attribute.
 */
PFord_ordering_t PFord_refine (const PFord_ordering_t, const PFalg_att_t);

unsigned int PFord_count (const PFord_ordering_t ordering);

PFalg_att_t PFord_order_at (const PFord_ordering_t ordering,
                            unsigned int index);

/**
 * Return true if the ordering @a a implies the ordering @a b.
 * I.e., ordering @a a is a refinement of ordering @a b.
 */
bool PFord_implies (const PFord_ordering_t a, const PFord_ordering_t b);

/**
 * Return true if orderings @a a and @a b share a common prefix.
 * 
 * (Ordering @a a can then be derived from @a b by @em refinement
 * and vice versa. It is generally cheaper to refine some existing
 * sort order instead of sorting entirely from scratch.)
 */
bool
PFord_common_prefix (const PFord_ordering_t a, const PFord_ordering_t b);

/**
 * Generate string representation of a given ordering.
 */
char *PFord_str (const PFord_ordering_t);

/**
 * Number of elements in a set of orderings.
 */
unsigned int PFord_set_count (const PFord_set_t);

/**
 * Append an element to a set of orderings.
 * Modifies @a set and gives it back as its return value.
 */
PFord_set_t PFord_set_add (PFord_set_t set, const PFord_ordering_t ord);

/**
 * Return item (ordering) at position @a i in a set of orderings @a set.
 */
PFord_ordering_t PFord_set_at (const PFord_set_t set, unsigned int i);

/**
 * Compute all orderings in the intersection of orderings in @a
 * and orderings in @a b.
 *
 * @note
 *   This function may return duplicates, or prefixes that are
 *   implied by other prefixes in the result.
 */
PFord_set_t PFord_intersect (const PFord_set_t a, const PFord_set_t b);

/**
 * Compute duplicate-free equivalent of the list of orderings @a a.
 * This also eliminates orderings that are implied by some more
 * specific ordering.
 */
PFord_set_t PFord_unique (const PFord_set_t a);

/**
 * Given a list of orderings @a a, compute all prefixes of the
 * orderings in @a a.
 *
 * This ``expands'' the current list of orderings by all those
 * orderings that the original list implies.  Does not produce
 * 1:1 duplicates in the result.
 */
PFord_set_t PFord_prefixes (PFord_set_t a);

/**
 * Compute all permutations of an ordering @a ordering and return
 * them as a set of orderings (see #PFord_set_t).
 */
PFord_set_t PFord_permutations (const PFord_ordering_t ordering);

#endif  /* ORDERING_H */

/* vim:set shiftwidth=4 expandtab: */
