/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.  If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright 1997 - July 2008 CWI, August 2008 - 2020 MonetDB B.V.
 */

#include "monetdb_config.h"
#include "gdk.h"
#include "gdk_private.h"
#include "gdk_cand.h"

bool
BATiscand(BAT *b)
{
	if (b->ttype == TYPE_msk)
		return true;
	if (ATOMtype(b->ttype) != TYPE_oid)
		return false;
	if (complex_cand(b))
		return true;
	if (b->ttype == TYPE_void && is_oid_nil(b->tseqbase))
		return false;
	return BATtordered(b) && BATtkey(b);
}

/* create a new, dense candidate list with values from `first' up to,
 * but not including, `last' */
static inline BAT *
newdensecand(oid first, oid last)
{
	if (last <= first)
		first = last = 0; /* empty range */
	return BATdense(0, first, last - first);
}

/* merge two candidate lists and produce a new one
 *
 * candidate lists are VOID-headed BATs with an OID tail which is
 * sorted and unique.
 */
BAT *
BATmergecand(BAT *a, BAT *b)
{
	BAT *bn;
	oid *restrict p, i;
	struct canditer cia, cib;

	BATcheck(a, NULL);
	BATcheck(b, NULL);

	canditer_init(&cia, NULL, a);
	canditer_init(&cib, NULL, b);

	/* we can return a if b is empty (and v.v.) */
	if (cia.ncand == 0) {
		bn = canditer_slice(&cib, 0, cib.ncand);
		goto doreturn;
	}
	if (cib.ncand == 0) {
		bn = canditer_slice(&cia, 0, cia.ncand);
		goto doreturn;
	}
	/* we can return a if a fully covers b (and v.v) */
	if (cia.tpe == cand_dense && cib.tpe == cand_dense) {
		/* both are dense */
		if (cia.seq <= cib.seq && cib.seq <= cia.seq + cia.ncand) {
			/* partial overlap starting with a, or b is
			 * smack bang after a */
			bn = newdensecand(cia.seq, cia.seq + cia.ncand < cib.seq + cib.ncand ? cib.seq + cib.ncand : cia.seq + cia.ncand);
			goto doreturn;
		}
		if (cib.seq <= cia.seq && cia.seq <= cib.seq + cib.ncand) {
			/* partial overlap starting with b, or a is
			 * smack bang after b */
			bn = newdensecand(cib.seq, cia.seq + cia.ncand < cib.seq + cib.ncand ? cib.seq + cib.ncand : cia.seq + cia.ncand);
			goto doreturn;
		}
	}
	if (cia.tpe == cand_dense
	    && cia.seq <= cib.seq
	    && canditer_last(&cia) >= canditer_last(&cib)) {
		bn = canditer_slice(&cia, 0, cia.ncand);
		goto doreturn;
	}
	if (cib.tpe == cand_dense
	    && cib.seq <= cia.seq
	    && canditer_last(&cib) >= canditer_last(&cia)) {
		bn = canditer_slice(&cib, 0, cib.ncand);
		goto doreturn;
	}

	bn = COLnew(0, TYPE_oid, cia.ncand + cib.ncand, TRANSIENT);
	if (bn == NULL)
		goto doreturn;
	p = (oid *) Tloc(bn, 0);
	if (cia.tpe == cand_dense && cib.tpe == cand_dense) {
		/* both lists are dense */
		if (cia.seq > cib.seq) {
			struct canditer ci;
			ci = cia;
			cia = cib;
			cib = ci;
		}
		/* cia completely before cib */
		assert(cia.seq + cia.ncand < cib.seq);
		for (i = cia.seq; i < cia.seq + cia.ncand; i++)
			*p++ = i;
		/* there is a gap */
		for (i = cib.seq; i < cib.seq + cib.ncand; i++)
			*p++ = i;
	} else if (cia.tpe == cand_dense || cib.tpe == cand_dense) {
		if (cib.tpe == cand_dense) {
			struct canditer ci;
			ci = cia;
			cia = cib;
			cib = ci;
		}
		/* only cia is dense */

		/* copy part of cib that's before the start of cia */
		while (canditer_peek(&cib) < cia.seq) {
			*p++ = canditer_next(&cib);
		}
		/* copy all of cia */
		for (i = cia.seq; i < cia.seq + cia.ncand; i++)
			*p++ = i;
		/* skip over part of cib that overlaps with cia */
		canditer_setidx(&cib, canditer_search(&cib, cia.seq + cia.ncand, true));
		/* copy rest of cib */
		while (cib.next < cib.ncand) {
			*p++ = canditer_next(&cib);
		}
	} else {
		/* a and b are both not dense */
		oid ao = canditer_next(&cia);
		oid bo = canditer_next(&cib);
		while (!is_oid_nil(ao) && !is_oid_nil(bo)) {
			if (ao < bo) {
				*p++ = ao;
				ao = canditer_next(&cia);
			} else if (bo < ao) {
				*p++ = bo;
				bo = canditer_next(&cib);
			} else {
				*p++ = ao;
				ao = canditer_next(&cia);
				bo = canditer_next(&cib);
			}
		}
		while (!is_oid_nil(ao)) {
			*p++ = ao;
			ao = canditer_next(&cia);
		}
		while (!is_oid_nil(bo)) {
			*p++ = bo;
			bo = canditer_next(&cib);
		}
	}

	/* properties */
	BATsetcount(bn, (BUN) (p - (oid *) Tloc(bn, 0)));
	bn->trevsorted = BATcount(bn) <= 1;
	bn->tsorted = true;
	bn->tkey = true;
	bn->tnil = false;
	bn->tnonil = true;
	bn = virtualize(bn);
  doreturn:
	TRC_DEBUG(ALGO, ALGOBATFMT "," ALGOBATFMT " -> " ALGOOPTBATFMT "\n",
		  ALGOBATPAR(a), ALGOBATPAR(b), ALGOOPTBATPAR(bn));
	return bn;
}

/* intersect two candidate lists and produce a new one
 *
 * candidate lists are VOID-headed BATs with an OID tail which is
 * sorted and unique.
 */
