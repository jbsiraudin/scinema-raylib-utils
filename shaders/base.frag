#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables

void main()
{
    finalColor = vec4(1.0, 0.0, 0.0, 1.0);
}
