#include "glwidget.h"
#include <QGenericMatrix>
#include <QFile>


MyGLWidget::MyGLWidget(QWidget *parent):QOpenGLWidget(parent),
    m_vbo(nullptr),
    m_program(nullptr),
    m_frames(0),
    m_attrPos(-1),
    m_attrTexCoord(-1),
    m_uniformMatrix(-1),
    m_stateFbo(nullptr)
{
    memset(m_uniformTexUnit,-1,sizeof(m_uniformTexUnit));
    memset(m_textures,0,sizeof(m_textures));
}

MyGLWidget::~MyGLWidget()
{
    if(m_vbo){
        m_vbo->destroy();
        delete m_vbo;
    }
    for(QOpenGLTexture *tex : m_textures)
    {
        if(tex)
        {
            tex->destroy();
            delete tex;
        }
    }
    if(m_program)
    {
        m_program->release();
        delete m_program;
    }
    if(m_stateFbo){
        m_stateFbo->release();
        delete m_stateFbo;
    }
}

void MyGLWidget::paintGL()
{

    static float count = 0;
    QMatrix4x4 m;
    m.scale(0.8f);
    m.rotate(count+=0.1,0,0,1);

    QPainter painter;
    painter.begin(this);
    painter.beginNativePainting();


    //glViewport(0,0,this->width(),this->height());
    glClearColor(0.3f,0.3f,0.35f,.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );
    m_program->bind();
    m_program->setUniformValue(m_uniformMatrix,m);

    m_vbo->bind();
    m_program->setAttributeBuffer(m_attrPos,GL_FLOAT,Vertex::offsetofX(),3,sizeof(Vertex));
    m_program->setAttributeBuffer(m_attrTexCoord,GL_FLOAT,Vertex::offsetofU(),2,sizeof(Vertex));
    m_vbo->release();

    m_program->enableAttributeArray(m_attrPos);
    m_program->enableAttributeArray(m_attrTexCoord);


    for(int i(0);i<16;i++)
    {
        if(m_textures[i])
        {
            m_program->setUniformValue(m_uniformTexUnit[i],i);
            m_textures[i]->bind(i);
        }
    }

    glDrawArrays(GL_QUADS,0,4);

    if(m_stateFbo){
        m_stateFbo->bind();
        glViewport(0,0,64,64);
        m_program->setUniformValue(m_uniformMatrix,QMatrix4x4());
        glDrawArrays(GL_QUADS,0,4);
        m_stateFbo->release();
    }

    for(int i(0);i<16;i++)
    {
        if(m_textures[i])
        {
            m_textures[i]->release();
        }
    }
    m_program->disableAttributeArray(m_attrPos);
    m_program->disableAttributeArray(m_attrTexCoord);
    m_program->release();
    glDisable( GL_BLEND );


    painter.endNativePainting();
#if 1
    if (const int elapsed = m_time.elapsed()) {
        QString framesPerSecond;
        framesPerSecond.setNum(m_frames /(elapsed / 1000.0), 'f', 2);
        painter.setPen(Qt::red);
        painter.drawText(20, 40, framesPerSecond + " paintGL calls / s");
    }

    if (!(m_frames % 60)) {
        m_time.start();
        m_frames = 0;
    }
    ++m_frames;

    update();
#endif
    painter.end();
}

void MyGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    static QVector<Vertex> vertices;
    vertices.push_back(Vertex(-1,1,0,0,0));
    vertices.push_back(Vertex(-1,-1,0,0,1));
    vertices.push_back(Vertex(1,-1,0,1,1));
    vertices.push_back(Vertex(1,1,0,1.0,0));

    m_vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    m_vbo->create();
    m_vbo->bind();
    m_vbo->allocate(&(vertices[0]),vertices.size()*sizeof(Vertex));
    m_vbo->release();

    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex,":/glsl/vshader.vert");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment,":/glsl/fshader.frag");
    m_program->link();
    m_attrPos = m_program->attributeLocation("vertex");
    m_attrTexCoord = m_program->attributeLocation("generalTexCoord");
    m_uniformMatrix = m_program->uniformLocation("matrix");

    m_vbo->bind();
    m_program->setAttributeBuffer(m_attrPos,GL_FLOAT,Vertex::offsetofX(),3,sizeof(Vertex));
    m_program->setAttributeBuffer(m_attrTexCoord,GL_FLOAT,Vertex::offsetofU(),2,sizeof(Vertex));
    m_vbo->release();



    m_stateFbo = new QOpenGLFramebufferObject(64,64);
    //m_stateFbo->bind();

    prePareTextures();
}