BAT *
BATintersectcand(BAT *a, BAT *b)
{
	BAT *bn;
	oid *restrict p;
	struct canditer cia, cib;

	BATcheck(a, NULL);
	BATcheck(b, NULL);

	canditer_init(&cia, NULL, a);
	canditer_init(&cib, NULL, b);

	if (cia.ncand == 0 || cib.ncand == 0) {
		bn = BATdense(0, 0, 0);
		goto doreturn;
	}

	if (cia.tpe == cand_dense && cib.tpe == cand_dense) {
		/* both lists are dense */
		bn = newdensecand(MAX(cia.seq, cib.seq), MIN(cia.seq + cia.ncand, cib.seq + cib.ncand));
		goto doreturn;
	}

	bn = COLnew(0, TYPE_oid, MIN(cia.ncand, cib.ncand), TRANSIENT);
	if (bn == NULL)
		goto doreturn;
	p = (oid *) Tloc(bn, 0);
	if (cia.tpe == cand_dense || cib.tpe == cand_dense) {
		if (cib.tpe == cand_dense) {
			struct canditer ci;
			ci = cia;
			cia = cib;
			cib = ci;
		}
		/* only cia is dense */

		/* search for first value in cib that is contained in cia */
		canditer_setidx(&cib, canditer_search(&cib, cia.seq, true));
		oid bo;
		while (!is_oid_nil(bo = canditer_next(&cib)) && bo < cia.seq + cia.ncand)
			*p++ = bo;
	} else {
		/* a and b are both not dense */
		oid ao = canditer_next(&cia);
		oid bo = canditer_next(&cib);
		while (!is_oid_nil(ao) && !is_oid_nil(bo)) {
			if (ao < bo)
				ao = canditer_next(&cia);
			else if (bo < ao)
				bo = canditer_next(&cib);
			else {
				*p++ = ao;
				ao = canditer_next(&cia);
				bo = canditer_next(&cib);
			}
		}
	}

	/* properties */
	BATsetcount(bn, (BUN) (p - (oid *) Tloc(bn, 0)));
	bn->trevsorted = BATcount(bn) <= 1;
	bn->tsorted = true;
	bn->tkey = true;
	bn->tnil = false;
	bn->tnonil = true;
	bn = virtualize(bn);
  doreturn:
	TRC_DEBUG(ALGO, ALGOBATFMT "," ALGOBATFMT " -> " ALGOOPTBATFMT "\n",
		  ALGOBATPAR(a), ALGOBATPAR(b), ALGOOPTBATPAR(bn));
	return bn;
}

/* calculate the difference of two candidate lists and produce a new one
 */
BAT *
BATdiffcand(BAT *a, BAT *b)
{
	BAT *bn;
	oid *restrict p;
	struct canditer cia, cib;

	BATcheck(a, NULL);
	BATcheck(b, NULL);

	canditer_init(&cia, NULL, a);
	canditer_init(&cib, NULL, b);

	if (cia.ncand == 0) {
		bn = BATdense(0, 0, 0);
		goto doreturn;
	}
	if (cib.ncand == 0) {
		bn = canditer_slice(&cia, 0, cia.ncand);
		goto doreturn;
	}

	if (cib.seq > canditer_last(&cia) || canditer_last(&cib) < cia.seq) {
		/* no overlap, return a */
		bn = canditer_slice(&cia, 0, cia.ncand);
		goto doreturn;
	}

	if (cia.tpe == cand_dense && cib.tpe == cand_dense) {
		/* both a and b are dense */
		if (cia.seq < cib.seq) {
			/* a starts before b */
			if (cia.seq + cia.ncand <= cib.seq + cib.ncand) {
				/* b overlaps with end of a */
				bn = canditer_slice(&cia, 0, cib.seq - cia.seq);
				goto doreturn;
			}
			/* b is subset of a */
			bn = canditer_slice2(&cia, 0, cib.seq - cia.seq,
					     cib.seq + cib.ncand - cia.seq,
					     cia.ncand);
			goto doreturn;
		} else {
			/* cia.seq >= cib.seq */
			if (cia.seq + cia.ncand > cib.seq + cib.ncand) {
				/* b overlaps with beginning of a */
				bn = canditer_slice(&cia, cib.seq + cib.ncand - cia.seq, cia.ncand);
				goto doreturn;
			}
			/* a is subset f b */
			bn = BATdense(0, 0, 0);
			goto doreturn;
		}
	}
	if (cib.tpe == cand_dense) {
		/* b is dense and a is not: we can copy the part of a
		 * that is before the start of b and the part of a
		 * that is after the end of b */
		bn = canditer_slice2val(&cia, oid_nil, cib.seq,
					cib.seq + cib.ncand, oid_nil);
		goto doreturn;
	}

	/* b is not dense */
	bn = COLnew(0, TYPE_oid, BATcount(a), TRANSIENT);
	if (bn == NULL)
		goto doreturn;
	p = Tloc(bn, 0);
	/* find first position in b that is in range of a */
	canditer_setidx(&cib, canditer_search(&cib, cia.seq, true));
	{
		/* because we may jump over this declaration, we put
		 * it inside a block */
		oid ob = canditer_next(&cib);
		for (BUN i = 0; i < cia.ncand; i++) {
			oid oa = canditer_next(&cia);
			while (!is_oid_nil(ob) && ob < oa) {
				ob = canditer_next(&cib);
			}
			if (!is_oid_nil(ob) && oa < ob)
				*p++ = oa;
		}
	}

	/* properties */
	BATsetcount(bn, (BUN) (p - (oid *) Tloc(bn, 0)));
	bn->trevsorted = BATcount(bn) <= 1;
	bn->tsorted = true;
	bn->tkey = true;
	bn->tnil = false;
	bn->tnonil = true;
	bn = virtualize(bn);
  doreturn:
	TRC_DEBUG(ALGO, ALGOBATFMT "," ALGOBATFMT " -> " ALGOOPTBATFMT "\n",
		  ALGOBATPAR(a), ALGOBATPAR(b), ALGOOPTBATPAR(bn));
	return bn;
}

