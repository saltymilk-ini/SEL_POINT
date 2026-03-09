#include "./sel/info/output.hpp"
#include "./sel/elements/point.hpp"
#include <iostream>

auto print = [](const auto &p) { std::cout << p; };

int main()
{
    sel::point4i p4i{0, 1, 2, 3};
    //swz stores a exchanger references to p4i
    auto swz = p4i.swizzle(x, y, z); print(p4i);
    // now p4i becomes {4, 5, 6, 3}
    swz = {4, 5, 6}; print(p4i);
    // now p4i becomes {3, 2, 1, 3}
    swz = sel::point3f{3.0, 2.0, 1.0}; print(p4i);

    sel::point4f p4f{10.0, 20.0, 30.0, 40.0};
    // now p4i becomes {40, 30, 20, 3}
    swz = p4f.swizzle(3, 2, 1); print(p4i);
    // use swizzle to reorder a point, p4f becomes {40.0, 30.0, 20.0, 10.0}
    p4f = p4f.swizzle(3, 2, 1, 0); print(p4f);
    // or copy backward/forward, p4f becomes {30.0, 20.0, 10.0, 10.0}
    p4f.swizzle(0, 1, 2) = p4f.swizzle(1, 2, 3); print(p4f);
    // or create an arbitrary point, p7i is {30, 20, 10, 10, 10, 20, 30}
    sel::point<int, 7> p7i = p4f.swizzle(0, 1, 2, 3, 2, 1, 0); print(p7i);

    // When swizzle on a rvalue point, the result is always a new point, no reference.
    // So const_rvalue_swizzle and rvalue_swizzle are both point2f, not exchanger.
    const sel::point3f cp3f(1.0, 2.0, 3.0);
    auto const_rvalue_swizzle = std::move(cp3f).swizzle(0, 2);
    auto rvalue_swizzle = sel::point3f(1.0f, 2.0f, 3.0f).swizzle(0, 2);

    // It's convenient to project a point to a lower dimension.
    sel::vec4f rgba{0.4f, 0.6f, 0.5f, 1.0f};
    sel::vec3f rgb = rgba.swizzle(r, g, b);
    // And it's also convenient to expand a point to a higher dimension with a constructor.
    sel::vec4f bgra{rgb.swizzle(b, g, r), 0.5f};

    // Because swizzle on a lvalue point creates an exchanger, which is actually a reference wrapper,
    // it's dangerous to use it after the point is destroyed.
    auto dangling_exchanger_maker = []() {
        sel::point3d p3d;
        return p3d.swizzle(x, y, z);
    };
    // This dangling_exchanger stores a reference to an already destroyed point.
    auto dangling_exchanger = dangling_exchanger_maker();
    // Undefined behaviour.
    /*dangling_exchanger = {1.0, 2.0, 3.0};*/

    // The macro SEL_NO_SAFE_CONVERSION_CHECK controls whether to prevent unsafe numeric conversions from being implicitly performed.
    // If it is not defined, an extremely strict check will be applied to every numeric conversion.
    sel::point2b p2b;
    sel::point2i p2i;
    sel::point2f p2f;
    sel::point2d p2d;
    // If SEL_NO_SAFE_CONVERSION_CHECK is not defined:
    p2b = p2i;   // forbidden, narrowing
    p2f = p2d;   // forbidden, narrowing
    p2i = p2f;   // forbidden, truncating
    p2i = p2d;   // forbidden, truncating
    p2f = p2i;   // forbidden, precision loss
    // point class has a member function 'cast', it performs the explicit conversion:
    p2b = p2i.cast<sel::uchar>();
    p2f = p2d.cast<float>();
    p2i = p2f.cast<int>();
    p2i = p2d.cast<int>();
    p2f = p2i.cast<float>();

    // Other functionalities can be found in the header files.

    return 0;
}