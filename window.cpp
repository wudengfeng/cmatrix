#include <QtWidgets>
#include <QtOpenGL>
#include <QtOpenGL/glut>
#include <QtOpenGL/glut.h>
#include "window.h"

#define MAX_CHAR 128

static void selectFont(int size, int charset, const char* face)
{
    HFONT hFont = CreateFontA(size, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
        charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);

    HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);

    DeleteObject(hOldFont);
}

Window::Window(QWidget *parent, bool fs)
    : QOpenGLWidget(parent)
    , fullscene(fs)
    , width(qApp->desktop()->width())
    , height(qApp->desktop()->height())
    , fontSize(24)
    , randMin(33)
    , randMax(126)
{
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
    setupWindow();
    initCMatrix();

    QTimer *timer = new QTimer;
    timer->start(20);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateCMatrix()));

    SetWindowLong((HWND)winId(), GWL_EXSTYLE
                  , GetWindowLong((HWND)winId(), GWL_EXSTYLE) | WS_EX_TRANSPARENT | WS_EX_LAYERED);
}

void Window::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.0 ,0.0, 0.0, 0.0);
}

void Window::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    glColor3f(0.0, 1.0, 0.0);
    selectFont(fontSize, ANSI_CHARSET, "Rockwell");
    glRasterPos2i(0.0, 0.0);

    for (int j = 0; j < cols; j++) {
        for (int i = 0; i < rows; i++) {
            glRasterPos2i(j * fontSize, i * fontSize);
            glColor3f(0.0, 1.0, 0.0);
            if((i + startChar[j]) % rows == (space[j] + rows - 1) % rows)
                glColor3f(1.0, 1.0, 1.0);
            printChar(cmatrix[(i + startChar[j]) % rows][j]);
        }
    }
}

void Window::resizeGL(int width, int height)
{
    if (height == 0) {
        height = 1;
    }
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Window::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    }
}

void Window::closeEvent(QCloseEvent *event)
{
    qApp->quit();
}

void Window::setupWindow()
{
    setGeometry(0, 0, width, height);
    setWindowFlags(windowFlags()
                   |Qt::FramelessWindowHint
                   |Qt::WindowStaysOnBottomHint
                   |Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowState(Qt::WindowNoState
                   |Qt::WindowFullScreen);
    setFocusPolicy(Qt::NoFocus);
    setWindowOpacity(1.0);
}

void Window::initCMatrix()
{
    rows = 2 * height / fontSize;
    cols = width / fontSize;

    cmatrix = new char *[rows];
    for (int i = 0; i < rows; i++) {
        cmatrix[i] = new char[cols];
    }

    space = new int[cols];
    length = new int[cols];
    updateFlag = new int[cols];
    startChar = new int[cols];

    for (int j = 0; j < cols; j++) {
        setCols(j);
    }
}

void Window::setCols(int j)
{
    length[j] = qrand() % (rows / 2 - 3) + 3;
    space[j] = qrand() % (rows / 2 - length[j]);
    updateFlag[j] = rows / 2 + length[j] + space[j];
    startChar[j] = length[j];
    for (int i = 0; i < space[j]; i++) {
        cmatrix[i][j] = ' ';
    }
    for (int i = space[j]; i < space[j] + length[j]; i++) {
        cmatrix[i][j] = qrand() % (randMax - randMin) + randMin;
    }
    for (int i = space[j] + length[j]; i < rows; i++) {
        cmatrix[i][j] = ' ';
    }
}

void Window::updateCMatrix()
{
    for (int j = 0; j < cols; j++) {
        startChar[j] ++;
        startChar[j] %= rows;
    }

    update();
}

void Window::printChar(const char ch)
{
    static int isFirstCall = 1;
    static GLuint lists;

    if (isFirstCall) {
        isFirstCall = 0;
        lists = glGenLists(MAX_CHAR);
        wglUseFontBitmaps(wglGetCurrentDC(), 0, MAX_CHAR, lists);
    }
    glCallList(lists + ch);
}