/* return offset of first value in cand that is >= o */
static inline BUN
binsearchcand(const oid *cand, BUN hi, oid o)
{
	BUN lo = 0;

	if (o <= cand[lo])
		return 0;
	if (o > cand[hi])
		return hi + 1;
	/* loop invariant: cand[lo] < o <= cand[hi] */
	while (hi > lo + 1) {
		BUN mid = (lo + hi) / 2;
		if (cand[mid] == o)
			return mid;
		if (cand[mid] < o)
			lo = mid;
		else
			hi = mid;
	}
	return hi;
}

/* count number of 1 bits in ci->mask between bit positions lo
 * (inclusive) and hi (not inclusive) */
static BUN
count_mask_bits(struct canditer *ci, BUN lo, BUN hi)
{
	BUN n;
	assert(lo <= hi);
	assert(ci->tpe == cand_mask);
	if (lo == hi)
		return 0;
	lo += ci->firstbit;
	hi += ci->firstbit;
	BUN loi = lo / 32;
	BUN hii = hi / 32;
	lo %= 32;
	hi %= 32;
	if (loi == hii)
		return (BUN) candmask_pop((ci->mask[loi] & ((1U << hi) - 1)) >> lo);
	n = (BUN) candmask_pop(ci->mask[loi++] >> lo);
	while (loi < hii)
		n += (BUN) candmask_pop(ci->mask[loi++]);
	if (hi != 0)
		n += (BUN) candmask_pop(ci->mask[loi] & ((1U << hi) - 1));
	return n;
}

