//
//  GLTexturePool.h
//  RedwoodExample
//
//  Created by lingtonke on 2019/9/6.
//  Copyright © 2019 tencent. All rights reserved.
//

#ifndef GLTexturePool_h
#define GLTexturePool_h

#include "GLTexture.h"
OGL_NAMESPACE_BEGIN(ObjectiveGL)
using namespace std;
class OGL_API GLTexturePool:public enable_shared_from_this<GLTexturePool> {
protected:
    map<pair<int,int>,GLTexture*> textures;
public:
    shared_ptr<GLTexture> getTexture(int width,int height) {
        auto pool = weak_ptr<GLTexturePool>(shared_from_this());
        auto func = [=](GLTexture *tex){
            
        };
        if (textures.size()>0) {
            auto it = textures.find(make_pair(width, height));
            if (it==textures.end()) {
                it = textures.begin();
            }
            textures.erase(it);
            return shared_ptr<GLTexture>(it->second,func);
        }
        else {
            auto texture = GLTexture::create(func);
            return texture;
        }
    }
};

OGL_NAMESPACE_END(ObjectiveGL)
#endif /* GLTexturePool_h */
