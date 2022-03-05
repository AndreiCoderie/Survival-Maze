#version 330

// Input
// TODO(student): Get vertex attributes from each location

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec3 v_coordinate;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;


uniform vec3 object_color;

// Output
// TODO(student): Output values to fragment shader

out vec3 color;

void main()
{
    // TODO(student): Send output to fragment shader
    color = object_color;

    gl_Position = Projection * View * Model * vec4(v_position, 1);


}