/* initialize a candidate iterator, return number of iterations */
BUN
canditer_init(struct canditer *ci, BAT *b, BAT *s)
{
	assert(ci != NULL);

	if (s == NULL) {
		if (b == NULL || BATcount(b) == 0) {
			/* trivially empty candidate list */
			*ci = (struct canditer) {
				.tpe = cand_dense,
			};
			return 0;
		}
		/* every row is a candidate */
		*ci = (struct canditer) {
			.tpe = cand_dense,
			.seq = b->hseqbase,
			.hseq = b->hseqbase,
			.ncand = BATcount(b),
		};
		return ci->ncand;
	}

	assert(ATOMtype(s->ttype) == TYPE_oid || s->ttype == TYPE_msk);
	assert(s->ttype == TYPE_msk|| s->tsorted);
	assert(s->ttype == TYPE_msk|| s->tkey);
	assert(s->ttype == TYPE_msk|| s->tnonil);
	assert(s->ttype == TYPE_void || s->tvheap == NULL);

	BUN cnt = BATcount(s);

	if (cnt == 0 || (b != NULL && BATcount(b) == 0)) {
		/* candidate list for empty BAT or empty candidate list */
		*ci = (struct canditer) {
			.tpe = cand_dense,
			.s = s,
		};
		return 0;
	}

	*ci = (struct canditer) {
		.seq = s->tseqbase,
		.hseq = s->hseqbase,
		.s = s,
	};

	if (mask_cand(s)) {
		ci->tpe = cand_mask;
		ci->mask = (const uint32_t *) ccand_first(s);
		ci->seq = s->hseqbase - CCAND(s)->firstbit;
		ci->hseq = ci->seq;
		ci->nvals = ccand_free(s) / sizeof(uint32_t);
		cnt = ci->nvals * 32;
	} else if (s->ttype == TYPE_msk) {
		assert(0);
		ci->tpe = cand_mask;
		ci->mask = (const uint32_t *) s->theap->base;
		ci->seq = s->hseqbase;
		ci->nvals = (cnt + 31U) / 32U;
	} else if (s->ttype == TYPE_void) {
		assert(!is_oid_nil(ci->seq));
		if (s->tvheap) {
			assert(ccand_free(s) % SIZEOF_OID == 0);
			ci->nvals = ccand_free(s) / SIZEOF_OID;
			if (ci->nvals > 0) {
				ci->tpe = cand_except;
				ci->oids = (const oid *) ccand_first(s);
			} else {
				/* why the vheap? */
				ci->tpe = cand_dense;
			}
		} else {
			ci->tpe = cand_dense;
		}
	} else if (is_oid_nil(ci->seq)) {
		ci->tpe = cand_materialized;
		ci->oids = (const oid *) Tloc(s, 0);
		ci->seq = ci->oids[0];
		ci->nvals = cnt;
	} else {
		/* materialized dense: no exceptions */
		ci->tpe = cand_dense;
	}
	switch (ci->tpe) {
	case cand_materialized:
		if (b != NULL) {
			BUN p = binsearchcand(ci->oids, cnt - 1U, b->hseqbase);
			/* p == cnt means candidate list is completely
			 * before b */
			ci->offset = p;
			ci->oids += p;
			cnt -= p;
			if (cnt > 0) {
				cnt = binsearchcand(ci->oids, cnt  - 1U,
						    b->hseqbase + BATcount(b));
				/* cnt == 0 means candidate list is
				 * completely after b */
			}
			if (cnt == 0) {
				/* no overlap */
				*ci = (struct canditer) {
					.tpe = cand_dense,
					.s = s,
				};
				return 0;
			}
			ci->seq = ci->oids[0];
			ci->nvals = cnt;
			if (ci->oids[cnt - 1U] - ci->seq == cnt - 1U) {
				/* actually dense */
				ci->tpe = cand_dense;
				ci->oids = NULL;
				ci->nvals = 0;
			}
		}
		break;
	case cand_except:
		/* exceptions must all be within range of s */
		assert(ci->oids[0] >= ci->seq);
		assert(ci->oids[ci->nvals - 1U] < ci->seq + cnt + ci->nvals);
		/* prune exceptions at either end of range of s */
		while (ci->nvals > 0 && ci->oids[0] == ci->seq) {
			ci->nvals--;
			ci->oids++;
			ci->seq++;
		}
		while (ci->nvals > 0 &&
		       ci->oids[ci->nvals - 1U] == ci->seq + cnt + ci->nvals - 1U)
			ci->nvals--;
		if (b != NULL) {
			if (ci->seq + cnt + ci->nvals <= b->hseqbase ||
			    ci->seq >= b->hseqbase + BATcount(b)) {
				/* candidate list does not overlap with b */
				*ci = (struct canditer) {
					.tpe = cand_dense,
					.s = s,
				};
				return 0;
			}
		}
		if (ci->nvals > 0) {
			if (b == NULL)
				break;
			BUN p;
			p = binsearchcand(ci->oids, ci->nvals - 1U, b->hseqbase);
			if (p == ci->nvals) {
				/* all exceptions before start of b */
				ci->offset = b->hseqbase - ci->seq - ci->nvals;
				cnt = ci->seq + cnt + ci->nvals - b->hseqbase;
				ci->seq = b->hseqbase;
				ci->nvals = 0;
				ci->tpe = cand_dense;
				ci->oids = NULL;
				break;
			}
			assert(b->hseqbase > ci->seq || p == 0);
			if (b->hseqbase > ci->seq) {
				/* skip candidates, possibly including
				 * exceptions */
				ci->oids += p;
				ci->nvals -= p;
				p = b->hseqbase - ci->seq - p;
				cnt -= p;
				ci->offset += p;
				ci->seq = b->hseqbase;
			}
			if (ci->seq + cnt + ci->nvals > b->hseqbase + BATcount(b)) {
				p = binsearchcand(ci->oids, ci->nvals - 1U,
						  b->hseqbase + BATcount(b));
				ci->nvals = p;
				cnt = b->hseqbase + BATcount(b) - ci->seq - ci->nvals;
			}
			while (ci->nvals > 0 && ci->oids[0] == ci->seq) {
				ci->nvals--;
				ci->oids++;
				ci->seq++;
			}
			while (ci->nvals > 0 &&
			       ci->oids[ci->nvals - 1U] == ci->seq + cnt + ci->nvals - 1U)
				ci->nvals--;
			if (ci->nvals > 0)
				break;
		}
		ci->tpe = cand_dense;
		ci->oids = NULL;
		/* fall through */
	case cand_dense:
		if (b != NULL) {
			if (ci->seq + cnt <= b->hseqbase ||
			    ci->seq >= b->hseqbase + BATcount(b)) {
				/* no overlap */
				*ci = (struct canditer) {
					.tpe = cand_dense,
					.s = s,
				};
				return 0;
			}
			if (b->hseqbase > ci->seq) {
				cnt -= b->hseqbase - ci->seq;
				ci->offset += b->hseqbase - ci->seq;
				ci->seq = b->hseqbase;
			}
			if (ci->seq + cnt > b->hseqbase + BATcount(b))
				cnt = b->hseqbase + BATcount(b) - ci->seq;
		}
		break;
	case cand_mask:
		if (b != NULL) {
			if (ci->seq + cnt <= b->hseqbase ||
			    ci->seq >= b->hseqbase + BATcount(b)) {
				/* no overlap */
				*ci = (struct canditer) {
					.tpe = cand_dense,
					.s = s,
				};
				return 0;
			}
			if (b->hseqbase > ci->seq) {
				cnt = b->hseqbase - ci->seq;
				ci->mask += cnt / 32U;
				ci->firstbit = (uint8_t) (cnt % 32U);
				cnt = BATcount(s) - cnt;
				ci->seq = b->hseqbase;
			}
			if (ci->seq + cnt > b->hseqbase + BATcount(b)) {
				cnt = b->hseqbase + BATcount(b) - ci->seq;
			}
			ci->nvals = (ci->firstbit + cnt + 31U) / 32U;
		}
		/* if first value is only partially used, check
		 * whether there are any 1 bits in the used part */
		if (ci->firstbit > 0 && (ci->mask[0] >> ci->firstbit) == 0) {
			if (cnt <= 32U - ci->firstbit) {
				cnt = 0;
				/* returns below */
			} else {
				cnt -= 32U - ci->firstbit;
				ci->firstbit = 0;
				ci->mask++;
				ci->nvals--;
			}
		}
		/* skip over any zero mask words that are used completely */
		if (ci->firstbit == 0) {
			while (cnt >= 32U && ci->mask[0] == 0) {
				cnt -= 32U;
				ci->mask++;
				ci->nvals--;
			}
		}
		/* check whether there are any 1 bits in the last word */
		if (cnt == 0 ||
		    (cnt < 32U - ci->firstbit &&
		     ((ci->mask[0] >> ci->firstbit) & ((1U << cnt) - 1U)) == 0)) {
			/* no one bits in the whole relevant portion
			 * of the bat */
			*ci = (struct canditer) {
				.tpe = cand_dense,
				.s = s,
			};
			return 0;
		}
		/* here we know there are 1 bits in the first mask
		 * word */
		int i = candmask_lobit(ci->mask[0] >> ci->firstbit);
		assert(i >= 0);	/* there should be a set bit */
		ci->firstbit += i;
		cnt -= i;
		if (mask_cand(s))
			ci->mskoff = s->hseqbase - CCAND(s)->firstbit + (ci->mask - (const uint32_t *) ccand_first(s)) * 32U;
		else
			ci->mskoff = s->hseqbase + (ci->mask - (const uint32_t *) s->theap->base) * 32U;
		ci->seq = ci->mskoff + ci->firstbit;
		ci->hseq = ci->seq;
		ci->nextbit = ci->firstbit;
		/* at this point we know that bit ci->firstbit is set
		 * in ci->mask[0] */
		ci->lastbit = (ci->firstbit + cnt - 1U) % 32U + 1U;
		if (ci->lastbit < 32 &&
		    (ci->mask[ci->nvals - 1] & ((1U << ci->lastbit) - 1)) == 0) {
			/* last partial word is all zero */
			cnt -= ci->lastbit;
			ci->lastbit = 32;
			ci->nvals--;
		}
		if (ci->lastbit == 32) {
			/* "remove" zero words at the end */
			while (cnt >= 32 && ci->mask[ci->nvals - 1] == 0) {
				ci->nvals--;
				cnt -= 32;
			}
		}
		ci->ncand = count_mask_bits(ci, 0, cnt);
		return ci->ncand;
	}
	ci->ncand = cnt;
	ci->hseq += ci->offset;
	return cnt;
}

