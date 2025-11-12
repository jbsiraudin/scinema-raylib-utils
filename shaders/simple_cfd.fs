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
#define PI 3.1415926535
#define SimFrameThreshold 5

#define Res  iChannelResolution0
#define Res1 iChannelResolution1

const float ang = 2.0*PI/float(5);
mat2 m = mat2(cos(ang),sin(ang),-sin(ang),cos(ang));

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
    
    fragColor=texture(iChannel0,fract((pos+v*vec2(-1,1)*iFlowStrength)/Res.xy));
    
    
    if(iFrame<=SimFrameThreshold) {
      fragColor=texture(iChannel2,gl_FragCoord.xy/Res.xy);
    }
    //else {
    //  fragColor=texture(iChannel0, gl_FragCoord.xy/Res.xy);
    //}
    
    //fragColor=vec4(1.0, 0.0, 0.0, 1.0);
}