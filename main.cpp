//#define POINT_SWIZZLE_USE_MIXED_INDICATORS
#define SEL_NO_SAFE_CONVERSION_CHECK

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
    sel::point3f p3f = p4f.swizzle(x, y, z);
    //And it's also convenient to expand a point to a higher dimension with a constructor.
    sel::point4d p4d{p3f, 1.0};

    // Because swizzle on a lvalue point creates an exchanger, which is actually a reference wrapper,
    // it's dangerous to use it after the point is destroyed.
    auto dangling_exchanger_maker = []() {
        sel::point3d p3d;
        return p3d.swizzle(x, y, z);
    };
    //This dangling_exchanger stores a reference to an already destroyed point.
    auto dangling_exchanger = dangling_exchanger_maker();
    //Undefined behaviour.
    dangling_exchanger = {1.0, 2.0, 3.0};
    

    sel::point<int, 5> p5i({1, 2, 3, 4});
    sel::point<float, 6> p6f(p5i, 4);

    sel::point3f p33f = p5i.swizzle(x, y, z);
    auto sw = p5i.swizzle(3, 4);

    auto *psw = new auto(p5i.swizzle(0, 1));
    psw->operator=(sel::point2f());

    p5i.swizzle(0, 1) = sw;
    p5i.swizzle(0, 1) = p5i.swizzle(3, 4);
    p5i = p5i.swizzle(1, 2, 2, 3, 4);
    p5i.swizzle(1, 2, 3);
    p5i.swizzle(0, 1);
    p5i = p5i.swizzle(4, 3, 2, 1, 0);
    const sel::vec3f tvc3f(3.0, 2.0, 1.0);
    auto sw1 = tvc3f.swizzle(0, 1);
    sw = sw1;
    sw = tvc3f.swizzle(1, 2);
    sw = {0, 1};

    sel::point4f color({1.0, 2.0, 3.0, 1.0});
    color = sel::point4f(1.0);
    sel::vec3d ppp(5.0);
    sel::vec3f vc3(0.0, 0.0, 1.0), vc31(0.0, 1.0, 0.0);
    vc3 = vc31 ^ ppp;
    vc3.normalize();

    sel::vec4i v4i(ppp, 0);

    print(vc3);
    sel::point2i p(5, 5);
    [[maybe_unused]] double l = p.length();
    print(p);
    sel::point3i hp(p, 3);
    sel::point2f pf(2.5, 2.5);
    print(pf);
    sel::point2f op(p);
    [[maybe_unused]] sel::point2f fp(op);
    pf = p;
    print(p = pf);
    pf = pf + pf;
    print(pf);
    pf = pf - p;
    print(pf);
    pf = pf * 5.5;
    print(pf);
    pf = pf / 4.0;
    print(pf);
    [[maybe_unused]] float val = pf[0];
    val = pf[1];
    val = pf.at(0);
    pf = -pf;
    print(pf);
    pf.normalize();
    print(pf);
    pf.set(7.8, 4.5);
    print(pf);
    pf.set_all(9.9);
    print(pf);
    pf += pf;
    pf -= p;
    pf *= 2.1;
    pf /= 2.3;
    sel::point2f pf1(2.3, 2.3);
    pf.set_all(1.0);
    [[maybe_unused]] bool tt = pf.exceeds(pf1);
    [[maybe_unused]] bool comp = pf < pf1;
    comp = pf == pf1;
    comp = pf != pf1;
    comp = pf == p;
    comp = pf == pf;

    pf = pf.normalized();

    return 0;
}