/* return the next candidate without advancing */
oid
canditer_peek(struct canditer *ci)
{
	oid o = oid_nil;
	if (ci->next == ci->ncand)
		return oid_nil;
	switch (ci->tpe) {
	case cand_dense:
		o = ci->seq + ci->next;
		break;
	case cand_materialized:
		assert(ci->next < ci->nvals);
		o = ci->oids[ci->next];
		break;
	case cand_except:
		o = ci->seq + ci->add + ci->next;
		while (ci->add < ci->nvals && o == ci->oids[ci->add]) {
			ci->add++;
			o++;
		}
		break;
	case cand_mask:
		while ((ci->mask[ci->nextmsk] >> ci->nextbit) == 0) {
			ci->nextmsk++;
			ci->nextbit = 0;
		}
		ci->nextbit += candmask_lobit(ci->mask[ci->nextmsk] >> ci->nextbit);
		o = ci->mskoff + ci->nextmsk * 32 + ci->nextbit;
		break;
	}
	return o;
}

/* return the previous candidate */
oid
canditer_prev(struct canditer *ci)
{
	if (ci->next == 0)
		return oid_nil;
	switch (ci->tpe) {
	case cand_dense:
		return ci->seq + --ci->next;
	case cand_materialized:
		return ci->oids[--ci->next];
	case cand_except:
		break;
	case cand_mask:
		for (;;) {
			if (ci->nextbit == 0) {
				ci->nextbit = 32;
				while (ci->mask[--ci->nextmsk] == 0)
					;
			}
			if (ci->mask[ci->nextmsk] & (1U << --ci->nextbit))
				break;
		}
		ci->next--;
		return ci->mskoff + ci->nextmsk * 32 + ci->nextbit;
	}
	oid o = ci->seq + ci->add + --ci->next;
	while (ci->add > 0 && o == ci->oids[ci->add - 1]) {
		ci->add--;
		o--;
	}
	return o;
}

/* return the previous candidate without retreating */
oid
canditer_peekprev(struct canditer *ci)
{
	oid o = oid_nil;

	if (ci->next == 0)
		return oid_nil;
	switch (ci->tpe) {
	case cand_dense:
		return ci->seq + ci->next - 1;
	case cand_materialized:
		return ci->oids[ci->next - 1];
	case cand_except:
		o = ci->seq + ci->add + ci->next - 1;
		while (ci->add > 0 && o == ci->oids[ci->add - 1]) {
			ci->add--;
			o--;
		}
		break;
	case cand_mask:
		for (;;) {
			if (ci->nextbit == 0) {
				ci->nextbit = 32;
				while (ci->mask[--ci->nextmsk] == 0)
					;
			}
			if (ci->mask[ci->nextmsk] & (1U << --ci->nextbit))
				break;
		}
		o = ci->mskoff + ci->nextmsk * 32 + ci->nextbit;
		if (++ci->nextbit == 32) {
			ci->nextbit = 0;
			ci->nextmsk++;
		}
		break;
	}
	return o;
}

/* return the last candidate */
oid
canditer_last(struct canditer *ci)
{
	if (ci->ncand == 0)
		return oid_nil;
	switch (ci->tpe) {
	case cand_dense:
		return ci->seq + ci->ncand - 1;
	case cand_materialized:
		return ci->oids[ci->ncand - 1];
	case cand_except:
		/* work around compiler error: control reaches end of
		 * non-void function */
		break;
	case cand_mask:
		for (uint8_t i = ci->lastbit; i > 0; ) {
			if (ci->mask[ci->nvals - 1] & (1U << --i))
				return ci->mskoff + (ci->nvals - 1) * 32 + i;
		}
		return oid_nil;	/* cannot happen */
	}
	return ci->seq + ci->ncand + ci->nvals - 1;
}

/* return the candidate at the given index */
oid
canditer_idx(struct canditer *ci, BUN p)
{
	if (p >= ci->ncand)
		return oid_nil;
	switch (ci->tpe) {
	case cand_dense:
		return ci->seq + p;
	case cand_materialized:
		return ci->oids[p];
	case cand_except:
		/* work around compiler error: control reaches end of
		 * non-void function */
		break;
	case cand_mask: {
		BUN x;
		if ((x = candmask_pop(ci->mask[0] >> ci->firstbit)) > p) {
			for (uint8_t i = ci->firstbit; ; i++) {
				if (ci->mask[0] & (1U << i)) {
					if (p == 0)
						return ci->mskoff + i;
					p--;
				}
			}
		}
		for (BUN n = 1; n < ci->nvals; n++) {
			uint32_t mask = ci->mask[n];
			p -= x;
			x = candmask_pop(mask);
			if (x > p) {
				for (uint8_t i = 0; ; i++) {
					if (mask & (1U << i)) {
						if (p == 0)
							return ci->mskoff + n * 32 + i;
						p--;
					}
				}
			}
		}
		return oid_nil;	/* cannot happen */
	}
	}
	oid o = ci->seq + p;
	if (o < ci->oids[0])
		return o;
	if (o + ci->nvals > ci->oids[ci->nvals - 1])
		return o + ci->nvals;
	BUN lo = 0, hi = ci->nvals - 1;
	while (hi - lo > 1) {
		BUN mid = (hi + lo) / 2;
		if (ci->oids[mid] - mid > o)
			hi = mid;
		else
			lo = mid;
	}
	return o + hi;
}

/* set the index for the next candidate to be returned */
void
canditer_setidx(struct canditer *ci, BUN p)
{
	if (p != ci->next) {
		if (p >= ci->ncand) {
			ci->next = ci->ncand;
			switch (ci->tpe) {
			case cand_except:
				ci->add = ci->nvals;
				break;
			case cand_mask:
				ci->nextbit = ci->lastbit;
				ci->nextmsk = ci->nvals;
				if (ci->nextbit == 32)
					ci->nextbit = 0;
				else
					ci->nextmsk--;
				break;
			default:
				break;
			}
		} else {
			ci->next = p;
			switch (ci->tpe) {
			case cand_except:
				ci->add = canditer_idx(ci, p) - ci->seq - p;
				break;
			case cand_mask: {
				oid o = canditer_idx(ci, p) - ci->mskoff;
				ci->nextmsk = o / 32;
				ci->nextbit = (uint8_t) (o % 32);
				break;
			}
			default:
				break;
			}
		}
	}
}

/* reset */
void
canditer_reset(struct canditer *ci)
{
	if (ci->tpe == cand_mask) {
		ci->nextbit = ci->firstbit;
		ci->nextmsk = 0;
	} else {
		ci->add = 0;
	}
	ci->next = 0;
}

