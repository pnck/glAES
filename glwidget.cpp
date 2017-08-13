#include "glwidget.h"



MyGLWidget::MyGLWidget(QWidget *parent):QOpenGLWidget(parent),
    m_vbo(nullptr),
    m_program(nullptr),
    m_frames(0),
    m_attrPos(-1),
    m_attrTexCoord(-1),
    m_uniformMatrix(-1)
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
}

void MyGLWidget::paintGL()
{

    static float count = 0;
    QMatrix4x4 m;
    //m.scale(0.7f);
    //m.rotate(count+=0.1,0,0,1);

    QPainter painter;
    painter.begin(this);
    painter.beginNativePainting();


    glViewport(0,0,this->width(),this->height());
    glClearColor(0.1f,.0f,0.3f,.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );
    m_program->bind();
    m_program->setUniformValue(m_uniformMatrix,m);
    m_program->enableAttributeArray(m_attrPos);
    m_program->enableAttributeArray(m_attrTexCoord);

    m_vbo->bind();
    m_program->setAttributeBuffer(m_attrPos,GL_FLOAT,Vertex::offsetofX(),3,sizeof(Vertex));
    m_program->setAttributeBuffer(m_attrTexCoord,GL_FLOAT,Vertex::offsetofU(),2,sizeof(Vertex));
    m_vbo->release();

    for(int i(0);i<16;i++)
    {
        if(m_textures[i])
        {
            m_program->setUniformValue(m_uniformTexUnit[i],i);
            m_textures[i]->bind(i);
        }
    }

    glDrawArrays(GL_TRIANGLE_STRIP,0,4);

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

    if (const int elapsed = m_time.elapsed()) {
        QString framesPerSecond;
        framesPerSecond.setNum(m_frames /(elapsed / 1000.0), 'f', 2);
        painter.setPen(Qt::red);
        painter.drawText(20, 40, framesPerSecond + " paintGL calls / s");
    }

    painter.end();
    if (!(m_frames % 60)) {
        m_time.start();
        m_frames = 0;
    }
    ++m_frames;
    update();
}

void MyGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    static QVector<Vertex> vertices;
    vertices.push_back(Vertex(-1,1,0,0,0));
    vertices.push_back(Vertex(-1,-1,0,0,1));
    vertices.push_back(Vertex(1,1,0,1,0));
    vertices.push_back(Vertex(1,-1,0,1,1));

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

    m_uniformTexUnit[0] = m_program->uniformLocation("qt_Texture0");
    m_uniformTexUnit[1] = m_program->uniformLocation("qt_Texture1");

    QImage sboxImg = QImage(":/sbox.png");

    QImage alpha = sboxImg.copy().alphaChannel();
    alpha = alpha.convertToFormat(QImage::Format_Grayscale8);
    uint32_t *p = (uint32_t*)alpha.bits();
    uchar *pp = alpha.bits();
    for(int i(0);i<64*256;i++)
    {
        p[i] ^= 0x66746368;
    }
    for(int i(0);i<256;i++){
        for(int j(0);j<256;j++){
            QColor color = sboxImg.pixelColor(j,i);
            sboxImg.setPixelColor(j,i,QColor(color.red(),color.green(),color.blue(),*(pp+(i*256+j)) ));
        }
    }
    m_textures[0] = new QOpenGLTexture(QImage(":/raw.png"));
    m_textures[1] = new QOpenGLTexture(sboxImg);
    //m_textures[1]->setFixedSamplePositions(true);
    m_textures[1]->setMagnificationFilter(QOpenGLTexture::Nearest);
    m_textures[1]->setMinificationFilter(QOpenGLTexture::Nearest);
    m_textures[1]->setAutoMipMapGenerationEnabled(false);
}
