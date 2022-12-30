#version 460
in vec4 interp;
in vec2 uv;
out vec4 fb_color;

#define TAU 6.28318530718
#define BLACK 0,0,0

float inverseMix(float a, float b, float v) {
    return (v - a) / (b - a);
}

vec2 inverseMix2(float a, float b, vec2 v) {
    return vec2((v.x - a) / (b - a), (v.y - a) / (b - a));
}

bool drawBorder() {
    float size = 0.007;
    if(0.0 <= uv.x && uv.x <= size) {
        return true;
    } else if(1 - size <= uv.x && uv.x <= 1.0) {
        return true;
    } else if(0.0 <= uv.y && uv.y <= size + .003) {
        return true;
    } else if(1 - size - .003 <= uv.y && uv.y <= 1.0) {
        return true;
    }

    return false;
}

void main() {
    // .5 * sin(TAU * (x+.25)) +.5
    // float time = cos(interp.w * TAU) * .5 + .5;
    // fb_color = vec4(interp.rg * time, interp.b, 1.0);
    // fb_color = vec4(interp.r * (cos(interp.w * TAU) * .5 + .5), interp.gb, 1.0);

    if(drawBorder()) {
        fb_color = vec4(BLACK, 1);
        return;
    }

    float xOffest = cos(uv.y * TAU * 8) * .01;
    // vec3 color = .5 * cos((uv.xxx + xOffest) * 5 * TAU) + .5;
    vec3 color = .5 * cos((uv.xxx + xOffest + interp.w * .1f) * 5 * TAU) + .5;

    // vec3 color = abs(fract(uv.xxx * 5) * 2 - 1); // triangle wave
    // fb_color = vec4(abs(uv.xxx * 2 - 1), 1.0);

    fb_color = vec4(color, 1.0);

    return;

    // float t = clamp(inverseMix(0.1, 0.25, uv.x), 0, 1);
    // float t = clamp(inverseMix(0, 1, color), 0, 1);
    // vec2 t = clamp(inverseMix2(0, 1, color), 0, 1);
    // t = t - floor(t); // frac
    // t = fract(t); // frac
    vec3 color1 = vec3(1, 0, 0);
    vec3 color2 = vec3(0, 0, 1);

    // vec3 result = mix(color1, color2, t);

    // fb_color = vec4(mix(result, 1.0), 1.0);

    // fb_color = vec4(interp.rgb, 1.0);
    // fb_color = vec4(1, 0, 0, 1.0);
}