/* return index of given candidate if it occurs; if the candidate does
 * not occur, if next is set, return index of next larger candidate,
 * if next is not set, return BUN_NONE */
BUN
canditer_search(struct canditer *ci, oid o, bool next)
{
	BUN p;

	switch (ci->tpe) {
	case cand_dense:
		if (o < ci->seq)
			return next ? 0 : BUN_NONE;
		if (o >= ci->seq + ci->ncand)
			return next ? ci->ncand : BUN_NONE;
		return o - ci->seq;
	case cand_materialized:
		if (ci->nvals == 0)
			return 0;
		p = binsearchcand(ci->oids, ci->nvals - 1, o);
		if (!next && (p == ci->nvals || ci->oids[p] != o))
			return BUN_NONE;
		return p;
	case cand_except:
		break;
	case cand_mask:
		if (o < ci->mskoff) {
			return next ? 0 : BUN_NONE;
		}
		o -= ci->mskoff;
		p = o / 32;
		if (p >= ci->nvals)
			return next ? ci->ncand : BUN_NONE;
		o %= 32;
		if (p == ci->nvals - 1 && o >= ci->lastbit)
			return next ? ci->ncand : BUN_NONE;
		if (ci->mask[p] & (1U << o))
			return count_mask_bits(ci, 0, p);
		if (!next)
			return BUN_NONE;
		return count_mask_bits(ci, 0, p) + 1;
	}
	if (o < ci->seq)
		return next ? 0 : BUN_NONE;
	if (o >= ci->seq + ci->ncand + ci->nvals)
		return next ? ci->ncand : BUN_NONE;
	p = binsearchcand(ci->oids, ci->nvals - 1, o);
	if (next || p == ci->nvals || ci->oids[p] != o)
		return o - ci->seq - p;
	return BUN_NONE;
}

static BAT *
canditer_sliceval_mask(struct canditer *ci, oid lo1, oid hi1, BUN cnt1,
		       oid lo2, oid hi2, BUN cnt2)
{
	assert(cnt2 == 0 || !is_oid_nil(lo2));
	assert(cnt2 == 0 || lo2 >= hi1);
	if (is_oid_nil(lo1) || lo1 < ci->mskoff + ci->firstbit)
		lo1 = ci->mskoff + ci->firstbit;
	if (is_oid_nil(hi1) || hi1 >= ci->mskoff + (ci->nvals - 1) * 32 + ci->lastbit)
		hi1 = ci->mskoff + (ci->nvals - 1) * 32 + ci->lastbit;

	BAT *bn = COLnew(0, TYPE_oid, cnt1 + cnt2, TRANSIENT);
	if (bn == NULL)
		return NULL;
	bn->tkey = true;

	lo1 -= ci->mskoff;
	hi1 -= ci->mskoff;
	for (oid o = lo1; o < hi1 && cnt1 > 0; o++) {
		if (ci->mask[o / 32] & (1U << (o % 32))) {
			if (BUNappend(bn, &(oid){o + ci->mskoff}, false) != GDK_SUCCEED) {
				BBPreclaim(bn);
				return NULL;
			}
			cnt1--;
		}
	}
	if (cnt2 > 0) {
		if (lo2 < ci->mskoff + ci->firstbit)
			lo2 = ci->mskoff + ci->firstbit;
		if (is_oid_nil(hi2) || hi2 >= ci->mskoff + (ci->nvals - 1) * 32 + ci->lastbit)
			hi2 = ci->mskoff + (ci->nvals - 1) * 32 + ci->lastbit;

		lo2 -= ci->mskoff;
		hi2 -= ci->mskoff;
		for (oid o = lo2; o < hi2 && cnt2 > 0; o++) {
			if (ci->mask[o / 32] & (1U << (o % 32))) {
				if (BUNappend(bn, &(oid){o + ci->mskoff}, false) != GDK_SUCCEED) {
					BBPreclaim(bn);
					return NULL;
				}
				cnt2--;
			}
		}
	}
	return bn;
}

/* return either an actual BATslice or a new BAT that contains the
 * "virtual" slice of the input candidate list BAT; except, unlike
 * BATslice, the hseqbase of the returned BAT is 0; note for cand_mask
 * candidate iterators, the BUN values refer to number of 1 bits */
BAT *
canditer_slice(struct canditer *ci, BUN lo, BUN hi)
{
	BAT *bn;
	oid o;
	BUN add;

	assert(ci != NULL);

	if (lo >= ci->ncand || lo >= hi)
		return BATdense(0, 0, 0);
	if (hi > ci->ncand)
		hi = ci->ncand;
	switch (ci->tpe) {
	case cand_materialized:
		if (ci->s) {
			bn = BATslice(ci->s, lo + ci->offset, hi + ci->offset);
			BAThseqbase(bn, 0);
			return bn;
		}
		bn = COLnew(0, TYPE_oid, hi - lo, TRANSIENT);
		if (bn == NULL)
			return NULL;
		BATsetcount(bn, hi - lo);
		memcpy(Tloc(bn, 0), ci->oids + lo, (hi - lo) * sizeof(oid));
		break;
	default: /* really: case cand_dense: */
		return BATdense(0, ci->seq + lo, hi - lo);
	case cand_except:
		o = canditer_idx(ci, lo);
		add = o - ci->seq - lo;
		assert(add <= ci->nvals);
		if (add == ci->nvals || o + hi - lo < ci->oids[add]) {
			/* after last exception or before next
			 * exception: return dense sequence */
			return BATdense(0, o, hi - lo);
		}
		bn = COLnew(0, TYPE_oid, hi - lo, TRANSIENT);
		if (bn == NULL)
			return NULL;
		BATsetcount(bn, hi - lo);
		for (oid *dst = Tloc(bn, 0); lo < hi; lo++) {
			while (add < ci->nvals && o == ci->oids[add]) {
				o++;
				add++;
			}
			*dst++ = o++;
		}
		break;
	case cand_mask:
		return canditer_sliceval_mask(ci, canditer_idx(ci, lo),
					      oid_nil, hi - lo,
					      oid_nil, oid_nil, 0);
	}
	bn->tsorted = true;
	bn->trevsorted = BATcount(bn) <= 1;
	bn->tkey = true;
	bn->tseqbase = oid_nil;
	bn->tnil = false;
	bn->tnonil = true;
	return virtualize(bn);
}

