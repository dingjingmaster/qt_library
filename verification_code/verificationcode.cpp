#include "verificationcode.h"

#include <QPainter>
#include <QTime>

VerificationCode::VerificationCode(QWidget *parent) : QLabel(parent)
{
    // 生成随机种子
    qsrand(QTime::currentTime().second() * 1000 + QTime::currentTime().msec());
    mColorArray = new QColor[mLetterNumber];
    mVerificationCode = new QChar[mLetterNumber];
    mNoiseNum = this->width();
}

VerificationCode::~VerificationCode()
{

}

QString VerificationCode::getVerificationCode() const
{
    QString str;
    QChar cTemp;
    for (int i = 0; i < mLetterNumber; ++i) {
        cTemp = mVerificationCode[i];
        str += cTemp > 97 ? cTemp.toUpper() : cTemp;
    }
    return str;
}

void VerificationCode::reflushVerification()
{
    repaint();
}

void VerificationCode::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPoint p;

    painter.fillRect(rect(), Qt::white);

    // 产生随机字符
    produceVerificationCode();

    // 产生随机颜色
    produceRandomColor();

    // 绘制验证码
    QFont font;
    font.setPointSize(20);
    painter.setFont(font);
    for (int i = 0; i < mLetterNumber; ++i) {

        p.setX(i * (width() / mLetterNumber) + width() / 8);
        p.setY(height() / 2 + (qrand() % 2) * qrand() % (height() / 3));
        painter.setPen(mColorArray[i]);
        painter.drawText(p, QString(mVerificationCode[i]));
    }

    // 绘制噪点
    for (int j = 0; j < mNoiseNum; ++j) {
        p.setX(qrand() % width());
        p.setY(qrand() % height());
        painter.setPen(mColorArray[j % 4]);
        painter.drawPoint(p);
    }
}

void VerificationCode::produceVerificationCode() const
{
    for (int i = 0; i < mLetterNumber; ++i) {
        mVerificationCode[i] = produceRandomLetter();
    }
}

QChar VerificationCode::produceRandomLetter() const
{
    QChar c;
    int flag = qrand() % mLetterNumber;
    switch (flag) {
    case NUMBER: c = '0' + qrand() % 10; break;
    case UPLETTER: c = 'A' + qrand() % 26; break;
    case LOWLETTER: c = 'a' + qrand() % 26; break;
    default: c = qrand() % 2 ? 'D' : 'J';
    }
    return c;
}

void VerificationCode::produceRandomColor() const
{
    for (int i = 0; i < mLetterNumber; ++i) {
        mColorArray[i] = QColor(qrand() % 255, qrand() % 255, qrand() % 255);
    }
}
