
#include "framebuffer.h"
#include "opengl.h"
#include "../common/util.h"

void Framebuffer::init(int w, int h) {
    glGenFramebuffers(1, &fbo);
    bind();

    col.init();
    col.create(w, h, COLOR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, col.tex, 0);
    norm.init();
    norm.create(w, h, COLOR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, norm.tex, 0);
    pos.init();
    pos.create(w, h, COLOR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, pos.tex, 0);
    erm.init();
    erm.create(w, h, COLOR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, erm.tex, 0);
    depth.init();
    depth.create(w, h, DEPTH_STENCIL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth.tex, 0);

    unsigned int attachments[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
    glDrawBuffers(4, attachments);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "framebuffer ded :(\n");
        exit(-1);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    this->w = w;
    this->h = h;

}

void Framebuffer::free() {
    glDeleteFramebuffers(1, &fbo);
}

void Framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void Framebuffer::renderTo() {
    bind();
    glViewport(0, 0, w, h);
}

void Framebuffer::resize(int w, int h) {
    if(w != this->w || h != this->h) {
        col.create(w, h, COLOR);
        norm.create(w, h, COLOR);
        pos.create(w, h, COLOR);
        erm.create(w, h, COLOR);
        depth.create(w, h, DEPTH_STENCIL);
        this->w = w;
        this->h = h;
    }
}