/* like the above, except the bounds are given by values instead of
 * indexes */
BAT *
canditer_sliceval(struct canditer *ci, oid lo, oid hi)
{
	if (ci->tpe != cand_mask) {
		return canditer_slice(
			ci,
			is_oid_nil(lo) ? 0 : canditer_search(ci, lo, true),
			is_oid_nil(hi) ? ci->ncand : canditer_search(ci, hi, true));
	}

	return canditer_sliceval_mask(ci, lo, hi, ci->ncand,
				      oid_nil, oid_nil, 0);
}

/* return the combination of two slices */
BAT *
canditer_slice2(struct canditer *ci, BUN lo1, BUN hi1, BUN lo2, BUN hi2)
{
	BAT *bn;
	oid o;
	BUN add;

	assert(lo1 <= hi1);
	assert(lo2 <= hi2);
	assert(hi1 <= lo2 || (lo2 == 0 && hi2 == 0));

	if (hi1 == lo2)		/* consecutive slices: combine into one */
		return canditer_slice(ci, lo1, hi2);
	if (lo2 == hi2 || hi1 >= ci->ncand || lo2 >= ci->ncand) {
		/* empty second slice */
		return canditer_slice(ci, lo1, hi1);
	}
	if (lo1 == hi1)		/* empty first slice */
		return canditer_slice(ci, lo2, hi2);
	if (lo1 >= ci->ncand)	/* out of range */
		return BATdense(0, 0, 0);

	if (hi2 >= ci->ncand)
		hi2 = ci->ncand;

	bn = COLnew(0, TYPE_oid, hi1 - lo1 + hi2 - lo2, TRANSIENT);
	if (bn == NULL)
		return NULL;
	BATsetcount(bn, hi1 - lo1 + hi2 - lo2);
	bn->tsorted = true;
	bn->trevsorted = BATcount(bn) <= 1;
	bn->tkey = true;
	bn->tseqbase = oid_nil;
	bn->tnil = false;
	bn->tnonil = true;

	oid *dst = Tloc(bn, 0);

	switch (ci->tpe) {
	case cand_materialized:
		memcpy(dst, ci->oids + lo1, (hi1 - lo1) * sizeof(oid));
		memcpy(dst + hi1 - lo1, ci->oids + lo2, (hi2 - lo2) * sizeof(oid));
		break;
	case cand_dense:
		while (lo1 < hi1)
			*dst++ = ci->seq + lo1++;
		while (lo2 < hi2)
			*dst++ = ci->seq + lo2++;
		break;
	case cand_except:
		o = canditer_idx(ci, lo1);
		add = o - ci->seq - lo1;
		assert(add <= ci->nvals);
		if (add == ci->nvals) {
			/* after last exception: return dense sequence */
			while (lo1 < hi1)
				*dst++ = ci->seq + add + lo1++;
		} else {
			while (lo1 < hi1) {
				while (add < ci->nvals && o == ci->oids[add]) {
					o++;
					add++;
				}
				*dst++ = o++;
				lo1++;
			}
		}
		o = canditer_idx(ci, lo2);
		add = o - ci->seq - lo2;
		assert(add <= ci->nvals);
		if (add == ci->nvals) {
			/* after last exception: return dense sequence */
			while (lo2 < hi2)
				*dst++ = ci->seq + add + lo2++;
		} else {
			while (lo2 < hi2) {
				while (add < ci->nvals && o == ci->oids[add]) {
					o++;
					add++;
				}
				*dst++ = o++;
				lo2++;
			}
		}
		break;
	case cand_mask:
		return canditer_sliceval_mask(ci, canditer_idx(ci, lo1),
					      oid_nil, hi1 - lo1,
					      canditer_idx(ci, lo2),
					      oid_nil, hi2 - lo2);
	}
	return virtualize(bn);
}

BAT *
canditer_slice2val(struct canditer *ci, oid lo1, oid hi1, oid lo2, oid hi2)
{
	if (ci->tpe != cand_mask) {
		return canditer_slice2(
			ci,
			is_oid_nil(lo1) ? 0 : canditer_search(ci, lo1, true),
			is_oid_nil(hi1) ? ci->ncand : canditer_search(ci, hi1, true),
			is_oid_nil(lo2) ? 0 : canditer_search(ci, lo2, true),
			is_oid_nil(hi2) ? ci->ncand : canditer_search(ci, hi2, true));
	}

	return canditer_sliceval_mask(ci, lo1, hi1, ci->ncand,
				      lo2, hi2, ci->ncand);
}

gdk_return
BATnegcands(BAT *dense_cands, BAT *odels)
{
	const char *nme;
	Heap *dels;
	BUN lo, hi;
	ccand_t *c;

	assert(BATtdense(dense_cands));
	assert(dense_cands->ttype == TYPE_void);
	assert(dense_cands->batRole == TRANSIENT);

	if (BATcount(odels) == 0)
		return GDK_SUCCEED;

	lo = SORTfndfirst(odels, &dense_cands->tseqbase);
	hi = SORTfndfirst(odels, &(oid) {dense_cands->tseqbase + BATcount(dense_cands)});
	if (lo == hi)
		return GDK_SUCCEED;

	nme = BBP_physical(dense_cands->batCacheid);
	if ((dels = (Heap*)GDKzalloc(sizeof(Heap))) == NULL ||
	    (dels->farmid = BBPselectfarm(dense_cands->batRole, dense_cands->ttype, varheap)) < 0){
		GDKfree(dels);
		return GDK_FAIL;
	}
	strconcat_len(dels->filename, sizeof(dels->filename),
		      nme, ".theap", NULL);

    	if (HEAPalloc(dels, hi - lo + (sizeof(ccand_t)/sizeof(oid)), sizeof(oid), 0) != GDK_SUCCEED) {
		GDKfree(dels);
        	return GDK_FAIL;
	}
	ATOMIC_INIT(&dels->refs, 1);
	c = (ccand_t *) dels->base;
	*c = (ccand_t) {
		.type = CAND_NEGOID,
	};
    	dels->parentid = dense_cands->batCacheid;
	dels->free = sizeof(ccand_t) + sizeof(oid) * (hi - lo);
	if (odels->ttype == TYPE_void) {
		oid *r = (oid *) (dels->base + sizeof(ccand_t));
		for (BUN x = lo; x < hi; x++)
			r[x - lo] = x + odels->tseqbase;
	} else {
		oid *r = (oid *) (dels->base + sizeof(ccand_t));
		memcpy(r, Tloc(odels, lo), sizeof(oid) * (hi - lo));
	}
	dense_cands->batDirtydesc = true;
	dense_cands->tvheap = dels;
	BATsetcount(dense_cands, dense_cands->batCount - (hi - lo));
	TRC_DEBUG(ALGO, "BATnegcands(cands=" ALGOBATFMT ","
		  "dels=" ALGOBATFMT ")\n",
		  ALGOBATPAR(dense_cands),
		  ALGOBATPAR(odels));
    	return GDK_SUCCEED;
}

