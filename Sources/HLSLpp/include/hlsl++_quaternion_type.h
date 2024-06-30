#pragma once

#include "hlsl++_common.h"

namespace hlslpp
{
	struct hlslpp_nodiscard quaternion
	{
		quaternion() hlslpp_noexcept : vec(_hlslpp_setzero_ps()) {}
		quaternion(const quaternion& q) hlslpp_noexcept : vec(q.vec) {}
		explicit quaternion(n128 vec) hlslpp_noexcept : vec(vec) {}

		template<typename T1, typename T2, typename T3, typename T4>
		quaternion(T1 f1, T2 f2, T3 f3, T4 f4, hlslpp_enable_if_number_4(T1, T2, T3, T4)) 
			hlslpp_noexcept : vec(_hlslpp_set_ps(float(f1), float(f2), float(f3), float(f4))) {}

		quaternion(const float1& f1, const float1& f2, const float1& f3, const float1& f4) 
		hlslpp_noexcept { vec = _hlslpp_blend_ps(_hlslpp_shuf_xxxx_ps(f1.vec, f3.vec), _hlslpp_shuf_xxxx_ps(f2.vec, f4.vec), HLSLPP_BLEND_MASK(1, 0, 1, 0)); }

		quaternion(const float2& f1, const float1& f2, const float1& f3) hlslpp_noexcept 
		{ vec = _hlslpp_blend_ps(_hlslpp_shuf_xyxx_ps(f1.vec, f2.vec), _hlslpp_perm_xxxx_ps(f3.vec), HLSLPP_BLEND_MASK(1, 1, 1, 0)); }

		quaternion(const float1& f1, const float2& f2, const float1& f3) hlslpp_noexcept
		{ vec = _hlslpp_blend_ps(_hlslpp_shuf_xxxx_ps(f1.vec, f3.vec), _hlslpp_perm_xxyx_ps(f2.vec), HLSLPP_BLEND_MASK(1, 0, 0, 1)); }

		quaternion(const float1& f1, const float1& f2, const float2& f3) hlslpp_noexcept
		{ vec = _hlslpp_blend_ps(_hlslpp_shuf_xxxy_ps(f1.vec, f3.vec), _hlslpp_perm_xxxx_ps(f2.vec), HLSLPP_BLEND_MASK(1, 0, 1, 1)); }

		quaternion(const float2& f1, const float2& f2) hlslpp_noexcept { vec = _hlslpp_shuf_xyxy_ps(f1.vec, f2.vec); }

		quaternion(const float1& f1, const float3& f2) hlslpp_noexcept { vec = _hlslpp_blend_ps(f1.vec, _hlslpp_perm_xxyz_ps(f2.vec), HLSLPP_BLEND_MASK(1, 0, 0, 0)); }
		quaternion(const float3& f1, const float1& f2) hlslpp_noexcept { vec = _hlslpp_blend_ps(f1.vec, _hlslpp_perm_xxxx_ps(f2.vec), HLSLPP_BLEND_MASK(1, 1, 1, 0)); }

		explicit quaternion(const float3& f) hlslpp_noexcept { vec = _hlslpp_and_ps(f.vec, _hlslpp_castsi128_ps(_hlslpp_set_epi32(0xffffffff, 0xffffffff, 0xffffffff, 0))); }

		explicit quaternion(const float4& f) hlslpp_noexcept { vec = f.vec; }
		//quaternion(const float1x4& q) { vec = q.vec; }

		explicit quaternion(const float3x3& m) hlslpp_noexcept;

		hlslpp_inline quaternion& operator = (const quaternion& q) { vec = q.vec; return *this; }

		static const quaternion& identity() { static const quaternion identity = quaternion(0.0f, 0.0f, 0.0f, 1.0f); return identity; };

		static quaternion rotation_x(float angle);

		static quaternion rotation_y(float angle);

		static quaternion rotation_z(float angle);

		static quaternion rotation_axis(const float3& axis, float angle);

		static quaternion rotation_axis_cosangle(const float3& axis, float cosangle, float sign);

		static quaternion rotation_euler_zxy(const float3& angles);

