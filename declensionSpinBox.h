#ifndef DECLENSIONSPINBOX_H
#define DECLENSIONSPINBOX_H
#include <QSpinBox>
#include <QLineEdit>
#include <QStringList>

class declensionSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    declensionSpinBox(QWidget *parent = nullptr) : QSpinBox(parent) {}
    void setWordForms(const QStringList &forms) {
        // forms должен быть размером 3: ["минута", "минуты", "минут"]
        wordForms = forms;
    }

    QLineEdit *getlineEdit() const{
        return QSpinBox::lineEdit();
    };

protected:
    QString textFromValue(int value) const override {
        QString word;

        int lastTwo = value % 100;
        int lastDigit = value % 10;

        if (lastTwo >= 11 && lastTwo <= 14) {
            word = wordForms.value(2, ""); // форма "минут"
        } else if (lastDigit == 0 || lastDigit == 1) {
            word = wordForms.value(0, ""); // форма "раз"
        } else if (lastDigit >= 2 && lastDigit <= 4) {
            word = wordForms.value(1, ""); // форма "минуты"
        } else {
            word = wordForms.value(2, "");
        }

        return QString("%1 %2").arg(value).arg(word);
    }

private:
    QStringList wordForms;
    QLineEdit *edit;
};
#endif // DECLENSIONSPINBOX_H
