/*
MIT License
Copyright (c) 2026 saltymilk-ini
The full license is in LICENSE file included with this distribution at https://github.com/saltymilk-ini/SEL_POINT
*/

#include "./implement/point_def.hpp"

SEL_NAMESPACE_BEGIN

//2 dimension
template<typename T> using point2 = point<T, 2>;
typedef point2<uchar>		point2b;
typedef point2<short>		point2s;
typedef point2<int>			point2i;
typedef point2<float>		point2f;
typedef point2<double>		point2d;

template<typename T> using vec2 = point<T, 2>;
typedef vec2<uchar>			vec2b;
typedef vec2<short>			vec2s;
typedef vec2<int>			vec2i;
typedef vec2<float>			vec2f;
typedef vec2<double>		vec2d;

//3 dimension
template<typename T> using point3 = point<T, 3>;
typedef point3<uchar>		point3b;
typedef point3<short>		point3s;
typedef point3<int>			point3i;
typedef point3<float>		point3f;
typedef point3<double>		point3d;

template<typename T> using vec3 = point<T, 3>;
typedef vec3<uchar>			vec3b;
typedef vec3<short>			vec3s;
typedef vec3<int>			vec3i;
typedef vec3<float>			vec3f;
typedef vec3<double>		vec3d;

//4 dimension
template<typename T> using point4 = point<T, 4>;
typedef point4<uchar>		point4b;
typedef point4<short>		point4s;
typedef point4<int>			point4i;
typedef point4<float>		point4f;
typedef point4<double>		point4d;

template<typename T> using vec4 = point<T, 4>;
typedef vec4<uchar>			vec4b;
typedef vec4<short>			vec4s;
typedef vec4<int>			vec4i;
typedef vec4<float>			vec4f;
typedef vec4<double>		vec4d;

SEL_NAMESPACE_END
