## Lighting and Shading (external lecture)
### <a href='https://mediasite.bris.ac.uk/Mediasite/Play/9fdeab3be07f480c93df3ee4e2f37f1f1d' target='_blank'> Weekly Briefing ![](../../resources/icons/briefing.png) </a>
### Task 1: Introduction 03.36
 <a href='01%20Introduction%2003.36/animation/segment-1.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a>

In this workbook we explore some of the more advanced aspects of model rendering. In particular, we will focus on the use of various lighting and shading effects to make our renders look more realistic and aesthetically appealing.

The animation linked to above shows that we can distinguish between three distinct types of light as a convenient _approximation_ to real-world lighting:

- Diffuse lighting: "general" surface lighting (which we later decomposed into Proximity and Angle of Incidence)
- Specular highlighting: to give objects the appearance of a glossy/shiny/reflective surface
- Ambient lighting: as an approximation to "global illumination"

This week, prompted by feedback from the mid-term review, the weekly briefing session contains a deep-dive exploration of all of the key topics in this workbook. As a result of this, there will be no small fragments of lecture material embedded within each task - instead you should view the recording of the entire lecture linked to at the very top of this workbook. If you did not attend the lecture, you should view this video now before moving on to attempt the practical tasks in this workbook.  


# 
### Task 2: Proximity Lighting 09.30
 <a href='02%20Proximity%20Lighting%2009.30/animation/segment-1.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a>

Proximity lighting takes into account the distance of a surface from a light source. Intuitively, the closer a point on a surface is to a light, the brighter it will appear. View the animation above to gain an appreciation of the visual effect of this form of lighting. Make sure you have an understanding of how to calculate this effect from the lecture recording before attempting the practical activity described below.

Add code to your renderer that will adjust the brightness of each pixel to take into account proximity of the surface to the light source. Your calculated brightness values should be within the range 0.0 (total darkness) to 1.0 (fully illuminated). These values can then be used as a multiplier to adjust the brightness of a particular pixel. Simply multiply each of the three colour channels (RGB) by the brightness float, before you pack them into the final 32 bit pixel colour.

Your algorithm should be _based_ on the 1/4&pi;r<sup>2</sup> approach outlined in the lecture - however, feel free to experiment with different multipliers (e.g. 1/3&pi;r<sup>2</sup>, 1/5&pi;r<sup>2</sup>, 2/7&pi;r<sup>2</sup> etc.) in order to produce an effect that you think looks good (this is were aesthetics rule over science !)  


**Hints & Tips:**  
To find the distance between the light and a point on a triangle, you might like to make use of the GLM `length` function - this will give your the magnitude of a `vec3` variable. Don't normalise your `vec3` otherwise you'll always get a length of 1 (remember in this situation, we _need_ the distance !).

To help you test this form of lighting and debug your code, you may wish to add some additional key event handlers that allow you to shift the position of your light source around the model.  


# 
### Task 3: Angle of Incidence Lighting 11.52
 <a href='03%20Angle%20of%20Incidence%20Lighting%2011.52/animation/segment-2.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a>

Your next task is to implement "Angle-of-Incidence" lighting. This lights a surface by considering the _orientation_ of that surface relative to the position of a light source. View the animation linked to above to gain an appreciation of the visual effect of this form of lighting.

In order to implement this kind of lighting, we must first calculate a "normal" for each triangle (a vector that is perpendicular to the surface of the triangle). Review the lecture recording for a reminder of how to calculate the normal for a particular surface. For each triangle in the Cornell Box model, calculate the surface normal, using the GLM `cross` function to calculate the cross product of two edges of each triangle.

Note that you can safely calculate and store the normals of all triangles when you first load in the OBJ geometry file (since these normals will not change during the operation of the renderer). It is worth noting that the `ModelTriangle` class has a `normal` attribute that can be used to store the calculated normal for each triangle. 

Once the surface normals have been calculated, you can then use these to calculate the angle of incidence of light falling onto the surfaces. This is done by taking the dot product of the normal and light direction vectors (as shown in the lecture). To calculate dot products, you might like to use the `dot` function provided by GLM.

Once calculated, use these angles of incidence to adjust the brightness of each pixel (in a similar way to that which you did with proximity in the previous task). You should end up with a render that looks like the image shown below. Note that we have removed the shadow feature from this render in order to more clearly illustrate the appearance of diffuse lighting. If you have implemented event handlers to move the position of your light point, use this feature to test that your lighting looks realistic from a number of different light positions.


  


![](03%20Angle%20of%20Incidence%20Lighting%2011.52/images/diffuse-lighting.png)

**Hints & Tips:**  
We need to be a bit careful - each triangles has TWO normals (the front face and the back face !). Calculating the correct normal requires us to pick the correct to triangle edges to cross product. However _which_ two edges we use will depend on the "winding order" used by the OBJ model. View <a href="resources/winding.mp4" target="_blank">this video</a> for an explanation of the concept of "winding order". You won't always know which winding was used when creating a particular model - the best advice is to just to pick two edges and try it. You will soon know if you were wrong (all your surfaces will be dark) so you can then just switch to using alternative edges. This might seem like trial and error, but this is often the way with exploratory rendering !

