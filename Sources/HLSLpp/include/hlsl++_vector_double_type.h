#pragma once

#include "hlsl++_common.h"

#if defined(HLSLPP_DOUBLE)

HLSLPP_WARNINGS_IMPLICIT_CONSTRUCTOR_BEGIN

namespace hlslpp
{
	template<int X>
	struct hlslpp_nodiscard dswizzle1
	{
		template<int A> friend struct dswizzle1;

		hlslpp_inline operator double() const { return f64[X]; }

		template<int E, int A>
		static hlslpp_inline n128d swizzle(n128d v)
		{
			return _hlslpp_perm_pd(v, (((IdentityMask2 >> E) & 1) << A) | (IdentityMask2 & ~((1 << A))));
		}

		template<int E, int A>
		hlslpp_inline n128d swizzle() const
		{
			return swizzle<E % 2, A % 2>(vec[X / 2]);
		}

		// Assignment

		hlslpp_inline dswizzle1& operator = (double f)
		{
			vec[X / 2] = _hlslpp_blend_pd(vec[X / 2], _hlslpp_set1_pd(f), HLSLPP_COMPONENT_X(X % 2));
			return *this;
		}

		// Revise these functions. Can I not do with swizzle?

		template<int A>
		hlslpp_inline dswizzle1& operator = (const dswizzle1<A>& s)
		{
			n128d t = _hlslpp_shuffle_pd(s.vec[A / 2], s.vec[A / 2], HLSLPP_SHUFFLE_MASK_PD(A % 2, A % 2));
			vec[X / 2] = _hlslpp_blend_pd(vec[X / 2], t, HLSLPP_COMPONENT_X(X % 2));
			return *this;
		}

		hlslpp_inline dswizzle1& operator = (const dswizzle1<X>& s)
		{
			n128d t = _hlslpp_shuffle_pd(s.vec[X / 2], s.vec[X / 2], HLSLPP_SHUFFLE_MASK_PD(X % 2, X % 2));
			vec[X / 2] = _hlslpp_blend_pd(vec[X / 2], t, HLSLPP_COMPONENT_X(X % 2));
			return *this;
		}

		hlslpp_inline dswizzle1& operator = (const double1& f);

	private:

		union
		{
			n128d vec[X < 2 ? 1 : 2];
			double f64[X < 2 ? 2 : 4];
		};
	};

	template<int X, int Y>
	struct hlslpp_nodiscard dswizzle2
	{
		void staticAsserts()
		{
			static_assert(X != Y, "\"l-value specifies const object\" No component can be equal for assignment.");
		}

		template<int SrcA, int SrcB, int DstA, int DstB>
		static hlslpp_inline n128d swizzle(n128d vec0, n128d vec1)
		{
			// Select which vector to read from and how to build the mask based on the output
			#define HLSLPP_SELECT(Dst) ((Dst % 2) == 0 ? (SrcA < 2 ? vec0 : vec1) : (SrcB < 2 ? vec0 : vec1))
			n128d result = _hlslpp_shuffle_pd(HLSLPP_SELECT(DstA), HLSLPP_SELECT(DstB), HLSLPP_SHUFFLE_MASK_PD((DstA % 2) == 0 ? (SrcA % 2) : (SrcB % 2), (DstB % 2) == 0 ? (SrcA % 2) : (SrcB % 2)));
			#undef HLSLPP_SELECT
			return result;
		}

		template<int SrcA, int SrcB, int DstA, int DstB>
		hlslpp_inline n128d swizzle() const
		{
			// Select which vector to read from and how to build the mask based on the output
			#define HLSLPP_SELECT(Dst) (Dst % 2) == 0 ? vec[(SrcA < 2) ? 0 : 1] : vec[(SrcB < 2) ? 0 : 1]
			n128d result = _hlslpp_shuffle_pd(HLSLPP_SELECT(DstA), HLSLPP_SELECT(DstB), HLSLPP_SHUFFLE_MASK_PD((DstA % 2) == 0 ? (SrcA % 2) : (SrcB % 2), (DstB % 2) == 0 ? (SrcA % 2) : (SrcB % 2)));
			#undef HLSLPP_SELECT
			return result;
		}
	
		template<int E, int F>
		hlslpp_inline void swizzle_all(const dswizzle2<E, F>& s)
		{
			HLSLPP_CONSTEXPR_IF((X < 2 && Y < 2) || (X >= 2 && Y >= 2))
			{
				vec[(X < 2 && Y < 2) ? 0 : 1] = s.template swizzle<E, F, X, Y>();
			}
			else
			{
				// Swizzle E and F into both 0 and 1
				n128d swizzledE = s.template swizzle<E, E, 0, 1>();
				n128d swizzledF = s.template swizzle<F, F, 0, 1>();

				// Blend with original vectors to preserve contents in remaining entries
				vec[X / 2] = _hlslpp_blend_pd(vec[X / 2], swizzledE, HLSLPP_BLEND_MASK_PD((X % 2) == 1, (X % 2) == 0));
				vec[Y / 2] = _hlslpp_blend_pd(vec[Y / 2], swizzledF, HLSLPP_BLEND_MASK_PD((Y % 2) == 1, (Y % 2) == 0));
			}
		}

		// Assignment
	
		template<int E, int F>
		hlslpp_inline dswizzle2& operator = (const dswizzle2<E, F>& s)
		{
			staticAsserts();
			swizzle_all<E, F>(s);
			return *this;
		}

		hlslpp_inline dswizzle2& operator = (const dswizzle2<X, Y>& s)
		{
			staticAsserts();
			swizzle_all<X, Y>(s);
			return *this;
		}
	
		hlslpp_inline dswizzle2& operator = (const double2& f);
	
	private:
		union
		{
			n128d vec[(X < 2 && Y < 2) ? 1 : 2];
			double f64[(X < 2 && Y < 2) ? 2 : 4];
		};
	};

	template<int X, int Y, int Z>
	struct hlslpp_nodiscard dswizzle3
	{
		void staticAsserts()
		{
			static_assert(X != Y && X != Z && Y != Z, "\"l-value specifies const object\" No component can be equal for assignment.");
		}

#if defined(HLSLPP_SIMD_REGISTER_FLOAT8)

		template<int SrcA, int SrcB, int SrcC>
		hlslpp_inline void swizzle(n256d& ovec) const
		{
			swizzle<SrcA, SrcB, SrcC>(vec, ovec);
		}

		template<int SrcA, int SrcB, int SrcC, int DstA, int DstB, int DstC>
		hlslpp_inline void swizzleblend(n256d& ovec) const
		{
			swizzleblend<SrcA, SrcB, SrcC, DstA, DstB, DstC>(vec, ovec);
		}

#else

		template<int SrcA, int SrcB>
		hlslpp_inline n128d swizzle() const
		{
			return swizzle<SrcA, SrcB>(vec[0], vec[1]);
		}

		template<int SrcA, int SrcB, int SrcC>
		hlslpp_inline void swizzle(n128d& ovec0, n128d& ovec1) const
		{
			ovec0 = swizzle<SrcA, SrcB>();
			ovec1 = swizzle<SrcC, 0   >();
		}

		template<int SrcA, int SrcB, int SrcC, int DstA, int DstB, int DstC>
		hlslpp_inline void swizzleblend(n128d& ovec0, n128d& ovec1) const
		{
			swizzleblend<SrcA, SrcB, SrcC, DstA, DstB, DstC>(vec[0], vec[1], ovec0, ovec1);
		}

#endif

		template<int A, int B, int C>
		hlslpp_inline void swizzle_all(const dswizzle3<A, B, C>& s)
		{
#if defined(HLSLPP_SIMD_REGISTER_FLOAT8)
			s.template swizzleblend<A, B, C, X, Y, Z>(vec);
#else
			s.template swizzleblend<A, B, C, X, Y, Z>(vec[0], vec[1]);
#endif
		}

		// Assignment