		HLSLPP_WARNING_ANONYMOUS_STRUCT_UNION_BEGIN
		union
		{
			n128 vec;
			float f32[4];
			// -------------------------------
// XYZW
// -------------------------------

hlslpp_swizzle_start swizzle1<0> x; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle2<0, 0> xx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<0, 0, 0> xxx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 0, 0, 0> xxxx; hlslpp_swizzle_end

// -------------------------------
// RGBA
// -------------------------------

hlslpp_swizzle_start swizzle1<0> r; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle2<0, 0> rr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<0, 0, 0> rrr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 0, 0, 0> rrrr; hlslpp_swizzle_end
			// -------------------------------
// XYZW
// -------------------------------

hlslpp_swizzle_start swizzle1<1> y; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle2<0, 1> xy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle2<1, 0> yx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle2<1, 1> yy; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<0, 0, 1> xxy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<0, 1, 0> xyx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<0, 1, 1> xyy; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<1, 0, 0> yxx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<1, 0, 1> yxy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<1, 1, 0> yyx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<1, 1, 1> yyy; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 0, 0, 1> xxxy; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 0, 1, 0> xxyx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 0, 1, 1> xxyy; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 1, 0, 0> xyxx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 1, 0, 1> xyxy; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 1, 1, 0> xyyx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 1, 1, 1> xyyy; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 0, 0, 0> yxxx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 0, 0, 1> yxxy; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 0, 1, 0> yxyx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 0, 1, 1> yxyy; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 1, 0, 0> yyxx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 1, 0, 1> yyxy; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 1, 1, 0> yyyx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 1, 1, 1> yyyy; hlslpp_swizzle_end

// -------------------------------
// RGBA
// -------------------------------

hlslpp_swizzle_start swizzle1<1> g; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle2<0, 1> rg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle2<1, 0> gr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle2<1, 1> gg; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<0, 0, 1> rrg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<0, 1, 0> rgr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<0, 1, 1> rgg; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<1, 0, 0> grr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<1, 0, 1> grg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<1, 1, 0> ggr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<1, 1, 1> ggg; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 0, 0, 1> rrrg; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 0, 1, 0> rrgr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 0, 1, 1> rrgg; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 1, 0, 0> rgrr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 1, 0, 1> rgrg; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 1, 1, 0> rggr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 1, 1, 1> rggg; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 0, 0, 0> grrr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 0, 0, 1> grrg; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 0, 1, 0> grgr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 0, 1, 1> grgg; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 1, 0, 0> ggrr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 1, 0, 1> ggrg; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 1, 1, 0> gggr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 1, 1, 1> gggg; hlslpp_swizzle_end
			// -------------------------------
// XYZW
// -------------------------------

hlslpp_swizzle_start swizzle1<2> z; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle2<0, 2> xz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle2<1, 2> yz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle2<2, 0> zx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle2<2, 1> zy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle2<2, 2> zz; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<0, 0, 2> xxz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<0, 1, 2> xyz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<0, 2, 0> xzx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<0, 2, 1> xzy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<0, 2, 2> xzz; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<1, 0, 2> yxz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<1, 1, 2> yyz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<1, 2, 0> yzx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<1, 2, 1> yzy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<1, 2, 2> yzz; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<2, 0, 0> zxx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<2, 0, 1> zxy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<2, 0, 2> zxz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<2, 1, 0> zyx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<2, 1, 1> zyy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<2, 1, 2> zyz; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<2, 2, 0> zzx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<2, 2, 1> zzy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<2, 2, 2> zzz; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 0, 0, 2> xxxz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 0, 1, 2> xxyz; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 0, 2, 0> xxzx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 0, 2, 1> xxzy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 0, 2, 2> xxzz; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 1, 0, 2> xyxz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 1, 1, 2> xyyz; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 1, 2, 0> xyzx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 1, 2, 1> xyzy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 1, 2, 2> xyzz; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 2, 0, 0> xzxx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 2, 0, 1> xzxy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 2, 0, 2> xzxz; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 2, 1, 0> xzyx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 2, 1, 1> xzyy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 2, 1, 2> xzyz; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 2, 2, 0> xzzx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 2, 2, 1> xzzy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 2, 2, 2> xzzz; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 0, 0, 2> yxxz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 0, 1, 2> yxyz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 0, 2, 0> yxzx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 0, 2, 1> yxzy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 0, 2, 2> yxzz; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 1, 0, 2> yyxz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 1, 1, 2> yyyz; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 1, 2, 0> yyzx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 1, 2, 1> yyzy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 1, 2, 2> yyzz; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 2, 0, 0> yzxx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 2, 0, 1> yzxy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 2, 0, 2> yzxz; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 2, 1, 0> yzyx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 2, 1, 1> yzyy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 2, 1, 2> yzyz; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 2, 2, 0> yzzx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 2, 2, 1> yzzy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 2, 2, 2> yzzz; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 0, 0, 0> zxxx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 0, 0, 1> zxxy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 0, 0, 2> zxxz; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 0, 1, 0> zxyx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 0, 1, 1> zxyy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 0, 1, 2> zxyz; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 0, 2, 0> zxzx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 0, 2, 1> zxzy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 0, 2, 2> zxzz; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 1, 0, 0> zyxx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 1, 0, 1> zyxy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 1, 0, 2> zyxz; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 1, 1, 0> zyyx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 1, 1, 1> zyyy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 1, 1, 2> zyyz; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 1, 2, 0> zyzx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 1, 2, 1> zyzy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 1, 2, 2> zyzz; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 2, 0, 0> zzxx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 2, 0, 1> zzxy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 2, 0, 2> zzxz; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 2, 1, 0> zzyx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 2, 1, 1> zzyy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 2, 1, 2> zzyz; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 2, 2, 0> zzzx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 2, 2, 1> zzzy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 2, 2, 2> zzzz; hlslpp_swizzle_end

// -------------------------------
// RGBA
// -------------------------------

hlslpp_swizzle_start swizzle1<2> b; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle2<0, 2> rb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle2<1, 2> gb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle2<2, 0> br; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle2<2, 1> bg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle2<2, 2> bb; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<0, 0, 2> rrb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<0, 1, 2> rgb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<0, 2, 0> rbr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<0, 2, 1> rbg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<0, 2, 2> rbb; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<1, 0, 2> grb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<1, 1, 2> ggb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<1, 2, 0> gbr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<1, 2, 1> gbg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<1, 2, 2> gbb; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<2, 0, 0> brr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<2, 0, 1> brg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<2, 0, 2> brb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<2, 1, 0> bgr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<2, 1, 1> bgg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<2, 1, 2> bgb; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<2, 2, 0> bbr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<2, 2, 1> bbg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<2, 2, 2> bbb; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 0, 0, 2> rrrb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 0, 1, 2> rrgb; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 0, 2, 0> rrbr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 0, 2, 1> rrbg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 0, 2, 2> rrbb; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 1, 0, 2> rgrb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 1, 1, 2> rggb; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 1, 2, 0> rgbr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 1, 2, 1> rgbg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 1, 2, 2> rgbb; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 2, 0, 0> rbrr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 2, 0, 1> rbrg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 2, 0, 2> rbrb; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 2, 1, 0> rbgr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 2, 1, 1> rbgg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 2, 1, 2> rbgb; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 2, 2, 0> rbbr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 2, 2, 1> rbbg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 2, 2, 2> rbbb; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 0, 0, 2> grrb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 0, 1, 2> grgb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 0, 2, 0> grbr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 0, 2, 1> grbg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 0, 2, 2> grbb; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 1, 0, 2> ggrb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 1, 1, 2> gggb; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 1, 2, 0> ggbr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 1, 2, 1> ggbg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 1, 2, 2> ggbb; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 2, 0, 0> gbrr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 2, 0, 1> gbrg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 2, 0, 2> gbrb; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 2, 1, 0> gbgr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 2, 1, 1> gbgg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 2, 1, 2> gbgb; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 2, 2, 0> gbbr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 2, 2, 1> gbbg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 2, 2, 2> gbbb; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 0, 0, 0> brrr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 0, 0, 1> brrg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 0, 0, 2> brrb; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 0, 1, 0> brgr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 0, 1, 1> brgg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 0, 1, 2> brgb; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 0, 2, 0> brbr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 0, 2, 1> brbg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 0, 2, 2> brbb; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 1, 0, 0> bgrr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 1, 0, 1> bgrg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 1, 0, 2> bgrb; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 1, 1, 0> bggr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 1, 1, 1> bggg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 1, 1, 2> bggb; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 1, 2, 0> bgbr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 1, 2, 1> bgbg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 1, 2, 2> bgbb; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 2, 0, 0> bbrr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 2, 0, 1> bbrg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 2, 0, 2> bbrb; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 2, 1, 0> bbgr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 2, 1, 1> bbgg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 2, 1, 2> bbgb; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 2, 2, 0> bbbr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 2, 2, 1> bbbg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 2, 2, 2> bbbb; hlslpp_swizzle_end
			// -------------------------------
// XYZW
// -------------------------------

hlslpp_swizzle_start swizzle1<3> w; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle2<0, 3> xw; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle2<1, 3> yw; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle2<2, 3> zw; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle2<3, 0> wx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle2<3, 1> wy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle2<3, 2> wz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle2<3, 3> ww; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<0, 0, 3> xxw; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<0, 1, 3> xyw; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<0, 2, 3> xzw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<0, 3, 0> xwx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<0, 3, 1> xwy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<0, 3, 2> xwz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<0, 3, 3> xww; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<1, 0, 3> yxw; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<1, 1, 3> yyw; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<1, 2, 3> yzw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<1, 3, 0> ywx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<1, 3, 1> ywy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<1, 3, 2> ywz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<1, 3, 3> yww; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<2, 0, 3> zxw; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<2, 1, 3> zyw; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<2, 2, 3> zzw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<2, 3, 0> zwx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<2, 3, 1> zwy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<2, 3, 2> zwz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<2, 3, 3> zww; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<3, 0, 0> wxx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<3, 0, 1> wxy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<3, 0, 2> wxz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<3, 0, 3> wxw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<3, 1, 0> wyx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<3, 1, 1> wyy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<3, 1, 2> wyz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<3, 1, 3> wyw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<3, 2, 0> wzx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<3, 2, 1> wzy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<3, 2, 2> wzz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<3, 2, 3> wzw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<3, 3, 0> wwx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<3, 3, 1> wwy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<3, 3, 2> wwz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<3, 3, 3> www; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 0, 0, 3> xxxw; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 0, 1, 3> xxyw; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 0, 2, 3> xxzw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 0, 3, 0> xxwx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 0, 3, 1> xxwy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 0, 3, 2> xxwz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 0, 3, 3> xxww; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 1, 0, 3> xyxw; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 1, 1, 3> xyyw; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 1, 2, 3> xyzw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 1, 3, 0> xywx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 1, 3, 1> xywy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 1, 3, 2> xywz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 1, 3, 3> xyww; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 2, 0, 3> xzxw; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 2, 1, 3> xzyw; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 2, 2, 3> xzzw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 2, 3, 0> xzwx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 2, 3, 1> xzwy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 2, 3, 2> xzwz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 2, 3, 3> xzww; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 3, 0, 0> xwxx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 3, 0, 1> xwxy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 3, 0, 2> xwxz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 3, 0, 3> xwxw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 3, 1, 0> xwyx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 3, 1, 1> xwyy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 3, 1, 2> xwyz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 3, 1, 3> xwyw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 3, 2, 0> xwzx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 3, 2, 1> xwzy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 3, 2, 2> xwzz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 3, 2, 3> xwzw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 3, 3, 0> xwwx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 3, 3, 1> xwwy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 3, 3, 2> xwwz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 3, 3, 3> xwww; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 0, 0, 3> yxxw; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 0, 1, 3> yxyw; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 0, 2, 3> yxzw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 0, 3, 0> yxwx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 0, 3, 1> yxwy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 0, 3, 2> yxwz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 0, 3, 3> yxww; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 1, 0, 3> yyxw; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 1, 1, 3> yyyw; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 1, 2, 3> yyzw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 1, 3, 0> yywx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 1, 3, 1> yywy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 1, 3, 2> yywz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 1, 3, 3> yyww; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 2, 0, 3> yzxw; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 2, 1, 3> yzyw; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 2, 2, 3> yzzw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 2, 3, 0> yzwx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 2, 3, 1> yzwy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 2, 3, 2> yzwz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 2, 3, 3> yzww; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 3, 0, 0> ywxx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 3, 0, 1> ywxy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 3, 0, 2> ywxz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 3, 0, 3> ywxw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 3, 1, 0> ywyx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 3, 1, 1> ywyy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 3, 1, 2> ywyz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 3, 1, 3> ywyw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 3, 2, 0> ywzx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 3, 2, 1> ywzy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 3, 2, 2> ywzz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 3, 2, 3> ywzw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 3, 3, 0> ywwx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 3, 3, 1> ywwy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 3, 3, 2> ywwz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 3, 3, 3> ywww; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 0, 0, 3> zxxw; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 0, 1, 3> zxyw; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 0, 2, 3> zxzw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 0, 3, 0> zxwx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 0, 3, 1> zxwy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 0, 3, 2> zxwz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 0, 3, 3> zxww; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 1, 0, 3> zyxw; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 1, 1, 3> zyyw; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 1, 2, 3> zyzw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 1, 3, 0> zywx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 1, 3, 1> zywy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 1, 3, 2> zywz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 1, 3, 3> zyww; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 2, 0, 3> zzxw; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 2, 1, 3> zzyw; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 2, 2, 3> zzzw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 2, 3, 0> zzwx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 2, 3, 1> zzwy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 2, 3, 2> zzwz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 2, 3, 3> zzww; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 3, 0, 0> zwxx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 3, 0, 1> zwxy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 3, 0, 2> zwxz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 3, 0, 3> zwxw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 3, 1, 0> zwyx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 3, 1, 1> zwyy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 3, 1, 2> zwyz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 3, 1, 3> zwyw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 3, 2, 0> zwzx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 3, 2, 1> zwzy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 3, 2, 2> zwzz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 3, 2, 3> zwzw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 3, 3, 0> zwwx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 3, 3, 1> zwwy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 3, 3, 2> zwwz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 3, 3, 3> zwww; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 0, 0, 0> wxxx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 0, 0, 1> wxxy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 0, 0, 2> wxxz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 0, 0, 3> wxxw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 0, 1, 0> wxyx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 0, 1, 1> wxyy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 0, 1, 2> wxyz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 0, 1, 3> wxyw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 0, 2, 0> wxzx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 0, 2, 1> wxzy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 0, 2, 2> wxzz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 0, 2, 3> wxzw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 0, 3, 0> wxwx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 0, 3, 1> wxwy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 0, 3, 2> wxwz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 0, 3, 3> wxww; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 1, 0, 0> wyxx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 1, 0, 1> wyxy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 1, 0, 2> wyxz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 1, 0, 3> wyxw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 1, 1, 0> wyyx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 1, 1, 1> wyyy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 1, 1, 2> wyyz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 1, 1, 3> wyyw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 1, 2, 0> wyzx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 1, 2, 1> wyzy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 1, 2, 2> wyzz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 1, 2, 3> wyzw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 1, 3, 0> wywx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 1, 3, 1> wywy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 1, 3, 2> wywz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 1, 3, 3> wyww; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 2, 0, 0> wzxx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 2, 0, 1> wzxy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 2, 0, 2> wzxz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 2, 0, 3> wzxw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 2, 1, 0> wzyx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 2, 1, 1> wzyy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 2, 1, 2> wzyz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 2, 1, 3> wzyw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 2, 2, 0> wzzx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 2, 2, 1> wzzy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 2, 2, 2> wzzz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 2, 2, 3> wzzw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 2, 3, 0> wzwx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 2, 3, 1> wzwy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 2, 3, 2> wzwz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 2, 3, 3> wzww; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 3, 0, 0> wwxx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 3, 0, 1> wwxy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 3, 0, 2> wwxz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 3, 0, 3> wwxw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 3, 1, 0> wwyx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 3, 1, 1> wwyy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 3, 1, 2> wwyz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 3, 1, 3> wwyw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 3, 2, 0> wwzx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 3, 2, 1> wwzy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 3, 2, 2> wwzz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 3, 2, 3> wwzw; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 3, 3, 0> wwwx; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 3, 3, 1> wwwy; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 3, 3, 2> wwwz; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 3, 3, 3> wwww; hlslpp_swizzle_end

// -------------------------------
// RGBA
// -------------------------------

hlslpp_swizzle_start swizzle1<3> a; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle2<0, 3> ra; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle2<1, 3> ga; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle2<2, 3> ba; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle2<3, 0> ar; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle2<3, 1> ag; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle2<3, 2> ab; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle2<3, 3> aa; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<0, 0, 3> rra; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<0, 1, 3> rga; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<0, 2, 3> rba; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<0, 3, 0> rar; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<0, 3, 1> rag; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<0, 3, 2> rab; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<0, 3, 3> raa; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<1, 0, 3> gra; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<1, 1, 3> gga; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<1, 2, 3> gba; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<1, 3, 0> gar; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<1, 3, 1> gag; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<1, 3, 2> gab; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<1, 3, 3> gaa; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<2, 0, 3> bra; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<2, 1, 3> bga; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<2, 2, 3> bba; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<2, 3, 0> bar; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<2, 3, 1> bag; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<2, 3, 2> bab; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<2, 3, 3> baa; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<3, 0, 0> arr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<3, 0, 1> arg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<3, 0, 2> arb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<3, 0, 3> ara; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<3, 1, 0> agr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<3, 1, 1> agg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<3, 1, 2> agb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<3, 1, 3> aga; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<3, 2, 0> abr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<3, 2, 1> abg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<3, 2, 2> abb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<3, 2, 3> aba; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle3<3, 3, 0> aar; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<3, 3, 1> aag; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<3, 3, 2> aab; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle3<3, 3, 3> aaa; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 0, 0, 3> rrra; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 0, 1, 3> rrga; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 0, 2, 3> rrba; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 0, 3, 0> rrar; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 0, 3, 1> rrag; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 0, 3, 2> rrab; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 0, 3, 3> rraa; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 1, 0, 3> rgra; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 1, 1, 3> rgga; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 1, 2, 3> rgba; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 1, 3, 0> rgar; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 1, 3, 1> rgag; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 1, 3, 2> rgab; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 1, 3, 3> rgaa; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 2, 0, 3> rbra; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 2, 1, 3> rbga; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 2, 2, 3> rbba; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 2, 3, 0> rbar; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 2, 3, 1> rbag; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 2, 3, 2> rbab; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 2, 3, 3> rbaa; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 3, 0, 0> rarr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 3, 0, 1> rarg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 3, 0, 2> rarb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 3, 0, 3> rara; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 3, 1, 0> ragr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 3, 1, 1> ragg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 3, 1, 2> ragb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 3, 1, 3> raga; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 3, 2, 0> rabr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 3, 2, 1> rabg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 3, 2, 2> rabb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 3, 2, 3> raba; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<0, 3, 3, 0> raar; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 3, 3, 1> raag; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 3, 3, 2> raab; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<0, 3, 3, 3> raaa; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 0, 0, 3> grra; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 0, 1, 3> grga; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 0, 2, 3> grba; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 0, 3, 0> grar; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 0, 3, 1> grag; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 0, 3, 2> grab; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 0, 3, 3> graa; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 1, 0, 3> ggra; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 1, 1, 3> ggga; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 1, 2, 3> ggba; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 1, 3, 0> ggar; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 1, 3, 1> ggag; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 1, 3, 2> ggab; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 1, 3, 3> ggaa; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 2, 0, 3> gbra; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 2, 1, 3> gbga; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 2, 2, 3> gbba; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 2, 3, 0> gbar; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 2, 3, 1> gbag; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 2, 3, 2> gbab; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 2, 3, 3> gbaa; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 3, 0, 0> garr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 3, 0, 1> garg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 3, 0, 2> garb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 3, 0, 3> gara; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 3, 1, 0> gagr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 3, 1, 1> gagg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 3, 1, 2> gagb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 3, 1, 3> gaga; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 3, 2, 0> gabr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 3, 2, 1> gabg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 3, 2, 2> gabb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 3, 2, 3> gaba; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<1, 3, 3, 0> gaar; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 3, 3, 1> gaag; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 3, 3, 2> gaab; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<1, 3, 3, 3> gaaa; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 0, 0, 3> brra; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 0, 1, 3> brga; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 0, 2, 3> brba; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 0, 3, 0> brar; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 0, 3, 1> brag; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 0, 3, 2> brab; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 0, 3, 3> braa; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 1, 0, 3> bgra; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 1, 1, 3> bgga; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 1, 2, 3> bgba; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 1, 3, 0> bgar; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 1, 3, 1> bgag; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 1, 3, 2> bgab; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 1, 3, 3> bgaa; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 2, 0, 3> bbra; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 2, 1, 3> bbga; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 2, 2, 3> bbba; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 2, 3, 0> bbar; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 2, 3, 1> bbag; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 2, 3, 2> bbab; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 2, 3, 3> bbaa; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 3, 0, 0> barr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 3, 0, 1> barg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 3, 0, 2> barb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 3, 0, 3> bara; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 3, 1, 0> bagr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 3, 1, 1> bagg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 3, 1, 2> bagb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 3, 1, 3> baga; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 3, 2, 0> babr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 3, 2, 1> babg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 3, 2, 2> babb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 3, 2, 3> baba; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<2, 3, 3, 0> baar; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 3, 3, 1> baag; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 3, 3, 2> baab; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<2, 3, 3, 3> baaa; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 0, 0, 0> arrr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 0, 0, 1> arrg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 0, 0, 2> arrb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 0, 0, 3> arra; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 0, 1, 0> argr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 0, 1, 1> argg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 0, 1, 2> argb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 0, 1, 3> arga; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 0, 2, 0> arbr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 0, 2, 1> arbg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 0, 2, 2> arbb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 0, 2, 3> arba; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 0, 3, 0> arar; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 0, 3, 1> arag; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 0, 3, 2> arab; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 0, 3, 3> araa; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 1, 0, 0> agrr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 1, 0, 1> agrg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 1, 0, 2> agrb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 1, 0, 3> agra; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 1, 1, 0> aggr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 1, 1, 1> aggg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 1, 1, 2> aggb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 1, 1, 3> agga; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 1, 2, 0> agbr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 1, 2, 1> agbg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 1, 2, 2> agbb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 1, 2, 3> agba; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 1, 3, 0> agar; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 1, 3, 1> agag; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 1, 3, 2> agab; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 1, 3, 3> agaa; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 2, 0, 0> abrr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 2, 0, 1> abrg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 2, 0, 2> abrb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 2, 0, 3> abra; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 2, 1, 0> abgr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 2, 1, 1> abgg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 2, 1, 2> abgb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 2, 1, 3> abga; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 2, 2, 0> abbr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 2, 2, 1> abbg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 2, 2, 2> abbb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 2, 2, 3> abba; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 2, 3, 0> abar; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 2, 3, 1> abag; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 2, 3, 2> abab; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 2, 3, 3> abaa; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 3, 0, 0> aarr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 3, 0, 1> aarg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 3, 0, 2> aarb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 3, 0, 3> aara; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 3, 1, 0> aagr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 3, 1, 1> aagg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 3, 1, 2> aagb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 3, 1, 3> aaga; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 3, 2, 0> aabr; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 3, 2, 1> aabg; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 3, 2, 2> aabb; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 3, 2, 3> aaba; hlslpp_swizzle_end

hlslpp_swizzle_start swizzle4<3, 3, 3, 0> aaar; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 3, 3, 1> aaag; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 3, 3, 2> aaab; hlslpp_swizzle_end
hlslpp_swizzle_start swizzle4<3, 3, 3, 3> aaaa; hlslpp_swizzle_end
		};
		HLSLPP_WARNING_ANONYMOUS_STRUCT_UNION_END
	};
};