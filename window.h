#ifndef SOLARSYSTEM_H
#define SOLARSYSTEM_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class Window : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit Window(QWidget *parent = 0, bool fs = false);

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int, int) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *) Q_DECL_OVERRIDE;
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    void setupWindow();
    void initCMatrix();
    void setCols(int);
    void printChar(const char);
private slots:
    void updateCMatrix();

private:
    bool fullscene;

    int width;
    int height;

    int randMin;
    int randMax;

    char **cmatrix;
    int *space;
    int *length;
    int *updateFlag;

    int rows;
    int cols;
    int fontSize;
};

#endif