		template<int A, int B, int C>
		hlslpp_inline dswizzle3& operator = (const dswizzle3<A, B, C>& s)
		{
			staticAsserts();
			swizzle_all<A, B, C>(s);
			return *this;
		}

		hlslpp_inline dswizzle3& operator = (const dswizzle3<X, Y, Z>& s)
		{
			staticAsserts();
			swizzle_all<X, Y, Z>(s);
			return *this;
		}

		hlslpp_inline dswizzle3& operator = (const double3& f);

	private:
#if defined(HLSLPP_SIMD_REGISTER_FLOAT8)

		// Swizzles SrcA into 0, SrcB into 1 and SrcC into 2
		// This version doesn't blend so only works for dswizzle3 -> double3 conversions
		template<int SrcA, int SrcB, int SrcC>
		static hlslpp_inline void swizzle(n256d vec, n256d& ovec)
		{
			ovec = _hlslpp256_perm_pd(vec, SrcA, SrcB, SrcC, SrcC);
		}

		// Swizzles SrcA, SrcB, SrcC into DstA, DstB, DstC
		template<int SrcA, int SrcB, int SrcC, int DstA, int DstB, int DstC>
		static hlslpp_inline void swizzleblend(n256d vec, n256d& ovec)
		{
			#define HLSLPP_SELECT(x) (DstA == x ? SrcA : (DstB == x ? SrcB : SrcC))
			#define HLSLPP_BLEND(x) ((DstA == x || DstB == x || DstC == x) ? 1 : 0)
			n256d perm = _hlslpp256_perm_pd(vec, HLSLPP_SELECT(0), HLSLPP_SELECT(1), HLSLPP_SELECT(2), HLSLPP_SELECT(3));
			ovec = _hlslpp256_blend_pd(perm, ovec, HLSLPP_BLEND_MASK(HLSLPP_BLEND(0), HLSLPP_BLEND(1), HLSLPP_BLEND(2), HLSLPP_BLEND(3)));
			#undef HLSLPP_SELECT
			#undef HLSLPP_BLEND
		}

#else

		// Swizzles SrcA into position 0 and SrcB into position 1
		template<int SrcA, int SrcB>
		static hlslpp_inline n128d swizzle(n128d vec0, n128d vec1)
		{
			return _hlslpp_shuffle_pd(SrcA < 2 ? vec0 : vec1, SrcB < 2 ? vec0 : vec1, HLSLPP_SHUFFLE_MASK_PD(SrcA % 2, SrcB % 2));
		}

		// Swizzles SrcA into 0, SrcB into 1 and SrcC into 2
		// This version doesn't blend so only works for dswizzle3 -> double3 conversions
		template<int SrcA, int SrcB, int SrcC>
		static hlslpp_inline void swizzle(n128d vec0, n128d vec1, n128d& ovec0, n128d& ovec1)
		{
			ovec0 = swizzle<SrcA, SrcB>(vec0, vec1);
			ovec1 = swizzle<SrcC, 0   >(vec0, vec1);
		}

		// Swizzles SrcA into DstA, SrcB into DstB and SrcC into DstC
		// Needs to blend to be able to preserve the remaining component
		template<int SrcA, int SrcB, int SrcC, int DstA, int DstB, int DstC>
		static hlslpp_inline void swizzleblend(n128d vec0, n128d vec1, n128d& ovec0, n128d& ovec1)
		{
			#define HLSLPP_SELECT(x) (DstA == x ? SrcA : (DstB == x ? SrcB : SrcC))
			#define HLSLPP_BLEND(x) ((DstA == x || DstB == x || DstC == x) ? 1 : 0)

			n128d swizzle0 = swizzle<HLSLPP_SELECT(0), HLSLPP_SELECT(1)>(vec0, vec1);
			n128d swizzle1 = swizzle<HLSLPP_SELECT(2), HLSLPP_SELECT(3)>(vec0, vec1);

			ovec0 = _hlslpp_blend_pd(swizzle0, ovec0, HLSLPP_BLEND_MASK_PD(HLSLPP_BLEND(0), HLSLPP_BLEND(1)));
			ovec1 = _hlslpp_blend_pd(swizzle1, ovec1, HLSLPP_BLEND_MASK_PD(HLSLPP_BLEND(2), HLSLPP_BLEND(3)));

			#undef HLSLPP_SELECT
			#undef HLSLPP_BLEND
		}

#endif

		union
		{
#if defined(HLSLPP_SIMD_REGISTER_FLOAT8)
			n256d vec;
#else
			n128d vec[2];
#endif
			double f64[4];
		};
	};

	template<int X, int Y, int Z, int W>
	struct hlslpp_nodiscard dswizzle4
	{
		void staticAsserts()
		{
			static_assert(X != Y && X != Z && X != W && Y != Z && Y != W && Z != W, "\"l-value specifies const object\" No component can be equal for assignment.");
		}

#if defined(HLSLPP_SIMD_REGISTER_FLOAT8)

		template<int SrcA, int SrcB, int SrcC, int SrcD>
		hlslpp_inline void swizzle(n256d& ovec) const
		{
			swizzle<SrcA, SrcB, SrcC, SrcD>(vec, ovec);
		}

		template<int SrcA, int SrcB, int SrcC, int SrcD, int DstA, int DstB, int DstC, int DstD>
		hlslpp_inline void swizzle(n256d& ovec) const
		{
			swizzle<SrcA, SrcB, SrcC, SrcD, DstA, DstB, DstC, DstD>(vec, ovec);
		}

#else

		template<int SrcA, int SrcB>
		hlslpp_inline n128d swizzle() const
		{
			return swizzle<SrcA, SrcB>(vec[0], vec[1]);
		}

		template<int SrcA, int SrcB, int SrcC, int SrcD, int DstA, int DstB, int DstC, int DstD>
		hlslpp_inline void swizzle(n128d& ovec0, n128d& ovec1) const
		{
			swizzle<SrcA, SrcB, SrcC, SrcD, DstA, DstB, DstC, DstD>(vec[0], vec[1], ovec0, ovec1);
		}

#endif

		template<int A, int B, int C, int D>
		hlslpp_inline void swizzle_all(const dswizzle4<A, B, C, D>& s)
		{
#if defined(HLSLPP_SIMD_REGISTER_FLOAT8)
			s. template swizzle<A, B, C, D, X, Y, Z, W>(vec);
#else
			s. template swizzle<A, B, C, D, X, Y, Z, W>(vec[0], vec[1]);
#endif
		}

		// Assignment
		
		template<int A, int B, int C, int D>
		hlslpp_inline dswizzle4& operator = (const dswizzle4<A, B, C, D>& s)
		{
			staticAsserts();
			swizzle_all<A, B, C, D>(s);
			return *this;
		}
		
		hlslpp_inline dswizzle4& operator = (const double4& f);

	private:
#if defined(HLSLPP_SIMD_REGISTER_FLOAT8)

		// Swizzles SrcA into 0, SrcB into 1 and SrcC into 2
		// This version doesn't blend so only works for dswizzle3 -> double3 conversions
		template<int SrcA, int SrcB, int SrcC, int SrcD>
		static hlslpp_inline void swizzle(n256d vec, n256d& ovec)
		{
			ovec = _hlslpp256_perm_pd(vec, SrcA, SrcB, SrcC, SrcD);
		}

		// Swizzles SrcA, SrcB, SrcC into DstA, DstB, DstC
		template<int SrcA, int SrcB, int SrcC, int SrcD, int DstA, int DstB, int DstC, int DstD>
		static hlslpp_inline void swizzle(n256d vec, n256d& ovec)
		{
			#define HLSLPP_SELECT(x) DstA == x ? SrcA : (DstB == x ? SrcB : (DstC == x ? SrcC : SrcD))
			ovec = _hlslpp256_perm_pd(vec, HLSLPP_SELECT(0), HLSLPP_SELECT(1), HLSLPP_SELECT(2), HLSLPP_SELECT(3));
			#undef HLSLPP_SELECT
		}

#else

