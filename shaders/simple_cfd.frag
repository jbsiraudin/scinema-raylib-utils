#version 330

// Input uniform values
uniform sampler2D iChannel0; // main buffer texture
uniform sampler2D iChannel1; // noise texture for random
uniform sampler2D iChannel2; // image input
uniform vec2 iChannelResolution0;
uniform vec2 iChannelResolution1;
uniform int iFrame;
uniform int iRotNum;
uniform int iItNum;
uniform float iFlowStrength;
uniform vec2 iFlowDirection;


// Output fragment color
out vec4 fragColor;

// macros
#define pow2(x) (x * x)
#define PI 3.1415926535
#define SimFrameThreshold 5

#define Res  iChannelResolution0
#define Res1 iChannelResolution1

const float ang = 2.0*PI/float(5);
mat2 m = mat2(cos(ang),sin(ang),-sin(ang),cos(ang));
const int samples = 8;
const float sigma = float(samples) * 0.05;

float gaussian(vec2 i) {
    return 1.0 / (2.0 * PI * pow2(sigma)) * exp(-((pow2(i.x) + pow2(i.y)) / (2.0 * pow2(sigma))));
}

vec3 blur(sampler2D sp, vec2 uv) {
    vec2 scale = vec2(1.0) / Res.xy;
    vec3 col = vec3(0.0);
    float accum = 0.0;
    float weight;
    vec2 offset;
    
    for (int x = -samples / 2; x < samples / 2; ++x) {
        for (int y = -samples / 2; y < samples / 2; ++y) {
            offset = vec2(x, y);
            weight = gaussian(offset);
            col += texture(sp, uv + scale * offset).rgb * weight;
            accum += weight;
        }
    }
    
    return col / accum;
}

vec4 randS(vec2 uv)
{
    return texture(iChannel1,uv*Res.xy/Res1.xy)-vec4(0.5);
}

float getRot(vec2 pos, vec2 b)
{
    vec2 p = b;
    float rot=0.0;
    for(int i=0;i<iRotNum;i++)
    {
        rot+=dot(texture(iChannel0,fract((pos+p)/Res.xy)).xy-vec2(0.5),p.yx*iFlowDirection);
        p = m*p;
    }
    return rot/float(iRotNum)/dot(b,b);
}

void main()
{
    vec2 pos = gl_FragCoord.xy;
    
    vec2 scr = (gl_FragCoord.xy/Res.xy)*2.0-vec2(1.0);
    float rnd = randS(vec2(float(iFrame)/Res.x,0.5/Res1.y)).x;
    
    vec2 b = vec2(cos(ang*rnd),sin(ang*rnd));
    vec2 v = vec2(0);
    float bbMax=0.7*Res.y; bbMax*=bbMax;
    for(int l=0;l<iItNum;l++)
    {
        if ( dot(b,b) > bbMax ) break;
        vec2 p = b;
        for(int i=0;i<iRotNum;i++)
        {
            // this is faster but works only for odd iRotNum
            v+=p.yx*getRot(pos+p,b);
            p = m*p;
        }
        b*=2.0;
    }
    // smash effect
    //v.y+=scr.y*0.5;
    
    //river effect
    //v.x+=(1.0-scr.y*scr.y)*0.8;
    
    vec2 displaced_p = fract((pos+v*iFlowDirection*iFlowStrength)/Res.xy);
    //fragColor=texture(iChannel0,fract((pos+v*iFlowDirection*iFlowStrength)/Res.xy));    
    fragColor = vec4(blur(iChannel0, displaced_p), 1.0);

    if(iFrame<=SimFrameThreshold) {
      fragColor=texture(iChannel2,gl_FragCoord.xy/Res.xy);
    }
    //else {
    //  fragColor=texture(iChannel0, gl_FragCoord.xy/Res.xy);
    //}
    
    //fragColor=vec4(1.0, 0.0, 0.0, 1.0);
}