void MyGLWidget::prePareTextures()
{
    //// ============  prepare texture  ==========
    QFile::remove("gen_sbox_image.py");
    QFile::remove("raw.png");
    QFile::remove("_1.png");
    QFile::remove("_2.png");
    QFile::copy(":/gen_sbox_image.py","gen_sbox_image.py");
    QFile::copy(":/raw.png","raw.png");
    system("python3 gen_sbox_image.py raw.png");

    auto decodeSboxImage = [&](QImage & img){
        QImage alpha = img.copy().alphaChannel();
        alpha = alpha.convertToFormat(QImage::Format_Grayscale8);
        uint32_t *p = (uint32_t*)alpha.bits();
        uchar *pp = alpha.bits();
        for(int i(0);i<64*256;i++)
        {
            p[i] ^= 0x66746368;
        }
        for(int i(0);i<256;i++){
            for(int j(0);j<256;j++){
                QColor color = img.pixelColor(j,i);
                img.setPixelColor(j,i,QColor(color.red(),color.green(),color.blue(),*(pp+(i*256+j)) ));
            }
        }
    };

    QImage sboxImg = QImage("_1.png");
    decodeSboxImage(sboxImg);
    //sboxImg.copy().convertToFormat(QImage::Format_RGBA8888).save("_2.png");
    m_textures[TF_RAW] = new QOpenGLTexture(QImage(":/raw.png"));
    m_textures[TF_SBOX_LUT] = new QOpenGLTexture(QOpenGLTexture::Target1D);
    //m_textures[TF_SBOX_LUT]->setFixedSamplePositions(true);
    m_textures[TF_SBOX_LUT]->setAutoMipMapGenerationEnabled(false);
    m_textures[TF_SBOX_LUT]->setSize(256);
    m_textures[TF_SBOX_LUT]->setData(sboxImg.alphaChannel());
    m_textures[TF_SBOX_LUT]->setMagnificationFilter(QOpenGLTexture::Nearest);
    m_textures[TF_SBOX_LUT]->setMinificationFilter(QOpenGLTexture::Nearest);
    m_textures[TF_SBOX_LUT]->setWrapMode(QOpenGLTexture::ClampToEdge);

    QImage invsboxImg = QImage("_2.png");
    decodeSboxImage(invsboxImg);
    m_textures[TF_INV_SBOX_LUT] = new QOpenGLTexture(QOpenGLTexture::Target1D);
    m_textures[TF_INV_SBOX_LUT]->setAutoMipMapGenerationEnabled(false);
    m_textures[TF_INV_SBOX_LUT]->setSize(256);
    m_textures[TF_INV_SBOX_LUT]->setData(invsboxImg.alphaChannel());
    m_textures[TF_INV_SBOX_LUT]->setMagnificationFilter(QOpenGLTexture::Nearest);
    m_textures[TF_INV_SBOX_LUT]->setMinificationFilter(QOpenGLTexture::Nearest);
    m_textures[TF_INV_SBOX_LUT]->setWrapMode(QOpenGLTexture::ClampToEdge);

    QImage xorTableImg(256,256,QImage::Format_Grayscale8);
    uchar *xorTable = xorTableImg.bits();
    for(int i(0);i<256;i++)
    {
        for(int j(0);j<256;j++)
        {
            xorTable[i*256+j] = i^j;
        }
    }
    m_textures[TF_XOR_LUT] = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_textures[TF_XOR_LUT]->setAutoMipMapGenerationEnabled(false);
    m_textures[TF_XOR_LUT]->setMagnificationFilter(QOpenGLTexture::Nearest);
    m_textures[TF_XOR_LUT]->setMinificationFilter(QOpenGLTexture::Nearest);
    m_textures[TF_XOR_LUT]->setWrapMode(QOpenGLTexture::ClampToEdge);
    m_textures[TF_XOR_LUT]->setData(xorTableImg);


    m_uniformTexUnit[TF_RAW] = m_program->uniformLocation("rawTex");
    m_uniformTexUnit[TF_SBOX_LUT] = m_program->uniformLocation("lut_sbox");
    m_uniformTexUnit[TF_INV_SBOX_LUT] = m_program->uniformLocation("lut_invsbox");
    m_uniformTexUnit[TF_XOR_LUT] = m_program->uniformLocation("lut_xor");

}

void MyGLWidget::capturing()
{
    QRect r = geometry();
    setGeometry(0,0,256,256);
    //makeCurrent();
    m_stateFbo->bind();
    //glViewport(0,0,256,256);
    glViewport(0,0,16,16);
    QImage img = m_stateFbo->toImage();
    img.save("_g.png");
    m_stateFbo->release();
    setGeometry(r);
}