		template<int SrcA, int SrcB>
		static hlslpp_inline n128d swizzle(n128d vec0, n128d vec1)
		{
			return _hlslpp_shuffle_pd(SrcA < 2 ? vec0 : vec1, SrcB < 2 ? vec0 : vec1, HLSLPP_SHUFFLE_MASK_PD(SrcA % 2, SrcB % 2));
		}

		template<int SrcA, int SrcB, int SrcC, int SrcD, int DstA, int DstB, int DstC, int DstD>
		static hlslpp_inline void swizzle(n128d vec0, n128d vec1, n128d& ovec0, n128d& ovec1)
		{
			#define HLSLPP_SELECT(x) DstA == x ? SrcA : (DstB == x ? SrcB : (DstC == x ? SrcC : SrcD))

			ovec0 = swizzle<HLSLPP_SELECT(0), HLSLPP_SELECT(1)>(vec0, vec1);
			ovec1 = swizzle<HLSLPP_SELECT(2), HLSLPP_SELECT(3)>(vec0, vec1);

			#undef HLSLPP_SELECT
		}

#endif

		union
		{
#if defined(HLSLPP_SIMD_REGISTER_FLOAT8)
			n256d vec;
#else
			n128d vec[2];
#endif
			double f64[4];
		};
	};

	//-------------//
	// Double type //
	//-------------//

	struct hlslpp_nodiscard double1
	{
		hlslpp_inline double1() : vec(_hlslpp_setzero_pd()) {}
		hlslpp_inline double1(const double1& f) : vec(f.vec) {}
		explicit hlslpp_inline double1(n128d vec) : vec(vec) {}

		template<typename T>
		hlslpp_inline double1(T f, hlslpp_enable_if_number(T)) : vec(_hlslpp_set_pd(double(f), 0.0)) {}

		template<int X> hlslpp_inline double1(const dswizzle1<X>& s) : vec(s.template swizzle<X, 0>()) {}

		//double1(const int1& i);

		hlslpp_inline double1& operator = (const double1& f) { vec = f.vec; return *this; }

		operator double() const { return f64[0]; }

		HLSLPP_WARNING_ANONYMOUS_STRUCT_UNION_BEGIN
		union
		{
			n128d vec;
			double f64[2];
			// -------------------------------
// XYZW
// -------------------------------

hlslpp_swizzle_start dswizzle1<0> x; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<0, 0> xx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 0, 0> xxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 0, 0> xxxx; hlslpp_swizzle_end

// -------------------------------
// RGBA
// -------------------------------

hlslpp_swizzle_start dswizzle1<0> r; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<0, 0> rr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 0, 0> rrr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 0, 0> rrrr; hlslpp_swizzle_end
		};
		HLSLPP_WARNING_ANONYMOUS_STRUCT_UNION_END
	};

	struct hlslpp_nodiscard double2
	{
		// Constructors

		hlslpp_inline double2() : vec(_hlslpp_setzero_pd()) {}
		hlslpp_inline double2(const double2& f) : vec(f.vec) {}
		explicit hlslpp_inline double2(n128d vec) : vec(vec) {}
		explicit hlslpp_inline double2(const double1& f) : vec(_hlslpp_perm_xx_pd(f.vec)) {}

		template<typename T>
		hlslpp_inline double2(T f, hlslpp_enable_if_number(T)) : vec(_hlslpp_set_pd(double(f), double(f))) {}

		template<typename T1, typename T2>
		hlslpp_inline double2(T1 f1, T2 f2, hlslpp_enable_if_number_2(T1, T2)) : vec(_hlslpp_set_pd(double(f1), double(f2))) {}

		hlslpp_inline double2(const double1& f1, const double1& f2) { vec = _hlslpp_blend_pd(f1.vec, _hlslpp_perm_xx_pd(f2.vec), HLSLPP_BLEND_MASK_PD(1, 0)); }
		
		template<int X, int Y> hlslpp_inline double2(const dswizzle2<X, Y>& s) : vec(s.template swizzle<X, Y, 0, 1>()) {}

		//double2(const int2& i);

		hlslpp_inline double2& operator = (const double2& f) { vec = f.vec; return *this; }

		HLSLPP_WARNING_ANONYMOUS_STRUCT_UNION_BEGIN
		union
		{
			n128d vec;
			double f64[2];
			// -------------------------------
// XYZW
// -------------------------------

hlslpp_swizzle_start dswizzle1<0> x; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<0, 0> xx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 0, 0> xxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 0, 0> xxxx; hlslpp_swizzle_end

// -------------------------------
// RGBA
// -------------------------------

hlslpp_swizzle_start dswizzle1<0> r; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<0, 0> rr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 0, 0> rrr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 0, 0> rrrr; hlslpp_swizzle_end
			// -------------------------------
// XYZW
// -------------------------------

hlslpp_swizzle_start dswizzle1<1> y; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle2<0, 1> xy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<1, 0> yx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<1, 1> yy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<0, 0, 1> xxy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 1, 0> xyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 1, 1> xyy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<1, 0, 0> yxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 0, 1> yxy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 1, 0> yyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 1, 1> yyy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 0, 0, 1> xxxy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 0, 1, 0> xxyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 1, 1> xxyy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 1, 0, 0> xyxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 0, 1> xyxy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 1, 1, 0> xyyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 1, 1> xyyy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 0, 0, 0> yxxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 0, 1> yxxy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 0, 1, 0> yxyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 1, 1> yxyy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 1, 0, 0> yyxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 0, 1> yyxy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 1, 1, 0> yyyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 1, 1> yyyy; hlslpp_swizzle_end

// -------------------------------
// RGBA
// -------------------------------

hlslpp_swizzle_start dswizzle1<1> g; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle2<0, 1> rg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<1, 0> gr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<1, 1> gg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<0, 0, 1> rrg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 1, 0> rgr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 1, 1> rgg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<1, 0, 0> grr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 0, 1> grg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 1, 0> ggr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 1, 1> ggg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 0, 0, 1> rrrg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 0, 1, 0> rrgr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 1, 1> rrgg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 1, 0, 0> rgrr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 0, 1> rgrg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 1, 1, 0> rggr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 1, 1> rggg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 0, 0, 0> grrr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 0, 1> grrg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 0, 1, 0> grgr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 1, 1> grgg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 1, 0, 0> ggrr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 0, 1> ggrg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 1, 1, 0> gggr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 1, 1> gggg; hlslpp_swizzle_end
		};
		HLSLPP_WARNING_ANONYMOUS_STRUCT_UNION_END
	};

	struct hlslpp_nodiscard double3
	{
		// Constructors

#if defined(HLSLPP_SIMD_REGISTER_FLOAT8)

		hlslpp_inline double3() : vec(_hlslpp256_setzero_pd()) {}
		hlslpp_inline double3(const double3& f) : vec(f.vec) {}
		explicit hlslpp_inline double3(n256d vec) : vec(vec) {}
		explicit hlslpp_inline double3(const double1& f) : vec(_hlslpp256_set128_pd(_hlslpp_perm_xx_pd(f.vec), _hlslpp_perm_xx_pd(f.vec))) {}

		template<typename T>
		hlslpp_inline double3(T f, hlslpp_enable_if_number(T)) : vec(_hlslpp256_set_pd(double(f), double(f), double(f), 0.0)) {}

		template<typename T1, typename T2, typename T3>
		hlslpp_inline double3(T1 f1, T2 f2, T3 f3, hlslpp_enable_if_number_3(T1, T2, T3)) : vec(_hlslpp256_set_pd(double(f1), double(f2), double(f3), 0.0)) {}

		hlslpp_inline double3& operator = (const double3& f) { vec = f.vec; return *this; }

#else

