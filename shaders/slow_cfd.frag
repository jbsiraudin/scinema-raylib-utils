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

#define angRnd 1.0
#define posRnd 0.0
#define Res  iChannelResolution0
#define Res1 iChannelResolution1

const float ang = 2.0*PI/float(5);
mat2 m = mat2(cos(ang),sin(ang),-sin(ang),cos(ang));

float hash(float seed) { return fract(sin(seed)*158.5453 ); }
vec4 getRand4(float seed) { return vec4(hash(seed),hash(seed+123.21),hash(seed+234.32),hash(seed+453.54)); }
vec4 randS(vec2 uv)
{
    return texture(iChannel1,uv*Res.xy/Res1.xy)-vec4(0.5);
}

float getRot(vec2 uv, float sc)
{
    float ang2 = angRnd*randS(uv).x*ang;
    vec2 p = vec2(cos(ang2),sin(ang2));
    float rot=0.0;
    for(int i=0;i<iRotNum;i++)
    {
        vec2 p2 = (p+posRnd*randS(uv+p*sc).xy)*sc;
        vec2 v = texture(iChannel0,fract(uv+p2)).xy-vec2(0.5);
        rot+=cross(vec3(v,0.0),vec3(p2,0.0)).z/dot(p2,p2);
        p = m*p;
    }
    rot/=float(iRotNum);
    return rot;
}

void main()
{
    vec2 pos = gl_FragCoord.xy;
    vec2 uv = gl_FragCoord.xy/Res.xy;
    vec2 scr = (gl_FragCoord.xy/Res.xy)*2.0-vec2(1.0);
    
    float rnd = randS(vec2(float(iFrame)/Res.x,0.5/Res1.y)).x;
    
    vec2 b = vec2(cos(ang*rnd),sin(ang*rnd));
    float sc = 1.0/max(Res.x,Res.y);
    vec2 v = vec2(0);
    float bbMax=0.7*Res.y; bbMax*=bbMax;
    for(int l=0;l<iItNum;l++)
    {
        if ( dot(b,b) > bbMax ) break;
        float ang2 = angRnd*ang*randS(uv).y;
        vec2 p = vec2(cos(ang2),sin(ang2));
        for(int i=0;i<iRotNum;i++)
        {
            vec2 p2=p*sc;
            float rot=getRot(uv+p2,sc);
            //v+=cross(vec3(0,0,rot),vec3(p2,0.0)).xy;
            v+=p2.yx*rot*vec2(-1,1); //maybe faster than above
            p = m*p;
        }
        sc*=2.0;
    }
    // smash effect
    //v.y+=scr.y*0.5;
    
    //river effect
    //v.x+=(1.0-scr.y*scr.y)*0.8;
    
    fragColor=texture(iChannel0,fract((pos+v*vec2(-1,1)*iFlowStrength)/Res.xy));
    
    
    if(iFrame<=SimFrameThreshold) {
      fragColor=texture(iChannel2,uv);
    }
    //else {
    //  fragColor=texture(iChannel0, gl_FragCoord.xy/Res.xy);
    //}
    
    //fragColor=vec4(1.0, 0.0, 0.0, 1.0);
}