#version 330

uniform mat4 Projection = mat4(1);
uniform mat4 ModelView = mat4(1);

in vec3 Position;
in vec2 TexCoord;

out vec2 vTexCoord;

void main() {
    gl_Position = Projection * ModelView * vec4(Position, 1);

    vTexCoord = TexCoord;
}