Note that when considering the angle that the light hits the surface...  
you **MUST** use: _the direction vector of the LIGHT from the POINT ON THE SURFACE_  
and **NOT**: _the direction vector of the POINT ON THE SURFACE from the LIGHT_  
If you use the wrong one, you will end up lighting the back surface of your triangles !
  


# 
### Task 4: Specular Lighting 24.44


Your next task is to implement a "Specular" lighting effect. Review the video of the lecture to make sure you have a full grasp of the theory of this lighting technique. As with the previous tasks, you should again make use of the GLM `dot` function, as well as the surfaces normals that you calculated previously. 

You should use a relatively large exponent in the calculation of specular spread (for example 256). This will ensure that you get a tightly focused spot of light, which will be easier to see than a specular highlight with a wider spread. It can be difficult to distinguish between the various different lighting effects if the parameters used are too subtle (for example, proximity lighting can look a lot like a specular spot with a wide spread !). You might even like to switch off the other forms of lighting in your code so that you can clearly see the specular effect.

You may need to move the light and/or camera to an appropriate location in the scene in order to be able to see the specular highlight effect. Remember that specular lighting represents the reflection of the light source, so the camera needs to be in a position where the reflection should be visible.

The image below provides some examples of the kinds of specular highlighting effect you might expect to see for different exponent values:  


![](04%20Specular%20Lighting%2024.44/images/various-exponents.jpg)

**Hints & Tips:**  
  


# 
### Task 5: Ambient Lighting 22.03


In the real world, light bounces off objects (potentially multiple times) in order to illuminate "hard to reach" areas of a model (for example, areas that are in shadow). As you might imagine, calculating this kind of lighting (which would involve multiple bounces) would be computationally very expensive. It is possible however to implement a plausible approximation to such lighting with very little additional effort (both in terms of computational power and lines of code).

One approach to creating such "ambient" lighting is to implement a "minimum threshold" as illustrated in the image below. Compare the shadow regions of this image with those of the previous week's workbook (which included no ambient lighting). This involves the use of an IF statement (or call to the `max` function) to ensure that the brightness of any point never falls below a certain threshold level.  


![](05%20Ambient%20Lighting%2022.03/images/shadows-with-ambient.png)

# 
### Task 6: Gouraud Shading 35.58
 <a href='06%20Gouraud%20Shading%2035.58/animation/segment-1.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a>

We finish this workbook by considering some rendering enhancements that will improve the aesthetics of your renders, with only a minimal additional performance overhead.

Due to the fact that we represent models as a series of triangles, any artefacts within a model will have a finite resolution. When rendering a model (unless we have a huge number of triangles) the surfaces may well appear "blocky" and "low-res". But what if we could shade triangles more "intelligently" and blend their edges together ? That would "smooth off" the sharp corners and provide an _apparently_ higher resolution final rendering !

_Gouraud shading_ in one such intelligent approach to surface rendering. Review the lecture recording to ensure you have a good understanding of the principles of Gouraud shading and then use this knowledge to implement this approach in your renderer. In order to achieve this, you will need to make use of _Barycentric coordinates_ to interpolate vertex brightnesses across the face of each triangle. The operation of Barycentric coordinates is illustrated in the animation at the top of this section.

You should interpolate both Angle of Incidence and Specular lighting effects when implementing Gouraud shading. Once this is working correctly, you can then also add in the effect of proximity and ambient lighting.

The image below illustrates the smoothing effect that can be achieved using Gouraud shading - it is important to note that both spheres shown in the diagram have the _same_ geometry (the same number of triangles). The surfaces in the sphere on the left are shaded with a flat colour, whereas the sphere on the right is rendered using Gouraud shading. Clearly the right hand sphere _looks_ a lot smoother !  


![](06%20Gouraud%20Shading%2035.58/images/gouraud.jpg)

**Hints & Tips:**  
One of the trickiest parts of this task is applying the _correct_ weighting to each vertex brightness. Watch the Barycentric coordinates animation closely in order to work out which vertices you need to apply `u`, `v` and `w` in order to calculate the correct brightness weightings.

The Cornell Box isn't the most useful model for testing Gouraud shading (it's hard to "smooth off" the edges of a cube !). For this reason, we have provided a simple <a href="resources/sphere.obj" target="_blank">additional OBJ model</a> for you to use when testing your code.

  


# 
### Task 7: Phong Shading 47.34


Gouraud shading in fine, but it only goes so far in creating smoothly rendered model surfaces. _Phong shading_ is an even more sophisticated approach that takes shading one step further (although it can only achieve this at the cost of additional computation !). Review the lecture recording in order to ensure you have a good understanding of Phong Shading, then adapt your renderer to make use of this more sophisticated approach. 

You should calculate both Angle of Incidence and Specular lighting effects when implementing Phong shading. Once this is working correctly, you can then also add in the effect of proximity and ambient lighting.

The diagram below illustrates the different results of flat, Gouraud and Phong shading. Again, all three spheres have exactly the same geometry and number of triangles. It is just the alternative rendering approaches that make them appear different.  


![](07%20Phong%20Shading%2047.34/images/phong.jpg)

# 
### End of workbook

Make sure you push all your work to your GitHub repository !<br>
Use of GitHub is a key development skill.<br>
It will allow us to keep track of your progress (and we will use it in the final marking process).