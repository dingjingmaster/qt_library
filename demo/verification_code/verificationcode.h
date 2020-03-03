#ifndef VERIFICATIONCODE_H
#define VERIFICATIONCODE_H

#include <QLabel>
#include <QObject>

class VerificationCode : public QLabel
{
    Q_OBJECT
public:
    VerificationCode(QWidget* parent=0);
    ~VerificationCode();
    // 获取验证码 字符串
    QString getVerificationCode() const;

public slots:
    // 刷新验证码
    void reflushVerification ();

protected:
    // 绘制验证码
    void paintEvent(QPaintEvent *) override;

private:
    const int mLetterNumber = 4;
    int mNoiseNum;
    enum {
        NUMBER,
        UPLETTER,
        LOWLETTER,
    };
    void produceVerificationCode () const;
    // 产生随机字符
    QChar produceRandomLetter () const;
    // 产生随机颜色
    void produceRandomColor () const;

    QChar* mVerificationCode;
    QColor* mColorArray;
};

#endif // VERIFICATIONCODE_H