		hlslpp_inline double3() : vec0(_hlslpp_setzero_pd()), vec1(_hlslpp_setzero_pd()) {}
		hlslpp_inline double3(const double3& f) : vec0(f.vec0), vec1(f.vec1) {}
		explicit hlslpp_inline double3(n128d vec0, n128d vec1) : vec0(vec0), vec1(vec1) {}
		explicit hlslpp_inline double3(const double1& f) : vec0(_hlslpp_perm_xx_pd(f.vec)), vec1(_hlslpp_perm_xx_pd(f.vec)) {}

		template<typename T>
		hlslpp_inline double3(T f, hlslpp_enable_if_number(T)) : vec0(_hlslpp_set_pd(double(f), double(f))), vec1(_hlslpp_set_pd(double(f), 0.0)) {}

		template<typename T1, typename T2, typename T3>
		hlslpp_inline double3(T1 f1, T2 f2, T3 f3, hlslpp_enable_if_number_3(T1, T2, T3)) : vec0(_hlslpp_set_pd(double(f1), double(f2))), vec1(_hlslpp_set_pd(double(f3), 0.0)) {}

		hlslpp_inline double3& operator = (const double3& f) { vec0 = f.vec0; vec1 = f.vec1; return *this; }

#endif

		hlslpp_inline double3(const double1& f1, const double1& f2, const double1& f3)
		{
#if defined(HLSLPP_SIMD_REGISTER_FLOAT8)
			n128d f1f2 = _hlslpp_shuf_xx_pd(f1.vec, f2.vec);
			vec = _hlslpp256_set128_pd(f1f2, f3.vec);
#else
			vec0 = _hlslpp_shuf_xx_pd(f1.vec, f2.vec);
			vec1 = f3.vec;
#endif
		}

		hlslpp_inline double3(const double2& f1, const double1& f2)
		{
#if defined(HLSLPP_SIMD_REGISTER_FLOAT8)
			vec = _hlslpp256_set128_pd(f1.vec, f2.vec);
#else
			vec0 = f1.vec;
			vec1 = f2.vec;
#endif
		}

		hlslpp_inline double3(const double1& f1, const double2& f2)
		{
#if defined(HLSLPP_SIMD_REGISTER_FLOAT8)
			n128d f1f2x = _hlslpp_shuf_xx_pd(f1.vec, f2.vec);
			n128d f2y = _hlslpp_perm_yx_pd(f2.vec);
			vec = _hlslpp256_set128_pd(f1f2x, f2y);
#else
			vec0 = _hlslpp_shuf_xx_pd(f1.vec, f2.vec);
			vec1 = _hlslpp_perm_yx_pd(f2.vec);
#endif
		}

		template<int X, int Y, int Z>
		hlslpp_inline double3(const dswizzle3<X, Y, Z>& s)
		{
#if defined(HLSLPP_SIMD_REGISTER_FLOAT8)
			s.template swizzle<X, Y, Z>(vec);
#else
			s.template swizzle<X, Y, Z>(vec0, vec1);
#endif
		}
		
		//float3(const int3& i);

		HLSLPP_WARNING_ANONYMOUS_STRUCT_UNION_BEGIN
		union
		{
#if defined(HLSLPP_SIMD_REGISTER_FLOAT8)

			n256d vec;

#else

			HLSLPP_WARNING_ANONYMOUS_STRUCT_UNION_BEGIN
			struct
			{
				n128d vec0;
				n128d vec1;
			};
			HLSLPP_WARNING_ANONYMOUS_STRUCT_UNION_END

#endif

			double f64[4];
			// -------------------------------
// XYZW
// -------------------------------

hlslpp_swizzle_start dswizzle1<0> x; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<0, 0> xx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 0, 0> xxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 0, 0> xxxx; hlslpp_swizzle_end

// -------------------------------
// RGBA
// -------------------------------

hlslpp_swizzle_start dswizzle1<0> r; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<0, 0> rr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 0, 0> rrr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 0, 0> rrrr; hlslpp_swizzle_end
			// -------------------------------
// XYZW
// -------------------------------

hlslpp_swizzle_start dswizzle1<1> y; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle2<0, 1> xy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<1, 0> yx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<1, 1> yy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<0, 0, 1> xxy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 1, 0> xyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 1, 1> xyy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<1, 0, 0> yxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 0, 1> yxy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 1, 0> yyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 1, 1> yyy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 0, 0, 1> xxxy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 0, 1, 0> xxyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 1, 1> xxyy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 1, 0, 0> xyxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 0, 1> xyxy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 1, 1, 0> xyyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 1, 1> xyyy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 0, 0, 0> yxxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 0, 1> yxxy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 0, 1, 0> yxyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 1, 1> yxyy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 1, 0, 0> yyxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 0, 1> yyxy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 1, 1, 0> yyyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 1, 1> yyyy; hlslpp_swizzle_end

// -------------------------------
// RGBA
// -------------------------------

hlslpp_swizzle_start dswizzle1<1> g; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle2<0, 1> rg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<1, 0> gr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<1, 1> gg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<0, 0, 1> rrg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 1, 0> rgr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 1, 1> rgg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<1, 0, 0> grr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 0, 1> grg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 1, 0> ggr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 1, 1> ggg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 0, 0, 1> rrrg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 0, 1, 0> rrgr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 1, 1> rrgg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 1, 0, 0> rgrr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 0, 1> rgrg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 1, 1, 0> rggr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 1, 1> rggg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 0, 0, 0> grrr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 0, 1> grrg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 0, 1, 0> grgr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 1, 1> grgg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 1, 0, 0> ggrr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 0, 1> ggrg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 1, 1, 0> gggr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 1, 1> gggg; hlslpp_swizzle_end
			// -------------------------------
// XYZW
// -------------------------------

hlslpp_swizzle_start dswizzle1<2> z; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle2<0, 2> xz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<1, 2> yz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<2, 0> zx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<2, 1> zy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<2, 2> zz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<0, 0, 2> xxz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 1, 2> xyz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 2, 0> xzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 2, 1> xzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 2, 2> xzz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<1, 0, 2> yxz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 1, 2> yyz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 2, 0> yzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 2, 1> yzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 2, 2> yzz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<2, 0, 0> zxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 0, 1> zxy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 0, 2> zxz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 1, 0> zyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 1, 1> zyy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 1, 2> zyz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<2, 2, 0> zzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 2, 1> zzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 2, 2> zzz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 0, 0, 2> xxxz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 1, 2> xxyz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 0, 2, 0> xxzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 2, 1> xxzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 2, 2> xxzz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 1, 0, 2> xyxz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 1, 2> xyyz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 1, 2, 0> xyzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 2, 1> xyzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 2, 2> xyzz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 2, 0, 0> xzxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 0, 1> xzxy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 0, 2> xzxz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 2, 1, 0> xzyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 1, 1> xzyy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 1, 2> xzyz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 2, 2, 0> xzzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 2, 1> xzzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 2, 2> xzzz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 0, 0, 2> yxxz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 1, 2> yxyz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 2, 0> yxzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 2, 1> yxzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 2, 2> yxzz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 1, 0, 2> yyxz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 1, 2> yyyz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 1, 2, 0> yyzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 2, 1> yyzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 2, 2> yyzz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 2, 0, 0> yzxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 0, 1> yzxy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 0, 2> yzxz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 2, 1, 0> yzyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 1, 1> yzyy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 1, 2> yzyz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 2, 2, 0> yzzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 2, 1> yzzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 2, 2> yzzz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 0, 0, 0> zxxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 0, 1> zxxy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 0, 2> zxxz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 0, 1, 0> zxyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 1, 1> zxyy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 1, 2> zxyz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 0, 2, 0> zxzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 2, 1> zxzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 2, 2> zxzz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 1, 0, 0> zyxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 0, 1> zyxy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 0, 2> zyxz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 1, 1, 0> zyyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 1, 1> zyyy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 1, 2> zyyz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 1, 2, 0> zyzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 2, 1> zyzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 2, 2> zyzz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 2, 0, 0> zzxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 0, 1> zzxy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 0, 2> zzxz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 2, 1, 0> zzyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 1, 1> zzyy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 1, 2> zzyz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 2, 2, 0> zzzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 2, 1> zzzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 2, 2> zzzz; hlslpp_swizzle_end

// -------------------------------
// RGBA
// -------------------------------

hlslpp_swizzle_start dswizzle1<2> b; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle2<0, 2> rb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<1, 2> gb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<2, 0> br; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<2, 1> bg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<2, 2> bb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<0, 0, 2> rrb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 1, 2> rgb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 2, 0> rbr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 2, 1> rbg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 2, 2> rbb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<1, 0, 2> grb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 1, 2> ggb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 2, 0> gbr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 2, 1> gbg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 2, 2> gbb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<2, 0, 0> brr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 0, 1> brg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 0, 2> brb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 1, 0> bgr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 1, 1> bgg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 1, 2> bgb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<2, 2, 0> bbr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 2, 1> bbg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 2, 2> bbb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 0, 0, 2> rrrb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 1, 2> rrgb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 0, 2, 0> rrbr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 2, 1> rrbg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 2, 2> rrbb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 1, 0, 2> rgrb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 1, 2> rggb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 1, 2, 0> rgbr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 2, 1> rgbg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 2, 2> rgbb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 2, 0, 0> rbrr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 0, 1> rbrg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 0, 2> rbrb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 2, 1, 0> rbgr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 1, 1> rbgg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 1, 2> rbgb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 2, 2, 0> rbbr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 2, 1> rbbg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 2, 2> rbbb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 0, 0, 2> grrb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 1, 2> grgb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 2, 0> grbr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 2, 1> grbg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 2, 2> grbb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 1, 0, 2> ggrb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 1, 2> gggb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 1, 2, 0> ggbr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 2, 1> ggbg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 2, 2> ggbb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 2, 0, 0> gbrr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 0, 1> gbrg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 0, 2> gbrb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 2, 1, 0> gbgr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 1, 1> gbgg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 1, 2> gbgb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 2, 2, 0> gbbr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 2, 1> gbbg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 2, 2> gbbb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 0, 0, 0> brrr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 0, 1> brrg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 0, 2> brrb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 0, 1, 0> brgr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 1, 1> brgg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 1, 2> brgb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 0, 2, 0> brbr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 2, 1> brbg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 2, 2> brbb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 1, 0, 0> bgrr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 0, 1> bgrg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 0, 2> bgrb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 1, 1, 0> bggr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 1, 1> bggg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 1, 2> bggb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 1, 2, 0> bgbr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 2, 1> bgbg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 2, 2> bgbb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 2, 0, 0> bbrr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 0, 1> bbrg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 0, 2> bbrb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 2, 1, 0> bbgr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 1, 1> bbgg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 1, 2> bbgb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 2, 2, 0> bbbr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 2, 1> bbbg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 2, 2> bbbb; hlslpp_swizzle_end
		};
		HLSLPP_WARNING_ANONYMOUS_STRUCT_UNION_END
	};

