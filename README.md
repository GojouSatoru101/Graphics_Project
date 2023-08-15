First of all, install and setup glfw and GLAD by following 
https://learnopengl.com/Getting-started/Creating-a-window

Add them in the include directory of your Visual Studio Project

Download GLM from https://glm.g-truc.net/0.9.8/index.html
Copy the root directory of the header files into your include directory

Download assimp from https://kimkulling.itch.io/the-asset-importer-lib

And do the same thing using Cmake while setting up Opengl 
OR just follow this youtube tutorial https://youtu.be/W_Ey_YPUjMk

Make sure to include the models and the images in same folder as your project solution or include directory and configure the path

Also, import the .obj file of model in blender and export it again as .obj file since, for different devices things will change.