gdk_return
BATmaskedcands(BAT *dense_cands, BAT *masked, bool selected)
{
	const char *nme;
	Heap *msks;
	ccand_t *c;
	BUN nmask;

	assert(BATtdense(dense_cands));
	assert(dense_cands->ttype == TYPE_void);
	assert(dense_cands->batRole == TRANSIENT);
	assert(masked->ttype == TYPE_msk);

	if (BATcount(masked) == 0)
		return GDK_SUCCEED;

	nme = BBP_physical(dense_cands->batCacheid);
	if ((msks = (Heap*)GDKzalloc(sizeof(Heap))) == NULL ||
	    (msks->farmid = BBPselectfarm(dense_cands->batRole, dense_cands->ttype, varheap)) < 0){
		GDKfree(msks);
		return GDK_FAIL;
	}
	strconcat_len(msks->filename, sizeof(msks->filename),
		      nme, ".theap", NULL);

	nmask = (BATcount(masked) + 31) / 32;
    	if (HEAPalloc(msks, nmask + (sizeof(ccand_t)/sizeof(uint32_t)), sizeof(uint32_t), 0) != GDK_SUCCEED) {
		GDKfree(msks);
        	return GDK_FAIL;
	}
	c = (ccand_t *) msks->base;
	*c = (ccand_t) {
		.type = CAND_MSK,
		.mask = selected,
	};
    	msks->parentid = dense_cands->batCacheid;
	msks->free = sizeof(ccand_t) + nmask * sizeof(uint32_t);
	uint32_t *r = (uint32_t*)(msks->base + sizeof(ccand_t));
	memcpy(r, Tloc(masked, 0), nmask * sizeof(uint32_t));
	/* make sure last word doesn't have any spurious bits set */
	BUN cnt = BATcount(masked) % 32;
	if (cnt > 0)
		r[nmask - 1] &= (1U << cnt) - 1;
	cnt = 0;
	for (BUN i = 0; i < nmask; i++) {
		if (cnt == 0 && r[i] != 0)
			c->firstbit = candmask_lobit(r[i]) + i * 32;
		cnt += candmask_pop(r[i]);
	}
	if (cnt > 0) {
		ATOMIC_INIT(&msks->refs, 1);
		dense_cands->tvheap = msks;
		dense_cands->hseqbase += c->firstbit;
	} else {
		/* no point having a mask if it's empty */
		HEAPfree(msks, true);
		GDKfree(msks);
	}
	dense_cands->batDirtydesc = true;
	BATsetcount(dense_cands, cnt);
	TRC_DEBUG(ALGO, "BATmaskedcands(cands=" ALGOBATFMT ","
		  "masked=" ALGOBATFMT ")\n",
		  ALGOBATPAR(dense_cands),
		  ALGOBATPAR(masked));
    	return GDK_SUCCEED;
}

BAT *
BATunmask(BAT *b)
{
	BAT *bn = COLnew(0, TYPE_oid, mask_cand(b) ? BATcount(b) : 1024, TRANSIENT);
	if (bn == NULL)
		return NULL;

	assert(!mask_cand(b) || CCAND(b)->mask); /* todo handle negmask case */
	BUN cnt;
	uint32_t rem;
	uint32_t val;
	const uint32_t *src;
	oid *dst = (oid *) Tloc(bn, 0);
	BUN n = 0;
	oid hseq = b->hseqbase;

	if (mask_cand(b)) {
		cnt = ccand_free(b) / sizeof(uint32_t);
		rem = 0;
		src = (const uint32_t *) ccand_first(b);
		hseq -= CCAND(b)->firstbit;
	} else {
		cnt = BATcount(b) / 32;
		rem = BATcount(b) % 32;
		src = (const uint32_t *) Tloc(b, 0);
	}
	for (BUN p = 0; p < cnt; p++) {
		if ((val = src[p]) == 0)
			continue;
		for (uint32_t i = 0; i < 32; i++) {
			if (val & (1U << i)) {
				if (n == BATcapacity(bn)) {
					BATsetcount(bn, n);
					if (BATextend(bn, BATgrows(bn)) != GDK_SUCCEED) {
						BBPreclaim(bn);
						return NULL;
					}
					dst = (oid *) Tloc(bn, 0);
				}
				dst[n++] = hseq + p * 32 + i;
			}
		}
	}
	/* the last partial mask word */
	if (rem > 0 && (val = src[cnt]) != 0) {
		for (uint32_t i = 0; i < rem; i++) {
			if (val & (1U << i)) {
				if (n == BATcapacity(bn)) {
					BATsetcount(bn, n);
					if (BATextend(bn, BATgrows(bn)) != GDK_SUCCEED) {
						BBPreclaim(bn);
						return NULL;
					}
					dst = (oid *) Tloc(bn, 0);
				}
				dst[n++] = hseq + cnt * 32 + i;
			}
		}
	}
	BATsetcount(bn, n);
	bn->hseqbase = b->hseqbase;
	bn->tkey = true;
	bn->tsorted = true;
	bn->trevsorted = n <= 1;
	bn->tnil = false;
	bn->tnonil = true;
	return virtualize(bn);
}