	struct hlslpp_nodiscard double4
	{
#if defined(HLSLPP_SIMD_REGISTER_FLOAT8)

		hlslpp_inline double4() : vec(_hlslpp256_setzero_pd()) {}
		hlslpp_inline double4(const double4& f) : vec(f.vec) {}
		explicit hlslpp_inline double4(n256d vec) : vec(vec) {}
		explicit hlslpp_inline double4(const double1& f) : vec(_hlslpp256_set128_pd(_hlslpp_perm_xx_pd(f.vec), _hlslpp_perm_xx_pd(f.vec))) {}

		template<typename T>
		hlslpp_inline double4(T f, hlslpp_enable_if_number(T)) : vec(_hlslpp256_set1_pd(double(f))) {}

		template<typename T1, typename T2, typename T3, typename T4>
		hlslpp_inline double4(T1 f1, T2 f2, T3 f3, T4 f4, hlslpp_enable_if_number_4(T1, T2, T3, T4)) : vec(_hlslpp256_set_pd(double(f1), double(f2), double(f3), double(f4))) {}

		hlslpp_inline double4& operator = (const double4& f) { vec = f.vec; return *this; }

#else

		hlslpp_inline double4() : vec0(_hlslpp_setzero_pd()), vec1(_hlslpp_setzero_pd()) {}
		hlslpp_inline double4(const double4& f) : vec0(f.vec0), vec1(f.vec1) {}
		explicit hlslpp_inline double4(n128d vec0, n128d vec1) : vec0(vec0), vec1(vec1) {}
		explicit hlslpp_inline double4(const double1& f) : vec0(_hlslpp_perm_xx_pd(f.vec)), vec1(_hlslpp_perm_xx_pd(f.vec)) {}

		template<typename T>
		hlslpp_inline double4(T f, hlslpp_enable_if_number(T)) : vec0(_hlslpp_set1_pd(double(f))), vec1(_hlslpp_set1_pd(double(f))) {}

		template<typename T1, typename T2, typename T3, typename T4>
		hlslpp_inline double4(T1 f1, T2 f2, T3 f3, T4 f4, hlslpp_enable_if_number_4(T1, T2, T3, T4)) : vec0(_hlslpp_set_pd(double(f1), double(f2))), vec1(_hlslpp_set_pd(double(f3), double(f4))) {}

		hlslpp_inline double4& operator = (const double4& f) { vec0 = f.vec0; vec1 = f.vec1; return *this; }

#endif

		hlslpp_inline double4(const double1& f1, const double1& f2, const double1& f3, const double1& f4)
		{
#if defined(HLSLPP_SIMD_REGISTER_FLOAT8)
			n128d f1f2 = _hlslpp_shuf_xx_pd(f1.vec, f2.vec);
			n128d f3f4 = _hlslpp_shuf_xx_pd(f3.vec, f4.vec);
			vec = _hlslpp256_set128_pd(f1f2, f3f4);
#else
			vec0 = _hlslpp_shuf_xx_pd(f1.vec, f2.vec);
			vec1 = _hlslpp_shuf_xx_pd(f3.vec, f4.vec);
#endif
		}

		hlslpp_inline double4(const double2& f1, const double1& f2, const double1& f3)
		{
#if defined(HLSLPP_SIMD_REGISTER_FLOAT8)
			vec = _hlslpp256_set128_pd(f1.vec, _hlslpp_shuf_xx_pd(f2.vec, f3.vec));
#else
			vec0 = f1.vec;
			vec1 = _hlslpp_shuf_xx_pd(f2.vec, f3.vec);
#endif
		}

		hlslpp_inline double4(const double1& f1, const double2& f2, const double1& f3)
		{
#if defined(HLSLPP_SIMD_REGISTER_FLOAT8)
			vec = _hlslpp256_set128_pd(_hlslpp_shuf_xx_pd(f1.vec, f2.vec), _hlslpp_shuf_yx_pd(f2.vec, f3.vec));
#else
			vec0 = _hlslpp_shuf_xx_pd(f1.vec, f2.vec);
			vec1 = _hlslpp_shuf_yx_pd(f2.vec, f3.vec);
#endif
		}

		hlslpp_inline double4(const double1& f1, const double1& f2, const double2& f3)
		{
#if defined(HLSLPP_SIMD_REGISTER_FLOAT8)
			vec = _hlslpp256_set128_pd(_hlslpp_shuf_xx_pd(f1.vec, f2.vec), f3.vec);
#else
			vec0 = _hlslpp_shuf_xx_pd(f1.vec, f2.vec);
			vec1 = f3.vec;
#endif
		}

		hlslpp_inline double4(const double2& f1, const double2& f2)
		{
#if defined(HLSLPP_SIMD_REGISTER_FLOAT8)
			vec = _hlslpp256_set128_pd(f1.vec, f2.vec);
#else
			vec0 = f1.vec;
			vec1 = f2.vec;
#endif
		}

