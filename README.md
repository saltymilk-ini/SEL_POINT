# SpaceElementLibrary-point

When I work with OpenGL, the swizzle in GLSL is very handy, but in C++, there is no such semantic.  
Some famous libraries do provide swizzle, but none of them fulfills all of my expectations.  
* GLM: with macro GLM_FORCE_SWIZZLE, glm provides swizzle functionality, but the combinations are predefined, in other words, limited. And because the swizzle name indicators are members, they pollute the intellisence, which is a little bit annoying.
![GLM swizzle](./images/GLM_swizzle.gif)
* Eigen: 