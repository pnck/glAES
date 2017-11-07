#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QObject>
#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_0>
#include <QOpenGLShaderProgram>
#include <QTime>
#include <QPainter>
#include <QPaintEngine>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QImage>
#include <QVector3D>
#include <QVector2D>
#include <QVector>
#include <QOpenGLFramebufferObject>


class Vertex
{
private:
    float _x,_y,_z,_u,_v;
public:
    Vertex():_x(0),_y(0),_z(0),_u(0),_v(){}
    Vertex(float x, float y, float z, float u, float v){set(x,y,z,u,v);}
    inline void set(float x, float y, float z, float u, float v){_x=x;_y=y;_z=z;_u=u;_v=v;}
    inline Vertex & operator <<(const QVector3D & v){_x=v.x(),_y=v.y(),_z=v.z();return *this;}
    inline Vertex & operator <<(const QVector2D & v){_u=v.x(),_v=v.y();return *this;}
    static inline int offsetofX(){return offsetof(Vertex,_x);}
    static inline int offsetofU(){return offsetof(Vertex,_u);}

};

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_0
{
    Q_OBJECT
private:
    enum TextureFunction{
        TF_RAW = 0,
        TF_SBOX_LUT,
        TF_INV_SBOX_LUT,
        TF_CALC_LUT,
        TF_STATE,
        TF_ROUND_KEY
    };

public:
    explicit MyGLWidget(QWidget *parent);
    ~MyGLWidget();
protected:
    void paintGL() Q_DECL_OVERRIDE;
    void initializeGL() Q_DECL_OVERRIDE;
private:
    void prepareTextures();
   public:
    void prepareData(QOpenGLTexture*pTexturedData);

protected:
    QOpenGLBuffer *m_vbo;
    QOpenGLTexture *m_textures[32];

    QOpenGLShaderProgram *m_progXorKey;
    QOpenGLShaderProgram *m_progSub_Shift;
    QOpenGLShaderProgram *m_progMixCol;
    QOpenGLShaderProgram *m_progCbc;

    QOpenGLShaderProgram *m_progForUI;

private:
    int m_frames;
    QTime m_time;
    int m_attrPos;
    int m_attrTexCoord;
    int m_uniformMatrix;
    int m_uniformTexUnit[16];
    QOpenGLFramebufferObject * m_stateFbo;
public slots:
    void capturing();
};

#endif // GLWIDGET_H