		hlslpp_inline double4(const double1& f1, const double3& f2)
		{
#if defined(HLSLPP_SIMD_REGISTER_FLOAT8)
			vec = _hlslpp256_set128_pd(_hlslpp_shuf_xx_pd(f1.vec, _hlslpp256_low_pd(f2.vec)), _hlslpp_shuf_yx_pd(_hlslpp256_low_pd(f2.vec), _hlslpp256_high_pd(f2.vec)));
#else
			vec0 = _hlslpp_shuf_xx_pd(f1.vec, f2.vec0);
			vec1 = _hlslpp_shuf_yx_pd(f2.vec0, f2.vec1);
#endif
		}

		hlslpp_inline double4(const double3& f1, const double1& f2)
		{
#if defined(HLSLPP_SIMD_REGISTER_FLOAT8)

			vec = _hlslpp256_set128_pd(_hlslpp256_low_pd(f1.vec), _hlslpp_shuf_xx_pd(_hlslpp256_high_pd(f1.vec), f2.vec));
#else
			vec0 = f1.vec0;
			vec1 = _hlslpp_shuf_xx_pd(f1.vec1, f2.vec);
#endif
		}
		
		template<int X, int Y, int Z, int W>
		hlslpp_inline double4(const dswizzle4<X, Y, Z, W>& s)
		{
#if defined(HLSLPP_SIMD_REGISTER_FLOAT8)
			s.template swizzle<X, Y, Z, W, 0, 1, 2, 3>(vec);
#else
			s.template swizzle<X, Y, Z, W, 0, 1, 2, 3>(vec0, vec1);
#endif
		}

		//double4(const int4& i);

		HLSLPP_WARNING_ANONYMOUS_STRUCT_UNION_BEGIN
		union
		{
#if defined(HLSLPP_SIMD_REGISTER_FLOAT8)
			n256d vec;
#else
			HLSLPP_WARNING_ANONYMOUS_STRUCT_UNION_BEGIN
			struct
			{
				n128d vec0;
				n128d vec1;
			};
			HLSLPP_WARNING_ANONYMOUS_STRUCT_UNION_END

