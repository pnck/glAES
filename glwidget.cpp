#include "glwidget.h"
#include <QGenericMatrix>
#include <QFile>


MyGLWidget::MyGLWidget(QWidget *parent): QOpenGLWidget(parent),
    m_vbo(nullptr),
    m_progXorKey(nullptr),
    m_progSub_Shift(nullptr),
    m_progMixCol(nullptr),
    m_progCbc(nullptr),
    m_progForUI(nullptr),
    m_frames(0),
    m_attrPos(-1),
    m_attrTexCoord(-1),
    m_uniformMatrix(-1),
    m_stateFbo(nullptr)
{
    memset(m_uniformTexUnit, -1, sizeof(m_uniformTexUnit));
    memset(m_textures, 0, sizeof(m_textures));
}

MyGLWidget::~MyGLWidget()
{
    if(m_vbo) {
        m_vbo->destroy();
        delete m_vbo;
    }
    for(QOpenGLTexture *tex : m_textures) {
        if(tex) {
            tex->destroy();
            delete tex;
        }
    }
    if(m_progXorKey) {
        m_progXorKey->release();
        delete m_progXorKey;
    }
    if(m_progSub_Shift) {
        m_progSub_Shift->release();
        delete m_progSub_Shift;
    }
    if(m_progMixCol) {
        m_progMixCol->release();
        delete m_progMixCol;
    }
    if(m_progCbc) {
        m_progCbc->release();
        delete m_progCbc;
    }
    if(m_progForUI) {
        m_progForUI->release();
        delete m_progForUI;
    }
    if(m_stateFbo) {
        m_stateFbo->release();
        delete m_stateFbo;
    }
}

