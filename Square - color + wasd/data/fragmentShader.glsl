#version 330 core

in vec3 fragmentColor;

out vec3 color;

void main(){
//    color = vec3(1,0.5,0);//R, G, B
	color = fragmentColor;
}