			n128d vec[2];
#endif
			double f64[4];
			// -------------------------------
// XYZW
// -------------------------------

hlslpp_swizzle_start dswizzle1<0> x; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<0, 0> xx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 0, 0> xxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 0, 0> xxxx; hlslpp_swizzle_end

// -------------------------------
// RGBA
// -------------------------------

hlslpp_swizzle_start dswizzle1<0> r; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<0, 0> rr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 0, 0> rrr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 0, 0> rrrr; hlslpp_swizzle_end
			// -------------------------------
// XYZW
// -------------------------------

hlslpp_swizzle_start dswizzle1<1> y; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle2<0, 1> xy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<1, 0> yx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<1, 1> yy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<0, 0, 1> xxy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 1, 0> xyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 1, 1> xyy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<1, 0, 0> yxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 0, 1> yxy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 1, 0> yyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 1, 1> yyy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 0, 0, 1> xxxy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 0, 1, 0> xxyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 1, 1> xxyy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 1, 0, 0> xyxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 0, 1> xyxy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 1, 1, 0> xyyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 1, 1> xyyy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 0, 0, 0> yxxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 0, 1> yxxy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 0, 1, 0> yxyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 1, 1> yxyy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 1, 0, 0> yyxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 0, 1> yyxy; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 1, 1, 0> yyyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 1, 1> yyyy; hlslpp_swizzle_end

// -------------------------------
// RGBA
// -------------------------------

hlslpp_swizzle_start dswizzle1<1> g; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle2<0, 1> rg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<1, 0> gr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<1, 1> gg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<0, 0, 1> rrg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 1, 0> rgr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 1, 1> rgg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<1, 0, 0> grr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 0, 1> grg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 1, 0> ggr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 1, 1> ggg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 0, 0, 1> rrrg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 0, 1, 0> rrgr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 1, 1> rrgg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 1, 0, 0> rgrr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 0, 1> rgrg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 1, 1, 0> rggr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 1, 1> rggg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 0, 0, 0> grrr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 0, 1> grrg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 0, 1, 0> grgr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 1, 1> grgg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 1, 0, 0> ggrr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 0, 1> ggrg; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 1, 1, 0> gggr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 1, 1> gggg; hlslpp_swizzle_end
			// -------------------------------
// XYZW
// -------------------------------

hlslpp_swizzle_start dswizzle1<2> z; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle2<0, 2> xz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<1, 2> yz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<2, 0> zx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<2, 1> zy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<2, 2> zz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<0, 0, 2> xxz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 1, 2> xyz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 2, 0> xzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 2, 1> xzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 2, 2> xzz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<1, 0, 2> yxz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 1, 2> yyz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 2, 0> yzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 2, 1> yzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 2, 2> yzz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<2, 0, 0> zxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 0, 1> zxy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 0, 2> zxz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 1, 0> zyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 1, 1> zyy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 1, 2> zyz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<2, 2, 0> zzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 2, 1> zzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 2, 2> zzz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 0, 0, 2> xxxz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 1, 2> xxyz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 0, 2, 0> xxzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 2, 1> xxzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 2, 2> xxzz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 1, 0, 2> xyxz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 1, 2> xyyz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 1, 2, 0> xyzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 2, 1> xyzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 2, 2> xyzz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 2, 0, 0> xzxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 0, 1> xzxy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 0, 2> xzxz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 2, 1, 0> xzyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 1, 1> xzyy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 1, 2> xzyz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 2, 2, 0> xzzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 2, 1> xzzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 2, 2> xzzz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 0, 0, 2> yxxz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 1, 2> yxyz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 2, 0> yxzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 2, 1> yxzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 2, 2> yxzz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 1, 0, 2> yyxz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 1, 2> yyyz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 1, 2, 0> yyzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 2, 1> yyzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 2, 2> yyzz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 2, 0, 0> yzxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 0, 1> yzxy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 0, 2> yzxz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 2, 1, 0> yzyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 1, 1> yzyy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 1, 2> yzyz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 2, 2, 0> yzzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 2, 1> yzzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 2, 2> yzzz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 0, 0, 0> zxxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 0, 1> zxxy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 0, 2> zxxz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 0, 1, 0> zxyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 1, 1> zxyy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 1, 2> zxyz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 0, 2, 0> zxzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 2, 1> zxzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 2, 2> zxzz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 1, 0, 0> zyxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 0, 1> zyxy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 0, 2> zyxz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 1, 1, 0> zyyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 1, 1> zyyy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 1, 2> zyyz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 1, 2, 0> zyzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 2, 1> zyzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 2, 2> zyzz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 2, 0, 0> zzxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 0, 1> zzxy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 0, 2> zzxz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 2, 1, 0> zzyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 1, 1> zzyy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 1, 2> zzyz; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 2, 2, 0> zzzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 2, 1> zzzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 2, 2> zzzz; hlslpp_swizzle_end

// -------------------------------
// RGBA
// -------------------------------

hlslpp_swizzle_start dswizzle1<2> b; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle2<0, 2> rb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<1, 2> gb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<2, 0> br; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<2, 1> bg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<2, 2> bb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<0, 0, 2> rrb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 1, 2> rgb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 2, 0> rbr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 2, 1> rbg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 2, 2> rbb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<1, 0, 2> grb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 1, 2> ggb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 2, 0> gbr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 2, 1> gbg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 2, 2> gbb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<2, 0, 0> brr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 0, 1> brg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 0, 2> brb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 1, 0> bgr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 1, 1> bgg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 1, 2> bgb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<2, 2, 0> bbr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 2, 1> bbg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 2, 2> bbb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 0, 0, 2> rrrb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 1, 2> rrgb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 0, 2, 0> rrbr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 2, 1> rrbg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 2, 2> rrbb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 1, 0, 2> rgrb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 1, 2> rggb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 1, 2, 0> rgbr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 2, 1> rgbg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 2, 2> rgbb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 2, 0, 0> rbrr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 0, 1> rbrg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 0, 2> rbrb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 2, 1, 0> rbgr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 1, 1> rbgg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 1, 2> rbgb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 2, 2, 0> rbbr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 2, 1> rbbg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 2, 2> rbbb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 0, 0, 2> grrb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 1, 2> grgb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 2, 0> grbr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 2, 1> grbg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 2, 2> grbb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 1, 0, 2> ggrb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 1, 2> gggb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 1, 2, 0> ggbr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 2, 1> ggbg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 2, 2> ggbb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 2, 0, 0> gbrr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 0, 1> gbrg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 0, 2> gbrb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 2, 1, 0> gbgr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 1, 1> gbgg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 1, 2> gbgb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 2, 2, 0> gbbr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 2, 1> gbbg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 2, 2> gbbb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 0, 0, 0> brrr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 0, 1> brrg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 0, 2> brrb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 0, 1, 0> brgr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 1, 1> brgg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 1, 2> brgb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 0, 2, 0> brbr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 2, 1> brbg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 2, 2> brbb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 1, 0, 0> bgrr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 0, 1> bgrg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 0, 2> bgrb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 1, 1, 0> bggr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 1, 1> bggg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 1, 2> bggb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 1, 2, 0> bgbr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 2, 1> bgbg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 2, 2> bgbb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 2, 0, 0> bbrr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 0, 1> bbrg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 0, 2> bbrb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 2, 1, 0> bbgr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 1, 1> bbgg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 1, 2> bbgb; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 2, 2, 0> bbbr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 2, 1> bbbg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 2, 2> bbbb; hlslpp_swizzle_end
			// -------------------------------
// XYZW
// -------------------------------

hlslpp_swizzle_start dswizzle1<3> w; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle2<0, 3> xw; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<1, 3> yw; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<2, 3> zw; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<3, 0> wx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<3, 1> wy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<3, 2> wz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<3, 3> ww; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<0, 0, 3> xxw; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 1, 3> xyw; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 2, 3> xzw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<0, 3, 0> xwx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 3, 1> xwy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 3, 2> xwz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 3, 3> xww; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<1, 0, 3> yxw; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 1, 3> yyw; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 2, 3> yzw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<1, 3, 0> ywx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 3, 1> ywy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 3, 2> ywz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 3, 3> yww; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<2, 0, 3> zxw; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 1, 3> zyw; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 2, 3> zzw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<2, 3, 0> zwx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 3, 1> zwy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 3, 2> zwz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 3, 3> zww; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<3, 0, 0> wxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<3, 0, 1> wxy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<3, 0, 2> wxz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<3, 0, 3> wxw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<3, 1, 0> wyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<3, 1, 1> wyy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<3, 1, 2> wyz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<3, 1, 3> wyw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<3, 2, 0> wzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<3, 2, 1> wzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<3, 2, 2> wzz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<3, 2, 3> wzw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<3, 3, 0> wwx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<3, 3, 1> wwy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<3, 3, 2> wwz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<3, 3, 3> www; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 0, 0, 3> xxxw; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 1, 3> xxyw; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 2, 3> xxzw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 0, 3, 0> xxwx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 3, 1> xxwy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 3, 2> xxwz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 3, 3> xxww; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 1, 0, 3> xyxw; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 1, 3> xyyw; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 2, 3> xyzw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 1, 3, 0> xywx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 3, 1> xywy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 3, 2> xywz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 3, 3> xyww; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 2, 0, 3> xzxw; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 1, 3> xzyw; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 2, 3> xzzw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 2, 3, 0> xzwx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 3, 1> xzwy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 3, 2> xzwz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 3, 3> xzww; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 3, 0, 0> xwxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 3, 0, 1> xwxy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 3, 0, 2> xwxz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 3, 0, 3> xwxw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 3, 1, 0> xwyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 3, 1, 1> xwyy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 3, 1, 2> xwyz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 3, 1, 3> xwyw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 3, 2, 0> xwzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 3, 2, 1> xwzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 3, 2, 2> xwzz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 3, 2, 3> xwzw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 3, 3, 0> xwwx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 3, 3, 1> xwwy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 3, 3, 2> xwwz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 3, 3, 3> xwww; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 0, 0, 3> yxxw; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 1, 3> yxyw; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 2, 3> yxzw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 0, 3, 0> yxwx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 3, 1> yxwy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 3, 2> yxwz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 3, 3> yxww; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 1, 0, 3> yyxw; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 1, 3> yyyw; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 2, 3> yyzw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 1, 3, 0> yywx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 3, 1> yywy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 3, 2> yywz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 3, 3> yyww; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 2, 0, 3> yzxw; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 1, 3> yzyw; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 2, 3> yzzw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 2, 3, 0> yzwx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 3, 1> yzwy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 3, 2> yzwz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 3, 3> yzww; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 3, 0, 0> ywxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 3, 0, 1> ywxy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 3, 0, 2> ywxz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 3, 0, 3> ywxw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 3, 1, 0> ywyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 3, 1, 1> ywyy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 3, 1, 2> ywyz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 3, 1, 3> ywyw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 3, 2, 0> ywzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 3, 2, 1> ywzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 3, 2, 2> ywzz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 3, 2, 3> ywzw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 3, 3, 0> ywwx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 3, 3, 1> ywwy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 3, 3, 2> ywwz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 3, 3, 3> ywww; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 0, 0, 3> zxxw; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 1, 3> zxyw; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 2, 3> zxzw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 0, 3, 0> zxwx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 3, 1> zxwy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 3, 2> zxwz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 3, 3> zxww; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 1, 0, 3> zyxw; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 1, 3> zyyw; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 2, 3> zyzw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 1, 3, 0> zywx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 3, 1> zywy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 3, 2> zywz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 3, 3> zyww; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 2, 0, 3> zzxw; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 1, 3> zzyw; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 2, 3> zzzw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 2, 3, 0> zzwx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 3, 1> zzwy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 3, 2> zzwz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 3, 3> zzww; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 3, 0, 0> zwxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 3, 0, 1> zwxy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 3, 0, 2> zwxz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 3, 0, 3> zwxw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 3, 1, 0> zwyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 3, 1, 1> zwyy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 3, 1, 2> zwyz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 3, 1, 3> zwyw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 3, 2, 0> zwzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 3, 2, 1> zwzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 3, 2, 2> zwzz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 3, 2, 3> zwzw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 3, 3, 0> zwwx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 3, 3, 1> zwwy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 3, 3, 2> zwwz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 3, 3, 3> zwww; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 0, 0, 0> wxxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 0, 0, 1> wxxy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 0, 0, 2> wxxz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 0, 0, 3> wxxw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 0, 1, 0> wxyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 0, 1, 1> wxyy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 0, 1, 2> wxyz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 0, 1, 3> wxyw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 0, 2, 0> wxzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 0, 2, 1> wxzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 0, 2, 2> wxzz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 0, 2, 3> wxzw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 0, 3, 0> wxwx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 0, 3, 1> wxwy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 0, 3, 2> wxwz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 0, 3, 3> wxww; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 1, 0, 0> wyxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 1, 0, 1> wyxy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 1, 0, 2> wyxz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 1, 0, 3> wyxw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 1, 1, 0> wyyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 1, 1, 1> wyyy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 1, 1, 2> wyyz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 1, 1, 3> wyyw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 1, 2, 0> wyzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 1, 2, 1> wyzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 1, 2, 2> wyzz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 1, 2, 3> wyzw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 1, 3, 0> wywx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 1, 3, 1> wywy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 1, 3, 2> wywz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 1, 3, 3> wyww; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 2, 0, 0> wzxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 2, 0, 1> wzxy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 2, 0, 2> wzxz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 2, 0, 3> wzxw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 2, 1, 0> wzyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 2, 1, 1> wzyy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 2, 1, 2> wzyz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 2, 1, 3> wzyw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 2, 2, 0> wzzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 2, 2, 1> wzzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 2, 2, 2> wzzz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 2, 2, 3> wzzw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 2, 3, 0> wzwx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 2, 3, 1> wzwy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 2, 3, 2> wzwz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 2, 3, 3> wzww; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 3, 0, 0> wwxx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 3, 0, 1> wwxy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 3, 0, 2> wwxz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 3, 0, 3> wwxw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 3, 1, 0> wwyx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 3, 1, 1> wwyy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 3, 1, 2> wwyz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 3, 1, 3> wwyw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 3, 2, 0> wwzx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 3, 2, 1> wwzy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 3, 2, 2> wwzz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 3, 2, 3> wwzw; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 3, 3, 0> wwwx; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 3, 3, 1> wwwy; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 3, 3, 2> wwwz; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 3, 3, 3> wwww; hlslpp_swizzle_end

// -------------------------------
// RGBA
// -------------------------------

hlslpp_swizzle_start dswizzle1<3> a; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle2<0, 3> ra; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<1, 3> ga; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<2, 3> ba; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<3, 0> ar; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<3, 1> ag; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<3, 2> ab; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle2<3, 3> aa; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<0, 0, 3> rra; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 1, 3> rga; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 2, 3> rba; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<0, 3, 0> rar; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 3, 1> rag; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 3, 2> rab; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<0, 3, 3> raa; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<1, 0, 3> gra; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 1, 3> gga; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 2, 3> gba; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<1, 3, 0> gar; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 3, 1> gag; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 3, 2> gab; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<1, 3, 3> gaa; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<2, 0, 3> bra; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 1, 3> bga; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 2, 3> bba; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<2, 3, 0> bar; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 3, 1> bag; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 3, 2> bab; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<2, 3, 3> baa; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<3, 0, 0> arr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<3, 0, 1> arg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<3, 0, 2> arb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<3, 0, 3> ara; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<3, 1, 0> agr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<3, 1, 1> agg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<3, 1, 2> agb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<3, 1, 3> aga; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<3, 2, 0> abr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<3, 2, 1> abg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<3, 2, 2> abb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<3, 2, 3> aba; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle3<3, 3, 0> aar; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<3, 3, 1> aag; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<3, 3, 2> aab; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle3<3, 3, 3> aaa; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 0, 0, 3> rrra; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 1, 3> rrga; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 2, 3> rrba; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 0, 3, 0> rrar; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 3, 1> rrag; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 3, 2> rrab; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 0, 3, 3> rraa; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 1, 0, 3> rgra; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 1, 3> rgga; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 2, 3> rgba; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 1, 3, 0> rgar; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 3, 1> rgag; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 3, 2> rgab; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 1, 3, 3> rgaa; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 2, 0, 3> rbra; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 1, 3> rbga; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 2, 3> rbba; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 2, 3, 0> rbar; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 3, 1> rbag; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 3, 2> rbab; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 2, 3, 3> rbaa; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 3, 0, 0> rarr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 3, 0, 1> rarg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 3, 0, 2> rarb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 3, 0, 3> rara; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 3, 1, 0> ragr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 3, 1, 1> ragg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 3, 1, 2> ragb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 3, 1, 3> raga; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 3, 2, 0> rabr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 3, 2, 1> rabg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 3, 2, 2> rabb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 3, 2, 3> raba; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<0, 3, 3, 0> raar; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 3, 3, 1> raag; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 3, 3, 2> raab; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<0, 3, 3, 3> raaa; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 0, 0, 3> grra; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 1, 3> grga; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 2, 3> grba; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 0, 3, 0> grar; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 3, 1> grag; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 3, 2> grab; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 0, 3, 3> graa; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 1, 0, 3> ggra; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 1, 3> ggga; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 2, 3> ggba; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 1, 3, 0> ggar; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 3, 1> ggag; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 3, 2> ggab; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 1, 3, 3> ggaa; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 2, 0, 3> gbra; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 1, 3> gbga; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 2, 3> gbba; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 2, 3, 0> gbar; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 3, 1> gbag; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 3, 2> gbab; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 2, 3, 3> gbaa; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 3, 0, 0> garr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 3, 0, 1> garg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 3, 0, 2> garb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 3, 0, 3> gara; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 3, 1, 0> gagr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 3, 1, 1> gagg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 3, 1, 2> gagb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 3, 1, 3> gaga; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 3, 2, 0> gabr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 3, 2, 1> gabg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 3, 2, 2> gabb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 3, 2, 3> gaba; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<1, 3, 3, 0> gaar; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 3, 3, 1> gaag; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 3, 3, 2> gaab; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<1, 3, 3, 3> gaaa; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 0, 0, 3> brra; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 1, 3> brga; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 2, 3> brba; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 0, 3, 0> brar; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 3, 1> brag; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 3, 2> brab; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 0, 3, 3> braa; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 1, 0, 3> bgra; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 1, 3> bgga; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 2, 3> bgba; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 1, 3, 0> bgar; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 3, 1> bgag; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 3, 2> bgab; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 1, 3, 3> bgaa; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 2, 0, 3> bbra; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 1, 3> bbga; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 2, 3> bbba; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 2, 3, 0> bbar; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 3, 1> bbag; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 3, 2> bbab; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 2, 3, 3> bbaa; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 3, 0, 0> barr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 3, 0, 1> barg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 3, 0, 2> barb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 3, 0, 3> bara; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 3, 1, 0> bagr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 3, 1, 1> bagg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 3, 1, 2> bagb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 3, 1, 3> baga; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 3, 2, 0> babr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 3, 2, 1> babg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 3, 2, 2> babb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 3, 2, 3> baba; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<2, 3, 3, 0> baar; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 3, 3, 1> baag; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 3, 3, 2> baab; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<2, 3, 3, 3> baaa; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 0, 0, 0> arrr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 0, 0, 1> arrg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 0, 0, 2> arrb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 0, 0, 3> arra; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 0, 1, 0> argr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 0, 1, 1> argg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 0, 1, 2> argb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 0, 1, 3> arga; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 0, 2, 0> arbr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 0, 2, 1> arbg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 0, 2, 2> arbb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 0, 2, 3> arba; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 0, 3, 0> arar; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 0, 3, 1> arag; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 0, 3, 2> arab; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 0, 3, 3> araa; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 1, 0, 0> agrr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 1, 0, 1> agrg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 1, 0, 2> agrb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 1, 0, 3> agra; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 1, 1, 0> aggr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 1, 1, 1> aggg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 1, 1, 2> aggb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 1, 1, 3> agga; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 1, 2, 0> agbr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 1, 2, 1> agbg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 1, 2, 2> agbb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 1, 2, 3> agba; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 1, 3, 0> agar; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 1, 3, 1> agag; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 1, 3, 2> agab; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 1, 3, 3> agaa; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 2, 0, 0> abrr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 2, 0, 1> abrg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 2, 0, 2> abrb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 2, 0, 3> abra; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 2, 1, 0> abgr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 2, 1, 1> abgg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 2, 1, 2> abgb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 2, 1, 3> abga; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 2, 2, 0> abbr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 2, 2, 1> abbg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 2, 2, 2> abbb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 2, 2, 3> abba; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 2, 3, 0> abar; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 2, 3, 1> abag; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 2, 3, 2> abab; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 2, 3, 3> abaa; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 3, 0, 0> aarr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 3, 0, 1> aarg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 3, 0, 2> aarb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 3, 0, 3> aara; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 3, 1, 0> aagr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 3, 1, 1> aagg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 3, 1, 2> aagb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 3, 1, 3> aaga; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 3, 2, 0> aabr; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 3, 2, 1> aabg; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 3, 2, 2> aabb; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 3, 2, 3> aaba; hlslpp_swizzle_end

hlslpp_swizzle_start dswizzle4<3, 3, 3, 0> aaar; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 3, 3, 1> aaag; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 3, 3, 2> aaab; hlslpp_swizzle_end
hlslpp_swizzle_start dswizzle4<3, 3, 3, 3> aaaa; hlslpp_swizzle_end
		};
		HLSLPP_WARNING_ANONYMOUS_STRUCT_UNION_END
	};
};

HLSLPP_WARNINGS_IMPLICIT_CONSTRUCTOR_BEGIN

#endif