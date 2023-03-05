# 3D-Graphics-DirectX-Fundamentals

![image1](https://user-images.githubusercontent.com/74312830/222975507-2e69eb76-6177-4f02-9bf4-7f07efc81d44.jpg)

# Introduction
The objective of this project demonstration was to showcase the use of DirectX 11 and the DirectX Toolkit to create and 3D application. The project made use of a variety of inbuilt libraries such as the SimpleMath library to assist in adding features within the project. The full working project was built within Visual Studio 2019 and made use of the directxtk framework (2015) which included support for Audio handling as well. The inspiration for the project came from dioramic room representation combined with house viewings and pictographic representations of modern housing.
<br>
The stylised room includes Ambient Audio, a sofa set, beanbag, table and a decorative bookcase with some books , a rotating fan, a Roomba vacuum in action, a carpet, a chair, a potted plant and some abstract art pieces on the wall. As for the environment the lighting, while basic (Blinn-Phong) , has been made to rotate around the scene to mimic that of the sun thus showcasing a passing day. The days may appear to be going quite fast but it is for demonstration purposes. The room can be viewed through the perspective of the camera which has been programmed to move along the coordinate axes.

# User Input Controls
The following are the controls to move around the scene and interact with the scene objects:

## Keyboard Inputs
1. W Key: Move forward
2. S Key: Move backward
3. A Key: Yaw left (rotate about the y axis)
4. D Key: Yaw right
5. X Key: Move vertically upwards
6. Z Key: Move vertically downwards
7. T Key: Toggle the roomba movement on key press. [once to stop, and press again to
restart movement] + This allows the texture change on the Roomba simultaneously.
8. Esc: Close Project
9. Alt + Enter: Move between Fullscreen mode and restore down mode.

## Mouse Inputs
Alternatively the mouse can be used for Yaw about the Y axis either to the left or the right, however it has been commented out and supplemented with keyboard movements instead which felt more robust.

# Implementation
This scene has been modified and adapted from the tutorials covered during the module as well as from the DirectXTK resource page maintained by Microsoft (Walbourn, 2021).

## Loading Models
All models loaded inside the project are in Wavefront (obj) file format and have been sourced from ones that pre-existed with the developer and were either wholly made or received through tutorials and online sites. Those that have been sourced from the internet are as follows :
- Sofa set (Cla, 2022)
- Carpet (Garnier, 2022)
- House Plant (Sage, 2022)

To load the models the method we use is  Game:: CreateDeviceDependentResources(), after which we can process the models in a manner that is desired whether it is to rotate or translate the object or even to scale up and down this is all done within the Game::Render() method . Before rendering any model it is imperative that the world matrix is set  to identity. This allows us to render the model without being affected by any other model loaded in prior to it.
<br>
Any translation or rotation should be done before loading in textures which is done inside the shader parameters method. The project scene makes use of a variety of texture loading styles : Regular and tiled and tiled + blended.
<br>
The definitions required for altering the textures such as the values of light diffusion are set within the light vertex shader (version 2) and the tiled effect is determined in the light pixel shader (version 2). These are then passed inside the shader class and applied to the object.
<br>
The reason for applying the three different styles was to observe the effect of texturing with a single texture and noticing how the image file (Direct draw surface format) was stretched over the model shapes vs how repeating the texture could change the look of the object in the scene. The blended + tiled approach uses two different texture files and is applied to the chair object.

## Rotations, directional movement And Interactions:

### Matrix Rotations
The matrix rotation has been applied to three objects in the project, The fan blades, fan centre and the roomba when it reaches the corner. In order to achieve this SimpleMath::Matrix was used.  SimpleMath::Matrix allows for rotation using CreateRotationY or CreateRotationZ depending on what axis about which one wants to rotate and this was multiplied by 180/XM_Pi since the rotation is calculated within radians and then by -1 or 1 if wanting to move clockwise or anti-clockwise respectively. ([DirectX 11 SDK study notes] 3D Transformation, 2022)
<br>
Rotation angle used in SimpleMath::Matrix::CreateRotation is always incremented such as in the case of the Roomba where the angle changed depending on what corner it had moved to in the scene. Translation always follows rotation.

### Quaternion Rotation:
As part of the second half the project requirements, one had to implement an aspect that was not part of the labs one of which was Quaternion rotations, since this was sufficiently covered in the maths part of the course it was opted to be implemented.
<br>
While this is often used for camera rotations, this project opted to apply it on the light source to imitate the sun rising and setting. The basic formula for quaternions is q * p * (q-1), where q, p and the conjugate of q and 4 dimensional vectors with the last vector being 1 for normalisation. The setup of this was understood via the DirectXTK wiki page (Walbourn, 2021).
<br>
A code excerpt for the setup is as follows :

![image2](https://user-images.githubusercontent.com/74312830/222976018-c5d84f1a-a0d4-431a-b71e-939541bbfa50.jpg)
<br>
The calculations here were done as taught in the math module. A fair amount of trial and error was needed in order to find the perfect vectors and to have the light source sufficiently far away enough to give the desired effect.

### Directional Movement :
The roomba in Scene has been set to a square perimeter and is made to move in scene when code is run, here a series of switch loop if statements were used to input the various states of the roomba from case 1 to 4. The frame range is also specified which helps to stop the roomba at a specific point in frame and change directions. The perimeters help define the starting x and z values of the object while the range allows to update the translated values of x and z. To limit the speed of movement the variable for speed is divided by a factor in this case 70 allowing the movement to seem smooth and prevents clipping as well. The perimeter values were determined after drawing it on paper to decide the direction of movement and to cross validate if the calculations reflected as envisioned.
<br>
The roomba was also made to rotate around the corners as previously specified by increasing the rotation angle starting from frame 1-91 in the first loop and from 91-181 in the next and so on and so forth. Once it has rotated it’s position in world is hardcoded to the position it would have at the direction point it will be moving in.

## Audio
The implementation of Audio within the scene was implemented by reading and understanding the methodology outlined in the Microsoft documentation.(Walbourn, 2021). The pch.h header file was altered to allow audio to play for windows systems above Windows 7, the audio redistribution package already present within our nuget package allowed for continuing with setting up audio. The audio used in the project was sourced from (Mixed Progressive House Kit, 2022) which essentially set up the ambient sound that is playing in the background. The entire implementation of Audio is set up within the Game class, where the audio is initialized in the Game::Initialize function and is then called later to play audio. There is also a check to ensure that the audio device actually exists and a retryAudio which when set to true ensures that the audio engine updates only once per frame.

## Custom Geometry
In place of custom geometry a small card square has been created using vectors defined in our game class. And while it has been coloured it does not have textures loaded onto it and suffers from back face culling. This has been intentionally left in as such since it allowed the developer to understand the render pipeline in more depth by having it render in 2D.

## User Input
While the main user controls are the WASD keys, the main functionality is the rotational Yaw using the keyboard and the toggle input system. For the toggle a global flag has been set in the input which when triggered will halt the movement of our roomba vacuum and change it’s texture while it is halted to a second texture that is set for this express purpose. And when the toggle key is pressed yet again movement is resumed and texture switches back to the original texture colour.

# Conclusion
The project called for a 3D application with a variety of implementation choices and requirements set. The above set up satisfies all the basic and advanced requirements needed by the assessment brief. Although this was a back up project created due to the developer’s original project crashing, it allowed for a deeper and more complex understanding of 3D fundamentals. In the original project the developer attempted at using an external library Assimp to assist with animations and other movements such as UV texture movement, while the project was salvageable there was not enough time to redo weeks of effort enough to be presentable as a final submission.  However regardless of the circumstances the developer fully understands the workings of normal and displacement maps along with UV movement. As for the project this report was based on, it allowed the developer to cement the basics of 3D fundamentals. Some additional features that would have been nice to add in had time permitted would have been to have a rotating skybox as present in the original as well as shadow-mapping as outlined in the book of 3D fundamentals by Frank Luna. (Luna., 2012)
<br>
Special thanks to Matthew Bett for his teaching and assistance during this project and module.

# References
1. Walbourn, C., 2021. Using DeviceResources · microsoft/DirectXTK Wiki. [online] GitHub. Available at: <https://github.com/microsoft/DirectXTK/wiki/Using-DeviceResources> [Accessed 20 December 2021].
2. Walbourn, C., 2021. Audio · microsoft/DirectXTK Wiki. [online] GitHub. Available at: <https://github.com/Microsoft/DirectXTK/wiki/Audio> [Accessed 28 June 2021].
3. Sample Focus. 2022. Mixed Progressive House Kit. [online] Available at: <https://samplefocus.com/collections/mixed-progressive-house-kit> [Accessed 13 January 2022].
4. Cla, C., 2022. Sofa grey fabric - Download Free 3D model by charles.cla (@charles.cla) [7f8cca8]. [online] Sketchfab. Available at: <https://sketchfab.com/3d-models/sofa-grey-fabric-7f8cca8fe9d8449684b1dfbb410af8c6> [Accessed 13 January 2022].
5. Garnier, L., 2022. Carpet - Download Free 3D model by Lucas Garnier (@nocyde) [8001f9f]. [online] Sketchfab. Available at: <https://sketchfab.com/3d-models/carpet-8001f9f214f94388840f0ce40cd13937> [Accessed 13 January 2022].
6. Sage, A., 2022. House Plant - Download Free 3D model by AdiSage (@adisage) [4c4cfc7]. [online] Sketchfab. Available at: <https://sketchfab.com/3d-models/house-plant-4c4cfc7a848a45ada6e5facf2280ac3a> [Accessed 13 January 2022].
7. Walbourn, C., 2021. Quaternion · microsoft/DirectXTK Wiki. [online] GitHub. Available at: <https://github.com/microsoft/DirectXTK/wiki/Quaternion> [Accessed 23 October 2021].
8. Titanwolf.org. 2022. [DirectX 11 SDK study notes] 3D Transformation. [online] Available at: <https://titanwolf.org/Network/Articles/Article?AID=6b0ad8b2-de7d-40ea-9843-f9a8ced90d33> [Accessed 13 January 2022].
9. Luna., F., 2012. Introduction to 3D Game Programming with DirectX 11. Mercury Learning.