void MyGLWidget::paintGL()
{

    static float count = 0;
    QMatrix4x4 m;
    m.scale(0.8f);
    m.rotate(count += 0.1, 0, 0, 1);

    QPainter painter;
    painter.begin(this);
    painter.beginNativePainting();


    glViewport(0,0,this->width(),this->height());
    glClearColor(0.3f, 0.3f, 0.35f, .0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);

    m_vbo->bind();

    m_progForUI->bind();
    m_progForUI->setUniformValue(m_uniformMatrix, m);
    m_progForUI->setAttributeBuffer(m_attrPos, GL_FLOAT, Vertex::offsetofX(), 3, sizeof(Vertex));
    m_progForUI->setAttributeBuffer(m_attrTexCoord, GL_FLOAT, Vertex::offsetofU(), 2, sizeof(Vertex));


    m_progForUI->enableAttributeArray(m_attrPos);
    m_progForUI->enableAttributeArray(m_attrTexCoord);


    m_progForUI->setUniformValue("rawTex", 0);
    m_textures[TF_RAW]->bind(0);

    glDrawArrays(GL_QUADS, 0, 4);

#if 1
    if(m_stateFbo) {
        m_progXorKey->setAttributeBuffer("vertex", GL_FLOAT, Vertex::offsetofX(), 3, sizeof(Vertex));
        m_progXorKey->setAttributeBuffer("generalTexCoord", GL_FLOAT, Vertex::offsetofU(), 2, sizeof(Vertex));

        m_progSub_Shift->setAttributeBuffer("vertex", GL_FLOAT, Vertex::offsetofX(), 3, sizeof(Vertex));
        m_progSub_Shift->setAttributeBuffer("generalTexCoord", GL_FLOAT, Vertex::offsetofU(), 2, sizeof(Vertex));

        m_progMixCol->setAttributeBuffer("vertex", GL_FLOAT, Vertex::offsetofX(), 3, sizeof(Vertex));
        m_progMixCol->setAttributeBuffer("generalTexCoord", GL_FLOAT, Vertex::offsetofU(), 2, sizeof(Vertex));

        m_progCbc->setAttributeBuffer("vertex", GL_FLOAT, Vertex::offsetofX(), 3, sizeof(Vertex));
        m_progCbc->setAttributeBuffer("generalTexCoord", GL_FLOAT, Vertex::offsetofU(), 2, sizeof(Vertex));

        m_stateFbo->bind();
        glViewport(0, 0, m_stateFbo->width(), m_stateFbo->height());

        m_stateFbo->release();
    }
#endif
    for(int i(0); i < 32; i++) {
        if(m_textures[i]) {
            m_textures[i]->release();
        }
    }
    m_progForUI->disableAttributeArray(m_attrPos);
    m_progForUI->disableAttributeArray(m_attrTexCoord);
    m_progForUI->release();



    m_vbo->release();

    glDisable(GL_BLEND);


    painter.endNativePainting();
#if 1
    if(const int elapsed = m_time.elapsed()) {
        QString framesPerSecond;
        framesPerSecond.setNum(m_frames / (elapsed / 1000.0), 'f', 2);
        painter.setPen(Qt::red);
        painter.drawText(20, 40, framesPerSecond + " paintGL calls / s");
    }

    if(!(m_frames % 60)) {
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
    vertices.push_back(Vertex(-1, 1, 0, 0, 0));
    vertices.push_back(Vertex(-1, -1, 0, 0, 1));
    vertices.push_back(Vertex(1, -1, 0, 1, 1));
    vertices.push_back(Vertex(1, 1, 0, 1.0, 0));

    m_vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    m_vbo->create();
    m_vbo->bind();
    m_vbo->allocate(&(vertices[0]), vertices.size()*sizeof(Vertex));
    m_vbo->release();


    m_progForUI = new QOpenGLShaderProgram;
    m_progForUI->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/glsl/vshader_ui.vert");
    m_progForUI->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/glsl/uishow.frag");
    m_progForUI->link();
    m_attrPos = m_progForUI->attributeLocation("vertex");
    m_attrTexCoord = m_progForUI->attributeLocation("generalTexCoord");
    m_uniformMatrix = m_progForUI->uniformLocation("matrix");

    m_progXorKey = new QOpenGLShaderProgram;
    m_progXorKey->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/glsl/vshader_trivial.vert");
    m_progXorKey->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/glsl/xorkey.frag");
    m_progXorKey->link();

    m_progSub_Shift = new QOpenGLShaderProgram;
    m_progSub_Shift->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/glsl/vshader_trivial.vert");
    m_progSub_Shift->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/glsl/xorkey.frag");
    m_progSub_Shift->link();

    m_progMixCol = new QOpenGLShaderProgram;
    m_progMixCol->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/glsl/vshader_trivial.vert");
    m_progMixCol->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/glsl/xorkey.frag");
    m_progMixCol->link();


    m_progCbc = new QOpenGLShaderProgram;
    m_progCbc->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/glsl/vshader_trivial.vert");
    m_progCbc->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/glsl/xorkey.frag");
    m_progCbc->link();

    m_vbo->bind();
    m_progForUI->setAttributeBuffer(m_attrPos, GL_FLOAT, Vertex::offsetofX(), 3, sizeof(Vertex));
    m_progForUI->setAttributeBuffer(m_attrTexCoord, GL_FLOAT, Vertex::offsetofU(), 2, sizeof(Vertex));
    m_vbo->release();



    prepareTextures();
}

void MyGLWidget::prepareTextures()
{
    //// ============  prepare texture  ==========
    QFile::remove("gen_sbox_image.py");
    QFile::remove("raw.png");
    QFile::remove("_1.png");
    QFile::remove("_2.png");
    QFile::copy(":/gen_sbox_image.py", "gen_sbox_image.py");
    QFile::copy(":/raw.png", "raw.png");
    system("python3 gen_sbox_image.py raw.png");

    auto decodeSboxImage = [&](QImage & img) {
        QImage alpha = img.copy().alphaChannel();
        alpha = alpha.convertToFormat(QImage::Format_Grayscale8);
        uint32_t *p = (uint32_t*)alpha.bits();
        uchar *pp = alpha.bits();
        for(int i(0); i < 64 * 256; i++) {
            p[i] ^= 0x66746368;
        }
        for(int i(0); i < 256; i++) {
            for(int j(0); j < 256; j++) {
                QColor color = img.pixelColor(j, i);
                img.setPixelColor(j, i, QColor(color.red(), color.green(), color.blue(), *(pp + (i * 256 + j))));
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


    QImage calcTableImg(256, 256, QImage::Format_RGBA8888);

    auto preMul = [](uint8_t a, uint8_t b)->uint8_t {  //有限域GF(2^8)上的乘法
        unsigned char bw[4];
        unsigned char res = 0;
        int i;
        bw[0] = b;
        for(i = 1; i < 4; i++)  //循环三次，分别得到参数b乘2、4、8后的值，储存到bw[i]里面
        {
            bw[i] = bw[i - 1] << 1; //原数值乘2
            if(bw[i - 1] & 0x80) { //判断原数值是否小于0x80
                bw[i] ^= 0x1b;  //如果大于0x80的话，减去一个不可约多项式
            }
        }
        for(i = 0; i < 4; i++)
        {
            if((a >> i) & 0x01) { //将参数a的值表示为1、2、4、8的线性组合
                res ^= bw[i];
            }
        }
        return res;
    };


    for(int i(0); i < 256; i++) {
        for(int j(0); j < 256; j++) {
            calcTableImg.setPixelColor(j, i, QColor(i ^ j, preMul(i,j),0,0));//r->xor  g->mul
        }
    }
    m_textures[TF_CALC_LUT] = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_textures[TF_CALC_LUT]->setAutoMipMapGenerationEnabled(false);
    m_textures[TF_CALC_LUT]->setMagnificationFilter(QOpenGLTexture::Nearest);
    m_textures[TF_CALC_LUT]->setMinificationFilter(QOpenGLTexture::Nearest);
    m_textures[TF_CALC_LUT]->setWrapMode(QOpenGLTexture::ClampToEdge);
    m_textures[TF_CALC_LUT]->setData(calcTableImg,QOpenGLTexture::DontGenerateMipMaps);

    m_textures[TF_ROUND_KEY] = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_textures[TF_ROUND_KEY]->setAutoMipMapGenerationEnabled(false);
    m_textures[TF_ROUND_KEY]->setMagnificationFilter(QOpenGLTexture::Nearest);
    m_textures[TF_ROUND_KEY]->setMinificationFilter(QOpenGLTexture::Nearest);
    m_textures[TF_ROUND_KEY]->setWrapMode(QOpenGLTexture::ClampToEdge);
    m_textures[TF_ROUND_KEY]->setData(QImage(":/key.png"),QOpenGLTexture::DontGenerateMipMaps);
/*
    m_uniformTexUnit[TF_RAW] = m_progForUI->uniformLocation("rawTex");
    m_uniformTexUnit[TF_SBOX_LUT] = m_progForUI->uniformLocation("lut_sbox");
    m_uniformTexUnit[TF_INV_SBOX_LUT] = m_progForUI->uniformLocation("lut_invsbox");
    m_uniformTexUnit[TF_CALC_LUT] = m_progForUI->uniformLocation("lut_xor");
*/
}

void MyGLWidget::prepareData(QOpenGLTexture *pTexturedData)
{
    makeCurrent();
    m_stateFbo = new QOpenGLFramebufferObject(4,pTexturedData->height());
}

void MyGLWidget::capturing()
{
    QRect r = geometry();
    setGeometry(0, 0, 256, 256);
    //makeCurrent();
    m_stateFbo->bind();
    //glViewport(0,0,256,256);
    glViewport(0, 0, 16, 16);
    QImage img = m_stateFbo->toImage();
    img.save("_g.png");
    m_stateFbo->release();
    setGeometry(r);
}
