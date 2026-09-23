#include "lumen.hpp"
#include <cstdio>
static lumen::Frame scene(int w,int h,int ox){
  auto f=lumen::Frame::alloc(w,h);
  for(int y=0;y<h;++y) for(int x=0;x<w;++x){
    bool bar=((x+ox)/8)&1; bool diag=(x+y+ox)%17<3;
    f.at(x,y,0)=bar?220:20; f.at(x,y,1)=diag?200:40; f.at(x,y,2)=uint8_t((x*3+y)&255);
  }
  return f;
}
int main(){
  auto a=scene(64,48,0), b=scene(64,48,3);
  auto y=lumen::luma(a);
  static const int blur[9]={1,2,1,2,4,2,1,2,1};
  auto soft=lumen::conv3(y,blur,16);
  auto edges=lumen::sobel_mag(soft);
  int e=0; for(auto p:edges.rgb) e+=p;
  std::printf("LumenKernel 64x48\nSAD shift3=%d\nSobel energy=%d\ncenter luma=%u edge=%u\n",
    lumen::sad(a,b), e/3, y.at(32,24,0), edges.at(32,24,0));
}
