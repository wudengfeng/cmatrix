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
    , nowSpeed(50)
{
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
    setupWindow();
    setupTrayIcon();
    initCMatrix();

    QTimer *timer = new QTimer;
    timer->start(nowSpeed);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateCMatrix()));
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
            if(i + startChar[j] < 0){
                printChar(' ');
                continue;
            }

            if((i + startChar[j]) % (length[j] + space[j]) == (space[j] + length[j] + space[j] - 1) % (length[j] + space[j]))
                glColor3f(1.0, 1.0, 1.0);
            printChar(cmatrix[(i + startChar[j]) % (length[j] + space[j])][j]);
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
    setWindowIcon(QIcon(":/resources/cmatrix.ico"));

    SetWindowLong((HWND)winId(), GWL_EXSTYLE
                  , GetWindowLong((HWND)winId(), GWL_EXSTYLE) | WS_EX_TRANSPARENT | WS_EX_LAYERED);
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
    startChar[j] = length[j] - rows;
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
        if(startChar[j] >= (length[j] + space[j]))
            startChar[j] -= length[j] + space[j];
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

void Window::setupTrayIcon()
{
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/resources/cmatrix.ico"));
    trayIcon->setToolTip(tr("PaShan cmatrix"));

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason))
            , this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));

    createTrayIconAction();
    createTrayIconMenu();

    trayIcon->show();
}

void Window::createTrayIconAction()
{
    quitAction = new QAction(tr("quit"), this);
    connect(quitAction, SIGNAL(triggered(bool)), this, SLOT(close()));
}

void Window::createTrayIconMenu()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(quitAction);
    trayIcon->setContextMenu(trayIconMenu);
}
