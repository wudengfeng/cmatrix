#ifndef SOLARSYSTEM_H
#define SOLARSYSTEM_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QSystemTrayIcon>

QT_BEGIN_NAMESPACE
class QMenu;
class QAction;
QT_END_NAMESPACE

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

    void setupTrayIcon();
    void createTrayIconAction();
    void createTrayIconMenu();

    void setRandomCharactor(int x, int y);

private slots:
    void updateCMatrix();

private:
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QAction *quitAction;

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
    int *startChar;
    int *whiteTail;
    QTimer *timer;

    int nowSpeed;

    int rows;
    int cols;
    int fontSize;
};

#endif
