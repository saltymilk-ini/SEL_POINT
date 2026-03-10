# SEL_POINT

When I work with OpenGL, I find the swizzle in GLSL very handy and flexible, but in C++, there is no such semantic.  
Some famous libraries do provide swizzle, but none of them fulfills all of my expectations.  
## GLM
 With macro GLM_FORCE_SWIZZLE, GLM provides swizzle functionality, but the combinations are predefined, in other words, limited. And because the swizzle combinations are class members, they pollute the completion list, which is a little bit annoying.
![GLM swizzle](./images/GLM_swizzle.gif)

## Eigen
 Eigen provides the **IndexedView** class, which is a very powerful and flexible swizzle-like implementation, but it is not intuitive to use.
```cpp
Eigen::Vector3f v{1.0f, 2.0f, 3.0f};
// swz is an IndexedView, it holds the reference to v, and the index list {1, 0}, which means swz is a view of v.yx.
auto swz = v({1, 0});
// v becomes {5.0f, 4.0f, 3.0f}
swz = Eigen::Vector2f{4.0f, 5.0f}; 
// reorder a vector, v becomes {3.0f, 5.0f, 4.0f}
v({0, 1, 2}) = v({2, 0, 1});
// copy forward/backward, v becomes {5.0f, 4.0f, 4.0f}
v({0, 1}) = v({1, 2});
```

## My Implementation
My swizzle implementation is also based on a proxy class **exchanger** like Eigen's IndexedView(but is not improved from Eigen, at the time it was created, I didn't know Eigen's IndexedView). Unlike Eigen, the index list is not stored as member data but is compiled into the code, the exchanger class only holds a pointer to the run-time data.  
It provides name identifiers for common indexes, and the combinations are not predefined, you can create arbitrary swizzles.
It can be used as follows:
```cpp
sel::point4i p4i{0, 1, 2, 3};
//swz stores a exchanger references to p4i
auto swz = p4i.swizzle(x, y, z);
// now p4i becomes {4, 5, 6, 3}
swz = {4, 5, 6};
// now p4i becomes {3, 2, 1, 3}
swz = sel::point3f{3.0, 2.0, 1.0};

sel::point4f p4f{10.0, 20.0, 30.0, 40.0};
// now p4i becomes {40, 30, 20, 3}
swz = p4f.swizzle(3, 2, 1);
// use swizzle to reorder a point, p4f becomes {40.0, 30.0, 20.0, 10.0}
p4f = p4f.swizzle(a, b, g, r);
// or copy backward/forward, p4f becomes {30.0, 20.0, 10.0, 10.0}
p4f.swizzle(s, t, p) = p4f.swizzle(t, p, q);
// or create an arbitrary point, p7i is {30, 20, 10, 10, 10, 20, 30}
sel::point<int, 7> p7i = p4f.swizzle(0, 1, 2, 3, 2, 1, 0);
// if the macro POINT_SWIZZLE_USE_MIXED_IDENTIFIERS is defined, you can use different types
// of identifiers in one swizzle expression, though it seems not very useful.
sel::point<double, 4> p4d = p4f.swizzle(x, g, p, 3); 
```

## Drawback
* In fact, swizzle is a member function of the point class, it needs to be called as a function, which does not fully replicate the syntax of GLSL.  
* swizzle is also a macro, it modifies every parameter of the swizzle function call, so it is invasive, and it may cause some unexpected errors if you define another function or macro with the same name.

Other usage can be found in the example code in test.cpp.