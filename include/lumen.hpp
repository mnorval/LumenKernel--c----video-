#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>
namespace lumen {
struct Frame {
  int w{0}, h{0};
  std::vector<uint8_t> rgb;
  static Frame alloc(int w, int h) { Frame f; f.w=w; f.h=h; f.rgb.assign(size_t(w*h*3),0); return f; }
  uint8_t& at(int x,int y,int c){ return rgb[(y*w+x)*3+c]; }
  uint8_t at(int x,int y,int c) const { return rgb[(y*w+x)*3+c]; }
};
inline uint8_t clamp8(int v){ return uint8_t(std::clamp(v,0,255)); }
inline Frame luma(const Frame& s){
  auto d=Frame::alloc(s.w,s.h);
  for(int y=0;y<s.h;++y) for(int x=0;x<s.w;++x){
    int Y=(77*s.at(x,y,0)+150*s.at(x,y,1)+29*s.at(x,y,2))>>8;
    d.at(x,y,0)=d.at(x,y,1)=d.at(x,y,2)=clamp8(Y);
  }
  return d;
}
inline Frame conv3(const Frame& s, const int k[9], int div, int bias=0){
  auto d=Frame::alloc(s.w,s.h);
  for(int y=1;y<s.h-1;++y) for(int x=1;x<s.w-1;++x) for(int c=0;c<3;++c){
    int acc=0,t=0;
    for(int j=-1;j<=1;++j) for(int i=-1;i<=1;++i) acc += s.at(x+i,y+j,c)*k[t++];
    d.at(x,y,c)=clamp8(acc/div+bias);
  }
  return d;
}
inline Frame sobel_mag(const Frame& g){
  static const int gx[9]={-1,0,1,-2,0,2,-1,0,1};
  static const int gy[9]={-1,-2,-1,0,0,0,1,2,1};
  auto d=Frame::alloc(g.w,g.h);
  for(int y=1;y<g.h-1;++y) for(int x=1;x<g.w-1;++x){
    int sx=0,sy=0,t=0;
    for(int j=-1;j<=1;++j) for(int i=-1;i<=1;++i){
      int p=g.at(x+i,y+j,0); sx+=p*gx[t]; sy+=p*gy[t]; ++t;
    }
    int m=int(std::sqrt(double(sx*sx+sy*sy)));
    d.at(x,y,0)=d.at(x,y,1)=d.at(x,y,2)=clamp8(m);
  }
  return d;
}
inline int sad(const Frame& a, const Frame& b){
  int s=0; auto n=std::min(a.rgb.size(), b.rgb.size());
  for(size_t i=0;i<n;++i) s += std::abs(int(a.rgb[i])-int(b.rgb[i]));
  return s;
}
}
